
#ifndef __COMMAND_H__
#define __COMMAND_H__
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

// 缓冲区大小（必须与工程一致）
#define UART_RECEIVE_BUFF_NUMBER    256
#define UART_TRANSMIT_BUFF_NUMBER   256
#define USB_UART_TICK_CLEAR         100

extern uint8_t g_UsbTxBuff[];
extern uint8_t usbRxBuf;
extern uint8_t deviceRxBuf;

int  strSearch(const char* source, const char* target);
uint16_t CharToInt(char* source, uint8_t start, int* number);
uint8_t  CharToFloat(char* source, uint8_t start, float* number);
uint8_t  Data_copy_from_pos(char *dest, const char *src, uint16_t start_pos, uint16_t copy_len);
void AT_HELP_Handle(char* command);
void AT_TEST_Handle(char* command);
void AT_FATFS_MOUNT_Handle(char* command);
void AT_FATFS_FORMAT_Handle(char* command);
void AT_FATFS_WRITE_Handle(char* command);
void AT_FATFS_READ_Handle(char* command);
void CheckCommandList(void);


#ifdef __cplusplus
}
#endif

#endif 
