#ifndef __IIC_SOFT_H__
#define __IIC_SOFT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "temp.h"
#include "manager.h"
typedef enum{
	I2C_NO_ACK =0,
	I2C_ACK,
	I2C_OK,
	I2C_Start_FAIL,
	I2C_slave_FAIL,
	
}IIC_soft;


#define  RTC_64_dev  0X64
#define  RTC_A2_dev  0XA2
#define  RTC_sec  	 0X02
#define  I2C_delay   500
#define  I2C_GPIO	 GPIOC

void IIC_GPIO_Init(uint16_t pinSda, uint16_t pinScl);

void IIC_SDA_IN(void);
void IIC_SDA_OUT(void);
uint8_t IIC_Read_SDA(void);
IIC_soft IIC_Start(void);
void IIC_Stop(void);
void IIC_ACK(void);
void IIC_NACK(void);
IIC_soft IIC_Wait_ACK(void);
void IIC_Send_Data(uint8_t data);
uint8_t IIC_Receive_Data(void);
IIC_soft IIC_Read_multi_bytes(uint8_t dev_addr,uint8_t reg_addr, uint8_t *buf, uint8_t size);
IIC_soft IIC_Write_multi_bytes(uint8_t dev_addr, uint8_t reg_addr, const uint8_t *buf, uint8_t size);

uint8_t I2C_Connect(void);
uint8_t I2C_ReadTime(uint8_t i,uint8_t j);
void Close8800_FREQ(void);
void Close8801_FREQ(void);
void Close8810_FREQ(void);
void Close8825_FREQ(void);
void Close8931_FREQ(void);										
void Close3178_FREQ(void);
void Close3077_FREQ(void);
void SD3077_Write_Enable(void);
void SD3077_Write_Disable(void);
void SD3178_Write_Enable(void);
void SD3178_Write_Disable(void);

void RTC_3077_TEMP(float* sysTemp , uint8_t* rtcTemp);
void RTC_3178_TEMP(float* sysTemp , uint8_t* rtcTemp);
void RTC_8800_TEMP(float* sysTemp , uint8_t* rtcTemp);
void RTC_8804_TEMP(float* sysTemp , uint8_t* rtcTemp);
void RTC_8810_TEMP(float* sysTemp , uint8_t* rtcTemp);
void RTC_8825_TEMP(float* sysTemp , uint8_t* rtcTemp);
void RTC_8931_TEMP(float* sysTemp , uint8_t* rtcTemp);


#ifdef __cplusplus
}
#endif
#endif 

