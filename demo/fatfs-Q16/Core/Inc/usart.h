/*
 * @Author: lchunqi 847597316@qq.com
 * @Date: 2025-10-28 11:02:06
 * @LastEditors: lchunqi 847597316@qq.com
 * @LastEditTime: 2025-11-08 16:23:35
 * @FilePath: \dma-doubao\Core\Inc\usart.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */

HAL_StatusTypeDef USART1_DMA_StartRx(void);
HAL_StatusTypeDef USART1_DMA_SendData(uint8_t *pData, uint16_t Length);

#define USART1_DMA_RX_BUFF_SIZE  256  // DMA 接收缓冲区大小（需大于最大单帧数据长度）
#define USART1_DMA_TX_BUFF_SIZE  256  // DMA 发送缓冲区大小

// 2. 外部变量声明（对应 usart.c 中的全局变量）
extern uint8_t g_usart1_dma_rx_buff[USART1_DMA_RX_BUFF_SIZE];    // DMA 接收缓冲区
extern uint8_t g_usart1_dma_tx_buff[USART1_DMA_TX_BUFF_SIZE];
extern volatile uint16_t g_usart1_rx_len; // 一帧数据长度
extern volatile uint8_t g_usart1_rx_done; // 接收完成标志


/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

