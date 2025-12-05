#ifndef __MPRINTF_H
#define __MPRINTF_H


#include "usart.h"
#include "rtc.h"
#include "otp.h"
#include <stdio.h>
#include <stdint.h>

void MPrint_Help(void);
void MPrint_Version(void);
rtc_exit_code MPrint_time(i2c_device_bus IIC_x);
void MPrint_ctr(void);
void MPrint_ram(void);
void MPrint_testmodel(void);
void MPrint_id(void);
void MPrint_retain(void);
void MPrint_config(void);
void MPrint_wenbu(void);
void MPrint_capin(void);
void MPrint_capout(void);
void MPrint_configx(void);
void MPrint_wenbuotp(void);
void MPrint_capinotp(void);
void MPrint_capoutotp(void);



void MPrint_TimeDate(uint8_t *p_dat);

void MPrint_CmdFail(void);
void MPrint_AckFail(void);




#endif
