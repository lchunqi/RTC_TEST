#ifndef __RTC_I2C_H
#define __RTC_I2C_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "soft_i2c.h"


typedef enum {
    RTC_VDD_OFF = 0,
    RTC_INIT_1006,
    RTC_SD8825,
    RTC_SD8840,
} RtcI2cState;

void rtc_sda_h(void);
void rtc_sda_l(void);
void rtc_scl_h(void);
void rtc_scl_l(void);

i2c_exit_code_e rtc_write_multi_bytes(unsigned char dev_addr, unsigned char reg_addr, const unsigned char *buf, unsigned short size);
i2c_exit_code_e rtc_read_multi_bytes(unsigned char dev_addr,unsigned char reg_addr, unsigned char *buf, unsigned short size);
void rtc_set_channel(uint8_t channel, RtcI2cState state);
void spi_read_multi_bytes(unsigned char reg_addr, unsigned char *buf, unsigned short size);
void spi_write_multi_bytes(unsigned char reg_addr, unsigned char *buf, unsigned short size);
uint8_t check_sda_level(void);

#ifdef __cplusplus
}
#endif

#endif 