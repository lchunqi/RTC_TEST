#ifndef __RTC_I2C_H
#define __RTC_I2C_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "soft_i2c.h"


typedef enum {
    RTC_VDD_OFF = 0,
    RTC_COMMON = 1,
    RTC_DOWNLOAD = 2,
    RTC_INIT_1006,
    RTC_SD3031,
    RTC_SD8800,
} RtcI2cState;

void rtc_sda_h(void);
void rtc_sda_l(void);
void rtc_scl_h(void);
void rtc_scl_l(void);

i2c_exit_code_e rtc_write_multi_bytes(unsigned char dev_addr, unsigned char reg_addr, const unsigned char *buf, unsigned short size);
i2c_exit_code_e rtc_read_multi_bytes(unsigned char dev_addr,unsigned char reg_addr, unsigned char *buf, unsigned short size);
void rtc_set_channel(uint8_t channel, RtcI2cState state);

#ifdef __cplusplus
}
#endif

#endif 