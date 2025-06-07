/*
 * @Author: Liang 
 * @Date: 2025-03-25 11:36:56
 * @LastEditors: HA HA 
 * @LastEditTime: 2025-03-31 09:33:46
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved.
 */
#include "esp8266.h"

//PG13  CHIP_EN   高电平有效
//PG14  RST       低电平有效
void ESP8266_GPIO_Config ( void )
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pins : PB0 PB1 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  
  /*Configure GPIO pin Output Level */
  // HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
  // HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
  macESP8266_RST_HIGH_LEVEL();
	macESP8266_CH_DISABLE();

}

void wifi_send_to(char * cmd)
{
  HAL_UART_Transmit(&huart3, (uint8_t *)cmd,strlen(cmd),0xFFFF); 
  while(HAL_UART_GetState(&huart3) == HAL_UART_STATE_BUSY_TX);//检测UART发送结束
}
  






