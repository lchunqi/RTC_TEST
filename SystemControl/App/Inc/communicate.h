#ifndef __COMMUNICATE_H
#define __COMMUNICATE_H

#ifdef __cplusplus
 extern "C" {
#endif
#include "main.h"

typedef struct {
    uint8_t state;
    uint16_t time;
    uint8_t falling;
} KeyState;

#define SYSTEM_KEY_NUMBER           4
#define COMMUNICATE_TASK_DELAY      5//ms
#define KEY_SELECT_SHORT            70//ms
#define KEY_SELECT_LONG             500//ms
#define RTC_TIME_SIZE              7           

typedef enum {
    KEY_UP = 0,
    KEY_DOWN,
    KEY_SELECT,
    KEY_ENTER
}KeyNameList;

void StartCommunicateTask(void const * argument);
void control_usart_receive(void);//接收中断回调函数
#ifdef __cplusplus
}
#endif

#endif 