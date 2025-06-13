#include "main.h"
#include "manager.h"
#include "current.h"
#include "voltage.h"
#include "temp.h"
#include "flash.h"
#include "log.h"
#include "task.h"
#include "rtc.h"
#include "calculate.h"
#include <string.h>
#include "power.h"
#include "drv_wave1006.h"

#define SD8800_CENTER_TEMP_UP                   30//30℃
#define SD8800_CENTER_TEMP_DOWN                 15//15℃

typedef struct {
    uint8_t number;
    float temp[MAX_CHIP_NUMBER];
    float cap[MAX_CHIP_NUMBER];
    uint8_t flag[MAX_CHIP_NUMBER];
}TestOneData;

static uint8_t g_testMode = FREE_MODE;
CapAdjust g_capAdjust = {0};
static TestOneData g_oneData = {0};
uint8_t g_testFlag[MAX_CHIP_NUMBER] = {0};

void manager_set_mode(uint8_t mode)
{
    g_testMode = mode;
}

uint8_t manager_get_mode(void)
{
    return g_testMode;
}


uint8_t system_voltage_test(void)
{
	uint32_t voltage;
    uint8_t result = 0;
	voltage = ststem_voltage(CHANNEL_3V3) * 3300 * 4.6 / 4096;
	log_printf("3.3V:%dmV,", voltage);
    if (voltage < 3000 && voltage > 3500) {result++;}
	voltage = ststem_voltage(CHANNEL_5V) * 3300 * 4.6 / 4096;
	log_printf("5V:%dmV,", voltage);
    if (voltage < 4500 && voltage > 5500) {result++;}
    return result;
} 

uint8_t check_history_download(uint8_t number)
{
    uint8_t buffer[2] = {0};
	rtc_set_channel(number, RTC_INIT_1006);
    rtc_read_multi_bytes(0x64, 0xb2, buffer, 2);
    if (buffer[0] == 24 || buffer[1] != 0) {
        log_printf("check id is download\n");
        return 1;
    }
    return 0;
}

int system_manage_temp(void)
{
	float temp;
    int i;
    float rtc_temp;
    uint8_t config[MAX_CHIP_NUMBER] = {0};
    // uint8_t flag[MAX_CHIP_NUMBER] = {0};
    uint8_t stepCount;
    TestResult tempTest = {0};

//step 1: read system temp ship, and check system voltage ok
	temp = temp_read_value(1);
    log_printf("temp1 value %.1fC\n", temp);
    if (temp > SD8800_CENTER_TEMP_UP || temp < SD8800_CENTER_TEMP_DOWN) {
        log_printf("check system temp is error, test stop.\n");
        ControlPrintf("message:系统温度超范围\n");
        return -1;
    } 
    if (system_voltage_test() > 0) {
        log_printf("check system votage is error, test stop.\n");
        ControlPrintf("message:板件电压超范围\n");
        return -2;
    }
//step 2: read rtc group config and check, read rtc group temp, calculate group temp config
    ControlPrintf("message:正在计算config\n");
    flash_test_flag_manage(g_testFlag, FLAG_TEMP, FLASH_FLAG_READ);
    stepCount = 0;
    for (i = 0; i < MAX_CHIP_NUMBER; i++) {
        if (g_testFlag[i] == 1) {
            log_printf("temp offset:chip%d history success!\n", i + 1);
            UsbPrintf("temp offset:chip%d history success!\n", i + 1);
            stepCount++;
            continue;
        }
	    temp = temp_read_value(1);
        tempTest.setTemp = temp;
	    rtc_group_power(i + 1, POWER_CAP_TEST);
        HAL_Delay(500);
        if (check_history_download(i + 1) > 0) { continue; }
        if (rtc_8800_adc_testmode(i + 1) != I2C_OK) { 
            rtc_1006_exit_testmode(i + 1);
	        log_printf("chip%d enter testmode fail\n", i + 1);
	        UsbPrintf("chip%d enter testmode fail\n", i + 1);
            continue;
        } else {
            HAL_Delay(10);
            rtc_temp = system_get_rtc_temp(); 
            config[i] = rtc_convert_temp_offset(rtc_temp, temp);
            log_printf("chip%d temp adc: %.1f, offset:%d\n", i + 1, rtc_temp, config[i]);
            UsbPrintf("chip%d temp adc: %.1f, offset:%d\n", i + 1, rtc_temp, config[i]);
            if (config[i] >= RTC_TEMP_OFFSET_MIN && config[i] <= RTC_TEMP_OFFSET_MAX) {
                g_testFlag[i] = 1;
                stepCount++;
                tempTest.tempConfig = config[i];
                flash_test_data_write_add(&tempTest, i + 1, SET_TEMP);
                flash_test_data_write_add(&tempTest, i + 1, TEMP_CONFIG);
            }
            rtc_1006_exit_testmode(i + 1);
        }
        ControlPrintf("okNumber:%d chipName:%d\n", stepCount, i + 1);
    }
    
    flash_test_flag_manage(g_testFlag, FLAG_TEMP, FLASH_FLAG_WRITE_ADD);
    log_printf("system set rtc temp config end, success:%d.\n", stepCount);
    UsbPrintf("system set rtc temp config end, success:%d.\n", stepCount);
    return stepCount;
}


//下载函数
//先做下载前的检查工作
//最后调用download_temp_cap函数去烧录
int system_manage_download_cap(void)
{
    int i;
    int count = 0;
    
    memset(g_testFlag, 0xff, MAX_CHIP_NUMBER);
    flash_test_flag_manage(g_testFlag, FLAG_TEMPLOAD, FLASH_FLAG_READ);
    for (i = 0; i < MAX_CHIP_NUMBER; i++) {
        if (g_testFlag[i] == '1') {
            log_printf("chip %d history success\n", i + 1);
			ControlPrintf("okNumber:%d chipName:%d message:success\n", count, i + 1);
            count++;
            continue;
        }

#if (VERSION_SELECT == VERSION_SD8804)
        if (sd8804_write_otp(i + 1) != 0) {
#else
        if (sd8800_write_otp(i + 1) != 0) {
#endif
            count++;
            g_testFlag[i] = '1';
            log_printf("chip %d download success!\n", i + 1);
			ControlPrintf("okNumber:%d chipName:%d\n", count, i + 1);
        } else {//只对本次烧写成功的做记录，失败的不改写，留待重复尝试烧写
            log_printf("chip %d download fail!!!\n", i + 1);
			ControlPrintf("okNumber:%d chipName:%d message:fail\n", count, i + 1);
        }

        HAL_Delay(100);
    }
    
    flash_test_flag_manage(g_testFlag, FLAG_TEMPLOAD, FLASH_FLAG_WRITE_ADD);
    return count;
}

void system_set_int(uint8_t channel)
{
    static int lastChannel = -1;
    log_printf("test chip %d\n", channel);
    if (lastChannel > 0) {
        rtc_group_power(lastChannel, POWER_I2C);
        rtc_disable_int();
        rtc_1006_exit_testmode(channel);
    }
    
    if (channel != 0) {
        rtc_group_power(channel, POWER_I2C);
        rtc_1006_set_testmode(channel);
        rtc_out_4096();
    } 
    lastChannel = channel;
}

int check_system_temp(void)
{
    float temp;
    temp = temp_read_value(1);
    int num = 100;
    if (-45 <= temp && temp <= -35) {
        num = TEST_RTC_TEMP_F40;
    } else if (-15 <= temp && temp <= -5) {
        num = TEST_RTC_TEMP_F10;
    } else  if (20 <= temp && temp <= 40) {
        num = TEST_RTC_TEMP_Z29;
    } else if (55 <= temp && temp <= 65) {
        num = TEST_RTC_TEMP_Z60;
    } else if (80 <= temp && temp <= 90) {
        num = TEST_RTC_TEMP_Z85;
    }
    return num;
}


void systm_cap_save_buffer(uint8_t rtcNumber, float testTemp, float testCap)
{
    int i;

    if (rtcNumber == 0) {
        for (i = 0; i < MAX_CHIP_NUMBER; i++) {
           g_oneData.flag[i] = CAP_ADJUST_DEFAULT;
        }
    } else {
        g_oneData.flag[rtcNumber - 1] = CAP_ADJUST_SUCCESS;
        g_oneData.temp[rtcNumber - 1] = testTemp;
        g_oneData.cap[rtcNumber - 1] = testCap;
    }
}

uint8_t system_check_test_dot(uint8_t rtcNumber, uint8_t testNumber)
{
    uint32_t address;
    uint8_t buffer[4] = {0};
    
    address = TEST_RESULT_START_ADDR + (rtcNumber - 1) * TEST_RESULT_ONE_SIZE + TEST_DOT_TEMP + testNumber * 4;
    flash_read_data(address, buffer, 4);
    if (buffer[0] != 0xff || buffer[1] != 0xff || buffer[2] != 0xff || buffer[3] != 0xff) { return 1;}//重复测试不会更新数据
    return 0;
}

void system_cap_write_flash(void)
{
    uint32_t address;
    uint8_t buffer[4] = {0};
    int i;

    for (i = 0; i < MAX_CHIP_NUMBER; i++) {
        if (g_oneData.flag[i] != CAP_ADJUST_SUCCESS) { continue; }//测试不成功，不会写入flash
        if (system_check_test_dot(i + 1, g_oneData.number) > 0) { continue; }//防止重复测试不会更新数据
        address = TEST_RESULT_START_ADDR + i * TEST_RESULT_ONE_SIZE + TEST_DOT_TEMP + g_oneData.number * 4;
        float_to_uint8(buffer, &g_oneData.temp[i]);
        flash_write_data(address, buffer, 4);
        HAL_Delay(5);
        address = TEST_RESULT_START_ADDR + i * TEST_RESULT_ONE_SIZE + TEST_DOT_CAP + g_oneData.number * 4;
        float_to_uint8(buffer, &g_oneData.cap[i]);
        flash_write_data(address, buffer, 4);
        HAL_Delay(5);
    }
}
//计算基于2000年以后的时间秒
int system_tool_calculate_sec(RealTime time)
{
    int number;
    uint8_t dayInMon[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t i;
    int day = 0; 
    if (time.year >= 100 || time.year == 0) {
        day = 0;
    } else {
        day = 365 * time.year + ((time.year - 1) / 4);//补足闰年的天数
    }
    for (i = 0; i < 12; i++) {
        if ((i + 1) == time.mon) { break; }
        day += dayInMon[i];
    }
    day += time.day; 
    if ((time.year % 4) == 0 && time.mon >= 3) {
        day += 1;       //闰月＋1
    }
    number = time.sec + (uint32_t)time.min * 60 + (uint32_t)time.hour * 3600 + day * 24 * 3600;
    return number;
}

void system_manage_test_mode(void)
{
    int result;
    static uint8_t state = 0;
    static uint8_t count = 0;
    int i;
    switch (g_testMode) {
        case FREE_MODE:// free mode
            state = 0;
            count = 0;
        break;
        case  FREQ_CONNECT:
            count = 0;
            UsbPrintf("group i2c test:");
            for (i = 0; i < MAX_CHIP_NUMBER; i++) {
                if (i % 10 == 0) { UsbPrintf("\n"); }
		        rtc_group_power(i + 1, POWER_I2C);
                rtc_set_channel(i + 1, RTC_INIT_1006);
                HAL_Delay(5);
                if (rtc_read_multi_bytes(RTC_8800_ADDRESS, 0x00, &state, 1)== I2C_OK) {
                    UsbPrintf("1\t");
                    count++;
                } else {
                    UsbPrintf("0\t");
                }
            }
            UsbPrintf("\nok sum:%d\n", count);
            g_testMode = FREE_MODE;
        break;
        case TEMP_TEST:
            result = system_manage_temp();
            g_testMode = FREE_MODE;
            log_printf("success count:%d\n", result);
            if (result > 0) {
                ControlPrintf("okNumber:%d chipName:%d message:测试完成\n", result, MAX_CHIP_NUMBER);
            } else {
                ControlPrintf("message:测试异常，已停止\n");
            }
        break;
        case CAP_ADJUST://调整电容以使得该温度点的误差最小
            switch (state) {
                case 0:
                    count = 0;
                    if (temp_read_value(1) > 115) { //客户宁德时代提的125度测试，实际不进行测试
	                    ControlPrintf("message:开始进行125度测试");
                        HAL_Delay(1500);
	                    ControlPrintf("message:125度测试完成");
                        g_testMode = FREE_MODE;
                        break;
                    }
                    g_oneData.number = check_system_temp();
                    UsbPrintf("system temp:%.2f, test dot:%d\n", temp_read_value(1), g_oneData.number);
                    if (g_oneData.number > TEST_RTC_TEMP_Z85) {
                        g_testMode = FREE_MODE;
                        log_printf("system temp %.2f can't test!\n", temp_read_value(1));
                    } else {
                        state = 1;
                        systm_cap_save_buffer(0, 0, 0);//init buffer
                        memset(g_testFlag, 0xff, MAX_CHIP_NUMBER);
                        flash_test_flag_manage(g_testFlag, FLAG_TEST_DOT, FLASH_FLAG_READ);
                        if (g_oneData.number == TEST_RTC_TEMP_Z85) {
                            AddOneTestNumberFlash();
                        }
                    }
                break;
                case 1:
                    count++;
                    if (count <= MAX_CHIP_NUMBER) {
                        if (system_check_test_dot(count, g_oneData.number) > 0) { break; }
                        log_printf("start test chip %d\n", count);
                        UsbPrintf("start test chip %d\n", count);
                        g_capAdjust.flag = CAP_ADJUST_DEFAULT;
                        g_capAdjust.count = count;
                        rtc_group_power(count, POWER_CAP_TEST);
                        HAL_Delay(500);
                        if (check_history_download(count) > 0) { break; }
                        rtc_8800_adc_testmode(count);
                        HAL_Delay(50);
                        g_capAdjust.temp = system_get_rtc_temp();
                        rtc_1006_exit_testmode(count);
                        rtc_1006_set_testmode(count);
                        rtc_out_4096();
                        rtc_freq_test(FREQ_TEST_STARTING);
                        state = 2;
                    } else {
                        system_cap_write_flash();//write flash
                        flash_test_flag_manage(g_testFlag, FLAG_TEST_DOT, FLASH_FLAG_WRITE_ADD);
                        result = 0;
                        for (i = 0; i < MAX_CHIP_NUMBER; i++) {
                            if ((g_testFlag[i] & (0x01 << g_oneData.number)) == 0) { result++; }
                        }
                        ControlPrintf("okNumber:%d message:单点测试完成\n", result, count);
                        g_testMode = FREE_MODE;
                        log_printf("success count:%d\n", result);
                        UsbPrintf("success count:%d\n", result);
                    }
                break;
                case 2:
                    if (g_capAdjust.flag != CAP_ADJUST_DEFAULT) {//只要状态改变，就进行下一个芯片测试，不管是否成功
                        state = 1;
                        if (g_capAdjust.flag == CAP_ADJUST_SUCCESS) {//先进行1次数据校验，若校验不合格就不保存
                            systm_cap_save_buffer(count, g_capAdjust.temp, g_capAdjust.cap);// save buffer
                            ControlPrintf("chipTemp:%d chipName:%d message:OK\n", (int)g_capAdjust.temp, count);
                            log_printf("count:%d, temp:%.1f, cap:%.2f\n", count, g_capAdjust.temp, g_capAdjust.cap);
                            g_testFlag[count - 1] &= (~(0x01 << g_oneData.number));//对某一个芯片的测试点数据标志位进行bit位的置0
                        } else {
                            ControlPrintf("chipName:%d message:fail!\n", count);
                        }
                        rtc_disable_int();
                        rtc_1006_exit_testmode(count);
                    }
                break;
                default:break;
            }
        break;
        case CAP_ADJUST_COVER:
            g_oneData.number = check_system_temp();
            if (g_oneData.number > TEST_RTC_TEMP_Z85) {
                g_testMode = FREE_MODE;
                log_printf("system temp can't test!\n");
                ControlPrintf("message:系统温度超范围\n");
                log_printf("system temp %.2f can't test!\n", temp_read_value(1));
            } else {
                log_printf("system temp test cover!\n");
                flash_test_data_clear(g_oneData.number + FLASH_CLEAR_DOT_0);
                flash_test_flag_manage(g_testFlag, FLAG_TEST_DOT, FLASH_FLAG_READ);
                for (i = 0; i < MAX_CHIP_NUMBER; i++) {
                    g_testFlag[i] |= (0x01 << g_oneData.number);
                }
                flash_test_flag_manage(g_testFlag, FLAG_TEST_DOT, FLASH_FLAG_WRITE_COVER);
                g_testMode = CAP_ADJUST;
            }
        break;
        case CALCULATE:
            count = 0;
            for (i = 0; i < MAX_CHIP_NUMBER; i++) {
                if (rtc_calculate_cap(i + 1) != 0) {
                    count++;
                    g_testFlag[i] = '1';
                } else {
                    g_testFlag[i] = 0xff;
                }
            }
            flash_test_flag_manage(g_testFlag, FLAG_CALCULATE, FLASH_FLAG_WRITE_ADD);
            g_testMode = FREE_MODE;
            log_printf("calculate success number %d\n", count);
			ControlPrintf("okNumber:%d chipName:%d message:计算完成！\n", count, MAX_CHIP_NUMBER);
        break;
        case CAP_DOWNLOAD:
            result = system_manage_download_cap();
            log_printf("download success number %d\n", result);
			ControlPrintf("okNumber:%d chipName:%d message:烧录完成！\n", result, MAX_CHIP_NUMBER);
            g_testMode = FREE_MODE;
        break;
        case SYNC_RTC_TIME:
			ControlPrintf("message:暂不支持该功能\n");
            g_testMode = FREE_MODE;
        break;
        case CHECK_ERROR_TIME:
			ControlPrintf("message:暂不支持该功能\n");
            g_testMode = FREE_MODE;
        break;
        case MACHINE_TEST://仅测试，不写入
			ControlPrintf("message:暂不支持该功能\n");
            g_testMode = FREE_MODE;
        break;
        case MACHINE_COVER://先清空，测试后写入
			ControlPrintf("message:暂不支持该功能\n");
            g_testMode = FREE_MODE;
        break;
        case MACHINE_WRITE://测试后写入
			ControlPrintf("message:暂不支持该功能\n");
            g_testMode = FREE_MODE;
        break;
        default: break;
    }
}


