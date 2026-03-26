
#ifndef __USART_H__
#define __USART_H__

#include "main.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

// ========== USART2 环形缓冲区 ==========
#define UART2_RX_BUF_SIZE    256

typedef struct {
    uint8_t buffer[UART2_RX_BUF_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
} UART_RingBufTypeDef;

extern UART_RingBufTypeDef uart2_rx_ringbuf;

// 读一个字节（阻塞/非阻塞自己选）
uint8_t UART2_ReadByte(uint8_t *byte);
// 读一行（返回长度）
uint16_t UART2_ReadLine(uint8_t *buf, uint16_t max_len);

#endif