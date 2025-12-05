#include "sd3068.h"


rtc_exit_code rtc_f32768(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x01;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;

		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}


rtc_exit_code rtc_f4096(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x02;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

rtc_exit_code rtc_f1024(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x03;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;

		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

rtc_exit_code rtc_f64(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x04;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;

		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

rtc_exit_code rtc_f32(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x05;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;

		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;		
		
		return RTC_OK;

}

rtc_exit_code rtc_f16(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x06;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

rtc_exit_code rtc_f8(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x07;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

rtc_exit_code rtc_f4(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x08;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

rtc_exit_code rtc_f2(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x09;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

rtc_exit_code rtc_f1(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x0A;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;

		return RTC_OK;

}

rtc_exit_code rtc_f1_2(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x0B;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

rtc_exit_code rtc_f1_4(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x0C;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

rtc_exit_code rtc_f1_8(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x0D;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

rtc_exit_code rtc_f1_16(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x0E;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}

rtc_exit_code rtc_f1s(i2c_device_bus IIC_x)
{
		uint8_t buf=0xA1;
	
		if(RTC_WriteOn(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_write_addr(RTC_CTR2,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
			buf=0x0F;
		if(RTC_write_addr(RTC_CTR3,&buf,1,IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		if(RTC_WriteOff(IIC_x)!=RTC_OK)
				return RTC_FAIL;
		
		return RTC_OK;

}


rtc_exit_code rtc_f4096_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f1_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f1s_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f1024_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f64_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f32_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f16_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f8_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f4_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f2_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f1_2_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f1_4_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f1_8_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f1_16_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

rtc_exit_code rtc_f32768_off(i2c_device_bus IIC_x)
{
		uint8_t buf=0x80;
	
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

