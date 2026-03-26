#include "fatfs.h"
#include "usart.h"
#include "user_diskio.h"
#include "rtc.h"

uint8_t retUSER;
char USERPath[4] = "0:";
FATFS USERFatFS;
FIL USERFile;
UINT fnum;

// 全局时间结构体
FatFs_Time_t g_fatfs_time = {2026, 3, 20, 0, 0, 0};//AT 时间用

/**
 * @brief  FATFS初始化（驱动挂载）
 */
void MX_FATFS_Init(void)
{
    retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);
    if (retUSER != FR_OK)
    {
        UsbPrintf("[fatfs] 驱动链接失败！错误码：%d\r\n", retUSER);
        // 不卡死，仅返回
        return;
    }
    UsbPrintf("[fatfs] 初始化完成！请执行挂载/格式化\r\n");
}

/**
 * @brief  获取FatFS时间戳（供FatFS内部调用）
 * @note   从RTC硬件获取实时时间，替代原固定全局时间
 */
DWORD get_fattime(void)
{
    // uint8_t year;    // 现在是 uint8_t
    // uint8_t month;
    // uint8_t day;
    uint8_t hour, min, sec;

    // 调用完全匹配！无警告！
    //RTC_GetCurrentDate(&year, &month, &day);
    RTC_GetCurrentTime(&hour, &min, &sec);

    // 20xx 年：year 是 26 → 2026
    //uint16_t full_year = 2000 + year;

    return ((DWORD)(g_fatfs_time.year - 1980) << 25)
         | ((DWORD)g_fatfs_time.month << 21)
         | ((DWORD)g_fatfs_time.day << 16)
         | ((DWORD)hour << 11)
         | ((DWORD)min << 5)
         | ((DWORD)(sec / 2));
}

/**
 * @brief  挂载文件系统
 */
FRESULT FatFs_Mount(void)
{
    // 先卸载
    retUSER = f_mount(NULL, USERPath, 0);

    // 挂载（1=立即挂载）
    retUSER = f_mount(&USERFatFS, USERPath, 1);

    switch(retUSER)
    {
        case FR_OK:
            UsbPrintf("[fatfs] 挂载成功\r\n");
            return FR_OK;

        case FR_NO_FILESYSTEM:
            UsbPrintf("[fatfs] 未格式化，请先格式化\r\n");
            return retUSER;

        default:
            UsbPrintf("[fatfs] 挂载失败：%d\r\n", retUSER);
            return retUSER;
    }
}

/**
 * @brief  格式化（修复版）
 */
FRESULT FatFs_Format(void)
{
    FRESULT res;

    UsbPrintf("[fatfs] 卸载并同步...\r\n");
    f_mount(NULL, USERPath, 0);
    disk_ioctl(0, CTRL_SYNC, NULL);  // 必须等Flash空闲

    // 预挂载
    res = f_mount(&USERFatFS, USERPath, 1);
    if(res != FR_OK && res != FR_NO_FILESYSTEM)
    {
        UsbPrintf("[fatfs] 预挂载失败\r\n");
        return res;
    }

    UsbPrintf("[fatfs] 开始格式化...\r\n");

    res = f_mkfs(USERPath, 1, 0);

    if(res != FR_OK)
    {
        UsbPrintf("[fatfs] 格式化失败：%d\r\n", res);
        return res;
    }

    disk_ioctl(0, CTRL_SYNC, NULL);
    res = f_mount(&USERFatFS, USERPath, 1);

    if(res == FR_OK)
    {
        UsbPrintf("[fatfs] 格式化+挂载成功！FS类型：%d\r\n", USERFatFS.fs_type);
    }
    else
    {
        UsbPrintf("[fatfs] 格式化后挂载失败：%d\r\n", res);
    }
    
    disk_ioctl(0, CTRL_SYNC, NULL);
    W25Q64_WaitBusy();
    return res;
}

/**
 * @brief  列出根目录文件
 */
void FatFs_ListDir(void)
{
    DIR dir;
    FILINFO fno;
    FRESULT res;

    UsbPrintf("\r\n====== [目录] ======\r\n");

    res = f_opendir(&dir, USERPath);
    if (res != FR_OK)
    {
        if (res == FR_NO_FILESYSTEM)
            UsbPrintf("[fatfs] 未格式化\r\n");
        else if (res == FR_NOT_READY)
            UsbPrintf("[fatfs] 未挂载\r\n");
        else
            UsbPrintf("[fatfs] 打开目录失败：%d\r\n", res);

        UsbPrintf("====== END ======\r\n");
        return;
    }

    uint8_t file_count = 0;
    while (1)
    {
        memset(&fno, 0, sizeof(FILINFO));
        res = f_readdir(&dir, &fno);

        // 结束或错误
        if (res != FR_OK || fno.fname[0] == 0)
            break;

        // 跳过隐藏/系统/点文件
        if ((fno.fname[0] == '.') || (fno.fattrib & (AM_HID | AM_SYS)))
            continue;

        // 时间解析
        uint16_t fdate = fno.fdate;
        uint16_t ftime = fno.ftime;

        uint16_t year  = ((fdate >> 9) & 0x7F) + 1980;
        uint8_t  month = (fdate >> 5) & 0x0F;
        uint8_t  day   = fdate & 0x1F;
        uint8_t  hour  = (ftime >> 11) & 0x1F;
        uint8_t  min   = (ftime >> 5) & 0x3F;
        uint8_t  sec   = (ftime & 0x1F) * 2;

        // 打印
        if (fno.fattrib & AM_DIR)
        {
            UsbPrintf("[DIR]  %-12s | %04d-%02d-%02d %02d:%02d:%02d\r\n",
                fno.fname, year, month, day, hour, min, sec);
        }
        else
        {
            UsbPrintf("%-12s | %04d-%02d-%02d %02d:%02d:%02d | %lu KB / %lu B\r\n",
                fno.fname, year, month, day, hour, min, sec, fno.fsize/1024,fno.fsize);
        }
        file_count++;
    }

    if (file_count == 0)
        UsbPrintf("[fatfs] 目录为空\r\n");

    f_closedir(&dir);
    UsbPrintf("====== END ======\r\n");
}

/**
 * @brief  卸载文件系统
 */
void FatFs_Unmount(void)
{
    f_close(&USERFile);      // ? 强制关闭残留文件句柄
    disk_ioctl(0, CTRL_SYNC, NULL);
    f_mount(NULL, USERPath, 0);
    UsbPrintf("[fatfs] 卸载成功\r\n");
}