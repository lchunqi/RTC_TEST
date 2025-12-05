#include "sd8939.h"

uint8_t  sd8939_temp;


//rtc读addr
rtc_exit_code SD8939_read_addr(uint8_t reg_addr, uint8_t *read_buf, uint8_t length, i2c_device_bus bus)
{
	if(IIC_read_multiple_bytes(0xD0,reg_addr,read_buf,length,bus)!=IIC_OK)
	{
		return RTC_NOACK;
	}
	return RTC_OK;
}

//rtc写addr
rtc_exit_code SD8939_write_addr(uint8_t reg_addr, uint8_t *write_buf, uint8_t length, i2c_device_bus bus)
{
	if(IIC_write_multiple_bytes(0xD0,reg_addr,write_buf,length,bus)!=IIC_OK)
	{
		return RTC_NOACK;
	}
	return RTC_OK;
}


rtc_exit_code SD8939_ReadTimeDate(rtc_time *time, i2c_device_bus IIC_x)
{
	uint8_t buf[7];

	if(SD8939_read_addr(0x00,buf,7,IIC_x)!=RTC_OK)
	{
		return RTC_FAIL;
	}
	
	time->second=bcd_to_dec(buf[0]);
	time->minute=bcd_to_dec(buf[1]);
	time->hour=bcd_to_dec(buf[2] & 0x3F);
	time->week=bcd_to_dec(buf[3]);	
	time->day=bcd_to_dec(buf[4]);
	time->month=bcd_to_dec(buf[5]);	
	time->year=bcd_to_dec(buf[6]);
		
	return RTC_OK;

}

rtc_exit_code SD8939_WriteTimeDate(rtc_time *time, i2c_device_bus IIC_x)
{
	uint8_t buf[7];

	buf[0]=dec_to_bcd(time->second);
	buf[1]=dec_to_bcd(time->minute);
	buf[2]=dec_to_bcd(time->hour);
	buf[3]=dec_to_bcd(time->week);
	buf[4]=dec_to_bcd(time->day);
	buf[5]=dec_to_bcd(time->month);
	buf[6]=dec_to_bcd(time->year);

	if(SD8939_write_addr(0x00,buf,7,IIC_x)!=RTC_OK)
	{
		return RTC_FAIL;
	}
	
	return RTC_OK;
	
}

//读取芯片id码
uint8_t sd8939_idbuff(uint8_t *Data, i2c_device_bus IIC_x)
{
	if(IIC_read_multiple_bytes(0xD0,0xB2,Data,8,IIC_x)!=IIC_OK)
		return SLAVE_NO_ACK;

	return TRUE;
}

uint8_t sd8939_id_show(i2c_device_bus IIC_x)
{
		uint8_t ifbuff[8];
				
		if(sd8939_idbuff(ifbuff,IIC_x)!=TRUE)
			return FALSE;

	uint8_t id0 = ((ifbuff[0] & 0xF0) >> 4) * 10 + (ifbuff[0] & 0x0F);
	uint8_t id1 = ((ifbuff[1] & 0xF0) >> 4) * 10 + (ifbuff[1] & 0x0F);
	uint8_t id2 = ((ifbuff[2] & 0xF0) >> 4) * 10 + (ifbuff[2] & 0x0F);
	uint8_t id3 = ((ifbuff[3] & 0xF0) >> 4) * 10 + (ifbuff[3] & 0x0F);
	uint8_t id4 = ((ifbuff[4] & 0xF0) >> 4) * 10 + (ifbuff[4] & 0x0F);
	uint8_t id5 = ((ifbuff[5] & 0xF0) >> 4) * 10 + (ifbuff[5] & 0x0F);
	uint8_t id6 = ((ifbuff[6] & 0xF0) >> 4) * 10 + (ifbuff[6] & 0x0F);
	uint8_t id7 = ((ifbuff[7] & 0xF0) >> 4) * 10 + (ifbuff[7] & 0x0F);

	OLED_ShowString(0,55,"ID",6);
	OLED_ShowString(12,55,":",6);
	
	OLED_ShowNum(17,55,id0,2,6);
	OLED_ShowNum(31,55,id1,2,6);
	OLED_ShowNum(45,55,id2,2,6);
	OLED_ShowNum(60,55,id3,2,6);
	OLED_ShowNum(74,55,id4,2,6);
	OLED_ShowNum(88,55,id5,2,6);
	OLED_ShowNum(102,55,id6,2,6);
	OLED_ShowNum(116,55,id7,2,6);

		return 0;

}


uint8_t sd8939_ReadTemp(i2c_device_bus IIC_x)
{
	uint8_t	Temp1;
	
	uint8_t buf = 0x10;
	if(SD8939_write_addr(0x56,&buf,1,IIC_x)!=RTC_OK)
		return RTC_NOACK;
					
	Temp1 = RTC_ReadByte(0x56,IIC_x);
	if(Temp1 & 0x80)
	{
		Temp1 = (~Temp1+1)&0xFF;
		sd8939_temp = Temp1;
		bcd_to_dec(sd8939_temp);
		sd8939_temp = 256-sd8939_temp;
		OLED_ShowChar(30,43,'-',6);
	}else{
		Temp1 = Temp1 & 0x7F;
		sd8939_temp = Temp1;
		bcd_to_dec(sd8939_temp);
	}

	return sd8939_temp;
}


rtc_exit_code sd8939_f32768(i2c_device_bus IIC_x)
{	
		uint8_t buf=0x1C;
		SD8939_write_addr(0x0E,&buf,1,IIC_x);
	
		return RTC_OK;

}

rtc_exit_code sd8939_f32768_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x10;
		SD8939_write_addr(0x0E,&buf,1,IIC_x);
	
		return RTC_OK;
}

rtc_exit_code sd8939_f8192(i2c_device_bus IIC_x)
{	
		uint8_t buf=0x14;
		SD8939_write_addr(0x0E,&buf,1,IIC_x);
	
		return RTC_OK;

}

rtc_exit_code sd8939_f8192_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x08;
		SD8939_write_addr(0x0E,&buf,1,IIC_x);
	
		return RTC_OK;
}

rtc_exit_code sd8939_f4096(i2c_device_bus IIC_x)
{	
		uint8_t buf=0x0C;
		SD8939_write_addr(0x0E,&buf,1,IIC_x);
	
		return RTC_OK;

}

rtc_exit_code sd8939_f4096_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x00;
		SD8939_write_addr(0x0E,&buf,1,IIC_x);
	
		return RTC_OK;
}

rtc_exit_code sd8939_f1(i2c_device_bus IIC_x)
{	
		uint8_t buf=0x00;
		SD8939_write_addr(0x0E,&buf,1,IIC_x);
	
		return RTC_OK;

}

rtc_exit_code sd8939_f1_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x04;
		SD8939_write_addr(0x0E,&buf,1,IIC_x);
	
		return RTC_OK;
}

uint8_t sd8939_and_std(void)
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
	
	if(SD8939_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
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

uint8_t sd8939_time_show(void)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;
	
	rtc_time time_bufstd;
	
	if(SD8939_ReadTimeDate(&time_bufstd,IIC_TEST)!=RTC_FAIL)
	{
		year_h = time_bufstd.year / 10;
		year_l = time_bufstd.year % 10;  //年

		month_h = time_bufstd.month / 10;
		month_l = time_bufstd.month % 10;  //月

		day_h = time_bufstd.day / 10;
		day_l = time_bufstd.day % 10;  //日

		week  = time_bufstd.week;
		
		hour_h = time_bufstd.hour / 10;
		hour_l = time_bufstd.hour % 10;  //时

		minute_h = time_bufstd.minute / 10;
		minute_l = time_bufstd.minute % 10;  //分

		second_h = time_bufstd.second / 10;
		second_l = time_bufstd.second % 10; //秒
		
		OLED_ShowNum(30,8,year_h,1,8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
		OLED_ShowNum(37,8,year_l,1,8);
		OLED_ShowChar(44,8,'-',8);
		OLED_ShowNum(51,8,month_h,1,8);
		OLED_ShowNum(58,8,month_l,1,8);
		OLED_ShowChar(65,8,'-',8);
		OLED_ShowNum(72,8,day_h,1,8);
		OLED_ShowNum(79,8,day_l,1,8);
		OLED_ShowChar(90,8,'W',8);
		OLED_ShowNum(99,8,week,1,8);
		
		OLED_ShowNum(35,22,hour_h,1,8);
		OLED_ShowNum(42,22,hour_l,1,8);
		OLED_ShowChar(49,22,':',8);
		OLED_ShowNum(56,22,minute_h,1,8);
		OLED_ShowNum(63,22,minute_l,1,8);
		OLED_ShowChar(70,22,':',8);
		OLED_ShowNum(77,22,second_h,1,8);
		OLED_ShowNum(84,22,second_l,1,8);
	
		sd8939_id_show(IIC_TEST);
		sd8939_ReadTemp(IIC_TEST);

		OLED_ShowNum(36,43,sd8939_temp,2,6);
		OLED_ShowChar(49,43,'C',6);
		OLED_DrawPoint(49,41);
			
		OLED_ShowChar(122,43,'V',6);
		
		OLED_ShowString(0,43,"Temp:",6);
		OLED_ShowString(70,43,"Vbat:",6);
			
		OLED_ShowChar(0,0,'-',OLED_6X8);
		OLED_ShowChar(5,0,'-',OLED_6X8);
		OLED_ShowChar(10,0,'-',OLED_6X8);
		OLED_ShowChar(15,0,'-',OLED_6X8);
		OLED_ShowChar(20,0,'-',OLED_6X8);
		OLED_ShowChar(25,0,'-',OLED_6X8);
		OLED_ShowChar(30,0,'-',OLED_6X8);
		OLED_ShowChar(35,0,'-',OLED_6X8);

		OLED_ShowString(40,0,"TEST IC",OLED_6X8);
		
		OLED_ShowChar(83,0,'-',OLED_6X8);
		OLED_ShowChar(88,0,'-',OLED_6X8);
		OLED_ShowChar(90,0,'-',OLED_6X8);
		OLED_ShowChar(95,0,'-',OLED_6X8);
		OLED_ShowChar(100,0,'-',OLED_6X8);
		OLED_ShowChar(105,0,'-',OLED_6X8);
		OLED_ShowChar(110,0,'-',OLED_6X8);
		OLED_ShowChar(115,0,'-',OLED_6X8);
		OLED_ShowChar(120,0,'-',OLED_6X8);
		OLED_ShowChar(125,0,'-',OLED_6X8);
		
	}else{
		
		OLED_ShowChar(30,8,'-',8);
		OLED_ShowChar(37,8,'-',8);
		OLED_ShowChar(44,8,'-',8);
		OLED_ShowChar(51,8,'-',8);
		OLED_ShowChar(58,8,'-',8);
		OLED_ShowChar(65,8,'-',8);
		OLED_ShowChar(72,8,'-',8);
		OLED_ShowChar(79,8,'-',8);
		OLED_ShowChar(90,8,'W',8);
		OLED_ShowChar(99,8,'-',8);
		
		OLED_ShowChar(35,22,'-',8);
		OLED_ShowChar(42,22,'-',8);
		OLED_ShowChar(49,22,':',8);
		OLED_ShowChar(56,22,'-',8);
		OLED_ShowChar(63,22,'-',8);
		OLED_ShowChar(70,22,':',8);
		OLED_ShowChar(77,22,'-',8);
		OLED_ShowChar(84,22,'-',8);

	  NO_ACK_sd3xxx_messgae_line();
	}
	return TRUE;
}

uint8_t sd8939_time_copy(void)
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
	
	if(SD8939_WriteTimeDate(&copy_buf,IIC_TEST)!=RTC_FAIL)
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


uint8_t RTC_ReadRAM_8939(uint8_t addr,uint8_t *data,uint8_t len,i2c_device_bus IIC_x)
{

	return SD8939_read_addr(addr,data,len,IIC_x);
	
}

uint8_t RTC_WriteRAM_8939(uint8_t addr,uint8_t *data,uint8_t len,i2c_device_bus IIC_x)
{

	 if(SD8939_write_addr(addr,data,len,IIC_x)!=RTC_OK)
		return RTC_NOACK;
	 
	 return TRUE;
	 
}


