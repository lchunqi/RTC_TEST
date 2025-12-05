#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
#include "stdint.h"


void LED_GPIO_Config(void);
void LED_Control(uint8_t BitValue);

#endif

