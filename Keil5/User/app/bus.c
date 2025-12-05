#include "config.h"

uint16_t BusBuffer[100];

/**
功能：向上位机发送一个字节数据
修改：sjm
参数：ack发送的数据
返回值：-
时间：2025.06.04
**/
static void Bus_SendByte(uint8_t ack)
{
	Usart_SendOneByte(USART1,ack);
}


/**
功能：向上位机发送多个字节数据，并且再发送一个结束符数据
修改：sjm
参数：len发送的长度，ack发送的结束符数据
返回值：-
时间：2025.06.04
**/
static void Bus_SendStr(uint8_t len,uint8_t ack)
{
	uint8_t i;
	
	for(i = 1; i <= len; i++)
	{
		Usart_SendOneByte(USART1,BusBuffer[i]);
	}
	Usart_SendOneByte(USART1,ack);
}

/**
功能：向上位机发送多个字节数据
修改：sjm
参数：len发送的长度
返回值：-
时间：2025.06.04
**/
static void Bus_SendStr_2200(uint8_t len)
{
	uint8_t i;
	
	for(i = 1; i <= len; i++)
	{
		Usart_SendOneByte(USART1,BusBuffer[i]);
	}
}


/**
功能：接收和处理上位机发送的数据
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void Bus_ReceData(void)
{
	uint8_t busFlag;
	uint8_t sum1;
	uint8_t sum2;

	busFlag = Get_BusFlag();

	if(busFlag == 1)//总线接收到写入命令
	{ 
		busFlag = 0;
		Get_UsartBuffer(BusBuffer);
		
		switch (BusBuffer[0])//和串口中断接收的数据包一致
		{
			//********写24寄存器00-13H*********//
			case 0x81:
				if(RTC_WriteRAM(0x00,BusBuffer+1,20,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x60);
				break;
			//********写24寄存器07-13H*********//
			case 0x9B:			
				if(RTC_WriteRAM(0x07,BusBuffer+8,13,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x61);
				break;
			//********写25寄存器00-1FH*********//
			case 0xA3:
				if(RTC_WriteRAM(0,BusBuffer+1,32,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x69);
				break;
			//********写25寄存器07-1FH*********//
			case 0xA4:											     
				if(RTC_WriteRAM(0x07,BusBuffer+8,23,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x73);
				break;
			//*******时钟源校准********//	 
			case 0xA8:					                 
				if(RTC_WriteRAM(0x00,BusBuffer+1,7,IIC_STD) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x67);
				break;
			//*******写24、23时间********//
			case 0x80:						 
			case 0xa5:
			case 0xBE:
				if(RTC_WriteRAM(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;		
			//*******写RAM(58)********//
		 	case 0x82:
	      if(RTC_WriteRAM(0x14,BusBuffer+1,44,IIC_TEST) == FALSE)
				{
					Bus_SendByte(0xEE);
					break;				
				}
				sum1 = CheckSum_uint(BusBuffer+1,44);
				if(RTC_ReadRAM(0x14,BusBuffer+1,44,IIC_TEST) == FALSE)
				{
					Bus_SendByte(0xEE);
					break;				
				}
				sum2 = CheckSum_uint(BusBuffer+1,44);
				if(sum1 == sum2)
					Bus_SendByte(0x63);
				else
					Bus_SendByte(0x64);	  
				break;
			//*******写RAM(68)********//
		  case 0x83:
			 if(RTC_WriteRAM(0x14,BusBuffer+1,12,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x63);
				break;
			//*******写2C-3F	RAM(25)********//
		  case 0x84:
			  if(RTC_WriteRAM(0x2c,BusBuffer+1,20,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x70);
				break;
			//*******写40-5F	RAM(25)********//
		  case 0xAD:
			  if(RTC_WriteRAM(0x40,BusBuffer+1,32,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x71);
				break;
			//*******写60-71	RAM(25)********//
		  case 0xAF:
			  if(RTC_WriteRAM(0x60,BusBuffer+1,18,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x72);
				break;
			//*******写23寄存器********//
	  	case 0x9A:
	      if(RTC_WriteRAM(0x70,BusBuffer+8,9,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x65);
				break;
			 //*******写8563 02-08H寄存器********//
	  	 case 0x11:
	      if(RTC_WriteRAM_8563(0x02,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x65);
				break;
			//*******写8563 00-0FH寄存器********//
	  	 case 0x13:
	      if(RTC_WriteRAM_8563(0x00,BusBuffer+1,16,IIC_TEST) == FALSE)//BusBuffer+1是为了跳过条件判断帧0x13
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x65);
				break;
/*
1，||BD CD 02 00 31 11 95 05 24 05 24||上位机发送指令，计划指令：BC CD 2(2以整数发送)
在串口助手上接收以hex显示解析得到-->BD CD 02 00 31 11 95 05 24 05 24，BusBuffer+2是为了跳过02 00 因此串口中断种接收时候是判断9位
2，旧指令BC CD 0xXX(16进制)，新指令BC CD n(10进制整数)
						|				|
						|				|
中断判断：写时间为例--> 8      			9
示例：BD CD | BF 31 11 95 05 24 05 24     BD CD | 02 00 31 11 95 05 24 05 24--BC CD为包头判断，8/9位是unsigned char(占1字节)和unsigned short int(占2字节)的区别--unsigned char/unsigned short int类型为上位机定义，单片机需要解析
3，修改发送指令类型：需要在串口助手上以hex显示看看解析出来多少位(BC CD后面跟几位)，然后在单片机中断中修改需要对应的usartRecNum数，同时在bus.c中修改BusBuffer跳过的位数---写比较特殊需要执行这一步(16进制：BusBuffer+1，10进制：BusBuffer+2)，读的话略过
*/
		 case 2:		//*******写2010时间********//			
		 case 4:		//*******写2010 00-06H********//
			 if(RTC_WriteRAM(0x00,BusBuffer+2,7,IIC_TEST) == FALSE)
					 Bus_SendByte(0xEE);
				 else
					 Bus_SendByte(0x62);
				 break;
				
			//*******写2010 00-13H********//
		 case 6:	
		  if(RTC_WriteRAM(0x00,BusBuffer+2,20,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
			//*******写2010 RAM********//
		 case 7:	
		  if(RTC_WriteRAM(0x15,BusBuffer+2,12,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;
			//*******写8810 时间********//
		 case 0x15:	
		  if(RTC_WriteRAM_8810(0x10,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;
			//*******写8810 10-16H********//
		 case 0x17:	
		  if(RTC_WriteRAM_8810(0x10,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;
			//*******写8810 10-1fH********//
		 case 0x19:	
		  if(RTC_WriteRAM_8810(0x10,BusBuffer+1,15,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;
			//*******写8810 20-5aH********//
		 case 0x21:	
		  if(RTC_WriteRAM_8810(0x20,BusBuffer+1,22,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
			//*******写8810 ram********//
		 case 0x23:	
		  if(RTC_WriteRAM_8810(0x7C,BusBuffer+1,54,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
			//*******写8810 fb-ffH********//
		 case 0x25:	
		  if(RTC_WriteRAM_8810(0xFC,BusBuffer+1,4,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
			//*******写8800 时间********//
		 case 0x37:	
		  if(RTC_WriteRAM_8800(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
			//*******写8800 00-06H********//
		 case 0x39:	
		  if(RTC_WriteRAM_8800(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
			//*******写8800 00-0fH********//
		 case 0x41:	
		  if(RTC_WriteRAM_8800(0x00,BusBuffer+1,16,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
			//*******写8800 17-5dH********//
		 case 0x43:	
		  if(RTC_WriteRAM_8800(0x17,BusBuffer+1,11,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
			//*******写8800 ram********//
		 case 0x45:	
		  if(RTC_WriteRAM_8800(0x6D,BusBuffer+1,69,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
			//*******写8800 f2-ffH********//
		 case 0x47:	
		  if(RTC_WriteRAM_8800(0xF2,BusBuffer+1,13,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;						
			//*******写8800 保留id********//
		 case 0x2F:	
		  if(RTC_WriteRAM_8800(0xBA,BusBuffer+1,32,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
			//*******写8931 时间********//
		 case 0x49:	
		  if(RTC_WriteRAM_8931(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;										
			//*******写8931 00-06H********//
		 case 0x51:	
		  if(RTC_WriteRAM_8931(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;			
			//*******写8931 00-58H********//
		 case 0x53:	
		  if(RTC_WriteRAM_8931(0x00,BusBuffer+1,20,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
			//*******写8931 6c-b9H********//
		 case 0x55:	
		  if(RTC_WriteRAM_8931(0x6C,BusBuffer+1,78,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
			//*******写8931 fb-ffH********//
		 case 0x57:	
		  if(RTC_WriteRAM_8931(0xFB,BusBuffer+1,4,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;			
			//*******写8825 时间********//
		 case 0x59:	
		  if(RTC_WriteRAM_8825(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8825 00-07H********//
		 case 0x61:	
		  if(RTC_WriteRAM_8825(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
		//*******写8825 00-5aH********//
		 case 0x63:	
		  if(RTC_WriteRAM_8825(0x00,BusBuffer+1,20,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8825 6d-b9H********//
		 case 0x65:	
		  if(RTC_WriteRAM_8825(0x6D,BusBuffer+1,78,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
		//*******写8825 fb-ffH********//
		 case 0x67:	
		  if(RTC_WriteRAM_8825(0xFB,BusBuffer+1,4,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8200 时间********//
		 case 0x69:	
		  if(RTC_WriteRAM_8200(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8200 00-06H********//
		 case 0x71:	
		  if(RTC_WriteRAM_8200(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
		//*******写8200 00-5bH********//
		 case 0x73:	
		  if(RTC_WriteRAM_8200(0x00,BusBuffer+1,17,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
		//*******写8200 6c-b9H********//
		 case 0x75:	
		  if(RTC_WriteRAM_8200(0x6C,BusBuffer+1,78,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
		//*******写8200 fb-ffH********//
		 case 0x77:	
		  if(RTC_WriteRAM_8200(0xFB,BusBuffer+1,4,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8804 时间********//
		 case 0x79:
		 case 0x3E:
		  if(RTC_WriteRAM_8804(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8804 00-0fH*********//
		 case 0x3F:
		  if(RTC_WriteRAM_8804(0x00,BusBuffer+1,16,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8804 10-5dH*********//
		 case 0x85:
		  if(RTC_WriteRAM_8804(0x10,BusBuffer+1,24,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8804 ram*********//
		 case 0x87:
		  if(RTC_WriteRAM_8804(0x6D,BusBuffer+1,69,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
		//*******写8804 fb-ffH*********//
		 case 0x4A:
		  if(RTC_WriteRAM_8804(0xFB,BusBuffer+1,4,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8804 保留id*********//
		 case 0x4B:
		  if(RTC_WriteRAM_8804(0xBA,BusBuffer+1,32,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8840 时间*********//
		 case 0x89:
		 case 0x91:
		  if(RTC_WriteRAM_8840(0x00,BusBuffer+1,7) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8840 00-0fH*********//
		 case 0x93:
		  if(RTC_WriteRAM_8840(0x00,BusBuffer+1,16) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8840 42-5aH*********//
		 case 0x95:
		  if(RTC_WriteRAM_8840(0x42,BusBuffer+1,9) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
		//*******写8840 6d-7fH*********//
		 case 0x97:
		  if(RTC_WriteRAM_8840(0x6D,BusBuffer+1,19) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8902 时间*********//
		 case 0x99:
		 case 0xA1:
		  if(RTC_WriteRAM_8902(0x80,BusBuffer+1,7) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写8902 80-90H*********//
		 case 0x4F:
		  if(RTC_WriteRAM_8902(0x80,BusBuffer+1,9) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
		//*******写8902 b2-b4H*********//
		 case 0x5A:
		  if(RTC_WriteRAM_8902(0xB2,BusBuffer+1,2) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
		//*******写8902 ba-bcH*********//
		 case 0x5B:
		  if(RTC_WriteRAM_8902(0xBA,BusBuffer+1,2) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;		
		//*******写8902 ram*********//
		 case 0x5C:
		  if(RTC_WriteRAM_8902(0xC0,BusBuffer+1,31) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;					
		//*******写2200 时间*********//
		 case 9:		 
		  if(RTC_WriteRAM_2201(0x64,BusBuffer+2,7,IIC_TEST) == FALSE)
		  {
			Bus_SendByte(0xEE);
		  }else
		  {
			sd2201_swap(BusBuffer+2);
			sd2201_swap(BusBuffer+3);
			sd2201_swap(BusBuffer+4);
			sd2201_swap(BusBuffer+5);
			sd2201_swap(BusBuffer+6);
			sd2201_swap(BusBuffer+7);
			sd2201_swap(BusBuffer+8);
			  
			sd2201_write_multi_bytes(0x64,BusBuffer+2,7,IIC_TEST);
			Bus_SendByte(0x62);
			  
		  }
				 break;			
		//*******写2200 时间1*********//
		 case 11:
		  if(RTC_WriteRAM_2201(0x64,BusBuffer+2,7,IIC_TEST) == FALSE)
		  {
			Bus_SendByte(0xEE);
		  }else
		  {
			sd2201_swap(BusBuffer+2);
			sd2201_swap(BusBuffer+3);
			sd2201_swap(BusBuffer+4);
			sd2201_swap(BusBuffer+5);
			sd2201_swap(BusBuffer+6);
			sd2201_swap(BusBuffer+7);
			sd2201_swap(BusBuffer+8);
			  
			sd2201_write_multi_bytes(0x64,BusBuffer+2,7,IIC_TEST);
			Bus_SendByte(0x62);
		  }
				break;	
		//*******写2200 写所有寄存器*********//
		 case 12:
		  if(RTC_WriteRAM_2201(0x60,BusBuffer+2,1,IIC_TEST) == FALSE)
		  {
			  Bus_SendByte(0xEE);
		  }else
		  {
			sd2201_write_multi_bytes(0x60,BusBuffer+2,1,IIC_TEST);
		  }

		  
		  if(RTC_WriteRAM_2201(0x62,BusBuffer+3,1,IIC_TEST) == FALSE)
		  {
			  Bus_SendByte(0xEE);
		  }else
		  {
			sd2201_write_multi_bytes(0x62,BusBuffer+3,1,IIC_TEST);
		  }

		  
		  if(RTC_WriteRAM_2201(0x64,BusBuffer+4,7,IIC_TEST) == FALSE)
		  {
			Bus_SendByte(0xEE);
		  }else
		  {
			sd2201_swap(BusBuffer+4);
			sd2201_swap(BusBuffer+5);
			sd2201_swap(BusBuffer+6);
			sd2201_swap(BusBuffer+7);
			sd2201_swap(BusBuffer+8);
			sd2201_swap(BusBuffer+9);
			sd2201_swap(BusBuffer+10);
			  
			sd2201_write_multi_bytes(0x64,BusBuffer+4,7,IIC_TEST);
		  }
		  
		  
		  if(RTC_WriteRAM_2201(0x68,BusBuffer+11,3,IIC_TEST) == FALSE)
		  {
			  Bus_SendByte(0xEE);
		  }else
		  {
			  if(BusBuffer[4]&0x20)
			  {
				sd2201_write_multi_bytes(0x68,BusBuffer+11,3,IIC_TEST);
			  }else if(BusBuffer[4]&0x80)
			  {
				sd2201_write_multi_bytes(0x68,BusBuffer+14,1,IIC_TEST);
			  
			  }

		  }
		  
		  
		  if(RTC_WriteRAM_2201(0x6A,BusBuffer+15,3,IIC_TEST) == FALSE)
		  {
			  Bus_SendByte(0xEE);
		  }else
		  {
			  if(BusBuffer[4]&0x02)
			  {
				sd2201_write_multi_bytes(0x6A,BusBuffer+15,3,IIC_TEST);
			  }else if(BusBuffer[4]&0x08)
			  {
				sd2201_write_multi_bytes(0x6A,BusBuffer+18,1,IIC_TEST);
			  
			  }

		  }

		  
		  if(RTC_WriteRAM_2201(0x6C,BusBuffer+19,1,IIC_TEST) == FALSE)
		  {
			  Bus_SendByte(0xEE);
		  }else
		  {
			sd2201_write_multi_bytes(0x6C,BusBuffer+19,1,IIC_TEST);
		  }		  

		  
		  if(RTC_WriteRAM_2201(0x6E,BusBuffer+20,1,IIC_TEST) == FALSE)
		  {
			  Bus_SendByte(0xEE);
		  }else
		  {
			sd2201_write_multi_bytes(0x6E,BusBuffer+20,1,IIC_TEST);
		  }		  

		  
				break;					
		 case 14:		//*******写85063 时间********//
			 if(RTC_WriteRAM_8573(0x04,BusBuffer+2,7,IIC_TEST) == FALSE)
					 Bus_SendByte(0xEE);
				 else
					 Bus_SendByte(0x62);
				 break;
		 case 254:		//*******写85063 04-0AH********//
			 if(RTC_WriteRAM_8573(0x04,BusBuffer+2,7,IIC_TEST) == FALSE)
					 Bus_SendByte(0xEE);
				 else
					 Bus_SendByte(0x62);
				 break;
		 case 255:		//*******写85063 00-11H********//
			 if(RTC_WriteRAM_8573(0x00,BusBuffer+2,18,IIC_TEST) == FALSE)
					 Bus_SendByte(0xEE);
				 else
					 Bus_SendByte(0x62);
				 break;




				
				


				
			default:
				break;
		}
		BusBuffer[0] = 0;
	}
	else if(busFlag == 2)//总线接收到读取命令
	{
		busFlag = 0;
		uint16_t buf=0;
		Get_UsartBuffer(BusBuffer);
		
		switch (BusBuffer[0])
		{
			//*******读24时间********//
			case 0xBF:
				if(RTC_ReadRAM(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;
			//*******读22时间********//	
			case 0xB2:					   
				if(RTC_ReadRAM(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;
			//*******读3068ID号********//
			case 0xF3:					  
				if(RTC_ReadID(BusBuffer+1,IIC_TEST) == FALSE)
					Bus_SendStr(8,0xEE);
				else
					Bus_SendStr(8,0xBB);
				break;
			//*******读3068温度********//
			case 0xF4:
				BusBuffer[1] = RTC_ReadTemp(IIC_TEST);
				if(BusBuffer[1] == FALSE)
					Bus_SendStr(1,0xEE);
				else
					Bus_SendStr(1,0xBB);
				break;
			//*******读2500电量********//
			case 0xF5:					   
				if(RTC_VbatQuan(BusBuffer+1,IIC_TEST) == FALSE)
					Bus_SendStr(2,0xEE);
				else
					Bus_SendStr(2,0xBB);
				break;
			//*******读RAM(58)********//
			case 0xFB:				   
				if(RTC_ReadRAM(0x14,BusBuffer+1,44,IIC_TEST) == FALSE)
					Bus_SendStr(44,0xEE);
				else
					Bus_SendStr(44,0xBB);
				break;
		 //*******读RAM(68)********//
			case 0xFC:
				if(RTC_ReadRAM(0x14,BusBuffer+1,12,IIC_TEST) == FALSE)
					Bus_SendStr(12,0xEE);
				else
					Bus_SendStr(12,0xBB);
				break;
			//*******读RAM(25)20-3FH********//
			case 0xC1:
				if(RTC_ReadRAM(0x20,BusBuffer+1,32,IIC_TEST) == FALSE)
					Bus_SendStr(32,0xEE);
				else
					Bus_SendStr(32,0xBB);
				break;
			//*******读RAM(25)40-5FH********//
			case 0xAE:
				if(RTC_ReadRAM(0x40,BusBuffer+1,32,IIC_TEST) == FALSE)
					Bus_SendStr(32,0xEE);
				else
					Bus_SendStr(32,0xBB);
				break;
			//*******读RAM(25)60-79H********//
			case 0xA7:
				if(RTC_ReadRAM(0x60,BusBuffer+1,26,IIC_TEST) == FALSE)
					Bus_SendStr(26,0xEE);
				else
					Bus_SendStr(26,0xBB);
				break;
			//******读寄存器(25)00-1FH******//
			case 0xA6:	    
				if(RTC_ReadRAM(0x00,BusBuffer+1,32,IIC_TEST) == FALSE)
					Bus_SendStr(32,0xEE);
				else
					Bus_SendStr(32,0xBB);
				break;
			//*******读寄存器00-13H*********//
			case 0xFA:	    
				if(RTC_ReadRAM(0x00,BusBuffer+1,20,IIC_TEST) == FALSE)
					Bus_SendStr(20,0xEE);
				else
					Bus_SendStr(20,0xBB);
				break;
			//*******读23寄存器00-0fH*******//
			case 0xFD:
				if(RTC_ReadRAM(0x00,BusBuffer+1,16,IIC_TEST) == FALSE)
					Bus_SendStr(16,0xEE);
				else
					Bus_SendStr(16,0xBB);
				break;
			//*******读8563时间*******//
			case 0x10:
				if(RTC_ReadRAM_8563(0x02,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xBB);
				break;
			//*******读8563寄存器00-0FH*******//
			case 0x12:
				if(RTC_ReadRAM_8563(0x00,BusBuffer+1,16,IIC_TEST) == FALSE)
					Bus_SendStr(16,0xEE);
				else
					Bus_SendStr(16,0xBB);
				break;
			//*******读2010时间*******//
			case 1:
				if(RTC_ReadRAM(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;
			//*******读2010 00-13H*******//
			case 3:
				if(RTC_ReadRAM(0x00,BusBuffer+1,20,IIC_TEST) == FALSE)
					Bus_SendStr(20,0xEE);
				else
					Bus_SendStr(20,0xCC);
				break;
			//*******读2010 RAM*******//
			case 5:
				if(RTC_ReadRAM(0x15,BusBuffer+1,12,IIC_TEST) == FALSE)
					Bus_SendStr(12,0xEE);
				else
					Bus_SendStr(12,0xCC);
				break;
			//*******读8810时间*******//
			case 0x14:
				if(RTC_ReadRAM_8810(0x10,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;
			//*******读8810 10-1FH*******//
			case 0x16:
				if(RTC_ReadRAM_8810(0x10,BusBuffer+1,16,IIC_TEST) == FALSE)
					Bus_SendStr(16,0xEE);
				else
					Bus_SendStr(16,0xCC);
				break;				
			//*******读8810 20-5AH*******//
			case 0x18:
				if(RTC_ReadRAM_8810(0x20,BusBuffer+1,22,IIC_TEST) == FALSE)
					Bus_SendStr(22,0xEE);
				else
					Bus_SendStr(22,0xCC);
				break;					
			//*******读8810 7c-b9H*******//
			case 0x20:
				if(RTC_ReadRAM_8810(0x7C,BusBuffer+1,62,IIC_TEST) == FALSE)
					Bus_SendStr(62,0xEE);
				else
					Bus_SendStr(62,0xCC);
				break;					
			//*******读8810 fb-ffH*******//
			case 0x22:
				if(RTC_ReadRAM_8810(0xFB,BusBuffer+1,4,IIC_TEST) == FALSE)
					Bus_SendStr(4,0xEE);
				else
					Bus_SendStr(4,0xCC);
				break;
			//*******读8810 id*******//
			case 0x24:
				if(RTC_ReadRAM_8810(0xB2,BusBuffer+1,8,IIC_TEST) == FALSE)
					Bus_SendStr(8,0xEE);
				else
					Bus_SendStr(8,0xCC);
				break;				
			//*******读8810 温度*******//
			case 0x1A:
				BusBuffer[1] = sd8810_ReadTemp(IIC_TEST);
				if(BusBuffer[1] == FALSE)
					Bus_SendStr(1,0xEE);
				else
					Bus_SendStr(1,0xBB);
				break;			
			//*******读8800 时间*******//
			case 0x36:
				if(RTC_ReadRAM_8800(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;					
			//*******读8800 00-0fH*******//
			case 0x38:
				if(RTC_ReadRAM_8800(0x00,BusBuffer+1,16,IIC_TEST) == FALSE)
					Bus_SendStr(16,0xEE);
				else
					Bus_SendStr(16,0xCC);
				break;					
			//*******读8800 17-5dH*******//
			case 0x40:
				if(RTC_ReadRAM_8800(0x17,BusBuffer+1,11,IIC_TEST) == FALSE)
					Bus_SendStr(11,0xEE);
				else
					Bus_SendStr(11,0xCC);
				break;					
			//*******读8800 ram*******//
			case 0x42:
				if(RTC_ReadRAM_8800(0x6D,BusBuffer+1,69,IIC_TEST) == FALSE)
					Bus_SendStr(109,0xEE);
				else
					Bus_SendStr(109,0xCC);
				break;					
			//*******读8800 f2-ffH*******//
			case 0x44:
				if(RTC_ReadRAM_8800(0xF2,BusBuffer+1,13,IIC_TEST) == FALSE)
					Bus_SendStr(13,0xEE);
				else
					Bus_SendStr(13,0xCC);
				break;					
			//*******读8800 id*******//
			case 0x46:
				if(RTC_ReadRAM_8800(0xB2,BusBuffer+1,8,IIC_TEST) == FALSE)
					Bus_SendStr(8,0xEE);
				else
					Bus_SendStr(8,0xCC);
				break;						
			//*******读8800 温度*******//
			case 0x1C:
				BusBuffer[1] = sd8800_ReadTemp(IIC_TEST);//BusBuffer[0]用于判断串口指令，因此使用BusBuffer[1]
				if(BusBuffer[1] == FALSE)
					Bus_SendStr(1,0xEE);
				else
					Bus_SendStr(1,0xBB);
				break;	
			//*******读8800 电量********//
			case 0x1D:					   
				if(sd8800_VbatQuan(BusBuffer+1,IIC_TEST) == FALSE)
					Bus_SendStr(2,0xEE);
				else
					Bus_SendStr(2,0xBB);
				break;
			//*******读8931 时间*******//
			case 0x48:
				if(RTC_ReadRAM_8931(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;			
			//*******读8931 00-58H*******//
			case 0x50:
				if(RTC_ReadRAM_8931(0x00,BusBuffer+1,20,IIC_TEST) == FALSE)
					Bus_SendStr(20,0xEE);
				else
					Bus_SendStr(20,0xCC);
				break;					
			//*******读8931 6c-b9H*******//
			case 0x52:
				if(RTC_ReadRAM_8931(0x6C,BusBuffer+1,78,IIC_TEST) == FALSE)
					Bus_SendStr(78,0xEE);
				else
					Bus_SendStr(78,0xCC);
				break;	
			//*******读8931 fb-ffH*******//
			case 0x54:
				if(RTC_ReadRAM_8931(0xFB,BusBuffer+1,4,IIC_TEST) == FALSE)
					Bus_SendStr(4,0xEE);
				else
					Bus_SendStr(4,0xCC);
				break;	
			//*******读8931 id*******//
			case 0x58:
				if(RTC_ReadRAM_8931(0xB2,BusBuffer+1,8,IIC_TEST) == FALSE)
					Bus_SendStr(8,0xEE);
				else
					Bus_SendStr(8,0xCC);
				break;
			//*******读8931 温度*******//
			case 0x1E:
				BusBuffer[1] = sd8931_ReadTemp(IIC_TEST);
				if(BusBuffer[1] == FALSE)
					Bus_SendStr(1,0xEE);
				else
					Bus_SendStr(1,0xBB);
				break;
			//*******读8825 时间*******//
			case 0x60:
				if(RTC_ReadRAM_8825(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;			
			//*******读8825 00-5aH*******//
			case 0x62:
				if(RTC_ReadRAM_8825(0x00,BusBuffer+1,20,IIC_TEST) == FALSE)
					Bus_SendStr(20,0xEE);
				else
					Bus_SendStr(20,0xCC);
				break;					
			//*******读8825 6d-b9H*******//
			case 0x64:
				if(RTC_ReadRAM_8825(0x6D,BusBuffer+1,77,IIC_TEST) == FALSE)
					Bus_SendStr(77,0xEE);
				else
					Bus_SendStr(77,0xCC);
				break;			
			//*******读8825 fb-ffH*******//
			case 0x66:
				if(RTC_ReadRAM_8825(0xFB,BusBuffer+1,4,IIC_TEST) == FALSE)
					Bus_SendStr(4,0xEE);
				else
					Bus_SendStr(4,0xCC);
				break;				
			//*******读8825 id*******//
			case 0x68:
				if(RTC_ReadRAM_8825(0xB2,BusBuffer+1,8,IIC_TEST) == FALSE)
					Bus_SendStr(8,0xEE);
				else
					Bus_SendStr(8,0xCC);
				break;					
			//*******读8825 温度*******//
			case 0x1F:
				BusBuffer[1] = sd8825_ReadTemp(IIC_TEST);
				if(BusBuffer[1] == FALSE)
					Bus_SendStr(1,0xEE);
				else
					Bus_SendStr(1,0xBB);
				break;
			//*******读8200 时间*******//
			case 0x70:
				if(RTC_ReadRAM_8200(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;				
			//*******读8200 00-5bH*******//
			case 0x72:
				if(RTC_ReadRAM_8200(0x00,BusBuffer+1,17,IIC_TEST) == FALSE)
					Bus_SendStr(16,0xEE);
				else
					Bus_SendStr(16,0xCC);
				break;					
			//*******读8200 6c-b9H*******//
			case 0x74:
				if(RTC_ReadRAM_8200(0x6C,BusBuffer+1,78,IIC_TEST) == FALSE)
					Bus_SendStr(78,0xEE);
				else
					Bus_SendStr(78,0xCC);
				break;					
			//*******读8200 fb-ffH*******//
			case 0x76:
				if(RTC_ReadRAM_8200(0xFB,BusBuffer+1,4,IIC_TEST) == FALSE)
					Bus_SendStr(4,0xEE);
				else
					Bus_SendStr(4,0xCC);
				break;				
			//*******读8200 id*******//
			case 0x78:
				if(RTC_ReadRAM_8200(0xB2,BusBuffer+1,8,IIC_TEST) == FALSE)
					Bus_SendStr(8,0xEE);
				else
					Bus_SendStr(8,0xCC);
				break;					
			//*******读8200 电量********//
			case 0x2A:					   
				if(sd8200_VbatQuan(BusBuffer+1,IIC_TEST) == FALSE)
					Bus_SendStr(2,0xEE);
				else
					Bus_SendStr(2,0xBB);
				break;				
			//*******读8804 时间*******//
			case 0x3A:
				if(RTC_ReadRAM_8804(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;			
			//*******读8804 00-06H*******//
			case 0x3B:
				if(RTC_ReadRAM_8804(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;			
			//*******读8804 00-0fH*******//
			case 0x3C:
				if(RTC_ReadRAM_8804(0x00,BusBuffer+1,16,IIC_TEST) == FALSE)
					Bus_SendStr(16,0xEE);
				else
					Bus_SendStr(16,0xCC);
				break;					
			//*******读8804 10-5dH*******//
			case 0x86:
				if(RTC_ReadRAM_8804(0x10,BusBuffer+1,24,IIC_TEST) == FALSE)
					Bus_SendStr(24,0xEE);
				else
					Bus_SendStr(24,0xCC);
				break;				
			//*******读8804 6d-d9H*******//
			case 0x3D:
				if(RTC_ReadRAM_8804(0x6D,BusBuffer+1,69,IIC_TEST) == FALSE)
					Bus_SendStr(69,0xEE);
				else
					Bus_SendStr(69,0xCC);
				break;				
			//*******读8804 id*******//
			case 0x88:
				if(RTC_ReadRAM_8804(0xB2,BusBuffer+1,8,IIC_TEST) == FALSE)
					Bus_SendStr(8,0xEE);
				else
					Bus_SendStr(8,0xCC);
				break;				
			//*******读8804 温度*******//
			case 0x2B:
				BusBuffer[1] = sd8804_ReadTemp(IIC_TEST);
				if(BusBuffer[1] == FALSE)
					Bus_SendStr(1,0xEE);
				else
					Bus_SendStr(1,0xBB);
				break;
			//*******读8840 时间*******//
			case 0x90:
				if(RTC_ReadRAM_8840(0x00,BusBuffer+1,7) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;							
			//*******读8840 00-0fH*******//
			case 0x92:
				if(RTC_ReadRAM_8840(0x00,BusBuffer+1,16) == FALSE)
					Bus_SendStr(16,0xEE);
				else
					Bus_SendStr(16,0xCC);
				break;	
			//*******读8840 42-5aH*******//
			case 0x94:
				if(RTC_ReadRAM_8840(0x42,BusBuffer+1,9) == FALSE)
					Bus_SendStr(9,0xEE);
				else
					Bus_SendStr(9,0xCC);
				break;	
			//*******读8840 6d-7fH*******//
			case 0x96:
				if(RTC_ReadRAM_8840(0x6D,BusBuffer+1,19) == FALSE)
					Bus_SendStr(19,0xEE);
				else
					Bus_SendStr(19,0xCC);
				break;	
			//*******读8840 温度*******//
			case 0x2C:
				BusBuffer[1] = sd8840_ReadTemp();
				if(BusBuffer[1] == FALSE)
					Bus_SendStr(1,0xEE);
				else
					Bus_SendStr(1,0xBB);
				break;
			//*******读8902 时间*******//
			case 0x98:
			case 0xA0:
				if(RTC_ReadRAM_8902(0x81,BusBuffer+1,7) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;	
			//*******读8902 8f-a1H*******//
			case 0xA2:
				if(RTC_ReadRAM_8902(0x8F,BusBuffer+1,10) == FALSE)
					Bus_SendStr(10,0xEE);
				else
					Bus_SendStr(10,0xCC);
				break;
			//*******读8902 af-8dH*******//
			case 0x4C:
				if(RTC_ReadRAM_8902(0xAF,BusBuffer+1,8) == FALSE)
					Bus_SendStr(8,0xEE);
				else
					Bus_SendStr(8,0xCC);
				break;
			//*******读8902 c1-fdH*******//
			case 0x4D:
				if(RTC_ReadRAM_8902(0xC1,BusBuffer+1,31) == FALSE)
					Bus_SendStr(31,0xEE);
				else
					Bus_SendStr(31,0xCC);
				break;
			//*******读8902 id*******//
			case 0x4E:
				if(RTC_ReadRAM_8902(0x93,BusBuffer+1,8) == FALSE)
					Bus_SendStr(8,0xEE);
				else
					Bus_SendStr(8,0xCC);
				break;
			//*******读8902 温度*******//
			case 0x2D:
				BusBuffer[1] = sd8902_ReadTemp();
				if(BusBuffer[1] == FALSE)
					Bus_SendStr(1,0xEE);
				else
					Bus_SendStr(1,0xBB);
				break;
			//*******读8902 电量********//
			case 0x2E:					   
				if(sd8902_VbatQuan(BusBuffer+1) == FALSE)
					Bus_SendStr(2,0xEE);
				else
					Bus_SendStr(2,0xBB);
				break;
			//*******读2201时间*******//
			case 8:
				if(RTC_ReadRAM_2201(0x65,BusBuffer+1,7,IIC_TEST) == FALSE)
				{
					Bus_SendStr(7,0xEE);
				}else{
					uint16_t buf1=0x40;
					uint16_t buf[7];
					uint8_t ret;
					sd2201_write_multi_bytes(0x60,&buf1,1,IIC_TEST);
					ret=sd2201_read_multi_bytes(0x65,buf,7,IIC_TEST);
						
						if(ret==TRUE)
						{
							buf[4]&=0xFC;
							sd2201_swap(&buf[0]);
							sd2201_swap(&buf[1]);
							sd2201_swap(&buf[2]);
							sd2201_swap(&buf[3]);
							sd2201_swap(&buf[4]);
							sd2201_swap(&buf[5]);
							sd2201_swap(&buf[6]);//seconds
						}
					
						Usart_SendOneByte(USART1,buf[0]);
						Usart_SendOneByte(USART1,buf[1]);
						Usart_SendOneByte(USART1,buf[2]);
						Usart_SendOneByte(USART1,buf[3]);
						Usart_SendOneByte(USART1,buf[4]);
						Usart_SendOneByte(USART1,buf[5]);
						Usart_SendOneByte(USART1,buf[6]);
				}
				
				break;
			//*******读2201所有寄存器*******//
			case 10:
				if(RTC_ReadRAM_2201(0x61,BusBuffer+1,1,IIC_TEST) == FALSE)
					Bus_SendStr_2200(1);
				else
					Bus_SendStr_2200(1);
				
				if(RTC_ReadRAM_2201(0x63,BusBuffer+1,1,IIC_TEST) == FALSE)
					Bus_SendStr_2200(1);
				else
					Bus_SendStr_2200(1);
								
				if(RTC_ReadRAM_2201(0x65,BusBuffer+1,7,IIC_TEST) == FALSE)
				{
					Bus_SendStr_2200(7);
				}else{
//					Bus_SendStr_2200(7);
					
					uint16_t buf1=0x40;
					uint16_t buf[7];
					uint8_t ret;
					sd2201_write_multi_bytes(0x60,&buf1,1,IIC_TEST);
					ret=sd2201_read_multi_bytes(0x65,buf,7,IIC_TEST);
						
						if(ret==TRUE)
						{
							buf[4]&=0xFC;
							sd2201_swap(&buf[0]);
							sd2201_swap(&buf[1]);
							sd2201_swap(&buf[2]);
							sd2201_swap(&buf[3]);
							sd2201_swap(&buf[4]);
							sd2201_swap(&buf[5]);
							sd2201_swap(&buf[6]);//seconds
						}
					
						Usart_SendOneByte(USART1,buf[0]);
						Usart_SendOneByte(USART1,buf[1]);
						Usart_SendOneByte(USART1,buf[2]);
						Usart_SendOneByte(USART1,buf[3]);
						Usart_SendOneByte(USART1,buf[4]);
						Usart_SendOneByte(USART1,buf[5]);
						Usart_SendOneByte(USART1,buf[6]);
						
					}
				
					
				buf=0x20;
				if(sd2201_write_multi_bytes(0x62,&buf,1,IIC_TEST)!=TRUE)
					return ;
					
				if(RTC_ReadRAM_2201(0x69,BusBuffer+1,3,IIC_TEST) == FALSE)
					Bus_SendStr_2200(3);
				else				
					Bus_SendStr_2200(3);

				
				buf=0x80;
				if(sd2201_write_multi_bytes(0x62,&buf,1,IIC_TEST)!=TRUE)
					return ;
						
				if(RTC_ReadRAM_2201(0x69,BusBuffer+1,1,IIC_TEST) == FALSE)
					Bus_SendStr_2200(1);
				else				
					Bus_SendStr_2200(1);
				
				
				buf=0x02;
				if(sd2201_write_multi_bytes(0x62,&buf,1,IIC_TEST)!=TRUE)
					return ;
					
				if(RTC_ReadRAM_2201(0x6B,BusBuffer+1,3,IIC_TEST) == FALSE)
					Bus_SendStr_2200(3);
				else				
					Bus_SendStr_2200(3);

				
				buf=0x08;
				if(sd2201_write_multi_bytes(0x62,&buf,1,IIC_TEST)!=TRUE)
					return ;
						
				if(RTC_ReadRAM_2201(0x6B,BusBuffer+1,1,IIC_TEST) == FALSE)
					Bus_SendStr_2200(1);
				else				
					Bus_SendStr_2200(1);

				
				if(RTC_ReadRAM_2201(0x6D,BusBuffer+1,1,IIC_TEST) == FALSE)
					Bus_SendStr_2200(1);
				else
					Bus_SendStr_2200(1);
				

				if(RTC_ReadRAM_2201(0x6F,BusBuffer+1,1,IIC_TEST) == FALSE)
					Bus_SendStr_2200(1);
				else
					Bus_SendStr_2200(1);
				
				break;				
			//*******读85063时间*******//
			case 13:
				if(RTC_ReadRAM_8573(0x04,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xBB);
				break;
			//*******读85063时间*******//
			case 15:
				if(RTC_ReadRAM_8573(0x04,BusBuffer+1,18,IIC_TEST) == FALSE)
					Bus_SendStr(18,0xEE);
				else
					Bus_SendStr(18,0xBB);
				break;				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
			default:
				break;
		}
		BusBuffer[0] = 0;
	}
}



