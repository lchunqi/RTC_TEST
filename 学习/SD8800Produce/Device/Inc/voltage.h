#ifndef __VOLTAGE_H
#define __VOLTAGE_H

#ifdef __cplusplus
 extern "C" {
#endif
#include "main.h"

#define CHANNEL_3V3     ADC_CHANNEL_0
#define CHANNEL_VCE     ADC_CHANNEL_1
#define CHANNEL_5V      ADC_CHANNEL_8
#define CHANNEL_9V      ADC_CHANNEL_9

uint32_t ststem_voltage(uint32_t channel);
#ifdef __cplusplus
}
#endif

#endif 