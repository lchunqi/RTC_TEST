#ifndef __SD8810_H
#define __SD8810_H


#include "iic.h"
#include "rtc.h"
#include "key.h"
#include "stm32f10x.h"
#include <stdint.h>

rtc_exit_code SD8810_read_addr(uint8_t reg_addr, uint16_t *read_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8810_write_addr(uint8_t reg_addr, uint16_t *write_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8810_ReadTimeDate(rtc_time *time, i2c_device_bus IIC_x);
rtc_exit_code SD8810_WriteTimeDate(rtc_time *time, i2c_device_bus IIC_x);
uint8_t RTC_ReadRAM_8810(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t RTC_WriteRAM_8810(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t sd8810_ReadTemp(i2c_device_bus IIC_x);

uint8_t sd8810_id_show(i2c_device_bus IIC_x);

uint8_t sd8810_and_std(void);
uint8_t sd8810_time_show(void);
uint8_t sd8810_time_copy(void);

rtc_exit_code sd8810_f32768(i2c_device_bus IIC_x);
rtc_exit_code sd8810_f32768_off(i2c_device_bus IIC_x);

rtc_exit_code sd8810_f1024(i2c_device_bus IIC_x);
rtc_exit_code sd8810_f1024_off(i2c_device_bus IIC_x);

rtc_exit_code sd8810_f1(i2c_device_bus IIC_x);
rtc_exit_code sd8810_f1_off(i2c_device_bus IIC_x);



#endif
