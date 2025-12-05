#ifndef __USART_H
#define __USART_H


#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "stm32f10x.h"


extern char Usart_RxPacket[];
extern uint8_t Usart_RxFlag;

void Usart_Init(void);
void Usart_SendOneByte(USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendByte(uint8_t Byte);
void Usart_SendArray(uint8_t *Array, uint16_t Length);
void Usart_SendString(char *String);
void Usart_SendNumber(uint32_t Number, uint8_t Length);
void Usart_Printf(char *format, ...);

uint8_t Get_BusFlag(void);
void Get_UsartBuffer(uint8_t *p_dat);

#endif
