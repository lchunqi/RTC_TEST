/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdarg.h>
#include "ff_gen_drv.h"
#include "usart.h"
#include "flash.h"
#include "user_diskio.h"

/* Private variables ---------------------------------------------------------*/
static volatile DSTATUS Stat = STA_NOINIT;

/* 串口打印 */
#define ENABLE_DISKIO_PRINT     0
#if ENABLE_DISKIO_PRINT
#define Diskio_Print(fmt, ...)  UsbPrintf(fmt, ##__VA_ARGS__)
#else
#define Diskio_Print(fmt, ...)  do {} while(0)
#endif

/* Private function prototypes -----------------------------------------------*/
DSTATUS USER_initialize (BYTE pdrv);
DSTATUS USER_status (BYTE pdrv);
DRESULT USER_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
DRESULT USER_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif
#if _USE_IOCTL == 1
DRESULT USER_ioctl (BYTE pdrv, BYTE cmd, void *buff);
#endif

Diskio_drvTypeDef  USER_Driver =
{
  USER_initialize,
  USER_status,
  USER_read,
#if  _USE_WRITE
  USER_write,
#endif
#if  _USE_IOCTL == 1
  USER_ioctl,
#endif
};

DSTATUS USER_initialize (BYTE pdrv)
{
    Diskio_Print("[DiskIO] 初始化设备：%d\r\n", pdrv);
    switch (pdrv)
    {
      case DEV_FLASH:
        // 真正初始化：自检 + 解除写保护
        if (W25Q64_SelfTest() != 0)
        {
            Stat = STA_NOINIT;
            Diskio_Print("[DiskIO] W25Q64 自检失败\r\n");
            return Stat;
        }
        Stat = USER_status(DEV_FLASH);
        if (!(Stat & STA_NOINIT))
        {
            Diskio_Print("[DiskIO] 初始化成功\r\n");
        }
        break;

      default:
        Stat = STA_NOINIT;
        Diskio_Print("[DiskIO] 无效设备\r\n");
    }
    return Stat;
}

DSTATUS USER_status (BYTE pdrv)
{
    switch (pdrv)
    {
      case DEV_FLASH:
      {
        uint32_t dev_id = W25Q64_ReadDeviceID();
        if (dev_id == W25Q64_DEVICE_ID || dev_id == 0xEF16)
        {
            Stat &= ~STA_NOINIT;
        }
        else
        {
            Stat = STA_NOINIT;
            Diskio_Print("[DiskIO] W25Q64 ID不匹配\r\n");
        }
        break;
      }
      default:
        Stat = STA_NOINIT;
    }
    return Stat;
}

DRESULT USER_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    Diskio_Print("[DiskIO] 读取：%lu 扇区，%u 个\r\n", sector, count);

    if (pdrv != DEV_FLASH || buff == NULL || count == 0)
        return RES_PARERR;

    if (USER_status(DEV_FLASH) & STA_NOINIT)
        return RES_NOTRDY;

    if (sector >= W25Q64_TOTAL_LOGIC_SECTOR || (sector + count) > W25Q64_TOTAL_LOGIC_SECTOR)
    {
        Diskio_Print("[DiskIO] 读取越界\r\n");
        return RES_ERROR;
    }

    uint32_t addr = sector * W25Q64_LOGIC_SECTOR_SIZE;
    uint32_t len  = count  * W25Q64_LOGIC_SECTOR_SIZE;

    // 读取返回值判断
    if (W25Q64_ReadData(addr, buff, len) != 0)
    {
        Diskio_Print("[DiskIO] 读取失败\r\n");
        return RES_ERROR;
    }

    Diskio_Print("[DiskIO] 读取成功\r\n");
    return RES_OK;
}

#if _USE_WRITE == 1
DRESULT USER_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    if (pdrv != DEV_FLASH || buff == NULL || count == 0)
        return RES_PARERR;

    if (USER_status(DEV_FLASH) & STA_NOINIT)
        return RES_NOTRDY;

    if (sector >= W25Q64_TOTAL_LOGIC_SECTOR || (sector + count) > W25Q64_TOTAL_LOGIC_SECTOR)
        return RES_ERROR;

    // 逐扇区写入（FatFS 目录/FAT 必须每次都擦除写入！）
    for (UINT i = 0; i < count; i++)
    {
        DWORD cur_sec = sector + i;
        uint32_t addr = cur_sec * W25Q64_LOGIC_SECTOR_SIZE;
        const uint8_t *src = buff + i * W25Q64_LOGIC_SECTOR_SIZE;

        W25Q64_EraseSector(addr);

        // 分页写入 4KB
        for (uint16_t j = 0; j < W25Q64_LOGIC_SECTOR_SIZE; j += W25Q64_PAGE_SIZE)
        {
            if (W25Q64_PageWrite(addr + j, src + j, W25Q64_PAGE_SIZE) != 0)
            {
                return RES_ERROR;
            }
        }
        W25Q64_WaitBusy();
    }

    disk_ioctl(0, CTRL_SYNC, NULL);
    return RES_OK;
}
#endif

#if _USE_IOCTL == 1
DRESULT USER_ioctl (BYTE pdrv, BYTE cmd, void *buff)
{
    if (pdrv != DEV_FLASH) return RES_PARERR;
    if (USER_status(DEV_FLASH) & STA_NOINIT) return RES_NOTRDY;
    if (!buff && cmd != CTRL_SYNC) return RES_PARERR;

    DRESULT res = RES_OK;

    switch (cmd)
    {
        case CTRL_SYNC:
            W25Q64_WaitBusy();
            break;

        case GET_SECTOR_COUNT:
            *(DWORD*)buff = W25Q64_TOTAL_LOGIC_SECTOR;
            break;

        case GET_SECTOR_SIZE:
            *(WORD*)buff = W25Q64_LOGIC_SECTOR_SIZE;
            break;

        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;
            break;

        case CTRL_ERASE_SECTOR:
        {
            // 批量擦除（FatFs 格式化用）
            DWORD *sectors = (DWORD*)buff;
            for (DWORD s = sectors[0]; s <= sectors[1]; s++)
            {
                if (s >= W25Q64_TOTAL_LOGIC_SECTOR) continue;
                W25Q64_EraseSector(s * W25Q64_LOGIC_SECTOR_SIZE);
            }
            W25Q64_WaitBusy();
            break;
        }

        default:
            res = RES_PARERR;
            break;
    }
    return res;
}
#endif /* _USE_IOCTL */