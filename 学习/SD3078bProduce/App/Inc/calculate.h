#ifndef __CALCULATE_H
#define __CALCULATE_H

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum {
    TEST_RTC_TEMP_F40 = 0,
    TEST_RTC_TEMP_F10,
    TEST_RTC_TEMP_Z29,
    TEST_RTC_TEMP_Z60,
    TEST_RTC_TEMP_Z85
} TestRtcTemp;

uint8_t check_rtc_temp_success(uint8_t rtcNumber, int testDot, float testTemp);
uint8_t download_temp_cap(uint8_t rtcNum, uint8_t config9);
uint8_t calculate_wave1006_temp_cap(uint8_t rtcNum, float* cap_value, int* digtal, uint16_t* offset);
uint8_t calculate_ACEQ100_temp_cap(float* cap_value, int* digtal, uint16_t offset, uint16_t rtcAdc, float maxCap);
#ifdef __cplusplus
}
#endif

#endif 