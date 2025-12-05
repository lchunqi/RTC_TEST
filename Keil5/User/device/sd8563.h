#ifndef __SD8563_H
#define __SD8563_H


#include "iic.h"
#include "rtc.h"
#include "stm32f10x.h"
#include <stdint.h>


rtc_exit_code SD8563_read_addr(uint8_t reg_addr, uint16_t *read_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8563_write_addr(uint8_t reg_addr, uint16_t *write_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8563_ReadTimeDate(rtc_time *time, i2c_device_bus IIC_x);
rtc_exit_code SD8563_WriteTimeDate(rtc_time *time, i2c_device_bus IIC_x);
uint8_t RTC_ReadRAM_8563(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t RTC_WriteRAM_8563(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x);

uint8_t sd85xx_and_std(void);
uint8_t sd85xx_time_show(void);
uint8_t sd85xx_time_copy(void);

rtc_exit_code sd8563_f1(i2c_device_bus IIC_x);
rtc_exit_code sd8563_f1_off(i2c_device_bus IIC_x);

rtc_exit_code sd8563_f32(i2c_device_bus IIC_x);
rtc_exit_code sd8563_f32_off(i2c_device_bus IIC_x);

rtc_exit_code sd8563_f1024(i2c_device_bus IIC_x);
rtc_exit_code sd8563_f1024_off(i2c_device_bus IIC_x);

rtc_exit_code sd8563_f32768(i2c_device_bus IIC_x);
rtc_exit_code sd8563_f32768_off(i2c_device_bus IIC_x);

/**********************sd8573*******************************/
rtc_exit_code SD8573_read_addr(uint8_t reg_addr, uint16_t *read_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8573_write_addr(uint8_t reg_addr, uint16_t *write_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8573_ReadTimeDate(rtc_time *time, i2c_device_bus IIC_x);
rtc_exit_code SD8573_WriteTimeDate(rtc_time *time, i2c_device_bus IIC_x);
uint8_t RTC_ReadRAM_8573(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t RTC_WriteRAM_8573(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x);

uint8_t sd8573_and_std(void);
uint8_t sd8573_time_show(void);
uint8_t sd8573_time_copy(void);

rtc_exit_code sd8573_f32768(i2c_device_bus IIC_x);
rtc_exit_code sd8573_f32768_off(i2c_device_bus IIC_x);

rtc_exit_code sd8573_f13684(i2c_device_bus IIC_x);
rtc_exit_code sd8573_f13684_off(i2c_device_bus IIC_x);

rtc_exit_code sd8573_f8192(i2c_device_bus IIC_x);
rtc_exit_code sd8573_f8192_off(i2c_device_bus IIC_x);

rtc_exit_code sd8573_f4096(i2c_device_bus IIC_x);
rtc_exit_code sd8573_f4096_off(i2c_device_bus IIC_x);

rtc_exit_code sd8573_f2048(i2c_device_bus IIC_x);
rtc_exit_code sd8573_f2048_off(i2c_device_bus IIC_x);

rtc_exit_code sd8573_f1024(i2c_device_bus IIC_x);
rtc_exit_code sd8573_f1024_off(i2c_device_bus IIC_x);

rtc_exit_code sd8573_f1(i2c_device_bus IIC_x);
rtc_exit_code sd8573_f1_off(i2c_device_bus IIC_x);


#endif
