#include "config.h"

/* Function	 -	SET_SDA_H
 * Brief		 -	选择一组IIC的SDA置位
 * Parameter -	iic_x SDA引脚编号
 * Return		 -	none
 * Editor		 -	
 * Date			 -	
 */
void SET_SDA_H(i2c_device_bus iic_x)
{
	switch(iic_x)
	{
		case IIC_TEST:
			SDA_TEST_H;
			break;
		case IIC_STD:
			SDA_STD_H;
			break;
		case IIC_GPS:
			SDA_GPS_H;
			break;
		case IIC_SD22:
			SDA_TEST_H;
			break;
		default:
			break;
	}
}

/* Function	 -	SET_SDA_L
 * Brief		 -	选择一组IIC的SDA拉低
 * Parameter -	iic_x SDA引脚编号
 * Return		 -	none
 * Editor		 -	
 * Date			 -	
 */
void SET_SDA_L(i2c_device_bus iic_x)
{
	switch(iic_x)
	{
		case IIC_TEST:
			SDA_TEST_L;
			break;
		case IIC_STD:
			SDA_STD_L;
			break;
		case IIC_GPS:
			SDA_GPS_L;
			break;
		case IIC_SD22:
			SDA_TEST_L;
			break;
		default:
			break;
	}
}

/* Function	 -	SET_SCL_H
 * Brief		 -	选择一组IIC的SCL置位
 * Parameter -	iic_x SCL引脚编号
 * Return		 -	none
 * Editor		 -	
 * Date			 -	
 */
void SET_SCL_H(i2c_device_bus iic_x)
{
	switch(iic_x)
	{
		case IIC_TEST:
			SCL_TEST_H;
			break;
		case IIC_STD:
			SCL_STD_H;
			break;
		case IIC_GPS:
			SCL_GPS_H;
			break;
		case IIC_SD22:
			SCL_TEST_H;
			break;
		default:
			break;
	}
}

/* Function	 -	SET_SCL_L
 * Brief		 -	选择一组IIC的SDA拉低
 * Parameter -	iic_x SCL引脚编号
 * Return		 -	none
 * Editor		 -	
 * Date			 -	
 */
void SET_SCL_L(i2c_device_bus iic_x)
{
	switch(iic_x)
	{
		case IIC_TEST:
			SCL_TEST_L;
			break;
		case IIC_STD:
			SCL_STD_L;
			break;
		case IIC_GPS:
			SCL_GPS_L;
			break;
		case IIC_SD22:
			SCL_TEST_L;
			break;
		default:
			break;
	}
}

/* Function	 -	Read_SDA
 * Brief		 -	选择一组IIC的读取SDA状态值
 * Parameter -	iic_x SDA引脚编号
 * Return		 -	SDA的高低电平信号
 * Editor		 -	
 * Date			 -	
 */
uint8_t Read_SDA(i2c_device_bus iic_x)
{
    switch(iic_x)
    {
        case IIC_TEST:
            if(SDA_TEST_read)
                return 1;
            else
                return 0;            
        case IIC_STD:
            if(SDA_STD_read)
                return 1;
            else
                return 0;
        case IIC_GPS:
            if(SDA_GPS_read)
                return 1;
            else
                return 0;
        case IIC_SD22:
            if(SDA_TEST_read)
                return 1;
            else
                return 0;
        default:
            break;
    }
    return 1;
}

/* Function	 -	IIC_Init
 * Brief		 -	IIC引脚初始化
 * Parameter -	none
 * Return		 -	none
 * Editor		 -	
 * Date			 -	
 */
void IIC_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);
	
	/*SCL*/
	GPIO_InitStructure.GPIO_Pin=SCL_TEST_Pin;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=SCL_STD_Pin;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=SCL_GPS_Pin;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	/*SDA*/
	GPIO_InitStructure.GPIO_Pin=SDA_TEST_Pin;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=SDA_STD_Pin;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=SDA_GPS_Pin;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
		
	/*BUSY*/
	GPIO_InitStructure.GPIO_Pin=BUSY;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/*GPS_EN*/
	GPIO_InitStructure.GPIO_Pin=GPS_EN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
		
	SCL_TEST_H;
	SCL_STD_H;
	SCL_GPS_H;
	
	SDA_TEST_H;
	SDA_STD_H;
	SDA_GPS_H;
	
}

/* Function	 -	IIC_Delay
 * Brief		 -	IIC延时函数
 * Parameter -	none
 * Return		 -	none
 * Editor		 -	
 * Date			 -	
 */
void IIC_Delay(void)
{
	Delay_us(5);
}

/* Function	 -	IIC_Stop
 * Brief		 -	选择一组IIC发送结束信号
 * Parameter -	iic_x IIC的编号
 * Return		 -	none
 * Editor		 -	
 * Date			 -	
 */
void IIC_Stop(i2c_device_bus iic_x)
{
	SET_SDA_L(iic_x);
	SET_SCL_H(iic_x);
	IIC_Delay();
	SET_SDA_H(iic_x);
	IIC_Delay();
}

/* Function	 -	IIC_Start
 * Brief		 -	选择一组IIC发送起始信号
 * Parameter -	iic_x IIC的编号
 * Return		 -	none
 * Editor		 -	
 * Date			 -	
 */
i2c_exit_code IIC_Start(i2c_device_bus iic_x)
{
	SET_SDA_H(iic_x);
	SET_SCL_H(iic_x);
	IIC_Delay();
	if(Read_SDA(iic_x) == 0)
	{
		IIC_Stop(iic_x);
		return IIC_FAIL;
	}
	SET_SDA_L(iic_x);
	IIC_Delay();
	SET_SCL_L(iic_x);
	
	return IIC_OK;
}

/* Function	 -	IIC_Ack
 * Brief		 -	选择一组IIC对从机发送应答信号
 * Parameter -	iic_x IIC的编号
 * Return		 -	none
 * Editor		 -	
 * Date			 -	
 */
void IIC_Ack(i2c_device_bus iic_x)
{
	SET_SCL_L(iic_x);
	SET_SDA_L(iic_x);
	IIC_Delay();
	SET_SCL_H(iic_x);
	IIC_Delay();
	SET_SCL_L(iic_x);
}

/* Function	 -	IIC_NoACK
 * Brief		 -	选择一组IIC对从机发送无应答信号
 * Parameter -	iic_x IIC的编号
 * Return		 -	none
 * Editor		 -	
 * Date			 -	
 */
void IIC_NoAck(i2c_device_bus iic_x)
{
	SET_SCL_L(iic_x);
	SET_SDA_H(iic_x);
	IIC_Delay();
	SET_SCL_H(iic_x);
	IIC_Delay();
	SET_SCL_L(iic_x);
}

/* Function	 -	IIC_WaitAck
 * Brief		 -	选择一组IIC等待从机应答信号
 * Parameter -	iic_x IIC的编号
 * Return		 -	TRUE	成功接收应答
 * 					 - FALSE	应答接收失败
 * Editor		 -	
 * Date			 -	
 */
i2c_exit_code IIC_WaitAck(i2c_device_bus iic_x)
{		
	uint8_t i;
	SET_SCL_L(iic_x);
	SET_SDA_H(iic_x);
	IIC_Delay();
	SET_SCL_H(iic_x);
	IIC_Delay();
	while(Read_SDA(iic_x))
	{
		i++;
		if (i >=  200)
		{
			IIC_Stop(iic_x);
			return SLAVE_NO_ACK;
		}
	}
	SET_SCL_L(iic_x);
	return SLAVE_ACK;
}

/* Function	 -	IIC_ReceiveByte
 * Brief		 -	选择一组IIC读取一个字节
 * Parameter -	iic_x IIC的编号
 * Return		 -	byte 	接收的IIC数据
 * Editor		 -	
 * Date			 -	
 */
uint8_t IIC_ReceiveByte(i2c_device_bus iic_x)
{	
	uint8_t i;
	uint8_t byte;
	SET_SDA_H(iic_x);
	for(i = 0;i < 8;i++)
	{
		SET_SCL_L(iic_x);
		IIC_Delay();
		SET_SCL_H(iic_x);
		IIC_Delay();
		byte <<= 1;
		if (Read_SDA(iic_x))
			byte |= 0x01;
	}
	SET_SCL_L(iic_x);
	return byte;
}

/* Function	 -	IIC_SendByte
 * Brief		 -	选择一组IIC写入一个字节
 * Parameter -	byte	发送的IIC数据
 * 					 - iic_x	IIC的编号
 * Return		 -	none
 * Editor		 -	
 * Date			 -	
 */
void IIC_SendByte(uint8_t byte, i2c_device_bus iic_x)
{
	uint8_t i;
	for(i = 0;i < 8;i++)
	{
		SET_SCL_L(iic_x);
		
		if(byte & 0x80)
			SET_SDA_H(iic_x);
		else
			SET_SDA_L(iic_x);
		byte <<= 1;
		IIC_Delay();
		SET_SCL_H(iic_x);
		IIC_Delay();
	}
	
	SET_SCL_L(iic_x);
}

/**
功能：iic读取寄存器，返回读取到的数据
修改：sjm
参数：addr-寄存器地址，bus-iic总线
返回值：mdata-返回读到的数值
**/
uint8_t RTC_ReadByte(uint8_t addr,i2c_device_bus bus)
{
	uint8_t mdata;
	
	IIC_Start(bus);

	IIC_SendByte(0x64,bus);
	
	if (IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}

	IIC_SendByte(addr,bus);
	if (IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}
	
	IIC_Start(bus);
	
	IIC_SendByte(0x65,bus);
	if (IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}
	
	mdata = IIC_ReceiveByte(bus);
	
	IIC_NoAck(bus);
	
	IIC_Stop(bus);
	
	return mdata;

}

/**
功能：iic读一个字节
修改：sjm
参数：dev_addr-设备地址，reg_addr-寄存器地址，*Data-保存读取的数据，bus-iic总线
返回值：IIC_OK-成功
**/
i2c_exit_code IIC_read_byte(uint8_t dev_addr, uint8_t reg_addr, uint16_t *Data, i2c_device_bus bus)
{
	
	if(IIC_Start(bus) != IIC_OK)
	return IIC_FAIL;
	
	IIC_SendByte(dev_addr | i2c_wr, bus);    /*写入设备的地址+写控制*/
	if (IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}
	
	IIC_SendByte(reg_addr, bus);            //要读寄存器地址
	if (IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}                
	
	if(IIC_Start(bus) != IIC_OK)						//重发开始信号
	return IIC_FAIL; 

	IIC_SendByte(dev_addr | i2c_rd, bus);   //读设备的地址+读控制
	if (IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}
	*Data = IIC_ReceiveByte(bus);          //读一个数据
	IIC_NoAck(bus);                    	 //主机发送非应答停止通信
	IIC_Stop(bus);
	
	return IIC_OK;	
}

/**
功能：iic写一个字节
修改：sjm
参数：dev_addr-设备地址，reg_addr-寄存器地址，Data-写入的数据，bus-iic总线
返回值：IIC_OK-成功
**/
i2c_exit_code IIC_write_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t Data, i2c_device_bus bus)
{

	if(IIC_Start(bus) != IIC_OK)					//I2C开始信号
	return IIC_FAIL;  
	
	IIC_SendByte(dev_addr | i2c_wr, bus);  //写入设备的地址+写控制
	if(IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}
	
	IIC_SendByte(reg_addr, bus);           //要写入寄存器地址
	if(IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}
	
	IIC_SendByte(Data, bus);           //写入的数据
	if(IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}
	
	IIC_Stop(bus);                      //I2C停止信号
	return IIC_OK;
}

/**
功能：iic读多个字节
修改：sjm
参数：dev_addr-设备地址，reg_addr-寄存器地址，*buf-保存读取的数据，length-读取长度，bus-iic总线
返回值：IIC_OK-成功
**/
i2c_exit_code IIC_read_multiple_bytes(uint8_t dev_addr,uint8_t reg_addr, uint16_t *buf, uint8_t length, i2c_device_bus bus)
{
	uint8_t i;

	if(IIC_Start(bus) != IIC_OK)
	return IIC_FAIL;
	
	IIC_SendByte(dev_addr | i2c_wr, bus);		/*send device address*/
	if(IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}
	
	IIC_SendByte(reg_addr, bus);						/*读数据起始地址*/
	if(IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}

	if(IIC_Start(bus) != IIC_OK)						/*restart and send read device address*/
	return IIC_FAIL;
	
	IIC_SendByte(dev_addr | i2c_rd, bus);
	if(IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}
	
	for (i = 0; i < length; i++)
	{
		*(buf+i) = IIC_ReceiveByte(bus);
		if(i < length-1)
		{
			IIC_Ack(bus);
		}
	}
	
	IIC_NoAck(bus);  					/*主机发送非应答结束通信*/
	
	IIC_Stop(bus);
	return  IIC_OK;

}

/**
功能：iic写多个字节
修改：sjm
参数：dev_addr-设备地址，reg_addr-寄存器地址，*buf-写入的数据，length-长度，bus-iic总线
返回值：IIC_OK-成功
**/
i2c_exit_code IIC_write_multiple_bytes(uint8_t dev_addr, uint8_t reg_addr, uint16_t *buf, uint8_t length, i2c_device_bus bus)
{
	uint8_t i;

	if(IIC_Start(bus) != IIC_OK)
	return IIC_FAIL;

	IIC_SendByte(dev_addr | i2c_wr, bus);
	if(IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}
	
	IIC_SendByte(reg_addr, bus);
	if(IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}
	
	for (i = 0; i < length; i++)
	{
		IIC_SendByte(*(buf + i), bus);
		if(IIC_WaitAck(bus) != SLAVE_ACK) 
		{
			return SLAVE_NO_ACK;
		}
	}
	
  IIC_Stop(bus);
	
	return	IIC_OK;

}
