
/*
 *  drivers  rtc i2c
 * Copyright (C) 2022 shenzhen wave
 * version:v1.0.0--2023-8-1
 * This program is free software; you can redistribute it and/or modify
 */
#include "main.h"
#include "rtc_i2c.h"
#include "string.h"


typedef struct {
	GPIO_TypeDef* busSdaIn;
	uint16_t pinSdaIn;
	GPIO_TypeDef* busSdaOut;
	uint16_t pinSdaOut;
	GPIO_TypeDef* busScl;
	uint16_t pinScl;
} I2cGpio;

typedef struct {
	GPIO_TypeDef* busCE;
	uint16_t pinCE;
	GPIO_TypeDef* busCLK;
	uint16_t pinCLK;
	GPIO_TypeDef* busMOSI;
	uint16_t pinMOSI;
	GPIO_TypeDef* busMISO;
	uint16_t pinMISO;
} SpiGpio;
#define HORIZONTAL_COUNT	5
#define VERTICAL_COUNT		10
uint16_t soft_rtc_delay = 600;
uint8_t g_state = 0;
static I2cGpio  g_i2c = {0};
static SpiGpio g_spi = {0};
void rtc_scl_init(uint32_t mode)
{
  	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
							|GPIO_PIN_8|GPIO_PIN_9;
	GPIO_InitStruct.Mode = mode;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);  
}

uint8_t check_sda_level(void)
{
	return HAL_GPIO_ReadPin(g_i2c.busSdaIn, g_i2c.pinSdaIn);
}

void rtc_set_channel(uint8_t channel, RtcI2cState state)
{
    uint8_t sda_channel = 0;
    uint8_t scl_channel = 0;
  	GPIO_InitTypeDef GPIO_InitStruct = {0};

	switch (state) {
		case RTC_VDD_OFF:
		break;
		case RTC_INIT_1006://SCL为列，SDA为行
			scl_channel = ((channel - 1) % VERTICAL_COUNT); 
			if (scl_channel < 4) {
				g_i2c.busScl = GPIOC;
				g_i2c.pinScl = GPIO_PIN_10 << (scl_channel);
			} else {
				g_i2c.busScl = GPIOD;
				g_i2c.pinScl = GPIO_PIN_10 << (scl_channel - 4);
			}

			sda_channel = ((channel - 1) / VERTICAL_COUNT);
			g_i2c.busSdaIn = GPIOE;
			g_i2c.pinSdaIn = GPIO_PIN_0 << sda_channel;//输入和输出不同的线
			g_i2c.busSdaOut = GPIOB;
			if (sda_channel < 2) {
				g_i2c.pinSdaOut = GPIO_PIN_8 << sda_channel;
			} else {
				g_i2c.pinSdaOut = GPIO_PIN_12 << (sda_channel - 2);
			}
            // HAL_GPIO_WritePin(g_i2c.busSdaOut, g_i2c.pinSdaOut, GPIO_PIN_RESET);
			g_state = state;
			soft_rtc_delay = 600;
		break;
		case RTC_SD8825://SCL为列，SDA为行
		//SD8825测试模式下
			scl_channel = ((channel - 1) % VERTICAL_COUNT); 
			g_i2c.busScl = GPIOC;
			g_i2c.pinScl = GPIO_PIN_0 << scl_channel;

			sda_channel = ((channel - 1) / VERTICAL_COUNT);
			g_i2c.busSdaIn = GPIOE;
			g_i2c.pinSdaIn = GPIO_PIN_0 << sda_channel;//输入和输出是相同的线
			g_i2c.busSdaOut = GPIOE;
			g_i2c.pinSdaOut = GPIO_PIN_0 << sda_channel;
			if (sda_channel < 2) {
            	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 << sda_channel, GPIO_PIN_RESET);
			} else {
            	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 << (sda_channel - 2), GPIO_PIN_RESET);
			}
			soft_rtc_delay = 50;
			g_state = state;
		break;
		case RTC_SD8840:
			sda_channel = ((channel - 1) % VERTICAL_COUNT);
			scl_channel = ((channel - 1) / VERTICAL_COUNT);
			g_spi.busMOSI = GPIOB;
			g_spi.pinMOSI = GPIO_PIN_6;
			GPIO_InitStruct.Pin = GPIO_PIN_6;  
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
			g_spi.busMISO = GPIOB;
			g_spi.pinMISO = GPIO_PIN_7;
			GPIO_InitStruct.Pin = GPIO_PIN_7;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
			HAL_GPIO_WritePin(GPIOB,  GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);
			if (scl_channel < 2) {
				g_spi.pinCE = GPIO_PIN_8 << scl_channel;
			} else {
				g_spi.pinCE = GPIO_PIN_12 << (scl_channel - 2);
			}
			g_spi.busCE = GPIOB;
			g_spi.busCLK = GPIOE;
			g_spi.pinCLK = GPIO_PIN_0 << scl_channel;

			soft_rtc_delay = 600;
			g_state = state;
		break;
		default:break;
	}
}
/**
 * @brief set sda is high
 * 
 */
void rtc_sda_h(void)
{
	/*user code*/
	if (g_state == RTC_SD8825) {
		HAL_GPIO_WritePin(g_i2c.busSdaOut, g_i2c.pinSdaOut, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(g_i2c.busSdaOut, g_i2c.pinSdaOut, GPIO_PIN_RESET);
	}
}

/**
 * @brief set sda is low
 * 
 */
void rtc_sda_l(void)
{
	/*user code*/
	if (g_state == RTC_SD8825) {
		HAL_GPIO_WritePin(g_i2c.busSdaOut, g_i2c.pinSdaOut, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(g_i2c.busSdaOut, g_i2c.pinSdaOut, GPIO_PIN_SET);
	}
}

/**
 * @brief set scl is high
 * 
 */
void rtc_scl_h(void)
{
	/*user code*/
    HAL_GPIO_WritePin(g_i2c.busScl, g_i2c.pinScl, GPIO_PIN_SET);
}

/**
 * @brief set scl is low
 * 
 */
void rtc_scl_l(void)
{
	/*user code*/
    HAL_GPIO_WritePin(g_i2c.busScl, g_i2c.pinScl, GPIO_PIN_RESET);
}

/**
 * @brief Read sda pin state
 * 
 */
static unsigned char rtc_read_sda(void)
{
	return HAL_GPIO_ReadPin(g_i2c.busSdaIn, g_i2c.pinSdaIn);
}

/**
 * @brief When read data, need to set sda as in
 * 
 */
static inline void rtc_sda_in(void)
{
	/*user code */
  	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = g_i2c.pinSdaIn;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(g_i2c.busSdaIn, &GPIO_InitStruct);
}

/**
 * @brief When send data, need to set sda as out
 * 
 */
static inline void rtc_sda_out(void)
{
	/*user code */
  	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = g_i2c.pinSdaOut;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(g_i2c.busSdaOut, &GPIO_InitStruct);
}

/**
 * @brief i2c wiat
 * 
 */
static inline void rtc_Wait(void)
{	
	int i = soft_rtc_delay;
	while (i--);
}
/**
 * @brief i2c wiat
 * 
 */
static inline void rtc_Wait_half(void)
{	
	int i = soft_rtc_delay / 2;
	while (i--);
}

/**
 * @brief Generate an i2c strat signal
 * 
 */
i2c_exit_code_e rtc_start(void)
{
	rtc_sda_out();
	rtc_sda_h();
	rtc_scl_h();
	rtc_Wait();

	rtc_sda_l();
	rtc_Wait();
	rtc_Wait();
	rtc_scl_l();
	rtc_Wait();
	return I2C_OK;
}

/**
 * @brief Generate an i2c stop signal
 * 
 */
void rtc_stop(void)
{
	rtc_sda_out();
	rtc_sda_l();
	rtc_scl_l();
	rtc_Wait();
	rtc_scl_h();
	rtc_Wait();
	rtc_sda_h();
}

/**
 * @brief When data is read, the host responds a ack signal
 * 
 */
void rtc_ack(void)
{	
	rtc_scl_l();
	rtc_Wait_half();
	rtc_sda_l();
	rtc_Wait();
	rtc_scl_h();
	rtc_Wait();
	rtc_scl_l();
	rtc_Wait();
}

/**
 * @brief when data is read, the host responds a nack signal
 * 
 */
void rtc_nack(void)
{	
	rtc_sda_h();
	rtc_scl_l();
	rtc_Wait();
	rtc_scl_h();
	rtc_Wait();
	rtc_scl_l();
}

/**
 * @brief When data is written, the slave responds  a ack signal
 * 
 */
i2c_exit_code_e rtc_wait_ack(void)
{
	rtc_sda_in();
	rtc_scl_h();
	rtc_Wait();

	if (rtc_read_sda())
	{
		rtc_scl_l();
		return I2C_NO_ACK;
	}
	rtc_scl_l();
	rtc_Wait();
	rtc_sda_h();
	rtc_sda_out();
	return I2C_OK;
}

/**
 * @brief Send a byte to slave
 * 
 */
void rtc_send_byte(unsigned char value)
{
	unsigned char i;

	rtc_scl_l();
	for (i = 0; i < 8; i ++)
	{
		if (value & 0x80)
			rtc_sda_h();
		else 
			rtc_sda_l();
		rtc_Wait();
		value <<= 1;
		rtc_scl_h();
		rtc_Wait_half();
		rtc_scl_l();
		// rtc_Wait_half();
	}
	rtc_sda_h();
	rtc_Wait();
}

/**
 * @brief Receive a byte timing
 * 
 */
unsigned char rtc_receive_byte(void)
{
	unsigned char i;
	unsigned char data = 0;

    rtc_sda_in();
	rtc_sda_h();
	for (i = 0; i < 8; i++)
	{
		rtc_scl_l();
		// rtc_Wait();
		rtc_Wait();
		rtc_scl_h();
		rtc_Wait_half();
		data <<= 1;
		if (rtc_read_sda()) { data |= 0x01;	}
	}
	rtc_Wait();
	rtc_scl_l();
    rtc_sda_out();
	return data;
}

/**
 * @brief Write multiple bytes to slave 
 * 
 */
i2c_exit_code_e rtc_write_multi_bytes(unsigned char dev_addr, unsigned char reg_addr, const unsigned char *buf, unsigned short size)
{
    unsigned short i;

    if (rtc_start() != I2C_OK)
		return I2C_START_FAIL;

    rtc_send_byte(dev_addr);
	if (rtc_wait_ack() != I2C_OK)
	{
		rtc_stop(); 
		return I2C_NO_ACK;
	}
	rtc_send_byte(reg_addr);
	if (rtc_wait_ack() != I2C_OK)
	{
		rtc_stop(); 
		return I2C_NO_ACK;
	}
    for (i = 0; i < size; i++)
    {
    	rtc_send_byte(*(buf + i));
    	rtc_wait_ack();
    }
    rtc_stop();

	return	I2C_OK;
}

/**
 * @brief Read multiple bytes from slave 
 * 
 */
i2c_exit_code_e rtc_read_multi_bytes(unsigned char dev_addr, unsigned char reg_addr, unsigned char *buf, unsigned short size)
{
	unsigned short i;

	if(rtc_start() != I2C_OK)
		return I2C_START_FAIL;
	/*send device address*/
	rtc_send_byte(dev_addr);
	if(rtc_wait_ack() != I2C_OK)
	{
		rtc_stop(); 
		return I2C_NO_ACK;
	}
	/*send register address*/
	rtc_send_byte(reg_addr);
	if (rtc_wait_ack() != I2C_OK)
	{
		rtc_stop(); 
		return I2C_NO_ACK;
	}
	/*restart and send read device address*/
	rtc_start();
	rtc_send_byte(dev_addr | 0x01);
	rtc_wait_ack();
	for (i = 0; i < size; i++)
	{
		*(buf+i) = rtc_receive_byte();

		if (i != size-1) { rtc_ack(); }			
	}
	rtc_nack();
	rtc_stop();

	return  I2C_OK;
}


//-----------------------------------------------soft spi  start-------------------------------------------------------

void spi_CS_H(void)
{
	HAL_GPIO_WritePin(g_spi.busCE, g_spi.pinCE, GPIO_PIN_RESET);
}

void spi_CS_L(void)
{
	HAL_GPIO_WritePin(g_spi.busCE, g_spi.pinCE, GPIO_PIN_SET);
}

void spi_CLK_H(void)
{
	HAL_GPIO_WritePin(g_spi.busCLK, g_spi.pinCLK, GPIO_PIN_SET);
}

void spi_CLK_L(void)
{
	HAL_GPIO_WritePin(g_spi.busCLK, g_spi.pinCLK, GPIO_PIN_RESET);
}

void spi_send_one_byte(unsigned char buf)
{
	uint8_t i;
	spi_CLK_L();
	for (i = 0; i < 8; i++) {
		if (buf & 0x80) {
			HAL_GPIO_WritePin(g_spi.busMOSI, g_spi.pinMOSI, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(g_spi.busMOSI, g_spi.pinMOSI, GPIO_PIN_RESET);
		}
		rtc_Wait();
		spi_CLK_H();
		rtc_Wait();
		spi_CLK_L();
		buf <<= 1;
	}
}

void spi_receive_one_byte(unsigned char* buf)
{
	uint8_t i;
	*buf = 0;
	for (i = 0; i < 8; i++) {
		spi_CLK_L();
		rtc_Wait();
		spi_CLK_H();
		*buf <<= 1;
		if (HAL_GPIO_ReadPin(g_spi.busMISO, g_spi.pinMISO)) {
			*buf |= 0x01;
		}
		rtc_Wait();
	}
}

void spi_read_multi_bytes(unsigned char reg_addr, unsigned char *buf, unsigned short size)
{
	uint8_t i;
	spi_CLK_H();
	spi_CS_H();
	rtc_Wait();
	reg_addr |= 0x80;
	spi_send_one_byte(reg_addr);
	for (i = 0; i < size; i++) {
		spi_receive_one_byte(buf + i);
	}
	spi_CS_L();
}

void spi_write_multi_bytes(unsigned char reg_addr, unsigned char *buf, unsigned short size)
{
	uint8_t i;
	
	spi_CLK_H();
	spi_CS_H();
	rtc_Wait();
	reg_addr &= 0x7f;
	spi_send_one_byte(reg_addr);
	for (i = 0; i < size; i++) {
		spi_send_one_byte(buf[i]);
	}
	spi_CS_L();
}
//-----------------------------------------------soft spi  end-------------------------------------------------------
