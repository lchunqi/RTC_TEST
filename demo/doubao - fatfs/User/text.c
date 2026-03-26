
//W25Q64 读写测试函数  
#include "spi.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

// W25Q64 基础宏定义 
#define W25Q64_CS_PIN     GPIO_PIN_4
#define W25Q64_CS_PORT    GPIOA
#define W25Q_SECTOR_SIZE  4096U    // 4KB 扇区大小
#define W25Q_PAGE_SIZE    256U     // 256 字节页大小
#define W25Q64_DEVICE_ID  0xEF4017U// JEDEC ID

// 快捷宏定义
#define CS_LOW()  HAL_GPIO_WritePin(W25Q64_CS_PORT, W25Q64_CS_PIN, GPIO_PIN_RESET)
#define CS_HIGH() HAL_GPIO_WritePin(W25Q64_CS_PORT, W25Q64_CS_PIN, GPIO_PIN_SET)




// 函数声明
void W25Q64_Init(void){}
uint16_t W25Q64_ReadID(void){}

void W25Q64_WritePage(uint32_t addr, const uint8_t *data, uint16_t len){;}


void W25Q64_ChipErase(void){;}
HAL_StatusTypeDef W25Q64_WriteDisable(void){}




/**
 * @brief 读取W25Q64状态寄存器
 * @retval 状态寄存器值
 */
uint8_t W25Q64_ReadStatus(void)
{
    uint8_t status = 0;
    CS_LOW();
    HAL_SPI_Transmit(&hspi1, (uint8_t[]){0x05}, 1, 100); // 读状态寄存器指令
    HAL_SPI_Receive(&hspi1, &status, 1, 100);
    CS_HIGH();
    return status;
}

/**
 * @brief 等待忙标志清零
 */
void W25Q64_WaitBusy(void)
{
    while ((W25Q64_ReadStatus() & 0x01) == 1);
}

/**
 * @brief 写使能
 */
void W25Q64_WriteEnable(void)
{
    CS_LOW();
    HAL_SPI_Transmit(&hspi1, (uint8_t[]){0x06}, 1, 100); // 写使能指令
    CS_HIGH();
}

/**
 * @brief 读取设备ID（9F指令）
 * @retval 设备ID（0xEF4017）
 */
uint32_t W25Q64_ReadDeviceID(void)
{
    uint32_t id = 0;
    uint8_t id_buf[3] = {0};
    CS_LOW();
    HAL_SPI_Transmit(&hspi1, (uint8_t[]){0x9F}, 1, 100); // 读ID指令
    HAL_SPI_Receive(&hspi1, id_buf, 3, 100);
    CS_HIGH();
    id = ((uint32_t)id_buf[0] << 16) | ((uint32_t)id_buf[1] << 8) | id_buf[2];
    return id;
}

/**
 * @brief 扇区擦除（4KB）
 * @param addr 扇区地址（需对齐4KB）
 * @retval 0-成功 1-失败
 */
uint8_t W25Q64_EraseSector(uint32_t addr)
{
    W25Q64_WriteEnable();
    CS_LOW();
    HAL_SPI_Transmit(&hspi1, (uint8_t[]){0x20, (addr>>16)&0xFF, (addr>>8)&0xFF, addr&0xFF}, 4, 100);
    CS_HIGH();
    W25Q64_WaitBusy();
    // 校验擦除结果（读取1字节，应为0xFF）
    uint8_t check_data = 0;
    W25Q64_ReadData(addr, &check_data, 1);
    if (check_data != 0xFF) {
        return 1;
    }
    return 0;
}

/**
 * @brief 页写入（256字节）
 * @param addr 页地址（需对齐256字节）
 * @param data 待写入数据
 * @param len 写入长度（≤256）
 * @retval 0-成功 1-失败
 */
uint8_t W25Q64_PageWrite(uint32_t addr, uint8_t *data, uint16_t len)
{
    if (len > W25Q_PAGE_SIZE || data == NULL) {
        return 1;
    }
    CS_LOW();
    HAL_SPI_Transmit(&hspi1, (uint8_t[]){0x02, (addr>>16)&0xFF, (addr>>8)&0xFF, addr&0xFF}, 4, 100);
    HAL_SPI_Transmit(&hspi1, data, len, len * 10);
    CS_HIGH();
    return 0;
}

/**
 * @brief 连续读取数据
 * @param addr 起始地址
 * @param data 读取缓冲区
 * @param len 读取长度
 */
void W25Q64_ReadData(uint32_t addr, uint8_t *data, uint32_t len)
{
    CS_LOW();
    HAL_SPI_Transmit(&hspi1, (uint8_t[]){0x03, (addr>>16)&0xFF, (addr>>8)&0xFF, addr&0xFF}, 4, 100);
    HAL_SPI_Receive(&hspi1, data, len, len * 10);
    CS_HIGH();
}





/************************ 读写测试核心函数 ************************/
/**
 * @brief W25Q64 基础功能自检（ID读取+写保护检查）
 * @retval 0-成功 1-失败
 */
uint8_t W25Q64_SelfTest(void)
{
    UsbPrintf("\r\n===== W25Q64 基础自检 =====\r\n");
    
    // 1. 读取设备ID
    uint32_t dev_id = W25Q64_ReadDeviceID();
    UsbPrintf("设备ID读取结果：0x%06X (预期：0x%06X)\r\n", dev_id, W25Q64_DEVICE_ID);
    if (dev_id != W25Q64_DEVICE_ID && dev_id != 0xEF16) { // 兼容90指令读取的0xEF16
        UsbPrintf("【错误】设备ID不匹配，硬件连接异常！\r\n");
        return 1;
    }
    
    // 2. 检查写保护状态
    uint8_t status = W25Q64_ReadStatus();
    UsbPrintf("状态寄存器：0x%02X → 忙标志：%d，写保护：%d\r\n", 
               status, status & 0x01, (status >> 1) & 0x01);
    if ((status & 0x02) != 0) {
        UsbPrintf("【警告】写保护已使能！请检查WP引脚或解除软件保护\r\n");
        // 尝试解除写保护
        W25Q64_WriteEnable();
        CS_LOW();
        HAL_SPI_Transmit(&hspi1, (uint8_t[]){0x01, 0x00}, 2, 100); // 写状态寄存器，关闭保护
        CS_HIGH();
        W25Q64_WaitBusy();
        status = W25Q64_ReadStatus();
        UsbPrintf("解除保护后状态寄存器：0x%02X → 写保护：%d\r\n", 
                   status, (status >> 1) & 0x01);
        if ((status & 0x02) != 0) {
            UsbPrintf("【错误】无法解除写保护，无法执行读写测试！\r\n");
            return 1;
        }
    }
    
    UsbPrintf("基础自检通过！\r\n");
    return 0;
}

/**
 * @brief 单字节读写测试（验证最基础的读写功能）
 * @param test_addr 测试地址（建议0x000000）
 * @retval 0-成功 1-失败
 */
uint8_t W25Q64_SingleByteTest(uint32_t test_addr)
{
    UsbPrintf("\r\n===== W25Q64 单字节读写测试 =====\r\n");
    UsbPrintf("测试地址：0x%06X\r\n", test_addr);
    
    // 1. 擦除目标扇区
    UsbPrintf("1. 擦除测试扇区...\r\n");
    if (W25Q64_EraseSector(test_addr) != 0) {
        UsbPrintf("【错误】扇区擦除失败！\r\n");
        return 1;
    }
    W25Q64_WaitBusy();
    
    // 2. 写入单字节数据
    uint8_t write_data = 0x55;
    UsbPrintf("2. 写入单字节数据：0x%02X\r\n", write_data);
    W25Q64_WriteEnable();
    if (W25Q64_PageWrite(test_addr, &write_data, 1) != 0) {
        UsbPrintf("【错误】单字节写入失败！\r\n");
        return 1;
    }
    W25Q64_WaitBusy();
    
    // 3. 读取并校验
    uint8_t read_data = 0;
    W25Q64_ReadData(test_addr, &read_data, 1);
    UsbPrintf("3. 读取数据：0x%02X\r\n", read_data);
    if (read_data != write_data) {
        UsbPrintf("【错误】单字节读写校验失败！\r\n");
        return 1;
    }
    
    UsbPrintf("单字节读写测试通过！\r\n");
    return 0;
}

/**
 * @brief 页写入测试（256字节，验证页内连续写入）
 * @param test_addr 测试地址（建议0x000000，需对齐页地址）
 * @retval 0-成功 1-失败
 */
uint8_t W25Q64_PageWriteTest(uint32_t test_addr)
{
    UsbPrintf("\r\n===== W25Q64 页写入测试（256字节） =====\r\n");
    UsbPrintf("测试地址：0x%06X\r\n", test_addr);
    
    // 1. 擦除目标扇区
    UsbPrintf("1. 擦除测试扇区...\r\n");
    if (W25Q64_EraseSector(test_addr) != 0) {
        UsbPrintf("【错误】扇区擦除失败！\r\n");
        return 1;
    }
    W25Q64_WaitBusy();
    
    // 2. 准备测试数据（0x00~0xFF循环）
    uint8_t write_buf[W25Q_PAGE_SIZE] = {0};
    for (uint16_t i = 0; i < W25Q_PAGE_SIZE; i++) {
        write_buf[i] = i % 256;
    }
    
    // 3. 页写入
    UsbPrintf("2. 写入256字节测试数据...\r\n");
    W25Q64_WriteEnable();
    if (W25Q64_PageWrite(test_addr, write_buf, W25Q_PAGE_SIZE) != 0) {
        UsbPrintf("【错误】页写入失败！\r\n");
        return 1;
    }
    W25Q64_WaitBusy();
    
    // 4. 读取并校验
    UsbPrintf("3. 读取并校验数据...\r\n");
    uint8_t read_buf[W25Q_PAGE_SIZE] = {0};
    W25Q64_ReadData(test_addr, read_buf, W25Q_PAGE_SIZE);
    
    uint8_t err_flag = 0;
    for (uint16_t i = 0; i < W25Q_PAGE_SIZE; i++) {
        if (read_buf[i] != write_buf[i]) {
            UsbPrintf("【错误】地址0x%06X，预期0x%02X，实际0x%02X\r\n", 
                       test_addr + i, write_buf[i], read_buf[i]);
            err_flag = 1;
            break;
        }
    }
    
    if (err_flag == 0) {
        UsbPrintf("页写入测试通过！256字节数据校验无误\r\n");
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief 大容量连续读写测试（4KB扇区，验证跨页/跨扇区读写）
 * @param test_addr 测试地址（建议0x001000）
 * @retval 0-成功 1-失败
 */
uint8_t W25Q64_SectorWriteTest(uint32_t test_addr)
{
    UsbPrintf("\r\n===== W25Q64 扇区读写测试（4KB） =====\r\n");
    UsbPrintf("测试地址：0x%06X\r\n", test_addr);
    
    // 1. 擦除目标扇区
    UsbPrintf("1. 擦除4KB测试扇区...\r\n");
    if (W25Q64_EraseSector(test_addr) != 0) {
        UsbPrintf("【错误】扇区擦除失败！\r\n");
        return 1;
    }
    W25Q64_WaitBusy();
    
    // 2. 准备4KB测试数据（0xAA填充）
    uint8_t write_buf[W25Q_SECTOR_SIZE] = {0};
    memset(write_buf, 0xAA, W25Q_SECTOR_SIZE);
    // 加入特征值，方便校验
    write_buf[1024] = 0x55;
    write_buf[2048] = 0x66;
    write_buf[3072] = 0x77;
    
    // 3. 分批次写入（跨页）
    UsbPrintf("2. 写入4KB测试数据...\r\n");
    uint32_t write_addr = test_addr;
    uint16_t remain_len = W25Q_SECTOR_SIZE;
    while (remain_len > 0) {
        uint16_t write_len = remain_len > W25Q_PAGE_SIZE ? W25Q_PAGE_SIZE : remain_len;
        W25Q64_WriteEnable();
        if (W25Q64_PageWrite(write_addr, write_buf + (write_addr - test_addr), write_len) != 0) {
            UsbPrintf("【错误】地址0x%06X写入失败！\r\n", write_addr);
            return 1;
        }
        W25Q64_WaitBusy();
        write_addr += write_len;
        remain_len -= write_len;
    }
    
    // 4. 读取并校验
    UsbPrintf("3. 读取4KB数据并校验...\r\n");
    uint8_t read_buf[W25Q_SECTOR_SIZE] = {0};
    W25Q64_ReadData(test_addr, read_buf, W25Q_SECTOR_SIZE);
    
    uint8_t err_flag = 0;
    for (uint32_t i = 0; i < W25Q_SECTOR_SIZE; i++) {
        if (read_buf[i] != write_buf[i]) {
            UsbPrintf("【错误】地址0x%06X，预期0x%02X，实际0x%02X\r\n", 
                       test_addr + i, write_buf[i], read_buf[i]);
            err_flag = 1;
            break;
        }
    }
    
    if (err_flag == 0) {
        UsbPrintf("扇区读写测试通过！4KB数据校验无误\r\n");
        // 打印特征值，直观验证
        UsbPrintf("特征值校验：0x%02X 0x%02X 0x%02X 0x%02X（预期：0xAA 0x55 0x66 0x77）\r\n",
                   read_buf[1024], read_buf[2048], read_buf[3072], read_buf[4095]);
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief W25Q64 全功能测试入口函数
 * @retval 0-全部通过 1-有失败项
 */
uint8_t W25Q64_FullTest(void)
{
    UsbPrintf("\r\n====================================\r\n");
    UsbPrintf("        W25Q64 全功能读写测试        \r\n");
    UsbPrintf("====================================\r\n");
    
    // 1. 基础自检
    if (W25Q64_SelfTest() != 0) {
        UsbPrintf("\r\n【最终结果】基础自检失败！\r\n");
        return 1;
    }
    
    // 2. 单字节测试（地址0x000000）
    if (W25Q64_SingleByteTest(0x000000) != 0) {
        UsbPrintf("\r\n【最终结果】单字节测试失败！\r\n");
        return 1;
    }
    
    // 3. 页写入测试（地址0x000100）
    if (W25Q64_PageWriteTest(0x000100) != 0) {
        UsbPrintf("\r\n【最终结果】页写入测试失败！\r\n");
        return 1;
    }
    
    // 4. 扇区读写测试（地址0x001000）
    if (W25Q64_SectorWriteTest(0x001000) != 0) {
        UsbPrintf("\r\n【最终结果】扇区读写测试失败！\r\n");
        return 1;
    }
    
    UsbPrintf("\r\n====================================\r\n");
    UsbPrintf("        所有测试项全部通过！        \r\n");
    UsbPrintf("====================================\r\n");
    return 0;
}



/************************ 基础硬件操作函数（需确保已实现） .h  ************************/
// 读取状态寄存器（必须实现）
uint8_t W25Q64_ReadStatus(void);
// 等待忙标志清零（必须实现）
void W25Q64_WaitBusy(void);
// 写使能（必须实现）
void W25Q64_WriteEnable(void);
// 读取设备ID（必须实现）
uint32_t W25Q64_ReadDeviceID(void);
// 扇区擦除（必须实现）
uint8_t W25Q64_EraseSector(uint32_t addr);
// 页写入（必须实现）
uint8_t W25Q64_PageWrite(uint32_t addr, uint8_t *data, uint16_t len);
// 连续读取（必须实现）
void W25Q64_ReadData(uint32_t addr, uint8_t *data, uint32_t len);
uint8_t W25Q64_FullTest(void);






