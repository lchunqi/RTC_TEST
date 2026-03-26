
#include "command.h"

#define UART_RECEIVE_BUFF_NUMBER    200
#define UART_TRANSMIT_BUFF_NUMBER   300
#define USB_UART_TICK_CLEAR         100

// extern UART_HandleTypeDef huart1;
// extern UART_HandleTypeDef huart2;

uint16_t usb_Rx_Cnt = 0;		
uint8_t usbRxBuf;
uint8_t g_UsbRxBuff[UART_RECEIVE_BUFF_NUMBER] = {0};
uint8_t g_UsbTxBuff[UART_TRANSMIT_BUFF_NUMBER] = {0};
uint8_t g_UsbATStatus = 0;

uint16_t device_Rx_Cnt = 0;		
uint8_t deviceRxBuf;
uint8_t g_DeviceRxBuff[UART_RECEIVE_BUFF_NUMBER] = {0};
uint8_t g_DeviceTxBuff[UART_TRANSMIT_BUFF_NUMBER] = {0};
uint8_t g_DeviceATStatus = 0;


#define RXBUFFERSIZE  1024         //最大接收字节数


uint8_t  wifiRxBuf;                //接收中断缓冲
uint32_t Uart3_Rx_Cnt = 0;         //接收缓冲计数
uint8_t  RxBuffer3[RXBUFFERSIZE];      //接收数据


#define MAX_COMMAND_NUMBER  40
#define MAX_COMMAND_LENGTH  20
static char CommandList[MAX_COMMAND_NUMBER][MAX_COMMAND_LENGTH] = {
	"AT^HELP",
	"AT^TEST",
	"AT^READDATA",
	"AT^IICINT",
	"AT^SETTIME",
	"AT^WIFICMD",
	"AT^SD8932",
};

void UsbPrintf(char* fmt,...)  
{  
	va_list ap;
	memset(g_UsbTxBuff, 0, UART_TRANSMIT_BUFF_NUMBER);
	va_start(ap, fmt);
	vsnprintf((char*)g_UsbTxBuff, UART_TRANSMIT_BUFF_NUMBER, fmt, ap);
	va_end(ap);
	HAL_UART_Transmit(&huart1, g_UsbTxBuff, strlen((char*)g_UsbTxBuff), 1000);
}

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

uint8_t CharToInt(char* source, uint8_t start, int* number)
{
	uint8_t length;
	uint8_t i;
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

int CopyPointString(char* dest, char* src, int start, int length) {
    int srcLength = strlen(src);
    if (start >= srcLength)
        {return -1;}
	
    int i;
    for (i = 0; i < length && (start + i) < srcLength; i++) {
        dest[i] = src[start + i];
    }
    dest[i] = '\0';
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

void AT_TEST_Handle(char* command)
{
	// wifi_send_to("AT\r\n");
	// HAL_Delay(1000);
    // show_wifi_Message();

	// char cmd0[]="AT+CWMODE=3\r\n";
	// wifi_send_to(cmd0);
	// HAL_Delay(1000);
	// show_wifi_Message();

	// wifi_send_to("AT+RST\r\n");
	// HAL_Delay(3000);
    // show_wifi_Message();

  	// char cmd1[]="AT+CWLAP\r\n";
	// wifi_send_to(cmd1);
	// HAL_Delay(3000);
	// show_wifi_Message();

	// char cmd2[]="AT+CWJAP=\"X\",\"123456789\"";
	// wifi_send_to(cmd2);
    // HAL_Delay(3000);
    // show_wifi_Message();


	// char cmd3[]="AT+CIFSR\r\n";
	// wifi_send_to(cmd3);
	// HAL_Delay(1000);
	// show_wifi_Message();

	char cmd3[]="AT+CIPSTART=\"TCP\",\"192.168.137.1\",8080\r\n";
	wifi_send_to(cmd3);
	HAL_Delay(1000);
	show_wifi_Message();


}

void  AT_READDATA_Handle(char* command)
{
	uint8_t buffer[7]={0};
	int num = 0,reg = 0;
	num = strSearch(command, "reg:");
	if (num != -1) {
		num += strlen("reg:");
		if (CharToInt(command, num, &reg) == 0 ) {	return; }
	}

	if(num == -1){
		UsbPrintf("reg error !\n");
	}
	else{
		switch (reg)
		{
		case 0X00:
			if (i2c_read_multi_bytes(0x64, reg, buffer, 7) == I2C_OK) {
				UsbPrintf("%02X年 %02X月 %02X日 星期%02X %02X:%02X:%02X\n", buffer[6], buffer[5], buffer[4], buffer[3], buffer[2], buffer[1], buffer[0]);
			}
			else {
				uint8_t flag=6;
				flag=i2c_read_multi_bytes(0x64, reg, buffer, 8);
				UsbPrintf("read error flag=%d !\n",flag);
			}
			break;
		case 0XB2:
			if (i2c_read_multi_bytes(0x64, reg, buffer, 8) == I2C_OK) {
				UsbPrintf("id:%02X%02X%02X%02X%02X%02X%02X%02X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
			}else if(i2c_read_multi_bytes(0xD0, reg, buffer, 8) == I2C_OK){
				UsbPrintf("id:%02X%02X%02X%02X%02X%02X%02X%02X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
			}
			break;

		default:
			UsbPrintf("reg error !\n");
			break;
		}
	}
}

void AT_IICINT_Handle(char* command)
{
	uint8_t buf[8]={0} ,state=9;
	int num = 0, reg = 0;
	num = strSearch(command, "reg:");
	if (num != -1) {
		num += strlen("reg:");
		if (CharToInt(command, num, &reg) == 0 ) {	return; }
	}
	state=i2c_read_multi_bytes(0x64, reg, buf, 7);
	UsbPrintf("state:%d\n", state);
	if (reg==0X00) {
		i2c_read_multi_bytes(0x64, reg, buf, 7);
		UsbPrintf("%02X年 %02X月 %02X日 星期%02X %02X:%02X:%02X\n", buf[6], buf[5], buf[4], buf[3], buf[2], buf[1], buf[0]);
	}else{
		i2c_read_multi_bytes(0x64, reg, buf, 8);
		UsbPrintf("%02X %02X %02X %02X %02X %02X %02X %02X \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
	}
}

void  AT_SETTIME_Handle(char* command)
{
	RealTime_S rtc = {0};
	int num = 0;
	int inNumber = 0;

	num = strSearch(command, "year:");
	if (num != -1) {
		num += strlen("year:");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		rtc.year = inNumber % 100;
	}
	num = strSearch(command, "mon:");
	if (num != -1) {
		num += strlen("mon:");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		rtc.mon = inNumber % 100;
	}	
	num = strSearch(command, "day:");
	if (num != -1) {
		num += strlen("day:");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		rtc.day = inNumber % 100;
	}	
	num = strSearch(command, "hour:");
	if (num != -1) {
		num += strlen("hour:");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		rtc.hour = inNumber % 100;
	}	
	num = strSearch(command, "min:");
	if (num != -1) {
		num += strlen("min:");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		rtc.min = inNumber % 100;
	}	
	num = strSearch(command, "sec:");
	if (num != -1) {
		num += strlen("sec:");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		rtc.sec = inNumber % 100;
	}

	uint8_t data[7]={rtc.sec, rtc.min, rtc.hour, rtc.week, rtc.day, rtc.mon, rtc.year};


	if (i2c_write_multi_bytes(0x64, 0x00, data, 7) == I2C_OK) {
		UsbPrintf("date set success!\r\n");

	}else {
		UsbPrintf("date set error!\nOK!\r\n");
	}

}

void  AT_WIFICMD_Handle(char* command)
{
	// int num = 0,reg = 0;
	// num = strSearch(command, "=");
	// if (num != -1) {
	// 	num += strlen("=");
	// }
	char WifiBuf[64]={0};
	//int flag = 0;
	//flag=CopyPointString(WifiBuf,command,11,(strlen(command)-11));
	//UsbPrintf("wificmd=%s\n",WifiBuf);
	//UsbPrintf("flag=%d\n",flag);

	CopyPointString(WifiBuf,command,11,(strlen(command)-11));
	wifi_send_to(WifiBuf);
	


}

void  AT_SD8932_Handle(char* command)
{
	
}



typedef void(*function)(char* s);
function UsbCommand[MAX_COMMAND_NUMBER] = {
    AT_HELP_Handle,
	AT_TEST_Handle,
    AT_READDATA_Handle,
    AT_IICINT_Handle,
	AT_SETTIME_Handle,
	AT_WIFICMD_Handle,
	AT_SD8932_Handle,
};




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        if (usb_Rx_Cnt < UART_RECEIVE_BUFF_NUMBER && g_UsbATStatus == 0) {
            g_UsbRxBuff[usb_Rx_Cnt] = usbRxBuf;
        	usb_Rx_Cnt++;
        }
        if (usb_Rx_Cnt >= UART_RECEIVE_BUFF_NUMBER || usbRxBuf == ' ') {
			g_UsbATStatus = 1;
        } 
        HAL_UART_Receive_IT(&huart1, &usbRxBuf, 1);
    }

	if (huart->Instance == USART2) {
		if (device_Rx_Cnt == 0 && (deviceRxBuf == ' ' || deviceRxBuf == '\r' || deviceRxBuf == '\n')) {
			HAL_UART_Receive_IT(&huart2, &deviceRxBuf, 1);
			return;
		}
		if (device_Rx_Cnt < UART_RECEIVE_BUFF_NUMBER && g_DeviceATStatus == 0) {
			g_DeviceRxBuff[device_Rx_Cnt] = deviceRxBuf;
			device_Rx_Cnt++;
		}
		if (device_Rx_Cnt >= UART_RECEIVE_BUFF_NUMBER ||  deviceRxBuf == '\0') {
			g_DeviceATStatus = 1;
		}    
		HAL_UART_Receive_IT(&huart2, &deviceRxBuf, 1);
	}

	if(huart->Instance==USART3){
        if(Uart3_Rx_Cnt >= (RXBUFFERSIZE-1)){  //溢出判断
            Uart3_Rx_Cnt = 0;
            memset(RxBuffer3,0x00,sizeof(RxBuffer3));
            HAL_UART_Transmit(&huart1, (uint8_t *)"WIFI BUF ERROR !", 100,0xFFFF);     
        }else{
            RxBuffer3[Uart3_Rx_Cnt++] = wifiRxBuf;   //接收数据转存
            
			/*
            if(((RxBuffer3[Uart3_Rx_Cnt-1] == 0x0A)&&(RxBuffer3[Uart3_Rx_Cnt-2] == 0x0D))){ //判断结束位
                HAL_UART_Transmit(&huart1, (uint8_t *)&RxBuffer3, Uart3_Rx_Cnt,0xFFFF); //将收到的信息发送出去
                //while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);//检测UART发送结束
                Uart3_Rx_Cnt = 0;
                memset(RxBuffer3,0x00,sizeof(RxBuffer3)); //清空数组
            }
            */
        }
		
        HAL_UART_Receive_IT(&huart3, (uint8_t *)&wifiRxBuf, 1);   //再开启接收中断
    }

}

void show_wifi_Message(void)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&RxBuffer3, Uart3_Rx_Cnt,0xFFFF); //将收到的信息发送出去
    while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);//检测UART发送结束
    Uart3_Rx_Cnt = 0;
    memset(RxBuffer3,0x00,sizeof(RxBuffer3)); //清空数组
}

void CheckCommandList(void)
{
    uint8_t i;
    static uint16_t usbTick = 0;
    static uint16_t lastUsbRx = 0;
    if (g_UsbATStatus == 0 && usb_Rx_Cnt == 0) {
        usbTick = 0;
        lastUsbRx = 0;
        return;
    }
    if (usb_Rx_Cnt > lastUsbRx) {
        lastUsbRx = usb_Rx_Cnt;
        usbTick = 0;
    } else {
        usbTick++;
    }
    if (usbTick >= USB_UART_TICK_CLEAR || g_UsbATStatus == 1) {
        for (i = 0; i < MAX_COMMAND_NUMBER; i++) {
            if (strlen(CommandList[i]) > 0 && strSearch((char*)g_UsbRxBuff, CommandList[i]) == 0) {
                UsbCommand[i]((char*)g_UsbRxBuff);
                break;
            }
        }				
        if (i == MAX_COMMAND_NUMBER) {
           UsbPrintf("\nNo this commmand !\n");
        }
        memset(g_UsbRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
        usb_Rx_Cnt = 0;
        g_UsbATStatus = 0;
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
			//Command_analyse(g_DeviceRxBuff);
		}
        memset(g_DeviceRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
        device_Rx_Cnt = 0;
        g_DeviceATStatus = 0;
        deviceTick = 0;
        lastDeviceRx = 0;
	 }
}







