#include "control.h"
#include "main.h"
#include "string.h"
#include <stdio.h>
#include <stdarg.h>
#include "user_tool.h"
#include "lcd_driver.h"

#define CONTROL_RECEIVE_BUFF_NUMBER    200
#define CONTROL_TRANSMIT_BUFF_NUMBER   300
#define CTRL_UART_TICK_CLEAR         50//50ms

extern UART_HandleTypeDef huart2;
static uint16_t control_Rx_Cnt = 0;		//
static uint8_t controlRxBuf;
uint8_t g_controlRxBuff[CONTROL_RECEIVE_BUFF_NUMBER] = {0};
uint8_t g_controlTxBuff[CONTROL_TRANSMIT_BUFF_NUMBER] = {0};
uint8_t g_controlATStatus = 0;

#define MAX_CONTROL_NUMBER  15
#define MAX_CONTROL_LENGTH  20
static char CtrlStringList[MAX_CONTROL_NUMBER][MAX_CONTROL_LENGTH] = {
    "AT^CONNECT?",
};

void ControlPrintf(char* fmt,...)  
{  
	va_list ap;//
	memset(g_controlTxBuff, 0, CONTROL_TRANSMIT_BUFF_NUMBER);
	va_start(ap, fmt);//
	vsnprintf((char*)g_controlTxBuff, CONTROL_TRANSMIT_BUFF_NUMBER, fmt, ap);//
	va_end(ap);
	HAL_UART_Transmit(&huart2, g_controlTxBuff, strlen((char*)g_controlTxBuff), 1000);
}


void control_usart_init(void)
{
	HAL_UART_Receive_IT(&huart2, &controlRxBuf, 1);		
    memset(g_controlRxBuff, 0x00, sizeof(g_controlRxBuff));
	memset(g_controlTxBuff, 0x00, sizeof(g_controlTxBuff));
    ControlPrintf("LCD uart init success...\n");
    SetScreen(DEFAULT_SCREEN_ID);
}

void CTL_CONNECT_Handle(char* command)
{
    ControlPrintf("OK\n");
}

typedef void(*function)(char* s);
function ControlCommand[MAX_CONTROL_NUMBER] = {
    CTL_CONNECT_Handle,
};

void control_usart_receive(void)//接收中断回调函数
{
    if (control_Rx_Cnt == 0 && (controlRxBuf == ' ' || controlRxBuf == '\r' || controlRxBuf == '\n')) {
        HAL_UART_Receive_IT(&huart2, &controlRxBuf, 1);
        return;
    }
    if (control_Rx_Cnt < CONTROL_RECEIVE_BUFF_NUMBER && g_controlATStatus == 0) {
        g_controlRxBuff[control_Rx_Cnt] = controlRxBuf;
        control_Rx_Cnt++;
    }
    if (control_Rx_Cnt >= CONTROL_RECEIVE_BUFF_NUMBER || controlRxBuf == ' ') {
        g_controlATStatus = 1;
    } 
    HAL_UART_Receive_IT(&huart2, &controlRxBuf, 1);
}

void CheckControlList(void)
{
    uint8_t i;
    static uint16_t controlTick = 0;
    static uint16_t lastControlRx = 0;
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
