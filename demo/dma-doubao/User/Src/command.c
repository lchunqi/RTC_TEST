#include "command.h"
#include "usart.h"
#include "main.h"
#include <stdint.h>
#include "flash.h"
#include "../FatFs/Inc/diskio.h"
#include "../FatFs/Inc/ff.h"


#define UART_RECEIVE_BUFF_NUMBER    200
#define UART_TRANSMIT_BUFF_NUMBER   300
#define USB_UART_TICK_CLEAR         100

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
uint16_t usb_Rx_Cnt = 0;		
uint8_t usbRxBuf;
uint8_t g_UsbRxBuff[UART_RECEIVE_BUFF_NUMBER] = {0};
uint8_t g_UsbATStatus = 0;

uint16_t device_Rx_Cnt = 0;		
uint8_t deviceRxBuf;
uint8_t g_DeviceRxBuff[UART_RECEIVE_BUFF_NUMBER] = {0};
uint8_t g_DeviceTxBuff[UART_TRANSMIT_BUFF_NUMBER] = {0};
uint8_t g_DeviceATStatus = 0;


char Command_buf[200] = {0};

#define MAX_COMMAND_NUMBER  40
#define MAX_COMMAND_LENGTH  20
static char CommandList[MAX_COMMAND_NUMBER][MAX_COMMAND_LENGTH] = {
	"AT^HELP",
	"AT^TEST",
	"AT^FATFS_MOUNT",
    "AT^FATFS_WRITE",
    "AT^FATFS_READ",
	
};


void DevicePrintf(char* fmt,...)  
{  
	va_list ap;
	memset(g_DeviceTxBuff, 0, UART_TRANSMIT_BUFF_NUMBER);
	va_start(ap, fmt);
	vsnprintf((char*)g_DeviceTxBuff, UART_TRANSMIT_BUFF_NUMBER, fmt, ap);
	va_end(ap);
	HAL_UART_Transmit(&huart2, g_DeviceTxBuff, strlen((char*)g_DeviceTxBuff), 1000);
}

int strSearch(char* source, char* target)
{
	int len_source = strlen(source);
	int len_target = strlen(target);
	int i, j;
	if (len_source < len_target) {
		return -1;
	}		
	for (i = 0; i <= len_source - len_target; i++) {
		for (j = 0; j < len_target; j++) {
			if (source[i + j] != target[j]) { break;}
		}
		if (j == len_target) { return i;}
	}
	return -1;	
}

uint16_t CharToInt(char* source, uint8_t start, int* number)
{
	uint16_t length;
	uint16_t i;
	int sum = 0;
	int state = 0;
	uint8_t hex = 0;
	length = strlen(source);
	if (start >= length) {
		*number = 0;
		return 0;
	}
	if (source[start] == '-') {
		state = -1;
		start++;
	} else {
		state = 1;
		if (start + 1 < length ) {
			if (source[start] == '0' && (source[start + 1] == 'x' ||source[start + 1] == 'X')) {
				start += 2;
				hex = 1;
			}
		}
	}
	for (i = start; i < length; i++) {
		if (source[i] >= '0' && source[i] <= '9') {
			if (hex == 1) {
				sum = sum * 16 + source[i] - '0';
			} else {
				sum = sum * 10 + source[i] - '0';
			}
		} else if (source[i] >= 'a' && source[i] <= 'f') {
			if (hex == 1) {
				sum = sum * 16  + source[i] - 'a' + 10;
			} else {
				*number = 0;
				return 0;
			}
		}else if (source[i] >= 'A' && source[i] <= 'F') {
			if (hex == 1) {
				sum = sum * 16  + source[i] - 'A' + 10;
			} else {
				*number = 0;
				return 0;
			}
		} else if (source[i] == ' ' || source[i] == ',') {
			break;
		}
	}
	*number = state * sum;
	return 1;
}

uint8_t CharToFloat(char* source, uint8_t start, float* number)
{
		uint8_t length;
		uint8_t i;
		float sum = 0;
		int state = 1;
		float smallNumber = 0.1;
		length = strlen(source);
		if (length <= start) {
				*number = 0;
				return 0;
		}
		
		if (source[start] == '-') {
				state = -1;
				start++;
		}
		for (i = start; i < length; i++){
				if (source[i] >= '0' && source[i] <= '9') {
						sum = sum * 10 + source[i] - '0';
				} else if (source[i] == '.') {
						i++;
						break;
				} else if (source[i] == ' ' || source[i] == ',') {
						break;
				}	else {
						*number = 0;
						return 0;
				}					
		}
		for (; i < length; i++) {
				if (source[i] >= '0' && source[i] <= '9') {
						sum += smallNumber * (source[i] - '0');
						smallNumber = smallNumber * 0.1;
				} else if (source[i] == ' ' || source[i] == ',') {
						break;
				}	else {
						*number = 0;
						return 0;
				}	
		}
		*number = state * sum;
		return 1;
}

/**
 * @brief 自定义字符串复制函数：从源字符串指定位置复制指定长度到目标字符串
 * @param dest 目标字符串（存放复制结果）
 * @param src 源字符串
 * @param start_pos 源字符串的起始复制位置（从0开始计数）
 * @param copy_len 需要复制的长度
 * @return int 成功返回1，失败返回0（参数非法）
 */
uint8_t Data_copy_from_pos(char *dest, const char *src, uint16_t start_pos, uint16_t copy_len) {
    // 1. 安全校验：空指针检查
    if (dest == NULL || src == NULL) {
        UsbPrintf("error pointer null !\n");
        return 0;
    }

    // 2. 计算源字符串长度，校验起始位置是否合法
    int src_len = strlen(src);
    if (start_pos < 0 || start_pos >= src_len) {
        UsbPrintf("error start len ng ! src_len:%d,start:%d\n", src_len, start_pos);
        return 0;
    }

    // 3. 校验复制长度：长度不能为负，且不能超出源字符串剩余长度
    if (copy_len < 0 || (start_pos + copy_len) > src_len) {
        UsbPrintf("error copy_len ! left_len:%d,copy_len:%d\n", src_len - start_pos, copy_len);
        return 0;
    }

    // 4. 核心复制逻辑：从start_pos开始，复制copy_len个字符
    for (int i = 0; i < copy_len; i++) {
        dest[i] = src[start_pos + i];
    }

    // 5. 手动添加字符串结束符（必须！否则目标字符串会乱码）
    dest[copy_len] = '\0';

    return 1;
}


void  AT_HELP_Handle(char* command)
{
    int i = 0, number = 0;
    command = NULL;
    for (i = 0; i < MAX_COMMAND_NUMBER; i++) {
		if (strlen(CommandList[i]) > 0) {
            if(i==0)    { UsbPrintf("\n"); }
        	UsbPrintf("%d. %s\n", number, CommandList[i]);
			number++;
		}
    }
	UsbPrintf("command number: %d\n", number);
}

void  AT_TEST_Handle(char* command)
{

	//uint8_t test_buf[4096] = {0};
	fatfs_read_file("test.txt");
}

void  AT_FATFS_MOUNT_Handle(char* command)
{
    Fatfs_Mount();  
}

void  AT_FATFS_WRITE_Handle(char* command)
{
    Fatfs_Write(command);
	
}
void AT_FATFS_READ_Handle(char* command)
{
    int i=0;
    CharToInt(command, 13, &i);
	
    Fatfs_Read(i);
}

typedef void(*function)(char* s);
function UsbCommand[MAX_COMMAND_NUMBER] = {
    AT_HELP_Handle,
    AT_TEST_Handle,
	AT_FATFS_MOUNT_Handle,
    AT_FATFS_WRITE_Handle,
    AT_FATFS_READ_Handle,
    
};


//DMA处理命令
void CheckCommandList(void)
{
    uint8_t i;
    static uint16_t usbTick = 0;
    static uint16_t lastUsbRx = 0;
    if (g_usart1_rx_done == 0 && g_usart1_rx_len == 0) {
        usbTick = 0;
        lastUsbRx = 0;
        return;
    }
    if (g_usart1_rx_len > lastUsbRx) {
        lastUsbRx = g_usart1_rx_len;
        usbTick = 0;
    } else {
        usbTick++;
    }
    if (usbTick >= USB_UART_TICK_CLEAR || g_usart1_rx_done == 1) {
        for (i = 0; i < MAX_COMMAND_NUMBER; i++) {
            if (strlen(CommandList[i]) > 0 && strSearch((char*)g_usart1_dma_rx_buff, CommandList[i]) == 0) {
                UsbCommand[i]((char*)g_usart1_dma_rx_buff);
                break;
            }
        }				
        if (i == MAX_COMMAND_NUMBER) {
           UsbPrintf("\nNo this commmand !\n");
        }
        memset(g_usart1_dma_rx_buff, 0, UART_RECEIVE_BUFF_NUMBER);
        g_usart1_rx_len = 0;
        g_usart1_rx_done = 0;
        usbTick = 0;
        lastUsbRx = 0;
    }
}

void DeviceMassage(void)
{

    static uint16_t deviceTick = 0;
    static uint16_t lastDeviceRx = 0;
    if (g_DeviceATStatus == 0 && device_Rx_Cnt == 0) {
        deviceTick = 0;
        lastDeviceRx = 0;
        return;
    }
    if (device_Rx_Cnt > lastDeviceRx) {
        lastDeviceRx = device_Rx_Cnt;
        deviceTick = 0;
    } else {
        deviceTick++;
    }

	if (deviceTick >= USB_UART_TICK_CLEAR || g_DeviceATStatus == 1) {

		if(g_DeviceRxBuff[0] == 0xFA && g_DeviceRxBuff[1] == 0xFB && g_DeviceRxBuff[device_Rx_Cnt]=='\0' )
		{
			//需要处理的代码命令  比如:Command_analyse(g_DeviceRxBuff);
		}
        memset(g_DeviceRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
        device_Rx_Cnt = 0;
        g_DeviceATStatus = 0;
        deviceTick = 0;
        lastDeviceRx = 0;
	 }
}







