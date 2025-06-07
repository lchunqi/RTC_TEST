/*
 * @Author: Liang 
 * @Date: 2025-03-06 15:19:01
 * @LastEditors: HA HA 
 * @LastEditTime: 2025-06-06 12:07:46
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved.
 */
#ifndef __COMMAND_H__
#define __COMMAND_H__
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "soft_i2c.h"
#include "esp8266.h"

typedef struct 
{
  uint8_t year;
  uint8_t mon;
  uint8_t day;
  uint8_t week;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint16_t tick;
}RealTime_S;

extern uint8_t  usbRxBuf;
extern uint8_t  deviceRxBuf;

extern uint8_t  wifiRxBuf; 


void UsbPrintf(char* fmt,...);
void show_wifi_Message(void);

void CheckCommandList(void);
void DeviceMassage(void);





#ifdef __cplusplus
}
#endif

#endif 
