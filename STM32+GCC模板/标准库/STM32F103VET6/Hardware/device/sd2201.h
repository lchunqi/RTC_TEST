#ifndef __SD2201_H
#define __SD2201_H

#include "iic.h"
#include "rtc.h"
#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>



uint8_t SD2201_ReadTimeDate(i2c_device_bus bus);
void Display_2200Time(void);





















#endif
