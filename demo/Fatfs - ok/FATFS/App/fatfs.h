/**
  ******************************************************************************
  * @file   fatfs.h
  * @brief  Header for fatfs applications
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __fatfs_H
#define __fatfs_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "ff.h"
#include "ff_gen_drv.h"
#include "user_diskio.h" /* defines USER_Driver as external */

/* USER CODE BEGIN Includes */
#include "storage.h"
/* USER CODE END Includes */

extern uint8_t retUSER; /* Return value for USER */
extern char USERPath[4]; /* USER logical drive path */
extern FATFS USERFatFS; /* File system object for USER logical drive */
extern FIL USERFile; /* File object for USER */

void MX_FATFS_Init(void);

/* USER CODE BEGIN Prototypes */

// 1. 封装时间结构体
typedef struct
{
    uint16_t year;    // 年（1980~2107，必填）
    uint8_t  month;   // 月（1~12，必填）
    uint8_t  day;     // 日（1~31，必填，适配闰月）
    uint8_t  hour;    // 时（0~23，可选，默认0）
    uint8_t  minute;  // 分（0~59，可选，默认0）
    uint8_t  second;  // 秒（0~59，可选，默认0）
} FatFs_Time_t;

extern FatFs_Time_t g_fatfs_time;


FRESULT FatFs_Mount(void);          // 挂载文件系统
FRESULT FatFs_Format(void);        // 格式化文件系统
void FatFs_Unmount(void);          // 卸载文件系统
void FatFs_ListDir(void);


/* USER CODE END Prototypes */
#ifdef __cplusplus
}
#endif
#endif /*__fatfs_H */
