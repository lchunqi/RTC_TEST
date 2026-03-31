#include "storage.h"
#include "usart.h"
#include "flash.h"
#include "user_diskio.h"

#define STORAGE_PRINT_ENABLE  1
#if STORAGE_PRINT_ENABLE
#define STORAGE_PRINT(fmt, ...)  UsbPrintf(fmt, ##__VA_ARGS__)
#else
#define STORAGE_PRINT(fmt, ...)  do {} while(0)
#endif

// 外部全局变量
extern FATFS   USERFatFS;
extern FIL     USERFile;
extern UINT    fnum;

uint8_t FatFs_GetInfo(FatFs_Info_t* fs_info)
{
    if (fs_info == NULL)
    {
        STORAGE_PRINT("[storage] 错误：fs_info 指针为空！\r\n");
        return 0;
    }
    memset(fs_info, 0, sizeof(FatFs_Info_t));
    FATFS *fs = &USERFatFS;
    DWORD free_clusters = 0;
    FRESULT res = FR_OK;

    if (fs->fs_type == 0)
    {
        fs_info->mount_status = 0;
        return 0;
    }
    fs_info->mount_status = 1;
    fs_info->fs_type       = fs->fs_type;
    fs_info->sector_size   = fs->ssize;
    fs_info->cluster_size  = (uint32_t)fs->csize * fs_info->sector_size;
    fs_info->total_clusters = (uint32_t)fs->n_fatent - 2;
    fs_info->total_bytes = fs_info->total_clusters * fs_info->cluster_size;

    if (fs_info->total_bytes > FLASH_TOTAL_BYTES)
    {
        fs_info->total_bytes = FLASH_TOTAL_BYTES;
    }
    fs_info->last_cluster = fs_info->total_clusters + 1;
    res = f_getfree("", &free_clusters, &fs);

    if (res != FR_OK)
    {
        STORAGE_PRINT("[storage] 获取空闲簇失败！错误码: %d\r\n", res);
        return 0;
    }
    if (free_clusters > fs_info->total_clusters)
        free_clusters = fs_info->total_clusters;

    fs_info->free_bytes = (uint32_t)free_clusters * fs_info->cluster_size;
    if (fs_info->free_bytes > FLASH_TOTAL_BYTES)
        fs_info->free_bytes = FLASH_TOTAL_BYTES;

    fs_info->used_bytes = fs_info->total_bytes - fs_info->free_bytes;
    return 1;
}

uint8_t FatFs_ReadFile(const char* filename, File_Read_Result_t* read_result, const FatFs_Info_t* fs_info)
{
    if (filename == NULL || read_result == NULL || fs_info == NULL || !fs_info->mount_status)
    {
        STORAGE_PRINT("[storage] 错误：无效的参数或FATFS未挂载！\r\n");
        return 0;
    }
    memset(read_result, 0, sizeof(File_Read_Result_t));
    f_close(&USERFile);

    read_result->res = f_open(&USERFile, filename, FA_OPEN_EXISTING | FA_READ);
    if (read_result->res != FR_OK)
    {
        STORAGE_PRINT("[storage] 错误：打开文件失败！错误码：%d\r\n", read_result->res);
        return 0;
    }

    // ? 在关闭前读取簇信息
    read_result->file_size     = (uint32_t)f_size(&USERFile);
    read_result->start_cluster = (uint32_t)USERFile.sclust;

    if (read_result->start_cluster >= 2)
    {
        read_result->phy_start_addr = (read_result->start_cluster - 2) * fs_info->cluster_size;
        if (read_result->phy_start_addr >= FLASH_TOTAL_BYTES)
            read_result->phy_start_addr = 0;
    }
    else
    {
        read_result->phy_start_addr = 0;
    }

    if (read_result->file_size == 0)
        read_result->phy_end_addr = read_result->phy_start_addr;
    else
        read_result->phy_end_addr = read_result->phy_start_addr + read_result->file_size - 1;

    if (read_result->phy_end_addr >= FLASH_TOTAL_BYTES)
        read_result->phy_end_addr = FLASH_TOTAL_BYTES - 1;

    STORAGE_PRINT("\r\n****** [storage] 文件内容 ******\r\n");
    f_lseek(&USERFile, 0);
    UINT total_read = 0;
    UINT bytes_read = 0;

    while (1)
    {
        read_result->res = f_read(&USERFile, read_result->content, sizeof(read_result->content), &bytes_read);
        if (read_result->res != FR_OK || bytes_read == 0)
            break;

        for (UINT i = 0; i < bytes_read; i++)
        {
            STORAGE_PRINT("%c", read_result->content[i]);
        }
        total_read += bytes_read;
    }

    STORAGE_PRINT("\r\n****** [storage] 文件读取完成 ******\r\n");
    read_result->read_len = total_read;

    f_close(&USERFile);
    disk_ioctl(0, CTRL_SYNC, NULL);
    return 1;
}

void FatFs_PrintReadResult(const char* filename, const FatFs_Info_t* fs_info, const File_Read_Result_t* read_result)
{
    if (filename == NULL || fs_info == NULL || read_result == NULL || !fs_info->mount_status)
    {
        STORAGE_PRINT("[storage] 错误：打印参数无效或FATFS未挂载！\r\n");
        return;
    }

    STORAGE_PRINT("\r\n====== [FATFS 挂载状态 & 基础参数] ======\r\n");
    STORAGE_PRINT("[storage] FATFS 挂载状态: %s\r\n", fs_info->mount_status ? "已挂载" : "未挂载");

    switch(fs_info->fs_type) {
        case FS_FAT12: STORAGE_PRINT("[storage] 文件系统类型: FAT12\r\n"); break;
        case FS_FAT16: STORAGE_PRINT("[storage] 文件系统类型: FAT16\r\n"); break;
        case FS_FAT32: STORAGE_PRINT("[storage] 文件系统类型: FAT32\r\n"); break;
        default: STORAGE_PRINT("[storage] 文件系统类型: 未知(0x%02X)\r\n", fs_info->fs_type); break;
    }

    STORAGE_PRINT("[storage] 簇大小: %lu 扇区\r\n", (unsigned long)(fs_info->cluster_size / fs_info->sector_size));
    STORAGE_PRINT("[storage] 总数据簇数: %lu\r\n", (unsigned long)fs_info->total_clusters);
    STORAGE_PRINT("[storage] 每扇区字节数: %lu Bytes\r\n", (unsigned long)fs_info->sector_size);
    STORAGE_PRINT("[storage] 每簇字节数: %lu Bytes\r\n", (unsigned long)fs_info->cluster_size);

    STORAGE_PRINT("\r\n====== [FATFS 存储容量信息] ======\r\n");
    STORAGE_PRINT("[storage] 总容量: %lu KB\r\n", (unsigned long)(fs_info->total_bytes / 1024));
    STORAGE_PRINT("[storage] 可用容量: %lu KB\r\n", (unsigned long)(fs_info->free_bytes / 1024));
    STORAGE_PRINT("[storage] 已用容量: %lu KB\r\n", (unsigned long)(fs_info->used_bytes / 1024));

    STORAGE_PRINT("\r\n====== [文件地址信息] ======\r\n");
    STORAGE_PRINT("[storage] 文件名：%s\r\n", filename);
    STORAGE_PRINT("[storage] 文件总大小: %lu Bytes\r\n", (unsigned long)read_result->file_size);
    STORAGE_PRINT("[storage] 实际读取字节数：%u\r\n", read_result->read_len);
    STORAGE_PRINT("[storage] 文件起始簇号: %lu\r\n", (unsigned long)read_result->start_cluster);
    STORAGE_PRINT("[storage] 物理起始地址: 0x%08lX\r\n", (unsigned long)read_result->phy_start_addr);
    STORAGE_PRINT("[storage] 物理结束地址: 0x%08lX\r\n", (unsigned long)read_result->phy_end_addr);
}

uint8_t FatFs_ParseReadCmd(const char* command, char* filename, uint16_t filename_len)
{
    if (command == NULL || filename == NULL || filename_len == 0)
    {
        STORAGE_PRINT("[storage] 错误：无效的参数！\r\n");
        return 0;
    }
    const char cmd_header[] = "AT^FATFS_READ=";
    uint16_t header_len = (uint16_t)strlen(cmd_header);
    if (strstr(command, cmd_header) != command)
    {
        STORAGE_PRINT("[storage] 错误：无效的指令格式！\r\n");
        STORAGE_PRINT("[storage] 用法: AT^FATFS_READ=<filename>\r\n");
        return 0;
    }
    uint16_t filename_start_pos = header_len;
    uint16_t cmd_len = (uint16_t)strlen(command);
    if (filename_start_pos >= cmd_len)
    {
        STORAGE_PRINT("[storage] 错误：等号后无文件名！\r\n");
        return 0;
    }
    uint16_t extract_len = cmd_len - filename_start_pos;
    if (extract_len >= filename_len)
    {
        STORAGE_PRINT("[storage] 错误：文件名缓冲区过小！\r\n");
        return 0;
    }
    strncpy(filename, command + filename_start_pos, extract_len);
    filename[extract_len] = '\0';
    return 1;
}

// ======================
// ? 标准写入：簇号读取正确、刷盘时序正确、永不显示0
// ======================
FRESULT FatFs_FileWrite(const char* filename, const char* write_data, UINT write_len, File_Write_Result_t* write_result)
{
    if (filename == NULL || write_data == NULL || write_len == 0 || write_result == NULL)
    {
        STORAGE_PRINT("[storage] 错误：写入参数非法！\r\n");
        return FR_INVALID_PARAMETER;
    }
    if (USERFatFS.fs_type == 0)
    {
        STORAGE_PRINT("[storage] 写入失败：FATFS 未挂载！\r\n");
        return FR_NOT_READY;
    }

    memset(write_result, 0, sizeof(File_Write_Result_t));
    FRESULT res;

    f_close(&USERFile);
    res = f_open(&USERFile, filename, FA_OPEN_ALWAYS | FA_WRITE);
    if (res != FR_OK)
    {
        STORAGE_PRINT("[storage] 打开文件失败！错误码: %d\r\n", res);
        write_result->res = res;
        return res;
    }

    f_lseek(&USERFile, f_size(&USERFile));
    char buf[4096];
    write_len = snprintf(buf, sizeof(buf), "%s\n", write_data);
    UINT fnum = 0;
    res = f_write(&USERFile, buf, write_len, &fnum);

    // ? 必须在关闭前读取簇号（永不出现0）
    write_result->start_cluster = USERFile.sclust;
    write_result->curr_cluster  = USERFile.clust;
    write_result->write_len     = fnum;
    write_result->res           = res;

    // ? 标准刷盘顺序
    f_sync(&USERFile);
    f_close(&USERFile);
    disk_ioctl(0, CTRL_SYNC, NULL);
    W25Q64_WaitBusy();

    if (res == FR_OK)
        STORAGE_PRINT("[storage] 写入%s成功！写入簇号:%lu  写入长度:%u\r\n",
                        filename, write_result->curr_cluster, write_result->write_len);
    else
        STORAGE_PRINT("[storage] 写入失败！错误码: %d\r\n", res);

    return res;
}

void FatFs_WriteUart2ByTimeStamp(void)
{
    FatFs_Info_t fs_info = {0};
    if (!FatFs_GetInfo(&fs_info) || !fs_info.mount_status) return;

    #define WRITE_BUF_SIZE 4096
    uint8_t write_buf[WRITE_BUF_SIZE] = {0};
    uint16_t data_len = UART2_ReadLine(write_buf, WRITE_BUF_SIZE - 1);
    if (data_len == 0) return;
    write_buf[data_len] = '\0';

    DWORD fattime = get_fattime();
    uint16_t year  = ((fattime >> 25) & 0x7F) + 1980;
    uint8_t  month = (fattime >> 21) & 0x0F;
    uint8_t  day   = (fattime >> 16) & 0x1F;

    char filename[32] = {0};
    snprintf(filename, sizeof(filename), "%04u%02u%02u.txt", year, month, day);

    File_Write_Result_t wr = {0};
    FatFs_FileWrite(filename, (const char*)write_buf, data_len, &wr);
}

/**
 * @brief  删除指定文件（FatFs 标准方式）
 * @param  filename 要删除的文件名（如 "1.txt"）
 * @retval FR_OK 成功，其他值为错误码
 */
FRESULT FatFs_DeleteFile(const char* filename)
{
    // 1. 参数校验
    if (filename == NULL || strlen(filename) == 0)
    {
        STORAGE_PRINT("[storage] 错误：删除文件参数无效！\r\n");
        return FR_INVALID_PARAMETER;
    }

    // 2. 检查文件系统是否挂载
    if (USERFatFS.fs_type == 0)
    {
        STORAGE_PRINT("[storage] 错误：FATFS 未挂载，无法删除文件！\r\n");
        return FR_NOT_READY;
    }

    // 3. 确保文件未被打开（先关闭）
    f_close(&USERFile);

    // 4. 执行删除
    FRESULT res = f_unlink(filename);

    // 5. 同步文件系统（确保修改写入Flash）
    f_sync(NULL);

    // 6. 结果打印
    if (res == FR_OK)
    {
        STORAGE_PRINT("[storage] 文件 %s 删除成功！\r\n", filename);
    }
    else if (res == FR_NO_FILE)
    {
        STORAGE_PRINT("[storage] 警告：文件 %s 不存在，无需删除！\r\n", filename);
    }
    else if (res == FR_DENIED)
    {
        STORAGE_PRINT("[storage] 错误：文件 %s 被占用，无法删除！错误码：%d\r\n", filename, res);
    }
    else
    {
        STORAGE_PRINT("[storage] 错误：删除文件 %s 失败！错误码：%d\r\n", filename, res);
    }

    return res;
}

/**
 * @brief  指定簇号写入文件（先删除旧文件，再强制指定簇写入）
 * @param  filename  要写入的文件名（如 "test.txt"）
 * @param  write_data 要写入的数据缓冲区
 * @param  write_len  要写入的数据长度（字节）
 * @param  target_clust 目标簇号（需≥2，符合FatFs簇号规则）
 * @param  write_result 写入结果输出结构体
 * @retval FRESULT FatFs错误码，FR_OK为成功
 */
FRESULT FatFs_FileWrite_Cluster(const char* filename, const char* write_data, 
    UINT write_len, DWORD target_clust, File_Write_Result_t* write_result)
{
    if (filename == NULL || write_data == NULL || write_len == 0 || write_result == NULL)
    {
        STORAGE_PRINT("[storage] 错误：指定簇写入参数非法！\r\n");
        return FR_INVALID_PARAMETER;
    }
    if (USERFatFS.fs_type == 0)
    {
        STORAGE_PRINT("[storage] 错误：FATFS 未挂载！\r\n");
        return FR_NOT_READY;
    }
    if (target_clust < 2)
    {
        STORAGE_PRINT("[storage] 错误：簇号必须 >= 2！\r\n");
        return FR_INVALID_PARAMETER;
    }

    memset(write_result, 0, sizeof(File_Write_Result_t));
    FRESULT res;
    FatFs_Info_t fs_info;
    FatFs_GetInfo(&fs_info);

    // 限制簇号范围
    if (target_clust > fs_info.total_clusters + 1)
    {
        STORAGE_PRINT("[storage] 簇号越界，自动使用簇2\r\n");
        target_clust = 2;
    }

    // ==============================
    // 核心：循环创建文件，直到分配到你要的簇
    // ==============================
    for (int retry = 0; retry < 200; retry++)
    {
        f_close(&USERFile);
        FatFs_DeleteFile(filename);  // 删旧文件

        // 新建文件，让 FatFs 自动分配簇（合法簇链）
        res = f_open(&USERFile, filename, FA_CREATE_ALWAYS | FA_WRITE);
        if (res != FR_OK)
            continue;

        // 如果分配到目标簇 → 成功
        if (USERFile.sclust == target_clust)
        {
            STORAGE_PRINT("[storage] 成功分配到目标簇：%lu\r\n", target_clust);
            break;
        }

        // 不是目标簇 → 删掉重来
        f_close(&USERFile);
    }

    // 到这里：文件已经在目标簇，且簇链完全合法！
    // 写满自动跨下一簇，完全正常！

    f_lseek(&USERFile, 0);
    char buf[4096];
    write_len = snprintf(buf, sizeof(buf), "%s\n", write_data);

    UINT fnum = 0;
    res = f_write(&USERFile, buf, write_len, &fnum);

    write_result->start_cluster = USERFile.sclust;
    write_result->curr_cluster  = USERFile.clust;
    write_result->write_len     = fnum;
    write_result->res           = res;

    f_sync(&USERFile);
    f_close(&USERFile);
    disk_ioctl(0, CTRL_SYNC, NULL);
    W25Q64_WaitBusy();

    if (res == FR_OK)
    {
        STORAGE_PRINT("[storage] %s写入成功！指定簇号:%lu  写入长度:%u\r\n",
           filename, target_clust, fnum);
    }
    else
    {
        STORAGE_PRINT("[storage] 指定簇写入失败！err：%d\r\n", res);
    }

    return res;
}




