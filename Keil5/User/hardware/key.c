#include "config.h"

/**
功能：按键初始化
修改：sjm
参数：-
返回值：-
**/
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);
//	
//	EXTI_InitTypeDef EXTI_InitStructure;
//	EXTI_InitStructure.EXTI_Line = EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;		//Rising_Falling;
//	EXTI_Init(&EXTI_InitStructure);
//	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
//	
//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_Init(&NVIC_InitStructure);
	
}


//void EXTI15_10_IRQHandler(void)
//{
//	if (EXTI_GetITStatus(EXTI_Line12)!= RESET)
//	{
//		if(KEY_UP==0)
//		{
//			func_index = table[func_index].up;   
//			while (!KEY_UP);
//		}
//		
//		EXTI_ClearITPendingBit(EXTI_Line12);
//	}
//	
// 
//	if (EXTI_GetITStatus(EXTI_Line13)!= RESET)
//	{

//		if(KEY_BACK==0)
//		{
//				func_index = table[func_index].back;    
//				while (!KEY_BACK);
//		}
//		
//		EXTI_ClearITPendingBit(EXTI_Line13);
//	}
//	
//	if (EXTI_GetITStatus(EXTI_Line14) != RESET)
//	{
//		if(KEY_DOWN==0)
//		{
//				func_index = table[func_index].down;    
//				while (!KEY_DOWN);

//		}
//		
//		EXTI_ClearITPendingBit(EXTI_Line14);
//	}
//	
//	if (EXTI_GetITStatus(EXTI_Line15)!= RESET)
//	{
//		
//		if(KEY_ENTER==0)
//		{
//				func_index = table[func_index].enter;    
//				while (!KEY_ENTER);
//		}
//		
//		EXTI_ClearITPendingBit(EXTI_Line15);
//	}
//	
//}

