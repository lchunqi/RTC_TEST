#include "wifi.h"
#include "main.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdarg.h>
#include "user_tool.h"
#include "flash.h"
#include <stdio.h>

#define WIFI_RECEIVE_BUFF_NUMBER    2000
#define WIFI_TRANSMIT_BUFF_NUMBER   300
#define WIFI_UART_TICK_CLEAR         100//ms

extern UART_HandleTypeDef huart3;
static uint16_t wifi_Rx_Cnt = 0;		//
static uint8_t wifiRxBuf;
uint8_t g_wifiRxBuff[WIFI_RECEIVE_BUFF_NUMBER] = {0};
uint8_t g_wifiTxBuff[WIFI_TRANSMIT_BUFF_NUMBER] = {0};
uint8_t g_wifiATStatus = 0;

uint8_t g_wifiState = WIFI_NOT_CONNECT;
uint8_t g_mesState = 0;
char g_ipName[20] = {0};

void WifiPrintf(char* fmt,...)  
{  
	va_list ap;//
	memset(g_wifiTxBuff, 0, WIFI_TRANSMIT_BUFF_NUMBER);
	va_start(ap, fmt);//
	vsnprintf((char*)g_wifiTxBuff, WIFI_TRANSMIT_BUFF_NUMBER, fmt, ap);//
	va_end(ap);
	HAL_UART_Transmit(&huart3, g_wifiTxBuff, strlen((char*)g_wifiTxBuff), 1000);
}

void wifi_usart_init(void)
{
	HAL_UART_Receive_IT(&huart3, &wifiRxBuf, 1);		
    memset(g_wifiRxBuff, 0x00, sizeof(g_wifiRxBuff));
	memset(g_wifiTxBuff, 0x00, sizeof(g_wifiTxBuff));
    osDelay(100);
    UsbPrintf("wifi usart init success......\n");
}

void clear_wifi_buffer(void)
{
	memset(g_wifiRxBuff, 0, WIFI_RECEIVE_BUFF_NUMBER);
	wifi_Rx_Cnt = 0;
}

void wifi_usart_receive(void)
{
	if (wifi_Rx_Cnt < WIFI_RECEIVE_BUFF_NUMBER && g_wifiATStatus == 0) {
		g_wifiRxBuff[wifi_Rx_Cnt] = wifiRxBuf;
		wifi_Rx_Cnt++;
	}
	if (wifi_Rx_Cnt >= WIFI_RECEIVE_BUFF_NUMBER) {
		g_wifiATStatus = 1;
	}
	HAL_UART_Receive_IT(&huart3, &wifiRxBuf, 1);

}

uint8_t wifi_set_restart(void)
{
	uint16_t i;

	WifiPrintf("+++");//先退出透传模式
	clear_wifi_buffer();
	osDelay(50);
	WifiPrintf("AT+RST\r\n");
	for (i = 0; i < 100; i++) {
		osDelay(20);
		if (strSearch((char*)g_wifiRxBuff, "ready") >= 0) {
			UsbPrintf("wifi restart OK\n");
			clear_wifi_buffer();
			break;
		}
		//UsbPrintf("%s", (char*)g_wifiRxBuff);
		clear_wifi_buffer();
	}
	if (i == 100) {
		UsbPrintf("wifi usart channel is error\n");
		g_wifiState = WIFI_NOT_CONNECT;
		return 0;
	}
	return 1;
}

uint8_t wifi_connect_system(void)
{
	uint16_t i;
	int left, right;

	clear_wifi_buffer();
	WifiPrintf("AT+CWJAP=\"d-wave5\",\"wave101203409\"\r\n");
	for (i = 0; i < 1000; i++) {
		osDelay(10);
		if (strSearch((char*)g_wifiRxBuff, "OK") >= 0) {
			UsbPrintf("connect wifi OK\n");
			clear_wifi_buffer();	
			break;
		}
	}
	if (i == 1000) {//等待10s超时
		UsbPrintf("%s", (char*)g_wifiRxBuff);
		UsbPrintf("can't connect wifi\n");
		g_wifiState = WIFI_NOT_CONNECT;
		return 0;
	}

	WifiPrintf("AT+CIFSR\r\n");//获取IP地址
	for (i = 0; i < 100; i++) {
		osDelay(10);
		left = strSearch((char*)g_wifiRxBuff, "STAIP");
		if (left >= 0) {
			left += 7;
			right = strSearch((char*)(g_wifiRxBuff + left), "\"");
			if (right >= 0) {
				right += left;
				for (int j = 0; j < (right - left); j++) {
					g_ipName[j] = g_wifiRxBuff[left + j];
				}
			}
			UsbPrintf("IP:%s\n", g_ipName);
			clear_wifi_buffer();
			break;
		}
	}
	if (i == 100) {
		UsbPrintf("ip address is error\n");
		g_wifiState = WIFI_NOT_CONNECT;
		return 0;
	}

	return 1;
}

uint8_t wifi_building_tcpip(void)
{
	uint16_t i;
	//WifiPrintf("AT+CIPSTART=\"TCP\",\"192.168.1.86\",8080\r\n");
	WifiPrintf("AT+CIPSTART=\"TCP\",\"192.168.1.33\",8385\r\n");
	for (i = 0; i < 1000; i++) {
		osDelay(10);
		if (strSearch((char*)g_wifiRxBuff, "OK") >= 0 || strSearch((char*)g_wifiRxBuff, "CONNECTED") >= 0) {
			UsbPrintf("tcp connect OK\n");
			clear_wifi_buffer();
			g_wifiState = WIFI_CONNECTED_CMD;
			break;
		} else if (strSearch((char*)g_wifiRxBuff, "CLOSED") >= 0) {
			UsbPrintf("tcp connect need to try\n");
			clear_wifi_buffer();
			g_wifiState = WIFI_BEFOR_TCP;
			return 0;
		}
 	}
	if (i == 1000) {
		UsbPrintf("tcp connect is error\n");
		return 0;
	}
	return 1;
}

uint8_t wifi_set_free_communicate(void)
{
	int i;
	int left;

	g_wifiState = WIFI_CONNECTED_CMD;
	WifiPrintf("AT+CIPMODE=1\r\n");
	for (i = 0; i < 100; i++) {
		osDelay(10);
		left = strSearch((char*)g_wifiRxBuff, "OK");
		if (left >= 0) {
			UsbPrintf("set ipmode OK\n");
			clear_wifi_buffer();
			break;
		} else if (strSearch((char*)g_wifiRxBuff, "ERROR") >= 0) {
			clear_wifi_buffer();
			g_wifiState = WIFI_BEFOR_TCP;
			UsbPrintf("free mode is error\n");
			return 0;
		}
	}
	if (i == 100) {
		UsbPrintf("set ipmode is error\n");
		return 0;
	}

	WifiPrintf("AT+CIPSEND\r\n");
	for (i = 0; i < 100; i++) {
		osDelay(10);
		left = strSearch((char*)g_wifiRxBuff, ">");
		if (strSearch((char*)g_wifiRxBuff, ">") >= 0) {
			clear_wifi_buffer();
			g_wifiState = WIFI_CONNECTED_FREE;
			break;
		} else if (strSearch((char*)g_wifiRxBuff, "ERROR") >= 0) {
			clear_wifi_buffer();
			g_wifiState = WIFI_BEFOR_TCP;
			UsbPrintf("free mode is error\n");
			return 0;
		}
	}
	if (i == 100) {
		UsbPrintf("free mode is error\n");
		return 0;
	}
	UsbPrintf("free communicate is starting...\n");
	return 1;
}


uint8_t wifi_set_cmd_communicate(void)
{
	WifiPrintf("+++");
	g_wifiState = WIFI_CONNECTED_CMD;
	return 1;
}

void connect_wifi(void)
{
	g_wifiState = WIFI_CONNECTING;
 	if (wifi_set_restart() == 0) {
		return;
	}
 	if (wifi_connect_system() == 0) {
		return;
	}
 	if (wifi_building_tcpip() == 0) {
		return;
	}
	if (wifi_set_free_communicate() == 0) {
		return;
	}
}

void wifi_cmd_update(char* command)
{
	int len;

	len = strSearch(command, "=");
	if (len > 0 && strlen(command) > len + 1) {
		ClearStateFlash(CLEAR_ONLY_VERSION);
		SaveStringToFlash(command + len + 1, FLASH_VERSION_ADDR);//更新版本号
		SetUpdateStateFlash(SYSTEM_WIFI_UPDATE);
		WifiPrintf("OK");
		UsbPrintf("wifi cmd update:%s\n", command + len + 1);
		NVIC_SystemReset();//重启
	} else {
		UsbPrintf("input error\n");
		return;
	}
}

void WifiMassage(void)
{
    // int num;
    static uint16_t wifiTick = 0;
    static uint16_t lastWifiRx = 0;
    if (wifi_Rx_Cnt == 0) {
        wifiTick = 0;
        lastWifiRx = 0;
        return;
    }
    if (wifi_Rx_Cnt > lastWifiRx) {
        lastWifiRx = wifi_Rx_Cnt;
        wifiTick = 0;
    } else {
        wifiTick++;
    }
    if (wifiTick >= WIFI_UART_TICK_CLEAR || g_wifiATStatus == 1) {
		if (strSearch((char*)g_wifiRxBuff, "AT^UPDATE") >= 0) {
			wifi_cmd_update((char*)g_wifiRxBuff);
		} 
        // else {
		// 	UsbPrintf("%s", (char*)g_wifiRxBuff);
		// }
		clear_wifi_buffer();
        g_wifiATStatus = 0;
        wifiTick = 0;
        lastWifiRx = 0;
	}
}

void handshake_mes_system(void)
{
	int i;
	char BSN[BSN_NAME_LENGTH] = {0};
    GetStringFromFlash(BSN, BSN_NAME_LENGTH - 1, FLASH_BSN_ADDR);
    WifiPrintf("%c%c%c%s", BSN[0], BSN[1], BSN[2], g_ipName);
    for (i = 0; i < 20; i++) {
        osDelay(20);
        if (strSearch((char*)g_wifiRxBuff, "OK") >= 0) {
			clear_wifi_buffer();
            g_mesState = 1;
            UsbPrintf("mes shakehand ok\n");
            return;
        } else if (strSearch((char*)g_wifiRxBuff, "NG") >= 0) {
			clear_wifi_buffer();
            g_mesState = 0;
            return;
        }
    }
    if (i == 20) {
		WifiPrintf("%c%c%c%s", BSN[0], BSN[1], BSN[2], g_ipName);
		for (i = 0; i < 50; i++) {
			osDelay(20);
			if (strSearch((char*)g_wifiRxBuff, "OK") >= 0) {
				clear_wifi_buffer();
				g_mesState = 1;
				UsbPrintf("mes shakehand ok\n");
				return;
			} else if (strSearch((char*)g_wifiRxBuff, "NG") >= 0) {
				clear_wifi_buffer();
				g_mesState = 0;
				return;
			}
		}
        UsbPrintf("mes shakehand timeout, restart wifi\n");
        g_mesState = 0;
        g_wifiState = WIFI_NOT_CONNECT;
    }
}

void wifi_heart(void)
{
	static uint32_t lastTick = 0;

	if (g_wifiState == WIFI_CONNECTED_FREE) {
		WifiMassage();
		if (HAL_GetTick() < lastTick + 60000) {return;}//每分钟发个消息确认是否在线
		lastTick = HAL_GetTick();
        handshake_mes_system();
	} else {
		if (HAL_GetTick() < lastTick + 4000) {return;}
		lastTick = HAL_GetTick();
		
		if (g_wifiState == WIFI_CONNECTED_CMD) {
			wifi_set_free_communicate();
		} else if (g_wifiState == WIFI_BEFOR_TCP) {
			if (wifi_building_tcpip() == 0) {
				return;
			}
			if (wifi_set_free_communicate() == 0) {
				return;
			}
		} else if (g_wifiState == WIFI_NOT_CONNECT){
			connect_wifi();
		}
        if (g_wifiState == WIFI_CONNECTED_FREE) {
           handshake_mes_system(); 
        }
	}
}

void StartWifiTask(void const * argument)
{
    wifi_usart_init();
    while (1) {
        osDelay(5);
        wifi_heart();
    }
}