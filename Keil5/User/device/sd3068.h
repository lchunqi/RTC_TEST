#ifndef __SD3068_H
#define __SD3068_H


#include "iic.h"
#include "rtc.h"
#include "stm32f10x.h"
#include <stdint.h>
#include <stdio.h>

rtc_exit_code rtc_f4096(i2c_device_bus IIC_x);
rtc_exit_code rtc_f4096_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f1(i2c_device_bus IIC_x);
rtc_exit_code rtc_f1_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f1s(i2c_device_bus IIC_x);
rtc_exit_code rtc_f1s_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f1024(i2c_device_bus IIC_x);
rtc_exit_code rtc_f1024_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f64(i2c_device_bus IIC_x);
rtc_exit_code rtc_f64_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f32(i2c_device_bus IIC_x);
rtc_exit_code rtc_f32_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f16(i2c_device_bus IIC_x);
rtc_exit_code rtc_f16_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f8(i2c_device_bus IIC_x);
rtc_exit_code rtc_f8_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f4(i2c_device_bus IIC_x);
rtc_exit_code rtc_f4_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f2(i2c_device_bus IIC_x);
rtc_exit_code rtc_f2_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f1_2(i2c_device_bus IIC_x);
rtc_exit_code rtc_f1_2_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f1_4(i2c_device_bus IIC_x);
rtc_exit_code rtc_f1_4_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f1_8(i2c_device_bus IIC_x);
rtc_exit_code rtc_f1_8_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f1_16(i2c_device_bus IIC_x);
rtc_exit_code rtc_f1_16_off(i2c_device_bus IIC_x);

rtc_exit_code rtc_f32768(i2c_device_bus IIC_x);
rtc_exit_code rtc_f32768_off(i2c_device_bus IIC_x);



#endif
