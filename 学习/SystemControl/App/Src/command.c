#include "command.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "cmsis_os.h"
#include "user_tool.h"
#include "soft_uart.h"
#include "flash.h"
#include "soft_i2c.h"
#include "sd2506.h"
// #include "voltage.h"
// #include "log.h"


#define UART_RECEIVE_BUFF_NUMBER    2000
#define UART_TRANSMIT_BUFF_NUMBER   300
#define USB_UART_TICK_CLEAR         100//10ms

extern UART_HandleTypeDef huart1;
static uint16_t usb_Rx_Cnt = 0;		//
static uint8_t usbRxBuf;
uint8_t g_UsbRxBuff[UART_RECEIVE_BUFF_NUMBER] = {0};
uint8_t g_UsbTxBuff[UART_TRANSMIT_BUFF_NUMBER] = {0};
uint8_t g_UsbATStatus = 0;


#define MAX_COMMAND_NUMBER  40
#define MAX_COMMAND_LENGTH  20
static char CommandList[MAX_COMMAND_NUMBER][MAX_COMMAND_LENGTH] = {
	"AT^HELP",
	"AT^REALTIME?", 
	"AT^SETTIME",
	"AT^WRITEFLASH",
	"AT^READFLASH",
	"AT^ERASEFLASH",
	"AT^ATTEST",
	"AT^RESTART",
	"AT^UPDATE",
	"AT^VERSION?",
	"AT^SETBSN",
	"AT^BSN?",
	"AT^2506VBAT?",
	"AT^2506CHARGE",
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


void usb_usart_init(void)
{
	HAL_UART_Receive_IT(&huart1, &usbRxBuf, 1);		
    memset(g_UsbRxBuff, 0x00, sizeof(g_UsbRxBuff));
	memset(g_UsbTxBuff, 0x00, sizeof(g_UsbTxBuff));
    osDelay(100);
    UsbPrintf("usb usart init success......\n");
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
	if (rtc_read_date(&rtc) == I2C_OK) {
		UsbPrintf("\n20%d/%d/%d %d:%d:%d\n", rtc.year, rtc.mon, rtc.day, rtc.hour, rtc.min, rtc.sec);
		UsbPrintf("OK\n");
	} else {
		UsbPrintf("sd2506 i2c error\n");
	}
}


void  AT_SETTIME_Handle(char* command)
{
	RealTime rtc = {0};
	int num = 0;
	int inNumber = 0;
	
	rtc_read_date(&rtc);
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
	rtc_write_date(&rtc);
	UsbPrintf("date set success!\nOK!\r\n");
}


void AT_WRITEFLASH_Handle(char* command)
{
	// static uint8_t number = 0;
	// command = NULL;
	// number++;
	// log_printf("2023.08.07 w25q128 flash log record test %03d\n", number);
	// UsbPrintf("write OK\n");
}

void AT_READFLASH_Handle(char* command)
{
	// int address;
	// uint8_t str[256] = {0};
	// int result;
    // int i;
	// uint32_t pageCount;

	// if (CharToInt(command, 13, &address) == 1) {
	// 	result = flash_read_data(address, str, 256);
	// 	if (result == 1) {
	// 		UsbPrintf("flash address[0x%x] print:\n%s\n", address, (char*)str);
	// 	} else {
	// 		UsbPrintf("read error %d\n", result);
	// 	}
	// } else {
	// 	pageCount = get_log_record_pages();
	// 	UsbPrintf("log use pages %d/%d:\n", pageCount, LOG_MAX_PAGE);
	// 	for (i = 0; i < pageCount; i++) {
	// 		flash_read_data(LOG_START_ADDR + i * PAGE_SIZE, str, PAGE_SIZE);
	// 		UsbPrintf("%s\n", (char*)str);
	// 	}
	// 	UsbPrintf("all log print success\n"); 
	// } 
}
void AT_ERASEFLASH_Handle(char* command)
{
	// int mode, address;
	// if (CharToInt(command, 14, &mode) == 1) {
	// 	CharToInt(command, strSearch(command, ",") + 1, &address);
	// 	if (mode >= 0 && mode <= 2) {
	// 		flash_erase(address, mode);
	// 		if (mode == 0) { 
	// 			UsbPrintf("full chip erase wait 1s\n");
	// 			osDelay(1000);
	// 			log_list_init(); 
	// 		}
	// 		UsbPrintf("OK\n");
	// 	}
	// } else {
	// 	UsbPrintf("input error!\n");
	// } 
}

extern float g_12Voltage;
extern float g_6Voltage;

void AT_ATTEST_Handle(char* command)
{
	// int i;
	// for (i = 0; i < MAX_DEVICES_CHANNEL; i++) {
	// 	devices_printf(&uart_dev[i], "device %d printf test...\n", i + 1);
	// }
	UsbPrintf("%.2fV %.2fV\n", g_12Voltage, g_6Voltage);
	UsbPrintf("OK\n");
}

void AT_RESTART_Handle(char* command)
{
	UsbPrintf("OK\n");
	NVIC_SystemReset();//重启
}

void AT_UPDATE_Handle(char* command)
{
	int len;

	len = strSearch(command, "=");
	if (len > 0 && strlen(command) > len + 1) {
		ClearStateFlash(CLEAR_ONLY_VERSION);
		SaveStringToFlash(command + len + 1, FLASH_VERSION_ADDR);//更新版本号
		SetUpdateStateFlash(SYSTEM_USB_UPDATE);
		UsbPrintf("OK\n");
	NVIC_SystemReset();//重启
	} else {
		UsbPrintf("input error\n");
		return;
	}
}

void AT_SETBSN_Handle(char* command)
{
	int len;
	len = strSearch(command, "=");
	if (len > 0 && strlen(command) > len + 1) {
		ClearStateFlash(CLEAR_ONLY_BSN);
		SaveStringToFlash(command + len + 1, FLASH_BSN_ADDR);
		UsbPrintf("OK\n");
	} else {
		UsbPrintf("input error\n");
	}
}

void AT_GETBSN_Handle(char* command)
{
	char bsn[BSN_NAME_LENGTH] = {0};
	GetStringFromFlash(bsn, BSN_NAME_LENGTH - 1, FLASH_BSN_ADDR);
	UsbPrintf("bsn:%s\nOK\n", bsn);
}

void AT_VERSION_Handle(char* command)
{
	char version[VERSION_NAME_LENGTH] = {0};
	GetStringFromFlash(version, VERSION_NAME_LENGTH - 1, FLASH_VERSION_ADDR);
	UsbPrintf("app version:%s\nOK\n", version);
}

void AT_2506VBAT_Handle(char* command)
{
	int vbat;
	rtc_read_vbat(&vbat);
	UsbPrintf("2506 vbat:%d0mV\n", vbat);
	UsbPrintf("OK\n");
}

void AT_2506CHARGE_Handle(char* command)
{
	rtc_set_bat_charge(1);
	UsbPrintf("OK\n");
}

typedef void(*function)(char* s);
function UsbCommand[MAX_COMMAND_NUMBER] = {
    AT_HELP_Handle,
    AT_REALTIME_Handle,
    AT_SETTIME_Handle,
	AT_WRITEFLASH_Handle,
	AT_READFLASH_Handle,
	AT_ERASEFLASH_Handle,
	AT_ATTEST_Handle,
	AT_RESTART_Handle,
	AT_UPDATE_Handle,
	AT_VERSION_Handle,
	AT_SETBSN_Handle,
	AT_GETBSN_Handle,
	AT_2506VBAT_Handle,
	AT_2506CHARGE_Handle,
};



void usb_usart_receive(void)
{
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


void CheckCommandList(void)
{
    uint8_t i;
    static uint32_t usbTick = 0;
    static uint32_t lastUsbRx = 0;
    if (usb_Rx_Cnt == 0) { return; }
    if (usb_Rx_Cnt > lastUsbRx) {
        lastUsbRx = usb_Rx_Cnt;
        usbTick = HAL_GetTick();
    }
    if (HAL_GetTick() >= usbTick + USB_UART_TICK_CLEAR || g_UsbATStatus == 1) {
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

