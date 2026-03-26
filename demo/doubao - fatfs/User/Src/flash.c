#include "flash.h"
#include "spi.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef ENABLE_FLASH_PRINT
void Flash_Print(const char *fmt,...) {
    va_list args;
    char buffer[256];
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), 100);
}
#else
#define Flash_Print(...)
#endif



void W25Q64_Init(void) {
    MX_SPI1_Init();
    W25Q64_CS_HIGH();
    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64: SPI初始化完成\n");
    #endif
    // 初始化后默认执行写失能，确保芯片处于安全状态
    W25Q64_WriteDisable();
}

uint16_t W25Q64_ReadID(void) {
    uint8_t cmd[] = {0x90, 0x00, 0x00, 0x00};
    uint8_t id[2] = {0};
    HAL_StatusTypeDef status;

    W25Q64_CS_LOW();
    status = HAL_SPI_Transmit(&W25Q64_SPI_HANDLE, cmd, 4, 100);
    if (status != HAL_OK) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_ReadID: SPI发送指令失败，状态: %d\n", status);
        #endif
        W25Q64_CS_HIGH();
        return 0;
    }

    status = HAL_SPI_Receive(&W25Q64_SPI_HANDLE, id, 2, 100);
    if (status != HAL_OK) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_ReadID: SPI接收ID失败，状态: %d\n", status);
        #endif
        W25Q64_CS_HIGH();
        return 0;
    }
    W25Q64_CS_HIGH();

    uint16_t flash_id = (id[0] << 8) | id[1];
    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64_ReadID: 读取到ID = 0x%04X（预期0x%04X）\n", flash_id, W25Q64_EXPECTED_ID);
    #endif

    if (flash_id != W25Q64_EXPECTED_ID) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_ReadID: 警告！ID不匹配，可能是芯片型号错误或硬件连接异常\n");
        #endif
    }

    return flash_id;
}

// 【新增核心函数】W25Q64写使能：允许后续擦除/写入操作
HAL_StatusTypeDef W25Q64_WriteEnable(void) {
    uint8_t cmd = W25Q64_WRITE_ENABLE_CMD;
    HAL_StatusTypeDef status;

    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64_WriteEnable: 发送写使能指令（0x%02X）\n", cmd);
    #endif

    W25Q64_CS_LOW();
    status = HAL_SPI_Transmit(&W25Q64_SPI_HANDLE, &cmd, 1, 100);
    W25Q64_CS_HIGH();

    if (status == HAL_OK) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_WriteEnable: 写使能成功\n");
        #endif
        HAL_Delay(1); // 确保指令生效
    } else {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_WriteEnable: 写使能失败，SPI状态: %d\n", status);
        #endif
    }

    return status;
}

// 【新增核心函数】W25Q64写失能：禁止擦除/写入操作，模拟写保护
HAL_StatusTypeDef W25Q64_WriteDisable(void) {
    uint8_t cmd = W25Q64_WRITE_DISABLE_CMD;
    HAL_StatusTypeDef status;

    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64_WriteDisable: 发送写失能指令（0x%02X）\n", cmd);
    #endif

    W25Q64_CS_LOW();
    status = HAL_SPI_Transmit(&W25Q64_SPI_HANDLE, &cmd, 1, 100);
    W25Q64_CS_HIGH();

    if (status == HAL_OK) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_WriteDisable: 写失能成功\n");
        #endif
        HAL_Delay(1); // 确保指令生效
    } else {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_WriteDisable: 写失能失败，SPI状态: %d\n", status);
        #endif
    }

    return status;
}

void W25Q64_ReadData(uint32_t addr, uint8_t *data, uint32_t len) {
    if (data == NULL || len == 0) {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_ReadData: 无效参数（空指针/长度0）\n");
        #endif
        return;
    }

    if (addr + len > W25Q64_TOTAL_SIZE) {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_ReadData: 地址越界！地址0x%08X+长度%lu > 总容量0x%08X\n", addr, len, W25Q64_TOTAL_SIZE);
        #endif
        return;
    }

    uint8_t cmd[] = {0x03, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)addr};
    HAL_StatusTypeDef status;

    W25Q64_CS_LOW();
    status = HAL_SPI_Transmit(&W25Q64_SPI_HANDLE, cmd, 4, 100);
    if (status != HAL_OK) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_ReadData: 发送读指令失败，地址0x%08X，状态: %d\n", addr, status);
        #endif
        W25Q64_CS_HIGH();
        return;
    }

    memset(data, 0x00, len);
    status = HAL_SPI_Receive(&W25Q64_SPI_HANDLE, data, len, 100);
    if (status != HAL_OK) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_ReadData: 接收数据失败，地址0x%08X，长度%lu，状态: %d\n", addr, len, status);
        #endif
        W25Q64_CS_HIGH();
        return;
    }
    W25Q64_CS_HIGH();

    uint8_t all_zero = 1;
    for (uint32_t i = 0; i < len && i < 8; i++) {
        if (data[i] != 0x00) {
            all_zero = 0;
            break;
        }
    }
    if (all_zero && len > 0) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_ReadData: 警告！读取数据前8字节全为0，可能SPI通信异常\n");
        #endif
    }
}

void W25Q64_WritePage(uint32_t addr, const uint8_t *data, uint16_t len) {
    uint8_t status_reg = 0xFF;
    for (uint8_t i=0; i<3; i++) {
        status_reg = W25Q64_ReadStatus();
        // 核心修改：允许状态0x03（擦除后忙+误判写保护）
        if (status_reg != 0x02 && status_reg != 0xFF) {
            break;
        }
        HAL_Delay(1);
    }

    if (status_reg == 0x02) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_WritePage: 硬件写保护已使能（稳定状态），取消写入操作\n");
        #endif
        return;
    }
    if (status_reg == 0xFF) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_WritePage: SPI通信失败，取消写入操作\n");
        #endif
        return;
    }

    if (data == NULL || len == 0 || len > W25Q64_PAGE_SIZE) {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_WritePage: 无效参数（空指针/长度0/超过单页最大%u字节）\n", W25Q64_PAGE_SIZE);
        #endif
        return;
    }
    if (addr >= W25Q64_TOTAL_SIZE) {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_WritePage: 地址越界！地址0x%08X >= 总容量0x%08X\n", addr, W25Q64_TOTAL_SIZE);
        #endif
        return;
    }

    uint8_t write_cmd[] = {0x02, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)addr};
    HAL_StatusTypeDef status_spi;

    // 步骤1：写使能
    status_spi = W25Q64_WriteEnable();
    if (status_spi != HAL_OK) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_WritePage: 写使能失败，取消写入操作\n");
        #endif
        return;
    }

    // 步骤2：发送写指令+地址
    W25Q64_CS_LOW();
    status_spi = HAL_SPI_Transmit(&W25Q64_SPI_HANDLE, write_cmd, 4, 100);
    if (status_spi != HAL_OK) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_WritePage: 发送写指令失败，地址0x%08X，状态: %d\n", addr, status_spi);
        #endif
        W25Q64_CS_HIGH();
        W25Q64_WriteDisable(); // 异常时执行写失能
        return;
    }

    // 步骤3：发送数据
    status_spi = HAL_SPI_Transmit(&W25Q64_SPI_HANDLE, (uint8_t*)data, len, 100);
    if (status_spi != HAL_OK) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_WritePage: 发送数据失败，地址0x%08X，长度%u，状态: %d\n", addr, len, status_spi);
        #endif
        W25Q64_CS_HIGH();
        W25Q64_WriteDisable(); // 异常时执行写失能
        return;
    }
    W25Q64_CS_HIGH();

    // 步骤4：等待写入完成
    uint32_t timeout = 1000;
    #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_WritePage: 等待写入完成（适配特殊状态）...\n");
    #endif
    while (1) {
        uint8_t curr_status = W25Q64_ReadStatus();
        if ((curr_status & 0x01) == 0) { // 忙状态结束
            break;
        }
        if (timeout-- == 0) {
            #ifdef ENABLE_FLASH_PRINT
                Flash_Print("W25Q64_WritePage: 写操作超时（1000ms）\n");
            #endif
            W25Q64_WriteDisable();
            return;
        }
        HAL_Delay(1);
    }

    // 步骤5：写后验证
    uint8_t verify_buf[W25Q64_PAGE_SIZE] = {0};
    W25Q64_ReadData(addr, verify_buf, len);
    if (memcmp(verify_buf, data, len) != 0) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_WritePage: 错误！写后验证失败，地址0x%08X\n", addr);
            Flash_Print("W25Q64_WritePage: 写入数据前8字节: ");
            for (uint16_t i=0; i<len && i<8; i++) Flash_Print("%02X ", data[i]);
            Flash_Print("\nW25Q64_WritePage: 读回数据前8字节: ");
            for (uint16_t i=0; i<len && i<8; i++) Flash_Print("%02X ", verify_buf[i]);
            Flash_Print("\n");
        #endif
        W25Q64_WriteDisable(); // 验证失败时执行写失能
        return;
    }

    // 步骤6：写入成功，执行写失能
    W25Q64_WriteDisable();

    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64_WritePage: 写入成功，地址0x%08X，长度%u字节\n", addr, len);
    #endif
}

void W25Q64_EraseSector(uint32_t addr) {
    // 先检查硬件写保护（WP引脚）
    // 预检查：确保硬件写保护已关闭（多次读取确认）
    uint8_t status_reg = 0xFF;
    for (uint8_t i=0; i<3; i++) {
        status_reg = W25Q64_ReadStatus();
        if (status_reg != 0x02 && status_reg != 0xFF) {
            break; // 状态稳定且无硬件写保护，继续
        }
        HAL_Delay(1);
    }

    if (status_reg == 0x02) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_EraseSector: 硬件写保护已使能（稳定状态），取消擦除操作\n");
        #endif
        return;
    }
    if (status_reg == 0xFF) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_EraseSector: SPI通信失败，取消擦除操作\n");
        #endif
        return;
    }


    if (addr % W25Q64_SECTOR_SIZE != 0) {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_EraseSector: 错误！地址0x%08X未按扇区大小（%u字节）对齐\n", addr, W25Q64_SECTOR_SIZE);
        #endif
        return;
    }
    if (addr >= W25Q64_TOTAL_SIZE) {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_EraseSector: 地址越界！地址0x%08X >= 总容量0x%08X\n", addr, W25Q64_TOTAL_SIZE);
        #endif
        return;
    }

    uint8_t erase_cmd[] = {0x20, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)addr};
    HAL_StatusTypeDef status_spi;

    // 步骤1：写使能
    status_spi = W25Q64_WriteEnable();
    if (status_spi != HAL_OK) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_EraseSector: 写使能失败，取消擦除操作\n");
        #endif
        return;
    }

    // 步骤2：发送擦除指令
    W25Q64_CS_LOW();
    status_spi = HAL_SPI_Transmit(&W25Q64_SPI_HANDLE, erase_cmd, 4, 100);
    if (status_spi != HAL_OK) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_EraseSector: 发送擦除指令失败，地址0x%08X，状态: %d\n", addr, status_spi);
        #endif
        W25Q64_CS_HIGH();
        W25Q64_WriteDisable(); // 异常时执行写失能
        return;
    }
    W25Q64_CS_HIGH();

    // 步骤3：等待擦除完成
    uint32_t timeout = 500;
    #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_EraseSector: 等待擦除完成...\n");
    #endif
    while ((W25Q64_ReadStatus() & 0x01) && timeout--) {
        HAL_Delay(1);
    }

    if (timeout == 0) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_EraseSector: 错误！擦除超时（500ms），地址0x%08X，状态寄存器0x%02X\n", addr, W25Q64_ReadStatus());
        #endif
        W25Q64_WriteDisable(); // 超时后执行写失能
        return;
    }

    // 步骤4：擦除后验证
    uint8_t verify_buf[16] = {0};
    W25Q64_ReadData(addr, verify_buf, 16);
    for (uint8_t i=0; i<16; i++) {
        if (verify_buf[i] != 0xFF) {
            #ifdef ENABLE_FLASH_PRINT
                Flash_Print("W25Q64_EraseSector: 错误！擦除验证失败，地址0x%08X，偏移%u字节为0x%02X（预期0xFF）\n", addr, i, verify_buf[i]);
            #endif
            W25Q64_WriteDisable(); // 验证失败时执行写失能
            return;
        }
    }

    // 步骤5：擦除成功，执行写失能
    W25Q64_WriteDisable();

    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64_EraseSector: 擦除成功，地址0x%08X\n", addr);
    #endif
}
uint8_t W25Q64_ReadStatus(void) {
    uint8_t cmd = 0x05;
    uint8_t status = 0xFF;
    HAL_StatusTypeDef spi_status;
    uint8_t retry = 3;
    uint8_t valid_status = 0xFF;

    while (retry--) {
        W25Q64_CS_LOW();
        spi_status = HAL_SPI_Transmit(&W25Q64_SPI_HANDLE, &cmd, 1, 100);
        if (spi_status != HAL_OK) {
            #ifdef ENABLE_FLASH_PRINT
                Flash_Print("W25Q64_ReadStatus: 第%d次发送指令失败，状态: %d\n", 3 - retry, spi_status);
            #endif
            W25Q64_CS_HIGH();
            HAL_Delay(1);
            continue;
        }

        uint32_t timeout = 100;
        spi_status = HAL_SPI_Receive(&W25Q64_SPI_HANDLE, &status, 1, timeout);
        W25Q64_CS_HIGH();

        if (spi_status == HAL_OK) {
            valid_status = status;
            #ifdef ENABLE_FLASH_PRINT
                Flash_Print("W25Q64_ReadStatus: 第%d次读取状态=0x%02X\n", 3 - retry, status);
            #endif
            // 核心修改：允许擦除后的特殊状态（0x03），视为“无硬件写保护”
            if ((valid_status & 0x02) == 0 || valid_status == 0x03) {
                break;
            }
        }
        HAL_Delay(1);
    }

    if (valid_status == 0xFF) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_ReadStatus: 多次读取失败，返回SPI错误\n");
        #endif
        return 0xFF;
    }

    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64_ReadStatus: 最终稳定状态=0x%02X → ", valid_status);
    Flash_Print("BIT0(忙状态): %s, ", (valid_status & 0x01) ? "忙" : "空闲");
    Flash_Print("BIT1(硬件写保护): %s, ", (valid_status & 0x02) ? "使能" : "关闭");
    Flash_Print("BIT2(软件写失能): %s\n", (valid_status & 0x04) ? "使能" : "关闭");
    #endif

    // 仅当状态为纯0x02（无忙状态）时，才视为真写保护
    if (valid_status == 0x02) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("=========================================\n");
            Flash_Print("W25Q64_ReadStatus: 错误！硬件写保护已使能（稳定状态）\n");
            Flash_Print("请重新检查：\n");
            Flash_Print("1. WP引脚是否通过10KΩ电阻可靠下拉到GND\n");
            Flash_Print("2. WP引脚电压是否稳定在0V左右\n");
            Flash_Print("3. 电阻是否开路、焊接是否牢固\n");
            Flash_Print("=========================================\n");
        #endif
        return 0x02;
    }

    if (valid_status & 0x04) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_ReadStatus: 软件写失能已使能，需先执行写使能再操作\n");
        #endif
        return 0x04;
    }

    // 特殊状态0x03视为“忙但无写保护”，返回状态供上层处理
    return valid_status;
}
void W25Q64_ChipErase(void) {
    // 先检查硬件写保护（WP引脚）
    uint8_t status_reg = W25Q64_ReadStatus();
    if (status_reg == 0x02) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_ChipErase: 硬件写保护已使能（WP引脚拉高），取消全局擦除操作\n");
        #endif
        return;
    }
    if (status_reg == 0xFF) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_ChipErase: SPI通信失败，取消全局擦除操作\n");
        #endif
        return;
    }

    uint8_t erase_cmd = 0xC7; // 全局擦除指令
    HAL_StatusTypeDef status;

    // 步骤1：写使能
    status = W25Q64_WriteEnable();
    if (status != HAL_OK) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_ChipErase: 写使能失败，取消全局擦除操作\n");
        #endif
        return;
    }

    // 步骤2：发送全局擦除指令
    W25Q64_CS_LOW();
    status = HAL_SPI_Transmit(&W25Q64_SPI_HANDLE, &erase_cmd, 1, 100);
    if (status != HAL_OK) {
        #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_ChipErase: 发送全局擦除指令失败，状态: %d\n", status);
        #endif
        W25Q64_CS_HIGH();
        W25Q64_WriteDisable(); // 异常时执行写失能
        return;
    }
    W25Q64_CS_HIGH();

    // 步骤3：等待擦除完成（全局擦除耗时极长）
    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64_ChipErase: 全局擦除开始，请勿断电...\n");
    #endif
    while (W25Q64_ReadStatus() & 0x01);

    // 步骤4：擦除成功，执行写失能
    W25Q64_WriteDisable();

    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64_ChipErase: 全局擦除完成\n");
    #endif
}




 // @brief W25Q64 读写完整性测试函数
 // @param test_addr: 测试起始地址（建议用0x00001000，避开文件系统元数据区）
 // @return uint8_t: 0=测试成功，1=硬件写保护失败，2=擦除失败，3=写入失败，4=读取失败，5=数据校验失败，0xFF=SPI通信失败
 // @note 测试流程：ID校验 → 状态检测 → 扇区擦除 → 写入测试数据 → 读取数据 → 校验一致性 → 恢复写失能
 //
uint8_t W25Q64_ReadWriteTest(uint32_t test_addr) {
    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("\n=========================================\n");
    Flash_Print("W25Q64_ReadWriteTest: 开始完整性测试\n");
    Flash_Print("测试起始地址: 0x%08X\n", test_addr);
    Flash_Print("=========================================\n");
    #endif

    // 测试数据定义（随机数+固定标识，便于校验）
    uint8_t test_write_data[W25Q64_PAGE_SIZE] = {0};
    uint8_t test_read_data[W25Q64_PAGE_SIZE] = {0};
    uint32_t test_len = 128; // 测试长度（≤256字节，单页最大）

    // 1. 填充测试数据（0x01~0x80，便于校验）
    for (uint32_t i = 0; i < test_len; i++) {
        test_write_data[i] = 0x01 + i; // 数据内容：0x01,0x02,...,0x80
    }

    // 步骤1：读取ID并校验
    uint16_t flash_id = W25Q64_ReadID();
    if (flash_id == 0 || flash_id != W25Q64_EXPECTED_ID) {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_ReadWriteTest: ID校验失败！读取0x%04X，预期0x%04X\n", flash_id, W25Q64_EXPECTED_ID);
        #endif
        return 0xFF; // SPI通信或芯片型号错误
    }

    // 步骤2：检测硬件写保护
    uint8_t status = W25Q64_ReadStatus();
    if (status == W25Q64_STATUS_HW_PROTECT) {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_ReadWriteTest: 错误！硬件写保护已使能（WP引脚拉高）\n");
        Flash_Print("请将WP引脚下拉到GND后重新测试\n");
        #endif
        return 1; // 硬件写保护失败
    }
    if (status == W25Q64_STATUS_SPI_ERROR) {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_ReadWriteTest: SPI通信失败\n");
        #endif
        return 0xFF;
    }

    // 步骤3：擦除测试扇区
    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64_ReadWriteTest: 开始擦除测试扇区（0x%08X）\n", test_addr);
    #endif
    W25Q64_EraseSector(test_addr);
    // 擦除后验证（扇区应为全0xFF）
    W25Q64_ReadData(test_addr, test_read_data, 16);
    uint8_t erase_ok = 1;
    for (uint8_t i = 0; i < 16; i++) {
        if (test_read_data[i] != 0xFF) {
            erase_ok = 0;
            break;
        }
    }
    if (!erase_ok) {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_ReadWriteTest: 扇区擦除失败！地址0x%08X\n", test_addr);
        #endif
        W25Q64_WriteDisable(); // 恢复写失能
        return 2; // 擦除失败
    }
    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64_ReadWriteTest: 扇区擦除成功\n");
    #endif

    // 步骤4：写入测试数据
    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64_ReadWriteTest: 开始写入测试数据（长度%lu字节）\n", test_len);
    #endif
    W25Q64_WritePage(test_addr, test_write_data, test_len);
    // 写入后检测忙状态
    status = W25Q64_ReadStatus();
    if ((status & 0x01) == 1) {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_ReadWriteTest: 写入后芯片仍处于忙状态\n");
        #endif
        W25Q64_WriteDisable();
        return 3; // 写入失败
    }

    // 步骤5：读取测试数据
    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64_ReadWriteTest: 开始读取测试数据\n");
    #endif
    memset(test_read_data, 0, sizeof(test_read_data)); // 清空接收缓冲区
    W25Q64_ReadData(test_addr, test_read_data, test_len);
    if (test_read_data[0] == 0x00 && test_read_data[1] == 0x00) {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("W25Q64_ReadWriteTest: 读取数据失败（缓冲区全0）\n");
        #endif
        W25Q64_WriteDisable();
        return 4; // 读取失败
    }

    // 步骤6：校验读写数据一致性
    #ifdef ENABLE_FLASH_PRINT
    Flash_Print("W25Q64_ReadWriteTest: 开始校验数据一致性\n");
    Flash_Print("写入数据前8字节: ");
    for (uint8_t i = 0; i < 8; i++) Flash_Print("%02X ", test_write_data[i]);
    Flash_Print("\n读取数据前8字节: ");
    for (uint8_t i = 0; i < 8; i++) Flash_Print("%02X ", test_read_data[i]);
    Flash_Print("\n");
    #endif

    uint8_t verify_ok = 1;
    for (uint32_t i = 0; i < test_len; i++) {
        if (test_read_data[i] != test_write_data[i]) {
            #ifdef ENABLE_FLASH_PRINT
            Flash_Print("W25Q64_ReadWriteTest: 数据校验失败！偏移%lu字节，写入0x%02X，读取0x%02X\n", i, test_write_data[i], test_read_data[i]);
            #endif
            verify_ok = 0;
            break;
        }
    }

    // 步骤7：恢复写失能，确保芯片安全
    W25Q64_WriteDisable();

    // 最终结果判断
    if (verify_ok) {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("\nW25Q64_ReadWriteTest: 所有测试通过！\n");
        Flash_Print("=========================================\n");
        #endif
        return 0; // 测试成功
    } else {
        #ifdef ENABLE_FLASH_PRINT
        Flash_Print("\nW25Q64_ReadWriteTest: 数据校验失败！\n");
        Flash_Print("=========================================\n");
        #endif
        return 5; // 数据校验失败
    }
}


