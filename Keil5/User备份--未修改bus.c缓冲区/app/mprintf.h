#ifndef __MPRINTF_H
#define __MPRINTF_H


#include "usart.h"
#include <stdio.h>
#include <stdint.h>

void MPrint_Help(void);
void MPrint_Version(void);

void MPrint_TimeDate(uint8_t *p_dat);

void MPrint_CmdFail(void);
void MPrint_AckFail(void);




#endif
