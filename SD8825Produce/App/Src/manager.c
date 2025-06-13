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

#define SD8800_CENTER_TEMP_UP                   30//30��
#define SD8800_CENTER_TEMP_DOWN                 15//15��

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
    // uint8_t mode;
	// uint32_t voltage;
    uint8_t result = 0;
	// voltage = ststem_voltage(CHANNEL_SW) * 3300 * 4.6 / 4096;
	// log_printf("vdd_sw:%dmV,", voltage);
    // if (voltage > 2000) {
    //     mode = 1;
    // } else {
    //     mode = 0;
    // }
	// voltage = ststem_voltage(CHANNEL_3V3) * 3300 * 4.6 / 4096;
	// log_printf("3.3V:%dmV,", voltage);
    // if (mode == 0 && voltage < 3100 && voltage > 3500) {result++;}
	// voltage = ststem_voltage(CHANNEL_5V) * 3300 * 4.6 / 4096;
	// log_printf("5V:%dmV,", voltage);
    // if (mode == 1 && voltage < 4600 && voltage > 5400) {result++;}
	// voltage = ststem_voltage(CHANNEL_9V) * 3300 * 4.6 / 4096;
	// log_printf("9V:%dmV,", voltage);
    // if (voltage < 9000) {
    //     result += 10;
    // }
	// voltage = ststem_voltage(CHANNEL_6V5) * 3300 * 4.6 / 4096;
	// log_printf("6.5V:%dmV\n", voltage);
    // if (voltage < 6250 || voltage > 6750) {
    //     result += 100;
    // }
    return result;
} 

int system_manage_temp(void)
{
	float temp;
    int i;
    float rtc_temp = 0;
    uint16_t config[MAX_CHIP_NUMBER] = {0};
    // uint16_t adcValue;
    uint8_t stepCount;
    TestResult tempTest = {0};

//step 1: read system temp ship, and check system voltage ok
	temp = temp_read_value(1);
    log_printf("temp1 value %.1fC\n", temp);
    UsbPrintf("temp1 value %.1fC\n", temp);
    if (temp > SD8800_CENTER_TEMP_UP || temp < SD8800_CENTER_TEMP_DOWN) {
        log_printf("check system temp is error, test stop.\n");
        UsbPrintf("check system temp is error, test stop.\n");
        ControlPrintf("message:ϵͳ�¶ȳ���Χ\n");
        return -1;
    } 
//step 2: read rtc group config and check, read rtc group temp, calculate group temp config
    rtc_group_power(1, POWER_I2C);
    HAL_Delay(500);
    ControlPrintf("message:���ڼ���...\n");
    flash_test_flag_manage(g_testFlag, FLAG_TEMP, FLASH_FLAG_READ);
    stepCount = 0;
    for (i = 0; i < MAX_CHIP_NUMBER; i++) {
	    UsbPrintf("temp offset:chip%d\n", i + 1);
	    log_printf("temp offset:chip%d\n", i + 1);
        if (g_testFlag[i] == 1) {
            UsbPrintf("history success!\n");
            log_printf("history success!\n");
            stepCount++;
            continue;
        }
	    temp = temp_read_value(1);
        tempTest.setTemp = temp;
	    rtc_group_power(i + 1, POWER_I2C);
        HAL_Delay(500);
         if (rtc_1006_set_testmode(i + 1) != I2C_OK) { 
            rtc_1006_exit_testmode(i + 1);
	        UsbPrintf("enter testmode fail\n");
	        log_printf("enter testmode fail\n");
            continue;
        }
        HAL_Delay(50);
        // adcValue = 0;
        // rtc_temp = 0;
        // for (j = 0; j < 5; j++) {
        //     if (wave1006_read_temp_adc(&adcValue)) {
        //         rtc_temp = adcValue;
        //         break;
        //     }
        // }
        rtc_temp = system_get_rtc_temp(); 
        config[i] = rtc_convert_temp_offset(rtc_temp, temp);
        UsbPrintf("temp adc: %.1f, offset:%d\n", rtc_temp, config[i]);
        log_printf("temp adc: %.1f, offset:%d\n", rtc_temp, config[i]);
        if (config[i] >= 50 && config[i] <= 140) {
            g_testFlag[i] = 1;
            stepCount++;
            tempTest.tempConfig = config[i];
            flash_test_data_write_add(&tempTest, i + 1, SET_TEMP);
            flash_test_data_write_add(&tempTest, i + 1, TEMP_CONFIG);
        } else {
            log_printf("check offset error!\n");
        }
        rtc_1006_exit_testmode(i + 1);
	    // rtc_group_power(i + 1, POWER_OFF);
	    ControlPrintf("temp:%.1f connect OK\n", temp_read_value(1));
        HAL_Delay(100);
    }
    
    flash_test_flag_manage(g_testFlag, FLAG_TEMP, FLASH_FLAG_WRITE_ADD);
    UsbPrintf("system set rtc group temp config end.\n");
    return stepCount;
}


//���غ���
//��������ǰ�ļ�鹤��
//������download_temp_cap����ȥ��¼
int system_manage_download_cap(void)
{
    int i;
    int count = 0;
    
    memset(g_testFlag, 0xff, MAX_CHIP_NUMBER);
    flash_test_flag_manage(g_testFlag, FLAG_TEMPLOAD, FLASH_FLAG_READ);
    for (i = 0; i < MAX_CHIP_NUMBER; i++) {
        if (g_testFlag[i] == '1') {
            log_printf("chip %d history success\n", i + 1);
            UsbPrintf("chip %d history success\n", i + 1);
			ControlPrintf("okNumber:%d chipName:%d message:success\n", count, i + 1);
            count++;
            continue;
        }

	    rtc_group_power(i + 1, POWER_I2C);
        HAL_Delay(500);
        if (sd8825_write_otp(i + 1) != 0) {
            count++;
            g_testFlag[i] = '1';
			ControlPrintf("okNumber:%d chipName:%d message:success\n", count, i + 1);
            log_printf("chip %d success\n", i + 1);
            UsbPrintf("chip %d success\n", i + 1);
        } else {//ֻ�Ա�����д�ɹ�������¼��ʧ�ܵĲ���д�������ظ�������д
			ControlPrintf("okNumber:%d chipName:%d message:fail\n", count, i + 1);
        }

    }
    
    flash_test_flag_manage(g_testFlag, FLAG_TEMPLOAD, FLASH_FLAG_WRITE_ADD);
    return count;
}

void system_set_int(uint8_t channel)
{
    static int lastChannel = -1;
    // int i;
    UsbPrintf("test chip %d\n", channel);
    log_printf("test chip %d\n", channel);

    if (lastChannel > 0) {
        rtc_group_power(lastChannel, POWER_I2C);
        sd8825_close_freq();
        rtc_1006_exit_testmode(lastChannel);
    }
    
    if (channel != 0) {
        rtc_group_power(channel, POWER_I2C);
        rtc_1006_set_testmode(channel);
        sd8825_out_freq();
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
    if (buffer[0] != 0xff || buffer[1] != 0xff || buffer[2] != 0xff || buffer[3] != 0xff) { return 1;}//�ظ����Բ����������
    return 0;
}

void system_cap_write_flash(void)
{
    uint32_t address;
    uint8_t buffer[4] = {0};
    int i;

    for (i = 0; i < MAX_CHIP_NUMBER; i++) {
        if (g_oneData.flag[i] != CAP_ADJUST_SUCCESS) { continue; }//���Բ��ɹ�������д��flash
        if (system_check_test_dot(i + 1, g_oneData.number) > 0) { continue; }//��ֹ�ظ����Բ����������
        // HAL_Delay(1);
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
//�������2000���Ժ��ʱ����
int system_tool_calculate_sec(RealTime time)
{
    int number;
    uint8_t dayInMon[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t i;
    int day = 0; 
    if (time.year >= 100 || time.year == 0) {
        day = 0;
    } else {
        day = 365 * time.year + ((time.year - 1) / 4);//�������������
    }
    for (i = 0; i < 12; i++) {
        if ((i + 1) == time.mon) { break; }
        day += dayInMon[i];
    }
    day += time.day; 
    if ((time.year % 4) == 0 && time.mon >= 3) {
        day += 1;       //���£�1
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
                rtc_group_power(i + 1, POWER_TEST);
                rtc_set_channel(i + 1, RTC_SD8825);
                HAL_Delay(100);
                if (rtc_read_multi_bytes(RTC_3900_ADDRESS, 0x00, &state, 1)== I2C_OK) {
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
            UsbPrintf("success count:%d\n", result);
            log_printf("success count:%d\n", result);
            if (result > 0) {
                ControlPrintf("okNumber:%d chipName:%d message:�������\n", result, MAX_CHIP_NUMBER);
            }
        break;
        case CAP_ADJUST://����������ʹ�ø��¶ȵ�������С
            switch (state) {
                case 0:
                    count = 0;
                    g_oneData.number = check_system_temp();
                    UsbPrintf("system temp:%.2f, test dot:%d\n", temp_read_value(1), g_oneData.number);
                    if (g_oneData.number > TEST_RTC_TEMP_Z85) {
                        g_testMode = FREE_MODE;
                        UsbPrintf("system temp can't test!\n");
                        log_printf("system temp %.2f can't test!\n", temp_read_value(1));
                    } else {
                        state = 1;
                        systm_cap_save_buffer(0, 0, 0);//init buffer
                        memset(g_testFlag, 0xff, MAX_CHIP_NUMBER);
                        flash_test_flag_manage(g_testFlag, FLAG_TEST_DOT, FLASH_FLAG_READ);
                    }
                break;
                case 1:
                    count++;
                    if (count <= MAX_CHIP_NUMBER) {
                        if (system_check_test_dot(count, g_oneData.number) > 0) { break; }
                        UsbPrintf("test chip:%d\n", count);
                        g_capAdjust.flag = CAP_ADJUST_DEFAULT;
                        g_capAdjust.count = count;
                        rtc_group_power(count, POWER_I2C);
                        HAL_Delay(500);
                        // rtc_8825_adc_testmode(count);//����ADCģʽ
                        rtc_1006_set_testmode(count);//���������ģʽ
                        HAL_Delay(20);
                        g_capAdjust.temp = system_get_rtc_temp();
                        // rtc_1006_exit_testmode(count);//���˳�����ģʽ
                        // rtc_1006_set_testmode(count);//�����½��������ģʽ
                        sd8825_out_freq();
                        HAL_Delay(100);
                        rtc_freq_test(FREQ_TEST_STARTING);
                        state = 2;
                    } else {
                        system_cap_write_flash();//write flash
                        flash_test_flag_manage(g_testFlag, FLAG_TEST_DOT, FLASH_FLAG_WRITE_ADD);
                        result = 0;
                        for (i = 0; i < MAX_CHIP_NUMBER; i++) {
                            if ((g_testFlag[i] & (0x01 << g_oneData.number)) == 0) { result++; }
                        }
                        ControlPrintf("okNumber:%d message:����������\n", result);
                        g_testMode = FREE_MODE;
                        UsbPrintf("cap adjust test end.\n");
                        log_printf("success count:%d\n", result);
                    }
                break;
                case 2:
                    if (g_capAdjust.flag != CAP_ADJUST_DEFAULT) {//ֻҪ״̬�ı䣬�ͽ�����һ��оƬ���ԣ������Ƿ�ɹ�
                        state = 1;
                        if (g_capAdjust.flag == CAP_ADJUST_SUCCESS) {//�Ƚ���1������У�飬��У�鲻�ϸ�Ͳ�����
                            systm_cap_save_buffer(count, g_capAdjust.temp, g_capAdjust.cap);// save buffer
                            ControlPrintf("chipTemp:%d chipName:%d message:OK\n", (int)g_capAdjust.temp, count);
                            UsbPrintf("count:%d, temp:%.1f, cap:%.2f\n", count, g_capAdjust.temp, g_capAdjust.cap);
                            g_testFlag[count - 1] &= (~(0x01 << g_oneData.number));//��ĳһ��оƬ�Ĳ��Ե����ݱ�־λ����bitλ����0
                        } else {
                            ControlPrintf("chipName:%d message:fail!\n", count);
                        }
                        sd8825_close_freq();
                        rtc_1006_exit_testmode(count);
                        // rtc_group_power(0, POWER_OFF);
                        HAL_Delay(100);
                    }
                break;
                default:break;
            }
        break;
        case CAP_ADJUST_COVER:
            g_oneData.number = check_system_temp();
            if (g_oneData.number > TEST_RTC_TEMP_Z85) {
                g_testMode = FREE_MODE;
                UsbPrintf("system temp can't test!\n");
                ControlPrintf("message:ϵͳ�¶ȳ���Χ\n");
                log_printf("system temp %.2f can't test!\n", temp_read_value(1));
            } else {
                UsbPrintf("system temp test cover!\n");
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
                if (sd8825_calcul_cap(i + 1) != 0) {
                    count++;
                    g_testFlag[i] = '1';
                } else {
                    g_testFlag[i] = 0xff;
                }
            }
            flash_test_flag_manage(g_testFlag, FLAG_CALCULATE, FLASH_FLAG_WRITE_ADD);
            g_testMode = FREE_MODE;
            UsbPrintf("calculate success number %d\n", count);
            log_printf("calculate success number %d\n", count);
			ControlPrintf("okNumber:%d chipName:%d message:������ɣ�\n", count, MAX_CHIP_NUMBER);
        break;
        case CAP_DOWNLOAD:
            result = system_manage_download_cap();
            UsbPrintf("download success number %d\n", result);
            log_printf("download success number %d\n", result);
			ControlPrintf("okNumber:%d chipName:%d message:��¼��ɣ�\n", result, MAX_CHIP_NUMBER);
            g_testMode = FREE_MODE;
        break;
        case SYNC_RTC_TIME:
            g_testMode = FREE_MODE;//����Ŀû�д˹�������
        break;
        case CHECK_ERROR_TIME://����Ŀû�д˹�������
            g_testMode = FREE_MODE;
        break;
        case MACHINE_TEST://�����ԣ���д��
            if (count == 0) {
                for (i = 0; i < MAX_CHIP_NUMBER; i++) {
                    g_testFlag[i] = 0xff;
                }
                count = 1;
                system_machine_test_one(count, 1);//����һ��оƬ�Ŀ�ʼ
            } else if (count <= MAX_CHIP_NUMBER) {
                result = system_machine_test_one(count, 0);//�������Խ���
                if (result != 0) {//��Ϊ0�ű�ʾ����һ��оƬ����
                    if (result > 0) {
                        g_testFlag[count - 1] = '1';
                        result = 0;
                        for (i = 0; i < count; i++) {
                            if (g_testFlag[i] == '1') { result++; }
                        }
                        ControlPrintf("okNumber:%d chipName:%d message:OK\n", result, count);
                    } else {
                        g_testFlag[count - 1] = 0xff;
                        ControlPrintf("chipName:%d message:fail\n", count);
                    }
                    if (count < MAX_CHIP_NUMBER) {//��оƬ������ɣ�������һ������
                        count++;
                        system_machine_test_one(count, 1);
                    } else {
                        count = 0;
                        ControlPrintf("message:�������\n");
                        g_testMode = FREE_MODE;
                        flash_test_flag_manage(g_testFlag, FLAG_MACHINE, FLASH_FLAG_WRITE_ADD);
                        break;
                    }
                }
            }
        break;
        case MACHINE_COVER://����գ����Ժ�д��
            for (i = 0; i < MAX_CHIP_NUMBER; i++) {
                g_testFlag[i] = 0xff;
            }
            flash_test_flag_manage(g_testFlag, FLAG_MACHINE, FLASH_FLAG_WRITE_COVER);
            flash_test_data_clear(FLASH_CLEAR_MACHINE);
            g_testMode = MACHINE_TEST;
            count = 0;
        break;
        case MACHINE_WRITE://���Ժ�д��,��Ϊ30xxϵ��оƬ������Ŀ��1006�˵�û��
        break;
        default: break;
    }
}


