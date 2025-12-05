#ifndef __MANAGER_H__
#define __MANAGER_H__
#ifdef __cplusplus
 extern "C" {
#endif


#include "main.h"   


void Manager_Init(void);
void Set_Port_Number(uint8_t port);
void Enable_IIC4051(uint8_t i,uint8_t j);
void Enable_SPI4051(uint8_t i,uint8_t j);

uint8_t INT_Choose (char* command);
void Control_level(uint8_t i ,uint8_t level);



#ifdef __cplusplus
}
#endif
 
#endif 




