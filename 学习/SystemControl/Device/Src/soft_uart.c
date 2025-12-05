#include "soft_uart.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define DEVICE1_IO_RX       GPIO_PIN_6
#define DEVICE1_IO_TX       GPIO_PIN_6
#define DEVICE1_TX_H        DEVICE1_IO_TX        
#define DEVICE1_TX_L        (uint32_t)DEVICE1_IO_TX << 16U  

#define UART_DATA_LENGTH    9//8bit data, 1bit stop, none parity
typedef struct {
    uint8_t rxState;
    uint8_t rxCount;
    uint8_t rxBit;
    uint8_t rxData;
    uint8_t txState;
    uint8_t txCount;
    uint8_t txBit;
    uint8_t txData;
    uint8_t txBuff;
}UartSoftTimer;

SoftUart uart_dev[MAX_DEVICES_CHANNEL] = {0};

void devices_printf(SoftUart* soft_uart, char* fmt,...)  
{  
    uint8_t quick_buffer[SOFT_UART_TRANSMIT_SHORT] = {0};
    int i;
    uint8_t length;
	va_list ap;//
    
	va_start(ap, fmt);//
	vsnprintf((char*)quick_buffer, SOFT_UART_TRANSMIT_SHORT, fmt, ap);//
	va_end(ap);
    length = strlen((char*)quick_buffer);
    for (i = 0; i < length; i++) {
        soft_uart->tx_buffer[soft_uart->tx_write] = quick_buffer[i];	//装发送缓冲
        if (++(soft_uart->tx_write) >= SOFT_UART_TRANSMIT_LONG) {
            soft_uart->tx_write = 0;
        }	
    }
}

void devices_receive_byte(SoftUart* soft_uart, uint8_t rxBuff)
{
    if (soft_uart->rx_length == 0 && (rxBuff == ' ' || rxBuff == '\r' ||rxBuff == '\n')) {
        return;
    }
    if (soft_uart->rx_length < SOFT_UART_RECEICE_LONG) {
        soft_uart->rx_buffer[soft_uart->rx_length++] = rxBuff;
        if (soft_uart->rx_length >= SOFT_UART_RECEICE_LONG || rxBuff == '\n') {
            soft_uart->rx_flag = 1;
        }
    }
}


void uart_bus_time(void)
{
    int i;
    static UartSoftTimer uartTimer[MAX_DEVICES_CHANNEL] = {0};
    uint16_t input[MAX_DEVICES_CHANNEL] = {0};
    for (i = 0; i < MAX_DEVICES_CHANNEL; i++) {
        input[i] = GPIOD->IDR & (DEVICE1_IO_RX << i);
    }
    for (i = 0; i < MAX_DEVICES_CHANNEL; i++) {
        if (uartTimer[i].rxState) {
            if (--(uartTimer[i].rxCount) == 0) {			  //接收数据以定时器的1/3来接收
                uartTimer[i].rxCount = MULTIPLE_OF_TIEMR;     //重置接收计数器  接收数据以定时器的1/3来接收
                if (--(uartTimer[i].rxBit) == 0) {			  //接收完一帧数据
                    uartTimer[i].rxState = 0;               //停止接收			
                    devices_receive_byte(&uart_dev[i], uartTimer[i].rxData);    //接收字符处理
                } else {
                    uartTimer[i].rxData >>= 1;			  //把接收的单bit数据 暂存到 rxData(接收缓冲)
                    if (input[i]) {
                        uartTimer[i].rxData |= 0x80;  
                    }
                }
            }
        } else if (input[i] == 0) {	//判断是不是开始位 RXB=0;
            uartTimer[i].rxState = 1;       //如果是则设置开始接收标志位 	
            uartTimer[i].rxCount = MULTIPLE_OF_TIEMR + 1;  //错开1/3周期接收，很关键，增强容错能力      	
            uartTimer[i].rxBit = UART_DATA_LENGTH;       //初始化接收的数据位数(8个数据位+1个停止位)   
            uartTimer[i].rxData = 0;
        }
    }
        //---------------------接收与发送分隔线-------------------------------------
    for (i = 0; i < MAX_DEVICES_CHANNEL; i++) {
        if (uartTimer[i].txState == 0 && uart_dev[i].tx_read != uart_dev[i].tx_write)//循环队列
        {
            uartTimer[i].txBuff = uart_dev[i].tx_buffer[uart_dev[i].tx_read];
            if (++(uart_dev[i].tx_read) >= SOFT_UART_TRANSMIT_LONG)	{
                uart_dev[i].tx_read = 0;
            }
            uartTimer[i].txState = 1;
        }
        if (uartTimer[i].txState != 0)	{		        //发送开始标志位 
            if (--(uartTimer[i].txCount) <= 0) {			//发送数据以定时器的1/3来发送
                uartTimer[i].txCount = MULTIPLE_OF_TIEMR;				//重置发送计数器 
                if (uartTimer[i].txBit == 0) {		    //发送计数器为0 表明单字节发送还没开始
                    GPIOE->BSRR = (DEVICE1_TX_L << i);	//发送开始位     				
                    uartTimer[i].txData = uartTimer[i].txBuff;		//把缓冲的数据放到发送的buff		
                    uartTimer[i].txBit = UART_DATA_LENGTH;	//发送数据位数 (8数据位+1停止位)	
                } else	{				                //发送计数器为非0 正在发送数据
                    if (--(uartTimer[i].txBit) == 0) {    //发送计数器减为0 表明单字节发送结束
                        GPIOE->BSRR = (DEVICE1_TX_H << i);		//送停止位数据
                        uartTimer[i].txState = 0;		//发送停止位
                    }	else {                          //先发送低位bit，再发送高位bit
                        if (uartTimer[i].txData & 0x01) {//发送单bit数据	
                            GPIOE->BSRR = (DEVICE1_TX_H << i);
                        } else {
                            GPIOE->BSRR = (DEVICE1_TX_L << i);
                        }			
                        uartTimer[i].txData >>= 1;
                    }
                }
            }
        }
    }
}