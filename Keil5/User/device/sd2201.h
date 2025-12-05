#ifndef __SD2201_H
#define __SD2201_H

#include "iic.h"
#include "rtc.h"
#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32f10x_gpio.h" // 包含 GPIO 库  

uint8_t sd2201_read_multi_bytes(uint8_t addr,uint16_t *buf,uint8_t len,i2c_device_bus bus);
uint8_t sd2201_write_multi_bytes(uint8_t addr,uint16_t *buf,uint8_t len,i2c_device_bus bus);
void sd2201_swap(uint16_t *buf);
uint8_t SD2201_WriteTimeDate(i2c_device_bus bus);
uint8_t sd2201_time_show(i2c_device_bus bus);//单独
uint8_t SD2201_ReadTimeDate_Show(i2c_device_bus bus);//对比
uint8_t sd2201_time_copy(void);
uint8_t sd2201_and_std(void);

uint8_t sd2201_f32768(i2c_device_bus IIC_x);
void sd2201_f32768_off(i2c_device_bus IIC_x);
uint8_t sd2201_f16(i2c_device_bus IIC_x);
uint8_t sd2201_f8(i2c_device_bus IIC_x);
uint8_t sd2201_f4(i2c_device_bus IIC_x);
uint8_t sd2201_f2(i2c_device_bus IIC_x);
uint8_t sd2201_f1(i2c_device_bus IIC_x);

uint8_t RTC_ReadRAM_2201(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t RTC_WriteRAM_2201(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x);


#endif
