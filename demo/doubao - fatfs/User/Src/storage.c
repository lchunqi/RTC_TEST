#include "storage.h"
#include "diskio.h"
#include "string.h"
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>

// 全局FatFs对象
static FATFS fs;
/************************** 基础打印模块（独立，带宏开关） **************************/
#ifdef  ENABLE_STORAGE_PRINT

//#define ENABLE_PRINT_ASCII  1  //打印ascii测试

// 封装存储模块专属打印函数，统一串口输出
void Storage_Print(const char *fmt,...) {
    va_list args;
    char buffer[256];
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    // 复用串口1输出，与其他模块打印逻辑一致
    HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), 100);
}
// 替换原有UsbPrintf，统一为存储模块打印宏
#define Storage_print Storage_Print
#else
// 关闭打印时，空宏屏蔽所有打印语句
#define Storage_print(...)
#endif
/************************** 存储系统核心模块（初始化/格式化） **************************/
// 修改storage.c中的Storage_Init函数
StorageError Storage_Init(void) {
    FRESULT res = FR_OK;
    Storage_print("存储初始化: 开始挂载FatFs文件系统...\n");
    // 先尝试挂载现有文件系统
    res = f_mount(&fs, "", 1);
    if (res == FR_OK) {
        Storage_print("存储初始化: 成功挂载现有文件系统！\n");
        return STORAGE_OK;
    } else if (res == FR_NO_FILESYSTEM) {
        Storage_print("存储初始化: 未检测到文件系统，创建新文件系统...\n");
        // 首次上电无文件系统时创建
        if (Storage_Format() != STORAGE_OK) {
            Storage_print("存储初始化: 格式化失败！\n");
            return STORAGE_ERROR_INIT;
        }
        res = f_mount(&fs, "", 1);
        if (res == FR_OK) {
            Storage_print("存储初始化: 新文件系统创建并挂载成功！\n");
            return STORAGE_OK;
        }
    } else {
        Storage_print("存储初始化: 挂载失败，尝试修复文件系统...\n");
        // 其他错误（如元数据损坏），尝试修复
        if (Storage_Format() != STORAGE_OK) {
            Storage_print("存储初始化: 修复失败！\n");
            return STORAGE_ERROR_INIT;
        }
        res = f_mount(&fs, "", 1);
        if (res == FR_OK) {
            Storage_print("存储初始化: 文件系统修复并挂载成功！\n");
            return STORAGE_OK;
        }
    }

    Storage_print("存储初始化: 挂载最终失败，错误码%u\n", res);
    return STORAGE_ERROR_INIT;
}

StorageError Storage_Format(void) {
    MKFS_PARM parm;
    parm.fmt = FM_FAT;
    parm.n_fat = 2;
    parm.align = 0;
    parm.n_root = 512;
    parm.au_size = 0;
    BYTE workBuffer[4096] = {0};
    Storage_print("存储格式化: 开始格式化FLASH为FAT格式...\n");
    FRESULT res = f_mkfs("", &parm, workBuffer, sizeof(workBuffer));
    if (res != FR_OK) {
        Storage_print("存储格式化: 格式化失败，错误码%u！\n", res);
        return STORAGE_ERROR_INIT;
    }
    Storage_print("存储格式化: FLASH格式化成功！\n");
    return STORAGE_OK;
}


/************************** 校验和计算模块（独立） **************************/
/**
 * @brief 计算数据的异或校验和
 * @param data 待计算的数据缓冲区
 * @param dataSize 数据长度
 * @return 计算得到的校验和
 */
uint8_t calculateChecksum(const uint8_t* data, UINT dataSize) {
    uint8_t checksum = 0;
    // 修改变量i为uint32_t，兼容16/32位，避免循环计数错误
    for (uint32_t i = 0; i < (uint32_t)dataSize; i++) {
        checksum ^= data[i];
    }
    return checksum;
}


/************************** 文件级读写模块（独立，基于FatFs） **************************/
/**
 * @brief 文件写入：向指定文件写入数据（覆盖写），带校验和输出
 * @param path 文件路径（如"test.txt"）
 * @param data 待写入数据缓冲区
 * @param size 写入字节数
 * @param outChecksum 输出参数：写入数据的异或校验和（传入指针，可传NULL忽略）
 * @return 存储操作状态码
 */
// 【修改】文件写入函数：增加f_sync强制刷新缓存，确保数据刷到物理存储
// 修改storage.c中的Storage_WriteFileWithCrc函数（核心新增f_mount同步）
StorageError Storage_WriteFileWithCrc(const char *path, const uint8_t *data, UINT size, uint8_t* outChecksum) {
    if (path == NULL || data == NULL || size == 0) {
        Storage_print("文件写入错误: 无效的输入参数\n");
        return STORAGE_ERROR_FILE;
    }

    FIL file = {0};
    FRESULT res = f_open(&file, path, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        Storage_print("文件写入错误: 打开文件%s失败，错误码%u\n", path, res);
        return STORAGE_ERROR_FILE;
    }

    UINT written = 0;
    res = f_write(&file, data, (UINT)size, &written);
    if (res == FR_OK) {
        f_sync(&file); // 同步文件数据
    }
    f_close(&file);

    // 核心新增：重新挂载后验证文件目录是否存在
    if (res == FR_OK && written == size) {
        f_mount(NULL, "", 1); // 卸载
        f_mount(&fs, "", 1);  // 重新挂载

        // 验证文件是否被目录识别
        FRESULT check_res = f_stat(path, NULL);
        if (check_res == FR_OK) {
            Storage_print("文件写入: 目录验证成功，文件%s已存在\n", path);
        } else if (check_res == FR_DISK_ERR) {
            Storage_print("文件写入: 警告！目录验证失败（磁盘I/O错误），错误码%u\n", check_res);
            return STORAGE_ERROR_FILE; // 写入失败，返回错误
        } else {
            Storage_print("文件写入: 警告！目录验证失败（文件未被识别），错误码%u\n", check_res);
            return STORAGE_ERROR_FILE;
        }
    }

    if (res != FR_OK || written != size) {
        Storage_print("文件写入错误: %s写入失败，错误码%u，实际写入%u/%u字节\n", 
                  path, res, written, size);
        return STORAGE_ERROR_FILE;
    }

    if (outChecksum != NULL) {
        *outChecksum = calculateChecksum(data, written);
    }

    Storage_print("文件写入成功: %s写入%u字节，校验和0x%02X\n", path, written, outChecksum ? *outChecksum : 0x00);
    return STORAGE_OK;
}


// 【新增】获取文件大小函数：独立模块，用于读写一致性校验
UINT Storage_GetFileSize(const char *path) {
    if (path == NULL) return 0;
    FIL file = {0};
    FRESULT res = f_open(&file, path, FA_READ);
    if (res != FR_OK) {
        if (res == FR_DISK_ERR) {
            Storage_print("获取文件大小错误: 磁盘I/O错误（底层FLASH读失败），错误码%u\n", res);
        } else if (res == FR_NO_FILE) {
            Storage_print("获取文件大小错误: 文件%s不存在，错误码%u\n", path, res);
        } else {
            Storage_print("获取文件大小错误: 打开%s失败，错误码%u\n", path, res);
        }
        return 0;
    }
    UINT fileSize = f_size(&file);
    f_close(&file);
    return fileSize;
}

/**
 * @brief 文件读取：从指定文件读取指定长度数据，带校验和输出
 * @param path 文件路径（如"test.txt"）
 * @param data 数据接收缓冲区
 * @param size 期望读取字节数
 * @param outChecksum 输出参数：读取数据的异或校验和（传入指针，可传NULL忽略）
 * @return 存储操作状态码
 */
StorageError Storage_ReadFileWithCrc(const char *path, uint8_t *data, UINT maxSize, uint8_t* outChecksum, UINT* actualReadSize) {
    // 新增actualReadSize输出参数：返回实际读取的字节数
    if (path == NULL || data == NULL || maxSize == 0 || actualReadSize == NULL) {
        Storage_print("文件读取错误: 无效的输入参数\n");
        return STORAGE_ERROR_FILE;
    }
    *actualReadSize = 0; // 初始化实际读取字节数
    FIL file = {0};
    FRESULT res = f_open(&file, path, FA_READ);
    if (res != FR_OK) {
        Storage_print("文件读取错误: 打开文件%s失败，错误码%u\n", path, res);
        return STORAGE_ERROR_FILE;
    }
    UINT read = 0;
    res = f_read(&file, data, (UINT)maxSize, &read); // 强制转换为FatFs的16位UINT
    f_close(&file);
    if (res != FR_OK) {
        Storage_print("文件读取错误: %s读取失败，错误码%u\n", path, res);
        return STORAGE_ERROR_FILE;
    }
    *actualReadSize = read; // 赋值实际读取字节数
    // 计算校验和：仅基于实际读取的有效数据（核心修复）
    if (outChecksum != NULL) {
        *outChecksum = calculateChecksum(data, read);
    }
    Storage_print("文件读取成功: %s读取%u字节，校验和0x%02X\n", path, read, outChecksum ? *outChecksum : 0x00);
    return STORAGE_OK;
}

/************************** 【优化后核心函数】通用FatFs文件数据读取（专用于读出内容） **************************/
/**
 * @brief 通用文件数据读取：稳定读出FatFs已写入的文件原始内容（可读全部/指定长度）
 * @param filePath 待读取文件路径（如"test.txt"，与写入路径完全一致）
 * @param readBuf  数据接收缓冲区（需外部提前分配，确保大小足够，存放读出的内容）
 * @param bufSize  接收缓冲区的最大长度（避免缓冲区溢出）
 * @param actualRead 输出参数：实际读取到的字节数（传入指针，必传，用于获取有效内容长度）
 * @param outCrc  输出参数：读取数据的异或校验和（传入指针，可传NULL忽略）
 * @return StorageError 操作状态码：STORAGE_OK成功，其他为失败
 * @note 1. 自动适配文件实际大小，优先读出全部内容；2. 读出的内容直接存入readBuf，可直接使用
 * @note 3. 开启ENABLE_STORAGE_PRINT后，可打印读出的内容十六进制/ASCII形式，方便调试
 */
StorageError Storage_ReadFileData(const char *filePath, uint8_t *readBuf, UINT bufSize, UINT *actualRead, uint8_t *outCrc)
{
    // 1. 严格参数校验：空指针/缓冲区大小为0直接返回错误
    if (filePath == NULL || readBuf == NULL || actualRead == NULL || bufSize == 0)
    {
        Storage_print("通用文件读取错误: 无效参数（空指针/缓冲区大小为0）\n");
        return STORAGE_ERROR_FILE;
    }
    *actualRead = 0;                // 初始化实际读取字节数，避免脏数据
    memset(readBuf, 0, bufSize);    // 清空接收缓冲区，避免残留数据干扰读出的内容

    // 2. 获取文件实际大小，判断文件是否存在/为空（不存在则无法读出内容）
    UINT fileRealSize = Storage_GetFileSize(filePath);
    if (fileRealSize == 0)
    {
        Storage_print("通用文件读取错误: 文件%s不存在或为空，无内容可读取\n", filePath);
        return STORAGE_ERROR_FILE;
    }
    Storage_print("通用文件读取: 文件%s存在，实际大小%u字节，缓冲区可接收%u字节\n", filePath, fileRealSize, bufSize);

    // 3. 确定实际读取长度：优先读出文件全部内容，缓冲区不足则读满缓冲区
    UINT readLen = (fileRealSize < bufSize) ? fileRealSize : bufSize;
    Storage_print("通用文件读取: 准备读取%u字节内容\n", readLen);

    // 4. 核心：调用底层读取接口，读出文件内容到readBuf
    StorageError ret = Storage_ReadFileWithCrc(filePath, readBuf, readLen, outCrc, actualRead);
    if (ret != STORAGE_OK || *actualRead != readLen)
    {
        Storage_print("通用文件读取错误: 内容读取失败，错误码%u，实际仅读取%u字节\n", ret, *actualRead);
        memset(readBuf, 0, bufSize); // 读取失败清空缓冲区，避免无效数据
        return ret;
    }

    // 5. 读取成功：打印核心信息，可选打印读出的内容（调试用）
    Storage_print("通用文件读取成功: 共读出%u字节有效内容\n", *actualRead);
    if (outCrc != NULL)
    {
        Storage_print("通用文件读取: 读出内容的校验和为0x%02X\n", *outCrc);
    }

    // -------------------------- 可选：打印读出的内容（十六进制+ASCII，方便调试） --------------------------
    #ifdef ENABLE_PRINT_ASCII
    if (*actualRead > 0)
    {
        Storage_print("通用文件读取: 读出的内容（十六进制）：");
        for (UINT i = 0; i < *actualRead; i++)
        {
            Storage_print("%02X ", readBuf[i]);
            // 每16个字节换行，排版更清晰
            if ((i + 1) % 16 == 0 && i != *actualRead - 1)
            {
                Storage_print("\n");
                Storage_print("通用文件读取: ");
            }
        }
        Storage_print("\n");

        Storage_print("通用文件读取: 读出的内容（ASCII）：");
        for (UINT i = 0; i < *actualRead; i++)
        {
            // 打印可显示ASCII字符，不可显示则用.替代
            Storage_print("%c", (readBuf[i] >= 0x20 && readBuf[i] <= 0x7E) ? readBuf[i] : '.');
        }
        Storage_print("\n");
    }
    #endif

    return STORAGE_OK;
}

/************************** 测试模块（独立，验证读写+校验和一致性） **************************/
/**
 * @brief 存储系统读写测试：验证文件级读写和校验和一致性
 * @return 无
 */
void Storage_TestWriteRead(void) {
    const char testFilePath[] = "test.txt";
    const uint8_t testData[] = "This is a test string for FatFs writing and reading";
    const uint32_t writeSize_32 = 51; // 显式32位长度，实际测试数据的准确字节数
    UINT writeSize = (UINT)writeSize_32; // 主动转换为FatFs的16位UINT
    uint8_t writeCrc = 0, readCrc = 0;
    uint8_t readBuffer[256] = {0}; 
    UINT actualReadSize = 0;       
    UINT fileActualSize = 0;
    Storage_print("\n===== 开始存储系统读写测试 =====\n");
    // 1. 写入数据并计算写入校验和
    StorageError writeStatus = Storage_WriteFileWithCrc(testFilePath, testData, writeSize, &writeCrc);
    if (writeStatus != STORAGE_OK) {
        Storage_print("测试失败: 文件写入过程失败\n");
        return;
    }
    // 2. 核心新增：获取文件实际存储大小，校验写入是否和文件大小一致
    fileActualSize = Storage_GetFileSize(testFilePath);
    if (fileActualSize == 0 || fileActualSize != writeSize) {
        Storage_print("测试警告: 写入大小%u字节，文件实际大小%u字节\n", writeSize, fileActualSize);
        // 若文件大小不一致，后续按文件实际大小读取
        writeSize = fileActualSize;
    }
    // 3. 读取数据：按文件实际大小读取，获取实际读取字节数和读取校验和
    StorageError readStatus = Storage_ReadFileWithCrc(testFilePath, readBuffer, writeSize, &readCrc, &actualReadSize);
    if (readStatus != STORAGE_OK) {
        Storage_print("测试失败: 文件读取过程失败\n");
        return;
    }
    // 4. 三重一致性校验：文件大小+实际读取字节数+校验和（核心修复）
    uint8_t dataCmpResult = memcmp(testData, readBuffer, actualReadSize);
    if (fileActualSize == actualReadSize && dataCmpResult == 0 && writeCrc == readCrc) {
        Storage_print("测试成功: 数据内容和校验和完全一致 | 校验和0x%02X\n", writeCrc);
    } else {
        Storage_print("测试失败: 数据不一致 \n");
        Storage_print("  - 文件大小%u | 实际读取%u字节\n", fileActualSize, actualReadSize);
        Storage_print("  - 写入校验和0x%02X | 读取校验和0x%02X\n", writeCrc, readCrc);
        Storage_print("  - 数据内容对比: %s\n", dataCmpResult == 0 ? "一致" : "不一致");
        // 新增：逐字节打印前20字节，查看差异
        Storage_print("  - 原始数据前20字节: ");
        for(uint32_t i=0; i<20; i++) Storage_print("%02X ", testData[i]);
        Storage_print("\n  - 读取数据前20字节: ");
        for(uint32_t i=0; i<20; i++) Storage_print("%02X ", readBuffer[i]);
        Storage_print("\n");
    }
    Storage_print("===== 存储系统读写测试结束 =====\n\n");
}