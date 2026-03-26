
#include "main.h"
#include <string.h>
#include "flash.h"
#include <stdint.h>
#include <spi.h>

/*
//--------------------------------
// flash_read_data函数和flash_write_data函数是最底层的函数
// 可以对25Q64系列的flash进行读写操作
// 但是要注意的是，读和写的长度不能超过页，否则会数据出错
// 另外的封装函数固定读取某些部分的数据，在设计的时候就避免了数据越界
//------------------------------
#define ERASE_SECTOR_ADDR   0x20                   // 
#define ERASE_BLOCK_ADDR    0xd8                    //
#define ERASE_CHIP_ADDR     0xC7                    // 

extern SPI_HandleTypeDef hspi1;
#define W25Q64SPI hspi1
uint8_t w25x_read_id = 0x90;                    // 
uint8_t check_addr   = 0x05;                    // 
uint8_t enable_write = 0x06;                    // 
uint8_t write_addr   = 0x02;                    // 
uint8_t read_addr    = 0x03;                    //
uint8_t g_bufferSector[SECTOR_SIZE];


int CheckBusy(void)
{
    uint8_t status = 1;
    int timeCount = 0;

    for (timeCount = 0; timeCount < 20; timeCount++) {
        CS_LOW;
        HAL_SPI_Transmit(&W25Q64SPI, &check_addr, 1, 10);
        HAL_SPI_Receive(&W25Q64SPI, &status, 1, 10);
        CS_HIGH;
        if ((status & 0x01) == 0) { return 1; }
        HAL_Delay(1);
    }
    return 0;
}

uint16_t flash1_device_id(void)
{
    uint8_t buff[5] = {0,0,0,0,0};
    uint16_t deviceId = 0;
    CS_LOW;
    HAL_SPI_Transmit(&W25Q64SPI, &w25x_read_id, 1, 10);
    HAL_SPI_Receive(&W25Q64SPI, buff, 5, 10);
    CS_HIGH;
    UsbPrintf("flash ID is 0x%x%x\n", buff[3], buff[4]);
    deviceId = buff[3];
    deviceId = deviceId * 256 + buff[4];
    return deviceId; 
}

void address_32_to_8(uint32_t address, uint8_t* addr8)
{
    int i;
    for (i = 2; i >= 0; i--) {
        addr8[i] = address % 256;
        address = address / 256;
    }
}

int flash_erase(uint32_t address, uint8_t mode)
{
    uint8_t erase_address;
    uint8_t addr[3] = {0};

    address_32_to_8(address, addr);
    if (CheckBusy() <= 0) {
        return -1;
    }
    CS_LOW;
    HAL_SPI_Transmit(&W25Q64SPI, &enable_write, 1, 10);
    CS_HIGH;
    switch (mode) {
        case 0:
            erase_address = ERASE_CHIP_ADDR;
            break;
        case 1:
            erase_address = ERASE_BLOCK_ADDR;
            break;
        case 2:
            erase_address = ERASE_SECTOR_ADDR;
            break;
        default:
            return 0;
            break;
    }
    
    CS_LOW;
    HAL_SPI_Transmit(&W25Q64SPI, &erase_address, 1, 10);//
    if (mode != 0) {                                    // 
        HAL_SPI_Transmit(&W25Q64SPI, addr, 3, 10);      //
    }
    CS_HIGH;
    return 1;
}

int flash_write_data(uint32_t address, uint8_t* buffer, uint16_t size)
{
    uint8_t addr[3] = {0};

    if (address >= FLASH_END_ADDR || size > PAGE_SIZE) {
        return 0;
    }
    if (CheckBusy() == 0) { return 0;  }
    address_32_to_8(address, addr);
    CS_LOW;
    HAL_SPI_Transmit(&W25Q64SPI, &enable_write, 1, 10);//
    CS_HIGH;
    if (CheckBusy() <= 0) {
        return -1;
    } 

    CS_LOW;
    HAL_SPI_Transmit(&W25Q64SPI, &write_addr, 1, 10);//
    HAL_SPI_Transmit(&W25Q64SPI, addr, 3, 10);//
    HAL_SPI_Transmit(&W25Q64SPI, buffer, size, 10);//
    CS_HIGH;
    return 1;
}

int flash_read_data(uint32_t address, uint8_t* buffer, uint16_t length)
{
    uint8_t addr[3] = {0};
    if (address >= FLASH_END_ADDR || length > PAGE_SIZE) {
        return 0;
    }
    if (CheckBusy() == 0) { return 0;  }
    address_32_to_8(address, addr);
    CS_LOW;
    HAL_SPI_Transmit(&W25Q64SPI, &read_addr, 1, 10);
    HAL_SPI_Transmit(&W25Q64SPI,addr, 3, 10);
    HAL_SPI_Receive(&W25Q64SPI, buffer, length, 50);
    CS_HIGH;
    return 1;
}

void float_to_uint8(uint8_t* target, float* source)
{
    uint32_t sum = 0;
    int i;
    sum = *((uint32_t *)source);
    for (i = 3; i >= 0; i--) {
        target[i] = sum % 256;
        sum = sum >> 8;
    }
}
void uint8_to_float(float* target, uint8_t* source)
{
    uint32_t sum = 0;
    int i;
    for (i = 0; i < 4; i++) {
        sum = (sum << 8) + source[i];
    }
    if (sum == 0xffffffff) {
        *target = -16777215;
    } else {
        *target = *((float*)&sum);
    }
}

int flash_test_flag_manage(uint8_t* buffer, TestFlagList handle, TestFlagHandle mode)
{
    uint32_t address;
    int i;
    uint8_t writeBuffer[PAGE_SIZE * 6] = {0xff};
    switch (mode) {
        case FLASH_FLAG_READ:
            address = TEST_FALG_START_ADDR + handle * PAGE_SIZE;
            flash_read_data(address, buffer, MAX_CHIP_NUMBER);
        break;
        case FLASH_FLAG_WRITE_ADD:
            address = TEST_FALG_START_ADDR + handle * PAGE_SIZE;
            flash_write_data(address, buffer, MAX_CHIP_NUMBER);
        break;
        case FLASH_FLAG_WRITE_COVER:
            for (i = 0; i < 6; i++) {
                flash_read_data(TEST_FALG_START_ADDR + i * PAGE_SIZE, writeBuffer + i * PAGE_SIZE, PAGE_SIZE);
            }
            for (i = 0; i < MAX_CHIP_NUMBER; i++) {
                writeBuffer[PAGE_SIZE * handle + i] = buffer[i];
            }
            HAL_Delay(5);

            flash_erase(TEST_FALG_START_ADDR, 2);//erase the sector    
            HAL_Delay(50);

            for (i = 0; i < 6; i++) {
                flash_write_data(TEST_FALG_START_ADDR + i * PAGE_SIZE, writeBuffer+ i * PAGE_SIZE, PAGE_SIZE);
                HAL_Delay(5);
            }
        break;
        default:
        break;
    }

    return 1;
}

void flash_test_data_read(TestResult* result, uint8_t rtcNumber, TestResultList handle)
{
    uint32_t address;
    uint8_t buffer[4] = {0};
    int i;
    if (rtcNumber < 1 || rtcNumber > MAX_CHIP_NUMBER) {
        return;
    }
    address = TEST_RESULT_START_ADDR + (rtcNumber - 1) * TEST_RESULT_ONE_SIZE + handle;
    switch (handle) {
        case CHIP_ID:
            flash_read_data(address, result->chipID, 8);
        break;
        case TEST_DOT_TEMP:
            for (i = 0; i < MAX_TEST_DOT_NUMBER; i++) {
                flash_read_data(address, buffer, 4);
                uint8_to_float(&(result->testDotTemp[i]), buffer);
                address  += 4;
            }
        break;
        case TEST_DOT_CAP:
            for (i = 0; i < MAX_TEST_DOT_NUMBER; i++) {
                flash_read_data(address, buffer, 4);
                uint8_to_float(&(result->testDotCap[i]), buffer);
                address  += 4;
            }
        break;
        case SET_TEMP:
            flash_read_data(address, &(result->setTemp), 1);
        break;
        case TEMP_CONFIG:
            flash_read_data(address, &(result->tempConfig), 1);
        break;
        case START_TIME_HOT:
            flash_read_data(address, buffer, 2);
            result->startTimeHot = buffer[0] * 256 + buffer[1];
        break;
        case START_TIME_COLD:
            flash_read_data(address, buffer, 2);
            result->startTimeCold = buffer[0] * 256 + buffer[1];
        break;
        case ERROR_TIME_HOT:
            flash_read_data(address, &(result->errorTimeHot), 1);
        break;
        case ERROR_TIME_COLD:
            flash_read_data(address, &(result->errorTimeCold), 1);
        break;
        case CALCUL_FLAG:
            flash_read_data(address, &(result->calculFlag), 1);
        break;
        case TEMP_LOAD_FLAG:
            flash_read_data(address, &(result->tempLoadFlag), 1);
        break;
        case TEST_CENTER:
            flash_read_data(address, buffer, 4);
            uint8_to_float(&(result->testCenter[0]), buffer);
            flash_read_data(address + 4, buffer, 4);
            uint8_to_float(&(result->testCenter[1]), buffer);
        break;
        case MACHINE_TEST_ALL:
            flash_read_data(address + MACHINE_I_VDD, buffer, 2);
            result->machine.Ivdd = buffer[0] * 256 + buffer[1];
            flash_read_data(address + MACHINE_I_VBAT, buffer, 2);
            result->machine.Ivbat = buffer[0] * 256 + buffer[1];
            flash_read_data(address + MACHINE_CONFIG_9, &(result->machine.config9), 1);
            flash_read_data(address + MACHINE_CONFIG_C, &(result->machine.configC), 1);
            flash_read_data(address + MACHINE_SYSTEM_TEMP, &(result->machine.sysTemp), 1);
            flash_read_data(address + MACHINE_RTC_TEMP, &(result->machine.rtcTemp), 1);
            flash_read_data(address + MACHINE_TEST_FREQ, buffer, 4);
            uint8_to_float(&(result->machine.testFreq), buffer);
            flash_read_data(address + MACHINE_VBAT_2V5, buffer, 2);
            result->machine.vbat_2V5 = buffer[0] * 256 + buffer[1];
            flash_read_data(address + MACHINE_CHARGE, buffer, 2);
            result->machine.charge = buffer[0] * 256 + buffer[1];
        break;
        case CAP_CALCULATE:
            flash_read_data(address, result->capCalcul, 151);
        break;
        case TEST_DATA_ALL:
            for (i = 0; i <= CAP_CALCULATE; i++) {
                flash_test_data_read(result, rtcNumber, i);
            }
        break;
        default: break;
    }
}

void flash_test_data_write_add(TestResult* result, uint8_t rtcNumber, TestResultList handle)
{
    uint32_t address;
    uint8_t buffer[4] = {0};
    uint8_t machineBuffer[FALSH_MACHINE_DATA_LENGTH] = {0};
    int i;
    if (rtcNumber < 1 || rtcNumber > MAX_CHIP_NUMBER) {
        return;
    }
    address = TEST_RESULT_START_ADDR + (rtcNumber - 1) * TEST_RESULT_ONE_SIZE + handle;
    switch (handle) {
        case CHIP_ID:
            flash_write_data(address, result->chipID, 8);
        break;
        case TEST_DOT_TEMP:
            for (i = 0; i < MAX_TEST_DOT_NUMBER; i++) {
                float_to_uint8(buffer, &(result->testDotTemp[i]));
                flash_write_data(address, buffer, 4);
                address += 4;
            }
        break;
        case TEST_DOT_CAP:
            for (i = 0; i < MAX_TEST_DOT_NUMBER; i++) {
                float_to_uint8(buffer, &(result->testDotCap[i]));
                flash_write_data(address, buffer, 4);
                address += 4;
            }
        break;
        case SET_TEMP:
            flash_write_data(address, &(result->setTemp), 1);
        break;
        case TEMP_CONFIG:
            flash_write_data(address, &(result->tempConfig), 1);
        break;
        case START_TIME_HOT:
            buffer[0] = result->startTimeHot / 256;
            buffer[1] = result->startTimeHot % 256;
            flash_write_data(address, buffer, 2);
        break;
        case START_TIME_COLD:
            buffer[0] = result->startTimeCold / 256;
            buffer[1] = result->startTimeCold % 256;
            flash_write_data(address, buffer, 2);
        break;
        case ERROR_TIME_HOT:
            flash_write_data(address, &(result->errorTimeHot), 1);
        break;
        case ERROR_TIME_COLD:
            flash_write_data(address, &(result->errorTimeCold), 1);
        break;
        case CALCUL_FLAG:
            flash_write_data(address, &(result->calculFlag), 1);
        break;
        case TEMP_LOAD_FLAG:
            flash_write_data(address, &(result->tempLoadFlag), 1);
        break;
        case TEST_CENTER:
            for (i = 0; i < 2; i++) {
                float_to_uint8(buffer, &(result->testCenter[i]));
                flash_write_data(address, buffer, 4);
                address += 4;
            }
        break;
        case MACHINE_TEST_ALL:
            machineBuffer[MACHINE_I_VDD] = result->machine.Ivdd / 256;
            machineBuffer[MACHINE_I_VDD + 1] = result->machine.Ivdd % 256;
            machineBuffer[MACHINE_I_VBAT] = result->machine.Ivbat / 256;
            machineBuffer[MACHINE_I_VBAT + 1] = result->machine.Ivbat % 256;
            machineBuffer[MACHINE_CONFIG_9] = result->machine.config9;
            machineBuffer[MACHINE_CONFIG_C] = result->machine.configC;
            machineBuffer[MACHINE_SYSTEM_TEMP] = result->machine.sysTemp;
            machineBuffer[MACHINE_RTC_TEMP] = result->machine.rtcTemp;
            float_to_uint8(buffer, &(result->machine.testFreq));
            for (i = 0; i < 4; i++) {
               machineBuffer[MACHINE_TEST_FREQ + i] = buffer[i]; 
            }
            machineBuffer[MACHINE_VBAT_2V5] = result->machine.vbat_2V5 / 256;
            machineBuffer[MACHINE_VBAT_2V5 + 1] = result->machine.vbat_2V5 % 256;
            machineBuffer[MACHINE_CHARGE] = result->machine.charge / 256;
            machineBuffer[MACHINE_CHARGE + 1] = result->machine.charge % 256;
            flash_write_data(address, machineBuffer, FALSH_MACHINE_DATA_LENGTH);
        break;
        case CAP_CALCULATE:
            flash_write_data(address, result->capCalcul, 151);
        break;
        case TEST_DATA_ALL:
            for (i = 0; i <= CAP_CALCULATE; i++) {
                flash_test_data_write_add(result, rtcNumber, i);
            }
        break;
        default: break;
    }
}

#define CHIP_SECTOR_NUMBER     7 //MAX_CHIP_NUMBER / 8
void flash_test_data_clear(FlashClearMode mode)
{
    int i, j, k;
    uint32_t address;
    uint32_t range;

    for (k = 0; k < CHIP_SECTOR_NUMBER; k++) {
        memset(g_bufferSector, 0xff, SECTOR_SIZE);
        address = TEST_RESULT_START_ADDR + SECTOR_SIZE * k;
        for (i = 0; i < PAGES_IN_SECTOR; i++) {
            flash_read_data(address + (PAGE_SIZE * i), g_bufferSector + (PAGE_SIZE * i), PAGE_SIZE);
        }
        for (i = 0; i < (SECTOR_SIZE / TEST_RESULT_ONE_SIZE); i++) {//每个sector有8个芯片
            switch (mode) {
                case FLASH_CLEAR_DOT_1:
                case FLASH_CLEAR_DOT_3://-10度和60度两个点需要把中心电容对应的误差值清零
                    range = i * TEST_RESULT_ONE_SIZE + TEST_CENTER + (mode / 2) * 4;
                    for (j = 0; j < 4; j++) {
                        g_bufferSector[range + j] = 0xff;
                    }
                case FLASH_CLEAR_DOT_0:
                case FLASH_CLEAR_DOT_2:
                case FLASH_CLEAR_DOT_4:
                    range = i * TEST_RESULT_ONE_SIZE + TEST_DOT_TEMP + mode * 4;
                    for (j = 0; j < 4; j++) {
                        g_bufferSector[range + j] = 0xff;
                    }
                    range = i * TEST_RESULT_ONE_SIZE + TEST_DOT_CAP + mode * 4;
                    for (j = 0; j < 4; j++) {
                        g_bufferSector[range + j] = 0xff;
                    }
                break;
                case FLASH_CLEAR_CALCUL:
                    range = i * TEST_RESULT_ONE_SIZE + CAP_CALCULATE;
                    for (j = 0; j < 151; j++) {
                        g_bufferSector[range + j] = 0xff;
                    }
                break;
                case FLASH_CLEAR_MACHINE:
                    range = i * TEST_RESULT_ONE_SIZE + MACHINE_TEST_ALL;
                    for (j = 0; j < FALSH_MACHINE_DATA_LENGTH; j++) {
                        g_bufferSector[range + j] = 0xff;
                    }
                break;
                default: break;
            }
        }
        flash_erase(address, 2);//erase the sector    
        HAL_Delay(20);
        for (i = 0; i < PAGES_IN_SECTOR; i++) {
            flash_write_data(address + (PAGE_SIZE * i), g_bufferSector + (PAGE_SIZE * i), PAGE_SIZE);
            HAL_Delay(5);
        }
    }
    
}


void test_data_printf(uint8_t mode, TestResult result) 
{
    int i;
    uint8_t k;
		switch (mode) {
			case 0://all printf
                for (k = 1; k < 6; k++) {
                    test_data_printf(k, result);
                }
			break;
			case 1:
				UsbPrintf("ID:%02X%02X%02X%02X%02X%02X%02X%02X\n", result.chipID[0], result.chipID[1], 
				result.chipID[2], result.chipID[3], result.chipID[4], result.chipID[5], result.chipID[6], result.chipID[7]);
			break;
			case 2:
				UsbPrintf("cap adjust tset:\n");
				for (i = 0; i < MAX_TEST_DOT_NUMBER; i++) {
					UsbPrintf("[%.1f, %.2f] ", result.testDotTemp[i], result.testDotCap[i]);
				}
                UsbPrintf("\n");
				UsbPrintf("[-10C error]%.2fppm, [60C error]%.2fppm \n", result.testCenter[0], result.testCenter[1]);
			break;
			case 3:
				UsbPrintf("aging data:\n");
				UsbPrintf("test temp:%d, config9:%d\n", result.setTemp, result.tempConfig);
				// UsbPrintf("start time cold:%dms, hot:%dms\n", result.startTimeCold, result.startTimeHot);
				// UsbPrintf("error time cold:%ds, hot:%ds\n", result.errorTimeCold, result.errorTimeHot);
			break;
			case 4:
				UsbPrintf("machine data:\n");
				UsbPrintf("Ivdd:%dnA, Ivbat:%dnA\n", result.machine.Ivdd, result.machine.Ivbat);
				UsbPrintf("config9:%02X, configC:%02X\n", result.machine.config9, result.machine.configC);
				UsbPrintf("temp system:%dC, rtc:%dC\n", result.machine.sysTemp, result.machine.rtcTemp);
				UsbPrintf("test frequency:%fkHz\n", result.machine.testFreq);
				UsbPrintf("vbat:%dmV, charge:%dmV\n", result.machine.vbat_2V5, result.machine.charge);
			break;
			case 5:
				UsbPrintf("cap calculate:");
				for (i = 0; i < 151; i++) {
					if (i % 20 == 0) { UsbPrintf("\n[%d]\t", i);}
					UsbPrintf("%02X ", result.capCalcul[i]);
				}
			break;
			default:
			break;
		}

}
//---------------------------MCU flash start------------------------------------------

void SaveStringToFlash(char* s, uint32_t addr)
{
	int length = strlen(s);
	uint16_t j;
	uint32_t data = 0;
	uint32_t address = addr;
	if (length <= 0) {return;}

	HAL_FLASH_Unlock();
	for (j = 0; j < length; j += 4) {
		if (j + 4 <= length) {
			data = s[j] + ((uint32_t)s[j + 1] << 8) + ((uint32_t)s[j + 2] << 16)+ ((uint32_t)s[j + 3] << 24);
		} else {
			if (length == j + 1) { 
				data = s[j];
			} else if (length == j + 2) { 
				data =  s[j] + ((uint32_t)s[j + 1] << 8);
			} else if (length == j + 3) { 
				data =  s[j] + ((uint32_t)s[j + 1] << 8) + ((uint32_t)s[j + 2] << 16);
			} 
		}
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);
		address += 4;
	}
	HAL_FLASH_Lock();
}
void GetStringFromFlash(char* target, uint16_t length, uint32_t addr)
{
	uint16_t i;
	uint32_t value;

	for (i = 0; i < length; i++) {
		value = *(uint8_t*)(addr + i);
		if (value == 0xff || value == 0) {
			target[i] = 0;
			break;
		}
		target[i] = value;
	}
}

void ClearBsnFlash(void)
{
	FLASH_EraseInitTypeDef EraseInitStruct = {0};
	uint32_t PageError = 0;
	HAL_FLASH_Unlock();

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.NbPages   = 1;
	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

	HAL_FLASH_Lock();
}

//---------------------------MCU flash end-----------------------------------------

*/

//fatfs
// W25Q64 核心指令集
#define W25Q_WriteEnable        0x06
#define W25Q_WriteDisable       0x04
#define W25Q_ReadStatusReg1     0x05
#define W25Q_ReadID             0x90
#define W25Q_EraseSector        0x20  // 4KB扇区擦除（核心）
#define W25Q_PageProgram        0x02  // 256字节页编程
#define W25Q_ReadData           0x03

// SPI单字节读写（底层通信，对接HAL库）
static uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{
    uint8_t RxData = 0;
    HAL_SPI_TransmitReceive(&hspi1, &TxData, &RxData, 1, 100);
    return RxData;
}

// 等待Flash忙状态（WIP位清零，擦除/写入必须等待）
void CheckBusy(void)
{
    uint8_t status = 0;
    CS_LOW;  // 宏定义来自flash.h，CS拉低
    SPI1_ReadWriteByte(W25Q_ReadStatusReg1);
    do{
        status = SPI1_ReadWriteByte(0xFF);
    }while((status & 0x01) == 0x01); // WIP=1 忙，WIP=0 空闲
    CS_HIGH; // CS拉高
}

// 写使能（擦除/写入前必须执行）
static void W25Qxx_WriteEnable(void)
{
    CS_LOW;
    SPI1_ReadWriteByte(W25Q_WriteEnable);
    CS_HIGH;
    HAL_Delay(1); // 短延时保证指令生效
}

// 读取W25Q64设备ID（返回0xEF17为正常，对接diskio.c校验）
uint16_t flash_device_id(void)
{
    uint16_t id = 0;
    CS_LOW;
    SPI1_ReadWriteByte(W25Q_ReadID);    // 读ID指令
    SPI1_ReadWriteByte(0x00);           // 地址位A23-A16
    SPI1_ReadWriteByte(0x00);           // 地址位A15-A8
    SPI1_ReadWriteByte(0x00);           // 地址位A7-A0
    id |= (uint16_t)SPI1_ReadWriteByte(0xFF) << 8; // 厂家ID 0xEF
    id |= SPI1_ReadWriteByte(0xFF);                // 设备ID 0x17
    CS_HIGH;
    return id;
}

// 4KB扇区擦除（diskio.c专用，返回1=成功，0=失败）
int flash_erase_sector(uint32_t address)
{
    // 地址必须4KB扇区对齐（W25Q64硬件要求）
    if((address & (SECTOR_SIZE - 1)) != 0) return 0;
    
    W25Qxx_WriteEnable();  // 写使能
    CheckBusy();         // 等待前序操作完成
    CS_LOW;
    SPI1_ReadWriteByte(W25Q_EraseSector); // 4KB扇区擦除指令
    // 发送24位擦除地址
    SPI1_ReadWriteByte((address >> 16) & 0xFF);
    SPI1_ReadWriteByte((address >> 8) & 0xFF);
    SPI1_ReadWriteByte(address & 0xFF);
    CS_HIGH;
    CheckBusy();         // 等待擦除完成（擦除耗时约100ms）
    return 1;            // 1=成功，对接diskio.c返回值逻辑
}

// 页编程（内部调用，256字节页对齐，返回1=成功，0=失败）
static int W25Q_PageWrite(uint32_t address, uint8_t* buffer, uint16_t size)
{
    if(size > PAGE_SIZE || (address & (PAGE_SIZE - 1)) != 0) return 0;
    
    W25Qxx_WriteEnable();
    CheckBusy();
    CS_LOW;
    SPI1_ReadWriteByte(W25Q_PageProgram); // 页编程指令
    // 发送24位写入地址
    SPI1_ReadWriteByte((address >> 16) & 0xFF);
    SPI1_ReadWriteByte((address >> 8) & 0xFF);
    SPI1_ReadWriteByte(address & 0xFF);
    // 写入数据
    for(uint16_t i=0; i<size; i++)
    {
        SPI1_ReadWriteByte(buffer[i]);
    }
    CS_HIGH;
    CheckBusy();
    return 1;
}

// 通用写数据（支持任意长度，自动分包，对接diskio.c，返回1=成功，0=失败）
int flash_write_data(uint32_t address, uint8_t* buffer, uint16_t length)
{
    // 参数合法性校验
    if(buffer == NULL || length == 0 || address > FLASH_END_ADDR) return 0;
    if((address + length) > (FLASH_END_ADDR + 1)) return 0; // 地址越界
    
    uint16_t remain_size = length;
    uint16_t write_size = 0;
    uint32_t curr_addr = address;
    uint8_t* curr_buff = buffer;

    // 循环分包写入（按256字节页拆分）
    while(remain_size > 0)
    {
        write_size = PAGE_SIZE - (curr_addr & (PAGE_SIZE - 1)); // 计算当前页剩余可写字节
        write_size = (write_size > remain_size) ? remain_size : write_size;
        // 页写入失败则整体返回失败
        if(W25Q_PageWrite(curr_addr, curr_buff, write_size) != 1)
        {
            return 0;
        }
        // 更新地址和缓冲区
        curr_addr += write_size;
        curr_buff += write_size;
        remain_size -= write_size;
    }
    return 1; // 全部写入成功
}

// 通用读数据（支持任意长度，对接diskio.c，返回1=成功，0=失败）
int flash_read_data(uint32_t address, uint8_t* buffer, uint16_t length)
{
    // 参数合法性校验
    if(buffer == NULL || length == 0 || address > FLASH_END_ADDR) return 0;
    if((address + length) > (FLASH_END_ADDR + 1)) return 0;

    CheckBusy();
    CS_LOW;
    SPI1_ReadWriteByte(W25Q_ReadData); // 读数据指令
    // 发送24位读取地址
    SPI1_ReadWriteByte((address >> 16) & 0xFF);
    SPI1_ReadWriteByte((address >> 8) & 0xFF);
    SPI1_ReadWriteByte(address & 0xFF);
    // 读取数据到缓冲区
    for(uint16_t i=0; i<length; i++)
    {
        buffer[i] = SPI1_ReadWriteByte(0xFF);
    }
    CS_HIGH;
    return 1; // 1=成功，对接diskio.c返回值逻辑
}






