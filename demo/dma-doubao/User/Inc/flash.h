/*
 * @Author: lchunqi 847597316@qq.com
 * @Date: 2026-02-27 11:22:36
 * @LastEditors: lchunqi 847597316@qq.com
 * @LastEditTime: 2026-03-02 18:46:54
 * @FilePath: \dma-doubao\User\Inc\flash.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __FLASH_H
#define __FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <string.h>


// FATFS/diskio 适配核心宏定义（W25Q64硬件参数+diskio逻辑扇区）

#define W25Q_PAGE_SIZE         256U    // W25Q64硬件页大小（256字节，单次编程最大）
#define W25Q_PAGE_MASK         (W25Q_PAGE_SIZE - 1) // 256字节对齐掩码（0xFF）
#define W25Q_SECTOR_SIZE       4096U   // W25Q64硬件扇区大小（4KB擦除粒度）
#define W25Q_FLASH_END_ADDR    0x7FFFFFU// W25Q64总容量8MB（0x000000~0x7FFFFF
// W25Q64 核心指令集
#define W25Q_CMD_JEDEC_ID   0x9F    // 读JEDEC ID（厂家+类型+容量）
#define W25Q_CMD_READ       0x03    // 普通读
#define W25Q_CMD_WRITE_EN   0x06    // 写使能
#define W25Q_CMD_SECTOR_ER  0x20    // 扇区擦除（4KB）
#define W25Q_CMD_PAGE_PROG  0x02    // 页编程
#define W25Q_CMD_READ_STAT  0x05    // 读状态寄存器

#define W25Q_WriteEnable        0x06
#define W25Q_WriteDisable       0x04
#define W25Q_ReadStatusReg1     0x05U
#define W25Q_ReadID             0x9F
#define W25Q_EraseSector        0x20  // 4KB扇区擦除（核心）
#define W25Q_PageProgram        0x02  // 256字节页编程（硬件自增核心）
#define W25Q_ReadData           0x03
// SPI片选宏定义（需在flash.h中定义FLASH_CS_GPIO_Port/FLASH_CS_Pin）
// CS引脚宏定义（默认GPIOA_PIN_4，按你的硬件修改）
#define CS_GPIO_PORT      GPIOA
#define CS_GPIO_PIN       GPIO_PIN_4
#define CS_HIGH           HAL_GPIO_WritePin(CS_GPIO_PORT, CS_GPIO_PIN, GPIO_PIN_SET)
#define CS_LOW            HAL_GPIO_WritePin(CS_GPIO_PORT, CS_GPIO_PIN, GPIO_PIN_RESET)

// W25Q64 硬件参数（8MB=128Mbit）
#define FLASH_TOTAL_SIZE  0x800000// 总容量8MB

uint8_t  CheckBusy(void);
uint32_t flash_device_id(void);
uint8_t  flash_erase_sector(uint32_t addr);
uint8_t  flash_read_data(uint32_t addr, uint8_t* buf, uint32_t len);
uint8_t  flash_write_data(uint32_t addr, uint8_t* buf, uint32_t len);




#ifdef __cplusplus
}
#endif

#endif 