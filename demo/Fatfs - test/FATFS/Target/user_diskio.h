
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * @file    user_diskio.h
  * @brief   This file contains the common defines and functions prototypes for
  *          the user_diskio driver.
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
#ifndef __USER_DISKIO_H
#define __USER_DISKIO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* USER CODE BEGIN 0 */

/* Private define ------------------------------------------------------------*/
#define DEV_FLASH            0       // 物理设备号：唯一SPI Flash设备（W25Q64）

/* W25Q64扇区/容量宏定义（必须与flash.c底层驱动一致，不可随意修改） */
#define W25Q64_LOGIC_SECTOR_SIZE     4096U    // FatFs逻辑扇区大小：4096字节（与物理扇区一致）
#define W25Q64_PHYSICAL_SECTOR_SIZE  4096U    // W25Q64硬件物理扇区大小：4096字节（最小擦除单位）
#define W25Q64_TOTAL_LOGIC_SECTOR    2048U    // 总逻辑扇区数：8MB Flash / 4096字节 = 2048（修正原注释错误）


DSTATUS USER_initialize (BYTE pdrv);   // 设备初始化函数
DSTATUS USER_status (BYTE pdrv);       // 设备状态检查函数
DRESULT USER_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count); // 扇区读取函数
DRESULT USER_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count); // 扇区写入函数（_USE_WRITE=1时使能）
DRESULT USER_ioctl (BYTE pdrv, BYTE cmd, void *buff); // 设备控制函数（_USE_IOCTL=1时使能，格式化必需）



/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern Diskio_drvTypeDef  USER_Driver;

/* USER CODE END 0 */

#ifdef __cplusplus
}
#endif

#endif /* __USER_DISKIO_H */
