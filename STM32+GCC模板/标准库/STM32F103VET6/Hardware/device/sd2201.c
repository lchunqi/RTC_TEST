#include "sd2201.h"

//uint8_t Time_DutBuf[7];

void I2C_SendBit(uint8_t bitValue) {  
    if (bitValue) 
	{  
        GPIO_SetBits(GPIOB, GPIO_Pin_7); // 设置高电平  
		
    }else{  
		
        GPIO_ResetBits(GPIOB, GPIO_Pin_7); // 设置低电平 
		
    } 
	
}

//void I2CSebdByte(uint8_t val, uint8_t High_Flg,i2c_device_bus iic_x)
//{
//	uint8_t i,temp;
//	for (i = 0; i < 8; i++)
//	{
//	   SET_SCL_L(iic_x);
//	   IIC_Delay();
//		if (High_Flg)
//		{
//		  temp = val & 0x80;
//			val = val << 1;
//		}
//		else//MCU发送数据给从机时，scl低电平阶段准备数据，从低到高上升时发送数据。
//		{
//			temp = val & 0x01;
//			val = val >> 1;
//		}
//		I2C_SendBit(temp);
//		IIC_Delay();
//		SET_SCL_H(iic_x);
//		IIC_Delay();
//	}
//	SET_SCL_L(iic_x);
//	IIC_Delay();
//}

//uint8_t I2CReceiveByte(uint8_t High_Flg,i2c_device_bus iic_x)
//{
//	uint8_t temp = 0;
//	uint8_t ret = 0;
//	uint8_t i = 0;

//	SET_SDA_H(iic_x);
//	IIC_Delay();
//	for (i = 0; i < 8; i++)
//	{
//		SET_SCL_L(iic_x);
//		IIC_Delay();
//		temp = (GPIOB->IDR & GPIO_Pin_7) ? 1 : 0; 
//		if (High_Flg)
//		{	
//			ret = ret << 1;
//			ret = ret | temp;
//		}
//		else//Mcu接收从机数据时，scl高电平阶段准备数据，从高到低下降时接收数据。
//		{
//			temp = temp << 7;
//			ret = ret >> 1;	
//			ret = ret | temp;
//		}
//		IIC_Delay();
//		SET_SCL_L(iic_x);
//		IIC_Delay();
//	}

//	return ret;
//}

uint8_t I2CReceiveByte(uint8_t order,i2c_device_bus bus)
{
	uint8_t i=8;
	uint8_t dat=0;
	SET_SDA_H(bus);
	if(order)
	{
		while(i--)
		{
			dat<<=1;
			SET_SCL_L(bus);
			IIC_Delay();
			SET_SCL_H(bus);
			IIC_Delay();
			if (Read_SDA(bus))
			{
				dat|=0x01;
			}
		}
	}else{
		while(i--)
		{
			dat>>=1;
			SET_SCL_L(bus);
			IIC_Delay();
			SET_SCL_H(bus);
			IIC_Delay();
			if (Read_SDA(bus))
			{
				dat|=0x80;
			}
		}
		
	}
	
	SET_SCL_L(bus);
	return dat;
	
}

void I2CSendByte(uint8_t demand,uint8_t order,i2c_device_bus bus)
{
	uint8_t i=8;
	if(order)
	{
		while(i--)
		{
			SET_SCL_L(bus);
			IIC_Delay();
			I2C_SendBit(demand & 0x80);
			demand<<=1;
			IIC_Delay();
			SET_SCL_H(bus);
			IIC_Delay();
		}
		SET_SCL_L(bus);
	}else{
		while(i--)
		{
			SET_SCL_L(bus);
			IIC_Delay();
			I2C_SendBit(demand & 0x01);
			demand>>=1;
			IIC_Delay();
			SET_SCL_H(bus);
			IIC_Delay();
		}
		SET_SCL_L(bus);
	}

}



//void ReadTime2200(uint8_t num,i2c_device_bus bus)
//{
//	uint8_t j = 0;
//	uint8_t data2 = 0;
//	
//	IIC_Start(bus);

//	if (num == 7)
//	{
//		I2CSebdByte(0x65,1,bus);
//	}
//	else
//	{
//		I2CSebdByte(0x67,1,bus);
//	}
//  for (j = 0; j < num; j++)
//  {
//		data2 = I2CReceiveByte(0,bus);
//		if (j < num - 1)
//		{ 
//		   IIC_Ack(bus);
//		}
//		if (num == 7)
//		{
//	    Time_DutBuf[6 - j] = data2;
//		}
//		else 
//		{
//			Time_DutBuf[2 - j] = data2;
//		}
//	  data2 = 0;
//	}
//	Time_DutBuf[4] &= 0x3F;	/*屏蔽小时寄存器第6位AM/PM*/
//	IIC_NoAck(bus);	 
//	IIC_Stop(bus);
//}

//void Display_2200Time(void)
//{
//	ReadTime2200(7,IIC_TEST);
//	

//	Usart_Printf("%d年%d月%d日 星期%d %d时%d分%d秒\n",bcd_to_dec(Time_DutBuf[0]),bcd_to_dec(Time_DutBuf[1]),bcd_to_dec(Time_DutBuf[2]),bcd_to_dec(Time_DutBuf[3]),bcd_to_dec(Time_DutBuf[4]),bcd_to_dec(Time_DutBuf[5]),bcd_to_dec(Time_DutBuf[6]));


//}


uint8_t SD2201_ReadTimeDate(i2c_device_bus bus)
{
	uint8_t n;
	uint8_t sd2201_buf[7];

	if(IIC_Start(bus) != IIC_OK)
	return IIC_FAIL;
	
	I2CSendByte(0x65,1,bus);
	
	for(n=0;n<7;n++)
	{
		sd2201_buf[n]=I2CReceiveByte(0,bus);
		if(n!=6)
		{
			IIC_Ack(bus);
		}
	
	}
	IIC_NoAck(bus);
	IIC_Stop(bus);
	
	Usart_Printf("%d年%d月%d日 星期%d %d时%d分%d秒\n",bcd_to_dec(sd2201_buf[0]),bcd_to_dec(sd2201_buf[1]),bcd_to_dec(sd2201_buf[2]),bcd_to_dec(sd2201_buf[3]),bcd_to_dec(sd2201_buf[4]),bcd_to_dec(sd2201_buf[5]),bcd_to_dec(sd2201_buf[6]));


	return TRUE;
}



