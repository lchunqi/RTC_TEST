
#include "main.h"
#include <string.h>
#include "flash.h"

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

#define CS_HIGH     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET)
#define CS_LOW      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET)
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

uint16_t flash_device_id(void)
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
        HAL_SPI_Transmit(&W25Q64SPI, addr, 3, 10);      //****
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
    *target = *((float*)&sum);
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
    if (rtcNumber < 1 || rtcNumber > 84) {
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
    if (rtcNumber < 1 || rtcNumber > 84) {
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

#define CHIP_SECTOR_NUMBER     (MAX_CHIP_NUMBER / 8) + 1 // 13
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
