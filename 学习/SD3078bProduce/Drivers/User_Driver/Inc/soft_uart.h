#ifndef __SOFT_UART_H
#define __SOFT_UART_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

#define SOFT_UART_BAUDRATE          9600
#define SOFT_UART_TRANSMIT_SHORT    64
#define SOFT_UART_TRANSMIT_LONG     300
#define SOFT_UART_RECEICE_LONG      200
#define SOFT_UART_TICK_CLEAR        10
#define MAX_DEVICES_CHANNEL         2
#define MULTIPLE_OF_TIEMR           3
typedef struct {
    uint16_t tx_read;
    uint16_t tx_write;
    uint16_t rx_length;
    uint8_t rx_flag;
    uint8_t tx_buffer[SOFT_UART_TRANSMIT_LONG];
    uint8_t rx_buffer[SOFT_UART_RECEICE_LONG];
} SoftUart;

extern SoftUart uart_dev[MAX_DEVICES_CHANNEL];
void uart_bus_time(void);
void devices_printf(SoftUart* soft_uart, char* fmt,...);
#ifdef __cplusplus
}
#endif

#endif 