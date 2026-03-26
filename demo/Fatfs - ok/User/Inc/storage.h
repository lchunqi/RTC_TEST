
#ifndef __STORAGE_H
#define __STORAGE_H

#include "fatfs.h"
#include <stdint.h>

// 外部全局变量
extern FATFS   USERFatFS;
extern FIL     USERFile;
extern UINT    fnum;


// 容量单位
#define KB      (1024UL)
#define MB      (1024UL * 1024UL)

// Flash 关键宏（与 W25Q64 + FatFs 完全对齐）
#define FLASH_SECTOR_SIZE        W25Q64_LOGIC_SECTOR_SIZE
#define FLASH_TOTAL_SECTOR       W25Q64_TOTAL_LOGIC_SECTOR
#define FLASH_TOTAL_BYTES        (FLASH_SECTOR_SIZE * FLASH_TOTAL_SECTOR)
#define DATA_AREA_START_SECTOR   4

// FATFS 基础信息结构体
typedef struct
{
    uint8_t     mount_status;    // 挂载状态：0-未挂载，1-已挂载
    uint8_t     fs_type;         // 文件系统类型：FS_FAT12/FS_FAT16/FS_FAT32
    uint32_t    sector_size;     // 扇区大小（字节）
    uint32_t    cluster_size;    // 簇大小（字节）
    uint32_t    total_clusters;  // 总数据簇数
    uint32_t    total_bytes;     // 总容量（字节）
    uint32_t    free_bytes;      // 空闲容量（字节）
    uint32_t    used_bytes;      // 已用容量（字节）
    uint32_t    last_cluster;    // 最后一个可用数据簇号

} FatFs_Info_t;

// 文件读取结果结构体
typedef struct
{
    FRESULT     res;             // 操作结果码
    uint32_t    file_size;       // 文件总大小（字节）
    uint32_t    start_cluster;   // 文件起始簇号
    uint32_t    phy_start_addr;  // 物理起始地址
    uint32_t    phy_end_addr;    // 物理结束地址
    UINT        read_len;        // 实际读取字节数
    char        content[4096];   // 读取内容缓冲区

} File_Read_Result_t;

// 文件写入结果结构体
typedef struct
{
    FRESULT     res;             // 操作结果码
    UINT        write_len;       // 实际写入字节数
    uint32_t    start_cluster;   // 文件起始簇号
    uint32_t    curr_cluster;    // 当前写入簇号

} File_Write_Result_t;

// 函数声明
uint8_t  FatFs_ParseReadCmd(const char* command, char* filename, uint16_t filename_len);
uint8_t  FatFs_GetInfo(FatFs_Info_t* fs_info);
uint8_t  FatFs_ReadFile(const char* filename, File_Read_Result_t* read_result, const FatFs_Info_t* fs_info);
void     FatFs_PrintReadResult(const char* filename, const FatFs_Info_t* fs_info, const File_Read_Result_t* read_result);
FRESULT  FatFs_FileWrite(const char* filename, const char* write_data, UINT write_len, File_Write_Result_t* write_result);
FRESULT  FatFs_FileWrite_Cluster(const char* filename, DWORD cluster, const char* write_data, UINT write_len, File_Write_Result_t* write_result);
void     FatFs_WriteUart2ByTimeStamp(void);

#endif /* __STORAGE_H */