#include "bus.h"
#include "rtc.h"
#include "spi.h"
#include "sd3068.h"
#include "sd8563.h"
#include "sd8931.h"
#include "sd8200.h"
#include "sd8939.h"
#include "sd8810.h"
#include "sd8804.h"
#include "sd8800.h"
#include "sd8825.h"



uint8_t BusBuffer[45];

static void Bus_SendByte(uint8_t ack)
{
	Usart_SendOneByte(USART1,ack);
}


static void Bus_SendStr(uint8_t len,uint8_t ack)
{
	uint8_t i;
	
	for(i = 1; i <= len; i++)
	{
		Usart_SendOneByte(USART1,BusBuffer[i]);
	}
	Usart_SendOneByte(USART1,ack);
}




void Bus_ReceData(void)
{
	uint8_t busFlag;
	uint8_t sum1;
	uint8_t sum2;

	busFlag = Get_BusFlag();

	if(busFlag == 1)//×ÜÏß½ÓÊÕµ½Ð´ÈëÃüÁî
	{ 
		busFlag = 0;
		Get_UsartBuffer(BusBuffer);
		
		switch (BusBuffer[0])//ºÍ´®¿ÚÖÐ¶Ï½ÓÊÕµÄÊý¾Ý°üÒ»ÖÂ
		{
			//********Ð´24¼Ä´æÆ÷00-13H*********//
			case 0x81:
				if(RTC_WriteRAM(0x00,BusBuffer+1,20,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x60);
				break;
			//********Ð´24¼Ä´æÆ÷07-13H*********//
			case 0x9B:			
				if(RTC_WriteRAM(0x07,BusBuffer+8,13,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x61);
				break;
			//********Ð´25¼Ä´æÆ÷00-1FH*********//
			case 0xA3:
				if(RTC_WriteRAM(0,BusBuffer+1,32,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x69);
				break;
			//********Ð´25¼Ä´æÆ÷07-1FH*********//
			case 0xA4:											     
				if(RTC_WriteRAM(0x07,BusBuffer+8,23,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x73);
				break;
			//*******Ê±ÖÓÔ´Ð£×¼********//	 
			case 0xA8:					                 
				if(RTC_WriteRAM(0x00,BusBuffer+1,7,IIC_STD) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x67);
				break;
			//*******Ð´24¡¢23Ê±¼ä********//
			case 0x80:						 
			case 0xa5:
			case 0xBE:	
			case 2:		//*******Ð´2010Ê±¼ä********//
			case 4:		//*******Ð´2010 00-06H********//
				if(RTC_WriteRAM(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;		
			//*******Ð´RAM(58)********//
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
			//*******Ð´RAM(68)********//
		  case 0x83:
			 if(RTC_WriteRAM(0x14,BusBuffer+1,12,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x63);
				break;
			//*******Ð´2C-3F	RAM(25)********//
		  case 0x84:
			  if(RTC_WriteRAM(0x2c,BusBuffer+1,20,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x70);
				break;
			//*******Ð´40-5F	RAM(25)********//
		  case 0xAD:
			  if(RTC_WriteRAM(0x40,BusBuffer+1,32,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x71);
				break;
			//*******Ð´60-71	RAM(25)********//
		  case 0xAF:
			  if(RTC_WriteRAM(0x60,BusBuffer+1,18,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x72);
				break;
			//*******Ð´23¼Ä´æÆ÷********//
	  	case 0x9A:
	      if(RTC_WriteRAM(0x70,BusBuffer+8,9,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x65);
				break;
			 //*******Ð´8563 02-08H¼Ä´æÆ÷********//
	  	 case 0x11:
	      if(RTC_WriteRAM_8563(0x02,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x65);
				break;
			//*******Ð´8563 00-0FH¼Ä´æÆ÷********//
	  	 case 0x13:
	      if(RTC_WriteRAM_8563(0x00,BusBuffer+1,16,IIC_TEST) == FALSE)//BusBuffer+1ÊÇÎªÁËÌø¹ýÌõ¼þÅÐ¶ÏÖ¡
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x65);
				break;
					
			//*******Ð´2010 00-13H********//
		 case 6:	
		  if(RTC_WriteRAM(0x00,BusBuffer+1,20,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;				
			//*******Ð´2010 RAM********//
		 case 7:	
		  if(RTC_WriteRAM(0x15,BusBuffer+1,12,IIC_TEST) == FALSE)
					Bus_SendByte(0xEE);
				else
					Bus_SendByte(0x62);
				break;


				
				
				
			default:
				break;
		}
		BusBuffer[0] = 0;
	}
	else if(busFlag == 2)//×ÜÏß½ÓÊÕµ½¶ÁÈ¡ÃüÁî
	{
		busFlag = 0;
		Get_UsartBuffer(BusBuffer);
		
		switch (BusBuffer[0])
		{
			//*******¶Á24Ê±¼ä********//
			case 0xBF:
				if(RTC_ReadRAM(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;
			//*******¶Á22Ê±¼ä********//	
			case 0xB2:					   
				if(RTC_ReadRAM(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;
			//*******¶Á3068IDºÅ********//
			case 0xF3:					  
				if(RTC_ReadID(BusBuffer+1,IIC_TEST) == FALSE)
					Bus_SendStr(8,0xEE);
				else
					Bus_SendStr(8,0xBB);
				break;
			//*******¶Á3068ÎÂ¶È********//
			case 0xF4:
				BusBuffer[1] = RTC_ReadTemp(IIC_TEST);
				if(BusBuffer[1] == FALSE)
					Bus_SendStr(1,0xEE);
				else
					Bus_SendStr(1,0xBB);
				break;
			//*******¶Á2500µçÁ¿********//
			case 0xF5:					   
				if(RTC_VbatQuan(BusBuffer+1,IIC_TEST) == FALSE)
					Bus_SendStr(2,0xEE);
				else
					Bus_SendStr(2,0xBB);
				break;
			//*******¶ÁRAM(58)********//
			case 0xFB:				   
				if(RTC_ReadRAM(0x14,BusBuffer+1,44,IIC_TEST) == FALSE)
					Bus_SendStr(44,0xEE);
				else
					Bus_SendStr(44,0xBB);
				break;
		 //*******¶ÁRAM(68)********//
			case 0xFC:
				if(RTC_ReadRAM(0x14,BusBuffer+1,12,IIC_TEST) == FALSE)
					Bus_SendStr(12,0xEE);
				else
					Bus_SendStr(12,0xBB);
				break;
			//*******¶ÁRAM(25)20-3FH********//
			case 0xC1:
				if(RTC_ReadRAM(0x20,BusBuffer+1,32,IIC_TEST) == FALSE)
					Bus_SendStr(32,0xEE);
				else
					Bus_SendStr(32,0xBB);
				break;
			//*******¶ÁRAM(25)40-5FH********//
			case 0xAE:
				if(RTC_ReadRAM(0x40,BusBuffer+1,32,IIC_TEST) == FALSE)
					Bus_SendStr(32,0xEE);
				else
					Bus_SendStr(32,0xBB);
				break;
			//*******¶ÁRAM(25)60-79H********//
			case 0xA7:
				if(RTC_ReadRAM(0x60,BusBuffer+1,26,IIC_TEST) == FALSE)
					Bus_SendStr(26,0xEE);
				else
					Bus_SendStr(26,0xBB);
				break;
			//******¶Á¼Ä´æÆ÷(25)00-1FH******//
			case 0xA6:	    
				if(RTC_ReadRAM(0x00,BusBuffer+1,32,IIC_TEST) == FALSE)
					Bus_SendStr(32,0xEE);
				else
					Bus_SendStr(32,0xBB);
				break;
			//*******¶Á¼Ä´æÆ÷00-13H*********//
			case 0xFA:	    
				if(RTC_ReadRAM(0x00,BusBuffer+1,20,IIC_TEST) == FALSE)
					Bus_SendStr(20,0xEE);
				else
					Bus_SendStr(20,0xBB);
				break;
			//*******¶Á23¼Ä´æÆ÷00-0fH*******//
			case 0xFD:
				if(RTC_ReadRAM(0x00,BusBuffer+1,16,IIC_TEST) == FALSE)
					Bus_SendStr(16,0xEE);
				else
					Bus_SendStr(16,0xBB);
				break;
			//*******¶Á8563Ê±¼ä*******//
			case 0x10:
				if(RTC_ReadRAM_8563(0x02,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xBB);
				break;				
			//*******¶Á8563¼Ä´æÆ÷00-0FH*******//
			case 0x12:
				if(RTC_ReadRAM_8563(0x00,BusBuffer+1,16,IIC_TEST) == FALSE)
					Bus_SendStr(16,0xEE);
				else
					Bus_SendStr(16,0xBB);
				break;
			//*******¶Á2010Ê±¼ä*******//
			case 1:
				if(RTC_ReadRAM(0x00,BusBuffer+1,7,IIC_TEST) == FALSE)
					Bus_SendStr(7,0xEE);
				else
					Bus_SendStr(7,0xCC);
				break;
			//*******¶Á2010 00-13H*******//
			case 3:
				if(RTC_ReadRAM(0x00,BusBuffer+1,20,IIC_TEST) == FALSE)
					Bus_SendStr(20,0xEE);
				else
					Bus_SendStr(20,0xCC);
				break;
			//*******¶Á2010 RAM*******//
			case 5:
				if(RTC_ReadRAM(0x15,BusBuffer+1,12,IIC_TEST) == FALSE)
					Bus_SendStr(12,0xEE);
				else
					Bus_SendStr(12,0xCC);
				break;

				
				
				
				
				
			default:
				break;
		}  
		BusBuffer[0] = 0;
	}
}



