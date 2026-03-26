
#ifndef FLASH_H
#define FLASH_H

#include "stm32f1xx_hal.h"

// 硬件配置
#define W25Q64_CS_GPIO_PORT      GPIOA
#define W25Q64_CS_PIN            GPIO_PIN_4
#define W25Q64_SPI_HANDLE        hspi1

#define W25Q64_CS_LOW()  HAL_GPIO_WritePin(W25Q64_CS_GPIO_PORT, W25Q64_CS_PIN, GPIO_PIN_RESET)
#define W25Q64_CS_HIGH() HAL_GPIO_WritePin(W25Q64_CS_GPIO_PORT, W25Q64_CS_PIN, GPIO_PIN_SET)

// 硬件参数宏定义（与flash.c一致）
#define W25Q64_PAGE_SIZE     256
#define W25Q64_SECTOR_SIZE   4096
#define W25Q64_TOTAL_SIZE    8*1024*1024
#define W25Q64_EXPECTED_ID   0xEF17
#define W25Q64_WRITE_ENABLE_CMD  0x06
#define W25Q64_WRITE_DISABLE_CMD 0x04

// 打印控制宏
#ifdef ENABLE_FLASH_PRINT
void Flash_Print(const char *fmt,...);
#endif

// 函数声明
void W25Q64_Init(void);
uint16_t W25Q64_ReadID(void);
void W25Q64_ReadData(uint32_t addr, uint8_t *data, uint32_t len);
void W25Q64_WritePage(uint32_t addr, const uint8_t *data, uint16_t len);
void W25Q64_EraseSector(uint32_t addr);
uint8_t W25Q64_ReadStatus(void);
void W25Q64_ChipErase(void);
HAL_StatusTypeDef W25Q64_WriteEnable(void);  // 写使能函数声明
HAL_StatusTypeDef W25Q64_WriteDisable(void); // 写失能函数声明

// 状态标识宏（上层可用于判断）
#define W25Q64_STATUS_HW_PROTECT  0x02 // 硬件写保护使能
#define W25Q64_STATUS_SW_DISABLE  0x04 // 软件写失能使能
#define W25Q64_STATUS_SPI_ERROR   0xFF // SPI通信失败

uint8_t W25Q64_ReadWriteTest(uint32_t test_addr);





#endif