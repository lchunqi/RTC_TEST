#include "config.h"

/**
功能：sd8563读addr
修改：sjm
参数：reg_addr-寄存器地址，*read_buf-保存读取的数据，length-长度，bus-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code SD8563_read_addr(uint8_t reg_addr, uint16_t *read_buf, uint8_t length, i2c_device_bus bus)
{
	if(IIC_read_multiple_bytes(0xA2,reg_addr,read_buf,length,bus)!=IIC_OK)
	{
		return RTC_NOACK;
	}
	return RTC_OK;
}

/**
功能：sd8563写addr
修改：sjm
参数：reg_addr-寄存器地址，*write_buf-写数据，length-长度，bus-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code SD8563_write_addr(uint8_t reg_addr, uint16_t *write_buf, uint8_t length, i2c_device_bus bus)
{
	if(IIC_write_multiple_bytes(0xA2,reg_addr,write_buf,length,bus)!=IIC_OK)
	{
		return RTC_NOACK;
	}
	return RTC_OK;
}

/**
功能：sd8563读时间数据
修改：sjm
参数：*time-结构体指针，IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code SD8563_ReadTimeDate(rtc_time *time, i2c_device_bus IIC_x)
{
	uint16_t buf[7];

	if(SD8563_read_addr(0x02,buf,7,IIC_x)!=RTC_OK)
	{
		return RTC_FAIL;
	}
	
	time->second=bcd_to_dec(buf[0] & 0x7F);
	time->minute=bcd_to_dec(buf[1]);
	time->hour=bcd_to_dec(buf[2]);
	time->day=bcd_to_dec(buf[3]);	
	time->week=bcd_to_dec(buf[4]);
	time->month=bcd_to_dec(buf[5]);	
	time->year=bcd_to_dec(buf[6]);
		
	return RTC_OK;

}

/**
功能：sd8563写时间数据
修改：sjm
参数：*time-结构体指针，IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code SD8563_WriteTimeDate(rtc_time *time, i2c_device_bus IIC_x)
{
	uint16_t buf[7];

	buf[0]=dec_to_bcd(time->second);
	buf[1]=dec_to_bcd(time->minute);
	buf[2]=dec_to_bcd(time->hour);
	buf[3]=dec_to_bcd(time->day);
	buf[4]=dec_to_bcd(time->week);
	buf[5]=dec_to_bcd(time->month);
	buf[6]=dec_to_bcd(time->year);

	if(SD8563_write_addr(0x02,buf,7,IIC_x)!=RTC_OK)
	{
		return RTC_FAIL;
	}
	
	return RTC_OK;
	
}

/**
功能：sd8563输出1Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8563_f1(i2c_device_bus IIC_x)
{	
		uint16_t buf=0x83;
		if(SD8563_write_addr(0x0D,&buf,1,IIC_x)!=RTC_OK)
			return RTC_FAIL;
	
		OLED_ShowImage(0, 0, 128, 64, wave_1);
	
		return RTC_OK;

}

/**
功能：sd8563关闭输出1Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8563_f1_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x00;
		SD8563_write_addr(0x0D,&buf,1,IIC_x);
	
		return RTC_OK;
}

/**
功能：sd8563输出32Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8563_f32(i2c_device_bus IIC_x)
{	
		uint16_t buf=0x82;
		if(SD8563_write_addr(0x0D,&buf,1,IIC_x)!=RTC_OK)
			return RTC_FAIL;
	
		OLED_ShowImage(0, 0, 128, 64, wave_32);
	
		return RTC_OK;

}

/**
功能：sd8563关闭输出32Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8563_f32_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x00;
		SD8563_write_addr(0x0D,&buf,1,IIC_x);
	
		return RTC_OK;
}

/**
功能：sd8563输出1024Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8563_f1024(i2c_device_bus IIC_x)
{	
		uint16_t buf=0x81;
		if(SD8563_write_addr(0x0D,&buf,1,IIC_x)!=RTC_OK)
			return RTC_FAIL;
	
		OLED_ShowImage(0, 0, 128, 64, wave_32768);
	
		return RTC_OK;

}

/**
功能：sd8563关闭输出1024Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8563_f1024_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x00;
		SD8563_write_addr(0x0D,&buf,1,IIC_x);
	
		return RTC_OK;
}

/**
功能：sd8563输出32768Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8563_f32768(i2c_device_bus IIC_x)
{	
		uint16_t buf=0x80;
		if(SD8563_write_addr(0x0D,&buf,1,IIC_x)!=RTC_OK)
			return RTC_FAIL;
	
		OLED_ShowImage(0, 0, 128, 64, wave_32768);
	
		return RTC_OK;

}

/**
功能：sd8563关闭输出32768Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8563_f32768_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x00;
		SD8563_write_addr(0x0D,&buf,1,IIC_x);
	
		return RTC_OK;
}

/**
功能：sd8563和标准模块时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
uint8_t sd85xx_and_std(void)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;
	
	rtc_time time_buf;
	
	sd2405showtime();
	Test_Line();
	
	if(SD8563_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
	{
			year_h = time_buf.year / 10;
			year_l = time_buf.year % 10;  //年

			month_h = time_buf.month / 10;
			month_l = time_buf.month % 10;  //月

			day_h = time_buf.day / 10;
			day_l = time_buf.day % 10;  //日

			week  = time_buf.week;

			hour_h = time_buf.hour / 10;
			hour_l = time_buf.hour % 10;  //时

			minute_h = time_buf.minute / 10;
			minute_l = time_buf.minute % 10;  //分

			second_h = time_buf.second / 10;
			second_l = time_buf.second % 10; //秒

			OLED_ShowNum(30,42,year_h,1,OLED_6X8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
			OLED_ShowNum(37,42,year_l,1,OLED_6X8);
			OLED_ShowChar(44,42,'-',OLED_6X8);
			OLED_ShowNum(51,42,month_h,1,OLED_6X8);
			OLED_ShowNum(58,42,month_l,1,OLED_6X8);
			OLED_ShowChar(65,42,'-',OLED_6X8);
			OLED_ShowNum(72,42,day_h,1,OLED_6X8);
			OLED_ShowNum(79,42,day_l,1,OLED_6X8);
			OLED_ShowChar(90,42,'W',OLED_6X8);
			OLED_ShowNum(99,42,week,1,OLED_6X8);

			OLED_ShowNum(35,53,hour_h,1,OLED_6X8);
			OLED_ShowNum(42,53,hour_l,1,OLED_6X8);
			OLED_ShowChar(49,53,':',OLED_6X8);
			OLED_ShowNum(56,53,minute_h,1,OLED_6X8);
			OLED_ShowNum(63,53,minute_l,1,OLED_6X8);
			OLED_ShowChar(70,53,':',OLED_6X8);
			OLED_ShowNum(77,53,second_h,1,OLED_6X8);
			OLED_ShowNum(84,53,second_l,1,OLED_6X8);
	
		return RTC_OK;
		
	}else{
		NO_ACK_Test_Line();
	}
	
	return TRUE;

}

/**
功能：sd8563时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
uint8_t sd85xx_time_show(void)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;
	
	rtc_time time_buf;
	
	if(SD8563_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
	{
		year_h = time_buf.year / 10;
		year_l = time_buf.year % 10;  //年

		month_h = time_buf.month / 10;
		month_l = time_buf.month % 10;  //月

		day_h = time_buf.day / 10;
		day_l = time_buf.day % 10;  //日

		week  = time_buf.week;

		hour_h = time_buf.hour / 10;
		hour_l = time_buf.hour % 10;  //时

		minute_h = time_buf.minute / 10;
		minute_l = time_buf.minute % 10;  //分

		second_h = time_buf.second / 10;
		second_l = time_buf.second % 10; //秒
		
		OLED_ShowNum(30,15,year_h,1,8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
		OLED_ShowNum(37,15,year_l,1,8);
		OLED_ShowChar(44,15,'-',8);
		OLED_ShowNum(51,15,month_h,1,8);
		OLED_ShowNum(58,15,month_l,1,8);
		OLED_ShowChar(65,15,'-',8);
		OLED_ShowNum(72,15,day_h,1,8);
		OLED_ShowNum(79,15,day_l,1,8);
		OLED_ShowChar(90,15,'W',8);
		OLED_ShowNum(99,15,week,1,8);
		
		OLED_ShowNum(35,30,hour_h,1,8);
		OLED_ShowNum(42,30,hour_l,1,8);
		OLED_ShowChar(49,30,':',8);
		OLED_ShowNum(56,30,minute_h,1,8);
		OLED_ShowNum(63,30,minute_l,1,8);
		OLED_ShowChar(70,30,':',8);
		OLED_ShowNum(77,30,second_h,1,8);
		OLED_ShowNum(84,30,second_l,1,8);
		
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
		
		return RTC_OK;
	}else{
		OLED_ShowChar(30,15,'-',8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
		OLED_ShowChar(37,15,'-',8);
		OLED_ShowChar(44,15,'-',8);
		OLED_ShowChar(51,15,'-',8);
		OLED_ShowChar(58,15,'-',8);
		OLED_ShowChar(65,15,'-',8);
		OLED_ShowChar(72,15,'-',8);
		OLED_ShowChar(79,15,'-',8);
		OLED_ShowChar(90,15,'W',8);
		OLED_ShowChar(99,15,'-',8);
		
		OLED_ShowChar(35,30,'-',8);
		OLED_ShowChar(42,30,'-',8);
		OLED_ShowChar(49,30,':',8);
		OLED_ShowChar(56,30,'-',8);
		OLED_ShowChar(63,30,'-',8);
		OLED_ShowChar(70,30,':',8);
		OLED_ShowChar(77,30,'-',8);
		OLED_ShowChar(84,30,'-',8);
	}

	return TRUE;

}

/**
功能：sd8563复制时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
uint8_t sd85xx_time_copy(void)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;
	
	rtc_time copy_buf;
	
	if(RTC_ReadTimeDate(&copy_buf,IIC_STD)!=RTC_OK)
	{
		Usart_Printf("time is error");
		return FALSE;
		
	}
	
	if(SD8563_WriteTimeDate(&copy_buf,IIC_TEST)!=RTC_FAIL)
	{
	year_h = copy_buf.year / 10;
	year_l = copy_buf.year % 10;  //年

	month_h = copy_buf.month / 10;
	month_l = copy_buf.month % 10;  //月

	day_h = copy_buf.day / 10;
	day_l = copy_buf.day % 10;  //日

	week  = copy_buf.week;

	hour_h = copy_buf.hour / 10;
	hour_l = copy_buf.hour % 10;  //时

	minute_h = copy_buf.minute / 10;
	minute_l = copy_buf.minute % 10;  //分

	second_h = copy_buf.second / 10;
	second_l = copy_buf.second % 10; //秒

	OLED_ShowNum(30,42,year_h,1,OLED_6X8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
	OLED_ShowNum(37,42,year_l,1,OLED_6X8);
	OLED_ShowChar(44,42,'-',OLED_6X8);
	OLED_ShowNum(51,42,month_h,1,OLED_6X8);
	OLED_ShowNum(58,42,month_l,1,OLED_6X8);
	OLED_ShowChar(65,42,'-',OLED_6X8);
	OLED_ShowNum(72,42,day_h,1,OLED_6X8);
	OLED_ShowNum(79,42,day_l,1,OLED_6X8);
	OLED_ShowChar(90,42,'W',OLED_6X8);
	OLED_ShowNum(99,42,week,1,OLED_6X8);

	OLED_ShowNum(35,53,hour_h,1,OLED_6X8);
	OLED_ShowNum(42,53,hour_l,1,OLED_6X8);
	OLED_ShowChar(49,53,':',OLED_6X8);
	OLED_ShowNum(56,53,minute_h,1,OLED_6X8);
	OLED_ShowNum(63,53,minute_l,1,OLED_6X8);
	OLED_ShowChar(70,53,':',OLED_6X8);
	OLED_ShowNum(77,53,second_h,1,OLED_6X8);
	OLED_ShowNum(84,53,second_l,1,OLED_6X8);
	 
 }
	
 return TRUE;
 
}


/**
功能：与上位机交互读取sd8563数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度，IIC_x-iic总线
返回值：SD8563_read_addr(addr,data,len,IIC_x)-读取到的数据
**/
uint8_t RTC_ReadRAM_8563(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x)
{

	return SD8563_read_addr(addr,data,len,IIC_x);
	
}

/**
功能：与上位机交互写sd8563数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度，IIC_x-iic总线
返回值：TRUE-成功
**/
uint8_t RTC_WriteRAM_8563(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x)
{

	 if(SD8563_write_addr(addr,data,len,IIC_x)!=RTC_OK)
		return RTC_NOACK;
	 
	 return TRUE;
	 
}

/**********************sd8573*******************************/
/**
功能：sd8573读addr
修改：sjm
参数：reg_addr-寄存器地址，*read_buf-保存读取的数据，length-长度，bus-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code SD8573_read_addr(uint8_t reg_addr, uint16_t *read_buf, uint8_t length, i2c_device_bus bus)
{
	if(IIC_read_multiple_bytes(0xA2,reg_addr,read_buf,length,bus)!=IIC_OK)
	{
		return RTC_NOACK;
	}
	return RTC_OK;

}

/**
功能：sd8573写addr
修改：sjm
参数：reg_addr-寄存器地址，*write_buf-写数据，length-长度，bus-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code SD8573_write_addr(uint8_t reg_addr, uint16_t *write_buf, uint8_t length, i2c_device_bus bus)
{
	if(IIC_write_multiple_bytes(0xA2,reg_addr,write_buf,length,bus)!=IIC_OK)
	{
		return RTC_NOACK;
	}
	return RTC_OK;
}

/**
功能：sd8573读时间数据
修改：sjm
参数：*time-结构体指针，IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code SD8573_ReadTimeDate(rtc_time *time, i2c_device_bus IIC_x)
{
	uint16_t buf[7];

	if(SD8563_read_addr(0x04,buf,7,IIC_x)!=RTC_OK)
	{
		return RTC_FAIL;
	}
	time->second=bcd_to_dec(buf[0]);
	time->minute=bcd_to_dec(buf[1]);
	time->hour=bcd_to_dec(buf[2]);
	time->day=bcd_to_dec(buf[3]);	
	time->week=bcd_to_dec(buf[4]);
	time->month=bcd_to_dec(buf[5]);	
	time->year=bcd_to_dec(buf[6]);
		
	return RTC_OK;

}

/**
功能：sd8573写时间数据
修改：sjm
参数：*time-结构体指针，IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code SD8573_WriteTimeDate(rtc_time *time, i2c_device_bus IIC_x)
{
	uint16_t buf[7];

	buf[0]=dec_to_bcd(time->second);
	buf[1]=dec_to_bcd(time->minute);
	buf[2]=dec_to_bcd(time->hour);
	buf[3]=dec_to_bcd(time->day);
	buf[4]=dec_to_bcd(time->week);
	buf[5]=dec_to_bcd(time->month);
	buf[6]=dec_to_bcd(time->year);

	if(SD8563_write_addr(0x04,buf,7,IIC_x)!=RTC_OK)
	{
		return RTC_FAIL;
	}
	
	return RTC_OK;
	
}

//与上位机交互
/**
功能：与上位机交互读取sd8573数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度，IIC_x-iic总线
返回值：SD8573_read_addr(addr,data,len,IIC_x)-读取到的数据
**/
uint8_t RTC_ReadRAM_8573(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x)
{

	return SD8573_read_addr(addr,data,len,IIC_x);
	
}

/**
功能：与上位机交互写sd8573数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度，IIC_x-iic总线
返回值：TRUE-成功
**/
uint8_t RTC_WriteRAM_8573(uint8_t addr,uint16_t *data,uint8_t len,i2c_device_bus IIC_x)
{

	 if(SD8573_write_addr(addr,data,len,IIC_x)!=RTC_OK)
		return RTC_NOACK;
	 
	 return TRUE;
	 
}

/**
功能：sd8573和标准模块时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/

uint8_t sd8573_and_std(void)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;
	
	rtc_time time_buf;
	
	sd2405showtime();
	Test_Line();
	
	if(SD8573_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
	{
			year_h = time_buf.year / 10;
			year_l = time_buf.year % 10;  //年

			month_h = time_buf.month / 10;
			month_l = time_buf.month % 10;  //月

			day_h = time_buf.day / 10;
			day_l = time_buf.day % 10;  //日

			week  = time_buf.week;

			hour_h = time_buf.hour / 10;
			hour_l = time_buf.hour % 10;  //时

			minute_h = time_buf.minute / 10;
			minute_l = time_buf.minute % 10;  //分

			second_h = time_buf.second / 10;
			second_l = time_buf.second % 10; //秒

			OLED_ShowNum(30,42,year_h,1,OLED_6X8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
			OLED_ShowNum(37,42,year_l,1,OLED_6X8);
			OLED_ShowChar(44,42,'-',OLED_6X8);
			OLED_ShowNum(51,42,month_h,1,OLED_6X8);
			OLED_ShowNum(58,42,month_l,1,OLED_6X8);
			OLED_ShowChar(65,42,'-',OLED_6X8);
			OLED_ShowNum(72,42,day_h,1,OLED_6X8);
			OLED_ShowNum(79,42,day_l,1,OLED_6X8);
			OLED_ShowChar(90,42,'W',OLED_6X8);
			OLED_ShowNum(99,42,week,1,OLED_6X8);

			OLED_ShowNum(35,53,hour_h,1,OLED_6X8);
			OLED_ShowNum(42,53,hour_l,1,OLED_6X8);
			OLED_ShowChar(49,53,':',OLED_6X8);
			OLED_ShowNum(56,53,minute_h,1,OLED_6X8);
			OLED_ShowNum(63,53,minute_l,1,OLED_6X8);
			OLED_ShowChar(70,53,':',OLED_6X8);
			OLED_ShowNum(77,53,second_h,1,OLED_6X8);
			OLED_ShowNum(84,53,second_l,1,OLED_6X8);
	
		return RTC_OK;
		
	}else{
		NO_ACK_Test_Line();
	}
	
	return TRUE;

}

/**
功能：sd8573时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
uint8_t sd8573_time_show(void)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;
	
	rtc_time time_buf;
	
	if(SD8573_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
	{
		year_h = time_buf.year / 10;
		year_l = time_buf.year % 10;  //年

		month_h = time_buf.month / 10;
		month_l = time_buf.month % 10;  //月

		day_h = time_buf.day / 10;
		day_l = time_buf.day % 10;  //日

		week  = time_buf.week;

		hour_h = time_buf.hour / 10;
		hour_l = time_buf.hour % 10;  //时

		minute_h = time_buf.minute / 10;
		minute_l = time_buf.minute % 10;  //分

		second_h = time_buf.second / 10;
		second_l = time_buf.second % 10; //秒
		
		OLED_ShowNum(30,15,year_h,1,8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
		OLED_ShowNum(37,15,year_l,1,8);
		OLED_ShowChar(44,15,'-',8);
		OLED_ShowNum(51,15,month_h,1,8);
		OLED_ShowNum(58,15,month_l,1,8);
		OLED_ShowChar(65,15,'-',8);
		OLED_ShowNum(72,15,day_h,1,8);
		OLED_ShowNum(79,15,day_l,1,8);
		OLED_ShowChar(90,15,'W',8);
		OLED_ShowNum(99,15,week,1,8);
		
		OLED_ShowNum(35,30,hour_h,1,8);
		OLED_ShowNum(42,30,hour_l,1,8);
		OLED_ShowChar(49,30,':',8);
		OLED_ShowNum(56,30,minute_h,1,8);
		OLED_ShowNum(63,30,minute_l,1,8);
		OLED_ShowChar(70,30,':',8);
		OLED_ShowNum(77,30,second_h,1,8);
		OLED_ShowNum(84,30,second_l,1,8);
		
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
		
		return RTC_OK;
	}else{
		OLED_ShowChar(30,15,'-',8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
		OLED_ShowChar(37,15,'-',8);
		OLED_ShowChar(44,15,'-',8);
		OLED_ShowChar(51,15,'-',8);
		OLED_ShowChar(58,15,'-',8);
		OLED_ShowChar(65,15,'-',8);
		OLED_ShowChar(72,15,'-',8);
		OLED_ShowChar(79,15,'-',8);
		OLED_ShowChar(90,15,'W',8);
		OLED_ShowChar(99,15,'-',8);
		
		OLED_ShowChar(35,30,'-',8);
		OLED_ShowChar(42,30,'-',8);
		OLED_ShowChar(49,30,':',8);
		OLED_ShowChar(56,30,'-',8);
		OLED_ShowChar(63,30,'-',8);
		OLED_ShowChar(70,30,':',8);
		OLED_ShowChar(77,30,'-',8);
		OLED_ShowChar(84,30,'-',8);
	}

	return TRUE;

}

/**
功能：sd8573复制时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
uint8_t sd8573_time_copy(void)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;
	
	rtc_time copy_buf;
	
	if(RTC_ReadTimeDate(&copy_buf,IIC_STD)!=RTC_OK)
	{
		Usart_Printf("time is error");
		return FALSE;
		
	}
	
	if(SD8573_WriteTimeDate(&copy_buf,IIC_TEST)!=RTC_FAIL)
	{
	year_h = copy_buf.year / 10;
	year_l = copy_buf.year % 10;  //年

	month_h = copy_buf.month / 10;
	month_l = copy_buf.month % 10;  //月

	day_h = copy_buf.day / 10;
	day_l = copy_buf.day % 10;  //日

	week  = copy_buf.week;

	hour_h = copy_buf.hour / 10;
	hour_l = copy_buf.hour % 10;  //时

	minute_h = copy_buf.minute / 10;
	minute_l = copy_buf.minute % 10;  //分

	second_h = copy_buf.second / 10;
	second_l = copy_buf.second % 10; //秒

	OLED_ShowNum(30,42,year_h,1,OLED_6X8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
	OLED_ShowNum(37,42,year_l,1,OLED_6X8);
	OLED_ShowChar(44,42,'-',OLED_6X8);
	OLED_ShowNum(51,42,month_h,1,OLED_6X8);
	OLED_ShowNum(58,42,month_l,1,OLED_6X8);
	OLED_ShowChar(65,42,'-',OLED_6X8);
	OLED_ShowNum(72,42,day_h,1,OLED_6X8);
	OLED_ShowNum(79,42,day_l,1,OLED_6X8);
	OLED_ShowChar(90,42,'W',OLED_6X8);
	OLED_ShowNum(99,42,week,1,OLED_6X8);

	OLED_ShowNum(35,53,hour_h,1,OLED_6X8);
	OLED_ShowNum(42,53,hour_l,1,OLED_6X8);
	OLED_ShowChar(49,53,':',OLED_6X8);
	OLED_ShowNum(56,53,minute_h,1,OLED_6X8);
	OLED_ShowNum(63,53,minute_l,1,OLED_6X8);
	OLED_ShowChar(70,53,':',OLED_6X8);
	OLED_ShowNum(77,53,second_h,1,OLED_6X8);
	OLED_ShowNum(84,53,second_l,1,OLED_6X8);
	 
 }
	
 return TRUE;
 
}

/**
功能：sd8573输出32768Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f32768(i2c_device_bus IIC_x)
{	
		uint16_t buf=0x00;
		if(SD8573_write_addr(0x01,&buf,1,IIC_x)!=RTC_OK)
			return RTC_FAIL;
	
		OLED_ShowImage(0, 0, 128, 64, wave_32768);
	
		return RTC_OK;

}

/**
功能：sd8573关闭输出32768Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f32768_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x07;
		SD8573_write_addr(0x01,&buf,1,IIC_x);
	
		return RTC_OK;
}

/**
功能：sd8573输出13684Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f13684(i2c_device_bus IIC_x)
{	
		uint16_t buf=0x01;
		if(SD8573_write_addr(0x01,&buf,1,IIC_x)!=RTC_OK)
			return RTC_FAIL;
	
		OLED_ShowImage(0, 0, 128, 64, wave_32768);
	
		return RTC_OK;

}

/**
功能：sd8573关闭输出13684Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f13684_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x07;
		SD8573_write_addr(0x01,&buf,1,IIC_x);
	
		return RTC_OK;
}

/**
功能：sd8573输出8192Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f8192(i2c_device_bus IIC_x)
{	
		uint16_t buf=0x02;
		if(SD8573_write_addr(0x01,&buf,1,IIC_x)!=RTC_OK)
			return RTC_FAIL;
	
		OLED_ShowImage(0, 0, 128, 64, wave_32768);
	
		return RTC_OK;

}

/**
功能：sd8573关闭输出8192Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f8192_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x07;
		SD8573_write_addr(0x01,&buf,1,IIC_x);
	
		return RTC_OK;
}

/**
功能：sd8573输出4096Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f4096(i2c_device_bus IIC_x)
{	
		uint16_t buf=0x03;
		if(SD8573_write_addr(0x01,&buf,1,IIC_x)!=RTC_OK)
			return RTC_FAIL;
	
		OLED_ShowImage(0, 0, 128, 64, wave_32768);
	
		return RTC_OK;

}

/**
功能：sd8573关闭输出4096Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f4096_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x07;
		SD8573_write_addr(0x01,&buf,1,IIC_x);
	
		return RTC_OK;
}

/**
功能：sd8573输出2048Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f2048(i2c_device_bus IIC_x)
{	
		uint16_t buf=0x04;
		if(SD8573_write_addr(0x01,&buf,1,IIC_x)!=RTC_OK)
			return RTC_FAIL;
	
		OLED_ShowImage(0, 0, 128, 64, wave_32768);
	
		return RTC_OK;

}

/**
功能：sd8573关闭输出2048Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f2048_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x07;
		SD8573_write_addr(0x01,&buf,1,IIC_x);
	
		return RTC_OK;
}

/**
功能：sd8573输出1024Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f1024(i2c_device_bus IIC_x)
{	
		uint16_t buf=0x05;
		if(SD8573_write_addr(0x01,&buf,1,IIC_x)!=RTC_OK)
			return RTC_FAIL;
	
		OLED_ShowImage(0, 0, 128, 64, wave_32768);
	
		return RTC_OK;

}

/**
功能：sd857关闭3输出1024Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f1024_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x07;
		SD8573_write_addr(0x01,&buf,1,IIC_x);
	
		return RTC_OK;
}

/**
功能：sd8573输出1Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f1(i2c_device_bus IIC_x)
{	
		uint16_t buf=0x06;
		if(SD8573_write_addr(0x01,&buf,1,IIC_x)!=RTC_OK)
			return RTC_FAIL;
	
		OLED_ShowImage(0, 0, 128, 64, wave_1);
	
		return RTC_OK;

}

/**
功能：sd8573关闭输出1Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code sd8573_f1_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x07;
		SD8573_write_addr(0x01,&buf,1,IIC_x);
	
		return RTC_OK;
}
