/*
 * @Author: lchunqi 847597316@qq.com
 * @Date: 2026-03-09 17:40:54
 * @LastEditors: lchunqi 847597316@qq.com
 * @LastEditTime: 2026-03-19 11:11:56
 * @FilePath: \Fatfs - test\Core\Src\usart.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_RingBufTypeDef uart2_rx_ringbuf={0};
/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  
  /* USER CODE END USART1_Init 2 */

}

void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_UART_Receive_IT(&huart2, &uart2_rx_ringbuf.buffer[uart2_rx_ringbuf.head], 1);
}

// ==============================
// USART2 环形缓冲区 读字节
// ==============================
uint8_t UART2_ReadByte(uint8_t *byte)
{
    if (byte == NULL) return 0;

    if (uart2_rx_ringbuf.head == uart2_rx_ringbuf.tail)
        return 0;

    *byte = uart2_rx_ringbuf.buffer[uart2_rx_ringbuf.tail];
    uart2_rx_ringbuf.tail = (uart2_rx_ringbuf.tail + 1) % UART2_RX_BUF_SIZE;
    return 1;
}

// ==============================
// USART2 读一行（遇到 \n 停止）
// ==============================
// 修复版：等待整行接收（直到换行符/超时，不再分段）
uint16_t UART2_ReadLine(uint8_t *buf, uint16_t max_len)
{
    uint16_t len = 0;
    uint8_t ch;
    uint32_t timeout = 0;
    uint8_t has_valid_data = 0; // 标记是否有有效数据（非空行）

    memset(buf, 0, max_len);

    while (len < max_len - 1)
    {
        if (UART2_ReadByte(&ch))
        {
            timeout = 0;

            // 跳过开头的\r\n（避免空行）
            if (len == 0 && (ch == '\r' || ch == '\n'))
            {
                continue;
            }

            // 遇到\r\n且已有有效数据 → 结束
            if ((ch == '\r' || ch == '\n') && has_valid_data)
            {
                break;
            }

            // 有效数据存入缓冲区
            if (ch != '\r' && ch != '\n')
            {
                buf[len++] = ch;
                has_valid_data = 1; // 标记有有效数据
            }
        }
        else
        {
            timeout++;
            HAL_Delay(1);
            if (timeout > 200)
            {
                break;
            }
        }
    }

    buf[len] = '\0';
    // 仅返回有有效数据的长度（空行返回0）
    return has_valid_data ? len : 0;
}

// 读取多个字节（返回实际读取的长度）
uint16_t UART2_RingBuf_Read(uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0) return 0;
    
    uint16_t read_len = 0;
    while (read_len < len && uart2_rx_ringbuf.head != uart2_rx_ringbuf.tail)
    {
        buf[read_len++] = uart2_rx_ringbuf.buffer[uart2_rx_ringbuf.tail];
        uart2_rx_ringbuf.tail = (uart2_rx_ringbuf.tail + 1) % UART2_RX_BUF_SIZE;
    }
    
    return read_len;
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE END USART1_MspInit 1 */
  }else if(uartHandle->Instance == USART2)
  {
    // 开时钟
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // USART2 对应引脚
    // PA2 → TX
    // PA3 → RX
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 开中断（可选）
    HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
}



/* USER CODE BEGIN 1 */
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
  if(uartHandle->Instance == USART1)
  {
    __HAL_RCC_USART1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  }
  else if(uartHandle->Instance == USART2)
  {
    __HAL_RCC_USART2_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  }
}
/* USER CODE END 1 */
