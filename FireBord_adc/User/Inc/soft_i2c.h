/*
 * @Author: Liang 
 * @Date: 2025-03-06 18:11:59
 * @LastEditors: HA HA 
 * @LastEditTime: 2025-03-06 19:03:28
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved.
 */
#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

#ifdef __cplusplus
 extern "C" {
#endif
#include "main.h"


#define I2C_SCL     GPIO_PIN_6
#define I2C_SDA     GPIO_PIN_7
#define I2C_GPIO    GPIOB


typedef enum {
    I2C_OK      =    0,
    I2C_START_FAIL  ,
    I2C_NO_ACK      ,
}i2c_exit_code_e;


void IIC_GPIO_Init(void);
i2c_exit_code_e i2c_write_multi_bytes(unsigned char dev_addr, unsigned char reg_addr, const unsigned char *buf, unsigned short size);
i2c_exit_code_e i2c_read_multi_bytes(unsigned char dev_addr,unsigned char reg_addr, unsigned char *buf, unsigned short size);
i2c_exit_code_e i2c_dev_detect(unsigned char dev_addr);

#ifdef __cplusplus
}
#endif

#endif 