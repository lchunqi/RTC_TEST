#ifndef __COMMAND_H
#define __COMMAND_H

#ifdef __cplusplus
 extern "C" {
#endif


void CheckCommandList(void);
void DeviceMassage(void);
void control_usart_receive(void);
void ControlInit(void);

#ifdef __cplusplus
}
#endif

#endif 