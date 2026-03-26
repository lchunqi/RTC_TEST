#ifndef FLASH_H
#define FLASH_H

#include "main.h"

// W25Q64 基础宏定义 

#define W25Q64_DEVICE_ID      0xEF4017U
#define W25Q64_SECTOR_SIZE    4096U    // 关键：与FatFs逻辑扇区大小一致
#define W25Q64_PAGE_SIZE      256U     // W25Q64硬件页大小（固定256字节）
#define W25Q64_MAX_ADDR       0x7FFFFFU // 8MB Flash最大地址（0x000000~0x7FFFFF）

// SPI片选引脚宏（根据实际硬件修改，此处为PA4）
#define CS_LOW()    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define CS_HIGH()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)


/************************ 基础硬件操作函数（需确保已实现） .h  ************************/

uint8_t  W25Q64_ReadStatus(void);
uint8_t  W25Q64_WaitBusy(void);
uint8_t  W25Q64_WriteEnable(void);
uint8_t  W25Q64_WriteDisable(void);
uint32_t W25Q64_ReadDeviceID(void);
uint8_t  W25Q64_EraseSector(uint32_t addr);
uint8_t  W25Q64_PageWrite(uint32_t addr,const uint8_t *data, uint16_t len);
uint8_t  W25Q64_ReadData(uint32_t addr, uint8_t *data, uint32_t len);
uint8_t  W25Q64_ChipErase(void);
// W25Q64 测试函数声明
uint8_t  W25Q64_SelfTest(void);
uint8_t  W25Q64_SingleByteTest(uint32_t test_addr);
uint8_t  W25Q64_PageWriteTest(uint32_t test_addr);
uint8_t  W25Q64_SectorWriteTest(uint32_t test_addr);
uint8_t  W25Q64_FullTest(void);




#endif