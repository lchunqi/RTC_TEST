
#ifndef __TOOL_H
#define __TOOL_H

#include "main.h"

// 模拟串口引脚配置
#define SOFT_UART_TX_Pin GPIO_PIN_13
#define SOFT_UART_TX_GPIO_Port GPIOB
#define SOFT_UART_RX_Pin GPIO_PIN_14
#define SOFT_UART_RX_GPIO_Port GPIOB

// 波特率配置（默认9600，可改115200）
#define BAUD_RATE        9600UL
#define BAUD_DELAY_US    (1000000UL / BAUD_RATE )

// 函数声明
void Soft_UART_Init(void);
void Soft_UART_SendByte(uint8_t data);
void Soft_UART_SendStr(uint8_t *str);
uint8_t Soft_UART_ReceiveReady(void);
uint8_t Soft_UART_ReceiveByte(uint32_t timeout);




#endif