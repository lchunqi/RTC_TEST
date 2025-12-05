#ifndef __SPI_H
#define __SPI_H

#include "rtc.h"
#include "Delay.h"
#include "oled.h"
#include "usart.h"
#include "stm32f10x.h"
#include <stdint.h>

typedef enum
{
	SPI_OK    = 0,  //正常退出
	SPI_FAIL = 1,  //异常退出
}spi_exit_code;

/*SD8840*/
#ifndef HIGH
#define HIGH                1
#endif /* HIGH */

#ifndef LOW
#define LOW                 0
#endif /* LOW */

#define SL_SPI_CS_CLK		RCC_APB2Periph_GPIOA
#define SL_SPI_CS_PORT   	GPIOA
#define SL_SPI_CS_PINS   	GPIO_Pin_4

#define SL_SPI_SCK_CLK		RCC_APB2Periph_GPIOA
#define SL_SPI_SCK_PORT		GPIOA
#define SL_SPI_SCK_PINS		GPIO_Pin_5

#define SL_SPI_MOSI_CLK		RCC_APB2Periph_GPIOA
#define SL_SPI_MOSI_PORT	GPIOA
#define SL_SPI_MOSI_PINS	GPIO_Pin_6

#define SL_SPI_MISO_CLK		RCC_APB2Periph_GPIOA
#define SL_SPI_MISO_PORT	GPIOA
#define SL_SPI_MISO_PINS	GPIO_Pin_7

#define SPI_CS(x)			GPIO_WriteBit(SL_SPI_CS_PORT, SL_SPI_CS_PINS, (BitAction)x)
#define SPI_SCK(x)			GPIO_WriteBit(SL_SPI_SCK_PORT, SL_SPI_SCK_PINS, (BitAction)x)
#define SPI_MOSI(x)			GPIO_WriteBit(SL_SPI_MOSI_PORT, SL_SPI_MOSI_PINS, (BitAction)x)
#define SPI_MISO			GPIO_ReadInputDataBit(SL_SPI_MISO_PORT, SL_SPI_MISO_PINS)

void SPI_init(void);
uint8_t spi_read_multi_bytes(uint8_t reg_addr, uint8_t *buf, uint8_t size);
uint8_t spi_write_multi_bytes(uint8_t reg_addr, uint8_t *buf, uint8_t size);
void spi_readtime(uint8_t *time);
void spi_readtime_show(uint8_t *time);

spi_exit_code SD8840_ReadTimeDate(void);//对比
spi_exit_code SD8840_ReadTimeDate_show(void);//单独
spi_exit_code SD8840_WriteTimeDate(void);

uint8_t sd8840_and_std(void);
uint8_t sd8840_time_show(void);
uint8_t sd8840_time_copy(void);
uint8_t RTC_ReadRAM_8840(uint8_t addr,uint8_t *data,uint8_t len);
uint8_t RTC_WriteRAM_8840(uint8_t addr,uint8_t *data,uint8_t len);
uint8_t sd8840_ReadTemp(void);

spi_exit_code sd8840_f32768(void);
spi_exit_code sd8840_f32768_off(void);

spi_exit_code sd8840_f1024(void);
spi_exit_code sd8840_f1024_off(void);

spi_exit_code sd8840_f1(void);
spi_exit_code sd8840_f1_off(void);



/*SD8902*/
#define sd8902_CLK_RCC		RCC_APB2Periph_GPIOA		//时钟
#define sd8902_CLK_PORT		GPIOA						//端口
#define sd8902_CLK_PIN		GPIO_Pin_5					//引脚

#define sd8902_DAT_RCC		RCC_APB2Periph_GPIOC		//时钟
#define sd8902_DAT_PORT		GPIOC						//端口
#define sd8902_DAT_PIN		GPIO_Pin_4					//引脚

#define sd8902_RST_RCC		RCC_APB2Periph_GPIOA		//时钟
#define sd8902_RST_PORT		GPIOA						//端口
#define sd8902_RST_PIN		GPIO_Pin_4					//引脚

#define sd8902_CLK_HIGH		GPIO_SetBits(sd8902_CLK_PORT, sd8902_CLK_PIN)			//引脚输出高电平
#define sd8902_CLK_LOW		GPIO_ResetBits(sd8902_CLK_PORT, sd8902_CLK_PIN)			//引脚输出低电平

#define sd8902_DAT_HIGH		GPIO_SetBits(sd8902_DAT_PORT, sd8902_DAT_PIN)			//引脚输出高电平
#define sd8902_DAT_LOW		GPIO_ResetBits(sd8902_DAT_PORT, sd8902_DAT_PIN)			//引脚输出低电平

#define sd8902_RST_HIGH		GPIO_SetBits(sd8902_RST_PORT, sd8902_RST_PIN)			//引脚输出高电平
#define sd8902_RST_LOW		GPIO_ResetBits(sd8902_RST_PORT, sd8902_RST_PIN)			//引脚输出低电平

#define sd8902_SET_IN		sd8902_set_input_mode()							//设置输入模式
#define sd8902_SET_OUT 		sd8902_set_output_mode()						//设置输出模式		

/*读相关寄存器地址*/
#define sd8902_READ_SECOND 	0X81   	//秒
#define sd8902_READ_MINUTE 	0X83	//分
#define sd8902_READ_HOUR   	0X85	//时
#define sd8902_READ_DAY		0X87	//日
#define sd8902_READ_MONTH 	0X89	//月
#define sd8902_READ_WEEK 	0X8B	//周
#define sd8902_READ_YEAR 	0X8D	//年

/*写相关寄存器地址*/
#define sd8902_WRITE_SECOND 0X80   	//秒
#define sd8902_WRITE_MINUTE 0X82	//分
#define sd8902_WRITE_HOUR   0X84	//时
#define sd8902_WRITE_DAY	0X86	//日
#define sd8902_WRITE_MONTH 	0X88	//月
#define sd8902_WRITE_WEEK 	0X8A	//周
#define sd8902_WRITE_YEAR 	0X8C	//年
#define sd8902_WRITE_PROTECT 0X8E	//保护

void sd8902_write_byte(uint8_t addr_or_data);//sd8902写一字节函数
void sd8902_write_data(uint8_t addr,uint8_t dat);//sd8902写命令函数
uint8_t sd8902_read_byte(void);//sd8902 读一字节 函数
uint8_t sd8902_read_data(uint8_t addr);	//sd8902 读一字节 函数
uint8_t sd8902_read_multi_bytes(uint8_t reg_addr, uint8_t *buf, uint8_t len);
uint8_t sd8902_write_multi_bytes(uint8_t reg_addr, uint8_t *buf, uint8_t len);

void bcd_to_decimal(uint8_t *bcd,uint8_t times);//BCD 转 十进制 函数
void decimal_to_bcd(uint8_t *dec,uint8_t times);//十进制 转 BCD 函数
void sd8902_init(void);	//sd8902 初始化日期和时间 函数
void sd8902_config(void);
void sd8902_readtime_show(uint8_t *time);

spi_exit_code SD8902_ReadTimeDate(void);//对比
spi_exit_code SD8902_ReadTimeDate_show(void);//单独
spi_exit_code SD8902_WriteTimeDate(void);

uint8_t sd8902_and_std(void);
uint8_t sd8902_time_show(void);
uint8_t sd8902_time_copy(void);
uint8_t RTC_ReadRAM_8902(uint8_t addr, uint8_t *data, uint8_t len);
uint8_t RTC_WriteRAM_8902(uint8_t addr,uint8_t *data,uint8_t len);
uint8_t sd8902_ReadTemp(void);
uint8_t sd8902_ReadVbat(void);
uint8_t sd8902_id_show(void);
void sd8902_charge_on(void);
void sd8902_charge_off(void);

spi_exit_code sd8902_f32768(void);
spi_exit_code sd8902_f32768_off(void);


#endif

