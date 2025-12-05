#ifndef __FLASH_H
#define __FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif


#define PAGE_SIZE               0x100                               //write and read must use 1 page
#define PAGES_IN_SECTOR         16
#define SECTOR_SIZE             PAGE_SIZE * PAGES_IN_SECTOR         //0X1000
#define SECTORS_IN_BLOCK        16
#define BLOCK_SIZE              SECTOR_SIZE * SECTORS_IN_BLOCK      //0x10000
#define PAGES_IN_BLOCK          PAGES_IN_SECTOR * SECTORS_IN_BLOCK  //256
#define BLOCKS_IN_CHIP          256
#define FULL_CHIP_SIZE          BLOCK_SIZE * BLOCKS_IN_CHIP         //0x1000000
#define FLASH_END_ADDR          FULL_CHIP_SIZE - 1                  //0xFFFFFF

#define TEST_DATA_START_ADDR    0x000000                            //use first block to temp test data
#define TEST_MAX_PAGE           PAGES_IN_BLOCK
#define TEST_FALG_START_ADDR    0x000000                            //test flag of all rtc
#define TEST_RESULT_START_ADDR  0x001000                            //test result of one rtc
#define TEST_RESULT_ONE_SIZE    PAGE_SIZE * 2
#define TEST_DATA_END_ADDR      0x00FFFF

#define LOG_START_ADDR          0x20000                             //not use first and second block
#define LOG_MAX_BUFFER          64                                  //buffer of one log
#define LOG_MAX_PAGE            PAGES_IN_BLOCK * (BLOCKS_IN_CHIP - 2)  //256 * (256 - 2) = 65024

#define MAX_CHIP_NUMBER         84
#define MAX_TEST_DOT_NUMBER     5
#define FALSH_MACHINE_DATA_LENGTH   16
typedef struct{
    uint8_t temp[MAX_CHIP_NUMBER];
    uint8_t testDot[MAX_CHIP_NUMBER];
    uint8_t calculate[MAX_CHIP_NUMBER];
    uint8_t tempLoad[MAX_CHIP_NUMBER];
    uint8_t machine[MAX_CHIP_NUMBER];
    uint8_t takedown[MAX_CHIP_NUMBER];
}TestFlag; 

typedef struct{
    uint16_t Ivdd;
    uint16_t Ivbat;
    uint8_t config9;
    uint8_t configC;
    uint8_t sysTemp;
    uint8_t rtcTemp;
    float testFreq;
    uint16_t vbat_2V5;
    uint16_t charge;
}MachineTest;

typedef struct{
    uint8_t chipID[8];
    float testDotTemp[MAX_TEST_DOT_NUMBER];
    float testDotCap[MAX_TEST_DOT_NUMBER];
    uint8_t setTemp;
    uint8_t tempConfig;
    uint16_t startTimeHot;
    uint16_t startTimeCold;
    uint8_t errorTimeHot;
    uint8_t errorTimeCold;
    uint8_t calculFlag;
    uint8_t tempLoadFlag;
    float testCenter[2];        // 中心温度点对应的电容在-10度和60度的误差值
    MachineTest machine;
    uint8_t capCalcul[151];
}TestResult; 

typedef enum {
    FLAG_TEMP = 0,
    FLAG_TEST_DOT,
    FLAG_CALCULATE,
    FLAG_TEMPLOAD,
    FLAG_MACHINE,
    FLAG_TEST_TIME,
    FLAG_TAKE_DOWN
}TestFlagList;

typedef enum {
    FLASH_FLAG_READ     =   0,
    FLASH_FLAG_WRITE_ADD,
    FLASH_FLAG_WRITE_COVER
}TestFlagHandle;


typedef enum {
    MACHINE_I_VDD = 0,
    MACHINE_I_VBAT = MACHINE_I_VDD + 2,
    MACHINE_CONFIG_9 = MACHINE_I_VBAT + 2,
    MACHINE_CONFIG_C,
    MACHINE_SYSTEM_TEMP,
    MACHINE_RTC_TEMP,
    MACHINE_TEST_FREQ,
    MACHINE_VBAT_2V5 = MACHINE_TEST_FREQ + 4, 
    MACHINE_CHARGE = MACHINE_VBAT_2V5 + 2,
}TestMachineList;

typedef enum {
    CHIP_ID = 0,
    TEST_DOT_TEMP = CHIP_ID + 8,
    TEST_DOT_CAP = TEST_DOT_TEMP + 20,
    SET_TEMP = TEST_DOT_CAP + 20,
    TEMP_CONFIG,
    START_TIME_HOT = TEMP_CONFIG + 2,
    START_TIME_COLD = START_TIME_HOT + 2,
    ERROR_TIME_HOT,
    ERROR_TIME_COLD,
    CALCUL_FLAG,
    TEMP_LOAD_FLAG,
    TEST_CENTER,
    MACHINE_TEST_ALL = TEST_CENTER + 8,
    CAP_CALCULATE = 256,
    TEST_DATA_ALL
}TestResultList;

typedef enum {
    FLASH_CLEAR_DOT_0 = 0,
    FLASH_CLEAR_DOT_1,
    FLASH_CLEAR_DOT_2,
    FLASH_CLEAR_DOT_3,
    FLASH_CLEAR_DOT_4,
    FLASH_CLEAR_CALCUL,
    FLASH_CLEAR_MACHINE,
} FlashClearMode;

int CheckBusy(void);
uint16_t flash_device_id(void);
int flash_erase(uint32_t address, uint8_t mode);
int flash_write_data(uint32_t address, uint8_t* buffer, uint16_t size);
int flash_read_data(uint32_t address, uint8_t* buffer, uint16_t length);
void flash_manage_test_data(TestResult* result, uint8_t rtcNumber,  TestResultList handle);
void float_to_uint8(uint8_t* target, float* source);
void uint8_to_float(float* target, uint8_t* source);
int flash_test_flag_manage(uint8_t* buffer, TestFlagList handle, TestFlagHandle mode);
void flash_test_data_read(TestResult* result, uint8_t rtcNumber, TestResultList handle);
void flash_test_data_write_add(TestResult* result, uint8_t rtcNumber, TestResultList handle);
void flash_test_data_clear(FlashClearMode mode);
void test_data_printf(uint8_t mode, TestResult result);

#ifdef __cplusplus
}
#endif

#endif 