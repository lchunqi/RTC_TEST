#include "command.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "soft_i2c.h"
#include "current.h"
#include "rtc.h"
#include "task.h"
#include "temp.h"
#include "rtc_i2c.h"
#include "flash.h"
#include "voltage.h"
#include "log.h"
#include "manager.h"
#include "power.h"
#include "calculate.h"
#include "drv_wave1006.h"

#define UART_RECEIVE_BUFF_NUMBER    200
#define UART_TRANSMIT_BUFF_NUMBER   300
#define USB_UART_TICK_CLEAR         100//10ms
extern FrequencyData g_lastFreq;

extern UART_HandleTypeDef huart1;
static uint16_t usb_Rx_Cnt = 0;		//
static uint8_t usbRxBuf;
uint8_t g_UsbRxBuff[UART_RECEIVE_BUFF_NUMBER] = {0};
uint8_t g_UsbTxBuff[UART_TRANSMIT_BUFF_NUMBER] = {0};
uint8_t g_UsbATStatus = 0;

extern UART_HandleTypeDef huart2;
static uint16_t device_Rx_Cnt = 0;		//
static uint8_t deviceRxBuf;
uint8_t g_DeviceRxBuff[UART_RECEIVE_BUFF_NUMBER] = {0};
uint8_t g_DeviceTxBuff[UART_TRANSMIT_BUFF_NUMBER] = {0};
uint8_t g_DeviceATStatus = 0;
uint8_t g_rtcChannel = 1;

#define MAX_COMMAND_NUMBER  40
#define MAX_COMMAND_LENGTH  20
static char CommandList[MAX_COMMAND_NUMBER][MAX_COMMAND_LENGTH] = {
	"AT^HELP",
	"AT^VERSION?",
	"AT^REALTIME?", 
	"AT^READDATA",
	"AT^SETTIME",
	"AT^RTCCURRENT?",
	"AT^RTC4096",
	"AT^RTCOTP",
	"AT^RTCADJCAP",
	"AT^RTCMODE",
	"AT^FREQTEST",
	"AT^READGROUP",
	"AT^SYSTEMP?",
	"AT^WRITEFLASH",
	"AT^READFLASH",
	"AT^ERASEFLASH",
	"AT^SETCURRENT",
	"AT^SYSVOLTAGE",
	"AT^SYSTEST",
	"AT^TESTMODE?",
	"AT^RESULT?",
	"AT^TESTFLAG",
	"AT^TESTDATA",
	"AT^ATTEST",
	"AT^MACHINE",
	"AT^1006TMODE",
#if (VERSION_SELECT == VERSION_SD8801)
	"AT^SD8801INT",
#elif (VERSION_SELECT == VERSION_SD8804)
	"AT^SD8804INT",
#else
	"AT^SD8800INT",
#endif
	"AT^RTCLOAD",
	"AT^HOTVOLTAGE",
	"AT^TEMPADC",
	"AT^BSN?",
	"AT^SETBSN",
	"AT^CSN?",
	"AT^SETCSN",
	"AT^TEST60S",
	"AT^CHECKBOARD",
};

void UsbPrintf(char* fmt,...)  
{  
	va_list ap;//
	memset(g_UsbTxBuff, 0, UART_TRANSMIT_BUFF_NUMBER);
	va_start(ap, fmt);//
	vsnprintf((char*)g_UsbTxBuff, UART_TRANSMIT_BUFF_NUMBER, fmt, ap);//
	va_end(ap);
	HAL_UART_Transmit(&huart1, g_UsbTxBuff, strlen((char*)g_UsbTxBuff), 1000);
}

void DevicePrintf(char* fmt,...)  
{  
	va_list ap;//
	memset(g_DeviceTxBuff, 0, UART_TRANSMIT_BUFF_NUMBER);
	va_start(ap, fmt);//
	vsnprintf((char*)g_DeviceTxBuff, UART_TRANSMIT_BUFF_NUMBER, fmt, ap);//
	va_end(ap);
	HAL_UART_Transmit(&huart2, g_DeviceTxBuff, strlen((char*)g_DeviceTxBuff), 1000);
}

void UsartInit(void)
{
	HAL_UART_Receive_IT(&huart1, &usbRxBuf, 1);		
    memset(g_UsbRxBuff, 0x00, sizeof(g_UsbRxBuff));
	memset(g_UsbTxBuff, 0x00, sizeof(g_UsbTxBuff));
	HAL_UART_Receive_IT(&huart2, &deviceRxBuf, 1);		
    memset(g_DeviceRxBuff, 0x00, sizeof(g_DeviceRxBuff));
	memset(g_DeviceTxBuff, 0x00, sizeof(g_DeviceTxBuff));
	ControlInit();
	DevicePrintf(":FUNC \"FREQ\"\n");
    HAL_Delay(100);
	DevicePrintf(":FREQ:ARM 300mS\n");
    HAL_Delay(100);
	DevicePrintf(":INP:FILT 1\n");
    HAL_Delay(100);
	DevicePrintf(":EVEN:LEV +1.50V\n");
    HAL_Delay(100);
	DevicePrintf(":SLOP NEG\n");
    HAL_Delay(100);
    UsbPrintf("usb usart init success......\n");
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_RESET); 
	UsbPrintf("VDD is 3.3V\n");
	rtc_otp_power(0);//配置dac电压
	if (CS1237Init()) {
    	UsbPrintf("CS1237 init success\n");
	} else {
    	UsbPrintf("CS1237 init error\n");
	}
	UsbPrintf("soft version: %s\n", SOFT_VERSION);
	UsbPrintf("hard version: %s\n", HARD_VERSION);
	UsbPrintf("compling time %s %s\n", __DATE__, __TIME__);
}

int strSearch(char* source, char* target)
{
	int len_source = strlen(source);
	int len_target = strlen(target);
	int i, j;
	if (len_source < len_target) {
		return -1;
	}		
	for (i = 0; i <= len_source - len_target; i++) {
		for (j = 0; j < len_target; j++) {
			if (source[i + j] != target[j]) { break;}
		}
		if (j == len_target) { return i;}
	}
	return -1;	
}


uint8_t CharToInt(char* source, uint8_t start, int* number)
{
	uint8_t length;
	uint8_t i;
	int sum = 0;
	int state = 0;
	uint8_t hex = 0;
	length = strlen(source);
	if (start >= length) {
		*number = 0;
		return 0;
	}
	
	if (source[start] == '-') {
		state = -1;
		start++;
	} else {
		state = 1;
		if (start + 1 < length ) {
			if (source[start] == '0' && (source[start + 1] == 'x' ||source[start + 1] == 'X')) {
				start += 2;
				hex = 1;
			}
		}
	}
	for (i = start; i < length; i++) {
		if (source[i] >= '0' && source[i] <= '9') {
			if (hex == 1) {
				sum = sum * 16 + source[i] - '0';
			} else {
				sum = sum * 10 + source[i] - '0';
			}
		} else if (source[i] >= 'a' && source[i] <= 'f') {
			if (hex == 1) {
				sum = sum * 16  + source[i] - 'a' + 10;
			} else {
				*number = 0;
				return 0;
			}
		}else if (source[i] >= 'A' && source[i] <= 'F') {
			if (hex == 1) {
				sum = sum * 16  + source[i] - 'A' + 10;
			} else {
				*number = 0;
				return 0;
			}
		} else if (source[i] == ' ' || source[i] == ',') {
			break;
		}
	}
	*number = state * sum;
	return 1;
}

uint8_t CharToFloat(char* source, uint8_t start, float* number)
{
		uint8_t length;
		uint8_t i;
		float sum = 0;
		int state = 1;
		float smallNumber = 0.1;
		length = strlen(source);
		if (length <= start) {
				*number = 0;
				return 0;
		}
		
		if (source[start] == '-') {
				state = -1;
				start++;
		}
		for (i = start; i < length; i++){
				if (source[i] >= '0' && source[i] <= '9') {
						sum = sum * 10 + source[i] - '0';
				} else if (source[i] == '.') {
						i++;
						break;
				} else if (source[i] == ' ' || source[i] == ',') {
						break;
				}	else {
						*number = 0;
						return 0;
				}					
		}
		for (; i < length; i++) {
				if (source[i] >= '0' && source[i] <= '9') {
						sum += smallNumber * (source[i] - '0');
						smallNumber = smallNumber * 0.1;
				} else if (source[i] == ' ' || source[i] == ',') {
						break;
				}	else {
						*number = 0;
						return 0;
				}	
		}
		*number = state * sum;
		return 1;
}

void  AT_HELP_Handle(char* command)
{
    int i = 0, number = 0;
    command = NULL;
    for (i = 0; i < MAX_COMMAND_NUMBER; i++) {
		if (strlen(CommandList[i]) > 0) {
        	UsbPrintf("%d. %s\n", number, CommandList[i]);
			number++;
		}
    }
	UsbPrintf("command number: %d\n", number);
}

void AT_VERSION_Handle(char* command)
{
	UsbPrintf("soft version: %s\n", SOFT_VERSION);
	UsbPrintf("hard version: %s\n", HARD_VERSION);
	UsbPrintf("compling time %s %s\n", __DATE__, __TIME__);
	UsbPrintf("\nOK\n");
}

void  AT_REALTIME_Handle(char* command)
{
	RealTime rtc = {0};
	system_get_real_time(&rtc);
	UsbPrintf("\n20%d/%d/%d %d:%d:%d\n", rtc.year, rtc.mon, rtc.day, rtc.hour, rtc.min, rtc.sec);
}


void  AT_READDATA_Handle(char* command)
{
    int reg, address, num = 0;
	uint8_t result = 0;
	if (CharToInt(command, 12, &address) == 1) {
		num = strSearch(command, ",");
		if (num < 0 || address < 0) { UsbPrintf("input error\n"); return; }
		CharToInt(command, num + 1, &reg);
		i2c_read_multi_bytes(address, reg, &result, 1);
		UsbPrintf("reg[0x%x] = 0x%x\n", reg, result);
	}
}

void  AT_SETTIME_Handle(char* command)
{
	RealTime rtc = {0};
	int num = 0;
	int inNumber = 0;

	system_get_real_time(&rtc);
	num = strSearch(command, "year:");
	if (num != -1) {
		num += strlen("year:");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		rtc.year = inNumber % 100;
	}
	num = strSearch(command, "mon:");
	if (num != -1) {
		num += strlen("mon:");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		rtc.mon = inNumber % 100;
	}	
	num = strSearch(command, "day:");
	if (num != -1) {
		num += strlen("day:");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		rtc.day = inNumber % 100;
	}	
	num = strSearch(command, "hour:");
	if (num != -1) {
		num += strlen("hour:");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		rtc.hour = inNumber % 100;
	}	
	num = strSearch(command, "min:");
	if (num != -1) {
		num += strlen("min:");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		rtc.min = inNumber % 100;
	}	
	num = strSearch(command, "sec:");
	if (num != -1) {
		num += strlen("sec:");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		rtc.sec = inNumber % 100;
	}
	system_set_real_time(rtc);
	UsbPrintf("date set success!\nOK!\r\n");
}

void  AT_RTCCURRENT_Handle(char* command)
{
	float cur;
	command = NULL;
	cur = CS1237Current();
	UsbPrintf("rtc current = %.1f nA\nOK\n", cur);
}


void AT_RTC4096_Handle(char* command)
{
	int inNumber;
	uint8_t result;
	if (CharToInt(command, 11, &inNumber) == 0 ) {
		UsbPrintf("input error\n");
		return;
	}
	
	if (inNumber > 0) {
		result = rtc_out_4096();
	} else {
		result = rtc_disable_int();
	}
	if (result == I2C_OK) {
		UsbPrintf("\nOK\n");
	} else {
		UsbPrintf("\niic error\n");
	}
}

void AT_RTCOTP_Handle(char* command)
{

	UsbPrintf("\nOK\n");
}

void AT_RTCADJCAP_Handle(char* command)
{
	// uint8_t osc_buf[2] = {0};
	// int high, low;
	// if (CharToInt(command, 13, &high) == 0 ) {
	// 	UsbPrintf("input error\n");
	// 	return;
	// }
	// CharToInt(command, strSearch(command, ",") + 1, &low);		
	// osc_buf[0] = low;
	// osc_buf[1] = high;
	// if (drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 21, 2, osc_buf) != WAVE1006_EXIT_OK) { //写ram22、ram23 
	// 	UsbPrintf("adjust error\n");
	// 	return;
	// }	

	// UsbPrintf("adjust success, write cap:0x%02X, 0x%02X\n", osc_buf[1], osc_buf[0]);

	rtc_freq_test(FREQ_TEST_STARTING);
}

void AT_RTCMODE_Handle(char* command)
{
	static uint8_t state = 1;
	state++;

	if (state % 2) {
		UsbPrintf("VDD is 3.3V\n");
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_RESET); 
	} else {
		UsbPrintf("VDD is 5V\n");
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_SET); 

	}
	UsbPrintf("OK\n");
}

void AT_FREQTEST_Handle(char* command)
{
	int i;
	DevicePrintf(":MEAS?\n");
	for (i = 0; i < 15; i++) {
		HAL_Delay(100);
		if (strSearch((char*)g_DeviceRxBuff, "Hz") > 0) {
			UsbPrintf("%s\n", (char*)g_DeviceRxBuff);
			break;
		}
	}
	if (i >= 15) {
		UsbPrintf("device no respose!\n");
	}

	UsbPrintf("OK\n");
}

void AT_READGROUP_Handle(char* command)
{
	int channel;
	int i;
    int reg, num = 0;
	uint8_t result[8] = {0};
	if (CharToInt(command, 13, &channel) == 1) {
		num = strSearch(command, ",");
		if (num < 0 || channel < 0) { UsbPrintf("input error\n"); return; }
		CharToInt(command, num + 1, &reg);
		rtc_group_power(channel, POWER_I2C);
		rtc_set_channel(channel, RTC_INIT_1006);
		g_rtcChannel = channel;
		// rtc_set_int_channel(channel);
		// HAL_Delay(700);
		rtc_read_multi_bytes(0x64, reg, result, 8);
		UsbPrintf("read group[%d] reg[0x%x]:", channel, reg);
		for (i = 0; i < 8; i++) {
			UsbPrintf("0x%02x ", result[i]);
		}
	}

}

void AT_SYSTEMP_Handle(char* command)
{
	UsbPrintf("\nsystem temp = %.1fC:\n", temp_read_value(1));
	UsbPrintf("\nsystem temp = %.1fC:\n", temp_read_value(2));
	UsbPrintf("\nOK\n");
}


void AT_WRITEFLASH_Handle(char* command)
{
	static uint8_t number = 0;
	command = NULL;
	number++;
	log_printf("2023.08.07 w25q128 flash log record test %03d\n", number);
	UsbPrintf("write OK\n");
}

void AT_READFLASH_Handle(char* command)
{
	int address;
	uint8_t str[256] = {0};
	int result;
    int i;
	uint32_t pageCount;

	if (strSearch(command, "=") > 0) {
		CharToInt(command, 13, &address);
		result = flash_read_data(address, str, 256);
		if (result == 1) {
			UsbPrintf("flash address[0x%x] print:\n%s\n", address, (char*)str);
		} else {
			UsbPrintf("read error %d\n", result);
		}
	} else {
		pageCount = get_log_record_pages();
		UsbPrintf("log use pages %d/%d:\n", pageCount, LOG_MAX_PAGE);
		for (i = 0; i < pageCount; i++) {
			flash_read_data(LOG_START_ADDR + i * PAGE_SIZE, str, PAGE_SIZE);
			UsbPrintf("%s\n", (char*)str);
		}
		UsbPrintf("all log print success\n"); 
	} 
}
void AT_ERASEFLASH_Handle(char* command)
{
	int mode, address;
	int i = 0;
	if (CharToInt(command, 14, &mode) == 1) {
		CharToInt(command, strSearch(command, ",") + 1, &address);
		if (mode >= 0 && mode <= 2) {
			flash_erase(address, mode);
			UsbPrintf("earse full chip need 40s ~ 200s\n");
			if (mode == 0) { 
				for (i = 0; i < 100; i++) {
					HAL_Delay(2000);
					UsbPrintf("wait for %ds...\n", (i + 1) * 2);
					if (CheckBusy() == 1) {
						break;
					}
				}
				log_list_init(); 
			}
			UsbPrintf("OK\n");
		}
	} else {
		UsbPrintf("input error!\n");
	} 
}

void AT_SETCURRENT_Handle(char* command)
{
	int mode, number;
	if (CharToInt(command, 14, &mode) == 1) {
		CharToInt(command, strSearch(command, ",") + 1, &number);
		if (mode == 0) {
			rtc_group_power(number, POWER_OFF);
		} else if (mode == 1) {
			HAL_Delay(5);
			rtc_group_power(number, POWER_VDD_I);
		} else if (mode == 2) {
			HAL_Delay(5);
			rtc_group_power(number, POWER_BAT_I);
		}
		UsbPrintf("current setting :mode %d, channel %d \nOK!\n", mode, number);
	} else {
		UsbPrintf("input error!\n");
	} 
}
void AT_SYSVOLTAGE_Handle(char* command)
{
	uint32_t voltage;
	voltage = ststem_voltage(0) * 3300 * 4.6 / 4096;
	UsbPrintf("\n3.3V\t: %dmV\n", voltage);
	voltage = ststem_voltage(1) * 3300 * 4.6 / 4096;
	UsbPrintf("VCE\t: %dmV\n", voltage);
	voltage = ststem_voltage(8) * 3300 * 4.6 / 4096;
	UsbPrintf("5V\t: %dmV\n", voltage);
	voltage = ststem_voltage(9) * 3300 * 4.6 / 4096;
	UsbPrintf("9V\t: %dmV\n", voltage);
	UsbPrintf("OK\n");
}

void AT_SYSTEST_Handle(char* command)
{
	int number;
	if (CharToInt(command, 11, &number) == 1) {
		if (number < FREE_MODE || number > MACHINE_WRITE) {
			UsbPrintf("no this test mode!\n");
			return;
		}
		manager_set_mode(number);
	}
	UsbPrintf("\nOK\n");
}

void AT_TESTMODE_Handle(char* command)//取料机查询状态
{
	DevicePrintf("mode:0\n");
	UsbPrintf("mode:0\n");
}

void AT_RESULT_Handle(char* command)//取料机查询信息
{
	uint8_t input[MAX_CHIP_NUMBER] = {5};
	int i, j;
	uint8_t all = 0, ok = 0, ng = 0;
	uint8_t length;
	uint8_t getNumber = 0;
	// uint8_t g_result[MAX_CHIP_NUMBER] = {0xff};
    // flash_test_flag_manage(g_result, FLAG_MACHINE, FLASH_FLAG_READ);	
	// for (i = 0; i < MAX_CHIP_NUMBER; i++) {
	// 	if (g_result[i] == '1') {
	// 		g_result[i] = 1;
	// 	} else {
	// 		g_result[i] = 0;
	// 	}
	// }
	uint8_t g_result[MAX_CHIP_NUMBER] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	};
	if (strSearch((char*)g_DeviceRxBuff, "AT^RESULT?") == 0 || strSearch((char*)g_UsbRxBuff, "AT^RESULT?") == 0) {//该指令支持USB和上料机两种方式读取
		for (i = 0; i < MAX_CHIP_NUMBER; i++) {
			if (g_result[i] < 2) {
				all++;
				if (g_result[i] == 0) {
					ng++;
				} else {
					ok++;
				}
			}
		}
		UsbPrintf("all:%d,ok:%d,ng:%d\n", all, ok, ng);
		DevicePrintf("all:%d,ok:%d,ng:%d", all, ok, ng);
		for(i = 0; i < MAX_CHIP_NUMBER; i++) {
			if (i % 10 == 0) {
				if (i > 0) {
					UsbPrintf("]\n");
					DevicePrintf("]");//\n是通信结束符
				}
				UsbPrintf("[");
				DevicePrintf("[");
			}
			UsbPrintf("%c", g_result[i] + '0');
			DevicePrintf("%c", g_result[i] + '0');
		}
		UsbPrintf("]\n");
		DevicePrintf("]\n");
	} else {
		length = strlen((char*)g_DeviceRxBuff);
		getNumber = 0;
		for (i = 0; i < length; i++) {
			if (g_DeviceRxBuff[i] == '[') {
				for (j = i; j < length; j++) {
					if (g_DeviceRxBuff[j] == '0' || g_DeviceRxBuff[j] == '1' || g_DeviceRxBuff[j] == 'N') {
						if (getNumber == MAX_CHIP_NUMBER) {//数量超范围了，肯定是错误数据	
							DevicePrintf("ERROR\n");
							return; 
						}
						input[getNumber] = g_DeviceRxBuff[j] - '0';
						getNumber++;
					} else if (g_DeviceRxBuff[j] == ']') {
						i = j;
						break;
					} else if (g_DeviceRxBuff[j] == '[') {
						continue;
					} else {//遇到异常字符
						DevicePrintf("ERROR\n");
						return; 
					}
				}
			}
			if (getNumber == MAX_CHIP_NUMBER) {	break; }
		}
		for (i = 0; i < MAX_CHIP_NUMBER; i++) {
			if (input[i] != g_result[i]) {
				DevicePrintf("ERROR\n");
				return; 
			}
		}
		DevicePrintf("OK\n");
	}
}

void AT_TESTFLAG_Handle(char* command)
{
	uint8_t buffer[MAX_CHIP_NUMBER] = {0xff};
	uint32_t address;
	uint8_t count;
	int i, j;
	UsbPrintf("chip group flag: config9, 5test, calculate, download\n");
	for (j = 0; j < 4; j++) {
		memset(buffer, 0xff, MAX_CHIP_NUMBER);
		address = TEST_FALG_START_ADDR + j * PAGE_SIZE;
		flash_read_data(address, buffer, MAX_CHIP_NUMBER);
		count = 0;
		for (i = 0; i < MAX_CHIP_NUMBER ; i++) {
			if (i % 10 == 0) {
				UsbPrintf("\n[%02d]", i + 1);
			}
			if (j == 1) {
				if (buffer[i] == 0xEA) {
					count++;
				}
				UsbPrintf("%02X ", buffer[i]);
			} else {
				if (buffer[i] == '1') {
					count++;
					UsbPrintf("1 ");
				} else {
					UsbPrintf("%02X ", buffer[i]);
				}
			}
		}
		UsbPrintf("\n count: %d\n", count);
	}
}

void AT_TESTDATA_Handle(char* command)
{
	int partNumber = 0;
	int chipNumber = 0;
	int num;
	int i;
	TestResult result = {0};

	num = strSearch(command, "chip:");
	if (num != -1) {
		num += strlen("chip:");
		CharToInt(command, num, &chipNumber);
	}
	
	num = strSearch(command, "part:");
	if (num != -1) {
		num += strlen("part:");
		CharToInt(command, num, &partNumber);
	}
	if (chipNumber == 0) {//所有芯片的某一个参数
		for (i = 0; i < MAX_CHIP_NUMBER; i++) {
			UsbPrintf("chip%d ", i + 1);
			flash_test_data_read(&result, i + 1, TEST_DATA_ALL);
			test_data_printf(partNumber, result);
		}
	} else {//某一个芯片的所有参数
		if (chipNumber > MAX_CHIP_NUMBER) { chipNumber = MAX_CHIP_NUMBER;}
		if (chipNumber < 0) { chipNumber = 1; }
		flash_test_data_read(&result, chipNumber, TEST_DATA_ALL);
		test_data_printf(partNumber, result);
	}
}

void AT_ATTEST_Handle(char* command)
{

	int number = 0;
	uint16_t offset;
	float cap[TEMP_ADC_LENGTH] = {0};
	int digtal[TEMP_ADC_LENGTH] = {0};
	int i;
	int count = 0;
	CharToInt(command, strSearch(command, "=") + 1, &number);
	if (number > 0) {
		calculate_wave1006_temp_cap(number, cap, digtal, &offset);
	} else {
		for (i = 0 ; i < MAX_CHIP_NUMBER; i++) {
			if(calculate_wave1006_temp_cap(i + 1, cap, digtal, &offset) > 0) {
				count++;
			}
		}
	}
		UsbPrintf("\ncount : %d\n", count);
	
}

void AT_MACHINE_Handle(char* command)
{	
	int j;
	uint8_t osc_buf[2];
	uint8_t mode  = 1;
	uint8_t com	 = 0;
	uint8_t incap = 0;
	uint8_t outcap = 0;
	int num;
	for (com = 0; com < 32; com++) {
		for (incap = 0; incap < 8; incap++) {
			for (outcap = 0; outcap < 8; outcap++) {
				osc_buf[1] = (mode << 3) | ((com & 0x1f) >> 2);
				osc_buf[0] = ((com & 0x03) << 6) | ((incap & 0x07) << 3) | (outcap & 0x07);
				if (drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 21, 2, osc_buf) != WAVE1006_EXIT_OK) { //写ram22、ram23 
					UsbPrintf("adjust error\n");
					return;
				}	
				UsbPrintf("cap[0x%02x,0x%02x] adjust ok\n", osc_buf[1], osc_buf[0]);
				HAL_Delay(300);
				DevicePrintf(":MEAS?\n");
				for (j = 0; j < 20; j++) {
					HAL_Delay(100);
					num = strSearch((char*)g_DeviceRxBuff, "PPM");
					if (num > 0) {
						UsbPrintf("%s", (char*)g_DeviceRxBuff);
						memset(g_DeviceRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
						device_Rx_Cnt = 0;
						break;
					} 
				}
				if (j >= 20) {
					UsbPrintf("device no respose!\n");
				}
			}
		}
	}
}

void AT_1006TMODE_Handle(char* command)
{
	int mode = 0;
	CharToInt(command, strSearch(command, "=") + 1, &mode);
	
	if (mode > 0) {
		if (rtc_1006_set_testmode(g_rtcChannel) != I2C_OK) {
			UsbPrintf("enter testmode fail \n");
		}
	} else {
		rtc_1006_exit_testmode(g_rtcChannel);
	}
	UsbPrintf("OK\n");
}


#if (VERSION_SELECT == VERSION_SD8801)
void AT_SD8801INT_Handle(char* command)
{
	int startNumber = 0;
	int endNumber = 0;
	int i, j;
	uint8_t buf;
	uint8_t result;
	float temp;
	float freq;
	float ppm;
	int num;
	char strFreq[10] = {0};
	float gain;
	uint8_t intBuffer[5] = {0x04, 0x00, 0x10, 0x00, 0x10};
	CharToInt(command, strSearch(command, "=") + 1, &startNumber);
	CharToInt(command, strSearch(command, ",") + 1, &endNumber);
	
	if (startNumber > 0) {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11| GPIO_PIN_12 |GPIO_PIN_13, GPIO_PIN_SET);
		if (endNumber > MAX_CHIP_NUMBER) {
			endNumber = MAX_CHIP_NUMBER;
		} else if (endNumber < startNumber) {
			endNumber = startNumber;
		} 
		UsbPrintf("\nsystem temp = %.1fC:\n", temp_read_value(1));
		for (i = startNumber; i <= endNumber; i++) {
			rtc_group_power(i, POWER_I2C);
			rtc_set_channel(i, RTC_INIT_1006);
			g_rtcChannel = i;
			HAL_Delay(10);
			buf = 0x10;
			result = rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x57, &buf, 1);
			if (result !=  I2C_OK) {
				UsbPrintf("chip%d i2c error\n", g_rtcChannel);
				continue;
			}
			HAL_Delay(20);
			rtc_read_multi_bytes(RTC_8800_ADDRESS, 0x17, &buf, 1);
			temp = ((uint32_t)buf * 2 - 187.19 ) / 3.218;

			rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0B, intBuffer, 5);//开启输出
			HAL_Delay(300);
			DevicePrintf(":MEAS?\n");
			for (j = 0; j < 30; j++) {
				HAL_Delay(100);
				num = strSearch((char*)g_DeviceRxBuff, "KHz");
				if (num > 0) {
					gain = 1.024;
					strncpy(strFreq, (char*)g_DeviceRxBuff, num);//读取其中的数字部分
					CharToFloat(strFreq, 0, &freq);//转换为浮点型数据
					ppm = (freq / gain - 1.0) * 1000000;
					UsbPrintf("chip%d temp:%.0f, system temp = %.1fC,ppm:%.2f\n", g_rtcChannel, temp, temp_read_value(1), ppm);
					memset(g_DeviceRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
					device_Rx_Cnt = 0;
					break;
				} 
			}
			if (j >= 30) {
				UsbPrintf("chip%d, device no respose!\n", g_rtcChannel);
			}
			buf = 0x00;
			rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0F, &buf, 1);
		}
	} else {
		UsbPrintf("input error\n");
	}
	UsbPrintf("OK\n");
}
#elif (VERSION_SELECT == VERSION_SD8804)
void AT_SD8804INT_Handle(char* command)
{
	int startNumber = 0;
	int endNumber = 0;
	int i, j;
	uint8_t buf;
	uint8_t result;
	float temp;
	float freq;
	float ppm;
	int num;
	char strFreq[10] = {0};
	float gain;
	uint8_t intBuffer[5] = {0x04, 0x00, 0x10, 0x00, 0x10};
	CharToInt(command, strSearch(command, "=") + 1, &startNumber);
	CharToInt(command, strSearch(command, ",") + 1, &endNumber);
	
	if (startNumber > 0) {
		UsbPrintf("\nsystem temp = %.1fC:\n", temp_read_value(1));
		for (i = startNumber; i <= endNumber; i++) {
			UsbPrintf("chip%d test 1.024KHz\n", i);
			rtc_group_power(i, POWER_I2C);
			rtc_set_channel(i, RTC_INIT_1006);
			g_rtcChannel = i;
			HAL_Delay(10);
			buf = 0x10;
			result = rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x57, &buf, 1);
			if (result !=  I2C_OK) {
				UsbPrintf("chip%d i2c error\n", g_rtcChannel);
				continue;
			}
			HAL_Delay(20);
			rtc_read_multi_bytes(RTC_8800_ADDRESS, 0x56, &buf, 1);
			temp = buf;
			UsbPrintf("chip%d temp:%.0f, system temp = %.1fC\n", g_rtcChannel, temp, temp_read_value(1));


			rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0B, intBuffer, 5);//开启输出
			HAL_Delay(300);
			DevicePrintf(":MEAS?\n");
			for (j = 0; j < 30; j++) {
				HAL_Delay(100);
				num = strSearch((char*)g_DeviceRxBuff, "KHz");
				if (num > 0) {
					UsbPrintf("%s", (char*)g_DeviceRxBuff);
					gain = 1.024;
					strncpy(strFreq, (char*)g_DeviceRxBuff, num);//读取其中的数字部分
					CharToFloat(strFreq, 0, &freq);//转换为浮点型数据
					ppm = (freq / gain - 1.0) * 1000000;
					UsbPrintf("ppm:%.2f\n", ppm);
					memset(g_DeviceRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
					device_Rx_Cnt = 0;
					break;
				} 
			}
			if (j >= 30) {
				UsbPrintf("device no respose!\n");
			}
			buf = 0x00;
			rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0F, &buf, 1);
		}
	} else {
		UsbPrintf("input error\n");
	}
	UsbPrintf("OK\n");
}
#else
void AT_SD8800INT_Handle(char* command)
{
	int startNumber = 0;
	int endNumber = 0;
	int i, j;
	uint8_t buf;
	uint8_t result;
	float temp;
	float freq;
	float ppm;
	int num;
	char strFreq[10] = {0};
	float gain;
	uint8_t intBuffer[5] = {0x04, 0x00, 0x10, 0x00, 0x10};
	CharToInt(command, strSearch(command, "=") + 1, &startNumber);
	CharToInt(command, strSearch(command, ",") + 1, &endNumber);
	
	if (startNumber > 0) {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11| GPIO_PIN_12 |GPIO_PIN_13, GPIO_PIN_SET);
		if (endNumber > MAX_CHIP_NUMBER) {
			endNumber = MAX_CHIP_NUMBER;
		} else if (endNumber < startNumber) {
			endNumber = startNumber;
		} 
		UsbPrintf("\nsystem temp = %.1fC:\n", temp_read_value(1));
		for (i = startNumber; i <= endNumber; i++) {
			rtc_group_power(i, POWER_I2C);
			rtc_set_channel(i, RTC_INIT_1006);
			g_rtcChannel = i;
			HAL_Delay(10);
			buf = 0x10;
			result = rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x57, &buf, 1);
			if (result !=  I2C_OK) {
				UsbPrintf("chip%d i2c error\n", g_rtcChannel);
				continue;
			}
			HAL_Delay(20);
			rtc_read_multi_bytes(RTC_8800_ADDRESS, 0x17, &buf, 1);
			temp = ((uint32_t)buf * 2 - 187.19 ) / 3.218;

			rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0B, intBuffer, 5);//开启输出
			HAL_Delay(300);
			DevicePrintf(":MEAS?\n");
			for (j = 0; j < 30; j++) {
				HAL_Delay(100);
				num = strSearch((char*)g_DeviceRxBuff, "KHz");
				if (num > 0) {
					gain = 1.024;
					strncpy(strFreq, (char*)g_DeviceRxBuff, num);//读取其中的数字部分
					CharToFloat(strFreq, 0, &freq);//转换为浮点型数据
					ppm = (freq / gain - 1.0) * 1000000;
					UsbPrintf("chip%d temp:%.0f, system temp = %.1fC,ppm:%.2f\n", g_rtcChannel, temp, temp_read_value(1), ppm);
					memset(g_DeviceRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
					device_Rx_Cnt = 0;
					break;
				} 
			}
			if (j >= 30) {
				UsbPrintf("chip%d, device no respose!\n", g_rtcChannel);
			}
			buf = 0x00;
			rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0F, &buf, 1);
		}
	} else {
		UsbPrintf("input error\n");
	}
	UsbPrintf("OK\n");
}
#endif


void AT_RTCLOAD_Handle(char* command)
{
	int number = 0;
	int i;
	uint8_t result[MAX_CHIP_NUMBER] = {0};
	CharToInt(command, strSearch(command, "=") + 1, &number);
	if (number > 0) {
#if (VERSION_SELECT == VERSION_SD8804)
		sd8804_write_otp(number);
#else
		sd8800_write_otp(number);
#endif
	} else {
		for (i = 1; i <= MAX_CHIP_NUMBER; i++) {
#if (VERSION_SELECT == VERSION_SD8804)
			result[i - 1] = sd8804_write_otp(i);
#else
			result[i - 1] = sd8800_write_otp(i);
#endif
			HAL_Delay(2000);
		}
		
		for (i = 1; i <= MAX_CHIP_NUMBER; i++) {
			UsbPrintf("%d\t", result[i - 1]);
			if (i % 10 == 0) {
				UsbPrintf("\n");
			}
		}
	}
	UsbPrintf("load end\n");
}

void AT_HOTVOLTAGE_Handle(char* command)
{
	float number;
	CharToFloat(command, strSearch(command, "=") + 1, &number);
	rtc_otp_power(number);//配置dac电压
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);//VCE_SDA选择VCE通道
	UsbPrintf("OK\n");
}

void AT_TEMPADC_Handle(char* command)
{
	uint16_t temp_adc = 0;
	uint8_t result;
	result = wave1006_read_temp_adc(&temp_adc);
	UsbPrintf("temp adc:%d, i2c[%d]\n", temp_adc, result);
}

void AT_BSN_Handle(char* command)
{
	char bsn[BSN_LENGTH + 1] = {0};
	GetStringFromFlash(bsn, BSN_LENGTH + 1, BSN_ADDR);
	UsbPrintf("BSN:%s\nOK\n", bsn);
}

void AT_SETBSN_Handle(char* command)
{
	int number, i, length;
	char bsn[BSN_LENGTH + 1] = {0};
	char buffer[24] = {0};
	number = strSearch(command, "=");
	if (number <= 1) {
		UsbPrintf("no input\n");
		return;
	}
	length = strlen(command);
	if (length < (number + 8)) {
		UsbPrintf("bsn length not enough\n");
	}
	for (i = number + 1; i < length; i++) {
		if ((command[i] >= 'A' && command[i] <= 'Z') || (command[i] >= '0' && command[i] <= '9')) {
			bsn[i - number - 1] = command[i];
		} else {
			UsbPrintf("bsn not right, A-Z, 0-9\n");
			return;
		}
		if (i >= (number + 8)) {
			break;
		}
	}
	
    GetStringFromFlash(buffer, BOARD_HISTORY_LENGTH, BOARD_HISTORY_ADDR);
	ClearBsnFlash();
	SaveStringToFlash(buffer, BOARD_HISTORY_ADDR);
	SaveStringToFlash(bsn, BSN_ADDR);
	UsbPrintf("OK\n");
}

extern uint16_t g_csn;
void AT_CSN_Handle(char* command)
{
	UsbPrintf("芯片批次号:%d\nOK\n", g_csn);
}

void AT_SETCSN_Handle(char* command)
{
	int number;
	CharToInt(command, strSearch(command, "=") + 1, &number);
	if (number <= 2401 || number > 9999) {
		UsbPrintf("input error\n");
	} else {
		g_csn = number;
		UsbPrintf("OK\n");
	}
}
void AT_TEST60S_Handle(char* command)
{
	int startNumber = 0;
	int endNumber = 0;
	int i, j;
	uint8_t buf;
	uint8_t result;
	float temp;
	float freq;
	float ppm;
	int num;
	char strFreq[10] = {0};
	float gain;
	uint8_t intBuffer[5] = {0x00, 0x00, 0x20, 0x00, 0x20};
	CharToInt(command, strSearch(command, "=") + 1, &startNumber);
	CharToInt(command, strSearch(command, ",") + 1, &endNumber);
	
	if (startNumber > 0) {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11| GPIO_PIN_12 |GPIO_PIN_13, GPIO_PIN_SET);
		if (endNumber > MAX_CHIP_NUMBER) {
			endNumber = MAX_CHIP_NUMBER;
		} else if (endNumber < startNumber) {
			endNumber = startNumber;
		} 
		UsbPrintf("\nsystem temp = %.1fC:\n", temp_read_value(1));
		for (i = startNumber; i <= endNumber; i++) {
			rtc_group_power(i, POWER_I2C);
			rtc_set_channel(i, RTC_INIT_1006);
			g_rtcChannel = i;
			HAL_Delay(10);
			buf = 0x10;
			result = rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x57, &buf, 1);
			if (result !=  I2C_OK) {
				UsbPrintf("chip%d i2c error\n", g_rtcChannel);
				continue;
			}
			HAL_Delay(20);
			rtc_read_multi_bytes(RTC_8800_ADDRESS, 0x17, &buf, 1);
			// rtc_read_multi_bytes(RTC_8800_ADDRESS, 0x56, &buf, 1);
			temp = ((uint32_t)buf * 2 - 187.19 ) /3.218;

			buf = 0x55;
			rtc_read_multi_bytes(RTC_8800_ADDRESS, 0x00, &buf, 1);
			rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0B, intBuffer, 5);//开启输出
			HAL_Delay(300);
			DevicePrintf(":MEAS?\n");
			for (j = 0; j < 1200; j++) {
				HAL_Delay(100);
				num = strSearch((char*)g_DeviceRxBuff, "mHz");
				if (num > 0) {
					gain = 1000.0 / 60.0;
					strncpy(strFreq, (char*)g_DeviceRxBuff, num);//读取其中的数字部分
					CharToFloat(strFreq, 0, &freq);//转换为浮点型数据
					// UsbPrintf("%s, %f\n",strFreq, freq);
					ppm = (freq / gain - 1.0) * 1000000;
					UsbPrintf("chip%d temp:%.0f, system temp = %.1fC, ppm:%.2f\n", g_rtcChannel, temp, temp_read_value(1), ppm);
					memset(g_DeviceRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
					device_Rx_Cnt = 0;
					break;
				} 
				if ((j + 1) % 200 == 0) {
					UsbPrintf("waited time %ds\n", (j+ 1) / 10);
				}
			}
			if (j >= 1200) {
				UsbPrintf("chip%d, device no respose!\n", g_rtcChannel);
			}
			buf = 0x00;
			rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0F, &buf, 1);
		}
	} else {
		UsbPrintf("input error\n");
	}
	UsbPrintf("OK\n");
}


void AT_CHECKBOARD_Hanle(char* command)
{
	char history[32] = {0};
	int num = 0;
	int count = 0;
    RealTime time = {0};

	num =strSearch(command, "=");
	if (num > 10) {
		CharToInt(command, num + 1, &count);
		if (count >= 0) {
			system_get_real_time(&time);
			GetStringFromFlash(history, BSN_LENGTH, BSN_ADDR);
			ClearBsnFlash();
			SaveStringToFlash(history, BSN_ADDR);
			memset(history, 0, 32);
			snprintf(history, 20, "%d %02d/%02d/%02d %02d:%02d", count, time.year % 100, time.mon % 100, time.day % 100, time.hour % 100, time.min % 100);
			SaveStringToFlash(history, BOARD_HISTORY_ADDR);
		}
	} else {
		GetStringFromFlash(history, BOARD_HISTORY_LENGTH, BOARD_HISTORY_ADDR);
	}
	UsbPrintf("测试次数：%s\nOK\n", history);
}

uint8_t CheckFreqValue(float* result)
{
	int num;
	char strFreq[10] = {0};
	float frequency = 0;

	num = strSearch((char*)g_DeviceRxBuff, "KHz");
	if (num > 0) {//表示是以KHz字符结尾的数据，判断为有效数据
		strncpy(strFreq, (char*)g_DeviceRxBuff, num);//读取其中的数字部分
		CharToFloat(strFreq, 0, &frequency);//转换为浮点型数据
		*result = frequency;
        memset(g_DeviceRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
        device_Rx_Cnt = 0;
		return 1;
	}
	return 0;
}


typedef void(*function)(char* s);
function UsbCommand[MAX_COMMAND_NUMBER] = {
    AT_HELP_Handle,
	AT_VERSION_Handle,
    AT_REALTIME_Handle,
    AT_READDATA_Handle,
    AT_SETTIME_Handle,
	AT_RTCCURRENT_Handle,
	AT_RTC4096_Handle,
	AT_RTCOTP_Handle,
	AT_RTCADJCAP_Handle,
	AT_RTCMODE_Handle,
	AT_FREQTEST_Handle,
	AT_READGROUP_Handle,
	AT_SYSTEMP_Handle,	
	AT_WRITEFLASH_Handle,
	AT_READFLASH_Handle,
	AT_ERASEFLASH_Handle,
	AT_SETCURRENT_Handle,
	AT_SYSVOLTAGE_Handle,
	AT_SYSTEST_Handle,
	AT_TESTMODE_Handle,
	AT_RESULT_Handle,
	AT_TESTFLAG_Handle,
	AT_TESTDATA_Handle,
	AT_ATTEST_Handle,
	AT_MACHINE_Handle,
	AT_1006TMODE_Handle,
#if (VERSION_SELECT == VERSION_SD8801)
	AT_SD8801INT_Handle,
#elif (VERSION_SELECT == VERSION_SD8804)
	AT_SD8804INT_Handle,
#else
	AT_SD8800INT_Handle,
#endif
	AT_RTCLOAD_Handle,
	AT_HOTVOLTAGE_Handle,
	AT_TEMPADC_Handle,
	AT_BSN_Handle,
	AT_SETBSN_Handle,
	AT_CSN_Handle,
	AT_SETCSN_Handle,
	AT_TEST60S_Handle,
	AT_CHECKBOARD_Hanle,
};


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        if (usb_Rx_Cnt == 0 && (usbRxBuf == ' ' || usbRxBuf == '\r' || usbRxBuf == '\n')) {
            HAL_UART_Receive_IT(&huart1, &usbRxBuf, sizeof(usbRxBuf));
            return;
        }
        if (usb_Rx_Cnt < UART_RECEIVE_BUFF_NUMBER && g_UsbATStatus == 0) {
            g_UsbRxBuff[usb_Rx_Cnt] = usbRxBuf;
        	usb_Rx_Cnt++;
        }
        if (usb_Rx_Cnt >= UART_RECEIVE_BUFF_NUMBER || usbRxBuf == ' ') {
            g_UsbATStatus = 1;
        } 
        HAL_UART_Receive_IT(&huart1, &usbRxBuf, 1);
    }
	
	if (huart->Instance == USART2) {
        if (device_Rx_Cnt == 0 && (deviceRxBuf == ' ' || deviceRxBuf == '\r' || deviceRxBuf == '\n')) {
        	HAL_UART_Receive_IT(&huart2, &deviceRxBuf, 1);
            return;
        }
        if (device_Rx_Cnt < UART_RECEIVE_BUFF_NUMBER && g_DeviceATStatus == 0) {
            g_DeviceRxBuff[device_Rx_Cnt] = deviceRxBuf;
        	device_Rx_Cnt++;
        }
        if (device_Rx_Cnt >= UART_RECEIVE_BUFF_NUMBER || deviceRxBuf == ' ') {
            g_DeviceATStatus = 1;
        }
        HAL_UART_Receive_IT(&huart2, &deviceRxBuf, 1);
	}

	if (huart->Instance == USART3) {//上位机信息处理
		control_usart_receive();
	}
}

void CheckCommandList(void)
{
    uint8_t i;
    static uint16_t usbTick = 0;
    static uint16_t lastUsbRx = 0;
    if (g_UsbATStatus == 0 && usb_Rx_Cnt == 0) {
        usbTick = 0;
        lastUsbRx = 0;
        return;
    }
    if (usb_Rx_Cnt > lastUsbRx) {
        lastUsbRx = usb_Rx_Cnt;
        usbTick = 0;
    } else {
        usbTick++;
    }
    if (usbTick >= USB_UART_TICK_CLEAR || g_UsbATStatus == 1) {
        for (i = 0; i < MAX_COMMAND_NUMBER; i++) {
            if (strlen(CommandList[i]) > 0 && strSearch((char*)g_UsbRxBuff, CommandList[i]) == 0) {
                UsbCommand[i]((char*)g_UsbRxBuff);
                break;
            }
        }				
        if (i == MAX_COMMAND_NUMBER) {
            UsbPrintf("\nno this commmand!\n");
        }
        memset(g_UsbRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
        usb_Rx_Cnt = 0;
        g_UsbATStatus = 0;
        usbTick = 0;
        lastUsbRx = 0;
    }
}

void DeviceMassage(void)
{
	float frequency = 0;
    static uint16_t deviceTick = 0;
    static uint16_t lastDeviceRx = 0;
    if (g_DeviceATStatus == 0 && device_Rx_Cnt == 0) {
        deviceTick = 0;
        lastDeviceRx = 0;
        return;
    }
    if (device_Rx_Cnt > lastDeviceRx) {
        lastDeviceRx = device_Rx_Cnt;
        deviceTick = 0;
    } else {
        deviceTick++;
    }
    if (deviceTick >= USB_UART_TICK_CLEAR || g_DeviceATStatus == 1) {
		if (CheckFreqValue(&frequency) > 0) {//表示是以KHz字符结尾的数据，判断为有效数据
			g_lastFreq.freq = frequency;
			g_lastFreq.recordTime = HAL_GetTick();//记录收到数据的时刻
		} else if (strSearch((char*)g_DeviceRxBuff, "AT^TESTMODE?") == 0) {
			AT_TESTMODE_Handle((char*)g_DeviceRxBuff);
		} else if (strSearch((char*)g_DeviceRxBuff, "AT^RESULT?") == 0) {
			AT_RESULT_Handle((char*)g_DeviceRxBuff);
		}
		if (strlen((char*)g_DeviceRxBuff) > 20) {
			AT_RESULT_Handle((char*)g_DeviceRxBuff);
		} 
        memset(g_DeviceRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
        device_Rx_Cnt = 0;
        g_DeviceATStatus = 0;
        deviceTick = 0;
        lastDeviceRx = 0;
	}
}
