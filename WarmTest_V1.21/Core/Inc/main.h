/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

typedef struct 
{
  uint8_t year;
  uint8_t mon;
  uint8_t day;
  uint8_t week;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint16_t tick;
}RealTime;

//3031 3077 3078 3178 3025 3900 ->64
#define RTC_3077_ADDRESS    0x64
#define RTC_3178_ADDRESS    0x64
#define RTC_3900_ADDRESS    0x64
#define RTC_8800_ADDRESS    0x64
#define RTC_8801_ADDRESS    0x64
#define RTC_8804_ADDRESS    0x64
#define RTC_8810_ADDRESS    0x64
#define RTC_8825_ADDRESS    0x64
#define RTC_8931_ADDRESS    0xD0
#define RTC_8840_ADDRESS    0x00

#define RTC_3077_REG    0x00
#define RTC_3178_REG    0x00
#define RTC_3900_REG    0x00
#define RTC_8800_REG    0x00
#define RTC_8801_REG    0x00
#define RTC_8810_REG    0x10
#define RTC_8825_REG    0x00
#define RTC_8931_REG    0x00
#define RTC_8840_REG    0x00

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

void UsbPrintf(char* fmt,...) ;
void DevicePrintf(char* fmt,...);

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
