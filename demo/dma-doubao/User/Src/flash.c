#include "flash.h"
#include "main.h"
#include "spi.h"
#include "usart.h"

// 全局SPI句柄（对接HAL库）
extern SPI_HandleTypeDef hspi1;

/************************ SPI底层通信函数 ************************/
static uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{
    uint8_t RxData = 0;
    HAL_SPI_TransmitReceive(&hspi1, &TxData, &RxData, 1, 100);
    return RxData;
}

/************************ W25Q64基础操作函数 ************************/
// 等待Flash忙状态结束
uint8_t CheckBusy(void)
{
    uint8_t status = 0xFF;
    uint8_t cmd = W25Q_ReadStatusReg1; // 定义uint8_t类型的指令变量
    CS_LOW;
    // 传入uint8_t变量的地址，匹配const uint8_t*类型要求
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY); 
    HAL_SPI_Receive(&hspi1, &status, 1, HAL_MAX_DELAY);
    CS_HIGH;
    return status;
}



// 写使能
static void W25Qxx_WriteEnable(void)
{
    CS_LOW;
    SPI1_ReadWriteByte(W25Q_WriteEnable);
    CS_HIGH;
    HAL_Delay(1); // 确保写使能指令生效
}

// 读取W25Q64设备ID
uint32_t flash_device_id(void)
{
    uint32_t id = 0;
    CS_LOW;
    SPI1_ReadWriteByte(W25Q_ReadID);
    id |= (uint16_t)SPI1_ReadWriteByte(0xFF) << 16; // 厂家ID 0xEF
    id |= (uint16_t)SPI1_ReadWriteByte(0xFF) << 8;  // 厂家ID 0x40
    id |= (uint16_t)SPI1_ReadWriteByte(0xFF);       // 设备ID 0x17
    CS_HIGH;
    return id;
}


// 硬件页编程（单次最大256字节，必须256字节对齐）
// 扇区擦除，addr：扇区起始地址（必须是4KB整数倍），返回1=成功，0=失败
uint8_t flash_erase_sector(uint32_t addr)
{
    if(addr % W25Q_SECTOR_SIZE != 0) // 地址必须对齐
        return 0;
    W25Qxx_WriteEnable(); // 擦除前必须写使能
    CS_LOW;
    // 发送擦除指令+3字节地址
    uint8_t cmd[4] = {W25Q_CMD_SECTOR_ER, (addr>>16)&0xFF, (addr>>8)&0xFF, addr&0xFF};
    HAL_SPI_Transmit(&hspi1, cmd, 4, 200);
    CS_HIGH;
    while(CheckBusy() & 0x01); // 等待擦除完成（擦除耗时约0.5s）
    return 1;
}

// 通用读数据（支持任意长度、任意地址，硬件自动地址自增）
// 读数据，addr：起始地址，buf：数据缓冲区，len：读取长度，返回1=成功
uint8_t flash_read_data(uint32_t addr, uint8_t* buf, uint32_t len)
{
    if(buf == NULL || len == 0)
        return 0;
    CS_LOW;
    // 发送读指令+3字节地址
    uint8_t cmd[4] = {W25Q_CMD_READ, (addr>>16)&0xFF, (addr>>8)&0xFF, addr&0xFF};
    HAL_SPI_Transmit(&hspi1, cmd, 4, 100);
    // 连续读取数据（硬件自增，支持跨页/跨扇区）
    HAL_SPI_Receive(&hspi1, buf, len, len*10); // 超时随长度调整
    CS_HIGH;
    return 1;
}

// 通用写数据（核心：自动分块，支持任意长度，跨页/跨扇区软件自增）
// 写数据，addr：起始地址，buf：待写入数据，len：写入长度，返回1=成功
// 1. Flash写函数中while循环修复
uint8_t flash_write_data(uint32_t addr, uint8_t* buf, uint32_t len)
{
    if(buf == NULL || len == 0)
        return 0;
    uint32_t write_addr = addr;
    uint32_t remain_len = len;
    uint32_t page_left = W25Q_PAGE_SIZE - (write_addr % W25Q_PAGE_SIZE);
    
    while(remain_len > 0)
    {
        uint32_t write_len = remain_len > page_left ? page_left : remain_len;
        W25Qxx_WriteEnable(); 
        CS_LOW;
        uint8_t cmd[4] = {W25Q_CMD_PAGE_PROG, (write_addr>>16)&0xFF, (write_addr>>8)&0xFF, write_addr&0xFF};
        HAL_SPI_Transmit(&hspi1, cmd, 4, 100);
        HAL_SPI_Transmit(&hspi1, buf + (write_addr - addr), write_len, write_len*10);
        CS_HIGH;
        
        // 修复：移除分号，添加显式空大括号
        while(CheckBusy() & 0x01) { } 
        
        remain_len -= write_len;
        write_addr += write_len;
        page_left = W25Q_PAGE_SIZE;
    }
    return 1;
}









