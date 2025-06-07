#include "control.h"
#include "main.h"
#include "string.h"
#include <stdio.h>
#include <stdarg.h>
#include "user_tool.h"
#include "lcd_driver.h"
#include "devices.h"

#define CONTROL_RECEIVE_BUFF_NUMBER    1000
#define CONTROL_TRANSMIT_BUFF_NUMBER   300
#define CTRL_UART_TICK_CLEAR         50//50ms
#define MAX_MESSAGE_BUFFER              10
#define LCD_COMMAND_LENGTH              100
#define CMD_HEAD                        0xEE
#define CMD_TAIL                        0xFFFCFFFF

extern UART_HandleTypeDef huart2;
static uint16_t g_rxCount = 0;		//
static uint8_t controlRxBuf;
uint8_t g_controlRxBuff[CONTROL_RECEIVE_BUFF_NUMBER] = {0};
uint8_t g_controlTxBuff[CONTROL_TRANSMIT_BUFF_NUMBER] = {0};
uint8_t g_cmdNumber = 0;
uint16_t g_lastRead = 0;
extern uint8_t g_screenId;
extern uint16_t g_inputNumber;
uint8_t g_commandList[MAX_MESSAGE_BUFFER][LCD_COMMAND_LENGTH] = {0};

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


//可能情况：
//1、一次接收了多条指令；
//2、一条指令还没接收完全；
//3、指令开始或结束符不正确；
uint8_t lcd_find_command(void)
{
    int i, j, k;
    uint32_t buffer = 0;
    uint8_t isCmd = 0;
    uint8_t cmd_number = 0;
    if (g_rxCount < 6) { return cmd_number;  }
    for (i = g_lastRead; i < g_rxCount; i++) {
        if (g_controlRxBuff[i] != CMD_HEAD) {
            continue;
        }
        k = 0;
        isCmd = 0;
        for (j = i; j < g_rxCount; j++) {
            buffer = (buffer << 8) | g_controlRxBuff[j];
            g_commandList[g_cmdNumber][k] = g_controlRxBuff[j];
            k++;
            if (buffer == CMD_TAIL) {
                isCmd = 1;
                cmd_number++;
                g_cmdNumber++;
                i = j;
                break;
            }
        }
        
        if (isCmd > 0 && (j + 1) >= g_rxCount) {//正常指令结束的，清空接收缓冲区
            memset(g_controlRxBuff, 0, CONTROL_RECEIVE_BUFF_NUMBER);
            g_rxCount = 0;
            g_lastRead = 0;
            break;
        }
    }
    return cmd_number;
}

#define KEYBOARD_ID             3  
#define BUTTON_SEND_ID          5
#define BUTTON_RETURN_ID        6
void handle_one_message(uint16_t screen_id, uint16_t moudle_id, uint8_t state, char* text)
{
    int number = 0;
    switch (screen_id) {
        case 0:
        break;
        case SCREEN_INPUT_NUMBER:
            switch (moudle_id) {
                case BUTTON_SEND_ID:
                    set_chipid_devices(g_inputNumber);
                break;
                case BUTTON_RETURN_ID:
                    g_screenId = SCREEN_MASSAGE_SHOW;
                    SetScreen(g_screenId);
                break;
                case KEYBOARD_ID:
                    CharToInt(text, 0, &number);
                    g_inputNumber = number;
                    show_chip_id();
                break;
                default: break;
            }
        break;
        default:  break;
    }
}

void get_one_message(uint8_t* command)
{
    uint16_t event;
    uint16_t screen_id;
    uint16_t moudle_id;
    uint8_t state = 0;
    char text[20] = {0};

    uint32_t buffer = 0;
    int l, k;
    for (l = 0; l < LCD_COMMAND_LENGTH; l++) {
        buffer = ((buffer << 8) | command[l]);
        if (buffer == CMD_TAIL) {
            break;
        }
    }
    if (command[0] == CMD_HEAD && l < LCD_COMMAND_LENGTH) {
        event = (command[1] << 8) + command[2];
        if (event == 0xB111) {
            screen_id = (command[3] << 8) + command[4];
            moudle_id = (command[5] << 8) + command[6];
            if (command[7] == 0x10) {
                state = command[l - 4];
            } else if (command[7] == 0x11) {
                state = 2;
                for (k = 0; k + 10 < l; k++) {
                    text[k] = command[7 + k];
                }
            }
            handle_one_message(screen_id, moudle_id, state, text);
            // for (k = 0; k <= l; k++) {  UsbPrintf("%02X ", command[k]);   }
        }
    }
}

void control_usart_receive(void)//接收中断回调函数
{
    if (g_rxCount < CONTROL_RECEIVE_BUFF_NUMBER) {
        g_controlRxBuff[g_rxCount] = controlRxBuf;
        g_rxCount++;
    }
    HAL_UART_Receive_IT(&huart2, &controlRxBuf, 1);
}

void CheckControlList(void)
{
    int i;
    if (lcd_find_command()) {
        for (i = 0; i < g_cmdNumber; i++) {
            get_one_message(g_commandList[i]);
            memset(g_commandList[i], 0, LCD_COMMAND_LENGTH);
        }
        g_cmdNumber = 0;
    }
}
