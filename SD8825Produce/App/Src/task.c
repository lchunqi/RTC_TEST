#include "main.h"
#include "task.h"
#include "manager.h"
#include "rtc.h"
#include "temp.h"
#include "math.h"
#include "power.h"
#include "current.h"
#include "flash.h"
#include "calculate.h"

RealTime g_realTime = {0};
FrequencyData g_lastFreq = {0};
extern CapAdjust g_capAdjust;
TestResult g_machineTest = {0};

void LedHeart(void)
{
    static uint32_t ledTick = 0;
    if (HAL_GetTick() > ledTick + 1000) {
        ledTick = HAL_GetTick();
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
    }
}


void system_time_update(void)
{
    static uint32_t lastTick = 0;

    g_realTime.tick += HAL_GetTick() - lastTick;
    lastTick = HAL_GetTick();
    if (g_realTime.tick < 1000) {
        return;
    }
    g_realTime.tick -= 1000;
    g_realTime.sec++;
    if (g_realTime.sec >= 60) {
        g_realTime.sec = 0;
        g_realTime.min++;
        if (g_realTime.min >= 60) {
            g_realTime.min = 0;
            g_realTime.hour++;
            if (g_realTime.hour >= 24) {
                g_realTime.hour = 0;
                g_realTime.day++;
                if ((g_realTime.mon == 2 && g_realTime.day >= 29) || 
                    ((g_realTime.mon == 4 || g_realTime.mon == 6 || g_realTime.mon == 9 || g_realTime.mon == 11) && g_realTime.day >= 31)
                    || g_realTime.day >= 32) {//不考虑闰年了，每小时会有信息校正过来
                    g_realTime.mon++;
                    g_realTime.day = 1;
                    if (g_realTime.mon >= 13) {
                        g_realTime.mon = 1;
                        g_realTime.year++;
                    }
                }
            }
        }
    }
}

void system_set_real_time(RealTime time)
{
    g_realTime.tick = 0;
    g_realTime.sec = time.sec;
    g_realTime.min = time.min;
    g_realTime.hour = time.hour;
    g_realTime.day = time.day;
    g_realTime.mon = time.mon;
    g_realTime.year = time.year;
	log_printf("\ncommand set time, 20%d/%d/%d %d:%d:%d\n", 
    g_realTime.year, g_realTime.mon, g_realTime.day, g_realTime.hour, g_realTime.min, g_realTime.sec);
}

void system_get_real_time(RealTime* time)
{
    time->sec = g_realTime.sec;
    time->min = g_realTime.min;
    time->hour = g_realTime.hour;
    time->day = g_realTime.day;
    time->mon = g_realTime.mon;
    time->year = g_realTime.year;
}

void system_manage(void)
{
    static uint32_t systemTick = 0;

    systemTick++;
    if (systemTick % 100 == 0) {
        LedHeart();
    }
    if (systemTick % 100 == 0) {
        rtc_freq_test(FREQ_TEST_DEFAULT);
    }
    if (systemTick % 20 == 0) {
        system_manage_test_mode();
    }
    if (systemTick % 200 == 0) {//系统本地走时
        system_time_update();
    }
    if (systemTick % 4000 == 0) {
	    ControlPrintf("temp:%.1f connect OK\n", temp_read_value(1));
    }
}


void rtc_freq_save(uint8_t mode,  float cap, float ppm)
{
    float temp, rtcTemp = 0, centerTemp;
    uint32_t address;
    uint8_t buffer[4] = {0};
    uint8_t check = 0;


    address = TEST_RESULT_START_ADDR + (g_capAdjust.count - 1) * TEST_RESULT_ONE_SIZE + TEST_DOT_TEMP + TEST_RTC_TEMP_Z29 * 4;//计算第n个芯片的中心温度点电容的保存地址
    flash_read_data(address, buffer, 4);//根据地址调用flash函数读取4byte数据
    uint8_to_float(&centerTemp, buffer);//类型转换为float
    if (mode == CAP_ADJUST_SUCCESS) {
        temp = temp_read_value(1);// read system temp
        rtcTemp = g_capAdjust.temp;
    
        if (temp < 35 && temp > 15) {
            if ((rtcTemp * 0.5 - temp) > 70 && (rtcTemp * 0.5 - temp) < 130) {                
                check = 1;
            } 
        } else {
            if ((centerTemp - rtcTemp) * 0.5 > (25.0 - temp - 8) && (centerTemp - rtcTemp) * 0.5 < (25.0 - temp + 8)) {
                check = 1;
            } 
        }
        if (check == 1) {
        // g_capAdjust.flag = CAP_ADJUST_SUCCESS;  等待中心温度点计算
            g_capAdjust.cap = cap;
            g_capAdjust.ppm = ppm;
            UsbPrintf("success, temp:%.2f, best cap:%.2f, min ppm %.2f\n", g_capAdjust.temp, g_capAdjust.cap,  g_capAdjust.ppm);
            log_printf("success, temp:%.2f, best cap:%.2f, min ppm %.2f\n", g_capAdjust.temp, g_capAdjust.cap,  g_capAdjust.ppm);
        } else {
            g_capAdjust.flag = CAP_ADJUST_ERROR;
            g_capAdjust.cap = 0;
            g_capAdjust.ppm = 1000;
            UsbPrintf("check rtc temp error, temp:%.2f\n", rtcTemp);
            log_printf("check rtc temp error, temp:%.2f\n", rtcTemp);
        }
    } 

    if (mode == CAP_ADJUST_ERROR) {
        g_capAdjust.flag = CAP_ADJUST_ERROR;
        g_capAdjust.cap = 0;
        g_capAdjust.ppm = 1000;
        UsbPrintf("error and stop, temp:%.2f, best cap:%.2f, min ppm %.2f\n", g_capAdjust.temp, g_capAdjust.cap,  g_capAdjust.ppm);
        log_printf("error and stop, temp:%.2f, best cap:%.2f, min ppm %.2f\n", g_capAdjust.temp, g_capAdjust.cap,  g_capAdjust.ppm);
    }
}


void rtc_freq_test(int mode)//调节电容测试
{
    static int testState = 0;
    static uint8_t responseCount = 0;
    static uint32_t lastTestTime = 0;
    float temp, ppm;
	uint8_t cap[2] = {0};
    static float lastCap = 11;
    static float minPpm = 1000;
    static float minCap = 0;
    static uint8_t adjustCount = 0;
    uint32_t address;
    uint8_t buffer[4] = {0xff};

    if (mode == FREQ_TEST_STARTING) {
        testState = 1;
        return;
    }

    switch (testState) {
        case 0:  break;
        case 1:
            temp = temp_read_value(1);// read system temp
            lastCap = rtc_init_test_cap(temp);
            cap_list_to_uint(lastCap, cap);
            if (rtc_set_testcap(cap) == I2C_OK) {
                UsbPrintf("system temp:%.1f set init cap %.0f, adjust start...\n", temp, lastCap);
                minPpm = 1000;
                testState = 2;
                adjustCount = 1;
            } else {
                UsbPrintf("cap adjust fail, i2c error\n");
                log_printf("cap adjust fail, i2c error\n");
                g_capAdjust.flag = CAP_ADJUST_ERROR;
                testState = 0;
            }
        break;
        case 2:
            lastTestTime = HAL_GetTick();
            g_lastFreq.recordTime = lastTestTime;
            DevicePrintf(":MEAS?\n");
            responseCount = 0;
            testState = 3;
        break;
        case 3:
            if (g_lastFreq.recordTime > lastTestTime) {
                ppm = (g_lastFreq.freq / CENTER_FREQENCY - 1) * 1000000;
                UsbPrintf("cap[%.1f], %fHz, %.2fppm\n", lastCap, g_lastFreq.freq, ppm);
                if (fabs(ppm) <= 0.5) {
                    testState = 4;
                    rtc_freq_save(CAP_ADJUST_SUCCESS, lastCap, ppm);
                    break;
                } else if (fabs(ppm) < 3) {
                    if (fabs(ppm) < fabs(minPpm)) {
                        minPpm = ppm;
                        minCap = lastCap;
                    }
                    if ((ppm > 0 && minPpm < 0) || (ppm < 0 && minPpm > 0)) {
                        testState = 4;
                        rtc_freq_save(CAP_ADJUST_SUCCESS, minCap, minPpm);
                        break;
                    }
                } else {
                    if ((lastCap >= SD8800_CAP_LIST_MAX && ppm >= 2) || (lastCap <= SD8800_CAP_LIST_MIN  && ppm <= -2)) {//fail and stop
                        rtc_freq_save(CAP_ADJUST_ERROR, lastCap, minPpm);
                        testState = 0;
                        break;
                    } 
                }
                if (fabs(ppm) < fabs(minPpm)) {
                    minPpm = ppm;
                    minCap = lastCap;
                }
                //---------------异常次数限制，避免卡死在该环节-----------------------
                adjustCount++;
                if (adjustCount > 10) { 
                    rtc_freq_save(CAP_ADJUST_ERROR, minCap, minPpm); 
                    testState = 0;
                    break;
                }//---------------------------------------------------------
                if (0 < ppm && ppm < 1) {
                    lastCap += 1;
                } else if (-1 < ppm && ppm < 0) {
                    lastCap -= 1;
                } else {
                    lastCap = lastCap + ppm * 2.5;
                }
                cap_list_to_uint(lastCap, cap);
                rtc_set_testcap(cap);
                testState = 2;
            } else if (lastTestTime + 1200 < HAL_GetTick()) {
                UsbPrintf("device no response\n");
                log_printf("device no response\n");             
                responseCount++;
                if (responseCount >= 3) {//stop test
                    testState = 0;
                    rtc_freq_save(CAP_ADJUST_ERROR, 0, 1000);
                } else {
                    lastTestTime = HAL_GetTick();
                }
            }
        break;
        case 4:
            temp = temp_read_value(1);
            if ((temp < -35 && temp > -45) || (temp < 90 && temp > 80)) {
                address = TEST_RESULT_START_ADDR + (g_capAdjust.count - 1) * TEST_RESULT_ONE_SIZE + TEST_DOT_CAP + TEST_RTC_TEMP_Z29 * 4;//计算第n个芯片的中心温度点电容的保存地址
                flash_read_data(address, buffer, 4);//根据地址调用flash函数读取4byte数据
                uint8_to_float(&lastCap, buffer);//类型转换为float
                cap_list_to_uint(lastCap, cap);//查表转换为实际电容
                if (rtc_set_testcap(cap) == I2C_OK) {//IIC写入rtc
                    testState++;
                } else {
                    rtc_freq_save(CAP_ADJUST_ERROR, lastCap, minPpm);
                    testState = 0;
                }
            } else {//不是-10和60度的点，调试正常退出
                g_capAdjust.flag = CAP_ADJUST_SUCCESS;
                testState = 0;
            }
        break;
        case 5:
            lastTestTime = HAL_GetTick();
            g_lastFreq.recordTime = lastTestTime;
            DevicePrintf(":MEAS?\n");
            responseCount = 0;
            testState++;
        break;
        case 6:
            if (g_lastFreq.recordTime > lastTestTime) {
                ppm = (g_lastFreq.freq / CENTER_FREQENCY - 1) * 1000000;
                address = TEST_RESULT_START_ADDR + (g_capAdjust.count - 1) * TEST_RESULT_ONE_SIZE + TEST_CENTER;//计算第n个芯片的的保存地址
                temp = temp_read_value(1);
                if (temp > 80) { address += 4;}
                float_to_uint8(buffer, &ppm);
                flash_write_data(address, buffer, 4);
                UsbPrintf("center cap in %.1f : %.2fppm\n", temp, ppm);
                log_printf("center cap in %.1f : %.2fppm\n", temp, ppm);
                g_capAdjust.flag = CAP_ADJUST_SUCCESS;
                testState = 0;
            } else if (lastTestTime + 1200 < HAL_GetTick()) {
                UsbPrintf("device no response\n");
                log_printf("device no response\n");             
                responseCount++;
                if (responseCount >= 3) {//stop test
                    testState = 0;
                    rtc_freq_save(CAP_ADJUST_ERROR, 0, 1000);
                } else {
                    lastTestTime = HAL_GetTick();
                }
            }
        break;
        default:  break;
    }
}

//---------------------------------------------
//system_machine_test_one函数，机台测试一个芯片
//number选定芯片序号，mode分为启动模式和运转模式
//除非是I2C不通，否则不管什么状态的芯片都会测试电流
//---------------------------------------------
int system_machine_test_one(uint8_t number, uint8_t mode)
{
	float ppm, cur;
	// uint8_t buffer[16] = {0};
	uint8_t i2c_result;
	static uint8_t result = 1;
    static uint8_t state = 0;
    static uint32_t timeout;
    static uint8_t testNumber = 0;
    static uint32_t startTime;
    float rtcTemp = 0;
    if (mode == 1) {//在设置开始时就设定测试序号，后续不能变更
        state = 1;
        testNumber = number;
        g_machineTest.machine.Ivdd = 0;
        g_machineTest.machine.Ivbat = 0xff;
        g_machineTest.machine.config9 = 0xff;
        g_machineTest.machine.configC = 0xff;
        g_machineTest.machine.sysTemp = 0;
        g_machineTest.machine.rtcTemp = 0;
        g_machineTest.machine.testFreq = 0;
        g_machineTest.machine.vbat_2V5 = 0;
        g_machineTest.machine.charge = 0;
        startTime = HAL_GetTick();
        UsbPrintf("machine test chip:%d\n", number);
        log_printf("machine test chip:%d\n", number);
    }

    switch (state) {
        case 1:
	        rtc_group_power(testNumber, POWER_I2C);
            result = 1; 
            timeout = HAL_GetTick();
            state++;
        break;
        case 2:
            if (HAL_GetTick() < (timeout + 500)) {//上电500ms以后开始测量
                break;
            }
            i2c_result = rtc_read_temp(&rtcTemp);
            if (i2c_result != I2C_OK) {
                UsbPrintf("chip%d i2c error, test end!\n", testNumber);
                log_printf("chip%d i2c error!\n", testNumber);
                return -1;
            }
                g_machineTest.machine.sysTemp = (int)temp_read_value(1);
                g_machineTest.machine.rtcTemp = (int)rtcTemp;
                if (g_machineTest.machine.rtcTemp > (g_machineTest.machine.sysTemp + 5) 
                    || g_machineTest.machine.rtcTemp < (g_machineTest.machine.sysTemp - 5)) {
                    result = 0;//温度测试异常，此次测试ng，但是会继续测试其他项目
                }
                state++;
        break;
//-----------------------------测试频率------------------------------------------
        case 3:	
	        sd8825_out_freq();//烧写完成以后的输出频率和之前不同            
            timeout = HAL_GetTick();
            state++;
        break;
        case 4:
            if (HAL_GetTick() > timeout + 100) {
	            DevicePrintf(":MEAS?\n");
                timeout = HAL_GetTick();
                g_lastFreq.recordTime = timeout;
                state++;
            }
        break;
        case 5:
            if (g_lastFreq.recordTime > timeout) {//有新的数据更新，计算误差
                g_machineTest.machine.testFreq = g_lastFreq.freq;
                ppm = (g_machineTest.machine.testFreq / CENTER_FREQENCY - 1) * 1000000;
                UsbPrintf("%f, ppm:%.2f\n", g_machineTest.machine.testFreq, ppm);
                log_printf("%f, ppm:%.2f\n", g_machineTest.machine.testFreq, ppm);
                if (ppm < -2 || ppm > 2) {
                    result = 0;
                }
	            sd8825_close_freq();
                state++;
            }
            if (HAL_GetTick() > timeout + 1500) {//1.5s超时
                UsbPrintf("device no respose!\n");
                log_printf("device no respose!\n");
                result = 0;
	            sd8825_close_freq();
                state++;
            }
        break;
//----------------------------测试电流----------------------------------------------------
        case 6:	
            rtc_group_power(testNumber, POWER_VDD_I);
            timeout = HAL_GetTick();
            state++;
        break;
        case 7:	
            if (HAL_GetTick() > (timeout + 1000)) {
                cur = CS1237Current();
                if (cur < 0) {cur = 0;}
                g_machineTest.machine.Ivdd = (int)cur;
                UsbPrintf("vdd:%dnA\n", g_machineTest.machine.Ivdd);
                log_printf("vdd:%dnA\n", g_machineTest.machine.Ivdd);
                if (g_machineTest.machine.Ivdd > 1000) {
                    result = 0;
                }
	            rtc_group_power(testNumber, POWER_I2C);
                timeout = HAL_GetTick();
                state++;
            }
        break;
        case 8:
            //-----------------------------测试结束---------------------------------------------------
            if (result == 0) {
                UsbPrintf("test all end, chip %d NG\n", testNumber);
                log_printf("test all end, chip %d NG\n", testNumber);
            } else {
                UsbPrintf("test all end, chip %d OK\n", testNumber);
                log_printf("test all end, chip %d OK\n", testNumber);
            }
            state = 0;
        break;
        default:  break;
    }
    if (HAL_GetTick() > startTime + 20000) { return -1; }//单个芯片测试时间超过20s时，强制判定ng；
    if (state == 0) {
        if (result != 0) {
            return 1;
        } else {
            return -1;
        }
    } else {
        return 0;
    }
}


int command_freq_test(uint8_t number)
{
    static uint32_t lastTestTime = 0;
    float temp, ppm;
    static uint8_t testState = 0;
    int result = 0;
    static uint8_t okCount = 0;

    if (number == 0) {
        testState = 1;
        temp = temp_read_value(1);
        UsbPrintf("system temp:%.1f\n", temp);
        okCount = 0;
        return 1;
    } 
    switch (testState) {
        case 0: break;
        case 1:
            rtc_group_power(number, POWER_I2C);
            HAL_Delay(5);
            if (rtc_read_temp(&temp) == I2C_OK) {
                UsbPrintf("chip:%d, temp:%.0f, ", number, temp);
                sd8825_out_freq();
                testState++;
            } else {
                result = -1;
            }
        break;
        case 2:
            lastTestTime = HAL_GetTick();
            g_lastFreq.recordTime = lastTestTime;
            DevicePrintf(":MEAS?\n");
            testState++;
        break;
        case 3:
            if (g_lastFreq.recordTime > lastTestTime) {
                ppm = (g_lastFreq.freq / CENTER_FREQENCY - 1) * 1000000;
                UsbPrintf("%.2fppm\n", ppm);
                sd8825_close_freq();
                testState = 1;
                okCount++;
                ControlPrintf("okNumber:%d chipName:%d message:%.2fppm", okCount, number, ppm);
                result = 1;
            } else if (lastTestTime + 1500 < HAL_GetTick()) {   
                UsbPrintf("no response\n"); 
                testState = 1;
                sd8825_close_freq();
                ControlPrintf("okNumber:%d chipName:%d message:频率计无结果", okCount, number);
                result = -1;
            }
        break;
        default:  break;
    }
    return result;
}