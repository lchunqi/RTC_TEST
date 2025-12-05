#ifndef __OTP_H
#define __OTP_H

#include "usart.h"
#include "rtc.h"
#include <stdio.h>
#include <stdint.h>

uint8_t Check_RTCtype(i2c_device_bus IIC_x);
uint8_t OTP_ReadBytes(uint32_t addr,uint8_t *dat,uint32_t len,i2c_device_bus IIC_x);


#endif
