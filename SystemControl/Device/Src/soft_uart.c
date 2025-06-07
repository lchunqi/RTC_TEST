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
        soft_uart->tx_buffer[soft_uart->tx_write] = quick_buffer[i];	//װ���ͻ���
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
            if (--(uartTimer[i].rxCount) == 0) {			  //���������Զ�ʱ����1/3������
                uartTimer[i].rxCount = MULTIPLE_OF_TIEMR;     //���ý��ռ�����  ���������Զ�ʱ����1/3������
                if (--(uartTimer[i].rxBit) == 0) {			  //������һ֡����
                    uartTimer[i].rxState = 0;               //ֹͣ����			
                    devices_receive_byte(&uart_dev[i], uartTimer[i].rxData);    //�����ַ�����
                } else {
                    uartTimer[i].rxData >>= 1;			  //�ѽ��յĵ�bit���� �ݴ浽 rxData(���ջ���)
                    if (input[i]) {
                        uartTimer[i].rxData |= 0x80;  
                    }
                }
            }
        } else if (input[i] == 0) {	//�ж��ǲ��ǿ�ʼλ RXB=0;
            uartTimer[i].rxState = 1;       //����������ÿ�ʼ���ձ�־λ 	
            uartTimer[i].rxCount = MULTIPLE_OF_TIEMR + 1;  //��1/3���ڽ��գ��ܹؼ�����ǿ�ݴ�����      	
            uartTimer[i].rxBit = UART_DATA_LENGTH;       //��ʼ�����յ�����λ��(8������λ+1��ֹͣλ)   
            uartTimer[i].rxData = 0;
        }
    }
        //---------------------�����뷢�ͷָ���-------------------------------------
    for (i = 0; i < MAX_DEVICES_CHANNEL; i++) {
        if (uartTimer[i].txState == 0 && uart_dev[i].tx_read != uart_dev[i].tx_write)//ѭ������
        {
            uartTimer[i].txBuff = uart_dev[i].tx_buffer[uart_dev[i].tx_read];
            if (++(uart_dev[i].tx_read) >= SOFT_UART_TRANSMIT_LONG)	{
                uart_dev[i].tx_read = 0;
            }
            uartTimer[i].txState = 1;
        }
        if (uartTimer[i].txState != 0)	{		        //���Ϳ�ʼ��־λ 
            if (--(uartTimer[i].txCount) <= 0) {			//���������Զ�ʱ����1/3������
                uartTimer[i].txCount = MULTIPLE_OF_TIEMR;				//���÷��ͼ����� 
                if (uartTimer[i].txBit == 0) {		    //���ͼ�����Ϊ0 �������ֽڷ��ͻ�û��ʼ
                    GPIOE->BSRR = (DEVICE1_TX_L << i);	//���Ϳ�ʼλ     				
                    uartTimer[i].txData = uartTimer[i].txBuff;		//�ѻ�������ݷŵ����͵�buff		
                    uartTimer[i].txBit = UART_DATA_LENGTH;	//��������λ�� (8����λ+1ֹͣλ)	
                } else	{				                //���ͼ�����Ϊ��0 ���ڷ�������
                    if (--(uartTimer[i].txBit) == 0) {    //���ͼ�������Ϊ0 �������ֽڷ��ͽ���
                        GPIOE->BSRR = (DEVICE1_TX_H << i);		//��ֹͣλ����
                        uartTimer[i].txState = 0;		//����ֹͣλ
                    }	else {                          //�ȷ��͵�λbit���ٷ��͸�λbit
                        if (uartTimer[i].txData & 0x01) {//���͵�bit����	
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