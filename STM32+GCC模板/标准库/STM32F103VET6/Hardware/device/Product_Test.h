#ifndef __PRODUCT_TEST_H
#define __PRODUCT_TEST_H

#include "usart.h"
#include "spi.h"
#include "rtc.h"
#include "Delay.h"
#include "stm32f10x.h"
#include <stdint.h>

spi_exit_code SD8840_PrintfTime(void);
void SD8840_SecCountDown(void);




#endif
