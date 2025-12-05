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
#include "soft_uart.h"

#define UART_RECEIVE_BUFF_NUMBER    200
#define UART_TRANSMIT_BUFF_NUMBER   300
#define USB_UART_TICK_CLEAR         10//10ms
extern FrequencyData g_lastFreq;

extern UART_HandleTypeDef huart1;
static uint16_t usb_Rx_Cnt = 0;		//
static uint8_t usbRxBuf;
uint8_t g_UsbRxBuff[UART_RECEIVE_BUFF_NUMBER] = {0};
uint8_t g_UsbTxBuff[UART_TRANSMIT_BUFF_NUMBER] = {0};
uint8_t g_UsbATStatus = 0;

extern UART_HandleTypeDef huart2;
uint16_t device_Rx_Cnt = 0;		//
uint8_t deviceRxBuf;
uint8_t g_DeviceRxBuff[UART_RECEIVE_BUFF_NUMBER] = {0};
uint8_t g_DeviceTxBuff[UART_TRANSMIT_BUFF_NUMBER] = {0};
uint8_t g_DeviceATStatus = 0;
uint8_t g_rtcChanenel = 1;

#define MAX_COMMAND_NUMBER  40
#define MAX_COMMAND_LENGTH  20
static char CommandList[MAX_COMMAND_NUMBER][MAX_COMMAND_LENGTH] = {
	"AT^HELP",
	"AT^REALTIME?", 
	"AT^READDATA",
	"AT^SETTIME",
	"AT^RTCCURRENT?",
	"AT^GROUPSETTEMP",
	"AT^RTCVBAT?",
	"AT^RTC4096",
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
	"AT^WRITECONFIG",
	"AT^TESTFLAG",
	"AT^TESTDATA",
	"AT^ATTEST",
	"AT^MACHINE",
	"AT^1006TMODE",
	"AT^SD3078INT",
	"AT^RTCLOAD",
	"AT^HOTVOLTAGE",
	"AT^TEMPADC",
	"AT^GETRTCID",
	"AT^AECQ100",
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

// void DevicePrintf(char* fmt,...)  
// {
// 	devices_printf(&uart_dev[0], fmt);
// }
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

void AT_GROUPSETTEMP_Handle(char* command)
{
	command = NULL;
	UsbPrintf("system set rtc group temp config start......\n");
	log_printf("receive AT^GROUPSETTEMP, start rtc temp config test\n");
	system_manage_temp();
}


void AT_RTCVBAT_Handle(char* command)
{
	int vbat;
	if (rtc_read_vbat(&vbat) != I2C_OK) {
		UsbPrintf("i2c error\n");
	} else {
		UsbPrintf("vbat = %d0mV\n", vbat);
	}
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
	// if (rtc_set_testcap(osc_buf) != I2C_OK) { //写ram22、ram23 
	// 	return;
	// }	

	// UsbPrintf("adjust success, write cap:0x%02X, 0x%02X\n", osc_buf[1], osc_buf[0]);

	uint8_t cap[2] = {0};
	int number = 0;

	CharToInt(command, strSearch(command, "=") + 1, &number);
	if (number == 2000) {
		cap[0] = 0x00;
		cap[1] = 0x16;
	} else {
		cap_list_to_uint(number, cap);
	}
	if (rtc_set_testcap(cap) != I2C_OK) { //写ram22、ram23 
		UsbPrintf("cap adjust error\n");
	} else {
		UsbPrintf("cap set success\n");
	}

	// rtc_freq_test(FREQ_TEST_STARTING);
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
		// rtc_set_channel(channel, RTC_SD3078B);
		g_rtcChanenel = channel;
		HAL_Delay(10);
		rtc_read_multi_bytes(0x64, reg, result, 8);
		UsbPrintf("read group[%d] reg[0x%x]:", channel, reg);
		for (i = 0; i < 8; i++) {
			UsbPrintf("0x%02x ", result[i]);
		}
	}

}

void AT_SYSTEMP_Handle(char* command)
{
	float temp;
	temp = temp_read_value(1);
	UsbPrintf("\nsystem temp = %.1fC:\n", temp);
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

}


void AT_RESULT_Handle(char* command)//取料机查询信息
{

}

void AT_WRITECONFIG_Handle(char* command)//暂时调试使用20231109
{

}

void AT_TESTFLAG_Handle(char* command)
{
	uint8_t buffer[MAX_CHIP_NUMBER] = {0xff};
	uint32_t address;
	uint8_t count;
	int i, j;
	UsbPrintf("chip group flag: config9, 5test, calculate, download, machine, takedown\n");
	for (j = 0; j < 6; j++) {
		memset(buffer, 0xff, MAX_CHIP_NUMBER);
		address = TEST_FALG_START_ADDR + j * PAGE_SIZE;
		flash_read_data(address, buffer, MAX_CHIP_NUMBER);
		count = 0;
		for (i = 0; i < MAX_CHIP_NUMBER ; i++) {
			if (i % 14 == 0) {
				UsbPrintf("\n[%02d]", i + 1);
			}
			if (j == 1) {
				if (buffer[i] == 0xE0) {
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
	float cap[420] = {0};
	int digtal[420] = {0};
	int i = 0;
	uint8_t count = 0;
	CharToInt(command, strSearch(command, "=") + 1, &number);
	if (number > 0) {
		calculate_wave1006_temp_cap(number, cap, digtal, &offset);
	} else {
		for (i = 0 ; i < MAX_CHIP_NUMBER ; i++) {
			if(calculate_wave1006_temp_cap(i + 1, cap, digtal, &offset) > 0) {
				count++;
			}
		}
		UsbPrintf("\ncount : %d\n", count);
	}

}

void AT_MACHINE_Handle(char* command)
{	
	// int j;
	// uint8_t osc_buf[2];
	// uint8_t mode  = 2;
	// uint8_t com	 = 0;
	// uint8_t incap = 0;
	// uint8_t outcap = 0;
	// int num;
	// for (com = 0; com < 32; com++) {
	// 	for (incap = 0; incap < 8; incap++) {
	// 		for (outcap = 0; outcap < 8; outcap++) {
	// 			// outcap = incap;
	// 			osc_buf[1] = (mode << 3) | ((com & 0x1f) >> 2);
	// 			osc_buf[0] = ((com & 0x03) << 6) | ((incap & 0x07) << 3) | (outcap & 0x07);
	// 			if (rtc_set_testcap(osc_buf) != I2C_OK) { //写ram22、ram23 
	// 				UsbPrintf("adjust error\n");
	// 				return;
	// 			}	
	// 			UsbPrintf("cap[0x%02x,0x%02x] adjust ok\n", osc_buf[1], osc_buf[0]);
	// 			HAL_Delay(500);
	// 			DevicePrintf(":MEAS?\n");
	// 			for (j = 0; j < 30; j++) {
	// 				HAL_Delay(100);
	// 				num = strSearch((char*)g_DeviceRxBuff, "PPM");
	// 				if (num > 0) {
	// 					UsbPrintf("%s", (char*)g_DeviceRxBuff);
	// 					memset(g_DeviceRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
	// 					device_Rx_Cnt = 0;
	// 					break;
	// 				} 
	// 			}
	// 			if (j >= 30) {
	// 				UsbPrintf("device no respose!\n");
	// 			}
	// 		}
	// 	}
	// }
	
	int i;
	uint8_t result[MAX_CHIP_NUMBER] = {0};
	float cur;
	uint8_t count = 0;
	UsbPrintf("bat current test start\n");
	for (i = 1; i <= MAX_CHIP_NUMBER; i++) {
		rtc_group_power(i, POWER_BAT_I);
		HAL_Delay(2000);
		cur = CS1237Current();
		if (cur > 100 && cur < 600) {
			result[i - 1] = 1;
			count++;
		} else {
			HAL_Delay(500);
			cur = CS1237Current();
			if (cur > 100 && cur < 600) {
				result[i - 1] = 1;
				count++;
			}
		}
		UsbPrintf("chip%d bat current:%.1fnA\n", i, cur);
	}
	rtc_group_power(0, POWER_OFF);
	for (i = 1; i <= MAX_CHIP_NUMBER; i++) {
		UsbPrintf("%d\t", result[i - 1]);
		if (i % 10 == 0) {
			UsbPrintf("\n");
		}
	}
	UsbPrintf("bat current test end, ok count:%d\n", count);
}

void AT_1006TMODE_Handle(char* command)
{
	int mode = 0;
	uint8_t buf;
	uint8_t osc_buf[2] = {0};
	CharToInt(command, strSearch(command, "=") + 1, &mode);
	
	if (mode > 0) {
		buf = 0x40;
		if(drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_CTR3, 1, &buf) != WAVE1006_EXIT_OK)   //如果是有应该先关闭32K输出(默认为关闭SD3068的32K)
		{
			return;
		}
		//选择为SD3302封装(必须在进测试模式之前选择，否则芯片pad改变)
		if (drv_wave1006_read_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 20, 1, &buf) != WAVE1006_EXIT_OK)   //回读ram21
		{
			return;
		}
		buf &= ~0x0f;
		buf |= 0x0d;
		if (drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 20, 1, &buf) != WAVE1006_EXIT_OK)//写入部分ram21寄存器
		{
			return;
		}
		if (wave1006_enter_test_mode(WAVE1006_TEST_PAD | WAVE1006_TEST_OSC | WAVE1006_TEST_ADC) != WAVE1006_EXIT_OK)   //此处一定要注意，没进入测试模式也需要将I0切换掉，或者不能继续运行程序
		{
			return;
		}
		UsbPrintf("pad change OK\n");
		HAL_Delay(5);
		rtc_set_channel(g_rtcChanenel, RTC_SD3068);
		if (drv_wave1006_read_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 19, 1, &buf) != WAVE1006_EXIT_OK) 
		{
			return;
		}
		buf &= ~0x38;   //清0 RuleTempAdj[2:0]      //ADC尺子 暂选5
		buf |=  0x28;
		if (drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 19, 1, &buf) != WAVE1006_EXIT_OK)
		{
			UsbPrintf("i2c error after pad change \n");
			return;
		}	
		if (drv_wave1006_read_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 24, 1, &osc_buf[1]) != WAVE1006_EXIT_OK)  //读ram25   
		{
			return;
		}
		osc_buf[0]  = 0xf7;   //ram24
		osc_buf[1] &= ~0x1f;
		osc_buf[1] |= 0x19;		//ram25
		if (drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 23, 2, osc_buf) != WAVE1006_EXIT_OK)    //写ram24、ram25
		{
			UsbPrintf("i2c error after pad change \n");
			return;
		}
		UsbPrintf("i2c OK after pad change \n");

		// if (drv_wave1006_read_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 25, 1, &osc_buf[1]) != WAVE1006_EXIT_OK)  //读ram25   
		// {
		// 	return;
		// }
		// osc_buf[1] &= 0xfc;
		// osc_buf[1] |= 0x01;
		// if (drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 25, 1, &osc_buf[1]) != WAVE1006_EXIT_OK)    //写ram24、ram25
		// {
		// 	return;
		// }
		// UsbPrintf("hot test in mode 01 \n");
	} else {
		rtc_1006_exit_testmode(g_rtcChanenel);
	}
}

void AT_SD3078INT_Handle(char* command)
{
	
	int startNumber = 0;
	int endNumber = 0;

	CharToInt(command, strSearch(command, "=") + 1, &startNumber);
	CharToInt(command, strSearch(command, ",") + 1, &endNumber);
	
	if (startNumber > 0) {
		set_int_test(1, startNumber, endNumber);
		UsbPrintf("start int test, start %d, end %d\n", startNumber, endNumber);
	} else {
		set_int_test(0, 0, 0);
		UsbPrintf("end int test\n");
	}
	UsbPrintf("OK\n");
}


void AT_RTCLOAD_Handle(char* command)
{
	int number = 0;
	int i;
	uint8_t result[MAX_CHIP_NUMBER] = {0};
	CharToInt(command, strSearch(command, "=") + 1, &number);
	if (number > 0) {
		sd3078b_write_otp(number);
	} else {
		for (i = 1; i <= MAX_CHIP_NUMBER; i++) {
			result[i - 1] = sd3078b_write_otp(i);
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

#define TestNumber 			100
void AT_TEMPADC_Handle(char* command)
{
	uint16_t temp_adc = 0;
	uint8_t result;
	result = wave1006_read_temp_adc(&temp_adc);
	UsbPrintf("temp adc:%d, i2c[%d]\n", temp_adc, result);
}

void AT_GETRTCID_Handle(char* command)
{
	int number = 1;
	uint8_t buf[8] = {0};
	uint8_t i;
	CharToInt(command, strSearch(command,"=") + 1, &number);
	
	rtc_group_power(number, POWER_I2C);
	rtc_set_channel(number, RTC_SD3078B);
	rtc_read_multi_bytes(RTC_3078_ADDRESS, 0x72, buf, 8);
	for (i = 0; i < 8; i++) {
		UsbPrintf("%02X", buf[i]);
	}
	UsbPrintf("\nOK\n");
}

extern uint8_t Aecq100State;
void AT_AECQ100_Handle(char* command)
{
	Aecq100State = 9;//进行频率测试和VBAT测试
	UsbPrintf("OK\n");
}

uint8_t CheckFreqValue(float* result)
{
	int num;
	char strFreq[10] = {0};
	float frequency = 0;

	num = strSearch((char*)g_DeviceRxBuff, "Hz");
	if (num > 0) {//表示是以Hz字符结尾的数据，判断为有效数据
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
    AT_REALTIME_Handle,
    AT_READDATA_Handle,
    AT_SETTIME_Handle,
	AT_RTCCURRENT_Handle,
	AT_GROUPSETTEMP_Handle,
	AT_RTCVBAT_Handle,
	AT_RTC4096_Handle,
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
	AT_WRITECONFIG_Handle,
	AT_TESTFLAG_Handle,
	AT_TESTDATA_Handle,
	AT_ATTEST_Handle,
	AT_MACHINE_Handle,
	AT_1006TMODE_Handle,
	AT_SD3078INT_Handle,
	AT_RTCLOAD_Handle,
	AT_HOTVOLTAGE_Handle,
	AT_TEMPADC_Handle,
	AT_GETRTCID_Handle,
	AT_AECQ100_Handle,
};


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {//USB调试
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
	
	if (huart->Instance == USART2) {//频率计和上料机
        if (device_Rx_Cnt < UART_RECEIVE_BUFF_NUMBER && g_DeviceATStatus == 0) {
            g_DeviceRxBuff[device_Rx_Cnt] = deviceRxBuf;
        	device_Rx_Cnt++;
        }
        if (device_Rx_Cnt >= UART_RECEIVE_BUFF_NUMBER || deviceRxBuf == '\n') {
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
		} 
        memset(g_DeviceRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
        device_Rx_Cnt = 0;
        g_DeviceATStatus = 0;
        deviceTick = 0;
        lastDeviceRx = 0;
	}
}

