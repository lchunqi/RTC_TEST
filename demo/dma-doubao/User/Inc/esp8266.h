/*
 * @Author: Liang 
 * @Date: 2025-03-25 11:36:35
 * @LastEditors: HA HA 
 * @LastEditTime: 2025-03-28 16:17:34
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved.
 */
#ifndef __ESP8266_H
#define __ESP8266_H

#ifdef __cplusplus
 extern "C" {
#endif
#include "main.h"
#include "usart.h"

#define  macESP8266_CH_PD_PORT         GPIOG
#define  macESP8266_CH_PD_PIN          GPIO_PIN_13
#define  macESP8266_RST_PORT           GPIOG
#define  macESP8266_RST_PIN            GPIO_PIN_14
#define  macESP8266_CH_ENABLE()        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET)
#define  macESP8266_CH_DISABLE()       HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET)
#define  macESP8266_RST_HIGH_LEVEL()   HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET)
#define  macESP8266_RST_LOW_LEVEL()    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET)






void ESP8266_GPIO_Config ( void );
void wifi_send_to(char * cmd);





#ifdef __cplusplus
}
#endif

#endif 