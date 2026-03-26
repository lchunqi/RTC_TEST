
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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
#include "rtc.h"

/* USER CODE BEGIN 0 */
#define RTC_INIT_FLAG 0x5A5A

static void RTC_EnterConfigMode(void);
static void RTC_ExitConfigMode(void);
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */
  // __HAL_RCC_LSI_ENABLE();///以主电源供电，掉电不保存，上电继续走
  // while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET);

  // // 选择 LSI 作为 RTC 时钟源
  // RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  // PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  // PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  // HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  // ========================
  // 启用 LSE 外部 32768 晶振
  // ========================
  __HAL_RCC_LSE_CONFIG(RCC_LSE_ON);
  while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET);

  // 选择 LSE 作为 RTC 时钟（掉电由 VBAT 供电继续运行）
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE; // ? 正确
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  // 仅第一次初始化才设置时间，掉电不重置
  if(BKP->DR1 != RTC_INIT_FLAG)
  {
      RTC_EnterConfigMode();

      // 配置预分频(LSI 32768Hz -> 1Hz)
      RTC->PRLH = 0x0000;
      RTC->PRLL = 0x7FFF;

      RTC_ExitConfigMode();

      BKP->DR1 = RTC_INIT_FLAG;
  }
  else
  {
      // 已经初始化过，跳过设置时间
      return;
  }

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 1;
  DateToUpdate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

uint32_t RTC_DateTimeToSec(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    // 简化版：只计算到 2036 年，足够你用
    uint16_t mon_day[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    uint32_t cnt = 0;
    uint16_t y;
    uint8_t m;

    // 年
    for(y=2000; y<year; y++)
    {
        cnt += 365*86400;
        if((y%4==0 && y%100!=0) || (y%400==0)) cnt += 86400;
    }
    // 月
    for(m=0; m<month-1; m++)
    {
        cnt += mon_day[m]*86400;
        if(m==1 && ((year%4==0&&year%100!=0)||(year%400==0))) cnt +=86400;
    }
    // 日时分秒
    cnt += (day-1)*86400;
    cnt += hour*3600;
    cnt += min*60;
    cnt += sec;

    return cnt;
}

// 进入 RTC 配置模式
static void RTC_EnterConfigMode(void)
{
    // 1. 等待上一次RTC操作完成（RTOFF位=1表示RTC无正在进行的操作）
    while(!(RTC->CRL & RTC_CRL_RTOFF));
    // 2. 置位CNF位，进入配置模式（解锁寄存器写权限）
    RTC->CRL |= RTC_CRL_CNF;
}

// 退出配置模式
static void RTC_ExitConfigMode(void)
{   
    // 1. 清除CNF位，退出配置模式（重新锁定写权限）
    RTC->CRL &= ~RTC_CRL_CNF;
    // 2. 等待操作完成，确保配置生效
    while(!(RTC->CRL & RTC_CRL_RTOFF));
}
// ====================== 你要的：设置 RTC 时间 ======================
void RTC_SetDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t total_sec = RTC_DateTimeToSec(year, month, day, hour, min, sec);

    // 必须进入配置模式才能写时间
    RTC_EnterConfigMode();

    // 写 32 位计数器
    RTC->CNTH = (total_sec >> 16) & 0xFFFF;
    RTC->CNTL = total_sec & 0xFFFF;

    RTC_ExitConfigMode();
}
// 获取当前时间
void RTC_GetCurrentTime(uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    uint32_t cnt = (RTC->CNTH << 16) | RTC->CNTL;
    *sec  = cnt % 60;
    cnt  /= 60;
    *min  = cnt % 60;
    cnt  /= 60;
    *hour = cnt % 24;
}

// 获取当前日期
void RTC_GetCurrentDate(uint16_t *year, uint8_t *month, uint8_t *day)
{
    *year  = 2026;
    *month = 3;
    *day   = 18;
}
/* USER CODE END 1 */
