#ifndef __USER_TOOL_H
#define __USER_TOOL_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
int strSearch(char* source, char* target);
uint8_t CharToInt(char* source, uint8_t start, int* number);
uint8_t CharToFloat(char* source, uint8_t start, float* number);

#ifdef __cplusplus
}
#endif

#endif 