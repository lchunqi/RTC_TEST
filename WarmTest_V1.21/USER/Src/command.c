#include "usart.h"
#include "iic_soft.h"
#include "temp.h"
#include "spi_soft.h"
#include "manager.h"
#include "command.h"

#define UART_RECEIVE_BUFF_NUMBER    200
#define UART_TRANSMIT_BUFF_NUMBER   300
#define USB_UART_TICK_CLEAR         100

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
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


char Command_buf[200] = {0};

#define MAX_COMMAND_NUMBER  40
#define MAX_COMMAND_LENGTH  20
static char CommandList[MAX_COMMAND_NUMBER][MAX_COMMAND_LENGTH] = {
	"AT^HELP",
	"AT^READDATA",
	"AT^SETTIME",
	"AT^IICINT",
	"AT^SD3077INT",
	"AT^SD3178INT",
	"AT^SD8825INT",
	"AT^SD8931INT",
	"AT^SD8800INT",
	"AT^SD8801INT",
	"AT^SD8810INT",
	"AT^SD8840INT",
	"AT^RTCOTP",
	"AT^SYSTEMP?",
	"AT^ATTEST",
	"AT^SYSTEST",
    "AT^IICREADTIME",
	"AT^SPIREADTIME",
	
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

void  AT_READDATA_Handle(char* command)
{
	uint8_t buffer[7]={0};
	int i=0,j=0,num = 0,reg = 0;
	CharToInt(command, 12, &i);
	CharToInt(command, 14, &j);
	IIC_GPIO_Init(i-1,j-1);
	SPI_GPIO_Init( i-1, j-1);
	num = strSearch(command, "reg:");
	if (num != -1) {
		num += strlen("reg:");
		if (CharToInt(command, num, &reg) == 0 ) {	return; }
	}
	//UsbPrintf("reg = %d \n",reg);
	if (IIC_Read_multi_bytes(0x64, reg, buffer, 8) == I2C_OK) {
		UsbPrintf("%02X %02X %02X %02X %02X %02X %02X %02X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
	}else if(IIC_Read_multi_bytes(0xD0, reg, buffer, 8) == I2C_OK){
		UsbPrintf("%02X %02X %02X %02X %02X %02X %02X %02X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
	}
	else if (SPI_Read_multi_byte( reg, buffer, 8) == 1) {
		UsbPrintf("%02X %02X %02X %02X %02X %02X %02X %02X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
	}else {
		UsbPrintf("Error\n");
	}

}

void  AT_SETTIME_Handle(char* command)
{
	static RealTime rtc = {0};
	static int k=0;
	int num = 0,inNumber = 0,i=0,j=0;
	CharToInt(command, 13, &i);
	CharToInt(command, 15, &j);
	CharToInt(command, 11, &k);
	
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
	num = strSearch(command, "week:");
	if (num != -1) {
		num += strlen("week:");
		if (CharToInt(command, num, &inNumber) == 0 ) {	return; }
		rtc.week = inNumber % 100;
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
	UsbPrintf("\ncommand set time \n--20%x/%x/%x %x %x:%x:%x--\n", 
    rtc.year, rtc.mon, rtc.day, rtc.week,rtc.hour, rtc.min, rtc.sec);
	
	uint8_t data2[8]={rtc.sec, rtc.min, rtc.hour, rtc.week, rtc.day, rtc.mon, rtc.year};
	
	switch (k)
	{
		case 1://代表iic
			IIC_GPIO_Init(i-1,j-1);
			if( IIC_Write_multi_bytes(RTC_8931_ADDRESS, RTC_8931_REG, data2, 7)== I2C_OK || 
				IIC_Write_multi_bytes(RTC_8825_ADDRESS, RTC_8825_REG, data2, 7)== I2C_OK)
				{
					UsbPrintf("Date set success !\r\n");
				}
				else{
					UsbPrintf("Date set ERROR: \r\n");
				}
			break;
		case 2://代表spi
			SPI_GPIO_Init( i-1, j-1);
			SPI_Write_multi_byte(RTC_8840_ADDRESS,data2,7);
			UsbPrintf("Date set success !\r\n");
			break;

		case 3:
			IIC_GPIO_Init(i-1,j-1);
			SD3178_Write_Enable();
			IIC_Write_multi_bytes(RTC_3178_ADDRESS, RTC_3178_REG, data2, 7);
			SD3178_Write_Disable();
			break;
		default://
			UsbPrintf("Date set incomplete !\r\n");
			break;
	}
}

void AT_IICINT_Handle(char* command)
{
	float sysTemp = 0;
	uint8_t rtcTemp = 0;
	int i,j;
	CharToInt(command, 10, &i);
	CharToInt(command, 12, &j);
	UsbPrintf("\n--TEST: \n");
	IIC_GPIO_Init(i-1,j-1);  
	RTC_8804_TEMP(&sysTemp, &rtcTemp);
	UsbPrintf("--板温 %d , 芯温 %d \n",(uint8_t)sysTemp , rtcTemp);
	
}

void AT_SD3077INT_Handle(char* command)
{
	int i=0,j=0;
	uint8_t SD3077_INTflag=0;
	uint8_t SD3077_4096[2]={0xA1,0x02},SD3077_1024[2]={0xa1,0x03},SD3077_64[2]={0xa1,0x04};//,SD3077_1HZ[2]={0xa1,0x0A};
	uint8_t SD3077_1min[2]={0xF4,0x30},SD3077_min[3]={0x01,0X00,0X00},SD3077_1S[2]={0xF4,0x20},SD3077_time[3]={0x01,0X00,0X00};
	uint8_t SD3077_1hz[2]={0xF4,0x40},SD3077_hz[3]={0x00,0x10,0x00};
	uint8_t rtcTemp=0 ;
	float   sysTemp=0 ;
	CharToInt(command, 13, &i);
	CharToInt(command, 15, &j);
	SD3077_INTflag=INT_Choose(command);
	switch (i)
	{
		case 0:
			UsbPrintf("\n--CLOSE SD3077 INT ! \n");
			Close3077_FREQ();
			break;
		default:
			break;
	}
	if(i!=0)
	{
		Close3077_FREQ();
		UsbPrintf("\n--SD3077 INT TEST:  i=%d  j=%d \n", i, j);
		IIC_GPIO_Init( i-1, j-1);
		SD3077_Write_Enable();
		RTC_3077_TEMP(&sysTemp,&rtcTemp);	
		switch (SD3077_INTflag)
		{
			case 96:
				if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x10,SD3077_4096,2)== I2C_OK) 
				{
					Enable_IIC4051(j,i);
				}
				break;
			case 24:
				if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x10,SD3077_1024,2)== I2C_OK) 
				{
					Enable_IIC4051(j,i);
				}
				break;
			case 64:
				if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x10,SD3077_64,2)== I2C_OK) 
				{
					Enable_IIC4051(j,i);
				}
				break;
			case 1://倒计时
				if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x10,SD3077_1hz,2)== I2C_OK) 
				{
					if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x13,SD3077_hz,3)== I2C_OK)
					{Enable_IIC4051(j,i);}
				}
				break;
			case 60://倒计时
				if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x10,SD3077_1min,2)== I2C_OK) 
				{
					if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x13,SD3077_min,3)== I2C_OK)
					Enable_IIC4051(j,i);
				}
				break;
			case 61://倒计时
				if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x10,SD3077_1S,2)== I2C_OK) 
				{
					if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x13,SD3077_time,3)== I2C_OK) 
					Enable_IIC4051(j,i);
				}
				break;
			default:
				UsbPrintf("--INT ERROR ! \r\n");
				break;
		}
		SD3077_Write_Disable();
	}
}

//此3178 1Hz 属于倒计时中断输出，原来使用的是频率中断输出
void AT_SD3178INT_Handle(char* command)
{
	int i=0,j=0;
	uint8_t SD3178_32k=0X00,SD3178_INTFLAG=0;
	uint8_t SD3178_4096[2]={0xA1,0x42},SD3178_1024[2]={0xa1,0x43},SD3178_64[2]={0xa1,0x44};//SD3178_1HZ[2]={0xa1,0x4a};
	uint8_t SD3178_1hz[2]={0xF4,0x40},SD3178_hz[3]={0x00,0x10,0x00};
	uint8_t SD3178_1min[2]={0xF4,0x70},SD3178_min[3]={0x01,0X00,0X00},SD3178_1S[2]={0xF4,0x60},SD3178_time[3]={0x01,0X00,0X00};
	uint8_t rtcTemp=0 ;
	float   sysTemp=0 ;
	CharToInt(command, 13, &i);
	CharToInt(command, 15, &j);
	SD3178_INTFLAG=INT_Choose(command);
	switch (i)
	{
		case 0:
			Close3178_FREQ();
			UsbPrintf("\n--CLOSE SD3178 INT ! \n");
			break;
		default:
			break;
	}
	if(i!=0)
	{
		Close3178_FREQ();
		UsbPrintf("\n--SD3178 INT TEST:  i=%d  j=%d \n", i, j);
		IIC_GPIO_Init( i-1, j-1);
		SD3178_Write_Enable();
		RTC_3178_TEMP(&sysTemp,&rtcTemp);

		switch (SD3178_INTFLAG)
		{
		case 32:
			if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x11,&SD3178_32k,1)== I2C_OK) 
			{
				Enable_IIC4051(j,i);
			}
			break;	
		case 96:
			if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x10,SD3178_4096,2)== I2C_OK) 
			{
				Enable_IIC4051(j,i);
			}
			break;
		case 24:
			if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x10,SD3178_1024,2)== I2C_OK) 
			{
				Enable_IIC4051(j,i);
			}
			break;
		case 64:
			if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x10,SD3178_64,2)== I2C_OK) 
			{
				Enable_IIC4051(j,i);
			}
			break;
		case 1://倒计时
			if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x10,SD3178_1hz,2)== I2C_OK) 
			{
				if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x13,SD3178_hz,3)== I2C_OK)
				{Enable_IIC4051(j,i);}
			}
			break;
		case 60://倒计时
			if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x10,SD3178_1min,2)== I2C_OK) 
			{
				if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x13,SD3178_min,3)== I2C_OK)
				{Enable_IIC4051(j,i);}
			}
			break;
		case 61://倒计时
			if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x10,SD3178_1S,2)== I2C_OK) 
			{
				if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x13,SD3178_time,3)== I2C_OK) 
				{Enable_IIC4051(j,i);}
			}
			break;
		default:
			UsbPrintf("-- INT ERROR ! \r\n");
			break;
		}

		SD3178_Write_Disable();
	}
}

void AT_SD8825INT_Handle(char* command)
{
	int i=0,j=0;
	uint8_t SD8825_32K=0X0C,SD8825_1024HZ=0X04,SD8825_FLAG=0;//SD8825_1HZ=0X08;
	uint8_t rtcTemp=0 ;
	float   sysTemp=0 ;
	//uint8_t SD8825_1S[5] = {0x40, 0x00, 0x11, 0x00, 0x10}; //倒计时中断
	uint8_t SD8825_1min[3]={0X20,0X00,0X20},SD8825_1S[3]={0X00,0X00,0X20}; //时间中断
	uint8_t SD8825INT_1HZ[5]={0X01,0X00,0X12,0X00,0X10};//倒计时
	CharToInt(command, 13, &i);
	CharToInt(command, 15, &j);
	SD8825_FLAG=INT_Choose(command);
	switch (i)
	{
		case 0:
			Close8825_FREQ();
			UsbPrintf("--CLOSE SD8825 FREQ ! \n");
			break;
		default:
			break;
	}
	if(i!=0)
	{
		Close8825_FREQ();
		IIC_GPIO_Init(i-1,j-1);
		UsbPrintf("\n--SD8825 FREQ TEST:  i=%d  j=%d \n", i, j);

		RTC_8825_TEMP(&sysTemp,&rtcTemp);
		switch (SD8825_FLAG)
		{
			case 32:
				Control_level(i,1);
				if(IIC_Write_multi_bytes(RTC_8825_ADDRESS, 0X0D, &SD8825_32K, 1)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			case 24:
				Control_level(i,1);	
				if(IIC_Write_multi_bytes(RTC_8825_ADDRESS, 0X0D, &SD8825_1024HZ, 1)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			case 1:
				//Control_level(i,1);//倒计时
				if(IIC_Write_multi_bytes(RTC_8825_ADDRESS, 0X0B,SD8825INT_1HZ, 5)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			case 60://时间更新中断
				if(IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0X0D, SD8825_1min, 3)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			case 61://时间更新中断
				if(IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0x0D, SD8825_1S, 3)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			default:
				UsbPrintf("--FREQ ERROR ! \r\n");
				break;
		}	

	}
}

void AT_SD8931INT_Handle(char* command)
{
	int i=0,j=0;
	uint8_t SD8931_32k[2]={0x04,0X08},SD8931_FLAG=0;	
	uint8_t SD8931_1HZ[2]={0X00,0X00},SD8931_1024HZ[2]={0X08,0X00},
			SD8931_4096HZ[2]={0X10,0X00},SD8931_8192HZ[2]={0X18,0X00};
	uint8_t rtcTemp=0 ;
	float   sysTemp=0 ;
	CharToInt(command, 13, &i);
	CharToInt(command, 15, &j);
	SD8931_FLAG=INT_Choose(command);
	switch (i)
	{
		case 0:
			Close8931_FREQ();
			UsbPrintf("\n--CLOSE SD8931 INT ! \n");
			break;
		default:
			break;
	}
	if(i!=0)//代表iic
	{
		Close8931_FREQ();
		IIC_GPIO_Init(i-1,j-1);
		UsbPrintf("\n--SD8931 INT TEST:  i=%d  j=%d \n", i, j);
	
		RTC_8931_TEMP(&sysTemp,&rtcTemp);
		switch (SD8931_FLAG)
		{
		case 32:
			if(IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X0E, SD8931_32k, 2)== I2C_OK){
				Enable_IIC4051(j,i);
			}else{
				UsbPrintf("--INT ERROR ! \r\n");
			}
			break;
		case 1:
			if(IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X0E, SD8931_1HZ, 2)== I2C_OK){
				Enable_IIC4051(j,i);
			}else{
				UsbPrintf("--INT ERROR ! \r\n");
			}
			break;
		case 24:
			if(IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X0E, SD8931_1024HZ, 2)== I2C_OK){
				Enable_IIC4051(j,i);
			}else{
				UsbPrintf("--INT ERROR ! \r\n");
			}
			break;
		case 96:
			if(IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X0E, SD8931_4096HZ, 2)== I2C_OK){
				Enable_IIC4051(j,i);
			}else{
				UsbPrintf("--INT ERROR ! \r\n");
			}
			break;
		case 92:
			if(IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X0E, SD8931_8192HZ, 2)== I2C_OK){
				Enable_IIC4051(j,i);
			}else{
				UsbPrintf("--INT ERROR ! \r\n");
			}
			break;
		default:
			UsbPrintf("--INT ERROR ! \r\n");
			break;
		}	
	}
}

void AT_SD8800INT_Handle(char* command)
{
	int i=0,j=0;
	uint8_t SD8800_32K=0X00,SD8800_1024=0X04,SD8800_FLAG=0;//,SD8800_1HZ=0X08
	uint8_t SD8800_1min[3]={0X20,0X00,0X20},SD8800_1S[3]={0X00,0X00,0X20}; //时间中断
	uint8_t SD8800_1HZ[5]={0X01,0X00,0X12,0X00,0X10};
	uint8_t rtcTemp=0 ;
	float   sysTemp=0 ;
	CharToInt(command, 13, &i);
	CharToInt(command, 15, &j);
	SD8800_FLAG=INT_Choose(command);
	switch (i)
	{
		case 0:
			Close8800_FREQ();
			UsbPrintf("\n--CLOSE SD8800 FREQ ! \n");
			break;
		default:
			break;
	}
	if(i!=0)//代表iic
	{
		Close8800_FREQ();
		IIC_GPIO_Init(i-1,j-1);
		UsbPrintf("\n--SD8800 FREQ TEST:  i=%d  j=%d \n", i, j);
		RTC_8800_TEMP(&sysTemp,&rtcTemp);
		switch (SD8800_FLAG)
		{
			case 32:
				Control_level(i,1);
				if(IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0X0D, &SD8800_32K, 1)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			case 24:
				Control_level(i,1);
				if(IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0X0D, &SD8800_1024, 1)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			case 1:
				//Control_level(i,1);
				if(IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0X0B, SD8800_1HZ, 5)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			case 60:
				if(IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0X0D, SD8800_1min, 3)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			case 61://int no fctr
				if(IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0X0D, SD8800_1S, 3)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			default:
				UsbPrintf("--INT ERROR ! \r\n");
				break;
		}
	}
}

void AT_SD8801INT_Handle(char* command)
{
	int i=0,j=0;
	uint8_t SD8801_1HZ[5]={0X01,0X00,0X12,0X00,0X10};
	uint8_t SD8801_1min[3]={0X20,0X00,0X20},SD8801_1S[3]={0X00,0X00,0X20}; //时间中断
	uint8_t SD8801_FLAG=0;
	uint8_t rtcTemp=0 ;
	float   sysTemp=0 ;
	CharToInt(command, 13, &i);
	CharToInt(command, 15, &j);
	SD8801_FLAG=INT_Choose(command);
	switch (i)
	{
		case 0:
			Close8801_FREQ();
			UsbPrintf("\n--CLOSE SD8801 FREQ ! \n");
			break;
		default:
			break;
	}
	if(i!=0)//代表iic
	{
		Close8801_FREQ();
		IIC_GPIO_Init(i-1,j-1);
		UsbPrintf("\n--SD8801 FREQ TEST:  i=%d  j=%d \n", i, j);
		
		RTC_8800_TEMP(&sysTemp,&rtcTemp);
		switch (SD8801_FLAG)
		{
			
			case 1:	
				if(IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0X0B, SD8801_1HZ, 5)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			case 60://int no fctr
				if(IIC_Write_multi_bytes(RTC_8801_ADDRESS, 0X0D, SD8801_1min, 3)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			case 61:
				if(IIC_Write_multi_bytes(RTC_8801_ADDRESS, 0X0D, SD8801_1S, 3)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			default:
				UsbPrintf("--INT ERROR ! \r\n");
				break;
		}
	}
}

void AT_SD8810INT_Handle(char* command)
{
	int i=0,j=0;
	uint8_t SD8810_1min[3]={0X20,0X00,0X20},SD8810_1S[3]={0X00,0X00,0X20},SD8810_1HZ[5]={0X01,0X00,0X12,0X00,0X10}; //时间中断
	uint8_t SD8810_32K=0XC0,SD8810_1024=0X80,SD8810_INT=0X00,SD8810_FLAG=0;//SD8810_INT int2输出
	uint8_t rtcTemp=0 ;
	float   sysTemp=0 ;
	CharToInt(command, 13, &i);
	CharToInt(command, 15, &j);
	SD8810_FLAG=INT_Choose(command);
	switch (i)
	{
		case 0:
			Close8810_FREQ();
			UsbPrintf("\n--CLOSE sd8810 INT ! \n");
			break;
		default:
			break;
	}
	if(i!=0)//代表iic
	{
		Close8810_FREQ();
		IIC_GPIO_Init(i-1,j-1);
		UsbPrintf("\n--SD8810 INT TEST:  i=%d  j=%d \n", i, j);
		
		RTC_8810_TEMP(&sysTemp,&rtcTemp);
		if(IIC_Write_multi_bytes(RTC_8810_ADDRESS, 0X32, &SD8810_INT, 1)== I2C_OK)//选择INT1输出0x01
		{
		switch (SD8810_FLAG)
		{
			case 32:
				if(IIC_Write_multi_bytes(RTC_8810_ADDRESS, 0X1D, &SD8810_32K, 1)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			case 24:
				if(IIC_Write_multi_bytes(RTC_8810_ADDRESS, 0X1D, &SD8810_1024, 1)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			case 1:                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
				SD8810_INT=0X04;
				if(IIC_Write_multi_bytes(RTC_8810_ADDRESS, 0X32, &SD8810_INT, 1)== I2C_OK){
					if(IIC_Write_multi_bytes(RTC_8810_ADDRESS, 0X1B, SD8810_1HZ, 5)== I2C_OK){
						Enable_IIC4051(j,i);
					}
				}
				break;
			case 60:
				if(IIC_Write_multi_bytes(RTC_8810_ADDRESS, 0X1D, SD8810_1min, 3)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			case 61:
				if(IIC_Write_multi_bytes(RTC_8810_ADDRESS, 0X1D, SD8810_1S, 3)== I2C_OK){
					Enable_IIC4051(j,i);
				}
				break;
			default:
				UsbPrintf("--INT ERROR ! \r\n");
				break;
			}
		}
	}
}

void AT_SD8840INT_Handle(char* command)
{
	int i=0,j=0;
	uint8_t SD8840_FLAG=0,SD8840_32K=0X0C, SD8840_1024=0X08; //SD8840_1HZ=0X04;
	uint8_t SD8840_1HZ[5]={0X01,0X00,0X12,0X00,0X10};
	uint8_t SD8840_1S[3]={0X00,0X00,0X20},SD8840_1min[3]={0X20,0X00,0X20};
	uint8_t rtcTemp=0 ;
	float   sysTemp=0 ;
	CharToInt(command, 13, &i);
	CharToInt(command, 15, &j);	
	SD8840_FLAG=INT_Choose(command);
	switch (i)
	{
		case 0:
			Close8840_FREQ();
			UsbPrintf("\n--CLOSE INT ! \n");
			break;
		default:
			break;
	}
	if(i!=0)//代表spi
	{
		UsbPrintf("\n--SD8840 INT TEST: \r\n  i=%d  j=%d \n", i, j);
		Close8840_FREQ();//开始就全部关闭
		SPI_GPIO_Init( i-1, j-1);
		
		RTC_8840_TEMP(&sysTemp,&rtcTemp);
		switch (SD8840_FLAG)
		{
		case 32:
			SPI_Write_multi_byte(0x0D,&SD8840_32K,1);
			Enable_SPI4051(j,i);
			break;
		case 24:
			SPI_Write_multi_byte(0x0D,&SD8840_1024,1);
			Enable_SPI4051(j,i);
			break;
		case 1:
			SPI_Write_multi_byte(0x0B,SD8840_1HZ,5);
			Enable_SPI4051(j,i);
			break;
		case 60:
			SPI_Write_multi_byte(0x0D,SD8840_1min,3);
			Enable_SPI4051(j,i);
			break;
		case 61:
			SPI_Write_multi_byte(0x0D,SD8840_1S,3);
			Enable_SPI4051(j,i);
			break;
		default:
			UsbPrintf("--SPI FREQ ERROR ! \r\n");
			break;
		}
	}
}

void AT_RTCOTP_Handle(char* command)
{
	UsbPrintf("\n---Write Success !\r\n");
}

void AT_SYSTEMP_Handle(char* command)
{
	UsbPrintf("\n--TEMP TEST\n");
	float temp;
	temp = temp_read_value(1);
	UsbPrintf("system temp1 = %.1f C\n", temp);
	// temp = temp_read_value(2);
	// UsbPrintf("system temp2 = %.1f C\n", temp);
}

void AT_ATTEST_Handle(char* command)
{
	int i,j;
	CharToInt(command, 10, &i);
	CharToInt(command, 12, &j);
	UsbPrintf("\n--TEST: \n");
	IIC_GPIO_Init(i-1,j-1);  
	//uint8_t SD8931_1min[4]={0X01,0x80,0X80,0X80};
	uint8_t SD8931_1min[3]={0x80,0X80,0X80};
	//uint8_t SD8931_CTR1=0x00;
	uint8_t SD8931_INT2=0x06;
	uint8_t buffer[7]={0};
	// if(IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X0F, &SD8931_CTR1, 1)== I2C_OK)
	// {
	// 	if(IIC_Read_multi_bytes(RTC_8931_ADDRESS, 0X0F, buffer, 1) == I2C_OK) {
	// 		UsbPrintf("OF=%02X\n", buffer[0]);
	// 	}
		if(IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X0B, SD8931_1min, 3)== I2C_OK)
		{
			if(IIC_Read_multi_bytes(RTC_8931_ADDRESS, 0X0B, buffer, 4) == I2C_OK) {
				UsbPrintf("0X07 %02X %02X %02X %02X\n", buffer[0],buffer[1],buffer[2],buffer[3]);
			}
			if(IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X0E, &SD8931_INT2, 1)== I2C_OK)
			{
				if(IIC_Read_multi_bytes(RTC_8931_ADDRESS, 0X0E, buffer, 1) == I2C_OK) {
					UsbPrintf("0E=%02X\n", buffer[0]);
				}
				// if(IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X0F, &SD8931_CTR1, 1)== I2C_OK)
				// {
					Enable_IIC4051(j,i);
					UsbPrintf("--OK ! \r\n");
				// }
			 }
		}
		else{
			UsbPrintf("--IIC FREQ ERROR ! \r\n");
		}
	//}

	
	
}

void AT_SYSTEST_Handle(char* command)
{
	int i;
	CharToInt(command, 11, &i);
	uint8_t IIC_OK=0;
	uint8_t SPI_OK=0;
	switch (i)
	{
		case 1://iic
			UsbPrintf("\n--IIC Connect TEST: \n");
			IIC_OK=I2C_Connect();
			UsbPrintf("--Count 24 Connect %d ! \n",IIC_OK);
			break;
		case 2://spi
			UsbPrintf("\n--SPI Connect TEST: \n");
			SPI_OK=SPI_Connect(0x00);
			UsbPrintf("--Count 24 Connect %d ! \n",SPI_OK);
			break;
		default:
    		UsbPrintf("version:1.3 on %s at %s \r\n",__DATE__,__TIME__);
			break;
	}
}

void AT_IICREADTIME_Handle(char* command)
{
	int i,j;
    UsbPrintf("\n--IIC READTIME: \n");
	CharToInt(command, 15, &i);
	CharToInt(command, 17, &j);
	I2C_ReadTime(i,j);
	HAL_GPIO_WritePin(GPIOC,0XFFFF,0);
 }

void AT_SPIREADTIME_Handle(char* command)
{	
	int i,j;
	UsbPrintf("\n--SPI READTIME: \n");	
	CharToInt(command, 15, &i);
	CharToInt(command, 17, &j);
	SPI_ReadTime(i,j);
	HAL_GPIO_WritePin(GPIOB,0XFFFF,0);
	HAL_GPIO_WritePin(GPIOC,0XFFFF,0);
}

typedef void(*function)(char* s);
function UsbCommand[MAX_COMMAND_NUMBER] = {
    AT_HELP_Handle,
    AT_READDATA_Handle,
    AT_SETTIME_Handle,
    AT_IICINT_Handle,
	AT_SD3077INT_Handle,
	AT_SD3178INT_Handle,
	AT_SD8825INT_Handle,
	AT_SD8931INT_Handle,
	AT_SD8800INT_Handle,
	AT_SD8801INT_Handle,
	AT_SD8810INT_Handle,
	AT_SD8840INT_Handle,
	AT_RTCOTP_Handle,
	AT_SYSTEMP_Handle,	
    AT_ATTEST_Handle,	
	AT_SYSTEST_Handle,
    AT_IICREADTIME_Handle,
	AT_SPIREADTIME_Handle,
};
/*
//3178 1min时间报警中断输出   1Hz倒计时中断输出   32K是F32K的11H寄存器输出
//8800 1min时间更新中断输出   1Hz倒计时中断输出   32K 频率输出
//8804 1min时间更新中断输出   1Hz倒计时中断输出   32K 频率输出
//8810 1min时间更新中断输出   1Hz倒计时中断输出   32K 频率输出   
//8825 1min时间更新中断输出   1Hz倒计时中断输出   32K 频率输出
//8840 1min时间更新中断输出   1Hz倒计时中断输出   32K 频率输出
//8931 1min时间报警中断输出   1Hz频率输出        32K FREQ输出
//uint8_t SD3178_1min[2]={0xF4,0x70},SD3178_min[3]={0x01,0X00,0X00};
*/
void Command_analyse(uint8_t* Command_buf)
{
	static uint8_t chipName = 0;
	uint8_t SD3178_1min=0xD2,SD3178_min[8]={0x01,0X00,0X00,0X00,0X00,0X00,0X00,0X01};
	uint8_t	SD3178_1hz[2]={0xF4,0x40},SD3178_hz[3]={0x00,0x10,0x00};
	uint8_t INT_1min[3]={0X20,0X00,0X20},int_1HZ[5]={0X01,0X00,0X12,0X00,0X10},INT_32k=0X00;
	uint8_t SD8810_32K=0XC0,SD8840_32K=0X0C,SD8810_INT=0X01;
	uint8_t SD8931_32K[2]={0x04,0X08},SD8931_1HZ[2]={0X00,0X00};
	uint8_t SD8931_1min[8]={0X01,0x80,0X80,0X80,0x80,0X80,0X80,0X05};//,SD8931_INT2=0x06;
	uint8_t buf[4] = {0xff,0xff,0xff,0xff};
	uint8_t i = 0;
	float sysTemp = 0;
	uint8_t rtcTemp = 0;
	uint8_t number=0;
	if(Command_buf[0] == 0xFA && Command_buf[1] == 0xFB )
	{
		number = (Command_buf[2] / 16) * 10 + (Command_buf[2] % 16);
		if (number < 30) {
		number = number - 1;
		} else if (number < 60) {
			number = number - 31;
		} else {
			number = number - 61;
		}
		chipName = Command_buf[3];
		UsbPrintf("--chipName = %d number = %d\n", chipName ,number);
		switch (chipName) {
			case 0x00://3178 		//测试ok  
				Close3178_FREQ();
				//UsbPrintf("--Command_buf[2] = 0x%X, number = %d\n", Command_buf[2], number);
				IIC_GPIO_Init((number / 8), (number % 8));//配置IIC的SCL和SDA引脚
				SD3178_Write_Enable();
				RTC_3178_TEMP(&sysTemp,&rtcTemp);
				UsbPrintf("--板温 %d , 芯温 %d \n",(uint8_t)sysTemp , rtcTemp);
				buf[0] = number; 
				if (sysTemp >= 0) {
					buf[1] = (int)sysTemp;
				} else {
					buf[1]= 256 + (int)sysTemp;
				}
				buf[2] = rtcTemp;
				buf[3] = rtcTemp;
				if (Command_buf[2] > 0x30) {//32k信号可以直接输出
					if (Command_buf[2] > 0x60){//1HZ
						for (i = 0; i < 3; i++) {
							if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x10,SD3178_1hz,2)== I2C_OK) 
							{
								if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x13,SD3178_hz,3)== I2C_OK)
								{
									HAL_Delay(2);
									Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
									break;
								}
							}
						}
						if (i >= 3) {
							UsbPrintf("3178 chip %d i2c error!\n", number);
						}
					}else{
						for (i = 0; i < 3; i++) {//1分钟
							if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x10,&SD3178_1min,1)== I2C_OK) 
							{
								if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x07,SD3178_min,8)== I2C_OK)
								{
									HAL_Delay(2);
									Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
									break;
								}
							}
						}
						if (i >= 3) {
							UsbPrintf("3178 chip %d i2c error!\n", number);
						}
					}
				} else {
					if (IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x11, &INT_32k, 1) == I2C_OK)
					{
						HAL_Delay(2);
						Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
					}else {
						UsbPrintf("--3178 chip %d i2c error!\n", number);
					}
				}
				SD3178_Write_Disable();
				break;
			case 0x01://8800 1     //测试ok   
				Close8800_FREQ();
				if (0x01 <= Command_buf[2] && Command_buf[2] <= 0x24) {
					Control_level((number / 8)+1 , 1);//拉高芯片的FCTR引脚  32k
				} else if (0x31 <= Command_buf[2] && Command_buf[2] <= 0x54) {
					Control_level((number / 8)+1, 0);//1分钟关闭ctr
				} else if (0x61 <= Command_buf[2] && Command_buf[2] <= 0x84) {
					Control_level((number / 8)+1, 0);//1HZ关闭ctr
				} else {
					UsbPrintf("--error number = %d\n",number);
					number = 0;
				}
				//UsbPrintf("--Command_buf[2] = 0x%X, number = %d\n", Command_buf[2], number);
				IIC_GPIO_Init((number / 8), (number % 8));//配置IIC的SCL和SDA引脚
				RTC_8800_TEMP(&sysTemp, &rtcTemp);
				UsbPrintf("--板温 %d , 芯温 %d \n",(uint8_t)sysTemp , rtcTemp);
				buf[0] = number; 
				if (sysTemp < 0) {
					buf[1] = (uint8_t)(sysTemp + 256);
				} else {
					buf[1] = (uint8_t)(sysTemp);
				}
				buf[2] = rtcTemp;
				buf[3] = rtcTemp;
				if (Command_buf[2] > 0x30) {//8800的32k信号可以直接输出
					if (Command_buf[2] > 0x60){//1HZ
						for (i = 0; i < 3; i++) {
							if (IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0X0B,int_1HZ, 5) == I2C_OK) {
								HAL_Delay(2);
								Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
								break;
							}
						}
						if (i >= 3) {
							UsbPrintf("8800 chip %d i2c error!\n", number);
						}
					}else{
						for (i = 0; i < 3; i++) {//1分钟
							if (IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0X0D,INT_1min, 3) == I2C_OK) {
								HAL_Delay(2);
								Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
								break;
							}
						}
						if (i >= 3) {
							UsbPrintf("8800 chip %d i2c error!\n", number);
						}
					}
				} else {
					if (IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0X0D, &INT_32k, 1) == I2C_OK)
					{
						HAL_Delay(2);
						Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
					}else{
						UsbPrintf("8800 chip %d i2c error!\n", number);
					}
				}
				break;
			case 0x02://8804  
				Close8800_FREQ();
				if (0x01 <= Command_buf[2] && Command_buf[2] <= 0x24) {
					Control_level((number / 8) +1, 1);//拉高芯片的FCTR引脚
				} else if (0x31 <= Command_buf[2] && Command_buf[2] <= 0x54) {
					Control_level((number / 8) +1, 0);
				} else if (0x61 <= Command_buf[2] && Command_buf[2] <= 0x84) {
					Control_level((number / 8) +1, 0);
				} else {
					UsbPrintf("--error number = %d\n",number);
					number = 0;
				}
				UsbPrintf("--Command_buf[2] = 0x%X, number = %d\n", Command_buf[2], number);
				IIC_GPIO_Init((number / 8), (number % 8));//配置IIC的SCL和SDA引脚
				RTC_8804_TEMP(&sysTemp, &rtcTemp);
				UsbPrintf("--板温 %d , 芯温 %d \n",(uint8_t)sysTemp , rtcTemp);
				buf[0] = number; 
				if (sysTemp >= 0) {
					buf[1] = (int)sysTemp;
				} else {
					buf[1]= 256 + (int)sysTemp;
				}
				buf[2] = rtcTemp;
				buf[3] = rtcTemp;
				if (Command_buf[2] > 0x30) {//32k信号可以直接输出
					if (Command_buf[2] > 0x60){//1HZ
						for (i = 0; i < 3; i++) {
							if (IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0X0B,int_1HZ, 5) == I2C_OK) {
								HAL_Delay(2);
								Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
								break;
							}
							HAL_Delay(1);
						}
						if (i >= 3) {
							UsbPrintf("8804 chip %d i2c error!\n", number);
						}
					}else{
						for (i = 0; i < 3; i++) {//1分钟
							if (IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0X0D,INT_1min, 3) == I2C_OK) {
								HAL_Delay(2);
								Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
								break;
								}
								HAL_Delay(1);
							}
							if (i >= 3) {
								UsbPrintf("8804 chip %d i2c error!\n", number);
							}
						}
				} else {
					if (IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0X0D, &INT_32k, 1) == I2C_OK)
					{
						HAL_Delay(2);
						Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
					}else{
						UsbPrintf("8804 chip %d i2c error!\n", number);
					}
				}
				break;
			case 0x03://8810
				Close8810_FREQ();
				UsbPrintf("--Command_buf[2] = 0x%X, number = %d\n", Command_buf[2], number);
				IIC_GPIO_Init((number / 8), (number % 8));//配置IIC的SCL和SDA引脚
				RTC_8810_TEMP(&sysTemp, &rtcTemp);
				UsbPrintf("--板温 %d , 芯温 %d \n",(uint8_t)sysTemp , rtcTemp);
				buf[0] = number; 
				if (sysTemp >= 0) {
					buf[1] = (int)sysTemp;
				} else {
					buf[1]= 256 + (int)sysTemp;
				}
				buf[2] = rtcTemp;
				buf[3] = rtcTemp;
				if(IIC_Write_multi_bytes(RTC_8810_ADDRESS, 0X32, &SD8810_INT, 1)== I2C_OK)
				{
				if (Command_buf[2] > 0x30) {//32k信号可以直接输出
					if (Command_buf[2] > 0x60){//1HZ
						for (i = 0; i < 3; i++) 
						{
							SD8810_INT=0X04;
							if(IIC_Write_multi_bytes(RTC_8810_ADDRESS, 0X32, &SD8810_INT, 1)== I2C_OK)
							{
								if(IIC_Write_multi_bytes(RTC_8810_ADDRESS, 0X1B, int_1HZ, 5)== I2C_OK)
								{
									HAL_Delay(2);
									Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
									break;
								}
							}
						}
						if (i >= 3) {
							UsbPrintf("8810 chip %d i2c error!\n", number);
						}
					}else{
						for (i = 0; i < 3; i++) {//1分钟
							if (IIC_Write_multi_bytes(RTC_8810_ADDRESS, 0X1D, INT_1min, 3) == I2C_OK) {
								HAL_Delay(2);
								Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
								break;
							}
						}
						if (i >= 3) {
							UsbPrintf("8810 chip %d i2c error!\n", number);
						}
					}
				} else {
					if (IIC_Write_multi_bytes(RTC_8810_ADDRESS, 0X1D, &SD8810_32K, 1) == I2C_OK)
						{
							HAL_Delay(2);
							Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
						}else{
							UsbPrintf("8810 chip %d i2c error!\n", number);
						}
					}
				}
				break;
			case 0x04://8825		//测试ok  
				Close8825_FREQ();
				if (0x01 <= Command_buf[2] && Command_buf[2] <= 0x24) {
					Control_level((number / 8)+1 , 1);//拉高芯片的FCTR引脚
				} else if (0x31 <= Command_buf[2] && Command_buf[2] <= 0x54) {
					Control_level((number / 8)+1 , 0);
				} else if (0x61 <= Command_buf[2] && Command_buf[2] <= 0x84) {
					Control_level((number / 8)+1 , 0);
				} else {
					UsbPrintf("--error number = %d\n",number);
					number = 0;
				}
				UsbPrintf("--Command_buf[2] = 0x%X, number = %d\n", Command_buf[2], number);
				IIC_GPIO_Init((number / 8), (number % 8));//配置IIC的SCL和SDA引脚
				RTC_8825_TEMP(&sysTemp, &rtcTemp);
				UsbPrintf("--板温 %d , 芯温 %d \n",(uint8_t)sysTemp , rtcTemp);
				buf[0] = number; 
				if (sysTemp >= 0) {
					buf[1] = (int)sysTemp;
				} else {
					buf[1]= 256 + (int)sysTemp;
				}
				buf[2] = rtcTemp;
				buf[3] = rtcTemp;
				if (Command_buf[2] > 0x30) {//8825的32k信号可以直接输出
					if (Command_buf[2] > 0x60){//1HZ
						for (i = 0; i < 3; i++) {
							if (IIC_Write_multi_bytes(RTC_8825_ADDRESS, 0X0B,int_1HZ, 5) == I2C_OK) {
								HAL_Delay(2);
								Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
								break;
							}
						}
						if (i >= 3) {
							UsbPrintf("8825 chip %d i2c error!\n", number);
						}
					}else{
						for (i = 0; i < 3; i++) {//1分钟
							if (IIC_Write_multi_bytes(RTC_8825_ADDRESS, 0X0D, INT_1min, 3) == I2C_OK) {
								HAL_Delay(2);
								Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
								break;
							}
						}
						if (i >= 3) {
							UsbPrintf("8825 chip %d i2c error!\n", number);
						}
					}
				} else {
					if (IIC_Write_multi_bytes(RTC_8825_ADDRESS, 0X0D, &INT_32k, 1) == I2C_OK)
					{
						HAL_Delay(2);
						Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
					}else{
						UsbPrintf("8825 chip %d i2c error!\n", number);
					}
				}

				break;
			case 0x05://8840		//测试ok  
				Close8840_FREQ();
				UsbPrintf("--Command_buf[2] = 0x%X, number = %d\n", Command_buf[2], number);
				SPI_GPIO_Init((number / 8), (number % 8));//配置IIC的SCL和SDA引脚
				RTC_8840_TEMP(&sysTemp, &rtcTemp);
				UsbPrintf("--板温 %d , 芯温 %d \n",(uint8_t)sysTemp , rtcTemp);
				buf[0] = number; 
				if (sysTemp >= 0) {
					buf[1] = (int)sysTemp;
				} else {
					buf[1]= 256 + (int)sysTemp;
				}
				buf[2] = rtcTemp;
				buf[3] = rtcTemp;
				if (Command_buf[2] > 0x30) {//32K
					if (Command_buf[2] > 0x60){//1HZ	
						SPI_Write_multi_byte(0x0B,int_1HZ, 5);
						HAL_Delay(2);
						Enable_SPI4051((number % 8) + 1, (number / 8) + 1);		
					}else{//1分钟
						SPI_Write_multi_byte(0x0D,INT_1min, 3);
						HAL_Delay(2);
						Enable_SPI4051((number % 8) + 1, (number / 8) + 1);
					}
				} else {//32k
						SPI_Write_multi_byte(0X0D, &SD8840_32K, 1);
						HAL_Delay(2);
						Enable_SPI4051((number % 8) + 1, (number / 8) + 1);
				}
				break;
			case 0x06://8931		//测试ok  
				Close8931_FREQ();
				UsbPrintf("--Command_buf[2] = 0x%X, number = %d\n", Command_buf[2], number);
				IIC_GPIO_Init((number / 8), (number % 8));//配置IIC的SCL和SDA引脚
				RTC_8931_TEMP(&sysTemp, &rtcTemp);
				UsbPrintf("--板温 %d , 芯温 %d \n",(uint8_t)sysTemp , rtcTemp);
				buf[0] = number; 
				if (sysTemp >= 0) {
					buf[1] = (int)sysTemp;
				} else {
					buf[1]= 256 + (int)sysTemp;
				}
				buf[2] = rtcTemp;
				buf[3] = rtcTemp; 
				if (Command_buf[2] > 0x30) {//32k信号
					if (Command_buf[2] > 0x60){//1HZ
						for (i = 0; i < 3; i++) {
							if (IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X0E,SD8931_1HZ, 2) == I2C_OK) 
							{
								HAL_Delay(2);
								Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
								break;
							}
						}
						if (i >= 3) {
							UsbPrintf("8931 chip %d i2c error!\n", number);
						}
					}else{//0x30
						for (i = 0; i < 3; i++) 
						{//1分钟
								if(IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X07, SD8931_1min, 8)== I2C_OK)
								{
								 	//if(IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X0E, &SD8931_INT2, 1)== I2C_OK)
									//{
										HAL_Delay(2);
										UsbPrintf("OK !\n");
										Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
										break;
									//}
								}
			
							HAL_Delay(1);
						}
						if (i >= 3) {
							UsbPrintf("8931 chip %d i2c error!\n", number);
							}
						}
				}else //32K
					{
						if(IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X0E, SD8931_32K, 2) == I2C_OK)
						{
							HAL_Delay(2);
							Enable_IIC4051((number % 8) + 1, (number / 8) + 1);
						}else{
							UsbPrintf("8931 chip %d i2c error!\n", number);
						}
					}
				break;
			default:
				break;
		}
		HAL_Delay(10);
		HAL_UART_Transmit(&huart2, buf, 4, 1000);
		memset(Command_buf, 0, UART_RECEIVE_BUFF_NUMBER);
	}
}


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
			Command_analyse(g_DeviceRxBuff);
		}
        memset(g_DeviceRxBuff, 0, UART_RECEIVE_BUFF_NUMBER);
        device_Rx_Cnt = 0;
        g_DeviceATStatus = 0;
        deviceTick = 0;
        lastDeviceRx = 0;
	 }
}







