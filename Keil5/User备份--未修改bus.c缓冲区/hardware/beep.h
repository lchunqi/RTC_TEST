#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f10x.h"
#include "stdint.h"

void BEEP_GPIO_Config(void);
void BEEP_Control(uint8_t BitValue);


#endif

