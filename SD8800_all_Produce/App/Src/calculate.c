#include "main.h"
#include <math.h>
#include "calculate.h"
#include "flash.h"
#include "rtc.h"
#include <string.h>

#define TEST_NUMBER   5
#define FIND_CENTER_TEMP_ERROR			20
// #define OFFSET_NO_ROAST	-8     //��д�濾125��һ���оƬ
//#define	OTP_LOAD_TEST	1      //ֱ����д������  ��д�������ݿ���ֱ��д�룬ֱ����дOTP
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
float initTempExample[TEST_NUMBER] = {118.4, 178.4, 240.4, 310.8, 359.4};
float initCapExample[TEST_NUMBER] = {457.15, 669.64, 753.63, 647.52, 432.57};
uint8_t calcul_read_data(uint8_t rtcNum)
{
	uint8_t i;
	if (rtcNum == 0 || rtcNum > MAX_CHIP_NUMBER) { return 0; }
	flash_test_data_read(&g_testData, rtcNum, TEST_DOT_TEMP);
	flash_test_data_read(&g_testData, rtcNum, TEST_DOT_CAP);
	for (i = 0; i < TEST_NUMBER; i++) {
		g_input[i] = g_testData.testDotTemp[i];
#ifdef OFFSET_NO_ROAST
		g_result[i] = g_testData.testDotCap[i] + OFFSET_NO_ROAST;
		if (i == TEST_RTC_TEMP_Z29) {
			g_result[i] = g_result[i] - 8;
		}
#else
		g_result[i] = g_testData.testDotCap[i];
		if (i == TEST_RTC_TEMP_Z29) {
			g_result[i] = g_result[i] - 3;
		}
#endif
#ifdef OTP_LOAD_TEST
		g_input[i] = initTempExample[i];
		g_result[i] = initCapExample[i];
#endif		
		UsbPrintf("temp:%.1f cap:%.2f\n", g_input[i], g_result[i]);
	}
#ifdef OFFSET_NO_ROAST
		UsbPrintf("not roast, offset is %.1fppm\n", OFFSET_NO_ROAST * (-0.4));
#endif
	if (g_input[TEST_RTC_TEMP_Z29] < 200 || g_input[TEST_RTC_TEMP_Z29] > 270) {
		UsbPrintf("check rtc 29�� ADC %.1f error!\n", g_input[TEST_RTC_TEMP_Z29]);
		log_printf("check rtc 29�� ADC %.1f error!\n", g_input[TEST_RTC_TEMP_Z29]);
		return 0;
	}
	if ((g_input[TEST_RTC_TEMP_Z29] > g_input[TEST_RTC_TEMP_F40] + 146) || (g_input[TEST_RTC_TEMP_Z29] < g_input[TEST_RTC_TEMP_F40] + 114)) {//���65�ȣ�ADCֵ���۲�130
		UsbPrintf("check rtc -40�� ADC %.1f error!\n", g_input[TEST_RTC_TEMP_F40]);
		log_printf("check rtc -40�� ADC %.1f error!\n", g_input[TEST_RTC_TEMP_F40]);
		return 0;
	}
	// if ((g_input[TEST_RTC_TEMP_Z29] > g_input[TEST_RTC_TEMP_F10] + 86) || (g_input[TEST_RTC_TEMP_Z29] < g_input[TEST_RTC_TEMP_F10] + 54)) {//���35�ȣ�ADCֵ���۲�70
	// 	UsbPrintf("check rtc -10�� ADC %.1f error!\n", g_input[TEST_RTC_TEMP_F10]);
	// 	log_printf("check rtc -10�� ADC %.1f error!\n", g_input[TEST_RTC_TEMP_F10]);
	// 	return 0;
	// }
	// if ((g_input[TEST_RTC_TEMP_Z60] > g_input[TEST_RTC_TEMP_Z29] + 86) || (g_input[TEST_RTC_TEMP_Z60] < g_input[TEST_RTC_TEMP_Z29] + 54)) {//���35�ȣ�ADCֵ���۲�70
	// 	UsbPrintf("check rtc 60�� ADC %.1f error!\n", g_input[TEST_RTC_TEMP_Z60]);
	// 	log_printf("check rtc 60�� ADC %.1f error!\n", g_input[TEST_RTC_TEMP_Z60]);
	// 	return 0;
	// }
	if ((g_input[TEST_RTC_TEMP_Z85] > g_input[TEST_RTC_TEMP_Z29] + 136) || (g_input[TEST_RTC_TEMP_Z85] < g_input[TEST_RTC_TEMP_Z29] + 104)) {//���60�ȣ�ADCֵ���۲�120
		UsbPrintf("check rtc 85�� ADC %.1f error!\n", g_input[TEST_RTC_TEMP_Z85]);
		log_printf("check rtc 85�� ADC %.1f error!\n", g_input[TEST_RTC_TEMP_Z85]);
		return 0;
	}

	for (i = 0; i < TEST_NUMBER; i++) {
		if (i == TEST_RTC_TEMP_F10 || i == TEST_RTC_TEMP_Z60) { continue; }
		if (g_result[i] >= SD8800_CAP_LIST_MAX || g_result[i] <= 170) {
			UsbPrintf("check record cap error!\n");
			log_printf("check record cap error!\n");
			return 0;
		}
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
		log_printf("check find center ppm error\n");
		return 0;
	}
	quick = sqrt(quick);
	center = (g_input[TEST_RTC_TEMP_F10] + quick * g_input[TEST_RTC_TEMP_Z60]) / (1 + quick);
	if ((center < g_input[TEST_RTC_TEMP_Z29] - FIND_CENTER_TEMP_ERROR) || (center > g_input[TEST_RTC_TEMP_Z29] + FIND_CENTER_TEMP_ERROR)) {
		UsbPrintf("check find center[%.1f] ppm error\n", center);
		log_printf("check find center[%.1f] ppm error\n", center);
		return 0;
	}
	g_result[TEST_RTC_TEMP_Z29] = g_result[TEST_RTC_TEMP_Z29] + (center - g_input[TEST_RTC_TEMP_Z29]) * (center - g_input[TEST_RTC_TEMP_Z29]) * 0.022;
	UsbPrintf("center:%.2f\n", center);
	log_printf("center:%.2f\n", center);
	g_input[TEST_RTC_TEMP_Z29] = center;
	return 1;
}

uint8_t find_crystal_center_bad(uint8_t rtcNum)
{
	float ppm[2] = {0};
	float center = 0;
	float quick;
	flash_test_data_read(&g_testData, rtcNum, TEST_CENTER);
	ppm[0] = g_testData.testCenter[0];
	ppm[1] = g_testData.testCenter[1];
	UsbPrintf("temp:%.1f, ppm:%.1f\n", g_input[TEST_RTC_TEMP_F40], ppm[0]);
	UsbPrintf("temp:%.1f, ppm:%.1f\n", g_input[TEST_RTC_TEMP_Z85], ppm[1]);
	quick = ppm[0] / ppm[1];
	if (quick <= 0 ) {
		UsbPrintf("check find center ppm error\n");
		return 0;
	}
	quick = sqrt(quick);
	center = (g_input[TEST_RTC_TEMP_F40] + quick * g_input[TEST_RTC_TEMP_Z85]) / (1 + quick);
	UsbPrintf("center:%.2f\n", center);	
	log_printf("center:%.2f\n", center);
	if ((center < g_input[TEST_RTC_TEMP_Z29] - FIND_CENTER_TEMP_ERROR) || (center > g_input[TEST_RTC_TEMP_Z29] + FIND_CENTER_TEMP_ERROR)) {
		UsbPrintf("check find center ppm error\n");
		log_printf("check find center ppm error\n");
		return 0;
	}
	g_result[TEST_RTC_TEMP_Z29] = g_result[TEST_RTC_TEMP_Z29] + (center - g_input[TEST_RTC_TEMP_Z29]) * (center - g_input[TEST_RTC_TEMP_Z29]) * 0.022;
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
	float minB, maxB;
	float minError, maxError, error;
	Parameter cac = {0};

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
	if ((minError >= 0 && maxError < 0 ) || (minError <= 0 && maxError > 0)) {//�����м��
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
	if ((minError >= 0 && maxError < 0 ) || (minError <= 0 && maxError > 0)) {//�����м��
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


float compensate_actuality_left(float nowTemp)//����20240816���Ե����ݣ������һ�β���ֵ��ʹ��5�Ȳ�Ҫ��ƫ
{
	float result, center;
	center = g_input[TEST_RTC_TEMP_Z29] - 40;
	result = -5 + 0.003 * (nowTemp - center) * (nowTemp - center);
	if (result < 0) {
		return result;
	} else {
		return 0;
	}
}

float compensate_actuality_right(float nowTemp)//����20240816���Ե����ݣ������һ�β���ֵ��ʹ��45�Ȳ�Ҫ��ƫ
{
	float result, center;
	center = g_input[TEST_RTC_TEMP_Z29] + 40;
	result = 5 - 0.003 * (nowTemp - center) * (nowTemp - center);
	if (result > 0) {
		return result;
	} else {
		return 0;
	}
}

uint8_t init_para_3dot(void)
{
	Parameter cac = {0};

	cac.X0 = g_input[TEST_RTC_TEMP_Z29];
	cac.C = g_result[TEST_RTC_TEMP_Z29];
	cac.B = 120;// 
	cac.A = (g_result[TEST_RTC_TEMP_F40] - cac.C) / log(cosh((g_input[TEST_RTC_TEMP_F40] - cac.X0) / cac.B));
	cac.mode = 0;
	copy_parameter(&g_left, &cac);

	UsbPrintf("left:\tA:%.2f, B:%.2f, C:%.2f; X0:%.2f, mode:%d\n", g_left.A, g_left.B, g_left.C, g_left.X0, g_left.mode);

//-------------------------------------------------------------------------------------------------------------------
	cac.X0 = g_input[TEST_RTC_TEMP_Z29];
	cac.B = 120;// 
	cac.A = (g_result[TEST_RTC_TEMP_Z29] - g_result[TEST_RTC_TEMP_Z85]) / (1 - cosh((g_input[TEST_RTC_TEMP_Z85] - cac.X0) / cac.B));
	cac.C = g_result[TEST_RTC_TEMP_Z29] - cac.A;
	cac.mode = 1;
	copy_parameter(&g_right, &cac);

	UsbPrintf("right:\tA:%.2f, B:%.2f, C:%.2f; X0:%.2f, mode:%d\n", g_right.A, g_right.B, g_right.C, g_right.X0, g_right.mode);

	log_printf("parameter 3dot init success!\n");
	UsbPrintf("parameter 3dot init success!\n");
	return 1;
}


uint8_t calculate_wave1006_temp_cap(uint8_t rtcNum, float* cap_value, int* digtal, uint16_t* offset)
{
    int i;
	uint16_t adcStart = 0;
	float quick,  paraK;

    log_printf("start calculte rtc %d cap\n", rtcNum);
    UsbPrintf("start calculte rtc %d cap\n", rtcNum);
    if (calcul_read_data(rtcNum) == 0) { return 0; }

	// if (find_crystal_center(rtcNum) == 0) { return 0;}
    // if (init_para_5dot() == 0) { return 0; }
	
#ifndef OTP_LOAD_TEST
	if (find_crystal_center_bad(rtcNum) == 0) { return 0;}
#endif
    if (init_para_3dot() == 0) { return 0; }
	paraK = (g_result[TEST_RTC_TEMP_Z85] - g_result[TEST_RTC_TEMP_Z29]) / ((g_input[TEST_RTC_TEMP_Z85] - g_input[TEST_RTC_TEMP_Z29]) * (g_input[TEST_RTC_TEMP_Z85] - g_input[TEST_RTC_TEMP_Z29]));
#ifdef OTP_LOAD_TEST
	*offset = 100;
#else
	flash_test_data_read(&g_testData, rtcNum, TEMP_CONFIG);//����offset��ֵ
	*offset = g_testData.tempConfig;
#endif
	UsbPrintf("offset:%d\n", *offset);
	if (*offset < RTC_TEMP_OFFSET_MIN || *offset > RTC_TEMP_OFFSET_MAX) {
		UsbPrintf("offset error! download end!\n");
		log_printf("offset error! download end!\n");
		return 0;
	}
	adcStart = (g_testData.tempConfig + 0x30) / 2;//1006оƬotp���ݿ�ʼλ��-60�ȵ�ADC,address = 0x30
    for (i = 0; i < TEMP_ADC_LENGTH; i++) {
		// if (i % 20 == 0) { UsbPrintf("\n[%d]\t", -60 + i / 2); }
		if (adcStart + i < g_input[TEST_RTC_TEMP_F40]) {//����-40�ȵ���ֵ
			quick = chip_temp_function(adcStart + i, g_left) - 0.375 * (g_input[TEST_RTC_TEMP_F40] - (adcStart + i));
			quick = (g_result[TEST_RTC_TEMP_Z29] - quick) * 0.4;
			digtal[i] = digtal_adj_convert(g_result[TEST_RTC_TEMP_Z29], quick);
			cap_value[i] = cap_list_number_convert(g_result[TEST_RTC_TEMP_Z29], quick);
		} else if (adcStart + i < g_input[TEST_RTC_TEMP_Z29]) {
            cap_value[i] = chip_temp_function(adcStart + i, g_left) + compensate_actuality_left(adcStart + i);
        } else if (adcStart + i <= g_input[TEST_RTC_TEMP_Z85]) {
            cap_value[i] = chip_temp_function(adcStart + i, g_right) + compensate_actuality_right(adcStart + i);
        } else {//����85����Ե�
			quick = paraK * (adcStart + i - g_input[TEST_RTC_TEMP_Z29]) * (adcStart + i - g_input[TEST_RTC_TEMP_Z29]) + g_result[TEST_RTC_TEMP_Z29] 
					- 0.01 * (adcStart + i - g_input[TEST_RTC_TEMP_Z85]) * (adcStart + i - g_input[TEST_RTC_TEMP_Z85]);
			quick = (g_result[TEST_RTC_TEMP_Z29] - quick) * 0.4;
			digtal[i] = digtal_adj_convert(g_result[TEST_RTC_TEMP_Z29], quick);
			cap_value[i] = cap_list_number_convert(g_result[TEST_RTC_TEMP_Z29], quick);
		}
		if (cap_value[i] < 0) { cap_value[i] = 0;}
		// UsbPrintf("%d, %.2f\t", digtal[i], cap_value[i]);
    }
	UsbPrintf("\ncalculte rtc %d cap ok\n", rtcNum);
	log_printf("calculte rtc %d cap ok\n", rtcNum);

    return 1;
}

