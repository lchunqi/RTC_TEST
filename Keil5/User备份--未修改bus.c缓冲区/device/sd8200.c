#include "sd8200.h"

double sd8200_vbat;


//rtc读addr
rtc_exit_code SD8200_read_addr(uint8_t reg_addr, uint8_t *read_buf, uint8_t length, i2c_device_bus bus)
{
	if(IIC_read_multiple_bytes(0xD0,reg_addr,read_buf,length,bus)!=IIC_OK)
	{
		return RTC_NOACK;
	}
	return RTC_OK;
}

//rtc写addr
rtc_exit_code SD8200_write_addr(uint8_t reg_addr, uint8_t *write_buf, uint8_t length, i2c_device_bus bus)
{
	if(IIC_write_multiple_bytes(0xD0,reg_addr,write_buf,length,bus)!=IIC_OK)
	{
		return RTC_NOACK;
	}
	return RTC_OK;
}

//iic读取寄存器，返回读取到的数据
uint8_t SD8200_ReadByte(uint8_t addr,i2c_device_bus bus)
{
	uint8_t mdata;
	
	IIC_Start(bus);

	IIC_SendByte(0xD0,bus);
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
	
	IIC_SendByte(0xD1,bus);
	if (IIC_WaitAck(bus) != SLAVE_ACK) 
	{
		return SLAVE_NO_ACK;
	}
	
	mdata = IIC_ReceiveByte(bus);
	
	IIC_NoAck(bus);
	
	IIC_Stop(bus);
	
	return mdata;

}

rtc_exit_code SD8200_ReadTimeDate(rtc_time *time, i2c_device_bus IIC_x)
{
	uint8_t buf[7];

	if(SD8200_read_addr(0x00,buf,7,IIC_x)!=RTC_OK)
	{
		return RTC_FAIL;
	}
	
	time->second=bcd_to_dec(buf[0]);
	time->minute=bcd_to_dec(buf[1]);
	time->hour=bcd_to_dec(buf[2]);
	time->week=bcd_to_dec(buf[3]);	
	time->day=bcd_to_dec(buf[4]);
	time->month=bcd_to_dec(buf[5]);	
	time->year=bcd_to_dec(buf[6]);
		
	return RTC_OK;

}

//读取芯片id码
uint8_t sd8200_idbuff(uint8_t *Data, i2c_device_bus IIC_x)
{
	if(IIC_read_multiple_bytes(0x64,0xB2,Data,8,IIC_x)!=IIC_OK)
		return SLAVE_NO_ACK;

	return TRUE;
}

uint8_t sd8200_id_show(i2c_device_bus IIC_x)
{
		uint8_t ifbuff[8];
				
		if(sd8200_idbuff(ifbuff,IIC_x)!=TRUE)
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

uint8_t sd8200_ReadVbat(i2c_device_bus IIC_x)
{
	uint8_t	Vbatbit8,Vbatlow7,Vbatlow6;
	
	uint8_t buf = 0x10;
	if(SD8200_write_addr(0x59,&buf,1,IIC_x)!=RTC_OK)
		return RTC_NOACK;
	
	Vbatlow6 = RTC_ReadByte(0x57,IIC_x);
	if((Vbatlow6 & 0x20)==0)
	{
		Vbatbit8 = RTC_ReadByte(0x5A,IIC_x);
		Vbatlow7 = RTC_ReadByte(0x5B,IIC_x);
		
		sd8200_vbat = ((Vbatbit8&0x80)>>7)*255+Vbatlow7;
		
		sd8200_vbat = sd8200_vbat/100;
		bcd_to_dec(sd8200_vbat);
	}

	return TRUE;
}


rtc_exit_code SD8200_WriteTimeDate(rtc_time *time, i2c_device_bus IIC_x)
{
	uint8_t buf[7];

	buf[0]=dec_to_bcd(time->second);
	buf[1]=dec_to_bcd(time->minute);
	buf[2]=dec_to_bcd(time->hour);
	buf[3]=dec_to_bcd(time->week);
	buf[4]=dec_to_bcd(time->day);
	buf[5]=dec_to_bcd(time->month);
	buf[6]=dec_to_bcd(time->year);

	if(SD8200_write_addr(0x00,buf,7,IIC_x)!=RTC_OK)
	{
		return RTC_FAIL;
	}
	
	return RTC_OK;
	
}

uint8_t sd8200_and_std(void)
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
	
	if(SD8200_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
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

uint8_t sd8200_time_show(void)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;
	
	rtc_time time_bufstd;
	
	if(SD8200_ReadTimeDate(&time_bufstd,IIC_TEST)!=RTC_FAIL)
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
			
		
		sd8200_id_show(IIC_TEST);
		
		sd8200_ReadVbat(IIC_TEST);

		OLED_ShowChar(46,43,'C',6);
		OLED_DrawPoint(46,41);
			
		OLED_ShowFloatNum(92,43,sd8200_vbat,1,2,6);
		OLED_ShowChar(122,43,'V',6);
		
		OLED_ShowString(0,43,"Temp:",6);
		OLED_ShowString(68,43,"Vbat:",6);
		
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

uint8_t sd8200_time_copy(void)
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
	
	if(SD8200_WriteTimeDate(&copy_buf,IIC_TEST)!=RTC_FAIL)
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

rtc_exit_code sd8200_f512(i2c_device_bus IIC_x)
{
	uint8_t buf=0xC0;
	SD8200_write_addr(0x07,&buf,1,IIC_x);
	
	buf=0x00;
	SD8200_write_addr(0x22,&buf,1,IIC_x);

	return RTC_OK;
}

rtc_exit_code sd8200_f512_off(i2c_device_bus IIC_x)
{
	uint8_t buf=0x00;
	SD8200_write_addr(0x07,&buf,1,IIC_x);
	
	buf=0x00;
	SD8200_write_addr(0x22,&buf,1,IIC_x);

	return RTC_OK;
}
rtc_exit_code sd8200_f1(i2c_device_bus IIC_x)
{
	uint8_t buf=0xC0;
	SD8200_write_addr(0x07,&buf,1,IIC_x);
	
	buf=0x01;
	SD8200_write_addr(0x22,&buf,1,IIC_x);
	
	return RTC_OK;
}

rtc_exit_code sd8200_f1_off(i2c_device_bus IIC_x)
{
	uint8_t buf=0x00;
	SD8200_write_addr(0x07,&buf,1,IIC_x);
	
	buf=0x00;
	SD8200_write_addr(0x22,&buf,1,IIC_x);

	return RTC_OK;
}



uint8_t RTC_ReadRAM_8200(uint8_t addr,uint8_t *data,uint8_t len,i2c_device_bus IIC_x)
{

	return SD8200_read_addr(addr,data,len,IIC_x);
	
}

uint8_t RTC_WriteRAM_8200(uint8_t addr,uint8_t *data,uint8_t len,i2c_device_bus IIC_x)
{

	 if(SD8200_write_addr(addr,data,len,IIC_x)!=RTC_OK)
		return RTC_NOACK;
	 
	 return TRUE;
	 
}

void sd8200_charge_on(void)
{
	uint8_t buf=0x20;
	SD8200_write_addr(0x08,&buf,1,IIC_TEST);
	  buf=0x05;
	SD8200_write_addr(0x09,&buf,1,IIC_TEST);
	OLED_ShowImage(110, 40, 12, 20, Charge);
}

void sd8200_charge_off(void)
{
	uint8_t buf=0x00;
	SD8200_write_addr(0x08,&buf,1,IIC_TEST);
	  buf=0x00;
	SD8200_write_addr(0x09,&buf,1,IIC_TEST);
	OLED_ClearArea(110,40,12,20);
}


