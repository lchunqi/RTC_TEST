#include "config.h"

/**
功能：sd2201读addr
修改：sjm
参数：addr-寄存器地址，*_buf-保存读取的数据，len-长度，bus-iic总线
返回值：TRUE-成功
**/
uint8_t sd2201_read_multi_bytes(uint8_t addr,uint16_t *buf,uint8_t len,i2c_device_bus bus)
{
	uint8_t i;
	
	if(IIC_Start(bus) != IIC_OK)
		return IIC_FAIL;
	
	IIC_SendByte(addr | 0x01,bus);

	if(IIC_WaitAck(bus) != SLAVE_ACK) 
		return SLAVE_NO_ACK;

	for(i=0;i<len;i++)
	{
		*(buf+i)=IIC_ReceiveByte(bus);

		if(i!=len-1)
		  IIC_Ack(bus);
	}
	
	IIC_NoAck(bus);
	
	IIC_Stop(bus);
	
	return TRUE;
}

/**
功能：sd2201写addr
修改：sjm
参数：addr-寄存器地址，*_buf-写入的数据，len-长度，bus-iic总线
返回值：TRUE-成功
**/
uint8_t sd2201_write_multi_bytes(uint8_t addr,uint16_t *buf,uint8_t len,i2c_device_bus bus)
{
	uint8_t i;

	if(IIC_Start(bus) != IIC_OK)
	return IIC_FAIL;
	
	IIC_SendByte(addr,bus);
	
	if(IIC_WaitAck(bus) != SLAVE_ACK) 
		return SLAVE_NO_ACK;

	for(i=0;i<len;i++)
	{
		IIC_SendByte(*(buf+i),bus);
		IIC_Ack(bus);
	}
	
	IIC_Stop(bus);
	
	return TRUE;
	
}


/**
功能：sd2201寄存器前后顺序交换
修改：sjm
参数：*_buf-保存读取的数据
返回值：-
**/
void sd2201_swap(uint16_t *buf)
{
	uint16_t i;
	uint8_t buf_temp=0;
	
	for(i=0;i<8;i++)
	{
		buf_temp=buf_temp<<1;
		buf_temp=((*buf>>i)&0x01)|buf_temp;
	}
		*buf=buf_temp;
}

/**
功能：sd2201写时间数据
修改：sjm
参数：bus-iic总线
返回值：TRUE-成功
**/
uint8_t SD2201_WriteTimeDate(i2c_device_bus bus)
{
	uint16_t buf[7];
	uint8_t wet;
	
	sd2201_swap(&buf[0]);
	sd2201_swap(&buf[1]);
	sd2201_swap(&buf[2]);
	sd2201_swap(&buf[3]);
	sd2201_swap(&buf[4]);
	sd2201_swap(&buf[5]);
	sd2201_swap(&buf[6]);
	
	//写时间需要写16进制
	wet=sd2201_write_multi_bytes(0x64,buf,7,bus);
	if(wet!=TRUE)
	  return FALSE;

	return TRUE;

}

/**
功能：sd2201时间oled显示
修改：sjm
参数：bus-iic总线
返回值：TRUE-成功
**/
uint8_t sd2201_time_show(i2c_device_bus bus)
{
	uint16_t buf1=0x40;
	uint16_t buf[7];
	uint8_t ret;

	sd2201_write_multi_bytes(0x60,&buf1,1,IIC_TEST);
	
	ret=sd2201_read_multi_bytes(0x65,buf,7,bus);
	if(ret!=TRUE)
	{
	OLED_ShowString(28,16,"--",8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
	OLED_ShowChar(43,16,'-',8);
	OLED_ShowString(52,16,"--",8);
	OLED_ShowChar(67,16,'-',8);
	OLED_ShowString(75,16,"--",8);
	OLED_ShowChar(95,16,'W',8);
	OLED_ShowString(103,16,"-",8);
	
	OLED_ShowString(34,30,"--",8);
	OLED_ShowChar(49,30,':',8);
	OLED_ShowString(57,30,"--",8);
	OLED_ShowChar(72,30,':',8);
	OLED_ShowString(80,30,"--",8);
	}else{
	buf[4]&=0xFC;//时间寄存器为24小时制
	sd2201_swap(&buf[0]);
	sd2201_swap(&buf[1]);
	sd2201_swap(&buf[2]);
	sd2201_swap(&buf[3]);
	sd2201_swap(&buf[4]);
	sd2201_swap(&buf[5]);
	sd2201_swap(&buf[6]);//seconds
	
	OLED_ShowHexNum(28,16,buf[0],2,8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
	OLED_ShowChar(43,16,'-',8);
	OLED_ShowHexNum(52,16,buf[1],2,8);
	OLED_ShowChar(67,16,'-',8);
	OLED_ShowHexNum(75,16,buf[2],2,8);
	OLED_ShowChar(95,16,'W',8);
	OLED_ShowHexNum(103,16,buf[3],1,8);
	
	OLED_ShowHexNum(34,30,buf[4],2,8);
	OLED_ShowChar(49,30,':',8);
	OLED_ShowHexNum(57,30,buf[5],2,8);
	OLED_ShowChar(72,30,':',8);
	OLED_ShowHexNum(80,30,buf[6],2,8);
	
	OLED_ShowChar(0,0,'-',8);
	OLED_ShowChar(5,0,'-',8);
	OLED_ShowChar(10,0,'-',8);
	OLED_ShowChar(15,0,'-',8);
	OLED_ShowChar(20,0,'-',8);
	OLED_ShowChar(25,0,'-',8);
	OLED_ShowChar(30,0,'-',8);

	OLED_ShowString(40,0,"TEST IC",8);
	
	OLED_ShowChar(95,0,'-',8);
	OLED_ShowChar(100,0,'-',8);
	OLED_ShowChar(105,0,'-',8);
	OLED_ShowChar(110,0,'-',8);
	OLED_ShowChar(115,0,'-',8);
	OLED_ShowChar(120,0,'-',8);
	OLED_ShowChar(125,0,'-',8);
	
	}
	 
	//16进制打印显示
//	Usart_Printf("%02x年%02x月%02x日 星期%02x %02x时%02x分%02x秒\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6]);
	
	return TRUE;
	
}

/**
功能：sd2201复制时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
uint8_t sd2201_time_copy(void)
{
	uint16_t buf[7];
	rtc_time copy_buf;
	
	if(RTC_ReadTimeDate2201(&copy_buf,IIC_STD)!=RTC_OK)
	{
		Usart_Printf("time is error");
		return FALSE;
	}

	buf[6]=copy_buf.second;
	buf[5]=copy_buf.minute;
	buf[4]=copy_buf.hour & 0x7F;
	buf[3]=copy_buf.week;
	buf[2]=copy_buf.day;
	buf[1]=copy_buf.month;
	buf[0]=copy_buf.year;
	
	
	sd2201_swap(&buf[0]);
	sd2201_swap(&buf[1]);
	sd2201_swap(&buf[2]);
	sd2201_swap(&buf[3]);
	sd2201_swap(&buf[4]);
	sd2201_swap(&buf[5]);
	sd2201_swap(&buf[6]);
	
	if(SD2201_WriteTimeDate(IIC_TEST)!=TRUE)
	{
		NO_ACK_Test_Line();
		
	}else{
		
		sd2201_write_multi_bytes(0x64,buf,7,IIC_TEST);
		
	}
	
	return TRUE;
}

/**
功能：sd2201时间oled显示
修改：sjm
参数：bus-iic总线
返回值：TRUE-成功
**/
uint8_t SD2201_ReadTimeDate_Show(i2c_device_bus bus)
{
	uint16_t buf1=0x40;
	uint16_t buf[7];
	uint8_t ret;

	sd2201_write_multi_bytes(0x60,&buf1,1,IIC_TEST);
	
	ret=sd2201_read_multi_bytes(0x65,buf,7,bus);
	if(ret!=TRUE)
	  return FALSE;	
	
	buf[4]&=0xFC;
	sd2201_swap(&buf[0]);
	sd2201_swap(&buf[1]);
	sd2201_swap(&buf[2]);
	sd2201_swap(&buf[3]);
	sd2201_swap(&buf[4]);
	sd2201_swap(&buf[5]);
	sd2201_swap(&buf[6]);//seconds
	
	OLED_ShowHexNum(28,42,buf[0],2,6);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
	OLED_ShowChar(43,42,'-',6);
	OLED_ShowHexNum(52,42,buf[1],2,6);
	OLED_ShowChar(67,42,'-',6);
	OLED_ShowHexNum(75,42,buf[2],2,6);
	OLED_ShowChar(95,42,'W',6);
	OLED_ShowHexNum(103,42,buf[3],1,6);
	
	OLED_ShowHexNum(34,53,buf[4],2,6);
	OLED_ShowChar(49,53,':',6);
	OLED_ShowHexNum(57,53,buf[5],2,6);
	OLED_ShowChar(72,53,':',6);
	OLED_ShowHexNum(80,53,buf[6],2,6);

	return TRUE;

}

/**
功能：sd2201和标准模块时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
uint8_t sd2201_and_std(void)
{
	sd2405showtime();
	Test_Line();
	
	if(SD2201_ReadTimeDate_Show(IIC_TEST)!=FALSE)
	{
		SD2201_ReadTimeDate_Show(IIC_TEST);
	}else{
		
	OLED_ShowString(28,42,"--",OLED_6X8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
	OLED_ShowChar(43,42,'-',OLED_6X8);
	OLED_ShowString(52,42,"--",OLED_6X8);
	OLED_ShowChar(67,42,'-',OLED_6X8);
	OLED_ShowString(75,42,"--",OLED_6X8);
	OLED_ShowChar(95,42,'W',OLED_6X8);
	OLED_ShowChar(103,42,'-',OLED_6X8);

	OLED_ShowString(34,53,"--",OLED_6X8);
	OLED_ShowChar(49,53,':',OLED_6X8);
	OLED_ShowString(57,53,"--",OLED_6X8);
	OLED_ShowChar(72,53,':',OLED_6X8);
	OLED_ShowString(80,53,"--",OLED_6X8);	
	
	}

	return TRUE;

}

/**
功能：sd2201输出32768Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：TRUE-成功
**/
uint8_t sd2201_f32768(i2c_device_bus IIC_x)
{
	uint16_t buf=0xF0;
	if(sd2201_write_multi_bytes(0x62,&buf,1,IIC_x)!=TRUE)
		return FALSE;
	
	OLED_ShowImage(0, 0, 128, 64, wave_32768);
	
	return TRUE;
}

/**
功能：sd2201关闭输出32768Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：TRUE-成功
**/
void sd2201_f32768_off(i2c_device_bus IIC_x)
{
	uint16_t buf=0x00;
	sd2201_write_multi_bytes(0x62,&buf,1,IIC_x);
}

/**
功能：sd2201输出16Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：TRUE-成功
**/
uint8_t sd2201_f16(i2c_device_bus IIC_x)
{
	uint16_t buf=0x80;
	if(sd2201_write_multi_bytes(0x62,&buf,1,IIC_x)!=TRUE)
		return FALSE;
	
	buf=0x08;
	if(sd2201_write_multi_bytes(0x68,&buf,1,IIC_x)!=TRUE)
		return FALSE;
		
	OLED_ShowImage(0, 0, 128, 64, wave_32);
	
	return TRUE;
}

/**
功能：sd2201输出8Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：TRUE-成功
**/
uint8_t sd2201_f8(i2c_device_bus IIC_x)
{
	uint16_t buf=0x80;
	if(sd2201_write_multi_bytes(0x62,&buf,1,IIC_x)!=TRUE)
		return FALSE;
	
	buf=0x10;
	if(sd2201_write_multi_bytes(0x68,&buf,1,IIC_x)!=TRUE)
		return FALSE;
		
	OLED_ShowImage(0, 0, 128, 64, wave_8);
	
	return TRUE;
}

/**
功能：sd2201输出4Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：TRUE-成功
**/
uint8_t sd2201_f4(i2c_device_bus IIC_x)
{
	uint16_t buf=0x80;
	if(sd2201_write_multi_bytes(0x62,&buf,1,IIC_x)!=TRUE)
		return FALSE;
	
	buf=0x20;
	if(sd2201_write_multi_bytes(0x68,&buf,1,IIC_x)!=TRUE)
		return FALSE;
		
	OLED_ShowImage(0, 0, 128, 64, wave_4);
	
	return TRUE;
}

/**
功能：sd2201输出2Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：TRUE-成功
**/
uint8_t sd2201_f2(i2c_device_bus IIC_x)
{
	uint16_t buf=0x80;
	if(sd2201_write_multi_bytes(0x62,&buf,1,IIC_x)!=TRUE)
		return FALSE;
	
	buf=0x40;
	if(sd2201_write_multi_bytes(0x68,&buf,1,IIC_x)!=TRUE)
		return FALSE;
		
	OLED_ShowImage(0, 0, 128, 64, wave_4);
	
	return TRUE;
}

/**
功能：sd2201输出1Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：TRUE-成功
**/
uint8_t sd2201_f1(i2c_device_bus IIC_x)
{
	uint16_t buf=0x80;
	if(sd2201_write_multi_bytes(0x62,&buf,1,IIC_x)!=TRUE)
		return FALSE;
	
	buf=0x80;
	if(sd2201_write_multi_bytes(0x68,&buf,1,IIC_x)!=TRUE)
		return FALSE;
		
	OLED_ShowImage(0, 0, 128, 64, wave_1);
	
	return TRUE;
}

/**
功能：与上位机交互读取sd2201数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度，IIC_x-iic总线
返回值：sd2201_read_multi_bytes(addr,data,len,IIC_x)-读取到的数据
**/
uint8_t RTC_ReadRAM_2201(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x)
{

	return sd2201_read_multi_bytes(addr,data,len,IIC_x);
	
}

/**
功能：与上位机交互写sd2201数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度，IIC_x-iic总线
返回值：TRUE-成功
**/
uint8_t RTC_WriteRAM_2201(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x)
{

	 if(sd2201_write_multi_bytes(addr,data,len,IIC_x)!=TRUE)
		return FALSE;
	 
	 return TRUE;
	 
}

