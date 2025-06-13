#ifndef __RTC_H
#define __RTC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "rtc_i2c.h"
#define RTC_8800_ADDRESS    0x64
#define RTC_8931_ADDRESS    0xA0



#define SD8800_CAP_MODE     1
#define SD8800_CAP_MIN      SD8800_CAP_MODE *1.5f
#define SD8800_CAP_MAX      SD8800_CAP_MODE *1.5f + 13.005f

#define SD8800_CAP_LIST_MIN     1
#define SD8800_CAP_LIST_MAX     812
#define CENTER_FREQENCY     4.096f//kHz
#define RTC_TEMP_OFFSET_MIN     50
#define RTC_TEMP_OFFSET_MAX     140
#define TEMP_ADC_LENGTH			488 //(1024 - 48) / 2


uint8_t rtc_read_temp(uint8_t* temp);
uint8_t rtc_set_testcap(uint8_t *p_dat);
uint8_t rtc_out_4096(void);
uint8_t rtc_disable_int(void);
uint16_t rtc_init_test_cap(float temp);
void cap_float_to_uint(float f_cap, uint8_t *p_dat1, uint8_t *p_dat2);
float cap_uint_to_float(uint8_t cap1, uint8_t cap2);
float system_get_rtc_temp(void);

uint8_t wave1006_read_temp_adc(uint16_t* temp);
uint8_t rtc_1006_set_testmode(uint8_t channel);
uint8_t rtc_1006_exit_testmode(uint8_t channel);
float cap_config_to_float(uint8_t mode, uint8_t cap);
void cap_config_to_uint(uint8_t mode, uint8_t cap, uint8_t* cap_buf);
uint8_t rtc_convert_temp_offset(float rtc_adc, float system_temp);
void cap_list_to_uint(uint16_t number, uint8_t* cap_buf);
uint16_t cap_list_number_convert(uint16_t start, float ppm);
int digtal_adj_convert(uint16_t start, float ppm);
uint8_t rtc_8800_adc_testmode(uint8_t channel);
#ifdef __cplusplus
}
#endif

#endif 