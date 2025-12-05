#include "main.h"
#include <math.h>
#include "calculate.h"
#include "flash.h"
#include "rtc.h"
#include <string.h>

#define TEST_NUMBER   5
#define FIND_CENTER_TEMP_ERROR			30

typedef struct {
	float A;
	float B;
	float X0;
	float C;
	uint8_t mode;
} Parameter;
static float g_result[TEST_NUMBER] = {0};
static float g_input[TEST_NUMBER] = {0};
static Parameter g_left = {0}, g_right = {0};
	
static TestResult g_testData = {0};
float initTempExample[TEST_NUMBER] = {105.2, 167.2, 236.8, 308.0, 355.0};
float initCapExample[TEST_NUMBER] = {252.49, 478.40, 578.10, 476.76, 280.44};
uint8_t calcul_read_data(uint8_t rtcNum)
{
	uint8_t i;
	if (rtcNum == 0 || rtcNum > MAX_CHIP_NUMBER) { return 0; }
	flash_test_data_read(&g_testData, rtcNum, TEST_DOT_TEMP);
	flash_test_data_read(&g_testData, rtcNum, TEST_DOT_CAP);
	for (i = 0; i < TEST_NUMBER; i++) {
		g_input[i] = g_testData.testDotTemp[i];
		g_result[i] = g_testData.testDotCap[i];
		// g_input[i] = initTempExample[i];
		// g_result[i] = initCapExample[i];
		UsbPrintf("temp:%.1f cap:%.2f\n", g_input[i], g_result[i]);
	}
	return 1;
}

uint8_t find_crystal_center(uint8_t rtcNum)
{
	float ppm[2] = {0};
	float center = 0;
	float quick;
	flash_test_data_read(&g_testData, rtcNum, TEST_CENTER);
	ppm[0] = g_testData.testCenter[0];
	ppm[1] = g_testData.testCenter[1];
	UsbPrintf("temp:%.1f, ppm:%.1f\n", g_input[TEST_RTC_TEMP_F10], ppm[0]);
	UsbPrintf("temp:%.1f, ppm:%.1f\n", g_input[TEST_RTC_TEMP_Z60], ppm[1]);
	quick = ppm[0] / ppm[1];
	if (quick <= 0 ) {
		UsbPrintf("check find center ppm error\n");
		return 0;
	}
	quick = sqrt(quick);
	center = (g_input[TEST_RTC_TEMP_F10] + quick * g_input[TEST_RTC_TEMP_Z60]) / (1 + quick);
	if ((center < g_input[TEST_RTC_TEMP_Z29] - FIND_CENTER_TEMP_ERROR) || (center > g_input[TEST_RTC_TEMP_Z29] + FIND_CENTER_TEMP_ERROR)) {
		UsbPrintf("check find center[%.1f] ppm error\n", center);
		return 0;
	}
	g_result[TEST_RTC_TEMP_Z29] = g_result[TEST_RTC_TEMP_Z29] + (center - g_input[TEST_RTC_TEMP_Z29]) * (center - g_input[TEST_RTC_TEMP_Z29]) * 0.022;
	UsbPrintf("center:%.2f\n", center);
	g_input[TEST_RTC_TEMP_Z29] = center;
	return 1;
}

void copy_parameter(Parameter* target, Parameter* source)
{
		target->A = source->A;
		target->B = source->B;
		target->C = source->C;
		target->X0 = source->X0;
		target->mode = source->mode;
}

float chip_temp_function(float tem, Parameter cac)
{
		float target, quick;
		if (cac.B != 0)	{
				quick = (float)(tem - cac.X0) / cac.B;
		} else {
				quick = 0;
		}
		if (cac.mode == 0) {
			target = cac.A  * log(cosh(quick)) + cac.C;
		} else {
			target = cac.A  * cosh(quick) + cac.C;
		}
		return target;
}

uint8_t init_para_5dot(void)
{
	int i;
	uint16_t temp[5] = {120, 180, 250, 320, 360};
	float minB, maxB;
	float minError, maxError, error;
	Parameter cac = {0};

	for (i = 0; i < 5; i++) {
		if ((g_input[i] < temp[i] * 0.7) || (g_input[i] > temp[i] * 1.2)) {
			UsbPrintf("check rtc temp error!\n");
			log_printf("check rtc temp error!\n");
			return 0;
		}
		if (g_result[i] > SD8800_CAP_LIST_MAX || g_result[i] < 181) {
			log_printf("check rtc cap error!\n");
			UsbPrintf("check rtc cap error!\n");
			return 0;
		}
	}
	cac.X0 = g_input[TEST_RTC_TEMP_Z29];
	cac.C = g_result[TEST_RTC_TEMP_Z29];
	minB = 10;
	maxB = 990;

	cac.B = minB;// 
	cac.A = (g_result[TEST_RTC_TEMP_F40] - cac.C) / log(cosh((g_input[TEST_RTC_TEMP_F40] - cac.X0) / cac.B));
	minError = cac.A * log(cosh((g_input[TEST_RTC_TEMP_F10] - cac.X0) / cac.B)) + cac.C - g_result[TEST_RTC_TEMP_F10];
	cac.B = maxB;// 
	cac.A = (g_result[TEST_RTC_TEMP_F40] - cac.C) / log(cosh((g_input[TEST_RTC_TEMP_F40] - cac.X0) / cac.B));
	maxError = cac.A * log(cosh((g_input[TEST_RTC_TEMP_F10] - cac.X0) / cac.B)) + cac.C - g_result[TEST_RTC_TEMP_F10];
	UsbPrintf("min:%.2f, max:%.2f\n", minError, maxError);
	if ((minError >= 0 && maxError < 0 ) || (minError <= 0 && maxError > 0)) {//存在中间解
		UsbPrintf("left para use mode 0\n");
		for (i = 0; i < 20; i++) {
			cac.B = (minB + maxB) / 2;
			cac.A = (g_result[TEST_RTC_TEMP_F40] - cac.C) / log(cosh((g_input[TEST_RTC_TEMP_F40] - cac.X0) / cac.B));
			error = cac.A * log(cosh((g_input[TEST_RTC_TEMP_F10] - cac.X0) / cac.B)) + cac.C - g_result[TEST_RTC_TEMP_F10];
			if (fabs(error) <= 0.2) {
				break;
			} else {
				if ((minError >= 0 && error >= 0) || (minError <= 0 && error <= 0)) {
					minB = cac.B;
				} else {
					maxB = cac.B;
				}
			}
		}
		if (fabs(error) <= 1) {
			cac.mode = 0;
			copy_parameter(&g_left, &cac);
			UsbPrintf("min error:%.2f\n", error);
		} else {
			UsbPrintf("check left para error, step 1!\n");
			return 0;
		}
	} else {
		cac.B = minB;// 
		cac.A = (g_result[TEST_RTC_TEMP_Z29] - g_result[TEST_RTC_TEMP_F40]) / (1 - cosh((g_input[TEST_RTC_TEMP_F40] - cac.X0) / cac.B));
		cac.C = g_result[TEST_RTC_TEMP_Z29] - cac.A;
		minError = cac.A * cosh((g_input[TEST_RTC_TEMP_F10] - cac.X0) / cac.B) + cac.C - g_result[TEST_RTC_TEMP_F10];
		cac.B = maxB;// 
		cac.A = (g_result[TEST_RTC_TEMP_Z29] - g_result[TEST_RTC_TEMP_F40]) / (1 - cosh((g_input[TEST_RTC_TEMP_F40] - cac.X0) / cac.B));
		cac.C = g_result[TEST_RTC_TEMP_Z29] - cac.A;
		maxError = cac.A * cosh((g_input[TEST_RTC_TEMP_F10] - cac.X0) / cac.B) + cac.C - g_result[TEST_RTC_TEMP_F10];
		UsbPrintf("min:%.2f, max:%.2f\n", minError, maxError);
		if ((minError > 0 && maxError < 0 ) || (minError < 0 && maxError > 0)) {
			UsbPrintf("left para use mode 1\n");
			for (i = 0; i < 20; i++) {
				cac.B = (minB + maxB) / 2;
				cac.A = (g_result[TEST_RTC_TEMP_Z29] - g_result[TEST_RTC_TEMP_F40]) / (1 - cosh((g_input[TEST_RTC_TEMP_F40] - cac.X0) / cac.B));
				cac.C = g_result[TEST_RTC_TEMP_Z29] - cac.A;
				error = cac.A * cosh((g_input[TEST_RTC_TEMP_F10] - cac.X0) / cac.B) + cac.C - g_result[TEST_RTC_TEMP_F10];
				if (fabs(error) <= 0.2) {
					break;
				} else {
					if ((minError >= 0 && error >= 0) || (minError <= 0 && error <= 0)) {
						minB = cac.B;
					} else {
						maxB = cac.B;
					}
				}
			}
			if (fabs(error) <= 1) {
				cac.mode = 1;
				copy_parameter(&g_left, &cac);
				UsbPrintf("min error:%.2f\n", error);
			} else {
				UsbPrintf("check left para error, step 2!\n");
				return 0;
			}
		} else {
			UsbPrintf("check left para error, step 0!\n");
			return 0;
		}
	}
	UsbPrintf("left:\tA:%.2f, B:%.2f, C:%.2f; X0:%.2f, mode:%d\n", g_left.A, g_left.B, g_left.C, g_left.X0, g_left.mode);

//-------------------------------------------------------------------------------------------------------------------
	cac.X0 = g_input[TEST_RTC_TEMP_Z29];
	cac.C = g_result[TEST_RTC_TEMP_Z29];
	minB = 10;
	maxB = 990;

	cac.B = minB;// 
	cac.A = (g_result[TEST_RTC_TEMP_Z85] - cac.C) / log(cosh((g_input[TEST_RTC_TEMP_Z85] - cac.X0) / cac.B));
	minError = cac.A * log(cosh((g_input[TEST_RTC_TEMP_Z60] - cac.X0) / cac.B)) + cac.C - g_result[TEST_RTC_TEMP_Z60];
	cac.B = maxB;// 
	cac.A = (g_result[TEST_RTC_TEMP_Z85] - cac.C) / log(cosh((g_input[TEST_RTC_TEMP_Z85] - cac.X0) / cac.B));
	maxError = cac.A * log(cosh((g_input[TEST_RTC_TEMP_Z60] - cac.X0) / cac.B)) + cac.C - g_result[TEST_RTC_TEMP_Z60];
	UsbPrintf("min:%.2f, max:%.2f\n", minError, maxError);
	if ((minError >= 0 && maxError < 0 ) || (minError <= 0 && maxError > 0)) {//存在中间解
		UsbPrintf("right para use mode 0\n");
		for (i = 0; i < 20; i++) {
			cac.B = (minB + maxB) / 2;
			cac.A = (g_result[TEST_RTC_TEMP_Z85] - cac.C) / log(cosh((g_input[TEST_RTC_TEMP_Z85] - cac.X0) / cac.B));
			error = cac.A * log(cosh((g_input[TEST_RTC_TEMP_Z60] - cac.X0) / cac.B)) + cac.C - g_result[TEST_RTC_TEMP_Z60];
			if (fabs(error) <= 0.2) {
				break;
			} else {
				if ((minError >= 0 && error >= 0) || (minError <= 0 && error <= 0)) {
					minB = cac.B;
				} else {
					maxB = cac.B;
				}
			}
		}
		if (fabs(error) <= 1) {
			cac.mode = 0;
			copy_parameter(&g_right, &cac);
			UsbPrintf("min error:%.2f\n", error);
		} else {
			UsbPrintf("check right para error, step 1!\n");
			return 0;
		}
	} else {
		cac.B = minB;// 
		cac.A = (g_result[TEST_RTC_TEMP_Z29] - g_result[TEST_RTC_TEMP_Z85]) / (1 - cosh((g_input[TEST_RTC_TEMP_Z85] - cac.X0) / cac.B));
		cac.C = g_result[TEST_RTC_TEMP_Z29] - cac.A;
		minError = cac.A * cosh((g_input[TEST_RTC_TEMP_Z60] - cac.X0) / cac.B) + cac.C - g_result[TEST_RTC_TEMP_Z60];
		cac.B = maxB;// 
		cac.A = (g_result[TEST_RTC_TEMP_Z29] - g_result[TEST_RTC_TEMP_Z85]) / (1 - cosh((g_input[TEST_RTC_TEMP_Z85] - cac.X0) / cac.B));
		cac.C = g_result[TEST_RTC_TEMP_Z29] - cac.A;
		maxError = cac.A * cosh((g_input[TEST_RTC_TEMP_Z60] - cac.X0) / cac.B) + cac.C - g_result[TEST_RTC_TEMP_Z60];
		UsbPrintf("min:%.2f, max:%.2f\n", minError, maxError);
		if ((minError > 0 && maxError < 0 ) || (minError < 0 && maxError > 0)) {
			UsbPrintf("right para use mode 1\n");
			for (i = 0; i < 20; i++) {
				cac.B = (minB + maxB) / 2;
				cac.A = (g_result[TEST_RTC_TEMP_Z29] - g_result[TEST_RTC_TEMP_Z85]) / (1 - cosh((g_input[TEST_RTC_TEMP_Z85] - cac.X0) / cac.B));
				cac.C = g_result[TEST_RTC_TEMP_Z29] - cac.A;
				error = cac.A * cosh((g_input[TEST_RTC_TEMP_Z60] - cac.X0) / cac.B) + cac.C - g_result[TEST_RTC_TEMP_Z60];
				if (fabs(error) <= 0.2) {
					break;
				} else {
					if ((minError >= 0 && error >= 0) || (minError <= 0 && error <= 0)) {
						minB = cac.B;
					} else {
						maxB = cac.B;
					}
				}
			}
			if (fabs(error) <= 1) {
				cac.mode = 1;
				copy_parameter(&g_right, &cac);
				UsbPrintf("min error:%.2f\n", error);
			} else {
				UsbPrintf("check right para error, step 2!\n");
				return 0;
			}
		} else {
			UsbPrintf("check right para error, step 0!\n");
			return 0;
		}
	}
	UsbPrintf("right:\tA:%.2f, B:%.2f, C:%.2f; X0:%.2f, mode:%d\n", g_right.A, g_right.B, g_right.C, g_right.X0, g_right.mode);

	UsbPrintf("parameter init success!\n");
	return 1;
}

#define TEMP_ERROR_GAIN		0.021875 //(temp - center) * (temp - center) / 4 * 0.035 * 2.5
uint8_t init_adjust_para_5dot(void)
{
	int i;
	float error1, error2;
	float minErrorRight, minErrorLeft;
	float moveTemp = 0;
	float bigger;
	float bestTempRight = 0, bestTempLeft = 0;

	g_left.B = 1000;
	g_right.B = 1000;
	minErrorRight = 1000;
	minErrorLeft = 1000;
	for (i = 0; i < 40; i++) {//1bit = 0.5℃，步进0.25℃,搜索范围±10℃
		moveTemp = i * 0.5;
	//-------------------------------------------------计算中心点右移，寻找最好的中心点-----------------------------------------------------------
		g_left.X0 = g_input[TEST_RTC_TEMP_Z29] + moveTemp;
		g_left.C = g_result[TEST_RTC_TEMP_Z29] + moveTemp * moveTemp * TEMP_ERROR_GAIN;
		g_left.A = (g_result[TEST_RTC_TEMP_F40] - g_left.C) / log(cosh((g_input[TEST_RTC_TEMP_F40] - g_left.X0) / g_left.B));
		error1 = chip_temp_function(g_input[TEST_RTC_TEMP_F10], g_left) - g_result[TEST_RTC_TEMP_F10];

		g_right.X0 = g_input[TEST_RTC_TEMP_Z29] + moveTemp;
		g_right.C = g_result[TEST_RTC_TEMP_Z29] + moveTemp * moveTemp * TEMP_ERROR_GAIN;
		g_right.A = (g_result[TEST_RTC_TEMP_Z85] - g_right.C) / log(cosh((g_input[TEST_RTC_TEMP_Z85] - g_right.X0) / g_right.B));
		error2 = chip_temp_function(g_input[TEST_RTC_TEMP_Z60], g_right)- g_result[TEST_RTC_TEMP_Z60];

		if (fabs(error1) < fabs(error2)) {
			bigger = error2;
		} else {
			bigger = error1;
		}
		if (fabs(bigger) < fabs(minErrorRight)) {
			minErrorRight = bigger;
			bestTempRight = moveTemp;
		} 
	//-------------------------------------------------计算中心点左移，寻找最好的中心点-----------------------------------------------------------
		
		g_left.X0 = g_input[TEST_RTC_TEMP_Z29] - moveTemp;
		g_left.C = g_result[TEST_RTC_TEMP_Z29] + moveTemp * moveTemp * TEMP_ERROR_GAIN;
		g_left.A = (g_result[TEST_RTC_TEMP_F40] - g_left.C) / log(cosh((g_input[TEST_RTC_TEMP_F40] - g_left.X0) / g_left.B));
		error1 = chip_temp_function(g_input[TEST_RTC_TEMP_F10], g_left) - g_result[TEST_RTC_TEMP_F10];

		g_right.X0 = g_input[TEST_RTC_TEMP_Z29] - moveTemp;
		g_right.C = g_result[TEST_RTC_TEMP_Z29] + moveTemp * moveTemp * TEMP_ERROR_GAIN;
		g_right.A = (g_result[TEST_RTC_TEMP_Z85] - g_right.C) / log(cosh((g_input[TEST_RTC_TEMP_Z85] - g_right.X0) / g_right.B));
		error2 = chip_temp_function(g_input[TEST_RTC_TEMP_Z60], g_right) - g_result[TEST_RTC_TEMP_Z60];

		if (fabs(error1) < fabs(error2)) {
			bigger = error2;
		} else {
			bigger = error1;
		}
		if (fabs(bigger) < fabs(minErrorLeft)) {
			minErrorLeft = bigger;
			bestTempLeft = -1 * moveTemp;
		} 
	//误差小于1时，可以直接结束查找过程	
		if (fabs(minErrorLeft) <= 1 || fabs(minErrorRight) <= 1) {
			break;
		}
	}
	
	//结束查找后，再计算1次
	if (fabs(minErrorLeft) < fabs(minErrorRight)) {
		g_right.X0 = g_input[TEST_RTC_TEMP_Z29] + bestTempLeft;
		g_right.C = g_result[TEST_RTC_TEMP_Z29] + bestTempLeft * bestTempLeft * TEMP_ERROR_GAIN;
	} else {
		g_right.X0 = g_input[TEST_RTC_TEMP_Z29] + bestTempRight;
		g_right.C = g_result[TEST_RTC_TEMP_Z29] + bestTempRight * bestTempRight * TEMP_ERROR_GAIN;
	}
	g_left.X0 = g_right.X0;
	g_left.C = g_right.C;
	g_left.A = (g_result[TEST_RTC_TEMP_F40] - g_left.C) / log(cosh((g_input[TEST_RTC_TEMP_F40] - g_left.X0) / g_left.B));
	error1 = chip_temp_function(g_input[TEST_RTC_TEMP_F10], g_left) - g_result[TEST_RTC_TEMP_F10];
	g_right.A = (g_result[TEST_RTC_TEMP_Z85] - g_right.C) / log(cosh((g_input[TEST_RTC_TEMP_Z85] - g_right.X0) / g_right.B));
	error2 = chip_temp_function(g_input[TEST_RTC_TEMP_Z60], g_right) - g_result[TEST_RTC_TEMP_Z60];

	log_printf("min left error:%.2f, right:%.2f, C:%.2f, X0:%.2f, move:%.2f\n", error1, error2, g_right.C, g_right.X0, g_right.X0 - g_input[TEST_RTC_TEMP_Z29]);
	UsbPrintf("min left error:%.2f, right:%.2f, C:%.2f, X0:%.2f, move:%.2f\n", error1, error2, g_right.C, g_right.X0, g_right.X0 - g_input[TEST_RTC_TEMP_Z29]);
	if (fabs(error1) < 6 && fabs(error2) < 6) {
		UsbPrintf("calculate success!\n");
		return 1;
	}  

	UsbPrintf("calculate fail!\n");
	return 0;
}


uint8_t calculate_wave1006_temp_cap(uint8_t rtcNum, float* cap_value, int* digtal, uint16_t* offset)
{
    int i;
	uint16_t adcStart = 0;
	float quick, paraK;

    log_printf("start calculte rtc %d cap\n", rtcNum);
    UsbPrintf("start calculte rtc %d cap\n", rtcNum);
	// flash_test_data_read(&g_testData, rtcNum, CALCUL_FLAG);
	// if (g_testData.calculFlag == '1') { 
	// 	log_printf("history success\n");
	// 	UsbPrintf("history success\n");
	// 	return 2; 
	// }
    if (calcul_read_data(rtcNum) == 0) { return 0; }

	if (find_crystal_center(rtcNum) == 0) { return 0;}
    // if (init_adjust_para_5dot() == 0) { return 0; }
    if (init_para_5dot() == 0) { return 0; }
	paraK = (g_result[TEST_RTC_TEMP_Z85] - g_result[TEST_RTC_TEMP_Z29]) / 
			((g_input[TEST_RTC_TEMP_Z85] -g_input[TEST_RTC_TEMP_Z29]) * (g_input[TEST_RTC_TEMP_Z85] -g_input[TEST_RTC_TEMP_Z29]));
	flash_test_data_read(&g_testData, rtcNum, TEMP_CONFIG);//读出offset数值
	*offset = g_testData.tempConfig;
	UsbPrintf("offset:%d\n", *offset);
	if (*offset < 60 || *offset > 140) {
		UsbPrintf("offset error! download end!\n");
		return 0;
	}
	adcStart = (g_testData.tempConfig + 0x30) / 2;//1006芯片otp数据开始位是-60度的ADC,address = 0x30
    for (i = 0; i < 420; i++) {
		if (i % 20 == 0) { UsbPrintf("\n[%d]\t", -60 + i / 2); }
        if (adcStart + i < g_input[TEST_RTC_TEMP_Z29]) {
            cap_value[i] = chip_temp_function(adcStart + i, g_left);
        } else if (adcStart + i <= g_input[TEST_RTC_TEMP_Z85]) {
            cap_value[i] = chip_temp_function(adcStart + i, g_right);
        } else {//超出85℃测试点
			quick = paraK * (adcStart + i - g_input[TEST_RTC_TEMP_Z29]) * (adcStart + i - g_input[TEST_RTC_TEMP_Z29]) + g_result[TEST_RTC_TEMP_Z29];
			quick = (g_result[TEST_RTC_TEMP_Z29] - quick) * 0.4;
			digtal[i] = digtal_adj_convert(g_result[TEST_RTC_TEMP_Z29], quick);
			cap_value[i] = cap_list_number_convert(g_result[TEST_RTC_TEMP_Z29], quick);
		}
		if (cap_value[i] < 1) { cap_value[i] = 1;}
		UsbPrintf("%d, %.2f\t", digtal[i], cap_value[i]);
    }
	UsbPrintf("\ncalculte rtc %d cap ok\n", rtcNum);
	// log_printf("calculte rtc %d cap ok\n", rtcNum);
	// g_testData.calculFlag = '1';
	// flash_test_data_write_add(&g_testData, rtcNum, CALCUL_FLAG);
	// flash_test_data_write_add(&g_testData, rtcNum, CAP_CALCULATE);

    return 1;
}

float compensate_sd8825_left(float nowTemp)//基于20240510测试的数据，计算的一段补偿值，使得5度不要负偏
{
	float result, center;
	center = (g_input[TEST_RTC_TEMP_Z29] + g_input[TEST_RTC_TEMP_F10]) * 0.5;
	result = -3 + 0.003 * (nowTemp - center) * (nowTemp - center);
	if (result < 0) {
		return result;
	} else {
		return 0;
	}
}
float compensate_sd8825_right(float nowTemp)//基于20240510测试的数据，计算的一段补偿值，使得45度不要正偏
{
	float result, center;
	center = (g_input[TEST_RTC_TEMP_Z29] + g_input[TEST_RTC_TEMP_Z60]) * 0.5;
	result = 4 - 0.0042 * (nowTemp - center) * (nowTemp - center);
	if (result > 0) {
		return result;
	} else {
		return 0;
	}
}

uint8_t calculate_ACEQ100_temp_cap(float* cap_value, int* digtal, uint16_t offset, uint16_t rtcAdc, float maxCap)
{
    int i;
	uint16_t adcStart = 0;
	float quick, paraK;

	for (i = 0; i < TEST_NUMBER; i++) {
		g_input[i] =  initTempExample[i] + rtcAdc - initTempExample[2];
		g_result[i] = initCapExample[i] + maxCap - initCapExample[2];
		UsbPrintf("[%.1f, %.1f] ", g_input[i], g_result[i]);
	}
	UsbPrintf("\n");
    if (init_para_5dot() == 0) { return 0; }
	paraK = (g_result[TEST_RTC_TEMP_Z85] - g_result[TEST_RTC_TEMP_Z29]) / 
			((g_input[TEST_RTC_TEMP_Z85] -g_input[TEST_RTC_TEMP_Z29]) * (g_input[TEST_RTC_TEMP_Z85] -g_input[TEST_RTC_TEMP_Z29]));

	adcStart = (offset + 0x30) / 2;//1006芯片otp数据开始位是-60度的ADC,address = 0x30
    for (i = 0; i < 420; i++) {
		// if (i % 20 == 0) { UsbPrintf("\n[%d]\t", -60 + i / 2); }
        if (adcStart + i < g_input[TEST_RTC_TEMP_Z29]) {
            cap_value[i] = chip_temp_function(adcStart + i, g_left) + compensate_sd8825_left(adcStart + i);
        } else if (adcStart + i <= g_input[TEST_RTC_TEMP_Z85]) {
            cap_value[i] = chip_temp_function(adcStart + i, g_right) + compensate_sd8825_right(adcStart + i);
        } else {//超出85℃测试点
			quick = paraK * (adcStart + i - g_input[TEST_RTC_TEMP_Z29]) * (adcStart + i - g_input[TEST_RTC_TEMP_Z29]) + g_result[TEST_RTC_TEMP_Z29];
			quick = (g_result[TEST_RTC_TEMP_Z29] - quick) * 0.4;
			digtal[i] = digtal_adj_convert(g_result[TEST_RTC_TEMP_Z29], quick);
			cap_value[i] = cap_list_number_convert(g_result[TEST_RTC_TEMP_Z29], quick);
		}
		if (cap_value[i] < 1) { cap_value[i] = 1;}
		// UsbPrintf("%d, %.2f\t", digtal[i], cap_value[i]);
    }
	// UsbPrintf("\ncalculte rtc %d cap ok\n", rtcNum);

    return 1;
}
