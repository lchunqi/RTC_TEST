#include <stdio.h>
#include "uart.h"
#include "string.h"
 
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;

uint32_t recvData = 0;
uint8_t recvStat = COM_STOP_BIT;
 
uint8_t SUartCnt;                    //模拟串口缓冲区长位置
uint8_t SUartBuff[SUartLength];  
 

 
/***************************************
*    函 数 名: Delay_Ms
*    功能说明: 延时
*    形    参：nTime,单位为uS
*    返 回 值: 无
****************************************/
void iouart1_delayUs(volatile uint32_t nTime)
{ 
    uint16_t tmp;
    tmp = __HAL_TIM_GetCounter(&htim6);    //获得 TIM6 计数器的值
    if(tmp + nTime <= 65535)
        while( (__HAL_TIM_GetCounter(&htim6) - tmp) < nTime );
    else
    {
        __HAL_TIM_SetCounter(&htim6,0);  //设置 TIM3 计数器寄存器值为0
        while( __HAL_TIM_GetCounter(&htim6) < nTime );
    }
}
 
/*****************************************
*    函 数 名: iouart1_SendByte
*    功能说明: 模拟串口发送一字节数据
*    形    参：无
*    返 回 值: 无
******************************************/
 
//模拟串口发送一个字节
void S_Uart_One_Tx(uint8_t Data)
{
    uint8_t i;
 
    iouart1_TXD(0);  //起始位
    iouart1_delayUs(104);
 
    for(i=0; i<8; i++)
    {
        if(Data & 0x01)            //串口协议 先发LSB
        {    
            iouart1_TXD(1);
        }
        else 
        {
            iouart1_TXD(0);
        }
        
        iouart1_delayUs(104);
        Data >>= 1;
    }
    
    iouart1_TXD(1);    //结束标志位
    iouart1_delayUs(104);
}
 
//模拟串口发送数组
void S_Uart_Send_Buff(uint8_t *buff,uint8_t length)
{
    for(uint8_t i=0; i<length; i++)
    {
        S_Uart_One_Tx(buff[i]);    
    }
}
 
 //模拟串口发送字符串
void S_Uart_Send_Str(uint8_t *str)
{
//    uint32_t i = 0;
    uint32_t len = strlen((const char*)str);
    for(uint8_t i=0; i<len; i++)
    {
        S_Uart_One_Tx(str[i]);    
    }
}
 
 
//接收串口数据处理
uint8_t S_Uart_Rx_Handler(uint8_t *buf,uint8_t *length)
{
    *length = 0;
    if(SUartCnt > 0)            //模拟串口缓冲区不为空
    {
        *length = SUartCnt;            
        memcpy(buf,SUartBuff,*length);        //将SUartBuff里面的数据复制到buf里面，长度为*length        
        SUartCnt = 0;
//        memset(SUartBuff,0x00,256);  //清除缓存        
    }    
    return *length;
}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_4)
    {                            
        if(iouart1_RXD() == 0) 
        {
            if(recvStat == COM_STOP_BIT)
            {
                HAL_NVIC_DisableIRQ(EXTI1_IRQn);   //关闭下降沿中断
                recvStat = COM_START_BIT;
                //延时52us
                iouart1_delayUs(52);  //延时超过检测电平的正中间
                HAL_TIM_Base_Start_IT(&htim7);   //开启定时器7                
            }
        }
    }    
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
    if(htim->Instance == TIM7)
        {            
            recvStat++;
            if(recvStat == COM_STOP_BIT)//COM_D7_BIT
            {
                //到这里接收完成一个字节数据
                HAL_NVIC_EnableIRQ(EXTI1_IRQn);//开启下降沿中断
                 HAL_TIM_Base_Stop(&htim7);//关闭定时器
                __HAL_TIM_SetCounter(&htim7,0);//定时器清零
                if(SUartCnt < SUartLength)
                    SUartBuff[SUartCnt++] = recvData;            //存入缓冲区
                else 
                    SUartCnt = 0;
                recvData =0;
                
                return;
            }
            if(iouart1_RXD())
            {
                recvData |= (1 << (recvStat - 1));
            }else{
                recvData &= ~(1 << (recvStat - 1));
            }
        }
  /* USER CODE END Callback 1 */
}
