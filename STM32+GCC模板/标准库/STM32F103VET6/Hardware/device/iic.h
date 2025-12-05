#ifndef __IIC_H
#define __IIC_H

#include "Delay.h"
#include "stm32f10x.h"
#include <stdint.h>
#include <stdio.h>

#define i2c_wr	0	       /* 写控制bit */
#define i2c_rd	1	       /* 读控制bit */

/*SCL*/
#define  	SCL_TEST_Pin		GPIO_Pin_6		//引脚
#define  	SCL_STD_Pin			GPIO_Pin_3
#define  	SCL_GPS_Pin			GPIO_Pin_6

#define  	SCL_TEST_H			GPIOB->BSRR = GPIO_Pin_6	//待测时钟模块  置1
#define  	SCL_STD_H			GPIOE->BSRR = GPIO_Pin_3	//标准时钟源
#define  	SCL_GPS_H			GPIOC->BSRR = GPIO_Pin_6	//GPS模块

#define  	SCL_TEST_L			GPIOB->BRR = GPIO_Pin_6	//待测时钟模块  置0
#define  	SCL_STD_L			GPIOE->BRR = GPIO_Pin_3	//标准时钟源
#define 	SCL_GPS_L			GPIOC->BRR = GPIO_Pin_6		//GPS模块

/*SDA*/
#define 	SDA_TEST_Pin		GPIO_Pin_7     //引脚
#define 	SDA_STD_Pin			GPIO_Pin_2
#define 	SDA_GPS_Pin			GPIO_Pin_7

#define 	SDA_TEST_H			GPIOB->BSRR = GPIO_Pin_7	//待测时钟模块  置1
#define 	SDA_STD_H			GPIOE->BSRR = GPIO_Pin_2	//标准时钟源
#define 	SDA_GPS_H			GPIOC->BSRR = GPIO_Pin_7	//GPS模块

#define 	SDA_TEST_L			GPIOB->BRR = GPIO_Pin_7	//待测时钟模块  置0
#define 	SDA_STD_L			GPIOE->BRR = GPIO_Pin_2	//标准时钟源
#define 	SDA_GPS_L			GPIOC->BRR = GPIO_Pin_7		//GPS模块

#define 	SDA_TEST_read       (GPIOB->IDR  & GPIO_Pin_7)   //读SDA引脚状态
#define 	SDA_STD_read        (GPIOE->IDR  & GPIO_Pin_2)
#define 	SDA_GPS_read        (GPIOC->IDR  & GPIO_Pin_7)


//gps,wifi暂未实现
#define  	BUSY		GPIO_Pin_8
#define  	BUSY_H		GPIOA->BSRR = GPIO_Pin_8
#define  	BUSY_L		GPIOA->BRR = GPIO_Pin_8
#define  	BUSY_read	(GPIOA->IDR & GPIO_Pin_8)

#define  	GPS_EN		GPIO_Pin_9
#define  	GPS_EN_H	GPIOC->BSRR = GPIO_Pin_9
#define  	GPS_EN_L	GPIOC->BRR = GPIO_Pin_9


typedef enum
{
	IIC_OK = 2,     			/*i2c正常*/
	IIC_FAIL = 3,					/*i2c开启失败*/
	SLAVE_ACK = 4,					/*slave应答*/
	SLAVE_NO_ACK = 5				/*slave没有应答*/
}i2c_exit_code;

//IIC总线挂载器件集合
typedef enum
{
	IIC_TEST    = 0,  //待测芯片
	IIC_STD     = 1,  //标准时钟源
	IIC_GPS     = 2,  //gps/wifi
	IIC_SD22  	= 3,  //sd22系列
}i2c_device_bus;



void SET_SDA_H(i2c_device_bus iic_x);
void SET_SDA_L(i2c_device_bus iic_x);
void SET_SCL_H(i2c_device_bus iic_x);
void SET_SCL_L(i2c_device_bus iic_x);
uint8_t Read_SDA(i2c_device_bus iic_x);
void IIC_Init(void);
void IIC_Delay(void);
void IIC_Stop(i2c_device_bus iic_x);
i2c_exit_code IIC_Start(i2c_device_bus iic_x);
void IIC_Ack(i2c_device_bus iic_x);
void IIC_NoAck(i2c_device_bus iic_x);
i2c_exit_code IIC_WaitAck(i2c_device_bus iic_x);
uint8_t IIC_ReceiveByte(i2c_device_bus iic_x);
void IIC_SendByte(uint8_t byte, i2c_device_bus iic_x);
uint8_t RTC_ReadByte(uint8_t addr,i2c_device_bus bus);
i2c_exit_code IIC_read_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t *Data, i2c_device_bus bus);
i2c_exit_code IIC_write_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t Data, i2c_device_bus bus);
i2c_exit_code IIC_read_multiple_bytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *buf, uint8_t length, i2c_device_bus bus);
i2c_exit_code IIC_write_multiple_bytes(uint8_t dev_addr,uint8_t reg_addr, uint8_t *buf, uint8_t length, i2c_device_bus bus);




#endif

