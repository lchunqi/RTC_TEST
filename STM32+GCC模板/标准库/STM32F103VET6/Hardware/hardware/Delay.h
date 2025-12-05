#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"
#include "stdint.h"

#define LED_G_GPIO_PIN              GPIO_Pin_8
#define LED_G_GPIO_PORT             GPIOB
#define LED_G_GPIO_CLK              RCC_APB2Periph_GPIOB


#define    ON        1
#define    OFF       0

// \  C语言里面叫续行符，后面不能有任何的东西

#define   LED_G(a)   if(a) \
	                       GPIO_ResetBits(LED_G_GPIO_PORT, LED_G_GPIO_PIN); \
                     else  GPIO_SetBits(LED_G_GPIO_PORT, LED_G_GPIO_PIN);

void LED_GPIO_Config(void);
void SysTick_Delay_us(uint32_t us);



void Delay_us(uint32_t xus);
void Delay_ms(uint32_t xms);
void Delay_s(uint32_t s);

#endif
