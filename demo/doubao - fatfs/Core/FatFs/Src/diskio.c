
#include "diskio.h"
#include "flash.h"
#include "string.h"
#include <stdio.h>
#include <stdarg.h>
#include "usart.h"

// 仅支持1个FLASH设备
#define DEV_COUNT 1

// 串口打印模块（按需开启，和storage.c打印逻辑一致）
#ifdef ENABLE_DISKIO_PRINT
void Diskio_Print(const char *fmt,...) {
    va_list args;
    char buffer[256];
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), 100);
}
#else
#define Diskio_Print(...)
#endif

// 全局磁盘状态
static DSTATUS Stat = STA_NOINIT;

/************************** 磁盘初始化 **************************/
DSTATUS disk_initialize(BYTE pdrv) {
    if (pdrv >= DEV_COUNT) {
        Diskio_Print("磁盘初始化: 无效驱动器号 %d\n", pdrv);
        return STA_NOINIT;
    }

    // 初始化前等待FLASH忙状态结束
    while (W25Q64_ReadStatus() & 0x01);
    // 初始化FLASH硬件
    W25Q64_Init();
    // 读取FLASH ID校验硬件连接
    uint16_t flash_id = W25Q64_ReadID();
    Diskio_Print("磁盘初始化: 读取FLASH ID = 0x%04X\n", flash_id);

    // 校验ID（沿用diskio.h的宏，兼容W25Q64ID/ W25Q64_EXPECTED_ID）
    if (flash_id == W25Q64_EXPECTED_ID ) {
        Stat &= ~STA_NOINIT; // 清除未初始化标志
        Diskio_Print("磁盘初始化: FLASH初始化成功\n");
    } else {
        Stat |= STA_NOINIT;
        Diskio_Print("磁盘初始化: FLASH ID不匹配，初始化失败\n");
    }
    return Stat;
}

/************************** 获取磁盘状态 **************************/
DSTATUS disk_status(BYTE pdrv) {
    if (pdrv >= DEV_COUNT) {
        return STA_NOINIT;
    }
    return Stat;
}

/************************** 磁盘扇区读取（FatFs标准接口） **************************/
DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count) {
    if (pdrv >= DEV_COUNT || buff == NULL || count == 0 || (Stat & STA_NOINIT)) {
        Diskio_Print("磁盘读取: 无效参数或未初始化\n");
        return RES_PARERR;
    }
    while (W25Q64_ReadStatus() & 0x01); // 读前等忙

    uint32_t read_addr = sector * 4096;
    uint32_t read_size = count * 4096;
    Diskio_Print("磁盘读取: 扇区%lu → 地址0x%08X，读取%lu字节\n", sector, read_addr, read_size);

    // 调用FLASH读函数，增加返回值校验
    W25Q64_ReadData(read_addr, buff, read_size);
    // 验证读取后的数据是否为全0（若全0可能是读失败）
    uint8_t all_zero = 1;
    for (uint32_t i=0; i<read_size && i<32; i++) { // 检查前32字节
        if (buff[i] != 0x00) {
            all_zero = 0;
            break;
        }
    }
    if (all_zero) {
        Diskio_Print("磁盘读取: 警告！读取数据全为0，可能读操作失败\n");
        return RES_ERROR; // 返回I/O错误，让FatFs识别
    }
    return RES_OK;
}

/************************** 磁盘扇区写入（FatFs标准接口） **************************/
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) {
    // 转发到自定义字节级写入函数，适配原有storage.c逻辑
    if (pdrv >= DEV_COUNT || buff == NULL || count == 0 || (Stat & STA_NOINIT)) {
        Diskio_Print("磁盘写入: 无效参数或设备未初始化\n");
        return RES_PARERR;
    }
    // FatFs扇区→FLASH字节地址
    return disk_write_any(pdrv, buff, sector * 512, count * 512);
}

/************************** 自定义字节级写入（兼容原有逻辑） **************************/
DRESULT disk_write_any(BYTE pdrv, const BYTE *buff, DWORD start_addr, UINT size) {
    if (pdrv >= DEV_COUNT || buff == NULL || size == 0 || (Stat & STA_NOINIT)) {
        Diskio_Print("字节写入: 无效参数或未初始化\n");
        return RES_PARERR;
    }
    while (W25Q64_ReadStatus() & 0x01);

    uint32_t curr_addr = start_addr;
    uint32_t remain_size = size;
    const uint8_t *data_ptr = buff;
    Diskio_Print("字节写入: 起始地址0x%08X，总大小%u字节\n", start_addr, size);

    // 擦除扇区（保留原有逻辑，增加错误校验）
    uint32_t start_sector = curr_addr / W25Q64_SECTOR_SIZE;
    uint32_t end_sector = (curr_addr + remain_size - 1) / W25Q64_SECTOR_SIZE;
    for (uint32_t i = start_sector; i <= end_sector; i++) {
        W25Q64_EraseSector(i * W25Q64_SECTOR_SIZE);
        while (W25Q64_ReadStatus() & 0x01);
        // 擦除后验证扇区是否为0xFF（正常擦除后应为全1）
        uint8_t verify_buf[16] = {0};
        W25Q64_ReadData(i * W25Q64_SECTOR_SIZE, verify_buf, 16);
        uint8_t erase_ok = 1;
        for (int j=0; j<16; j++) {
            if (verify_buf[j] != 0xFF) {
                erase_ok = 0;
                break;
            }
        }
        if (!erase_ok) {
            Diskio_Print("字节写入: 扇区%lu擦除失败！\n", i);
            return RES_ERROR;
        }
    }

    // 分页写入（增加写后读回验证）
    while (remain_size > 0) {
        uint16_t page_offset = curr_addr % W25Q64_PAGE_SIZE;
        uint16_t write_len = (W25Q64_PAGE_SIZE - page_offset) > remain_size ? remain_size : (W25Q64_PAGE_SIZE - page_offset);

        W25Q64_WritePage(curr_addr, (uint8_t *)data_ptr, write_len);
        while (W25Q64_ReadStatus() & 0x01);

        // 写后立即读回验证
        uint8_t verify_buf[256] = {0};
        W25Q64_ReadData(curr_addr, verify_buf, write_len);
        if (memcmp(verify_buf, data_ptr, write_len) != 0) {
            Diskio_Print("字节写入: 地址0x%08X写入失败（验证不通过）\n", curr_addr);
            return RES_ERROR;
        }

        curr_addr += write_len;
        data_ptr += write_len;
        remain_size -= write_len;
    }

    Diskio_Print("字节写入: 地址0x%08X，成功写入%u字节\n", start_addr, size);
    return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
    if (pdrv >= DEV_COUNT || (Stat & STA_NOINIT)) {
        Diskio_Print("IO控制: 无效设备或未初始化\n");
        return RES_PARERR;
    }
    DRESULT res = RES_PARERR;

    switch (cmd) {
        case CTRL_SYNC: // diskio.h定义：0 → 强制刷新
            while (W25Q64_ReadStatus() & 0x01);
            res = RES_OK;
            Diskio_Print("IO控制: 执行CTRL_SYNC，FLASH已就绪\n");
            break;
        // 其他指令保留原有逻辑，增加参数校验
        case GET_SECTOR_COUNT:
            if (buff != NULL) {
                *(DWORD *)buff = W25Q64_TOTAL_LOGIC_SECTOR;
                res = RES_OK;
                Diskio_Print("IO控制: 总扇区数 = %lu\n", *(DWORD *)buff);
            }
            break;
        case GET_SECTOR_SIZE:
            if (buff != NULL) {
                *(WORD *)buff = W25Q64_PHYSICAL_SECTOR_SIZE;
                res = RES_OK;
                Diskio_Print("IO控制: 扇区大小 = %u字节\n", *(WORD *)buff);
            }
            break;
        case GET_BLOCK_SIZE:
            if (buff != NULL) {
                *(DWORD *)buff = 1;
                res = RES_OK;
                Diskio_Print("IO控制: 擦除块大小 = %lu字节\n", *(DWORD *)buff);
            }
            break;
        default:
            Diskio_Print("IO控制: 未识别命令 0x%02X\n", cmd);
            res = RES_PARERR;
            break;
    }
    return res;
}
