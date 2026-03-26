/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Basic definitions of FatFs */
#include "diskio.h"		/* Declarations FatFs MAI */
#include "spi.h"
#include "flash.h"
/* Example: Declarations of the platform and disk functions in the project */
//#include "platform.h"
//#include "storage.h"

/* Example: Mapping of physical drive number for each drive */

/* 物理驱动器映射 */
#define DEV_FLASH	0	    /* W25Q16映射到物理驱动器0 */
/* W25Q16 硬件参数（严格匹配实际芯片，2MB SPI Flash）*/
#define W25Q16_DEVICE_ID        0xEF16   // 实际芯片ID
#define FLASH_SECTOR_SIZE       4096     // 最小擦除单位：4KB扇区
const DWORD FLASH_TOTAL_SECTOR = 512;    // 总扇区数：2MB/4KB=512
#define FLASH_MAX_ADDR          0x1FFFFF // 最大物理地址：2MB-1


/*-----------------------------------------------------------------------*/
/* 获取驱动器状态  */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (BYTE pdrv)
{
	DSTATUS stat = STA_NOINIT; // 默认未初始化
	switch (pdrv) {
		case DEV_FLASH :
		{
			uint16_t flash_id = flash_device_id();
			// 匹配实际W25Q16 ID，清除未初始化标志
			if (flash_id == W25Q16_DEVICE_ID) { 
				stat &= ~STA_NOINIT;
			}
			UsbPrintf("--flash id=0x%04x, stat=%d\r\n", flash_id, stat);
			return stat; 
		}	
		default:
			stat = STA_NOINIT;
			return stat;
	}
}

/*-----------------------------------------------------------------------*/
/* 初始化驱动器  */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (BYTE pdrv)
{
	DSTATUS stat = STA_NOINIT;
	switch (pdrv) {
		case DEV_FLASH :
		{
			extern SPI_HandleTypeDef hspi1; 
			if (hspi1.State == HAL_SPI_STATE_RESET) {
				MX_SPI1_Init();
				HAL_Delay(20);
			}
			stat = disk_status(DEV_FLASH);
			UsbPrintf("--init stat=%d\r\n", stat);
			return stat;
		}
		default:
			stat = STA_NOINIT;
			return stat;
	}
}

/*-----------------------------------------------------------------------*/
/* 读取扇区（修复LBA_t打印+64位运算+地址越界严格校验）*/
/*-----------------------------------------------------------------------*/
DRESULT disk_read (BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    DRESULT res = RES_PARERR;
    // 1. 基础参数校验
    if (pdrv != DEV_FLASH || buff == NULL || count == 0) {
        UsbPrintf("--read err: param invalid\r\n");
        return res;
    }
    // 2. 扇区越界校验（64位转32位，避免溢出）
    uint32_t sect = (uint32_t)sector;
    if (sect + count > FLASH_TOTAL_SECTOR) {
        UsbPrintf("--read err: sector out of range (sect=%lu, count=%u, total=%lu)\r\n", 
                  sect, count, FLASH_TOTAL_SECTOR);
        res = RES_ERROR;
        return res;
    }
    // 3. 计算物理地址（4KB扇区：左移12位）
    uint32_t address = sect << 12;
    uint32_t read_len = count << 12;
    // 4. 底层读取
    int flash_res = flash_read_data(address, buff, read_len);
    if (flash_res == 1) {
        res = RES_OK;
    } else {
        res = RES_ERROR;
        UsbPrintf("--read err: flash read fail (res=%d, addr=0x%08X)\r\n", flash_res, address);
    }
    // 修复打印：%lu适配uint32_t，解决乱码
    UsbPrintf("--read sector=%lu, count=%u, address=0x%08X, res=%d\r\n", 
              sect, count, address, (int)res);
    return res;
}

/*-----------------------------------------------------------------------*/
/* 写入扇区（先擦后写+64位适配+错误码精准返回）*/
/*-----------------------------------------------------------------------*/
#if FF_FS_READONLY == 0
DRESULT disk_write (BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
	DRESULT res = RES_PARERR;
	switch (pdrv) {
		case DEV_FLASH :
		{
			uint32_t sect = (uint32_t)sector;
			UsbPrintf("--write start: sector=%lu, count=%u\r\n", sect, count);
			// 1. 基础参数校验
			if (buff == NULL || count == 0) {
			    UsbPrintf("--write err: param invalid\r\n");
			    return res;
			}
			// 2. 扇区越界校验
			if (sect + count > FLASH_TOTAL_SECTOR) {
			    UsbPrintf("--write err: sector out of range\r\n");
			    res = RES_ERROR;
			    return res;
			}
			// 3. 初始化成功状态，逐扇区擦除+写入
			res = RES_OK;
			uint32_t address = sect << 12;
			for (UINT i=0; i<count; i++) {
				uint32_t curr_addr = address + i*FLASH_SECTOR_SIZE;
				// 擦除失败，立即返回错误
				if (flash_erase_sector(curr_addr) != 1) {
					UsbPrintf("--write err: erase sect %lu fail (addr=0x%08X)\r\n", sect+i, curr_addr);
					res = RES_ERROR;
					break;
				}
				// 写入失败，立即返回错误
				if (flash_write_data(curr_addr, (uint8_t*)(buff + i*FLASH_SECTOR_SIZE), FLASH_SECTOR_SIZE) != 1) {
					UsbPrintf("--write err: write sect %lu fail (addr=0x%08X)\r\n", sect+i, curr_addr);
					res = RES_ERROR;
					break;
				}
			}
			UsbPrintf("--write end: sector=%lu, count=%u, res=%d\r\n", sect, count, (int)res);
			return res;
		}
	}
	return res;
}
#endif

/*-----------------------------------------------------------------------*/
/* 设备控制（所有指令显式赋RES_OK+参数精准返回）*/
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff)
{
	DRESULT res = RES_PARERR;
	if (pdrv != DEV_FLASH) {
	    UsbPrintf("--ioctl err: invalid pdrv=%d\r\n", pdrv);
	    return res;
	}
	// 临时变量，避免buff空指针（仅打印用）
	uint32_t buff_val = 0;
	if(buff != NULL) buff_val = *(DWORD*)buff;
	
	switch (cmd)  
	{
		case CTRL_SYNC :        // 等待Flash操作完成
			CheckBusy();
			res = RES_OK;
			break;
		case GET_SECTOR_COUNT:  // 获取总扇区数：512
            *(DWORD*)buff = FLASH_TOTAL_SECTOR;
            buff_val = FLASH_TOTAL_SECTOR;
            res = RES_OK;
            break;
        case GET_SECTOR_SIZE:   // 获取扇区大小：4096
            *(WORD*)buff = FLASH_SECTOR_SIZE;
            buff_val = FLASH_SECTOR_SIZE;
            res = RES_OK;
            break;  
        case GET_BLOCK_SIZE:    // 擦除块大小：16个4KB扇区（64KB）
            *(DWORD*)buff = 16;
            buff_val = 16;
            res = RES_OK;
            break;
        default: // 未知指令
            UsbPrintf("--ioctl err: unknown cmd=%d\r\n", cmd);
            res = RES_PARERR;
            break;
    }
	UsbPrintf("--ioctl cmd=%d, res=%d, buff_val=%lu\r\n", cmd, (int)res, buff_val);
	return res;
}


/*-----------------------------------------------------------------------*/
/* 获取文件时间（FatFs必需，无RTC返回0即可）*/
/*-----------------------------------------------------------------------*/
DWORD get_fattime (void)
{
	return 0;
}

