#include "config.h"

/*
 * MODE 0:CPOL=0，CPHA=0：此时空闲态时，SCLK处于低电平，数据采样是在第 1个边沿，	\
 * 			也就是 SCLK由低电平到高电平的跳变，所以数据采样是在上升沿，数据发送是在下降沿。

 * MODE 1:CPOL=0，CPHA=1：此时空闲态时，SCLK处于低电平，数据发送是在第 2个边沿，	\
 * 			也就是 SCLK由高电平到低电平的跳变，所以数据采样是在下降沿，数据发送是在上升沿。

 * MODE 2:CPOL=1，CPHA=0：此时空闲态时，SCLK处于高电平，数据采集是在第 1个边沿，	\
 * 			也就是 SCLK由高电平到低电平的跳变，所以数据采集是在下降沿，数据发送是在上升沿。

 * MODE 3:CPOL=1，CPHA=1：此时空闲态时，SCLK处于高电平，数据发送是在第 2个边沿，	\
 * 			也就是 SCLK由低电平到高电平的跳变，所以数据采集是在上升沿，数据发送是在下降沿。
*/

uint16_t spi_time_buf[7];
uint16_t spi_show_time[7];

uint8_t  sd8840_temp;

uint8_t  sd8902_temp;
double  sd8902_vbat;

#define WAIT_TIME		2

/**
功能：spi延时
修改：sjm
参数：Count-需要延时的时间
返回值：-
**/
static void SPI_Delay_us( uint32_t Count )
{
    while(Count)
    {
        Count--;
    }
}

/**
功能：spi初始化
修改：sjm
参数：-
返回值：-
**/
void SPI_init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/* CS */
	GPIO_InitStructure.GPIO_Pin = SL_SPI_CS_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SL_SPI_CS_PORT, &GPIO_InitStructure);
	
	/* SCK */
	GPIO_InitStructure.GPIO_Pin = SL_SPI_SCK_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SL_SPI_SCK_PORT, &GPIO_InitStructure);
	
	/* MISO */
	GPIO_InitStructure.GPIO_Pin = SL_SPI_MISO_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SL_SPI_MISO_PORT, &GPIO_InitStructure);
	
	/* MOSI */
	GPIO_InitStructure.GPIO_Pin = SL_SPI_MOSI_PINS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SL_SPI_MOSI_PORT, &GPIO_InitStructure);

}

/******************************4线spi****************************************/
/**
功能：spi发送一个字节
修改：sjm
参数：buf-发送的数据
返回值：-
**/
void spi_send_one_byte(uint8_t buf)
{
	uint8_t i;
	

	for (i = 0; i < 8; i++) 
	{
		SPI_SCK(LOW);
		if (buf & 0x80) 
		{
			SPI_MOSI(HIGH);
		} 
		else 
		{
			SPI_MOSI(LOW);
		}
		SPI_Delay_us( WAIT_TIME );
		SPI_SCK(HIGH);
		SPI_Delay_us( WAIT_TIME );
		buf <<= 1;
	}
}

/**
功能：spi接收一个字节
修改：sjm
参数：*buf-接收的数据
返回值：-
**/
void spi_receive_one_byte(uint16_t *buf)
{
	uint8_t i;
	
	*buf = 0;
	for (i = 0; i < 8; i++) 
	{
		SPI_SCK(LOW);
		SPI_Delay_us( WAIT_TIME );
		SPI_SCK(HIGH);
		*buf <<= 1;
		if (SPI_MISO) 
		{
			*buf |= 0x01;
		}
		SPI_Delay_us( WAIT_TIME );
	}
}

/**
功能：spi读取多个字节
修改：sjm
参数：reg_addr-寄存器，*buf-读取的数据，size-数量
返回值：TRUE-成功
**/
uint8_t spi_read_multi_bytes(uint8_t reg_addr, uint16_t *buf, uint8_t size)
{	
	uint8_t i;
	
	SPI_SCK(HIGH);
	SPI_CS(HIGH);
	SPI_Delay_us( WAIT_TIME );
	reg_addr |= 0x80;
	spi_send_one_byte(reg_addr);
	for (i = 0; i < size; i++) 
	{
		spi_receive_one_byte(buf + i);
	}
	SPI_CS(LOW);
	
	return TRUE;
}

/**
功能：spi写多个字节
修改：sjm
参数：reg_addr-寄存器，*buf-写数据，size-数量
返回值：TRUE-成功
**/
uint8_t spi_write_multi_bytes(uint8_t reg_addr, uint16_t *buf, uint8_t size)
{
	uint8_t i;
	
	SPI_SCK(HIGH);
	SPI_CS(HIGH);
	SPI_Delay_us( WAIT_TIME );
	reg_addr &= 0x7f;
	spi_send_one_byte(reg_addr);
	for (i = 0; i < size; i++) 
	{
		spi_send_one_byte(buf[i]);
	}
	SPI_CS(LOW);
	
	return TRUE;
}

/**
功能：sd8840读时间数据
修改：sjm
参数：-
返回值：SPI_OK-成功
**/
spi_exit_code SD8840_ReadTimeDate(void)
{
	uint8_t i;
	spi_read_multi_bytes(0x00,spi_time_buf,7);
	for(i=0;i<8;i++)
	{
		spi_show_time[i]=bcd_to_dec(spi_time_buf[i]);
	}
	spi_readtime(spi_show_time);
	
	return SPI_OK;
}

/**
功能：sd8840写时间数据
修改：sjm
参数：-
返回值：SPI_OK-成功
**/
spi_exit_code SD8840_WriteTimeDate(void)
{
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		spi_show_time[i]=bcd_to_dec(spi_time_buf[i]);
	}
	
	spi_write_multi_bytes(0x00,spi_show_time,7);
	
	spi_readtime(spi_show_time);
	
	return SPI_OK;
}

/**
功能：sd8840时间oled显示
修改：sjm
参数：-
返回值：SPI_OK-成功
**/
spi_exit_code SD8840_ReadTimeDate_show(void)
{
	uint8_t i;
	spi_read_multi_bytes(0x00,spi_time_buf,7);
	for(i=0;i<8;i++)
	{
		spi_show_time[i]=bcd_to_dec(spi_time_buf[i]);
	}
	spi_readtime_show(spi_show_time);
	
	return SPI_OK;
}

/**
功能：sd8840读时间oled格式显示
修改：sjm
参数：*time-输入的数组
返回值：-
**/
void spi_readtime(uint16_t *time)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;

	year_h = time[6]/ 10;
	year_l = time[6] % 10;  //年
	
	month_h = time[5] / 10;
	month_l = time[5]% 10;  //月
	
	day_h = time[4] / 10;
	day_l = time[4] % 10;  //日
	
	week = time[3];
	
	hour_h = time[2] / 10;
	hour_l = time[2] % 10;  //时
	
	minute_h = time[1] / 10;
	minute_l = time[1] % 10;  //分
	
	second_h = time[0] / 10;
	second_l = time[0] % 10; //秒

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

/**
功能：sd8840读时间oled格式显示
修改：sjm
参数：*time-输入的数组
返回值：-
**/
void spi_readtime_show(uint16_t *time)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;

	year_h = time[6]/ 10;
	year_l = time[6] % 10;  //年
	
	month_h = time[5] / 10;
	month_l = time[5]% 10;  //月
	
	day_h = time[4] / 10;
	day_l = time[4] % 10;  //日
	
	week = time[3];
	
	hour_h = time[2] / 10;
	hour_l = time[2] % 10;  //时
	
	minute_h = time[1] / 10;
	minute_l = time[1] % 10;  //分
	
	second_h = time[0] / 10;
	second_l = time[0] % 10; //秒

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
}

/**
功能：sd8902时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
void sd8902_readtime_show(uint16_t *time)
{
	uint8_t year_h,year_l;
	uint8_t month_h,month_l;
	uint8_t day_h,day_l;
	uint8_t week;
	uint8_t hour_h,hour_l;
	uint8_t minute_h,minute_l;
	uint8_t second_h,second_l;

	year_h = time[6]/ 10;
	year_l = time[6] % 10;  //年
	
	month_h = time[5] / 10;
	month_l = time[5]% 10;  //月
	
	day_h = time[4] / 10;
	day_l = time[4] % 10;  //日
	
	week = time[3];
	
	hour_h = time[2] / 10;
	hour_l = time[2] % 10;  //时
	
	minute_h = time[1] / 10;
	minute_l = time[1] % 10;  //分
	
	second_h = time[0] / 10;
	second_l = time[0] % 10; //秒

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

/******************************3线spi****************************************/
/**
功能：sd8902 data pin设置成输入模式
修改：sjm
参数：-
返回值：-
**/
void sd8902_set_input_mode(void)
{
	GPIO_InitTypeDef sd8902_Struction;				//定义结构体成员
	RCC_APB2PeriphClockCmd(sd8902_DAT_RCC, ENABLE);	//打开时钟
	
	sd8902_Struction.GPIO_Pin  = sd8902_DAT_PIN;		//引脚
	sd8902_Struction.GPIO_Mode = GPIO_Mode_IPU;			//上拉输入 或者 浮空输入 GPIO_Mode_IN_FLOATING 都可以
		
	GPIO_Init(sd8902_DAT_PORT,&sd8902_Struction);		//对成员进行初始化	
}

/**
功能：sd8902 data pin设置成输出模式
修改：sjm
参数：-
返回值：-
**/
void sd8902_set_output_mode(void)
{
	GPIO_InitTypeDef sd8902_Struction;				//定义结构体成员
	RCC_APB2PeriphClockCmd(sd8902_DAT_RCC, ENABLE);	//打开时钟
	
	sd8902_Struction.GPIO_Pin  = sd8902_DAT_PIN;		//引脚
	sd8902_Struction.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出
	sd8902_Struction.GPIO_Speed = GPIO_Speed_50MHz;		//速率	

	GPIO_Init(sd8902_DAT_PORT,&sd8902_Struction);		//对成员进行初始化	
}



/**
功能：sd8902初始化
修改：sjm
参数：-
返回值：-
**/
void sd8902_config(void)
{
	GPIO_InitTypeDef sd8902_Struction;				//定义结构体成员

	RCC_APB2PeriphClockCmd(sd8902_CLK_RCC, ENABLE);	//打开时钟
	RCC_APB2PeriphClockCmd(sd8902_DAT_RCC, ENABLE);	//打开时钟
	RCC_APB2PeriphClockCmd(sd8902_RST_RCC, ENABLE);	//打开时钟

	//sd8902 CLK
	sd8902_Struction.GPIO_Pin  = sd8902_CLK_PIN;		//引脚
	sd8902_Struction.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出	
	sd8902_Struction.GPIO_Speed = GPIO_Speed_50MHz; 	//50MHz
	GPIO_Init(sd8902_CLK_PORT,&sd8902_Struction);		//对成员进行初始化		
	//sd8902 DAT
	sd8902_Struction.GPIO_Pin  = sd8902_DAT_PIN;		//引脚
	sd8902_Struction.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出	
	sd8902_Struction.GPIO_Speed = GPIO_Speed_50MHz; 	//50MHz
	GPIO_Init(sd8902_DAT_PORT,&sd8902_Struction);		//对成员进行初始化	
	//sd8902 RST
	sd8902_Struction.GPIO_Pin  = sd8902_RST_PIN;		//引脚
	sd8902_Struction.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出	
	sd8902_Struction.GPIO_Speed = GPIO_Speed_50MHz; 	//50MHz
	GPIO_Init(sd8902_RST_PORT,&sd8902_Struction);		//对成员进行初始化	
	
}

/**
功能：sd8902写一个字节
修改：sjm
参数：addr_or_data-写入的地址或者数据
返回值：-
**/
void sd8902_write_byte(uint8_t addr_or_data)
{
	uint8_t i;
	sd8902_SET_OUT;
	for(i=0;i<8;i++)
	{
		if(addr_or_data & 0x01)		//从低位开始传输
		{
			sd8902_DAT_HIGH;
		}
		else
		{
			sd8902_DAT_LOW;
		}
		addr_or_data >>= 1;		//右移一位
		sd8902_CLK_HIGH;		//拉高时钟表示已经发送
		sd8902_CLK_LOW;			//拉低时钟准备继续放新的数据位
	}
}

/**
功能：sd8902写数据
修改：sjm
参数：addr-写入的地址，dat-写入的数据
返回值：-
**/
void sd8902_write_data(uint8_t addr,uint8_t dat)
{
	sd8902_RST_LOW;
	sd8902_CLK_LOW;
	sd8902_RST_HIGH;
	sd8902_write_byte(addr);
	sd8902_write_byte(dat);
	sd8902_RST_LOW;
}

/**
功能：sd8902读一个字节
修改：sjm
参数：-
返回值：dat-读到的数据
**/
uint8_t sd8902_read_byte(void)
{
	uint8_t i;
	uint8_t dat = 0;
	sd8902_SET_IN;
	for(i=0;i<8;i++)
	{
		dat >>= 1;		//左移一次 低位开始 共有效左移7次
		if( GPIO_ReadInputDataBit(sd8902_DAT_PORT, sd8902_DAT_PIN) == SET)
		{
			dat = dat | 0x80;
		}
		sd8902_CLK_HIGH;
		sd8902_CLK_LOW;
	}
	return dat;
}

/**
功能：sd8902读数据
修改：sjm
参数：addr-读地址
返回值：dat-读到的数据
**/
uint8_t sd8902_read_data(uint8_t addr)
{
	uint8_t dat = 0;
	sd8902_RST_LOW;
	sd8902_CLK_LOW;
	sd8902_RST_HIGH;
	sd8902_write_byte(addr);
	dat = sd8902_read_byte();
	sd8902_RST_LOW;
	return dat;
}

/**
功能：BCD转10进制
修改：sjm
参数：*bcd-转换的数据，times-转换长度
返回值：-
**/
void bcd_to_decimal(uint16_t *bcd,uint8_t times)
{
	uint8_t i;
	for(i=0;i<times;i++)
	{
		bcd[i] = ((bcd[i]>>4)*10) + (bcd[i]&0x0f);
	}
}

/**
功能：10进制转BCD
修改：sjm
参数：*dec-转换的数据，times-转换长度
返回值：-
**/
void decimal_to_bcd(uint16_t *dec,uint8_t times)
{
	uint8_t i;
	for(i=0;i<times;i++)
	{
		dec[i] = ((dec[i]/10)<<4) | (dec[i]%10);
	}
}

/**
功能：sd8902写时间
修改：sjm
参数：-
返回值：SPI_OK-成功
**/
spi_exit_code SD8902_WriteTimeDate(void)
{
	uint16_t sd8902_buff[7];

	decimal_to_bcd(sd8902_buff,7);	//十进制转BCD码
	sd8902_write_data(sd8902_WRITE_PROTECT,0x00); //关闭写保护	
	
	sd8902_write_data(sd8902_WRITE_YEAR,sd8902_buff[6]);	//年
	sd8902_write_data(sd8902_WRITE_MONTH,sd8902_buff[5]);	//月
	sd8902_write_data(sd8902_WRITE_DAY,	sd8902_buff[4]);  	//日
	sd8902_write_data(sd8902_WRITE_WEEK,sd8902_buff[3]);  	//周	
	sd8902_write_data(sd8902_WRITE_HOUR,sd8902_buff[2]);   	//时
	sd8902_write_data(sd8902_WRITE_MINUTE,sd8902_buff[1]); 	//分
	sd8902_write_data(sd8902_WRITE_SECOND,sd8902_buff[0]);  //秒

	sd8902_write_data(sd8902_WRITE_PROTECT,0x80); //开启写保护	
	
	return SPI_OK;
	
}

/**
功能：sd8902读取日期和时间
修改：sjm
参数：-
返回值：SPI_OK-成功
**/
spi_exit_code SD8902_ReadTimeDate(void)
{
	uint16_t sd8902_buff[7];

	sd8902_buff[6] = sd8902_read_data(sd8902_READ_YEAR);  //读取年
	sd8902_buff[5] = sd8902_read_data(sd8902_READ_MONTH); //读取月
	sd8902_buff[4] = sd8902_read_data(sd8902_READ_DAY);   //读取日
	sd8902_buff[3] = sd8902_read_data(sd8902_READ_WEEK);  //读取周
	sd8902_buff[2] = sd8902_read_data(sd8902_READ_HOUR);  //读取时
	sd8902_buff[1] = sd8902_read_data(sd8902_READ_MINUTE);//读取分
	sd8902_buff[0] = sd8902_read_data(sd8902_READ_SECOND);//读取秒

	bcd_to_decimal(sd8902_buff,7);	//BCD码转成十进制
	
	spi_readtime(sd8902_buff);
	
	return SPI_OK;

}

/**
功能：sd8902读取id码oled显示
修改：sjm
参数：-
返回值：SPI_OK-成功
**/
uint8_t sd8902_id_show(void)
{
	uint16_t buf[8];

	buf[7] = sd8902_read_data(0xA1);
	buf[6] = sd8902_read_data(0x9F);
	buf[5] = sd8902_read_data(0x9D);
	buf[4] = sd8902_read_data(0x9B);
	buf[3] = sd8902_read_data(0x99);
	buf[2] = sd8902_read_data(0x97);
	buf[1] = sd8902_read_data(0x95);
	buf[0] = sd8902_read_data(0x93);

	bcd_to_decimal(buf,8);

	OLED_ShowString(0,55,"ID",6);
	OLED_ShowString(12,55,":",6);

	OLED_ShowNum(17,55,buf[0],2,6);
	OLED_ShowNum(31,55,buf[1],2,6);
	OLED_ShowNum(45,55,buf[2],2,6);
	OLED_ShowNum(60,55,buf[3],2,6);
	OLED_ShowNum(74,55,buf[4],2,6);
	OLED_ShowNum(88,55,buf[5],2,6);
	OLED_ShowNum(102,55,buf[6],2,6);
	OLED_ShowNum(116,55,buf[7],2,6);

	return SPI_OK;
	
}

/**
功能：sd8902读取时间oled显示
修改：sjm
参数：-
返回值：SPI_OK-成功
**/
spi_exit_code SD8902_ReadTimeDate_show(void)
{
	uint16_t sd8902_buff[7];
	
	sd8902_buff[6] = sd8902_read_data(sd8902_READ_YEAR);  //读取年
	sd8902_buff[5] = sd8902_read_data(sd8902_READ_MONTH); //读取月
	sd8902_buff[4] = sd8902_read_data(sd8902_READ_DAY);   //读取日
	sd8902_buff[3] = sd8902_read_data(sd8902_READ_WEEK);  //读取周
	sd8902_buff[2] = sd8902_read_data(sd8902_READ_HOUR);  //读取时
	sd8902_buff[1] = sd8902_read_data(sd8902_READ_MINUTE);//读取分
	sd8902_buff[0] = sd8902_read_data(sd8902_READ_SECOND);//读取秒

	bcd_to_decimal(sd8902_buff,7);	//BCD码转成十进制

	sd8902_readtime_show(sd8902_buff);
	
	return SPI_OK;

}

/**
功能：sd8840和标准模块时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
uint8_t sd8840_and_std(void)
{
	sd2405showtime();
	Test_Line();
	
	if(SD8840_ReadTimeDate()!=SPI_OK)
	{
		NO_ACK_Test_Line();
	}else{
		SD8840_ReadTimeDate();
		return SPI_OK;
	}
	
	return TRUE;
}

/**
功能：sd8840时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
uint8_t sd8840_time_show(void)
{
	if(SD8840_ReadTimeDate_show()!=SPI_OK)
	{
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
		
	}else{
		
		SD8840_ReadTimeDate_show();
		
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
		
		sd8840_ReadTemp();
		
		OLED_ShowNum(36,50,sd8840_temp,2,6);
		OLED_ShowChar(49,50,'C',6);
		OLED_DrawPoint(49,48);

		OLED_ShowString(0,50,"Temp:",6);
		
		
		return SPI_OK;
	}
	
	return TRUE;
}

/**
功能：sd8840复制时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
uint8_t sd8840_time_copy(void)
{
	uint16_t buf[7];
	rtc_time copy_buf;
	
	if(RTC_ReadTimeDate(&copy_buf,IIC_STD)!=RTC_OK)
	{
		Usart_Printf("time is error");
		return FALSE;
		
	}

	buf[0]=dec_to_bcd(copy_buf.second);
	buf[1]=dec_to_bcd(copy_buf.minute);
	buf[2]=dec_to_bcd(copy_buf.hour);
	buf[3]=dec_to_bcd(copy_buf.week);
	buf[4]=dec_to_bcd(copy_buf.day);
	buf[5]=dec_to_bcd(copy_buf.month);
	buf[6]=dec_to_bcd(copy_buf.year);

	if(SD8840_WriteTimeDate()!=SPI_OK)
	{
		NO_ACK_Test_Line();
	}else{
		spi_write_multi_bytes(0x00,buf,7);
		return SPI_OK;
	}

	return TRUE;
	
}

/**
功能：sd8840输出32768Hz频率
修改：sjm
参数：-
返回值：SPI_OK-成功
**/
spi_exit_code sd8840_f32768(void)
{
		uint16_t buf=0x0E;

		spi_write_multi_bytes(0x0D,&buf,1);
	
		OLED_ShowImage(0, 0, 128, 64, wave_32768);

		return SPI_OK;
}

/**
功能：sd8840关闭输出32768Hz频率
修改：sjm
参数：-
返回值：SPI_OK-成功
**/
spi_exit_code sd8840_f32768_off(void)
{
		uint16_t buf=0x00;

		spi_write_multi_bytes(0x0D,&buf,1);

		return SPI_OK;
}

/**
功能：sd8840输出1024Hz频率
修改：sjm
参数：-
返回值：SPI_OK-成功
**/
spi_exit_code sd8840_f1024(void)
{
		uint16_t buf=0x0A;

		spi_write_multi_bytes(0x0D,&buf,1);
	
		OLED_ShowImage(0, 0, 128, 64, wave_32768);

		return SPI_OK;
}

/**
功能：sd8840关闭输出1024Hz频率
修改：sjm
参数：-
返回值：SPI_OK-成功
**/
spi_exit_code sd8840_f1024_off(void)
{
		uint16_t buf=0x00;

		spi_write_multi_bytes(0x0D,&buf,1);

		return SPI_OK;
}

/**
功能：sd8840输出1Hz频率
修改：sjm
参数：-
返回值：SPI_OK-成功
**/
spi_exit_code sd8840_f1(void)
{
		uint16_t buf=0x06;

		spi_write_multi_bytes(0x0D,&buf,1);
	
		OLED_ShowImage(0, 0, 128, 64, wave_1);

		return SPI_OK;
}

/**
功能：sd8840关闭输出1Hz频率
修改：sjm
参数：-
返回值：SPI_OK-成功
**/
spi_exit_code sd8840_f1_off(void)
{
		uint16_t buf=0x00;

		spi_write_multi_bytes(0x0D,&buf,1);

		return SPI_OK;
}

/**
功能：sd8840读取温度
修改：sjm
参数：-
返回值：sd8840_temp-读取到的温度
**/
uint8_t sd8840_ReadTemp(void)
{
	uint16_t Temp1;
	uint16_t buf=0x10;
	
	spi_write_multi_bytes(0x57,&buf,1);
	
	spi_read_multi_bytes(0x56,&Temp1,2);
	
		if((Temp1 & 0x80))
		{
			Temp1 = (~Temp1+1)&0xFF;
			sd8840_temp = Temp1;
			bcd_to_dec(sd8840_temp);
			sd8840_temp = 256-sd8840_temp;
			OLED_ShowChar(30,50,'-',6);
		}else{
			Temp1 = Temp1 & 0x7F;
			sd8840_temp = Temp1;
			bcd_to_dec(sd8840_temp);
		}
			
	return sd8840_temp;
		
}

/**
功能：sd8902和标准模块时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
uint8_t sd8902_and_std(void)
{
	sd2405showtime();
	Test_Line();
	
	if(SD8902_ReadTimeDate()!=SPI_OK)
	{
		NO_ACK_Test_Line();
	}else{
		SD8902_ReadTimeDate();
		return SPI_OK;
	}
	
	return TRUE;
	
}

/**
功能：sd8902时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
uint8_t sd8902_time_show(void)
{
	if(SD8902_ReadTimeDate_show()!=SPI_OK)
	{
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
		
	}else{
		
		SD8902_ReadTimeDate_show();
				
		sd8902_id_show();
		sd8902_ReadTemp();
		sd8902_ReadVbat();

		OLED_ShowNum(36,43,sd8902_temp,2,6);
		OLED_ShowChar(49,43,'C',6);
		OLED_DrawPoint(49,41);
			
		OLED_ShowFloatNum(92,43,sd8902_vbat,1,2,6);
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
		
		return SPI_OK;
	}
	
	return TRUE;
}

/**
功能：sd8902复制时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/
uint8_t sd8902_time_copy(void)
{
	uint8_t buf[7];
	rtc_time copy_buf;
	
	if(RTC_ReadTimeDate(&copy_buf,IIC_STD)!=RTC_OK)
	{
		Usart_Printf("time is error");
		return FALSE;
		
	}

	buf[0]=dec_to_bcd(copy_buf.second);
	buf[1]=dec_to_bcd(copy_buf.minute);
	buf[2]=dec_to_bcd(copy_buf.hour);
	buf[3]=dec_to_bcd(copy_buf.week);
	buf[4]=dec_to_bcd(copy_buf.day);
	buf[5]=dec_to_bcd(copy_buf.month);
	buf[6]=dec_to_bcd(copy_buf.year);

	if(SD8902_WriteTimeDate()!=SPI_OK)
	{
		NO_ACK_Test_Line();
	}else{
		
		sd8902_write_data(sd8902_WRITE_PROTECT,0x00); //关闭写保护	
		
		sd8902_write_data(sd8902_WRITE_SECOND,buf[0]);
		sd8902_write_data(sd8902_WRITE_MINUTE,buf[1]);
		sd8902_write_data(sd8902_WRITE_HOUR,buf[2]);
		sd8902_write_data(sd8902_WRITE_WEEK,buf[3]);
		sd8902_write_data(sd8902_WRITE_DAY,buf[4]);
		sd8902_write_data(sd8902_WRITE_MONTH,buf[5]);
		sd8902_write_data(sd8902_WRITE_YEAR,buf[6]);
				
		sd8902_write_data(sd8902_WRITE_PROTECT,0x80); //开启写保护	

		return SPI_OK;
	}

	return TRUE;
	
}

/**
功能：sd8902输出32768Hz频率
修改：sjm
参数：-
返回值：RTC_OK-成功
**/
spi_exit_code sd8902_f32768(void)
{
//	sd8902_write_data(sd8902_WRITE_PROTECT,0x00); //关闭写保护	

	
	sd8902_write_data(0xB2,0x01);
	SPI_CS(1);

	OLED_ShowImage(0, 0, 128, 64, wave_32768);
	
//	sd8902_write_data(sd8902_WRITE_PROTECT,0x80); //开启写保护	
	
	return SPI_OK;
}

/**
功能：sd8902关闭输出32768Hz频率
修改：sjm
参数：-
返回值：RTC_OK-成功
**/
spi_exit_code sd8902_f32768_off(void)
{
	sd8902_write_data(sd8902_WRITE_PROTECT,0x00); //关闭写保护	

	SPI_CS(0);
	sd8902_write_data(0xB2,0x00);
	
	sd8902_write_data(sd8902_WRITE_PROTECT,0x80); //开启写保护	

	return SPI_OK;
}

/**
功能：sd8902读取温度
修改：sjm
参数：-
返回值：sd8902_temp-读到的温度数据
**/
uint8_t sd8902_ReadTemp(void)
{
	uint8_t	Temp1;	
	Temp1 = sd8902_read_data(0xAF);
	
	if(Temp1 & 0x80)
	{
		Temp1 = (~Temp1+1)&0xFF;
		sd8902_temp = Temp1;
		bcd_to_dec(sd8902_temp);
		sd8902_temp = 256-sd8902_temp;
		OLED_ShowChar(30,43,'-',6);
	}else{
		Temp1 = Temp1 & 0x7F;
		sd8902_temp = Temp1;
		bcd_to_dec(sd8902_temp);
	}

	return sd8902_temp;
	
}

/**
功能：sd8902读取vbat电压
修改：sjm
参数：-
返回值：sd8902_vbat-读取到vbat电压
**/
uint8_t sd8902_ReadVbat(void)
{
	uint8_t	Vbatbit8,Vbatbit7;	

	Vbatbit8 = sd8902_read_data(0xB7);
	Vbatbit7 = sd8902_read_data(0xB9);
	
	sd8902_vbat = (Vbatbit8>>7)*255+Vbatbit7;
	
	sd8902_vbat = sd8902_vbat/100;
	bcd_to_dec(sd8902_vbat);
	
	return sd8902_vbat;
	
}

/**
功能：sd8902读取vbat电压和上位机使用
修改：sjm
参数：-
返回值：sd8902_vbat-读取到vbat电压
**/
uint8_t sd8902_VbatQuan(uint16_t *data)
{
	uint8_t time = 5;
	
	while(time--)
	{
		Delay_ms(1);
		
		sd8902_read_multi_bytes(0xB7,data,2);
			return TRUE;
		
	}
	
	return FALSE;	

}

/**
功能：与上位机交互读取sd8840数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度
返回值：spi_read_multi_bytes(addr,data,len)-读取到的数据
**/
uint8_t RTC_ReadRAM_8840(uint8_t addr,uint16_t *data,uint8_t len)
{

	return spi_read_multi_bytes(addr,data,len);
	
}

/**
功能：与上位机交互写sd8840数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度，IIC_x-iic总线
返回值：TRUE-成功
**/
uint8_t RTC_WriteRAM_8840(uint8_t addr,uint16_t *data,uint8_t len)
{
		if(spi_write_multi_bytes(addr,data,len)!=TRUE)
			return FALSE;
		
		return TRUE;
}


uint8_t sd8902_read_buff[]={0x81,0x83,0x85,0x87,0x89,0x8B,0x8D,0x8F,0x91,0x93,0x95,0x97,0x99,0x9B,0x9D,0x9F,0xA1,0xAF,0xB1,0xB3,0xB5,0xB7,0xB9,0xBB,0xBD,0xC1,0xC3,0xC5,0xC7,0xC9,0xCB,0xCD,0xCD,0xCF,0xD1,0xD3,0xD5,0xD7,0xD9,0xDB,0xDD,0xDF,0xF1,0xF3,0xF5,0xF7,0xF9,0xFB,0xFD};
uint8_t sd8902_write_buff[]={0x80,0x82,0x84,0x86,0x88,0x8A,0x8C,0x8E,0x90,0xB2,0xB4,0xBA,0xBC,0xC0,0xC2,0xC4,0xC6,0xC8,0xCA,0xCC,0xCE,0xD0,0xD2,0xD4,0xD6,0xD8,0xDA,0xDC,0xDE,0xE0,0xE2,0xE4,0xE6,0xE8,0xEA,0xEC,0xEE,0xF0,0xF2,0xF4,0xF6,0xF8,0xFA,0xFC};
/**
功能：sd8902读取多个寄存器数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度
返回值：TRUE-成功
**/
uint8_t sd8902_read_multi_bytes(uint8_t reg_addr, uint16_t *buf, uint8_t len)
{

	 uint8_t index = 0;
    // 找到reg_addr在sd8902_read_buff数组中的索引
    for(uint8_t i = 0; i < sizeof(sd8902_read_buff); i++)
    {
        if(sd8902_read_buff[i] == reg_addr)
        {
            index = i;
            break;
        }
    }
    
    // 依次读取从index开始的len个字节
    for(uint8_t i = 0; i < len; i++)
    {
        if(index + i < sizeof(sd8902_read_buff))
        {
            buf[i] = sd8902_read_data(sd8902_read_buff[index + i]);
        }
        else
        {
            return FALSE;
        }
    }
	
		return TRUE;
}

/**
功能：sd8902写多个寄存器数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度
返回值：TRUE-成功
**/
uint8_t sd8902_write_multi_bytes(uint8_t reg_addr, uint16_t *buf, uint8_t len)
{
	uint8_t index = 0;
    for(uint8_t i = 0; i < sizeof(sd8902_write_buff); i++)
    {
        if(sd8902_write_buff[i] == reg_addr)
        {
            index = i;
            break;
        }
    }

	 for(uint8_t i = 0; i < len; i++)
    {
        if(index + i < sizeof(sd8902_write_buff))
        {
			
				sd8902_write_data(sd8902_write_buff[index + i], buf[i]);
			
        }
        else
        {
            return FALSE;
        }
    }

	return TRUE;
	
}

/**
功能：与上位机交互读取sd8902数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度
返回值：TRUE-成功
**/
uint8_t RTC_ReadRAM_8902(uint8_t addr, uint16_t *data, uint8_t len)
{
	
	return sd8902_read_multi_bytes(addr,data,len);
	
}

/**
功能：与上位机交互写sd8902数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度
返回值：TRUE-成功
**/
uint8_t RTC_WriteRAM_8902(uint8_t addr,uint16_t *data,uint8_t len)
{
	uint16_t buf=0x00;
	if(sd8902_write_multi_bytes(0x8E,&buf,1)!=TRUE)
		return FALSE;	

	if(sd8902_write_multi_bytes(addr,data,len)!=TRUE)
		return FALSE;

	buf=0x80;
	if(sd8902_write_multi_bytes(0x8E,&buf,1)!=TRUE)
		return FALSE;
	
	return TRUE;
	
}

/**
功能：sd8902打开充电功能
修改：sjm
参数：-
返回值：-
**/
void sd8902_charge_on(void)
{
	sd8902_write_data(0x90,0xA9);
	OLED_ShowImage(110, 40, 12, 20, Charge);
}

/**
功能：sd8902关闭充电功能
修改：sjm
参数：-
返回值：-
**/
void sd8902_charge_off(void)
{
	sd8902_write_data(0x90,0x00);
	OLED_ClearArea(110,40,12,20);
}

