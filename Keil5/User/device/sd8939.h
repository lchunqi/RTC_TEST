#ifndef __SD8939_H
#define __SD8939_H


#include "iic.h"
#include "rtc.h"
#include "stm32f10x.h"
#include <stdint.h>


rtc_exit_code SD8939_read_addr(uint8_t reg_addr, uint16_t *read_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8939_write_addr(uint8_t reg_addr, uint16_t *write_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8939_ReadTimeDate(rtc_time *time, i2c_device_bus IIC_x);
rtc_exit_code SD8939_WriteTimeDate(rtc_time *time, i2c_device_bus IIC_x);
uint8_t RTC_ReadRAM_8939(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t RTC_WriteRAM_8939(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t sd8939_ReadTemp(i2c_device_bus IIC_x);


uint8_t sd8939_id_show(i2c_device_bus IIC_x);

uint8_t sd8939_and_std(void);
uint8_t sd8939_time_show(void);
uint8_t sd8939_time_copy(void);


rtc_exit_code sd8939_f32768(i2c_device_bus IIC_x);
rtc_exit_code sd8939_f32768_off(i2c_device_bus IIC_x);

rtc_exit_code sd8939_f8192(i2c_device_bus IIC_x);
rtc_exit_code sd8939_f8192_off(i2c_device_bus IIC_x);

rtc_exit_code sd8939_f4096(i2c_device_bus IIC_x);
rtc_exit_code sd8939_f4096_off(i2c_device_bus IIC_x);

rtc_exit_code sd8939_f1(i2c_device_bus IIC_x);
rtc_exit_code sd8939_f1_off(i2c_device_bus IIC_x);





#endif
