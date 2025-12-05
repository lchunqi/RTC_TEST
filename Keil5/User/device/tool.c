#include "config.h"

/**
功能：10进制转bcd码
修改：sjm
参数：*p_dat-转换的数据
返回值：-
**/
void Data_DecodeBCD(uint16_t *p_dat)
{
	uint8_t i;
	for (i = 0; i < 7; i ++)
		p_dat[i] = p_dat[i]/16*10 + p_dat[i]%16;
}

/**
功能：检验和
修改：sjm
参数：*p_dat-检验的数据，leng-长度
返回值：-
**/
uint16_t CheckSum_uint(uint16_t *p_dat,uint8_t leng)
{
	uint16_t  sum = 0;
	do
	{
		leng --;
		sum += p_dat[leng];
	}while(leng);
	return sum;
}



