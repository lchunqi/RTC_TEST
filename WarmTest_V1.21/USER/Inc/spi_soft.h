/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-10-24 16:16:39
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-11-13 13:46:15
 * @FilePath: \WarmTest_V1.2\USER\Inc\spi_soft.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __SPI_SOFT_H__
#define __SPI_SOFT_H__
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "temp.h"

#define SPI_GPIO     GPIOB
#define SPI_GPIOC    GPIOC

#define SPI_CS_PIN   GPIO_PIN_0
#define SPI_CLK_PIN	 GPIO_PIN_8
//引脚反接了
// #define SPI_DI_PIN   GPIO_PIN_11
// #define SPI_DO_PIN	 GPIO_PIN_14
#define SPI_DI_PIN   GPIO_PIN_14
#define SPI_DO_PIN	 GPIO_PIN_11

// #define SPI_CS_PIN   GPIO_PIN_1
// #define SPI_CLK_PIN	 GPIO_PIN_10
// #define SPI_DI_PIN   GPIO_PIN_13
// #define SPI_DO_PIN	 GPIO_PIN_14

#define SPI_delay    250

void    SPI_GPIO_Init(  uint16_t Row_pin , uint16_t pinCS );

void    SPI_Write_multi_byte(uint8_t reg_add, uint8_t *data,uint8_t size);
uint8_t SPI_Read_multi_byte(uint8_t reg_add, uint8_t *data,uint8_t size);
uint8_t SPI_Connect(uint8_t REG);
uint8_t SPI_ReadTime(uint8_t j,uint8_t i);
void    Close8840_FREQ(void);
void    RTC_8840_TEMP(float* sysTemp,uint8_t* rtcTemp);






#ifdef __cplusplus
}
#endif

#endif 



