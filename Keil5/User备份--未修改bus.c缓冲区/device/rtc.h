#ifndef __RTC_H
#define __RTC_H

#include "iic.h"
#include "spi.h"
#include "oled.h"
#include "tool.h"
#include "Delay.h"
#include "usart.h"
#include "stm32f10x.h"
#include <stdint.h>
#include <stdio.h>


#define	RTC_WRITE		0x64
#define	RTC_READ		0x65

#define	FALSE		0
#define	TRUE		1


typedef enum
{
	RTC_OK    = 6,  //正常退出
	RTC_NOACK   = 7,  //无应答
	RTC_FAIL = 8,  //异常退出
}rtc_exit_code;


typedef enum{
	RTC_TIME		= 0x00,
	RTC_CTR1		= 0x0F,
	RTC_CTR2		= 0x10,
	RTC_CTR3		= 0x11,
	RTC_TEMPER		= 0x16,
	AGTC			= 0x17,
	RTC_CHARGE  	= 0x18,
	RTC_CTR4		= 0x19,
	RTC_CTR5		= 0x1A,	
	RTC_VBAT		= 0x1B,
	RTC_GPSEN  	 	= 0x2C,
	RTC_ID			= 0x72
}RtcAddr_TypeDef;


typedef struct{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t week;
	uint8_t day;
	uint8_t month;
	uint8_t year;	
}rtc_time;

//decimal to bcd
uint8_t dec_to_bcd(uint8_t dec);
//bcd to decimal
uint8_t bcd_to_dec(uint8_t bcd);
int HextoDecimal(uint8_t hex);

rtc_exit_code RTC_WriteOn(i2c_device_bus IIC_x);
rtc_exit_code RTC_WriteOff(i2c_device_bus IIC_x);
rtc_exit_code RTC_read_addr(uint8_t reg_addr, uint8_t *read_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code RTC_write_addr(uint8_t reg_addr, uint8_t *write_buf, uint8_t length, i2c_device_bus bus);
rtc_exit_code RTC_ReadTimeDate(rtc_time *time, i2c_device_bus IIC_x);
rtc_exit_code RTC_WriteTimeDate(rtc_time *time, i2c_device_bus IIC_x);

uint8_t RTC_ReadRAM(uint8_t addr,uint8_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t RTC_WriteRAM(uint8_t addr,uint8_t *data,uint8_t len,i2c_device_bus IIC_x);
uint8_t RTC_ReadID(uint8_t *data,i2c_device_bus IIC_x);
uint8_t RTC_VbatQuan(uint8_t *data,i2c_device_bus IIC_x);


//串口走时测试
uint8_t sd2405time(void);
uint8_t sd3068time(void);
//oled显示
uint8_t sd2405showtime(void);//模块时间
uint8_t Time_test_show(void);//模块/待测 走时

void NO_ACK_Test_Line(void);//无应答线
void Test_Line(void);//测试线
void Test_Control(void);//测试框架

uint8_t RTC_ReadTemp(i2c_device_bus IIC_x);
double RTC_ReadVbat(i2c_device_bus IIC_x);

uint8_t rtc_ifbuff(uint8_t *Data, i2c_device_bus IIC_x);

void sd2506_idshow(void);
void Test_id(void);
uint8_t sd2505showtime(void);
void sd2506_messgae_line(void);

uint8_t sd2xxx_and_std(void);
uint8_t sd2xxx_time_show(void);
uint8_t sd2xxx_time_copy(void);

uint8_t sd3xxx_and_std(void);
uint8_t sd3xxx_time_show(void);
uint8_t sd3xxx_time_copy(void);
void sd3xxx_charge_on(void);
void sd3xxx_charge_off(void);
void sd3xxx_messgae_line(void);
void NO_ACK_sd3xxx_messgae_line(void);



#endif
