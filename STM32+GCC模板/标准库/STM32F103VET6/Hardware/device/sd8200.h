#ifndef __SD8200_H
#define __SD8200_H


#include "iic.h"
#include "rtc.h"
#include "stm32f10x.h"
#include <stdint.h>



uint8_t SD8200_ReadByte(uint8_t addr,i2c_device_bus bus);
rtc_exit_code SD8200_read_addr(uint8_t reg_addr, uint8_t *read_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8200_write_addr(uint8_t reg_addr, uint8_t *write_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8200_ReadTimeDate(rtc_time *time, i2c_device_bus IIC_x);
rtc_exit_code SD8200_WriteTimeDate(rtc_time *time, i2c_device_bus IIC_x);
uint8_t RTC_ReadRAM_8200(uint8_t addr,uint8_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t RTC_WriteRAM_8200(uint8_t addr,uint8_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t sd8200_ReadVbat(i2c_device_bus IIC_x);
uint8_t sd8200_VbatQuan(uint8_t *data,i2c_device_bus IIC_x);

uint8_t sd8200_id_show(i2c_device_bus IIC_x);

uint8_t sd8200_and_std(void);
uint8_t sd8200_time_show(void);
uint8_t sd8200_time_copy(void);
void sd8200_charge_on(void);
void sd8200_charge_off(void);


rtc_exit_code sd8200_f512(i2c_device_bus IIC_x);
rtc_exit_code sd8200_f512_off(i2c_device_bus IIC_x);

rtc_exit_code sd8200_f1(i2c_device_bus IIC_x);
rtc_exit_code sd8200_f1_off(i2c_device_bus IIC_x);


#endif
