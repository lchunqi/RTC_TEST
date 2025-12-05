#include "Delay.h"


/**
  * @brief  微秒级延时
  * @param  xus 延时时长，范围：0~233015
  * @retval 无
  */
void Delay_us(uint32_t xus)
{
	
//	  uint32_t count = xus * (64000000 / 1000000) / 10; // 计算所需的循环次数
//    while(count--) {
//        __NOP(); // 空操作指令，用于增加延时
//    }
		
	uint32_t i;
	SysTick_Config(72);
	
	for(i=0; i<xus; i++)
	{
		while( !((SysTick->CTRL) & (1<<16)) );
	}
	
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
	
}

/**
  * @brief  毫秒级延时
  * @param  xms 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_ms(uint32_t xms)
{
	while(xms--)
	{
		Delay_us(1000);
	}
}

/**
  * @brief  秒级延时
  * @param  xs 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_s(uint32_t xs)
{
	while(xs--)
	{
		Delay_ms(1000);
	}
} 
