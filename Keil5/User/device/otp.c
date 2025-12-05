#include "config.h"

/**
功能：检查rtc类型
修改：sjm
参数：IIC_x-iic总线
返回值：TRUE-成功
**/
uint8_t Check_RTCtype(i2c_device_bus IIC_x)
{
	IIC_Start(IIC_x);

	IIC_SendByte(0x64,IIC_x);
	
	if(!IIC_WaitAck(IIC_x))
		return FALSE;
	
	IIC_SendByte(0x20,IIC_x);
	
	if(!IIC_WaitAck(IIC_x))
		return FALSE;
	
	IIC_Stop(IIC_x);

	return TRUE;

}

/**
功能：读取opt字节
修改：sjm
参数：addr-地址，*dat-读取的数据，len-长度，IIC_x-iic总线
返回值：TRUE-成功
**/
uint8_t OTP_ReadBytes(uint32_t addr,uint8_t *dat,uint32_t len,i2c_device_bus IIC_x)
{
	uint32_t i;
	
	IIC_Start(IIC_x);
	
	IIC_SendByte(0x64,IIC_x);
	if(IIC_WaitAck(IIC_x)==SLAVE_NO_ACK)
		return FALSE;
	
	if(addr>0xFF)
		IIC_SendByte(0xFF,IIC_x);
	else
		IIC_SendByte((uint8_t)addr,IIC_x);
	
	if(IIC_WaitAck(IIC_x)==SLAVE_NO_ACK)
		return FALSE;
	
	IIC_Start(IIC_x);
	
	IIC_SendByte(0x65,IIC_x);
	if(IIC_WaitAck(IIC_x)==SLAVE_NO_ACK)
		return FALSE;
	
	if(addr>0xFF)
	{
		i=addr-0xFF;
		while(i>0)
		{
			IIC_ReceiveByte(IIC_x);
			IIC_Ack(IIC_x);
			i--;
		}
	
	}
	
	for(i=1;i<len;i++)
	{
	    *dat++=IIC_ReceiveByte(IIC_x);
		IIC_Ack(IIC_x);
	}
	*dat=IIC_ReceiveByte(IIC_x);
	IIC_NoAck(IIC_x);
	
	IIC_Stop(IIC_x);

	return TRUE;
	
}


