/*-----------------------------------------------------------------------*/
/* Low level disk I/O module for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
#include "ff.h"			/* Basic definitions of FatFs */
#include "diskio.h"		/* Declarations FatFs MAI */
#include "spi.h"
#include "flash.h"
#include "usart.h"
#include <string.h>  // 字符串拼接、拷贝
#include <stdio.h>   // sprintf格式化命名

/************************ 与flash.c完全一致的宏定义（重命名避冲突） ************************/
#define DEV_FLASH		        0U	    /* 映射FLASH到物理驱动器0 */
#define W25Q_SECTOR_SIZE        4096U   /* W25Q64硬件扇区大小（4KB擦除粒度） */
#define W25Q_FLASH_END_ADDR     0x7FFFFFU// W25Q64总容量8MB(0x000000~0x7FFFFF)
#define W25Q64_DEVICE_ID        0xEF4017U // 9F指令读取的JEDEC ID，无需修改
#define DISKIO_LOGIC_SECTOR     512U    // diskio标准逻辑扇区（512字节）
#define FILE_NUM_MAX            999U    // 最大文件编号，防止溢出

/************************ FatFs全局对象定义 ************************/
FATFS fs;          // 文件系统对象
FIL file;          // 全局文件对象（单任务下可用，多任务建议局部）
FRESULT f_res;     // FATFS函数返回值（用于判断操作结果）
UINT br;           // 实际读取的字节数
#define READ_BUF_SIZE 4096U
uint8_t g_read_buf[READ_BUF_SIZE] = {0};
static uint16_t g_W25Q_SECTOR_flag = 0;
// 格式化锁改为只读，仅格式化成功后置1，掉电后首次挂载自动检测，不再手动重置
uint8_t g_fs_formatted_flag = 0; 
#define MKFS_WORK_BUF_SIZE  4096U  // 格式化工作缓冲区大小（适配W25Q64）
uint8_t mkfs_work_buf[MKFS_WORK_BUF_SIZE] = {0}; // 格式化工作缓冲区

/*-----------------------------------------------------------------------*/
/* 获取驱动器状态 ：校验Flash ID，清除未初始化标志                        */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(BYTE pdrv)
{
    DSTATUS stat = STA_NOINIT; // 默认未初始化
    switch (pdrv) {
        case DEV_FLASH:
        {
            // 读取Flash ID并校验，确保是目标设备（9F指令）
            uint32_t flash_id = flash_device_id();
            if (flash_id == W25Q64_DEVICE_ID || flash_id == 0xEF16) { // 兼容90指令的0xEF16
                stat &= ~STA_NOINIT; // 清除未初始化标志，设备就绪
            }
            return stat;
        }
        default:
            stat = STA_NOINIT;
            return stat;
    }
}

/*-----------------------------------------------------------------------*/
/* 初始化驱动器 ：移除静态标志，每次初始化都重置SPI，保证通信稳定        */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize(BYTE pdrv)
{
    DSTATUS stat = STA_NOINIT;
    switch (pdrv) {
        case DEV_FLASH:
        {
            // 移除静态标志，上电/异常后可重新初始化SPI
            MX_SPI1_Init();
            HAL_Delay(20); // 缩短延时，保证稳定即可
            stat = disk_status(DEV_FLASH);
            return stat;
        }
        default:
            stat = STA_NOINIT;
            return stat;
    }
}

/*-----------------------------------------------------------------------*/
/* 读取扇区 ：对接flash_read_data，硬件连续自增，支持跨页/跨扇区          */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(
    BYTE pdrv,		/* 物理驱动器号 */
    BYTE *buff,		/* 数据缓冲区 */
    LBA_t sector,	/* 起始扇区（LBA） */
    UINT count		/* 读取扇区数 */
)
{
    DRESULT res = RES_PARERR; // 默认参数错误
    if (pdrv != DEV_FLASH || buff == NULL || count == 0) {
        UsbPrintf("--read err: 参数非法 (pdrv=%d, buff=%p, count=%u)\r\n", pdrv, buff, count);
        return res;
    }
    // 逻辑扇区 → Flash物理地址（512字节/逻辑扇区）
    uint32_t flash_addr = (uint32_t)sector * DISKIO_LOGIC_SECTOR;
    uint32_t read_len = (uint32_t)count * DISKIO_LOGIC_SECTOR;
    // 地址越界校验（W25Q64：0x000000 ~ 0x7FFFFF）
    if (flash_addr + read_len > W25Q_FLASH_END_ADDR + 1) {
        UsbPrintf("--read err: 地址越界 (0x%06X + 0x%06X > 0x%06X)\r\n", flash_addr, read_len, W25Q_FLASH_END_ADDR);
        return RES_ERROR;
    }
    // 调用flash.c读函数（天然支持跨页/跨扇区硬件自增）
    if (flash_read_data(flash_addr, buff, read_len) == 1) {
        res = RES_OK;
    } else {
        UsbPrintf("--read err: flash_read_data执行失败 @0x%06X\r\n", flash_addr);
        res = RES_ERROR;
    }
    return res;
}

/*-----------------------------------------------------------------------*/
/* 新增：校验扇区是否擦除成功（全为0xFF）                                */
/*-----------------------------------------------------------------------*/
static uint8_t check_sector_erased(uint32_t addr)
{
    uint8_t test_data[32] = {0};
    flash_read_data(addr, test_data, sizeof(test_data));
    for (uint8_t i = 0; i < sizeof(test_data); i++) {
        if (test_data[i] != 0xFF)
            return 0; // 未擦除成功
    }
    return 1; // 擦除成功
}

/*-----------------------------------------------------------------------*/
/* 写入扇区 ：核心修复：写入后等待Flash忙标志，确保操作彻底完成           */
/*-----------------------------------------------------------------------*/
#if FF_FS_READONLY == 0
DRESULT disk_write(
    BYTE pdrv,			/* 物理驱动器号 */
    const BYTE *buff,	/* 待写入数据 */
    LBA_t sector,		/* 起始扇区（LBA） */
    UINT count			/* 写入扇区数 */
)
{
    DRESULT res = RES_PARERR; // 默认参数错误
    if (pdrv != DEV_FLASH || buff == NULL || count == 0) {
        UsbPrintf("--write err: 参数非法 (pdrv=%d, buff=%p, count=%u)\r\n", pdrv, buff, count);
        return res;
    }
    // 1. 逻辑扇区 → Flash物理地址（512字节/逻辑扇区）
    uint32_t flash_addr = (uint32_t)sector * DISKIO_LOGIC_SECTOR;
    uint32_t write_len = (uint32_t)count * DISKIO_LOGIC_SECTOR;
    // 2. 地址越界校验（严格限制在W25Q64容量内）
    if (flash_addr + write_len > W25Q_FLASH_END_ADDR + 1) {
        UsbPrintf("--write err: 地址越界 (0x%06X + 0x%06X > 0x%06X)\r\n", flash_addr, write_len, W25Q_FLASH_END_ADDR);
        return RES_ERROR;
    }
    // 3. 计算需要擦除的4KB硬件扇区范围（W25Q64写前必须擦除）
    uint32_t erase_start_sec = flash_addr / W25Q_SECTOR_SIZE;
    uint32_t erase_end_sec = (flash_addr + write_len - 1) / W25Q_SECTOR_SIZE;
    // 循环擦除所有涉及的硬件扇区，确保写入区域为擦除状态
    for (uint32_t i = erase_start_sec; i <= erase_end_sec; i++) {
        uint32_t erase_addr = i * W25Q_SECTOR_SIZE;
        if (flash_erase_sector(erase_addr) == 0) {
            UsbPrintf("--write err: 擦除扇区失败 @0x%06X\r\n", erase_addr);
            return RES_ERROR;
        }
        while (CheckBusy() & 0x01); // 等待擦除完成
        // 二次校验扇区擦除状态
        if (check_sector_erased(erase_addr) == 0) {
            UsbPrintf("--write err: 扇区擦除校验失败 @0x%06X\r\n", erase_addr);
            return RES_ERROR;
        }
    }
    // 4. 调用flash.c封装的写函数（自动处理页内硬件自增+跨页/跨扇区软件自增）
    if (flash_write_data(flash_addr, (uint8_t *)buff, write_len) == 1) {
        while (CheckBusy() & 0x01) {} // 等待写入完成
        res = RES_OK;
    } else {
        UsbPrintf("--write err: flash_write_data执行失败 @0x%06X\r\n", flash_addr);
        res = RES_ERROR;
    }
    return res;
}
#endif

/*-----------------------------------------------------------------------*/
/* 设备控制 ：FatFs挂载/格式化/容量查询核心接口 强化CTRL_SYNC              */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl(
    BYTE pdrv,		/* 物理驱动器号 */
    BYTE cmd,		/* 控制指令 */
    void *buff		/* 数据缓冲区 */
)
{
    DRESULT res = RES_PARERR;
    if (pdrv != DEV_FLASH) {
        UsbPrintf("--ioctl err: 驱动器号非法 %d\r\n", pdrv);
        return res;
    }
    switch (cmd) {
        case CTRL_SYNC: // 同步Flash缓存，等待所有操作完成（强化版）
            while (CheckBusy() & 0x01); // 等待忙标志清零
            res = RES_OK;
            break;
        case GET_SECTOR_COUNT: // 获取总逻辑扇区数（8MB / 512字节 = 16384）
            *(LBA_t *)buff = 16384U;
            res = RES_OK;
            break;
        case GET_SECTOR_SIZE: // 获取逻辑扇区大小（diskio标准512字节）
            *(WORD *)buff = 512U;
            res = RES_OK;
            break;
        case GET_BLOCK_SIZE: // 获取擦除块大小（4KB / 512字节 = 8个逻辑扇区）
            *(DWORD *)buff = 8U;
            res = RES_OK;
            break;
        default: // 未知指令
            UsbPrintf("--ioctl err: 未知指令 %d\r\n", cmd);
            res = RES_PARERR;
            break;
    }
    return res;
}

/*-----------------------------------------------------------------------*/
/* 获取文件时间 ：FatFs必需，无RTC时返回固定时间                          */
/*-----------------------------------------------------------------------*/
DWORD get_fattime(void)
{
    // 格式：年(16-63) << 25 | 月(1-12) << 21 | 日(1-31) << 16 | 时(0-23) << 11 | 分(0-59) << 5 | 秒(0-59)/2
    return (DWORD)(24 - 1980) << 25 | (DWORD)1 << 21 | (DWORD)1 << 16 | (DWORD)0 << 11 | (DWORD)0 << 5 | (DWORD)0 / 2;
}

/*-----------------------------------------------------------------------*/
/* 统一挂载/格式化函数 ：重构格式化锁逻辑，永不手动重置                   */
/*-----------------------------------------------------------------------*/
void Fatfs_Mount(void)
{
    // 格式化锁为1时快速挂载，挂载失败仅打印，不重置锁
    if (g_fs_formatted_flag == 1) {
        f_res = f_mount(&fs, "0:", 1);
        if (f_res == FR_OK) {
            UsbPrintf("文件系统快速挂载成功！\r\n");
            disk_ioctl(DEV_FLASH, CTRL_SYNC, NULL); // 强制同步磁盘
            HAL_Delay(10); // 等待根目录加载完成
            return;
        } else {
            UsbPrintf("快速挂载失败，执行全检测挂载...\r\n");
        }
    }
    // 首次挂载/快速挂载失败，执行全检测挂载
    f_res = f_mount(&fs, "0:", 1);
    if (f_res != FR_OK) {
        UsbPrintf("文件系统首次挂载失败！错误码：%d\r\n", f_res);
        // 仅无文件系统时格式化，排除FR_NOT_ENABLED（防止误格式化）
        if (f_res == FR_NO_FILESYSTEM) {
            UsbPrintf("Flash无文件系统，开始格式化...\r\n");
            MKFS_PARM mkfs_opt = {
                .fmt = FM_FAT,        // 强制FAT16，适配8MB小容量
                .au_size = 8,         // 簇大小=8*512=4KB，匹配W25Q64擦除粒度
                .n_fat = 2,           // FAT表数量（容错，2个）
                .align = 1,           // 扇区对齐，匹配Flash物理扇区
                .n_root = 512         // FAT16根目录项数
            };
            f_res = f_mkfs("0:", &mkfs_opt, mkfs_work_buf, MKFS_WORK_BUF_SIZE);
            if (f_res == FR_OK) {
                UsbPrintf("格式化成功！重新挂载文件系统...\r\n");
                f_res = f_mount(&fs, "0:", 1); // 重新挂载
                if (f_res == FR_OK) {
                    UsbPrintf("格式化后挂载成功！\r\n");
                    g_fs_formatted_flag = 1; // 仅格式化成功后置1
                } else {
                    UsbPrintf("格式化后挂载失败！错误码：%d\r\n", f_res);
                    g_fs_formatted_flag = 0;
                    return;
                }
            } else {
                UsbPrintf("格式化失败！错误码：%d\r\n", f_res);
                g_fs_formatted_flag = 0;
                return;
            }
        } else {
            UsbPrintf("挂载失败原因：\r\nFR_DISK_ERR(3):Flash硬件错误;FR_NOT_ENABLED(13):驱动未就绪\r\n");
            g_fs_formatted_flag = 0;
            return;
        }
    }
    UsbPrintf("文件系统检测挂载成功！（Flash已有文件系统）\r\n");
    g_fs_formatted_flag = 1; // 已有文件系统，挂载成功后置1
}

/*-----------------------------------------------------------------------*/
/* 文件读取函数 ：资源释放规范化，永不重置格式化锁                       */
/*-----------------------------------------------------------------------*/
void fatfs_read_file(const char *file_path)
{
    if (file_path == NULL) {
        UsbPrintf("读取失败！文件路径为空\r\n");
        return;
    }
    // 1. 统一挂载（唯一挂载入口）
    Fatfs_Mount();
    if (f_res != FR_OK) {
        UsbPrintf("挂载失败，无法读取文件！\r\n");
        return;
    }
    // 2. 打开文件（仅读，避免误写）
    f_res = f_open(&file, file_path, FA_READ);
    if (f_res != FR_OK) {
        UsbPrintf("打开文件失败！路径：%s,错误码：%d\r\n", file_path, f_res);
        f_mount(NULL, "0:", 1); // 仅挂载成功后卸载
        return;
    }
    UsbPrintf("成功打开文件：%s\r\n", file_path);
    // 3. 循环读取文件内容
    UsbPrintf("==================== 文件内容 ====================\r\n");
    FSIZE_t file_size = f_size(&file); // 获取文件总大小
    FSIZE_t read_total = 0;         // 已读取总字节数
    memset(g_read_buf, 0, READ_BUF_SIZE); // 清空缓冲区
    while (read_total < file_size) {
        UINT read_len = (file_size - read_total) > READ_BUF_SIZE ? READ_BUF_SIZE : (UINT)(file_size - read_total);
        f_res = f_read(&file, g_read_buf, read_len, &br);

        if (f_res != FR_OK || br == 0) {
            UsbPrintf("文件读取失败！已读取：%lu字节,错误码:%d\r\n", read_total, f_res);
            break;
        }
        UsbPrintf("%.*s", br, g_read_buf); // 打印读取内容
        read_total += br;
        memset(g_read_buf, 0, READ_BUF_SIZE); // 清空缓冲区
    }
    // 4. 读取完成统计
    if (f_res == FR_OK) {
        UsbPrintf("\r\n==================== 读取完成 ====================\r\n");
        UsbPrintf("文件总大小：%lu 字节, 实际读取：%lu 字节\r\n", file_size, read_total);
    }
    // 5. 资源释放：先关文件，再卸载
    if (f_close(&file) != FR_OK) {
        UsbPrintf("文件句柄关闭失败！\r\n");
    }
    f_mount(NULL, "0:", 1);
    UsbPrintf("文件系统已卸载，读取操作完成！\r\n");
}

/*-----------------------------------------------------------------------*/
/* 文件尾缀自增命名函数 修复缓冲区溢出，支持正确传递缓冲区长度            */
/*-----------------------------------------------------------------------*/
uint8_t FatFs_GetAutoIncFileName(const char *prefix, const char *suffix, uint8_t num_len, char *new_name, uint16_t new_name_len)
{
    // 缓冲区长度校验（至少10字节：test001.txt共10字符）
    if (prefix == NULL || suffix == NULL || new_name == NULL || num_len == 0 || num_len > 4 || new_name_len < 10)
        return 1;
    uint32_t file_num = 1;
    char check_name[64] = {0};

    while (1) {
        // 格式符%lu适配uint32_t，避免类型不匹配
        if (num_len == 1) sprintf(check_name, "%s%lu%s", prefix, file_num, suffix);
        else if (num_len == 2) sprintf(check_name, "%s%02lu%s", prefix, file_num, suffix);
        else if (num_len == 3) sprintf(check_name, "%s%03lu%s", prefix, file_num, suffix);
        else if (num_len == 4) sprintf(check_name, "%s%04lu%s", prefix, file_num, suffix);

        // 检测文件是否存在
        FILINFO fno;
        FRESULT res = f_stat(check_name, &fno);
        if (res == FR_NO_FILE) {
            if (new_name != NULL && check_name != NULL) {
                // 用new_name_len限制拷贝长度，确保不溢出
                strncpy(new_name, check_name, new_name_len - 1);
                new_name[new_name_len - 1] = '\0'; // 强制添加字符串终止符
                // 验证拷贝完整性
                if (strlen(check_name) >= new_name_len) {
                    UsbPrintf("--GetAutoIncFileName err: new_name buffer too small\r\n");
                    return 1;
                }
            }
            return 0;
        } else if (res != FR_OK) {
            return 1;
        }
        file_num++;
        if (file_num > FILE_NUM_MAX) // 溢出保护
            return 1;
    }
}

/*-----------------------------------------------------------------------*/
/* 文件夹尾缀自增命名函数 修复缓冲区溢出，支持正确传递缓冲区长度          */
/*-----------------------------------------------------------------------*/
uint8_t FatFs_GetAutoIncDirName(const char *prefix, uint8_t num_len, char *new_name, uint16_t new_name_len)
{
    // 缓冲区长度校验（至少8字节：data001共6字符）
    if (prefix == NULL || new_name == NULL || num_len == 0 || num_len > 4 || new_name_len < 8)
        return 1;
    uint32_t dir_num = 1;
    char check_name[64] = {0};

    while (1) {
        // 格式符%lu适配uint32_t，避免类型不匹配
        if (num_len == 1) sprintf(check_name, "%s%lu", prefix, dir_num);
        else if (num_len == 2) sprintf(check_name, "%s%02lu", prefix, dir_num);
        else if (num_len == 3) sprintf(check_name, "%s%03lu", prefix, dir_num);
        else if (num_len == 4) sprintf(check_name, "%s%04lu", prefix, dir_num);

        // 检测文件夹是否存在
        DIR dir;
        FRESULT res = f_opendir(&dir, check_name);
        f_closedir(&dir); // 无论是否存在，都关闭句柄
        if (res == FR_NO_FILE) {
            if (new_name != NULL && check_name != NULL) {
                strncpy(new_name, check_name, new_name_len - 1);
                new_name[new_name_len - 1] = '\0'; // 强制添加字符串终止符
                // 验证拷贝完整性
                if (strlen(check_name) >= new_name_len) {
                    UsbPrintf("--GetAutoIncDirName err: new_name buffer too small\r\n");
                    return 1;
                }
            }
            return 0;
        } else if (res != FR_OK) {
            return 1;
        }
        dir_num++;
        if (dir_num > FILE_NUM_MAX) // 溢出保护
            return 1;
    }
}

/*-----------------------------------------------------------------------*/
/* 创建文件夹函数                                                        */
/*-----------------------------------------------------------------------*/
uint8_t FatFs_CreateDir(const char *dir_name)
{
    if (dir_name == NULL) return 1;
    FRESULT res = f_mkdir(dir_name);
    if (res == FR_OK || res == FR_EXIST)
        return 0; // 存在也返回成功
    else
        return 1;
}

/*-----------------------------------------------------------------------*/
/* 指定编号拼接文件名函数 修复缓冲区溢出，增加长度校验                    */
/*-----------------------------------------------------------------------*/
uint8_t FatFs_AssignNumToFileName(uint32_t file_num, char *new_name, uint16_t new_name_len)
{
    if (new_name == NULL || file_num < 1 || file_num > FILE_NUM_MAX || new_name_len < 10)
        return 1; // 至少10字节（test001.txt）
    // 固定格式：test + 3位补0数字 + .txt
    sprintf(new_name, "test%03lu.txt", file_num);
    // 验证：确保sprintf未溢出
    if (strlen(new_name) >= new_name_len) {
        UsbPrintf("--AssignNumToFileName err: buffer too small\r\n");
        return 1;
    }
    return 0;
}

/*-----------------------------------------------------------------------*/
/* 指定位置拷贝字符串 越界+缓冲区保护                                    */
/*-----------------------------------------------------------------------*/
void str_copy_from_pos(char *dest, uint16_t dest_len, const char *src, uint16_t start_pos, uint16_t len)
{
    // 增加目标缓冲区长度校验，防止溢出
    if (dest == NULL || src == NULL || dest_len == 0 || start_pos >= strlen(src) || len == 0) {
        dest[0] = '\0';
        return;
    }
    uint16_t src_len = strlen(src);
    uint16_t copy_len = (start_pos + len) > src_len ? (src_len - start_pos) : len;
    // 确保拷贝长度不超过目标缓冲区
    copy_len = copy_len >= dest_len ? (dest_len - 1) : copy_len;
    strncpy(dest, src + start_pos, copy_len);
    dest[copy_len] = '\0'; // 手动加结束符，防止字符串乱码
}

/*-----------------------------------------------------------------------*/
/* 自增文件名写入函数 强制数据落盘，修复文件名截断问题                   */
/*-----------------------------------------------------------------------*/
void Fatfs_Write(char *command)
{
    if (command == NULL) {
        UsbPrintf("写入失败！命令为空\r\n");
        return;
    }
    g_W25Q_SECTOR_flag++;
    UsbPrintf("===== W25Q64 FatFs 自增写入 =====\r\n");
    FIL f = {0}; // 局部文件对象，避免全局覆盖
    FRESULT res = FR_OK;
    char data[4096] = {0};
    uint16_t length = 0;
    uint16_t cmd_len = strlen(command);

    // 1. 统一挂载
    Fatfs_Mount();
    if (f_res != FR_OK) {
        UsbPrintf("挂载失败，无法执行写入操作！\r\n");
        return;
    }
    // 2. 数据解析：增加缓冲区长度校验
    if (cmd_len < 15) {
        UsbPrintf("数据解析失败！命令长度不足，有效长度：%d\r\n", cmd_len);
        f_mount(NULL, "0:", 1);
        return;
    }
    length = cmd_len - 15;
    // 传入目标缓冲区长度，防止溢出
    str_copy_from_pos(data, sizeof(data), command, 15, length);
    UsbPrintf("待写入数据长度：%d 字节\r\n", length);
    // 3. 生成自增尾缀的文件名
    char new_file[64] = {0};
    // 传递sizeof(new_file)（实际长度64），修复文件名截断
    if (FatFs_GetAutoIncFileName("test", ".txt", 3, new_file, sizeof(new_file)) != 0) {
        UsbPrintf("生成自增文件名失败！\r\n");
        f_mount(NULL, "0:", 1);
        return;
    }
    UsbPrintf("待创建自增文件：%s\r\n", new_file); // 打印完整文件名
    // 4. 打开文件并写入
    res = f_open(&f, new_file, FA_CREATE_ALWAYS | FA_WRITE);
    if (res == FR_OK) {
        UINT bw = 0;
        res = f_write(&f, data, length, &bw);
        // 强制数据落盘：文件缓存+磁盘同步
        f_sync(&f);          // 刷写当前文件缓存
        disk_ioctl(DEV_FLASH, CTRL_SYNC, NULL); // 同步磁盘FAT表/根目录
        while (CheckBusy() & 0x01); // 等待Flash硬件操作完成

        if (res == FR_OK && bw > 0) { // 只要写入字节数>0，即判定成功
            UsbPrintf("写入成功！实际写入：%d 字节\r\n", bw);
            // 写入后检测文件是否存在
            FILINFO fno;
            if (f_stat(new_file, &fno) == FR_OK) {
                UsbPrintf("文件创建成功！已记录到根目录\r\n");
            } else {
                UsbPrintf("警告：文件写入成功，但未在根目录找到！\r\n");
            }
        } else {
            UsbPrintf("写入失败！Res=%d, 实际写入：%d 字节\r\n", res, bw);
        }
        // 无论成功与否，都关闭文件句柄
        if (f_close(&f) != FR_OK) {
            UsbPrintf("文件句柄关闭失败！\r\n");
        }
    } else {
        UsbPrintf("创建文件失败！Res=%d\r\n", res);
    }
    // 5. 写入后测试读取（验证文件是否可正常读取）
    UsbPrintf("写入后测试读取文件：%s\r\n", new_file);
    // 解析文件编号，用于测试读取
    uint32_t test_num = 0;
    sscanf(new_file, "test%03lu.txt", &test_num);
    if (test_num >= 1 && test_num <= FILE_NUM_MAX) {
        Fatfs_Read(test_num);
    }
    // 6. 资源释放：仅卸载，永不重置格式化锁
    f_mount(NULL, "0:", 1);
    UsbPrintf("自增写入操作完成！\r\n");
}

/*-----------------------------------------------------------------------*/
/* 指定编号文件写入函数 强制数据落盘+规范化                               */
/*-----------------------------------------------------------------------*/
void Fatfs_Write_AssignNum(uint32_t file_num, char *command)
{
    if (command == NULL || file_num < 1 || file_num > FILE_NUM_MAX) {
        UsbPrintf("写入失败！命令为空或编号非法\r\n");
        return;
    }
    g_W25Q_SECTOR_flag++;
    UsbPrintf("===== W25Q64 FatFs 指定编号写入 [No.%lu] =====\r\n", file_num);
    FIL f = {0}; // 局部文件对象，避免全局污染
    char data[4096] = {0};
    uint16_t length = 0;
    uint16_t cmd_len = strlen(command);

    // 1. 统一挂载
    Fatfs_Mount();
    if (f_res != FR_OK) {
        UsbPrintf("文件系统挂载失败！错误码：%d\r\n", f_res);
        return;
    }
    // 2. 数据解析：增加缓冲区长度校验
    if (cmd_len < 15) {
        UsbPrintf("数据解析失败！命令长度不足，有效长度：%d\r\n", cmd_len);
        f_mount(NULL, "0:", 1);
        return;
    }
    length = cmd_len - 15;
    str_copy_from_pos(data, sizeof(data), command, 15, length);
    UsbPrintf("待写入数据长度：%d 字节\r\n", length);
    // 3. 拼接指定编号的文件名
    char target_file[64] = {0};
    if (FatFs_AssignNumToFileName(file_num, target_file, sizeof(target_file)) != 0) {
        UsbPrintf("写入失败！文件编号非法或缓冲区过小\r\n");
        f_mount(NULL, "0:", 1);
        return;
    }
    UsbPrintf("待写入目标文件：%s\r\n", target_file);
    // 4. 打开文件：覆盖写入模式
    f_res = f_open(&f, target_file, FA_CREATE_ALWAYS | FA_WRITE);
    if (f_res != FR_OK) {
        UsbPrintf("打开/创建文件失败！错误码：%d\r\n", f_res);
        f_mount(NULL, "0:", 1);
        return;
    }
    // 5. 写入数据+强制落盘
    UINT bw = 0;
    f_res = f_write(&f, data, length, &bw);
    f_sync(&f);          // 刷写当前文件缓存
    disk_ioctl(DEV_FLASH, CTRL_SYNC, NULL); // 同步磁盘FAT表
    while (CheckBusy() & 0x01); // 等待Flash硬件操作彻底完成

    if (f_res == FR_OK && bw > 0) {
        UsbPrintf("写入文件成功！实际写入：%d 字节\r\n", bw);
        // 读取文件信息
        FSIZE_t file_size = f_size(&f);
        UsbPrintf("==================== 写入完成 ====================\r\n");
        UsbPrintf("文件编号：%lu\r\n", file_num);
        UsbPrintf("文件名称：%s\r\n", target_file);
        UsbPrintf("文件总大小：%lu 字节\r\n", file_size);
    } else {
        UsbPrintf("写入文件失败！错误码：%d，实际写入：%d 字节\r\n", f_res, bw);
    }
    // 6. 资源释放：先关文件，再卸载
    if (f_close(&f) != FR_OK) {
        UsbPrintf("文件句柄关闭失败！\r\n");
    }
    f_mount(NULL, "0:", 1);
    UsbPrintf("指定编号写入操作完成！\r\n");
}

/*-----------------------------------------------------------------------*/
/* 指定编号文件读取函数 规范化+缓冲区保护                                 */
/*-----------------------------------------------------------------------*/
void Fatfs_Read(uint32_t file_num)
{
    if (file_num < 1 || file_num > FILE_NUM_MAX) {
        UsbPrintf("读取失败！文件编号非法\r\n");
        return;
    }
    UsbPrintf("===== W25Q64 FatFs 指定编号读取 [No.%lu] =====\r\n", file_num);
    FIL f = {0}; // 局部文件对象
    // 1. 统一挂载
    Fatfs_Mount();
    if (f_res != FR_OK) {
        UsbPrintf("文件系统挂载失败！错误码：%d\r\n", f_res);
        return;
    }
    // 2. 拼接指定编号的文件名
    char target_file[64] = {0};
    if (FatFs_AssignNumToFileName(file_num, target_file, sizeof(target_file)) != 0) {
        UsbPrintf("读取失败！文件编号非法或缓冲区过小\r\n");
        f_mount(NULL, "0:", 1);
        return;
    }
    UsbPrintf("待读取目标文件：%s\r\n", target_file);
    // 3. 检测文件是否存在
    FILINFO fno;
    f_res = f_stat(target_file, &fno);
    if (f_res == FR_NO_FILE) {
        UsbPrintf("读取失败！文件 %s 不存在\r\n", target_file);
        f_mount(NULL, "0:", 1);
        return;
    } else if (f_res != FR_OK) {
        UsbPrintf("文件检测失败！错误码：%d\r\n", f_res);
        f_mount(NULL, "0:", 1);
        return;
    }
    // 4. 打开文件（只读模式）
    f_res = f_open(&f, target_file, FA_READ);
    if (f_res != FR_OK) {
        UsbPrintf("打开文件失败！错误码：%d\r\n", f_res);
        f_mount(NULL, "0:", 1);
        return;
    }
    UsbPrintf("成功打开文件：%s\r\n", target_file);
    // 5. 读取文件内容
    UsbPrintf("==================== 文件内容 ====================\r\n");
    FSIZE_t file_size = f_size(&f);
    FSIZE_t read_total = 0;
    UINT br_temp = 0;
    memset(g_read_buf, 0, READ_BUF_SIZE);
    while (read_total < file_size) {
        UINT read_len = (file_size - read_total) > READ_BUF_SIZE ? READ_BUF_SIZE : (UINT)(file_size - read_total);
        f_res = f_read(&f, g_read_buf, read_len, &br_temp);

        if (f_res != FR_OK || br_temp == 0) {
            UsbPrintf("文件读取异常！已读取：%lu 字节，错误码：%d\r\n", read_total, f_res);
            break;
        }
        UsbPrintf("%.*s", br_temp, g_read_buf);
        read_total += br_temp;
        memset(g_read_buf, 0, READ_BUF_SIZE);
    }
    // 6. 读取完成统计
    if (f_res == FR_OK) {
        UsbPrintf("\r\n==================== 读取完成 ====================\r\n");
        UsbPrintf("文件编号：%lu\r\n", file_num);
        UsbPrintf("文件名称：%s\r\n", target_file);
        UsbPrintf("文件总大小：%lu 字节\r\n", file_size);
        UsbPrintf("实际读取：%lu 字节\r\n", read_total);
    }
    // 7. 资源释放
    if (f_close(&f) != FR_OK) {
        UsbPrintf("文件句柄关闭失败！\r\n");
    }
    f_mount(NULL, "0:", 1);
    UsbPrintf("指定编号读取操作完成！\r\n");
}

/*-----------------------------------------------------------------------*/
/* 遍历读取1~N的所有存在的文件                                           */
/*-----------------------------------------------------------------------*/
void Fatfs_Read_All(uint32_t max_num)
{
    if (max_num < 1 || max_num > FILE_NUM_MAX) {
        UsbPrintf("遍历失败！最大编号非法\r\n");
        return;
    }
    UsbPrintf("===== W25Q64 FatFs 遍历读取 [1~%lu] =====\r\n", max_num);
    for (uint32_t i = 1; i <= max_num; i++) {
        char check_file[64] = {0};
        if (FatFs_AssignNumToFileName(i, check_file, sizeof(check_file)) == 0) {
            FILINFO fno;
            if (f_stat(check_file, &fno) == FR_OK) {
                Fatfs_Read(i);
                UsbPrintf("----------------------------------------\r\n");
            }
        }
    }
    UsbPrintf("所有文件遍历读取完成！\r\n");
}







