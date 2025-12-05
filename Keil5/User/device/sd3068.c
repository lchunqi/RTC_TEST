#include "config.h"

/**
功能：sd3068输出32768Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f32768(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x01;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;

		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_32768);
		
		return RTC_OK;

}

/**
功能：sd3068输出4096Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f4096(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x02;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_32768);
		
		return RTC_OK;

}

/**
功能：sd3068输出1024Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1024(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x03;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;

		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_32768);
		
		return RTC_OK;

}

/**
功能：sd3068输出64Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f64(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x04;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;

		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_64);
		
		return RTC_OK;

}

/**
功能：sd3068输出32Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f32(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x05;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;

		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;	
		
		OLED_ShowImage(0, 0, 128, 64, wave_32);
		
		return RTC_OK;

}

/**
功能：sd3068输出16Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f16(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x06;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_32);
		
		return RTC_OK;

}

/**
功能：sd3068输出8Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f8(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x07;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_8);
		
		return RTC_OK;

}

/**
功能：sd3068输出4Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f4(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x08;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_4);
		
		return RTC_OK;

}

/**
功能：sd3068输出2Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f2(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x09;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_4);
		
		return RTC_OK;

}

/**
功能：sd3068输出1Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x0A;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_1);

		return RTC_OK;

}

/**
功能：sd3068输出1/2Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1_2(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x0B;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_1_2);
		
		return RTC_OK;

}

/**
功能：sd3068输出1/4Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1_4(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x0C;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_1_2);
		
		return RTC_OK;

}

/**
功能：sd3068输出1/8Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1_8(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x0D;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_1_2);
		
		return RTC_OK;

}

/**
功能：sd3068输出1/16Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1_16(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x0E;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_1_2);
		
		return RTC_OK;

}

/**
功能：sd3068输出1s频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1s(i2c_device_bus IIC_x)
{
		uint16_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x0F;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		OLED_ShowImage(0, 0, 128, 64, wave_1);
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出4096Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f4096_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出1Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		return RTC_OK;

}

/**
功能：sd3068关闭输出1s频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1s_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x00;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出1024Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1024_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出64Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f64_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出32Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f32_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出16Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f16_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出8Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f8_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出4Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f4_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出2Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f2_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出1/2Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1_2_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出1/4Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1_4_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出1/8Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1_8_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出1/16Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f1_16_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

/**
功能：sd3068关闭输出32768Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/
rtc_exit_code rtc_f32768_off(i2c_device_bus IIC_x)
{
		uint16_t buf=0x80;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x00;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

