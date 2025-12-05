#ifndef __UART_H_
#define __UART_H_
 
#include "stm32f4xx_hal.h"

//#include "sys.h"
//#include "delay.h"
 
 
//定义通信波特率
#define BaudRate_9600    104        //1000000us/9600=104.1666    发送1个位所需要的时间
//GPIO TX脚宏定义
#define iouart1_TXD(n)  if(n) HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET); \
                              else  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
//GPIO RX脚宏定义
#define iouart1_RXD()  HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6)
 
#define SUartLength        200            //模拟串口缓冲区长度
 
enum{
    COM_START_BIT,
    COM_D0_BIT,
    COM_D1_BIT,
    COM_D2_BIT,
    COM_D3_BIT,
    COM_D4_BIT,
    COM_D5_BIT,
    COM_D6_BIT,
    COM_D7_BIT,
    COM_STOP_BIT,
};
 
void iouart1_delayUs(volatile uint32_t nTime);
void iouart1_delayUs_104(void);
void S_Uart_Send_Buff(uint8_t *buff,uint8_t length);
void S_Uart_Send_Str(uint8_t *str);
void S_Uart_One_Tx(uint8_t Data);
uint8_t S_Uart_Rx_Handler(uint8_t *buf,uint8_t *length);
 
 
#endif /* __LED_H */