#include "flash.h"
#include "spi.h"
#include "usart.h"

// 核心宏定义（必须）
#define SPI_TIMEOUT_MS          1000        // 统一超时
#define WAIT_BUSY_TIMEOUT_MS    50000       // 忙等待超时

// 日志打印宏
#define FLASH_DEBUG_ENABLE      0
#if FLASH_DEBUG_ENABLE
#define FLASH_LOG(fmt, ...)  UsbPrintf(fmt, ##__VA_ARGS__)
#else
#define FLASH_LOG(fmt, ...)  do {} while(0)
#endif


/**
 * @brief  读取状态寄存器
 * @retval 状态值
 */
uint8_t W25Q64_ReadStatus(void)
{
    uint8_t cmd = 0x05;
    uint8_t status = 0;

    CS_LOW();
    if (HAL_SPI_Transmit(&hspi1, &cmd, 1, SPI_TIMEOUT_MS) != HAL_OK)
    {
        CS_HIGH();
        return 0xFF;
    }
    if (HAL_SPI_Receive(&hspi1, &status, 1, SPI_TIMEOUT_MS) != HAL_OK)
    {
        CS_HIGH();
        return 0xFF;
    }
    CS_HIGH();
    return status;
}

/**
 * @brief  等待 Flash 空闲（带超时，永不死机）
 * @retval 0=成功 1=超时
 */
uint8_t W25Q64_WaitBusy(void)
{
    uint32_t timeout = WAIT_BUSY_TIMEOUT_MS;
    while ((W25Q64_ReadStatus() & 0x01) && timeout--)
    {
        HAL_Delay(1);
    }
    return (timeout == 0) ? 1 : 0;
}

/**
 * @brief  写使能
 * @retval 0=成功 1=失败
 */
uint8_t W25Q64_WriteEnable(void)
{
    uint8_t cmd = 0x06;
    CS_LOW();
    if (HAL_SPI_Transmit(&hspi1, &cmd, 1, SPI_TIMEOUT_MS) != HAL_OK)
    {
        CS_HIGH();
        return 1;
    }
    CS_HIGH();
    return 0;
}

/**
 * @brief  写失能
 * @retval 0=成功 1=失败
 */
uint8_t W25Q64_WriteDisable(void)
{
    uint8_t cmd = 0x04;
    CS_LOW();
    if (HAL_SPI_Transmit(&hspi1, &cmd, 1, SPI_TIMEOUT_MS) != HAL_OK)
    {
        CS_HIGH();
        return 1;
    }
    CS_HIGH();
    return 0;
}

/**
 * @brief  读取设备 ID
 * @retval ID(0xEF4017)
 */
uint32_t W25Q64_ReadDeviceID(void)
{
    uint8_t cmd = 0x9F;
    uint8_t id_buf[3] = {0};
    uint32_t id = 0;

    CS_LOW();
    if (HAL_SPI_Transmit(&hspi1, &cmd, 1, SPI_TIMEOUT_MS) != HAL_OK)
    {
        CS_HIGH();
        return 0xFFFFFFFF;
    }
    if (HAL_SPI_Receive(&hspi1, id_buf, 3, SPI_TIMEOUT_MS) != HAL_OK)
    {
        CS_HIGH();
        return 0xFFFFFFFF;
    }
    CS_HIGH();

    id = ((uint32_t)id_buf[0] << 16) | ((uint32_t)id_buf[1] << 8) | id_buf[2];
    return id;
}

/**
 * @brief  扇区擦除 4KB
 * @param  addr 24位地址
 * @retval 0=成功 1=失败
 */
uint8_t W25Q64_EraseSector(uint32_t addr)
{
    uint8_t cmd[4] = {0x20, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};
    uint8_t check_data;

    // 地址越界保护
    if (addr > W25Q64_MAX_ADDR)
    {
        FLASH_LOG("[flash] 地址越界\r\n");
        return 1;
    }

    if (W25Q64_WriteEnable() != 0) return 1;

    CS_LOW();
    if (HAL_SPI_Transmit(&hspi1, cmd, 4, SPI_TIMEOUT_MS) != HAL_OK)
    {
        CS_HIGH();
        W25Q64_WriteDisable();
        return 1;
    }
    CS_HIGH();

    // 等待完成
    if (W25Q64_WaitBusy() != 0)
    {
        FLASH_LOG("[flash] 擦除超时\r\n");
        W25Q64_WriteDisable();
        return 1;
    }

    // 擦除校验
    W25Q64_ReadData(addr, &check_data, 1);
    if (check_data != 0xFF)
    {
        FLASH_LOG("[flash] 擦除校验失败\r\n");
        W25Q64_WriteDisable();
        return 1;
    }

    W25Q64_WriteDisable();
    FLASH_LOG("[flash] 扇区擦除成功\r\n");
    return 0;
}

/**
 * @brief  页写入（≤256字节）
 * @param  addr 页对齐地址
 * @param  data 数据指针
 * @param  len  长度
 * @retval 0=成功 1=失败
 */
uint8_t W25Q64_PageWrite(uint32_t addr, const uint8_t *data, uint16_t len)
{
    uint8_t cmd[4] = {0x02, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};

    // 严格参数校验
    if (data == NULL || len == 0 || len > W25Q64_PAGE_SIZE || addr > W25Q64_MAX_ADDR)
    {
        FLASH_LOG("[flash] 页写入参数错误\r\n");
        return 1;
    }

    if (W25Q64_WriteEnable() != 0) return 1;

    CS_LOW();
    if (HAL_SPI_Transmit(&hspi1, cmd, 4, SPI_TIMEOUT_MS) != HAL_OK)
    {
        CS_HIGH();
        W25Q64_WriteDisable();
        return 1;
    }
    if (HAL_SPI_Transmit(&hspi1, (uint8_t *)data, len, SPI_TIMEOUT_MS) != HAL_OK)
    {
        CS_HIGH();
        W25Q64_WriteDisable();
        return 1;
    }
    CS_HIGH();

    if (W25Q64_WaitBusy() != 0)
    {
        FLASH_LOG("[flash] 写入超时\r\n");
        W25Q64_WriteDisable();
        return 1;
    }

    W25Q64_WriteDisable();
    FLASH_LOG("[flash] 页写入成功\r\n");
    return 0;
}

/**
 * @brief  任意长度读取
 * @param  addr 起始地址
 * @param  data 接收缓冲区
 * @param  len  长度
 * @retval 0=成功 1=失败
 */
uint8_t W25Q64_ReadData(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint8_t cmd[4] = {0x03, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};

    // 空指针 + 越界保护
    if (data == NULL || len == 0 || (addr + len - 1) > W25Q64_MAX_ADDR)
    {
        FLASH_LOG("[flash] 读取参数错误\r\n");
        return 1;
    }

    CS_LOW();
    if (HAL_SPI_Transmit(&hspi1, cmd, 4, SPI_TIMEOUT_MS) != HAL_OK)
    {
        CS_HIGH();
        return 1;
    }
    if (HAL_SPI_Receive(&hspi1, data, len, SPI_TIMEOUT_MS) != HAL_OK)
    {
        CS_HIGH();
        return 1;
    }
    CS_HIGH();

    FLASH_LOG("[flash] 读取成功\r\n");
    return 0;
}

/**
 * @brief  整片擦除
 * @retval 0=成功 1=失败
 */
uint8_t W25Q64_ChipErase(void)
{
    uint8_t cmd = 0xC7;
    uint8_t check;

    FLASH_LOG("[flash] 开始整片擦除\r\n");

    if (W25Q64_WriteEnable() != 0) return 1;

    CS_LOW();
    if (HAL_SPI_Transmit(&hspi1, &cmd, 1, SPI_TIMEOUT_MS) != HAL_OK)
    {
        CS_HIGH();
        W25Q64_WriteDisable();
        return 1;
    }
    CS_HIGH();

    if (W25Q64_WaitBusy() != 0)
    {
        FLASH_LOG("[flash] 整片擦除超时\r\n");
        W25Q64_WriteDisable();
        return 1;
    }

    // 校验
    W25Q64_ReadData(0x000000, &check, 1);
    if (check != 0xFF)
    {
        FLASH_LOG("[flash] 整片擦除校验失败\r\n");
        W25Q64_WriteDisable();
        return 1;
    }

    W25Q64_WriteDisable();
    FLASH_LOG("[flash] 整片擦除成功\r\n");
    return 0;
}

/**
 * @brief  基础自检
 */
uint8_t W25Q64_SelfTest(void)
{
    uint32_t dev_id = W25Q64_ReadDeviceID();
    uint8_t status;

    FLASH_LOG("\r\n===== W25Q64 自检 =====\r\n");
    FLASH_LOG("设备ID: 0x%06X\r\n", dev_id);

    if (dev_id != W25Q64_DEVICE_ID && dev_id != 0xEF16)
    {
        FLASH_LOG("[flash] ID 不匹配\r\n");
        return 1;
    }

    status = W25Q64_ReadStatus();
    if (status & 0x02)
    {
        FLASH_LOG("[flash] 解除写保护...\r\n");
        W25Q64_WriteEnable();
        CS_LOW();
        uint8_t wr_cmd[2] = {0x01, 0x00};
        HAL_SPI_Transmit(&hspi1, wr_cmd, 2, SPI_TIMEOUT_MS);
        CS_HIGH();
        W25Q64_WaitBusy();
        W25Q64_WriteDisable();
    }

    FLASH_LOG("[flash] 自检通过\r\n");
    return 0;
}

/**
 * @brief  单字节测试
 */
uint8_t W25Q64_SingleByteTest(uint32_t test_addr)
{
    uint8_t w_data = 0x55;
    uint8_t r_data;

    FLASH_LOG("\r\n===== 单字节测试 =====\r\n");

    if (W25Q64_EraseSector(test_addr) != 0) return 1;
    if (W25Q64_PageWrite(test_addr, &w_data, 1) != 0) return 1;
    W25Q64_ReadData(test_addr, &r_data, 1);

    if (r_data != w_data)
    {
        FLASH_LOG("[flash] 单字节校验失败\r\n");
        return 1;
    }

    FLASH_LOG("[flash] 单字节测试通过\r\n");
    return 0;
}

/**
 * @brief  页写入测试
 */
uint8_t W25Q64_PageWriteTest(uint32_t test_addr)
{
    uint8_t w_buf[W25Q64_PAGE_SIZE];
    uint8_t r_buf[W25Q64_PAGE_SIZE];

    FLASH_LOG("\r\n===== 页测试 =====\r\n");

    if (W25Q64_EraseSector(test_addr) != 0) return 1;

    for (int i = 0; i < W25Q64_PAGE_SIZE; i++)
        w_buf[i] = i % 256;

    if (W25Q64_PageWrite(test_addr, w_buf, W25Q64_PAGE_SIZE) != 0) return 1;
    W25Q64_ReadData(test_addr, r_buf, W25Q64_PAGE_SIZE);

    for (int i = 0; i < W25Q64_PAGE_SIZE; i++)
    {
        if (r_buf[i] != w_buf[i])
        {
            FLASH_LOG("[flash] 页校验失败\r\n");
            return 1;
        }
    }

    FLASH_LOG("[flash] 页测试通过\r\n");
    return 0;
}

/**
 * @brief  扇区测试
 */
uint8_t W25Q64_SectorWriteTest(uint32_t test_addr)
{
    uint8_t w_buf[W25Q64_SECTOR_SIZE];
    uint8_t r_buf[4];

    FLASH_LOG("\r\n===== 扇区测试 =====\r\n");

    if (W25Q64_EraseSector(test_addr) != 0) return 1;
    memset(w_buf, 0xAA, W25Q64_SECTOR_SIZE);
    w_buf[1024] = 0x55;
    w_buf[2048] = 0x66;
    w_buf[3072] = 0x77;

    uint32_t addr = test_addr;
    uint16_t remain = W25Q64_SECTOR_SIZE;
    while (remain > 0)
    {
        uint16_t l = (remain > W25Q64_PAGE_SIZE) ? W25Q64_PAGE_SIZE : remain;
        if (W25Q64_PageWrite(addr, w_buf + (addr - test_addr), l) != 0)
            return 1;
        addr += l;
        remain -= l;
    }

    W25Q64_ReadData(test_addr + 1024, &r_buf[0], 1);
    W25Q64_ReadData(test_addr + 2048, &r_buf[1], 1);
    W25Q64_ReadData(test_addr + 3072, &r_buf[2], 1);
    W25Q64_ReadData(test_addr + 4095, &r_buf[3], 1);

    if (r_buf[0] != 0x55 || r_buf[1] != 0x66 || r_buf[2] != 0x77 || r_buf[3] != 0xAA)
    {
        FLASH_LOG("[flash] 扇区校验失败\r\n");
        return 1;
    }

    FLASH_LOG("[flash] 扇区测试通过\r\n");
    return 0;
}

/**
 * @brief  全功能测试入口
 */
uint8_t W25Q64_FullTest(void)
{
    UsbPrintf("\r\n====================================\r\n");
    UsbPrintf("        W25Q64 全功能测试        \r\n");
    UsbPrintf("====================================\r\n");

    if (W25Q64_SelfTest() != 0) return 1;
    if (W25Q64_SingleByteTest(0x000000) != 0) return 1;
    if (W25Q64_PageWriteTest(0x000100) != 0) return 1;
    if (W25Q64_SectorWriteTest(0x001000) != 0) return 1;

    UsbPrintf("\r\n====================================\r\n");
    UsbPrintf("        所有测试全部通过！        \r\n");
    UsbPrintf("====================================\r\n");
    return 0;
}