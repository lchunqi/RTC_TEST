#ifndef __STORAGE_H
#define __STORAGE_H

#include "stm32f1xx_hal.h"
#include "ff.h"

// 存储操作错误码定义
typedef enum {
    STORAGE_OK = 0,        // 操作成功
    STORAGE_ERROR_INIT,    // 初始化失败
    STORAGE_ERROR_WRITE,   // 扇区写入失败
    STORAGE_ERROR_READ,    // 扇区读取失败
    STORAGE_ERROR_FILE     // 文件操作失败
} StorageError;

// 基础模块接口
uint8_t calculateChecksum(const uint8_t* data, UINT dataSize);

// 核心模块接口
StorageError Storage_Init(void);
StorageError Storage_Format(void);

// 文件操作接口
StorageError Storage_WriteFileWithCrc(const char *path, const uint8_t *data, UINT size, uint8_t* outChecksum);
StorageError Storage_ReadFileWithCrc(const char *path, uint8_t *data, UINT maxSize, uint8_t* outChecksum, UINT* actualReadSize);
UINT Storage_GetFileSize(const char *path); // 新增获取文件大小接口


void Storage_TestWriteRead(void);
StorageError Storage_ReadFileData(const char *filePath, uint8_t *readBuf, UINT bufSize, UINT *actualRead, uint8_t *outCrc);


#endif