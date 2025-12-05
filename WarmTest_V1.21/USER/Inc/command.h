#ifndef __COMMAND_H__
#define __COMMAND_H__
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"



extern uint8_t usbRxBuf;
extern uint8_t deviceRxBuf;
void CheckCommandList(void);
void DeviceMassage(void);

void Command_analyse(uint8_t* Command_buf);



#ifdef __cplusplus
}
#endif

#endif 
