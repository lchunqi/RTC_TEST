#include "config.h"

/**
功能：蜂鸣器初始化
修改：sjm
参数：-
返回值：-
**/
void BEEP_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
}

/**
功能：蜂鸣器控制
修改：sjm
参数：BitValue-选择0或者1，打开或者关闭蜂鸣器
返回值：-
**/
void BEEP_Control(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)BitValue);
}

