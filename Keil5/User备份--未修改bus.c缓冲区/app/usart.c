#include "usart.h"

char Usart_RxPacket[100];				//定义接收数据包数组，数据包格式"@MSG\r\n"
uint8_t Usart_RxFlag;					//定义接收数据包标志位

uint8_t usartFlag;
uint8_t usartRecNum;
uint16_t UsartBuffer[45];

uint8_t Get_BusFlag(void)
{
	if(usartFlag == 1)
	{
		usartFlag = 0;
		return 1;
	}
	else if(usartFlag == 2)
	{
		usartFlag = 0;
		return 2;
	}
	return 0;
}

void Get_UsartBuffer(uint8_t *p_dat)
{
	uint8_t i;
	for(i = 0; i < usartRecNum; i ++)
		p_dat[i] = UsartBuffer[i];
	usartRecNum = 0;
}


/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  */
void Usart_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					//定义gpio结构体变量
	USART_InitTypeDef USART_InitStructure;				//定义串口结构体变量
	NVIC_InitTypeDef NVIC_InitStructure;					//定义中断结构体变量

	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//开启USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA9引脚初始化为复用推挽输出
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA10引脚初始化为上拉输入
	
	/*USART初始化*/
	USART_InitStructure.USART_BaudRate = 19200;				//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制，不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//模式，发送模式和接收模式均选择
	USART_InitStructure.USART_Parity = USART_Parity_No;		//奇偶校验，不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//停止位，选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长，选择8位
	USART_Init(USART1, &USART_InitStructure);				//将结构体变量交给USART_Init，配置USART1
	
	/*中断输出配置*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//开启串口接收数据的中断
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			//配置NVIC为分组2
	
	/*NVIC配置*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//选择配置NVIC的USART1线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);							//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*USART使能*/
	USART_Cmd(USART1, ENABLE);								//使能USART1，串口开始运行
}


//串口发送一个字节数据
void Usart_SendOneByte(USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void Usart_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}

/**
  * 函    数：串口发送一个数组
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
void Usart_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		Usart_SendByte(Array[i]);		//依次调用Usart_SendByte发送每个字节数据
	}
}

/**
  * 函    数：串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void Usart_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		Usart_SendByte(String[i]);		//依次调用Usart_SendByte发送每个字节数据
	}
}

/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
uint32_t Usart_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//设置结果初值为1
	while (Y --)			//执行Y次
	{
		Result *= X;		//将X累乘到结果
	}
	return Result;
}

/**
  * 函    数：串口发送数字
  * 参    数：Number 要发送的数字，范围：0~4294967295
  * 参    数：Length 要发送数字的长度，范围：0~10
  * 返 回 值：无
  */
void Usart_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)		//根据数字长度遍历数字的每一位
	{
		Usart_SendByte(Number / Usart_Pow(10, Length - i - 1) % 10 + '0');	//依次调用Usart_SendByte发送每位数字
	}
}

/**
  * 函    数：使用printf需要重定向的底层函数
  * 参    数：保持原始格式即可，无需变动
  * 返 回 值：保持原始格式即可，无需变动
  */
int fputc(int ch, FILE *f)
{
	Usart_SendByte(ch);			//将printf的底层重定向到自己的发送字节函数
	return ch;
}

/**
  * 函    数：自己封装的prinf函数
  * 参    数：format 格式化字符串
  * 参    数：... 可变的参数列表
  * 返 回 值：无
  */
void Usart_Printf(char *format, ...)
{
	char String[100];				//定义字符数组
	va_list arg;					//定义可变参数列表数据类型的变量arg
	va_start(arg, format);			//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);					//结束变量arg
	Usart_SendString(String);		//串口发送字符数组（字符串）
}

/**
  * 函    数：USART1中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  *           函数名为预留的指定名称，可以从启动文件复制
  *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
  */
void USART1_IRQHandler(void)
{
	static uint8_t  recFlag;
	uint16_t  buffer;
	
	static uint8_t RxState = 0;		//定义表示当前状态机状态的静态变量
	static uint8_t pRxPacket = 0;	//定义表示当前接收数据位置的静态变量

	 if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	  {
		buffer = USART_ReceiveData(USART1);
			
		switch(recFlag)
		{
		case 0:
			if(buffer == 0xBC)
				recFlag = 1;
		break;
	//		
		case 1:
		if(buffer == 0xCD)	//到这里才正确判断出包头 
		{ 
			recFlag = 2; 
			usartRecNum = 0;
			usartFlag = 0; 
		} 
		else
			recFlag = 0;
		break;
	//			
		case 2:
		UsartBuffer[usartRecNum] = buffer;   
		
		switch(UsartBuffer[0])
		{
		case 0xA8: 
		case 0xBE: 
		case 0xA5:
		case 0x80:	  		//写时钟源
		case 4:
		case 2:
		case 0x15:
		case 0x11:
		usartRecNum ++;
		if(usartRecNum >= 8)             
		{
			UsartBuffer[usartRecNum] = '\0';
			recFlag = 0;
			usartFlag = 1;
		}  
		break;  
	//						
		case 0x9B:	  		//写58,68 00-13寄存器
		case 0x84:	  		//写25，2C-3F寄存器
		case 0x81:    		//写58,68 07-13寄存器
		case 6:
		usartRecNum ++;             
		if(usartRecNum >= 21) 
		{
			UsartBuffer[usartRecNum] = '\0';
			recFlag = 0;
			usartFlag = 1;
		}
		break;   
	//						
		case 0xAF:	  		//写25 60-71寄存器
		usartRecNum ++;             
		if(usartRecNum >= 19) 
		{
			UsartBuffer[usartRecNum] = '\0';
			recFlag = 0;
			usartFlag = 1;
		} 
		break; 
	//		
		case 0x82:	  		//写2058RAM寄存器
		usartRecNum ++;             
		if(usartRecNum >= 45) 
		{
			UsartBuffer[usartRecNum] = '\0';
			recFlag = 0;
			usartFlag = 1;
		} 
		break;    
	//					
		case 0x83:	  		//写2068RAM寄存器
		case 7:
		usartRecNum ++;             
		if(usartRecNum >= 13) 
		{
			UsartBuffer[usartRecNum] = '\0';
			recFlag = 0;
			usartFlag = 1;
		} 
		break;   
	//						
		case 0xBD:
		case 0xBA:	  		//写22寄存器
		usartRecNum ++;            
		if (usartRecNum >= 20) 
		{
			UsartBuffer[usartRecNum] = '\0';
			recFlag = 0;
			usartFlag = 1;
		} 
		break;    
	//					
		case 0xEA:	  		//写F型EEPROM
		usartRecNum ++;             
		if (usartRecNum >= 9) 
		{
			UsartBuffer[usartRecNum] = '\0';
			recFlag = 0;
			usartFlag = 1;
		} 
		break;     
	//
		case 0xA3:	  		//写25 00-1F RAM寄存器
		case 0xA4:  			//写25 07-1FH寄存器
		case 0xAD:  			//写25 40-5F RAM寄存器		    
		usartRecNum ++;             
		if (usartRecNum >= 33) 
		{
			UsartBuffer[usartRecNum] = '\0';
			recFlag = 0;
			usartFlag = 1;
		} 
		break;
	//					
		case 0x9A:	  		//写23寄存器
		case 0xC5:	  		//写C型EEPROM  
		case 0xD5:				//写D型EEPROM  
		case 0xE5:				//写E型EEPROM  
		case 0xE7:				//写G型EEPROM
		case 0xB5:				//写B型EEPROusartRecNum
		case 0x13:
		usartRecNum ++;             
		if(usartRecNum >= 17) 
		{
			UsartBuffer[usartRecNum] = '\0';
			recFlag = 0;
			usartFlag = 1;
		}
		break;
	
		


		
		
		
		
		
	//						
		case 0xBF:				//读24时间
		case 0xB2:				//读22时间
		case 0xF3:				//读3068ID号
		case 0xF4:				//读3068温度
		case 0xF5:				//读2500电量
		case 0xFB:				//读RAM(58)
		case 0xFC:				//读RAM(68)
		case 0xC1:				//读RAM(25)20-3FH
		case 0xAE:				//读RAM(25)40-5FH
		case 0xA7:				//读RAM(25)60-79H
		case 0xA6:				//读寄存器(25)00-1FH
		case 0xFA:				//读寄存器00-13H
		case 0xFD:				//读23寄存器00-0fH
		case 0xBB:				//读22所有寄存器
		case 0xB6:				//读B型EEPROM
		case 0xC6:				//读C型EEPROM
		case 0xD6:				//读D型EEPROM
		case 0xE6:				//读E型EEPROM
		case 0xE8:				//读G型EEPROM
		case 0x10:				//读8563时间
		case 0x12:				//读读8563寄存器00-0FH
		case 0x14:				//读8810时间
		case 1:					//读2010时间
		case 3:					//读2010 00-13H
		case 5:					//读2010 RAM

			
		recFlag = 0;
		usartRecNum = 1;
		usartFlag = 2;
		break;

					default:
					break;
						}
			break;		
			default:
		
		break;
			}
			
		switch(RxState)
		{
		case 0:
		if(buffer == '@' && Usart_RxFlag == 0)
		{
				RxState = 1;			//置下一个状态
				pRxPacket = 0;			//数据包的位置归零
		
		}
		break;
	//		
		case 1:
			if (buffer == '\r')			//如果收到第一个包尾
			{
				RxState = 2;			//置下一个状态
			}
			else						//接收到了正常的数据
			{
				Usart_RxPacket[pRxPacket] = buffer;		//将数据存入数据包数组的指定位置
				pRxPacket ++;			//数据包的位置自增
			}
		break;
	//			
		case 2:
			if (buffer == '\n')			//如果收到第二个包尾
			{
				RxState = 0;			//状态归0
				Usart_RxPacket[pRxPacket] = '\0';			//将收到的字符数据包添加一个字符串结束标志
				Usart_RxFlag = 1;		//接收数据包标志位置1，成功接收一个数据包
			}
		break;
			
				default:
				break;
			}
		
			USART_ClearITPendingBit(USART1, USART_IT_RXNE);		//清除标志位
		
		}
	
	
}


