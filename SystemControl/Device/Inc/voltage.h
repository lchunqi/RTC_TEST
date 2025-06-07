#ifndef __VOLTAGE_H
#define __VOLTAGE_H

#ifdef __cplusplus
 extern "C" {
#endif
#include "main.h"

#define CHANNEL_12V     ADC_CHANNEL_0
#define CHANNEL_6V5     ADC_CHANNEL_8

uint32_t ststem_voltage(uint32_t channel);
#ifdef __cplusplus
}
#endif

#endif 