#ifndef __CONTROL_H
#define __CONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif




void ControlPrintf(char* fmt,...);
void CheckControlList(void);
void control_usart_init(void);

#ifdef __cplusplus
}
#endif

#endif 