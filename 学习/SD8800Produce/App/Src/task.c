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
void Aecq100Sample(void);


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
    // if (systemTick % 100 == 0) {
    //     LedHeart();
    // }
    if (systemTick % 100 == 0) {
        rtc_freq_test(FREQ_TEST_DEFAULT);
    }
    if (systemTick % 20 == 0) {
        system_manage_test_mode();
    }
    if (systemTick % 200 == 0) {//系统本地走时
        system_time_update();
    }
    if (systemTick % 50 == 0) {//临时命令
        Aecq100Sample();
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
        // rtcTemp = system_get_rtc_temp();
        rtcTemp =  g_capAdjust.temp;
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
            g_capAdjust.temp = rtcTemp;
            UsbPrintf("success, temp:%.2f, best cap:%.2f, min ppm %.2f\n", g_capAdjust.temp, g_capAdjust.cap,  g_capAdjust.ppm);
            log_printf("success, temp:%.2f, best cap:%.2f, min ppm %.2f\n", g_capAdjust.temp, g_capAdjust.cap,  g_capAdjust.ppm);
        } else {
            g_capAdjust.flag = CAP_ADJUST_ERROR;
            g_capAdjust.cap = 0;
            g_capAdjust.ppm = 1000;
            g_capAdjust.temp = system_get_rtc_temp();
            UsbPrintf("check rtc temp error, temp:%.2f\n", g_capAdjust.temp);
            log_printf("check rtc temp error, temp:%.2f\n", g_capAdjust.temp);
        }
    } 

    if (mode == CAP_ADJUST_ERROR) {
        g_capAdjust.flag = CAP_ADJUST_ERROR;
        g_capAdjust.cap = 0;
        g_capAdjust.ppm = 1000;
        g_capAdjust.temp = system_get_rtc_temp();
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
            // if ((temp < -5 && temp > -15) || (temp < 65 && temp > 55)) {
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
                // if (temp > 55) { address += 4;}
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
//配置温度config9才会测试温度误差
//配置电池configC才会测试电池
//除非是I2C不通，否则不管什么状态的芯片都会测试电流
//---------------------------------------------
int system_machine_test_one(uint8_t number, uint8_t mode)
{
	float ppm4096, cur;
	uint8_t buffer[16] = {0};
	int vbat = 0;
	uint8_t i2c_result;
	static uint8_t result = 1;
    static uint8_t state = 0;
    static uint32_t timeout;
    static uint8_t testNumber = 0;
    static uint32_t startTime;

    if (mode == 1) {//在设置开始时就设定测试序号，后续不能变更
        state = 1;
        testNumber = number;
        g_machineTest.machine.Ivdd = 0;
        g_machineTest.machine.Ivbat = 0;
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
            if (HAL_GetTick() < (timeout + 800)) {
                break;
            }
            i2c_result = rtc_read_otp_data(buffer, SD3900_CONFIG0_ADDR, 16);
            if (i2c_result != I2C_OK) {
                UsbPrintf("chip%d i2c error, test end!\n", testNumber);
                log_printf("chip%d i2c error!\n", testNumber);
                return -1;
            }
            g_machineTest.machine.config9 = buffer[9];
            g_machineTest.machine.configC = buffer[0xC];
            UsbPrintf("config9:0x%02X, configC:0x%02X\n", g_machineTest.machine.config9, g_machineTest.machine.configC);
            log_printf("config9:0x%02X, configC:0x%02X\n", g_machineTest.machine.config9, g_machineTest.machine.configC);
            rtc_set_testmode();
            if (g_machineTest.machine.config9 != 0xff) {//温度计配置了，测试温度
                g_machineTest.machine.sysTemp = (int)temp_read_value(1);
                UsbPrintf("system temp:%d, ", g_machineTest.machine.sysTemp);
                rtc_read_temp(&g_machineTest.machine.rtcTemp);
                UsbPrintf("temp:%d\n", g_machineTest.machine.rtcTemp);
                log_printf("temp:%d\n", g_machineTest.machine.rtcTemp);
                if (g_machineTest.machine.rtcTemp > (g_machineTest.machine.sysTemp + 5) 
                    || g_machineTest.machine.rtcTemp < (g_machineTest.machine.sysTemp - 5)) {
                    result = 0;
                }
                state++;
            } else {//如果温度计没有配置，就不必测试频率误差了
                if (g_machineTest.machine.configC != 0xff) {//没有配置configC就不测试电池
                    state = 6;
                } else {
                    state = 8;
                    result = 0;
                }
            }
        break;
//-----------------------------测试频率------------------------------------------
        case 3:	
            rtc_set_int_channel(testNumber);
	        rtc_out_4096();            
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
                ppm4096 = (g_machineTest.machine.testFreq / 4.096 - 1) * 1000000;
                UsbPrintf("%f, ppm:%.2f\n", g_machineTest.machine.testFreq, ppm4096);
                log_printf("%f, ppm:%.2f\n", g_machineTest.machine.testFreq, ppm4096);
                if (ppm4096 < -3 || ppm4096 > 3) {
                    result = 0;
                }
	            rtc_disable_int();
                state++;
            }
            if (HAL_GetTick() > timeout + 1500) {//1.5s超时
                UsbPrintf("device no respose!\n");
                log_printf("device no respose!\n");
                result = 0;
	            rtc_disable_int();
                state++;
            }
        break;
//---------------------------测试电池--------------------------------------------------
        case 6://测试电池功能，电池电压
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2 << ((testNumber - 1) / 14), GPIO_PIN_RESET);
            HAL_Delay(5);
            rtc_read_vbat(&vbat);
            g_machineTest.machine.vbat_2V5 = vbat * 10;
            UsbPrintf("vbat = %dmV\n", g_machineTest.machine.vbat_2V5);
            log_printf("vbat = %dmV\n", g_machineTest.machine.vbat_2V5);
            if (g_machineTest.machine.vbat_2V5 < 2300 || g_machineTest.machine.vbat_2V5 > 2700) {
                result = 0;
            }
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2 << ((testNumber - 1) / 14), GPIO_PIN_SET);
            state++;
        break;
        case 7://充电电压
            rtc_set_bat_charge(1);
            HAL_Delay(5);
            rtc_read_vbat(&vbat);
            g_machineTest.machine.charge = vbat * 10;
            UsbPrintf("charge = %dmV\n", g_machineTest.machine.charge);
            if (g_machineTest.machine.charge < 2800 || g_machineTest.machine.charge > 3500) {//检查充电电压，不能太小也不能太大
                result = 0;
            }
            rtc_set_bat_charge(0);//关闭充电功能
            state++;
        break;
//----------------------------测试电流----------------------------------------------------
        case 8:	
            rtc_set_testmode();
            rtc_group_power(testNumber, POWER_VDD_I);
            timeout = HAL_GetTick();
            state++;
        break;
        case 9:	
            if (HAL_GetTick() > (timeout + 1000)) {
                cur = CS1237Current();
                if (cur < 0) {cur = 0;}
                g_machineTest.machine.Ivdd = (int)cur;
                UsbPrintf("vdd:%dnA\n", g_machineTest.machine.Ivdd);
                log_printf("vdd:%dnA\n", g_machineTest.machine.Ivdd);
                if (g_machineTest.machine.Ivdd > 1500) {
                    result = 0;
                }
                rtc_group_power(testNumber, POWER_BAT_I);
                timeout = HAL_GetTick();
                state++;
            }
        break;
        case 10:
            if (HAL_GetTick() > (timeout + 1000)) {
                cur = CS1237Current();
                if (cur < 0) {cur = 0;}
                g_machineTest.machine.Ivbat =(int)cur;
                UsbPrintf("bat:%dnA\n", g_machineTest.machine.Ivbat);
                log_printf("bat:%dnA\n", g_machineTest.machine.Ivbat);
                if (g_machineTest.machine.Ivbat > 1500) {
                    result = 0;
                }
                rtc_group_power(testNumber, POWER_I2C);
                rtc_exit_testmode();
                state++;
            }
        break;
        case 11:
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

void make_chip_ID(uint8_t* buffer, uint8_t number)
{
    buffer[0] = 0x23;
    buffer[1] = 0x10;
    buffer[2] = 0x09;
    buffer[3] = 0x00;
    buffer[4] = 0x23;
    buffer[5] = 0x41;
    buffer[6] = 0x00;
    buffer[7] = ((number / 10) * 16) + (number % 10);
}



//-------------------检烧模式-----------------------------------------------
uint8_t machineState = 0;
int system_machine_write_one(uint8_t number, uint8_t mode)
{
   //----------------
    static uint8_t testNumber = 0;
    uint8_t buffer[8];
	float temp, ppm;
    static float rtc_temp;
    static uint32_t lastTestTime = 0;
	uint8_t intBuffer[5] = {0x04, 0x00, 0x10, 0x00, 0x10};
    float current;
    uint8_t result;
    uint16_t chargeVoltage;
    static uint8_t resume = 0;
    int i;

    if (mode == 1) {//在设置开始时就设定测试序号，后续不能变更
        machineState = 0;
        testNumber = number;
        g_machineTest.machine.Ivdd = 0;//暂时没有保存测试数据
        g_machineTest.machine.Ivbat = 0;
        g_machineTest.machine.config9 = 0xff;
        g_machineTest.machine.configC = 0xff;
        g_machineTest.machine.sysTemp = 0;
        g_machineTest.machine.rtcTemp = 0;
        g_machineTest.machine.testFreq = 0;
        g_machineTest.machine.vbat_2V5 = 0;
        g_machineTest.machine.charge = 0;
        UsbPrintf("machine test chip:%d start\n", number);
        log_printf("machine test chip:%d start\n", number);
        return 0;
    }
    //  { "chip:", "download:", "systemTemp:", "chipTemp:", "freq:", "ppm:", "id:", "Ivdd:", "Vbat:", "Ibat:", "result:"}; 
    switch (machineState) {
        case 0:
            rtc_group_power(testNumber, POWER_I2C);
            rtc_set_channel(testNumber, RTC_INIT_1006);
			HAL_Delay(500);
            for (i = 0; i < 5; i++) {
                result = rtc_read_multi_bytes(0x64, 0xB2, buffer, 8);
                if (result == I2C_OK) {
                    UsbPrintf("id:%02X%02X%02X%02X%02X%02X%02X%02X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
                    if (buffer[0] >= 0x24 && buffer[0] <= 0x99) {
                        machineState = 9;
                        lastTestTime = HAL_GetTick();
                        UsbPrintf("rtc ID check OK\n");
                        resume = 0;
                        break;
                    } else {
                        UsbPrintf("rtc read ID error, test end!\n");
                        machineState = 50;
                        return -1;
                    }
                }
    			HAL_Delay(10);
            }
            if (result != I2C_OK) {
                UsbPrintf("rtc I2C error, test end!\n");
                machineState = 50;
                return -1;
            }
        break;
        
        case 9:
			buffer[0] = 0x10;
			result = rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x57, buffer, 1);
			if (result !=  I2C_OK) {
				UsbPrintf("read temp i2c error\n");
                machineState = 50;
                    return -1;
			}
			HAL_Delay(10);
			rtc_read_multi_bytes(RTC_8800_ADDRESS, 0x17, buffer, 1);
			rtc_temp = ((uint32_t)buffer[0] * 2 - 187.19 ) / 3.218;
            temp  = temp_read_value(1);
			// UsbPrintf("temp:%.0f, system temp = %.1fC\n", rtc_temp, temp);
            UsbPrintf("temp:%d, system temp = %dC\n", (int)rtc_temp, (int)temp);
            if (rtc_temp > temp + 3 || rtc_temp < temp - 3) {
                UsbPrintf(" check rtc temp error!\n");
                machineState = 50;
                return -1;
            }
			rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0B, intBuffer, 5);//开启输出
            lastTestTime = HAL_GetTick();
            machineState++;
            break;
        case 10:
            if (lastTestTime + 100 < HAL_GetTick()) {
                lastTestTime = HAL_GetTick();
                g_lastFreq.recordTime = lastTestTime;
                DevicePrintf(":MEAS?\n");
                machineState++;
            }
            break;
        case 11:
            if (g_lastFreq.recordTime > lastTestTime) {
                ppm = (g_lastFreq.freq / 1.024 - 1) * 1000000;
                UsbPrintf(" %fkHz, %.2fppm\n", g_lastFreq.freq, ppm);
                if (ppm > 5 || ppm < -5) {
                    resume++;
                    if (resume >= 3) {
                        UsbPrintf("frequency over standard!\n");
                        machineState = 50;
                        buffer[0] = 0x00;
                        rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0F, buffer, 1);//关闭频率输出
                        return -1;
                    } else {
                        machineState = 10;
                        break;
                    }
                } else {
                    lastTestTime = HAL_GetTick();
                    machineState++;
                }
                buffer[0] = 0x00;
                rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0F, buffer, 1);//关闭频率输出

                buffer[0] = 0x10;
                rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x59, buffer, 1);
                HAL_Delay(10);
                rtc_read_multi_bytes(RTC_8800_ADDRESS, 0x5A, buffer, 2);
                chargeVoltage = (((uint16_t)buffer[0] & 0x80) << 1) + buffer[1]; 
                UsbPrintf("rtc charge voltage: %d0mV\n", chargeVoltage);
                if (chargeVoltage < 310 || chargeVoltage > 330) {
                    UsbPrintf(" check rtc charge error!\n");
                    machineState = 50;
                    return -1;
                }
			    rtc_group_power(testNumber, POWER_BAT_I);
            }
            if (lastTestTime + 1200 < HAL_GetTick()) {
                resume++;
                if (resume >= 3) {
                    UsbPrintf("device no response\n");
                    machineState = 50;
                    return -1;
                } else {
                    machineState = 9;
                }
            }
        break;
        case 12:
            if (lastTestTime + 500 < HAL_GetTick()) {
                current = CS1237Current();
                if (current > 300 && current < 1000) {
                    UsbPrintf("Ibat:%.2fuA, OK\n", current * 0.001);
                    machineState += 2;
                } else {
                    lastTestTime = HAL_GetTick();
                    machineState ++;
                }
            }
            break;
        case 13:
            if (lastTestTime + 300 < HAL_GetTick()) {
                current = CS1237Current();
                if (current > 300 && current < 1000) {
                    UsbPrintf("Ibat:%.2fuA, OK\n", current * 0.001);
                    machineState ++;
			        rtc_group_power(testNumber, POWER_VDD_I);
                    lastTestTime = HAL_GetTick();
                } else {
                    UsbPrintf("Ibat:%.2fuA, NG\n", current * 0.001);
                    machineState = 50;
                    return -1;
                }
            }
            break;
        case 14:
            if (lastTestTime + 300 < HAL_GetTick()) {
                current = CS1237Current();
                if (current > 300 && current < 1000) {
                    UsbPrintf("Ivdd:%.2fuA, OK\n", current * 0.001);
                    UsbPrintf("chip %d test result: PASS\n", testNumber);
                    machineState = 50;
                    return 1;
                } else {
                    lastTestTime = HAL_GetTick();
                    machineState ++;
                }
            }
            break;
        case 15:
            if (lastTestTime + 300 < HAL_GetTick()) {
                current = CS1237Current();
                if (current > 300 && current < 1000) {
                    UsbPrintf("Ivdd:%.2fuA, OK\n", current * 0.001);
                    UsbPrintf("chip %d test result: PASS\n", testNumber);
                    machineState = 50;
                    return 1;
                } else {
                    UsbPrintf("Ivdd:%.2fuA, NG\n", current * 0.001);
                    machineState = 50;
                    return -1;
                }
            }
        break;
        case  50:
            break;
        default: break;
    }
    return 0;
}



int command_freq_test(uint8_t number)
{
    static uint32_t lastTestTime = 0;
    float temp, ppm;
    uint8_t buffer;
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
            if (rtc_read_temp(&buffer) == I2C_OK) {
                temp = buffer;
                if (buffer > 200) {temp -= 256;}
                UsbPrintf("chip:%d, temp:%.0f, ", number, temp);
                rtc_set_int_channel(number);
                rtc_out_4096();
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
                ppm = (g_lastFreq.freq / 4.096 - 1) * 1000000;
                UsbPrintf("%.2fppm\n", ppm);
                rtc_disable_int();
                testState = 1;
                okCount++;
                ControlPrintf("okNumber:%d chipName:%d message:%.2fppm", okCount, number, ppm);
                result = 1;
            } else if (lastTestTime + 1500 < HAL_GetTick()) {   
                UsbPrintf("no response\n"); 
                testState = 1;
                rtc_disable_int();
                ControlPrintf("okNumber:%d chipName:%d message:频率计无结果", okCount, number);
                result = -1;
            }
        break;
        default:  break;
    }
    return result;
}

void ClearControlMessage(void)
{
    char message[11][20] = { "chip:", "download:", "systemTemp:", "chipTemp:", "freq:", "ppm:", "id:", "Ivdd:", "Vbat:", "Ibat:", "result:"}; 
    int i;
    for (i = 0; i < 11; i++) {
        if (i == 2) continue;
        ControlPrintf("%s\n", message[i]);
        HAL_Delay(5);
    }

}

uint8_t Aecq100State = 0;
void Aecq100Sample(void)//临时任务 测试机台
{
    uint8_t buffer[8];
	float temp, ppm, sysTemp;
    // static float rtc_temp;
    // static uint8_t responseCount = 0;
    static uint32_t lastTestTime = 0;
	uint8_t intBuffer[5] = {0x04, 0x00, 0x10, 0x00, 0x10};
    float current;
    uint8_t result;
    //  { "chip:", "download:", "systemTemp:", "chipTemp:", "freq:", "ppm:", "id:", "Ivdd:", "Vbat:", "Ibat:", "result:"}; 
    switch (Aecq100State) {
        case 0:
            rtc_group_power(AECQ100_CHANNEL, POWER_I2C);
            rtc_set_channel(AECQ100_CHANNEL, RTC_INIT_1006);
            if (rtc_read_multi_bytes(0x64, 0xB2, buffer, 8) == I2C_OK) {
                ClearControlMessage();
                ControlPrintf("chip:SD8800\n");
                HAL_Delay(5);
                if (buffer[0] == 0x24) {
                    Aecq100State = 9;
                    lastTestTime = HAL_GetTick();
                    UsbPrintf("rtc ID check OK\n");
                    ControlPrintf("download:此前烧写成功\n");
                    HAL_Delay(5);
                    ControlPrintf("id:%02X%02X%02X%02X%02X%02X%02X%02X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
                } else {
                    HAL_Delay(500);
                    result = rtc_read_multi_bytes(0x64, 0x00, buffer, 8);
                    if (result == I2C_OK && (buffer[0] != 0x00 || buffer[1] != 0x00 || buffer[2] != 0x00 || buffer[3] != 0x00 || buffer[4] != 0x00 || buffer[5] != 0x00)) {
                        UsbPrintf("receive I2C OK\n");
                        ControlPrintf("download:没烧写,通信OK\n");
                        HAL_Delay(5);
                        ControlPrintf("result:NULL\n");
                        Aecq100State = 50;
                    } else {
                        UsbPrintf("receive I2C error\n");
                        ControlPrintf("download:通信异常\n");
                        HAL_Delay(5);
                        ControlPrintf("result:NG\n");
                        Aecq100State = 50;
                    }
                }
            }
            if (HAL_GetTick() > lastTestTime + 1000) {
                lastTestTime = HAL_GetTick();
                ControlPrintf("systemTemp:%.1f\n", temp_read_value(1));
            }
        break;

        case 9:
            if (lastTestTime + 1000 < HAL_GetTick()) {
                UsbPrintf("等待1s\n");
                rtc_group_power(AECQ100_CHANNEL, POWER_I2C);
                rtc_set_channel(AECQ100_CHANNEL, RTC_INIT_1006);
                Aecq100State++;
                break;
            }
        break;
        case 10:
			buffer[0] = 0x10;
			result = rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x57, buffer, 1);
			if (result !=  I2C_OK) {
				UsbPrintf(" i2c error\n");
                Aecq100State = 50;
                ControlPrintf("download:通信失败\n");
			    HAL_Delay(5);
                ControlPrintf("result:NG\n");
                break;
			}
			HAL_Delay(10);
			rtc_read_multi_bytes(RTC_8800_ADDRESS, 0x17, buffer, 1);
			temp = ((uint32_t)buffer[0] * 2 - 187.19 ) / 3.218;
            sysTemp = temp_read_value(1);
			UsbPrintf("temp:%.0f, system temp = %.1fC\n", temp, sysTemp);
            ControlPrintf("chipTemp:%.0f\n", temp);
            if (temp > 3 + sysTemp || temp < -3 + sysTemp) {
                HAL_Delay(10);
                ControlPrintf("result:NG\n");
                Aecq100State = 50;
                break;
            }else {
                rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0B, intBuffer, 5);//开启输出
                lastTestTime = HAL_GetTick();
                Aecq100State++;
            }
            break;
        case 11:
            if (lastTestTime + 300 < HAL_GetTick()) {
                lastTestTime = HAL_GetTick();
                g_lastFreq.recordTime = lastTestTime;
                DevicePrintf(":MEAS?\n");
                Aecq100State++;
            }
            break;
        case 12:
            if (g_lastFreq.recordTime > lastTestTime) {
                ppm = (g_lastFreq.freq / 1.024 - 1) * 1000000;
                UsbPrintf(" %fkHz, %.2fppm\n", g_lastFreq.freq, ppm);
                ControlPrintf("freq:%fkHz\n", g_lastFreq.freq);
			    HAL_Delay(5);
                ControlPrintf("ppm:%.2fppm\n", ppm);
                if (ppm > 3 || ppm < -3) {
                    lastTestTime = HAL_GetTick();
                    g_lastFreq.recordTime = lastTestTime;
                    DevicePrintf(":MEAS?\n");
                    Aecq100State++;
                    break;
                } else {
                    lastTestTime = HAL_GetTick();
                    Aecq100State += 2;
                }
                buffer[0] = 0x00;
                rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0F, buffer, 1);
			    rtc_group_power(AECQ100_CHANNEL, POWER_BAT_I);
            }
            if (lastTestTime + 1200 < HAL_GetTick()) {
                UsbPrintf("device no response\n");
                Aecq100State = 50;
                ControlPrintf("freq:no response\n");
			    HAL_Delay(5);
                ControlPrintf("result:NG\n");
            }
        break;
        case 13:
            if (g_lastFreq.recordTime > lastTestTime) {
                ppm = (g_lastFreq.freq / 1.024 - 1) * 1000000;
                UsbPrintf(" %fkHz, %.2fppm\n", g_lastFreq.freq, ppm);
                ControlPrintf("freq:%fkHz\n", g_lastFreq.freq);
			    HAL_Delay(5);
                ControlPrintf("ppm:%.2fppm\n", ppm);
                buffer[0] = 0x00;
                rtc_write_multi_bytes(RTC_8800_ADDRESS, 0x0F, buffer, 1);
                if (ppm > 3 || ppm < -3) {
			        HAL_Delay(5);
                    ControlPrintf("result:NG\n");
                    Aecq100State = 50;
                } else {
                    lastTestTime = HAL_GetTick();
                    Aecq100State++;
			        rtc_group_power(AECQ100_CHANNEL, POWER_BAT_I);
                }
            }
            if (lastTestTime + 1200 < HAL_GetTick()) {
                UsbPrintf("device no response\n");
                Aecq100State = 50;
                ControlPrintf("freq:no response\n");
			    HAL_Delay(5);
                ControlPrintf("result:NG\n");
            }
        break;
        case 14:
            if (lastTestTime + 1500 < HAL_GetTick()) {
                current = CS1237Current();
                UsbPrintf("rtc bat current = %.1f nA\n", current);
                if (current > 300 && current < 1000) {
                    UsbPrintf("current OK\n");
                    ControlPrintf("Ibat:%.2fuA\n", current * 0.001);
                    Aecq100State += 2;
                    rtc_group_power(AECQ100_CHANNEL, POWER_VDD_I);
                    lastTestTime = HAL_GetTick();
                } else {
                    lastTestTime = HAL_GetTick();
                    Aecq100State ++;
                }
            }
            break;
        case 15:
            if (lastTestTime + 500 < HAL_GetTick()) {
                current = CS1237Current();
                UsbPrintf("rtc bat current = %.1f nA\n", current);
                ControlPrintf("Ibat:%.2fuA\n", current * 0.001);
                if (current > 300 && current < 1000) {
                    UsbPrintf("current OK\n");
                    Aecq100State++;
                    rtc_group_power(AECQ100_CHANNEL, POWER_VDD_I);
                    lastTestTime = HAL_GetTick();
                } else {
                    UsbPrintf("bat current error");
                    ControlPrintf("result:NG\n");
                    Aecq100State = 50;
                    rtc_group_power(AECQ100_CHANNEL, POWER_I2C);
                    rtc_set_channel(AECQ100_CHANNEL, RTC_INIT_1006);
                }
            }
            break;
        case 16:
            if (lastTestTime + 500 < HAL_GetTick()) {
                current = CS1237Current();
                UsbPrintf("rtc vdd current = %.1f nA\n", current);
                if (current > 300 && current < 1200) {
                    ControlPrintf("Ivdd:%.2fuA\n", current * 0.001);
			        HAL_Delay(5);
                    ControlPrintf("result:PASS\n");
                    Aecq100State = 50;
                    rtc_group_power(AECQ100_CHANNEL, POWER_I2C);
                    rtc_set_channel(AECQ100_CHANNEL, RTC_INIT_1006);
                } else {
                    Aecq100State++;
                    lastTestTime = HAL_GetTick();
                }
            }
            break;
        case 17:
            if (lastTestTime + 500 < HAL_GetTick()) {
                current = CS1237Current();
                UsbPrintf("rtc vdd current = %.1f nA\n", current);
                ControlPrintf("Ivdd:%.2fuA\n", current * 0.001);
                if (current > 300 && current < 1200) {
			        HAL_Delay(5);
                    ControlPrintf("result:PASS\n");
                } else {
			        HAL_Delay(5);
                    ControlPrintf("result:NG\n");
                }
                Aecq100State = 50;
                rtc_group_power(AECQ100_CHANNEL, POWER_I2C);
                rtc_set_channel(AECQ100_CHANNEL, RTC_INIT_1006);
            }
            break;
        case  50:
            if (rtc_read_multi_bytes(0x64, 0x00, buffer, 1) != I2C_OK) {
                Aecq100State = 0;
            }
            if (HAL_GetTick() > lastTestTime + 1000) {
                lastTestTime = HAL_GetTick();
                ControlPrintf("systemTemp:%.1f\n", temp_read_value(1));
            }
            break;
        default: break;
    }
}