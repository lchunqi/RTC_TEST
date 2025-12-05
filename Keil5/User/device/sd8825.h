#ifndef __SD8825_H
#define __SD8825_H


#include "iic.h"
#include "rtc.h"
#include "stm32f10x.h"
#include <stdint.h>

#define sd8825_fctr(x)		GPIO_WriteBit(GPIOC, GPIO_Pin_3,(BitAction)x)


rtc_exit_code SD8825_read_addr(uint8_t reg_addr, uint16_t *read_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8825_write_addr(uint8_t reg_addr, uint16_t *write_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code SD8825_ReadTimeDate(rtc_time *time, i2c_device_bus IIC_x);
rtc_exit_code SD8825_WriteTimeDate(rtc_time *time, i2c_device_bus IIC_x);
uint8_t RTC_ReadRAM_8825(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t RTC_WriteRAM_8825(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t sd8825_ReadTemp(i2c_device_bus IIC_x);

uint8_t sd8825_id_show(i2c_device_bus IIC_x);

uint8_t sd8825_and_std(void);
uint8_t sd8825_time_show(void);
uint8_t sd8825_time_copy(void);

rtc_exit_code sd8825_f32768(i2c_device_bus IIC_x);
rtc_exit_code sd8825_f32768_off(i2c_device_bus IIC_x);

rtc_exit_code sd8825_f1024(i2c_device_bus IIC_x);
rtc_exit_code sd8825_f1024_off(i2c_device_bus IIC_x);

rtc_exit_code sd8825_f1(i2c_device_bus IIC_x);
rtc_exit_code sd8825_f1_off(i2c_device_bus IIC_x);


#endif
