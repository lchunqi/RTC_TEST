#include "main.h"
#include "devices.h"
#include "cmsis_os.h"
#include "soft_uart.h"
#include <string.h>
#include "user_tool.h"
#include "lcd_driver.h"
#include  "sd2506.h"
#include <stdio.h>

uint32_t g_rxTick[MAX_DEVICES_CHANNEL] = {0};
extern ExcelMessage g_deviceMessage[MAX_DEVICES_CHANNEL];
extern RealTime g_systemTime;

// typedef struct {
//     uint8_t board;
//     float systemTemp;
//     uint8_t okNumber;
//     uint8_t chipName;
//     int chipTemp;
//     char state[20];
//     char message[20];
// }ExcelMessage;
#define KEY_WORD_NUMBER             7
char DeviceKeyWord[KEY_WORD_NUMBER][15] = {
    "board:", "systemTemp:", "okNumber:", "chipName:", "chipTemp:", "state:", "message:"}; 
uint8_t g_lengthKeyWord[KEY_WORD_NUMBER] = {0};
uint8_t g_syncState[MAX_DEVICES_CHANNEL] = {0};

void init_key_word_length(void)
{
    int i;
    for (i = 0; i < KEY_WORD_NUMBER; i++) {
        g_lengthKeyWord[i] = strlen(DeviceKeyWord[i]);
    }
}

void selete_vddmode_devices(uint8_t command)
{
    int i;
    for (i = 0; i < MAX_DEVICES_CHANNEL; i++) {
        if (strSearch(g_deviceMessage[i].state, "已连接") >= 0) {
            devices_printf(&uart_dev[i], "AT^VDDMODE=%d\n", command);
        }
    }
}

void set_chipid_devices(uint16_t command)
{
    int i;
    for (i = 0; i < MAX_DEVICES_CHANNEL; i++) {
        if (strSearch(g_deviceMessage[i].state, "已连接") >= 0) {
            devices_printf(&uart_dev[i], "AT^CSN=%d\n", command);
        }
    }
}


void operate_to_devices(uint8_t command)
{
    int i;
    for (i = 0; i < MAX_DEVICES_CHANNEL; i++) {
        if (strSearch(g_deviceMessage[i].state, "已连接") >= 0) {
            switch (command) {
                case OPERATE_FREE_MODE: 
                break;
	            case OPERATE_READ_FLASH:
                case OPERATE_ERASE_FLASH:
                    devices_printf(&uart_dev[i], "AT^OPERATE=%d\n", command);
                break;
                case OPERATE_READ_VERSION:
                    devices_printf(&uart_dev[i], "AT^VERSION?\n");// 插队1个命令
                break;
                case OPERATE_READ_BSN:
                    devices_printf(&uart_dev[i], "AT^BSN?\n");// 插队1个命令
                break;
                case OPERATE_IIC_TEST:
                case OPERATE_READ_TEMP:
                case OPERATE_CONNECT:
                case OPERATE_SET_CONFIG9:
                case OPERATE_SHOW_CONFIG9:
                case OPERATE_CAPADJUST_ADD:
                case OPERATE_CAPADJUST_COVER:
                case OPERATE_CAPADJUST_RESULT:
                case OPERATE_CALCULATE:
                case OPERATE_CALCUL_RESULT:
                    devices_printf(&uart_dev[i], "AT^OPERATE=%d\n", command - 2);
                break;
                case OPERATE_SET_ID://2024.7.18新增             // 插队1个命令
                break;
                case OPERATE_READ_CHIPID:
                    devices_printf(&uart_dev[i], "AT^GETCSN\n");// 插队1个命令
                break;
                case OPERATE_DOWNLOAD:
                case OPERATE_LOAD_RESULT:
                case OPERATE_COLD_SYNC:
                case OPERATE_COLD_ERROR:
                case OPERATE_HOT_SYNC:
                case OPERATE_HOT_ERROR:
                case OPERATE_MACHINE_TEST:
                case OPERATE_MACHINE_WRITE:
                case OPERATE_MACHINE_COVER://and write
                case OPERATE_MACHINE_RESULT:
                case OPERATE_TAKE_OUT_MESSAGE:
                    devices_printf(&uart_dev[i], "AT^OPERATE=%d\n", command - 4);
                break;
                default: break;
            }
        }
    }
}

//单通道的接收处理函数
void DeviceMessageHandle(uint8_t channel, char* str)
{
    int num;
    float sysTemp = 0;
    int convert = 0;
    int messageEnd = 0;

    g_rxTick[channel] = HAL_GetTick();
    num = strSearch(str, "connect OK");
    if (num >= 0) {
        g_deviceMessage[channel].board = channel;
        num = strSearch(str, "temp:");
        CharToFloat(str, num + 5, &sysTemp);
        g_deviceMessage[channel].systemTemp = sysTemp;
        memset(g_deviceMessage[channel].state, 0, 20);
        strcpy(g_deviceMessage[channel].state, "已连接");
        // g_txFlag[channel] = DEVICE_NO_TX;
    }
    num = strSearch(str, "sync time OK");
    if (num >= 0) {
        memset(g_deviceMessage[channel].message, 0, 20);
        snprintf(g_deviceMessage[channel].message, 20, "同步时间 %d分", g_systemTime.min);
        g_syncState[channel] = 2;
    }
    
    num = strSearch(str, DeviceKeyWord[1]);
    if (num >= 0) {
        CharToFloat(str, num + g_lengthKeyWord[1], &sysTemp);
        g_deviceMessage[channel].systemTemp = sysTemp;
    }
    num = strSearch(str, DeviceKeyWord[2]);
    if (num >= 0) {
        CharToInt(str, num + g_lengthKeyWord[2], &convert);
        g_deviceMessage[channel].okNumber = convert;
    }
    num = strSearch(str, DeviceKeyWord[3]);
    if (num >= 0) {
        CharToInt(str, num + g_lengthKeyWord[3], &convert);
        g_deviceMessage[channel].chipName = convert;
    }
    num = strSearch(str, DeviceKeyWord[4]);
    if (num >= 0) {
        CharToInt(str, num + g_lengthKeyWord[4], &convert);
        g_deviceMessage[channel].chipTemp = convert;
    }
    num = strSearch(str, DeviceKeyWord[6]);
    if (num >= 0) {
        memset(g_deviceMessage[channel].message, 0, 20);
        // messageEnd = strSearch(str + num, "\n") - num - g_lengthKeyWord[6];
        messageEnd = strSearch(str, "\n") - num - g_lengthKeyWord[6];
        if (messageEnd > 19) {
            messageEnd = 19;
        }
        strncpy(g_deviceMessage[channel].message, str + num + g_lengthKeyWord[6], messageEnd);
        g_deviceMessage[channel].message[19] = '\0';
    }
}

void CheckDeviceList(void)
{
    static uint16_t count[MAX_DEVICES_CHANNEL] = {0};
    static uint16_t lastLength[MAX_DEVICES_CHANNEL] = {0};
    int i;
    for (i = 0; i < MAX_DEVICES_CHANNEL; i++) {
        if (uart_dev[i].rx_length == 0) {
            continue;
        }
        if (uart_dev[i].rx_length > lastLength[i]) {
            lastLength[i] = uart_dev[i].rx_length;
            count[i] = 0;
        } else {
            count[i]++;
        }
        if (count[i] >= SOFT_UART_TICK_CLEAR || uart_dev[i].rx_flag == 1) {
            DeviceMessageHandle(i, (char*)uart_dev[i].rx_buffer);//根据不同的场景处理设备消息的函数
            memset(uart_dev[i].rx_buffer, 0, SOFT_UART_RECEICE_LONG);
            uart_dev[i].rx_length = 0;
            uart_dev[i].rx_flag = 0;
            count[i] = 0;
            lastLength[i] = 0;
        }
    }
}

void CheckDeviceTime(void)
{
    static uint32_t lastTick[MAX_DEVICES_CHANNEL] = {0};
    int i;
    
    for (i = 0; i < MAX_DEVICES_CHANNEL; i++) {
        if (strSearch(g_deviceMessage[i].state, "已连接") >= 0) {
            if (g_syncState[i] == 0 || (g_syncState[i] == 1 && (HAL_GetTick() >= lastTick[i] + 4000))  ||  (g_syncState[i] == 2 && (HAL_GetTick() >= lastTick[i] + 600000))) {//刚刚连接上和每10分钟发送1次同步时间
                UsbPrintf("通道[%d]发起同步时间\n", i);
                lastTick[i] = HAL_GetTick();
                g_syncState[i] = 1;
                devices_printf(&uart_dev[i], "AT^REALTIME=year:%d,mon:%d,day:%d,hour:%d,min:%d,sec:%d\n",
                    g_systemTime.year, g_systemTime.mon, g_systemTime.day, g_systemTime.hour, g_systemTime.min, g_systemTime.sec);
            }
        } else {
            g_syncState[i] = 0;
        }
    }
}

void  CheckDeviceHeart(void)
{
    int i;
    static uint32_t lastTick[MAX_DEVICES_CHANNEL] = {0};

    for (i = 0; i < MAX_DEVICES_CHANNEL; i++) {
        if (HAL_GetTick() < lastTick[i] + 4000) {
            continue;
        }
        lastTick[i] = HAL_GetTick();
        if ((HAL_GetTick() > g_rxTick[i] + 5000)) {
            if (strSearch(g_deviceMessage[i].state, "已连接") >= 0) {
                UsbPrintf("check %d disconnect\n", i);
                memset(g_deviceMessage[i].state, 0, 20);
                strcpy(g_deviceMessage[i].state, "未连接");
                memset(g_deviceMessage[i].message, 0, 20);
                g_rxTick[i] = HAL_GetTick();
            }
        }
        // devices_printf(&uart_dev[i], "AT^CONNECT?\n");
    }
} 

void StartDevicesTask(void const * argument)
{
    init_key_word_length();
    while (1) {
        osDelay(4);
        // LedDeviceTest();
        CheckDeviceList();
        CheckDeviceHeart();
        osDelay(1);
        CheckDeviceTime();
    }
}


