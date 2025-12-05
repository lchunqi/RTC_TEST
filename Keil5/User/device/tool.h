#ifndef __TOOL_H
#define __TOOL_H

#include "stm32f10x.h"
#include <stdint.h>

void Data_DecodeBCD(uint16_t *p_dat);
uint16_t CheckSum_uint(uint16_t *p_dat,uint8_t leng);



#endif
