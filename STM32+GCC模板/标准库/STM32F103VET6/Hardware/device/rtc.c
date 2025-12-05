#include "rtc.h"
#include "sd8563.h"
#include "sd8931.h"
#include "sd8939.h"
#include "sd8200.h"
#include "sd8810.h"
#include "sd8804.h"
#include "sd8800.h"
#include "sd8825.h"


rtc_time time_buf;//读时间结构体变量

double Vbatbuf;
uint8_t Tempbuf;


//decimal to bcd
uint8_t dec_to_bcd(uint8_t dec)
{
	return ((dec / 10) << 4) | (dec % 10);
}

//bcd to decimal
uint8_t bcd_to_dec(uint8_t bcd)
{
	return (bcd >> 4) * 10 + (bcd & 0x0f);
}

//16进制转10进制
int HextoDecimal(uint8_t hex) {
    int high_nibble = (hex >> 4) & 0x0F; // 获取高4位
    int low_nibble = hex & 0x0F; // 获取低4位

    int decimal = high_nibble * 16 + low_nibble; // 计算10进制数

    return decimal;
}

//rtc读addr
rtc_exit_code RTC_read_addr(uint8_t reg_addr, uint8_t *read_buf, uint8_t length, i2c_device_bus bus)
{
	if(IIC_read_multiple_bytes(RTC_WRITE,reg_addr,read_buf,length,bus)!=IIC_OK)
	{
		return RTC_NOACK;
	}
	return RTC_OK;
}


//rtc写addr
rtc_exit_code RTC_write_addr(uint8_t reg_addr, uint8_t *write_buf, uint8_t length, i2c_device_bus bus)
{
	if(IIC_write_multiple_bytes(RTC_WRITE,reg_addr,write_buf,length,bus)!=IIC_OK)
	{
		return RTC_NOACK;
	}
	return RTC_OK;
}


//写保护打开
rtc_exit_code RTC_WriteOn(i2c_device_bus IIC_x)
{	
	uint8_t buf;
	buf=0x80;
	if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x) != RTC_OK)
		return RTC_FAIL;
	buf=0xFF;
	if(RTC_write_addr(RTC_CTR1,&buf,1,IIC_x) != RTC_OK)
		return RTC_FAIL;
	
	return RTC_OK;
}

//写保护关闭
rtc_exit_code RTC_WriteOff(i2c_device_bus IIC_x)
{
	uint8_t buf;
	buf=0x7B;
	if(RTC_write_addr(RTC_CTR1,&buf,1,IIC_x) != RTC_OK)
		return RTC_FAIL;
	buf=0x00;
	if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x) != RTC_OK)
		return RTC_FAIL;
	
	return RTC_OK;
}


rtc_exit_code RTC_ReadTimeDate(rtc_time *time, i2c_device_bus IIC_x)
{
	uint8_t buf[7];

	if(RTC_read_addr(RTC_TIME,buf,7,IIC_x)!=RTC_OK)
	{
		return RTC_FAIL;
	}
	
	time->second=bcd_to_dec(buf[0]);
	time->minute=bcd_to_dec(buf[1]);
	time->hour=bcd_to_dec(buf[2] & 0x7F);
	time->week=bcd_to_dec(buf[3]);	
	time->day=bcd_to_dec(buf[4]);
	time->month=bcd_to_dec(buf[5]);	
	time->year=bcd_to_dec(buf[6]);
		
	return RTC_OK;
	
}


rtc_exit_code RTC_WriteTimeDate(rtc_time *time, i2c_device_bus IIC_x)
{
	RTC_WriteOn(IIC_x);
	
	uint8_t buf[7];

	buf[0]=dec_to_bcd(time->second);
	buf[1]=dec_to_bcd(time->minute);
	buf[2]=dec_to_bcd(time->hour)| 0x80;
	buf[3]=dec_to_bcd(time->week);
	buf[4]=dec_to_bcd(time->day);
	buf[5]=dec_to_bcd(time->month);
	buf[6]=dec_to_bcd(time->year);

	if(RTC_write_addr(RTC_TIME,buf,7,IIC_x)!=RTC_OK)
	{
		return RTC_FAIL;
	}
	
	RTC_WriteOff(IIC_x);
	
	return RTC_OK;
	
}


uint8_t sd2405showtime(void)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;
	
	rtc_time time_bufstd;
	
	if(RTC_ReadTimeDate(&time_bufstd,IIC_STD)!=RTC_OK)
	{
		Usart_Printf("time is error");
		return FALSE;
	}else{
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
		
		OLED_ShowNum(30,10,year_h,1,OLED_6X8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
		OLED_ShowNum(37,10,year_l,1,OLED_6X8);
		OLED_ShowChar(44,10,'-',OLED_6X8);
		OLED_ShowNum(51,10,month_h,1,OLED_6X8);
		OLED_ShowNum(58,10,month_l,1,OLED_6X8);
		OLED_ShowChar(65,10,'-',OLED_6X8);
		OLED_ShowNum(72,10,day_h,1,OLED_6X8);
		OLED_ShowNum(79,10,day_l,1,OLED_6X8);
		OLED_ShowChar(90,10,'W',OLED_6X8);
		OLED_ShowNum(99,10,week,1,OLED_6X8);
		
		OLED_ShowNum(35,22,hour_h,1,OLED_6X8);
		OLED_ShowNum(42,22,hour_l,1,OLED_6X8);
		OLED_ShowChar(49,22,':',OLED_6X8);
		OLED_ShowNum(56,22,minute_h,1,OLED_6X8);
		OLED_ShowNum(63,22,minute_l,1,OLED_6X8);
		OLED_ShowChar(70,22,':',OLED_6X8);
		OLED_ShowNum(77,22,second_h,1,OLED_6X8);
		OLED_ShowNum(84,22,second_l,1,OLED_6X8);
		
//		sd2506_TempShow();
//		sd2506_ReadVbat();
	}

	return TRUE;

}

uint8_t sd2505showtime(void)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;
	
	rtc_time time_bufstd;
	
	if(RTC_ReadTimeDate(&time_bufstd,IIC_STD)!=RTC_OK)
	{
		Usart_Printf("time is error");
		return FALSE;
	}else{
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

	}

	return TRUE;

}


//uint8_t Time_test_show(void)
//{
//	sd2405showtime();
//	Test_Line();
//		
//	if(RTC_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
//	{
//		Test_Control();
//		Test_TempShow();
//		Test_VbatShow();
//		return RTC_OK;
//	}else if(SD8563_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
//	{
//		Test_Control();
//		return RTC_OK;
//	}else if(SD8931_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
//	{
//		Test_Control();
//		return RTC_OK;
//	}else if(SD8939_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
//	{
//		Test_Control();
//		return RTC_OK;
//	}else if((SD8200_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL))
//	{
//		Test_Control();
//		return RTC_OK;
//	}else if((SD8810_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL))
//	{
//		Test_Control();
//		return RTC_OK;
//	}else if((SD8804_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL))
//	{
//		Test_Control();
//		return RTC_OK;
//	}else if((SD8800_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL))
//	{
//		Test_Control();
//		return RTC_OK;
//	}else if((SD8825_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL))
//	{
//		Test_Control();
//		return RTC_OK;
//	}
//	
//	
//	else{
//	
//		NO_ACK_Test_Line();
//	
//	}
//	return TRUE;

//}


uint8_t sd2xxx_and_std(void)
{
	sd2405showtime();
	Test_Line();
	
	if(RTC_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
	{
		Test_Control();
//		Test_TempShow();
//		Test_VbatShow();
		return RTC_OK;
	}else{
		
	OLED_ShowChar(30,42,'-',OLED_6X8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
	OLED_ShowChar(37,42,'-',OLED_6X8);
	OLED_ShowChar(44,42,'-',OLED_6X8);
	OLED_ShowChar(51,42,'-',OLED_6X8);
	OLED_ShowChar(58,42,'-',OLED_6X8);
	OLED_ShowChar(65,42,'-',OLED_6X8);
	OLED_ShowChar(72,42,'-',OLED_6X8);
	OLED_ShowChar(79,42,'-',OLED_6X8);
	OLED_ShowChar(90,42,'W',OLED_6X8);
	OLED_ShowChar(99,42,'-',OLED_6X8);

	OLED_ShowChar(35,53,'-',OLED_6X8);
	OLED_ShowChar(42,53,'-',OLED_6X8);
	OLED_ShowChar(49,53,':',OLED_6X8);
	OLED_ShowChar(56,53,'-',OLED_6X8);
	OLED_ShowChar(63,53,'-',OLED_6X8);
	OLED_ShowChar(70,53,':',OLED_6X8);
	OLED_ShowChar(77,53,'-',OLED_6X8);
	OLED_ShowChar(84,53,'-',OLED_6X8);
		
	}

	return TRUE;

}

//uint8_t sd25xxx_time_show(void)
//{
//	sd2405showtime();
//	Test_Line();
//	
//	if(RTC_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
//	{
//		Test_Control();
//		Test_TempShow();
//		Test_VbatShow();
//		return RTC_OK;
//	}else{
//		NO_ACK_Test_Line();
//	}

//	return TRUE;

//}

uint8_t sd2xxx_time_show(void)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;
	
	if(RTC_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
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

uint8_t sd2xxx_time_copy(void)
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
	
	if(RTC_WriteTimeDate(&copy_buf,IIC_TEST)!=RTC_FAIL)
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


uint8_t sd3xxx_time_show(void)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;
	
	rtc_time time_bufstd;
	
	if(RTC_ReadTimeDate(&time_bufstd,IIC_TEST)!=RTC_FAIL)
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
		
		sd3xxx_messgae_line();
		
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

uint8_t sd3xxx_and_std(void)
{
	sd2405showtime();
	Test_Line();
	
	if(RTC_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_FAIL)
	{
		Test_Control();
		return RTC_OK;
	}else{
		NO_ACK_Test_Line();
	}

	return TRUE;

}

uint8_t sd3xxx_time_copy(void)
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
	
	if(RTC_WriteTimeDate(&copy_buf,IIC_TEST)!=RTC_FAIL)
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

void sd3xxx_charge_on(void)
{
	uint8_t buf=0x82;
	RTC_write_addr(0x18,&buf,1,IIC_TEST);
	OLED_ShowImage(110, 40, 12, 20, Charge);
}

void sd3xxx_charge_off(void)
{
	uint8_t buf=0x00;
	RTC_write_addr(0x18,&buf,1,IIC_TEST);
	OLED_ClearArea(110,40,12,20);
}


void Test_Control(void)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;

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

}
void sd2506_messgae_line(void)
{	
		sd2506_idshow();

		RTC_ReadTemp(IIC_STD);
		RTC_ReadVbat(IIC_STD);
	
		OLED_ShowNum(36,43,Tempbuf,2,6);
		OLED_ShowChar(49,43,'C',6);
		OLED_DrawPoint(49,41);
			
		OLED_ShowString(68,43,"Vbat:",6);		
		OLED_ShowFloatNum(92,43,Vbatbuf,1,2,6);
		OLED_ShowChar(122,43,'V',6);
		
		OLED_ShowString(0,43,"Temp:",6);		
			
		OLED_ShowChar(0,0,'-',OLED_6X8);
		OLED_ShowChar(5,0,'-',OLED_6X8);
		OLED_ShowChar(10,0,'-',OLED_6X8);
		OLED_ShowChar(15,0,'-',OLED_6X8);
		OLED_ShowChar(20,0,'-',OLED_6X8);
		OLED_ShowChar(25,0,'-',OLED_6X8);
		OLED_ShowChar(30,0,'-',OLED_6X8);
		OLED_ShowChar(35,0,'-',OLED_6X8);

		OLED_ShowString(40,0,"STD TIME",OLED_6X8);
		
		OLED_ShowChar(88,0,'-',OLED_6X8);
		OLED_ShowChar(90,0,'-',OLED_6X8);
		OLED_ShowChar(95,0,'-',OLED_6X8);
		OLED_ShowChar(100,0,'-',OLED_6X8);
		OLED_ShowChar(105,0,'-',OLED_6X8);
		OLED_ShowChar(110,0,'-',OLED_6X8);
		OLED_ShowChar(115,0,'-',OLED_6X8);
		OLED_ShowChar(120,0,'-',OLED_6X8);
		OLED_ShowChar(125,0,'-',OLED_6X8);
	
}


void sd3xxx_messgae_line(void)
{
		Test_id();

		RTC_ReadTemp(IIC_TEST);	
		RTC_ReadVbat(IIC_TEST);
	
		OLED_ShowNum(36,43,Tempbuf,2,6);
		OLED_ShowChar(49,43,'C',6);
		OLED_DrawPoint(49,41);
			
		OLED_ShowString(68,43,"Vbat:",6);		
		OLED_ShowFloatNum(92,43,Vbatbuf,1,2,6);
		OLED_ShowChar(122,43,'V',6);
		
		OLED_ShowString(0,43,"Temp:",6);
			
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
	
}

void NO_ACK_sd3xxx_messgae_line(void)
{
	OLED_ShowString(36,43,"--",6);
	OLED_ShowChar(49,43,'C',6);
	OLED_DrawPoint(49,41);

	OLED_ShowString(92,43,"-----",6);
	OLED_ShowChar(122,43,'V',6);

	OLED_ShowString(0,43,"Temp:",6);
	OLED_ShowString(68,43,"Vbat:",6);
	
	OLED_ShowString(17,55,"--",6);
	OLED_ShowString(31,55,"--",6);
	OLED_ShowString(45,55,"--",6);
	OLED_ShowString(60,55,"--",6);
	OLED_ShowString(74,55,"--",6);
	OLED_ShowString(88,55,"--",6);
	OLED_ShowString(102,55,"--",6);
	OLED_ShowString(116,55,"--",6);
	
}


void Test_Line(void)
{
/*-----------------------xxxxxxxx------------------------*/
		OLED_ShowChar(0,0,'-',OLED_6X8);
		OLED_ShowChar(5,0,'-',OLED_6X8);
		OLED_ShowChar(10,0,'-',OLED_6X8);
		OLED_ShowChar(15,0,'-',OLED_6X8);
		OLED_ShowChar(20,0,'-',OLED_6X8);
		OLED_ShowChar(25,0,'-',OLED_6X8);
		OLED_ShowChar(30,0,'-',OLED_6X8);
		OLED_ShowChar(35,0,'-',OLED_6X8);

		OLED_ShowString(40,0,"STD TIME",OLED_6X8);
		
		OLED_ShowChar(88,0,'-',OLED_6X8);
		OLED_ShowChar(90,0,'-',OLED_6X8);
		OLED_ShowChar(95,0,'-',OLED_6X8);
		OLED_ShowChar(100,0,'-',OLED_6X8);
		OLED_ShowChar(105,0,'-',OLED_6X8);
		OLED_ShowChar(110,0,'-',OLED_6X8);
		OLED_ShowChar(115,0,'-',OLED_6X8);
		OLED_ShowChar(120,0,'-',OLED_6X8);
		OLED_ShowChar(125,0,'-',OLED_6X8);
		
/*--------------------------------------------------------*/
		OLED_ShowChar(0,32,'-',OLED_6X8);
		OLED_ShowChar(10,32,'-',OLED_6X8);
		OLED_ShowChar(20,32,'-',OLED_6X8);
		OLED_ShowChar(30,32,'-',OLED_6X8);
		OLED_ShowChar(40,32,'-',OLED_6X8);
		OLED_ShowChar(50,32,'-',OLED_6X8);	
		OLED_ShowChar(60,32,'-',OLED_6X8);
		OLED_ShowChar(70,32,'-',OLED_6X8);
		OLED_ShowChar(80,32,'-',OLED_6X8);
		OLED_ShowChar(90,32,'-',OLED_6X8);
		OLED_ShowChar(100,32,'-',OLED_6X8);
		OLED_ShowChar(110,32,'-',OLED_6X8);
		OLED_ShowChar(120,32,'-',OLED_6X8);
		OLED_ShowChar(125,32,'-',OLED_6X8);
		
/*-------xx------------------------------------------
---------xx--------------------------------------------*/
		OLED_ShowString(0,41,"TEST",OLED_6X8);
		OLED_ShowString(5,51,"IC:",OLED_6X8);
}

void NO_ACK_Test_Line(void)
{
/*---------------------------------w--------------------
------------------------:---:---:--------------------*/
	OLED_ShowChar(30,42,'-',OLED_6X8);//相隔显示的数据X轴间隔7个坐标，相隔显示的数据Y轴间隔15个坐标
	OLED_ShowChar(37,42,'-',OLED_6X8);
	OLED_ShowChar(44,42,'-',OLED_6X8);
	OLED_ShowChar(51,42,'-',OLED_6X8);
	OLED_ShowChar(58,42,'-',OLED_6X8);
	OLED_ShowChar(65,42,'-',OLED_6X8);
	OLED_ShowChar(72,42,'-',OLED_6X8);
	OLED_ShowChar(79,42,'-',OLED_6X8);
	OLED_ShowChar(90,42,'W',OLED_6X8);
	OLED_ShowChar(99,42,'-',OLED_6X8);

	OLED_ShowChar(35,53,'-',OLED_6X8);
	OLED_ShowChar(42,53,'-',OLED_6X8);
	OLED_ShowChar(49,53,':',OLED_6X8);
	OLED_ShowChar(56,53,'-',OLED_6X8);
	OLED_ShowChar(63,53,'-',OLED_6X8);
	OLED_ShowChar(70,53,':',OLED_6X8);
	OLED_ShowChar(77,53,'-',OLED_6X8);
	OLED_ShowChar(84,53,'-',OLED_6X8);
	
//	OLED_ShowString(109,45,"--",6);
//	OLED_ShowChar(122,45,'C',6);
//	
//	OLED_ShowString(103,55,"---",6);
//	OLED_ShowChar(122,55,'V',6);
}

/************************串口读时间测试*********************************************/
uint8_t sd2405time(void)
{
	rtc_time time_buf;
	
	if(RTC_ReadTimeDate(&time_buf,IIC_STD)!=RTC_OK)
	{
		Usart_Printf("time is error");
		return FALSE;
	}else{
		
	Usart_Printf("year:%d  month:%d  day:%d  week:%d  hour:%d  minute:%d  second:%d\r\n",
	time_buf.year, time_buf.month, time_buf.day, time_buf.week, time_buf.hour, time_buf.minute, time_buf.second);
	
	}
	
	return TRUE;

}

uint8_t sd3068time(void)
{
	rtc_time time_buf;
	
	if(RTC_ReadTimeDate(&time_buf,IIC_TEST)!=RTC_OK)
	{
		Usart_Printf("time is error");
		return FALSE;
	}else{
		
	Usart_Printf("year:%d  month:%d  day:%d  week:%d  hour:%d  minute:%d  second:%d\r\n",
	time_buf.year, time_buf.month, time_buf.day, time_buf.week, time_buf.hour, time_buf.minute, time_buf.second);
	
	}
	
	return TRUE;

}
/************************串口读时间测试*********************************************/
//读取温度
uint8_t RTC_ReadTemp(i2c_device_bus IIC_x)
{
	uint8_t time = 5;
	uint8_t	Temp2;
	
	if(RTC_WriteOn(IIC_x)!=RTC_OK)
		return 5;
	
	if(IIC_write_byte(0x64,AGTC,0x10,IIC_x)!=IIC_OK)
		return SLAVE_NO_ACK;
	
	if(RTC_WriteOff(IIC_x)!=RTC_OK)
		return 5;
	
	while(time--)
	{
		Delay_ms(1);
		
		if((RTC_ReadByte(AGTC,IIC_x) & 0x20)==0)
		{
			Temp2 = RTC_ReadByte(RTC_TEMPER, IIC_x);
			if(Temp2 & 0x80)
			{
				Temp2 = (~Temp2+1)&0xFF;
				Tempbuf = Temp2;
				bcd_to_dec(Tempbuf);
				Tempbuf = 256-Tempbuf;
				OLED_ShowChar(30,43,'-',6);
			}else{
				Temp2 = Temp2 & 0x7F;
				Tempbuf = Temp2;
				bcd_to_dec(Tempbuf);
			}
		}
	}
	
	return Tempbuf;
	
}

//读取vbat电压
double RTC_ReadVbat(i2c_device_bus IIC_x)
{
	uint8_t time = 5;
	uint8_t Vbatbit8,Vbatlow7;//电池电量
	
	if(RTC_WriteOn(IIC_x)!=RTC_OK)
		return 5;

	if(IIC_write_byte(0x64,RTC_CTR4,0x10,IIC_x)!=IIC_OK)
		return SLAVE_NO_ACK;
	
	if(RTC_WriteOff(IIC_x)!=RTC_OK)
		return 5;
	
	while(time--)
	{
		Delay_ms(1);
		
		Vbatbit8 = RTC_ReadByte(0x1A,IIC_x);
		Vbatlow7 = RTC_ReadByte(0x1B,IIC_x);
		
		Vbatbuf = (Vbatbit8>>7)*255+Vbatlow7;
		
		Vbatbuf=Vbatbuf/100;
		bcd_to_dec(Vbatbuf);
				
	}
	
	return Vbatbuf;
		
}

////读取vbat电压
//uint8_t RTC_ReadVbat(i2c_device_bus IIC_x)
//{
//	uint8_t time = 5;
//	int Vbatbit8,Vbatlow7;//电池电量
//	
//	if(RTC_WriteOn(IIC_x)!=RTC_OK)
//		return 5;

//	if(IIC_write_byte(0x64,RTC_CTR4,0x10,IIC_x)!=IIC_OK)
//		return SLAVE_NO_ACK;
//	
//	if(RTC_WriteOff(IIC_x)!=RTC_OK)
//		return 5;
//	
//	while(time--)
//	{
//		Delay_ms(1);
//		Vbatbit8 = RTC_ReadByte(0x1A,IIC_x);
//		Vbatlow7 = RTC_ReadByte(0x1B,IIC_x);
//		Vbatbuf = (Vbatbit8>>7)*255+Vbatlow7;
//		bcd_to_dec(Vbatbuf);
//		
//		return Vbatbuf;
//		
//	}
//	
//	return TRUE;
//	
//}

//读取vbat电压和上位机使用
uint8_t RTC_VbatQuan(uint8_t *data,i2c_device_bus IIC_x)
{
	uint8_t time = 5;
	uint8_t buf=0x10;

	if(RTC_WriteOn(IIC_x)!=RTC_OK)
		return RTC_FAIL;

	 if(RTC_write_addr(RTC_CTR4,&buf,1,IIC_x)!=RTC_OK)
		return RTC_NOACK;
	
	if(RTC_WriteOff(IIC_x)!=RTC_OK)
		return RTC_FAIL;

	while(time--)
	{
		Delay_ms(1);
		
		RTC_read_addr(RTC_CTR5,data,2,IIC_x);
			return TRUE;
		
	}
	
	return FALSE;
	
}

//读取芯片id码
uint8_t rtc_ifbuff(uint8_t *Data, i2c_device_bus IIC_x)
{
	if(IIC_read_multiple_bytes(0x64,RTC_ID,Data,8,IIC_x)!=IIC_OK)
		return SLAVE_NO_ACK;

	return TRUE;
}

void sd2506_idshow(void)
{
	uint8_t ifbuff[8];
	rtc_ifbuff(ifbuff,IIC_STD);

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

}

void Test_id(void)
{
	uint8_t ifbuff[8];
	rtc_ifbuff(ifbuff,IIC_TEST);

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


}


//与上位机交互
uint8_t RTC_ReadRAM(uint8_t addr,uint8_t *data,uint8_t len,i2c_device_bus IIC_x)
{

	return RTC_read_addr(addr,data,len,IIC_x);
	
}

uint8_t RTC_WriteRAM(uint8_t addr,uint8_t *data,uint8_t len,i2c_device_bus IIC_x)
{
	if(RTC_WriteOn(IIC_x)!=RTC_OK)
		return RTC_FAIL;
		
	 if(RTC_write_addr(addr,data,len,IIC_x)!=RTC_OK)
		 return RTC_NOACK;
	
	if(RTC_WriteOff(IIC_x)!=RTC_OK)
		return RTC_FAIL;

	return TRUE;
}

uint8_t RTC_ReadID(uint8_t *data,i2c_device_bus IIC_x)
{

	if(RTC_read_addr(RTC_ID,data,8,IIC_x)!=RTC_OK)
		return RTC_NOACK;

	return TRUE;

}




