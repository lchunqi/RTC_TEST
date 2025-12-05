
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
}I2cGpio;

uint16_t soft_rtc_delay = 600;
uint8_t g_state = 0;
static I2cGpio  g_i2c = {0};
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

void rtc_set_channel(uint8_t channel, RtcI2cState state)
{
    uint8_t sda_channel = 0;
    uint8_t scl_channel = 0;

    g_i2c.busScl = GPIOC;
    scl_channel = ((channel - 1) % NUMBER_VERTICAL); 
	g_i2c.pinScl = GPIO_PIN_0 << scl_channel;
	g_state = state;

	switch (state) {
		case RTC_VDD_OFF:
		break;
		case RTC_INIT_1006:
			sda_channel = ((channel - 1) /  NUMBER_VERTICAL);
			g_i2c.busSdaIn = GPIOE;
			g_i2c.pinSdaIn = GPIO_PIN_0;
			g_i2c.busSdaOut = GPIOE;
			g_i2c.pinSdaOut = GPIO_PIN_1;
			//切换模拟开关通道
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 0x01 & sda_channel);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 0x02 & sda_channel);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 0x04 & sda_channel);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0x08 & sda_channel);
			soft_rtc_delay = 600;
			rtc_scl_init(GPIO_MODE_OUTPUT_OD);
		break;
		case RTC_SD3031:
			sda_channel = ((channel - 1) /  NUMBER_VERTICAL);
			g_i2c.busSdaIn = GPIOE;
			g_i2c.pinSdaIn = GPIO_PIN_0;
			g_i2c.busSdaOut = GPIOE;
			g_i2c.pinSdaOut = GPIO_PIN_0;
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 0x01 & sda_channel);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 0x02 & sda_channel);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 0x04 & sda_channel);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0x08 & sda_channel);
			soft_rtc_delay = 50;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);//必须,拉高芯片的PIN7 线，便于操作4号线
			rtc_scl_init(GPIO_MODE_OUTPUT_OD);
		break;
		case RTC_SD8800:
			sda_channel = ((channel - 1) /  NUMBER_VERTICAL);
			g_i2c.busSdaIn = GPIOE;
			g_i2c.pinSdaIn = GPIO_PIN_0;
			g_i2c.busSdaOut = GPIOE;
			g_i2c.pinSdaOut = GPIO_PIN_1;
			//切换模拟开关通道
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, 0x01 & sda_channel);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, 0x02 & sda_channel);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, 0x04 & sda_channel);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0x08 & sda_channel);
			soft_rtc_delay = 600;
			rtc_scl_init(GPIO_MODE_OUTPUT_OD);
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
	if (g_state == RTC_SD3031) {
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
	if (g_state == RTC_SD3031) {
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
	/*user code*/	
    // if (g_state == RTC_SD3031) {
	 	return g_i2c.busSdaIn->IDR & g_i2c.pinSdaIn;
	// } else {
	// 	if ((g_i2c.busSdaIn->IDR & g_i2c.pinSdaIn) != GPIO_PIN_RESET) {
	// 		return 0;
	// 	} else {
	// 		return 1;
	// 	}
	// }
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
	rtc_sda_l();
	rtc_scl_l();
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


uint8_t rtc_write_lock_on(void)
{		
    uint8_t buffer;
    buffer = 0x80;
    if (rtc_write_multi_bytes(0x64, 0x10, &buffer, 1) != I2C_OK) {return I2C_NO_ACK;}
    buffer = 0xff;
    rtc_write_multi_bytes(0x64, 0x0f, &buffer, 1);
    return	I2C_OK;
}

uint8_t rtc_write_lock_off(void)
{		
    uint8_t buffer;
    buffer = 0x7B;
    if (rtc_write_multi_bytes(0x64, 0x0f, &buffer, 1) != I2C_OK) {return I2C_NO_ACK;}
    buffer = 0x0;
    rtc_write_multi_bytes(0x64, 0x10, &buffer, 1);
    return	I2C_OK;
}

uint8_t rtc_download_otp_data(uint8_t* buffer, uint16_t startAddress, uint16_t length)
{
	int i;
	if (rtc_write_lock_on() != I2C_OK) {
		return I2C_NO_ACK;
	}
	
    if (rtc_start() != I2C_OK) {
		return I2C_START_FAIL;
	}
    rtc_send_byte(0x64);
	if (rtc_wait_ack() != I2C_OK) {
		rtc_stop(); 
		return I2C_NO_ACK;
	}
	if(startAddress > 0xFF) {
    	rtc_send_byte(0xff);
	} else {
    	rtc_send_byte(startAddress);
	}
	if (rtc_wait_ack() != I2C_OK) {
		rtc_stop(); 
		return I2C_NO_ACK;
	}
	if (startAddress > 0xFF) {
		for (i = 0xFF; i < startAddress; i++) {
			rtc_send_byte(0xff);
			if (rtc_wait_ack() != I2C_OK) {
				rtc_stop(); 
				return I2C_NO_ACK;
			}
		}
	}
	for (i = 0; i < length; i++) {				
		rtc_send_byte(buffer[i]);
		HAL_Delay(1);
		if (rtc_wait_ack() != I2C_OK) {
			rtc_stop(); 
			return I2C_NO_ACK;
		}
	}
	rtc_stop(); 
	
	rtc_write_lock_off();
	return I2C_OK;
}
