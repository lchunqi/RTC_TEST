#include "cmd.h"


void COMMAND(void)
{
		if (Usart_RxFlag == 1)		//如果接收到数据包
		{
			
			/*将收到的数据包与预设的指令对比，以此决定将要执行的操作*/
			if (strcmp(Usart_RxPacket, "help") == 0)			
			{
				MPrint_Help();
			}else if(strcmp(Usart_RxPacket, "version") == 0)
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "time") == 0)	
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "ctr") == 0)	
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "ram") == 0)
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "testmode") == 0)	
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "id") == 0)	
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "retain") == 0)	
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "config") == 0)	
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "wenbu") == 0)	
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "capin") == 0)	
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "capout") == 0)	
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "configX") == 0)	
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "wenbuOTP") == 0)	
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "capinOTP") == 0)	
			{
				MPrint_Version();
			}else if(strcmp(Usart_RxPacket, "capoutOTP") == 0)	
			{
				MPrint_Version();
			}
			else						//上述所有条件均不满足，即收到了未知指令
			{
				MPrint_CmdFail();
				Usart_SendString("串口指令错误请重新输入！\r\n");			//串口回传一个字符串ERROR_COMMAND
			}
			
			Usart_RxFlag = 0;			//处理完成后，需要将接收数据包标志位清零，否则将无法接收后续数据包
		}

}
