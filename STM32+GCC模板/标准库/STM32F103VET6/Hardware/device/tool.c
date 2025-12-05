#include "tool.h"

void Data_DecodeBCD(uint8_t *p_dat)
{
	uint8_t i;
	for (i = 0; i < 7; i ++)
		p_dat[i] = p_dat[i]/16*10 + p_dat[i]%16;
}

uint16_t CheckSum_uint(uint8_t *p_dat,uint8_t leng)
{
	uint16_t  sum = 0;
	do
	{
		leng --;
		sum += p_dat[leng];
	}while(leng);
	return sum;
}



