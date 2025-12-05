#ifndef __SD8931_H
#define __SD8931_H


#include "iic.h"
#include "rtc.h"
#include "stm32f10x.h"
#include <stdint.h>

rtc_exit_code SD8931_read_addr(uint8_t reg_addr, uint8_t *read_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8931_write_addr(uint8_t reg_addr, uint8_t *write_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8931_ReadTimeDate(rtc_time *time, i2c_device_bus IIC_x);
rtc_exit_code SD8931_WriteTimeDate(rtc_time *time, i2c_device_bus IIC_x);
uint8_t RTC_ReadRAM_8931(uint8_t addr,uint8_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t RTC_WriteRAM_8931(uint8_t addr,uint8_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t sd8931_ReadTemp(i2c_device_bus IIC_x);


rtc_exit_code sd8931_f8192(i2c_device_bus IIC_x);
rtc_exit_code sd8931_f8192_off(i2c_device_bus IIC_x);

rtc_exit_code sd8931_f4096(i2c_device_bus IIC_x);
rtc_exit_code sd8931_f4096_off(i2c_device_bus IIC_x);

rtc_exit_code sd8931_f1024(i2c_device_bus IIC_x);
rtc_exit_code sd8931_f1024_off(i2c_device_bus IIC_x);

rtc_exit_code sd8931_f1(i2c_device_bus IIC_x);
rtc_exit_code sd8931_f1_off(i2c_device_bus IIC_x);

rtc_exit_code sd8931_f32768(i2c_device_bus IIC_x);
rtc_exit_code sd8931_f32768_off(i2c_device_bus IIC_x);

uint8_t sd8931_id_show(i2c_device_bus IIC_x);

uint8_t sd8931_and_std(void);
uint8_t sd8931_time_show(void);
uint8_t sd8931_time_copy(void);
void sd8931_charge_on(void);
void sd8931_charge_off(void);


#endif
