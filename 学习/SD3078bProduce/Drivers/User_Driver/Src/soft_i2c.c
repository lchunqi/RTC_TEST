
/*
 *  drivers  i2c
 * Copyright (C) 2022 shenzhen wave
 * version:v1.0.0--2023-2-12
 * This program is free software; you can redistribute it and/or modify
 */
#include "main.h"
#include "soft_i2c.h"


uint16_t 	soft_i2c_delay = 200;
/**
 * @brief set sda is high
 * 
 */
static inline void i2c_sda_h(void)
{
	/*user code*/
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
}

/**
 * @brief set sda is low
 * 
 */
static inline void i2c_sda_l(void)
{
	/*user code*/
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
}

/**
 * @brief set scl is high
 * 
 */
static inline void i2c_scl_h(void)
{
	/*user code*/
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
}

/**
 * @brief set scl is low
 * 
 */
static inline void i2c_scl_l(void)
{
	/*user code*/
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
}

/**
 * @brief Read sda pin state
 * 
 */
static unsigned char i2c_read_sda(void)
{
	/*user code*/
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_SET) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief When read data, need to set sda as in
 * 
 */
static inline void i2c_sda_in(void)
{
	/*user code */
  	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
 * @brief When send data, need to set sda as out
 * 
 */
static inline void i2c_sda_out(void)
{
	/*user code */
  	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
}

/**
 * @brief i2c wiat
 * 
 */
static inline void i2c_Wait(void)
{	
	int i = soft_i2c_delay;
	while (i--);
}

/**
 * @brief Generate an i2c strat signal
 * 
 */
i2c_exit_code_e i2c_start(void)
{
	i2c_sda_out();
	i2c_sda_h();
	i2c_scl_h();
	i2c_Wait();

	if(!i2c_read_sda())
	{
		return I2C_START_FAIL;
	}

	i2c_sda_l();
	i2c_Wait();
	i2c_scl_l();
	i2c_Wait();
	return I2C_OK;
}

/**
 * @brief Generate an i2c stop signal
 * 
 */
void i2c_stop(void)
{
	i2c_sda_out();
	i2c_sda_l();
	i2c_scl_l();
	i2c_Wait();
	i2c_scl_h();
	i2c_Wait();
	i2c_sda_h();
}

/**
 * @brief When data is read, the host responds a ack signal
 * 
 */
void i2c_ack(void)
{	
	i2c_sda_l();
	i2c_scl_l();
	i2c_Wait();
	i2c_scl_h();
	i2c_Wait();
	i2c_scl_l();
}

/**
 * @brief when data is read, the host responds a nack signal
 * 
 */
void i2c_nack(void)
{	
	i2c_sda_h();
	i2c_scl_l();
	i2c_Wait();
	i2c_scl_h();
	i2c_Wait();
	i2c_scl_l();
}

/**
 * @brief When data is written, the slave responds  a ack signal
 * 
 */
i2c_exit_code_e i2c_wait_ack(void)
{
	i2c_sda_in();
	i2c_scl_h();
	i2c_Wait();

	if (i2c_read_sda())
	{
		i2c_scl_l();
		return I2C_NO_ACK;
	}
	i2c_scl_l();
	i2c_Wait();
	i2c_sda_h();
	i2c_sda_out();
	return I2C_OK;
}

/**
 * @brief Send a byte to slave
 * 
 */
void i2c_send_byte(unsigned char value)
{
	unsigned char i;

	i2c_scl_l();
	for (i = 0; i < 8; i ++)
	{
		if(value & 0x80)
			i2c_sda_h();
		else 
			i2c_sda_l();
		i2c_Wait();
		value <<= 1;
		i2c_scl_h();
		i2c_Wait();
		i2c_scl_l();
	}
}

/**
 * @brief Receive a byte timing
 * 
 */
unsigned char i2c_receive_byte(void)
{
	unsigned char i;
	unsigned char data = 0;

    i2c_sda_in();
	i2c_sda_h();
	for (i = 0; i < 8; i++)
	{
		i2c_scl_l();
		data <<= 1;
		i2c_Wait();
		i2c_scl_h();
		i2c_Wait();
		if(i2c_read_sda())
		{
			data |= 0x01;
		}
	}
	i2c_scl_l();
	i2c_Wait();
    i2c_sda_out();
	return data;
}

/**
 * @brief Write multiple bytes to slave 
 * 
 */
i2c_exit_code_e i2c_write_multi_bytes(unsigned char dev_addr, unsigned char reg_addr, const unsigned char *buf, unsigned short size)
{
    unsigned short i;

    if (i2c_start() != I2C_OK)
		return I2C_START_FAIL;

    i2c_send_byte(dev_addr);
	if (i2c_wait_ack() != I2C_OK)
	{
		i2c_stop(); 
		return I2C_NO_ACK;
	}
	i2c_send_byte(reg_addr);
	if (i2c_wait_ack() != I2C_OK)
	{
		i2c_stop(); 
		return I2C_NO_ACK;
	}
    for (i = 0; i < size; i++)
    {
    	i2c_send_byte(*(buf + i));
    	i2c_wait_ack();
    }
    i2c_stop();

	return	I2C_OK;
}

/**
 * @brief Read multiple bytes from slave 
 * 
 */
i2c_exit_code_e i2c_read_multi_bytes(unsigned char dev_addr,unsigned char reg_addr, unsigned char *buf, unsigned short size)
{
	unsigned short i;

	if(i2c_start() != I2C_OK)
		return I2C_START_FAIL;
	/*send device address*/
	i2c_send_byte(dev_addr);
	if(i2c_wait_ack() != I2C_OK)
	{
		i2c_stop(); 
		return I2C_NO_ACK;
	}
	/*send register address*/
	i2c_send_byte(reg_addr);
	if (i2c_wait_ack() != I2C_OK)
	{
		i2c_stop(); 
		return I2C_NO_ACK;
	}
	/*restart and send read device address*/
	i2c_start();
	i2c_send_byte(dev_addr | 0x01);
	i2c_wait_ack();
	for (i = 0; i < size; i++)
	{
		*(buf+i) = i2c_receive_byte();

		if (i != size-1) { i2c_ack(); }			
	}
	i2c_nack();
	i2c_stop();

	return  I2C_OK;
}

i2c_exit_code_e i2c_dev_detect(unsigned char dev_addr)
{
	i2c_exit_code_e ret = I2C_OK;

	if (i2c_start() != I2C_OK) {
		return I2C_START_FAIL;
	}
	i2c_send_byte(dev_addr);
	if (i2c_wait_ack() != I2C_OK) {
		ret = I2C_NO_ACK;
	}	
	i2c_stop(); 

	return ret;
}
