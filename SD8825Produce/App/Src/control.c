#include "control.h"
#include "main.h"
#include "string.h"
#include <stdio.h>
#include <stdarg.h>
#include "task.h"
#include "flash.h"
#include "manager.h"
#include "temp.h"
#include "rtc.h"
#include "power.h"

#define CONTROL_RECEIVE_BUFF_NUMBER    200
#define CONTROL_TRANSMIT_BUFF_NUMBER   300
#define CTRL_UART_TICK_CLEAR         10//10ms

// #define FUNCTION0_NUMBER        1
// #define FUNCTION1_NUMBER        FUNCTION0_NUMBER + 2
// #define FUNCTION2_NUMBER        FUNCTION1_NUMBER + 3
// #define FUNCTION3_NUMBER        FUNCTION2_NUMBER + 2
// #define FUNCTION4_NUMBER        FUNCTION3_NUMBER + 3
// #define FUNCTION5_NUMBER        FUNCTION4_NUMBER + 2
// #define FUNCTION6_NUMBER        FUNCTION5_NUMBER + 2
// #define FUNCTION7_NUMBER        FUNCTION6_NUMBER + 4
// #define FUNCTION8_NUMBER        FUNCTION7_NUMBER + 4
// #define FUNCTION9_NUMBER        FUNCTION8_NUMBER + 1
// #define MAX_SELECT_NUMBER     FUNCTION9_NUMBER
// char selectList[MAX_SELECT_NUMBER][FUNCTION_NAME_LENGTH] = {
//     "[ѡ����]",
//     "[��ȡ�ڴ�]", "[����ڴ�]",
//     "[IIC�Ӵ�����]", "[�¶ȶ�ȡ]", "[Ƶ�ʼ���ͨ����]",
//     "[�����¶�]", "[�鿴���ý��]", 
//     "[�������(�ۼ�)]", "[�������(�ظ�)]", "[��ȡ���Խ��]",
//     "[�����²�ֵ]", "[�鿴������]",
//     "[��д����]", "[�鿴��д���]",
//     "[����ͬ��ʱ��]", "[���������֤]", "[����ͬ��ʱ��]", "[���������֤]",
//     "[ͨ�ò���]", "[����ģʽ/�ۼ�]", "[����ģʽ/�ظ�]", "[�鿴���Խ��]",
//     "[�鿴ȡ����Ϣ]"
// };

typedef enum {
	OPERATE_FREE_MODE = 0,
	OPERATE_READ_FLASH,
	OPERATE_ERASE_FLASH,
	OPERATE_IIC_TEST,
	OPERATE_READ_TEMP,
	OPERATE_CONNECT,
	OPERATE_SET_CONFIG9,
	OPERATE_SHOW_CONFIG9,
	OPERATE_CAPADJUST_ADD,
	OPERATE_CAPADJUST_COVER,
	OPERATE_CAPADJUST_RESULT,
	OPERATE_CALCULATE,
	OPERATE_CALCUL_RESULT,
	OPERATE_DOWNLOAD,
	OPERATE_LOAD_RESULT,
	OPERATE_COLD_SYNC,
	OPERATE_COLD_ERROR,
	OPERATE_HOT_SYNC,
	OPERATE_HOT_ERROR,
	OPERATE_MACHINE_TEST,
	OPERATE_MACHINE_WRITE,
	OPERATE_MACHINE_COVER,//and write
	OPERATE_MACHINE_RESULT,
	OPERATE_TAKE_OUT_MESSAGE,
} OperateCommand;

extern UART_HandleTypeDef huart3;
static uint16_t control_Rx_Cnt = 0;		//
static uint8_t controlRxBuf;
uint8_t g_controlRxBuff[CONTROL_RECEIVE_BUFF_NUMBER] = {0};
uint8_t g_controlTxBuff[CONTROL_TRANSMIT_BUFF_NUMBER] = {0};
uint8_t g_controlATStatus = 0;
static uint8_t g_earseFlash = 0;

#define MAX_CONTROL_NUMBER  15
#define MAX_CONTROL_LENGTH  20
static char CtrlStringList[MAX_CONTROL_NUMBER][MAX_CONTROL_LENGTH] = {
    "AT^CONNECT?",
	"AT^REALTIME", 
	"AT^OPERATE",
	"AT^VDDMODE",
	"AT^VERSION?",
	"AT^CSN",
	"AT^BSN?",
	"AT^GETCSN",
};


void ControlPrintf(char* fmt,...)  
{  
	va_list ap;//
	memset(g_controlTxBuff, 0, CONTROL_TRANSMIT_BUFF_NUMBER);
	va_start(ap, fmt);//
	vsnprintf((char*)g_controlTxBuff, CONTROL_TRANSMIT_BUFF_NUMBER, fmt, ap);//
	va_end(ap);
	HAL_UART_Transmit(&huart3, g_controlTxBuff, strlen((char*)g_controlTxBuff), 1000);
}


void ControlInit(void)
{
	HAL_UART_Receive_IT(&huart3, &controlRxBuf, 1);		
    memset(g_controlRxBuff, 0x00, sizeof(g_controlRxBuff));
	memset(g_controlTxBuff, 0x00, sizeof(g_controlTxBuff));
}

void CTL_CONNECT_Handle(char* command)
{
	// ControlPrintf("temp:%.1f connect OK\n", temp_read_value(1));
}

void CTL_REALTIME_Handle(char* command)
{
    RealTime rtc = {0};
	int num = 0;
	int inNumber = 0;

	// UsbPrintf("receive system:%s", command);
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
	ControlPrintf("sync time OK\n");
}


void OPERATE_READ_FLASH_Handle(void)
{
	uint8_t buffer[MAX_CHIP_NUMBER] = {0xff};
	uint32_t address;
	uint8_t count, lastCount = 0;
	int i, j;
	char message[7][20] = {
		"���������", "�¶ȼ�", "�²�����", "����", "��¼", "��̨����", "ȡ�ϼ�¼"};
	if (g_earseFlash > 0) {
		ControlPrintf("message:������գ��Ժ�����\n");
		return;
	}
	for (j = 0; j < 6; j++) {//6�����Ա�־
		memset(buffer, 0xff, MAX_CHIP_NUMBER);
		address = TEST_FALG_START_ADDR + j * PAGE_SIZE;
		flash_read_data(address, buffer, MAX_CHIP_NUMBER);
		count = 0;
		for (i = 0; i < MAX_CHIP_NUMBER ; i++) {
			if (j == 1) {
				if (buffer[i] == 0xE0) {
					count++;
				}
			} else {
				if (buffer[i] == '1') {
					count++;
				}
			}
		}
		if (count == 0) {
			ControlPrintf("okNumber:%d chipName:%d message:%s\n", lastCount, MAX_CHIP_NUMBER, message[j]);
			return;
		} else {
			lastCount = count;
		}
	}
	ControlPrintf("okNumber:%d chipName:%d message:%s\n", lastCount, MAX_CHIP_NUMBER, message[j]);
}

void OPERATE_ERASE_FLASH_Handle(void)
{
	HAL_Delay(200);
	if (flash_erase(0, 0) != 1) {//ȫƬ����
        ControlPrintf("message:����쳣��������\n");
		return;
    }
	ControlPrintf("message:��ʼ�������\n");
	UsbPrintf("earse full chip need 40s ~ 200s\n");
	g_earseFlash = 1;
}

void CheckFlashEarse(void)
{
	static uint32_t lastTick = 0;
	static uint32_t usedTime = 0;
	if (g_earseFlash == 1) {
		g_earseFlash = 2;
		lastTick = HAL_GetTick();
		usedTime = 0;
	} else if (g_earseFlash == 2) {
		if (HAL_GetTick() < lastTick + 2000) { return; }
		usedTime += HAL_GetTick() - lastTick;
		lastTick = HAL_GetTick();
		UsbPrintf("wait for %ds...\n", usedTime / 1000);
		ControlPrintf("message:��ȴ�,�ѽ���%ds\n", usedTime / 1000);
		if (CheckBusy() == 1) {
			log_list_init(); 
			ControlPrintf("okNumber:%d chipName:%d message:�������ɣ�\n", MAX_CHIP_NUMBER, MAX_CHIP_NUMBER);
			g_earseFlash = 0;
		}
	}
}


void OPERATE_IIC_TEST_Handle(void)
{
	int i = 0;
	uint8_t count = 0;
	uint8_t buffer;
	ControlPrintf("okNumber:0 chipName:0 message:��ʼiic��ȡ\n");
	for (i = 1; i <= MAX_CHIP_NUMBER; i++) {
		rtc_group_power(i, POWER_I2C);
		rtc_set_channel(i, RTC_SD8825);
		HAL_Delay(5);
		if (rtc_read_multi_bytes(0x64, 0x00, &buffer, 8) == I2C_OK) {
			count++;
		}
	}
	ControlPrintf("okNumber:%d chipName:%d message:����ɣ�\n", count, MAX_CHIP_NUMBER);
}

void OPERATE_READ_TEMP_Handle(void)
{
	int i = 0;
	uint8_t count = 0;
	uint8_t buffer[512] = {0};
	uint16_t temp[MAX_CHIP_NUMBER] = {0};
	uint8_t maxCount = 0;
	int maxIndex = 0;

	ControlPrintf("okNumber:0 chipName:0 message:��ʼ�¶ȶ�ȡ\n");
	for (i = 1; i <= MAX_CHIP_NUMBER; i++) {
		rtc_group_power(i, POWER_I2C);
		HAL_Delay(5);
		if (wave1006_read_temp_adc(&temp[i]) == I2C_OK) {
			count++;
			buffer[temp[i]]++;
		}
	}
	for (i = 0; i < 512; i++) {
		if (buffer[i] > maxCount) {
			maxCount = buffer[i];
			maxIndex = i;
		}
	}
	if (maxIndex > 400) {//�¶�ֵ����0��ʱ������ת��
		maxIndex = maxIndex - 512;
	}
	ControlPrintf("okNumber:%d chipName:%d message:����[%d��]:%d��\n", count, MAX_CHIP_NUMBER, maxIndex, maxCount);
}


void OPERATE_CAPADJUST_RESULT_Handle(void)
{
	uint8_t buffer[MAX_CHIP_NUMBER] = {0xff};
	int i = 0;
	uint8_t value[256] = {0};
	uint8_t maxCount = 0;
	int maxIndex = 0;
	uint8_t count = 0;

	flash_read_data(TEST_FALG_START_ADDR + FLAG_TEST_DOT * PAGE_SIZE, buffer, MAX_CHIP_NUMBER);
	for (i = 0; i < MAX_CHIP_NUMBER; i++) {
		value[buffer[i]]++;
		if (buffer[i] != 0xff) {
			count++;
		}
	}
	for (i = 0; i < 256; i++) {
		if (value[i] > maxCount) {
			maxCount = value[i];
			maxIndex = i;
		}
	}
	ControlPrintf("okNumber:%d chipName:%d message:����[%02X]:%d��\n", count, MAX_CHIP_NUMBER, maxIndex, maxCount);
}

void OPERATE_RESULT_Handle(TestFlagList mode)
{
	uint8_t buffer[MAX_CHIP_NUMBER] = {0xff};
	int i = 0;

	uint8_t count = 0;

	flash_read_data(TEST_FALG_START_ADDR + mode * PAGE_SIZE, buffer, MAX_CHIP_NUMBER);
	for (i = 0; i < MAX_CHIP_NUMBER; i++) {
		if (buffer[i] == '1') {
			count++;
		}
	}
	ControlPrintf("okNumber:%d chipName:%d message:��ȡ���\n", count, MAX_CHIP_NUMBER);
}

void OPERATE_COLD_SYNC_Handle(void)
{

}

void OPERATE_COLD_ERROR_Handle(void)
{

}

void OPERATE_HOT_SYNC_Handle(void)
{

}

void OPERATE_HOT_ERROR_Handle(void)
{

}

void OPERATE_TAKE_OUT_MESSAGE_Handle(void)
{

}


void CTL_OPERATE_Handle(char* command)//���ù���ģʽ
{	
	int num = 0;
	int inNumber = 0;

	UsbPrintf("receive system:%s", command);
	log_printf("receive system:%s", command);
	num = strSearch(command, "AT^OPERATE=");
	if (num >= 0) {
		num += strlen("AT^OPERATE=");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		switch (inNumber) {
			case OPERATE_FREE_MODE:
			break;
			case OPERATE_READ_FLASH:
				OPERATE_READ_FLASH_Handle();
			break;
			case OPERATE_ERASE_FLASH:
				OPERATE_ERASE_FLASH_Handle();
			break;
			case OPERATE_IIC_TEST:
				OPERATE_IIC_TEST_Handle();
			break;
			case OPERATE_READ_TEMP:
				OPERATE_READ_TEMP_Handle();
			break;
			case OPERATE_CONNECT:
				ControlPrintf("okNumber:0 chipName:0 message:������ͨ����\n");
				manager_set_mode(FREQ_CONNECT);
			break;
			case OPERATE_SET_CONFIG9:
				ControlPrintf("okNumber:0 chipName:0 message:��ʼ����...\n");
				manager_set_mode(TEMP_TEST);
			break;
			case OPERATE_SHOW_CONFIG9:
			// OPERATE_SHOW_CONFIG9_Handle();
			break;
			case OPERATE_CAPADJUST_ADD:
				ControlPrintf("okNumber:0 chipName:0 message:��ʼ�������...\n");
				manager_set_mode(CAP_ADJUST);
			break;
			case OPERATE_CAPADJUST_COVER:
				ControlPrintf("okNumber:0 chipName:0 message:����ٵ������...\n");
				manager_set_mode(CAP_ADJUST_COVER);
			break;
			case OPERATE_CAPADJUST_RESULT:
				OPERATE_CAPADJUST_RESULT_Handle();
			break;
			case OPERATE_CALCULATE:
				ControlPrintf("okNumber:0 chipName:0 message:��ʼ����...\n");
				manager_set_mode(CALCULATE);
			break;
			case OPERATE_CALCUL_RESULT:
				OPERATE_RESULT_Handle(FLAG_CALCULATE);
			break;
			case OPERATE_DOWNLOAD:
				ControlPrintf("okNumber:0 chipName:0 message:��ʼ��¼�²�����\n");
				manager_set_mode(CAP_DOWNLOAD);
			break;
			case OPERATE_LOAD_RESULT:
				OPERATE_RESULT_Handle(FLAG_TEMPLOAD);
			break;
			case OPERATE_COLD_SYNC:
				manager_set_mode(SYNC_RTC_TIME);
			break;
			case OPERATE_COLD_ERROR:
				manager_set_mode(CHECK_ERROR_TIME);
			break;
			case OPERATE_HOT_SYNC:
				manager_set_mode(SYNC_RTC_TIME);
			break;
			case OPERATE_HOT_ERROR:
				manager_set_mode(CHECK_ERROR_TIME);
			break;
			case OPERATE_MACHINE_TEST:
				ControlPrintf("okNumber:0 chipName:0 message:��̨��ͨ����\n");
				manager_set_mode(MACHINE_TEST);
			break;
			case OPERATE_MACHINE_WRITE:
				ControlPrintf("okNumber:0 chipName:0 message:�޴�ģʽ\n");
				manager_set_mode(MACHINE_WRITE);
			break;
			case OPERATE_MACHINE_COVER:
				ControlPrintf("okNumber:0 chipName:0 message:������ٻ�̨����\n");
				manager_set_mode(MACHINE_COVER);
			break;
			case OPERATE_MACHINE_RESULT:
				OPERATE_RESULT_Handle(FLAG_MACHINE);
			break;
			case OPERATE_TAKE_OUT_MESSAGE:
			break;
			default:break;
		}
	}

}


void CTL_VDDMODE_Handle(char* command)
{

}

void CTL_VERSION_Handle(char* command)
{
	ControlPrintf("message:%s\n", SOFT_VERSION);
}

extern uint16_t g_csn;
void CTL_CSN_Handle(char* command)
{
	int number;
	CharToInt(command, strSearch(command, "=") + 1, &number);
	if (number > 2000 && 9999) {
		g_csn = number;
		UsbPrintf("receive command csn = %d\n", g_csn);
		log_printf("receice command csn = %d\n", g_csn);
	}
}

void CTL_GETCSN_Handle(char* command)
{
	ControlPrintf("message:%d\n", g_csn);
}

void CTL_BSN_Handle(char* command)
{
	char bsn[BSN_LENGTH + 1] = {0};
	GetStringFromFlash(bsn, BSN_LENGTH + 1, BSN_ADDR);
	ControlPrintf("message:%s\n", bsn);
}

typedef void(*function)(char* s);
function ControlCommand[MAX_CONTROL_NUMBER] = {
    CTL_CONNECT_Handle,
    CTL_REALTIME_Handle,
    CTL_OPERATE_Handle,
	CTL_VDDMODE_Handle,
	CTL_VERSION_Handle,
	CTL_CSN_Handle,
	CTL_BSN_Handle,
	CTL_GETCSN_Handle,
};

void control_usart_receive(void)
{
    if (control_Rx_Cnt < CONTROL_RECEIVE_BUFF_NUMBER && g_controlATStatus == 0) {
        g_controlRxBuff[control_Rx_Cnt] = controlRxBuf;
    	control_Rx_Cnt++;
    }
    if (control_Rx_Cnt >= CONTROL_RECEIVE_BUFF_NUMBER || controlRxBuf == '\n') {
        g_controlATStatus = 1;
    }
    HAL_UART_Receive_IT(&huart3, &controlRxBuf, 1);
}

void CheckControlList(void)
{
    uint8_t i;
    static uint16_t controlTick = 0;
    static uint16_t lastControlRx = 0;

	if (g_earseFlash > 0) { CheckFlashEarse(); }//�봮��ͨ���޹أ�����flash����ʱ����
    if (control_Rx_Cnt == 0) { return; }

    if (control_Rx_Cnt > lastControlRx) {
        lastControlRx = control_Rx_Cnt;
        controlTick = 0;
    } else {
        controlTick++;
    }
    if (controlTick >= CTRL_UART_TICK_CLEAR || g_controlATStatus == 1) {
        for (i = 0; i < MAX_CONTROL_NUMBER; i++) {
            if (strlen(CtrlStringList[i]) > 0 && strSearch((char*)g_controlRxBuff, CtrlStringList[i]) == 0) {
                ControlCommand[i]((char*)g_controlRxBuff);
                break;
            }
        }			
        memset(g_controlRxBuff, 0, CONTROL_RECEIVE_BUFF_NUMBER);
        control_Rx_Cnt = 0;
        g_controlATStatus = 0;
        controlTick = 0;
        lastControlRx = 0;
    }
}