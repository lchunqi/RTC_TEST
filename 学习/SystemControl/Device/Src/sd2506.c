#include "main.h"
#include "sd2506.h"
#include <string.h>
#include "soft_i2c.h"


uint8_t rtc_write_lock_on(void)
{		
    uint8_t buffer;
    buffer = 0x80;
    if (i2c_write_multi_bytes(0x64, 0x10, &buffer, 1) != I2C_OK) {return I2C_NO_ACK;}
    buffer = 0xff;
    i2c_write_multi_bytes(0x64, 0x0f, &buffer, 1);
    return	I2C_OK;
}

uint8_t rtc_write_lock_off(void)
{		
    uint8_t buffer;
    buffer = 0x7B;
    if (i2c_write_multi_bytes(0x64, 0x0f, &buffer, 1) != I2C_OK) {return I2C_NO_ACK;}
    buffer = 0x0;
    i2c_write_multi_bytes(0x64, 0x10, &buffer, 1);
    return	I2C_OK;
}

uint8_t rtc_read_temp(uint8_t* temp)
{
	uint8_t time = 20;
	uint8_t buffer = 0;


	if(rtc_write_lock_on() != I2C_OK)
		return I2C_NO_ACK;
	
    buffer = 0x10;
    if (i2c_write_multi_bytes(RTC_3078_ADDRESS, 0x17, &buffer, 1) != I2C_OK) 
		return I2C_NO_ACK;
	
	if(rtc_write_lock_off() != I2C_OK)
		return I2C_NO_ACK;
	
	while(time --)
	{
		HAL_Delay(1);
		
        i2c_read_multi_bytes(RTC_3078_ADDRESS, 0x17, &buffer, 1);
		if (!(buffer & 0x20)) {
            i2c_read_multi_bytes(RTC_3078_ADDRESS, 0x16, &buffer, 1);
            *temp = buffer;
            return I2C_OK;
        }
	}
	
	return I2C_NO_ACK;
}

uint8_t rtc_set_bat_charge(uint8_t mode)
{
	uint8_t buffer = 0;

	if(rtc_write_lock_on() != I2C_OK)
		return I2C_NO_ACK;
		
	if (mode == 0) { 
		buffer = 0x00; 
	} else {
		buffer = 0x82;
	}
    if (i2c_write_multi_bytes(RTC_3078_ADDRESS, 0x18, &buffer, 1) != I2C_OK) 
		return I2C_NO_ACK;
	
	if(rtc_write_lock_off() != I2C_OK)
		return I2C_NO_ACK;
	
	return I2C_OK;
}


uint8_t rtc_read_vbat(int* vbat)
{
	uint8_t time = 20;
	uint8_t buffer[2] = {0};
    int value = 0;
    *vbat = 0;

	if(rtc_write_lock_on() != I2C_OK)
		return I2C_NO_ACK;
	
    buffer[0] = 0x10;
    if (i2c_write_multi_bytes(RTC_3078_ADDRESS, 0x19, buffer, 1) != I2C_OK) 
		return I2C_NO_ACK;
	
	if(rtc_write_lock_off() != I2C_OK)
		return I2C_NO_ACK;
	
	while (time --)
	{
		HAL_Delay(1);
		
        i2c_read_multi_bytes(RTC_3078_ADDRESS, 0x17, buffer, 1);
		if (!(buffer[0] & 0x20)) {
            i2c_read_multi_bytes(RTC_3078_ADDRESS, 0x1A, buffer, 2);
            value = buffer[0] & 0x80;
            value = (value << 1) + buffer[1];
            *vbat = value;
            return I2C_OK;
        }
	}
	
	return I2C_NO_ACK;

}
uint8_t rtc_read_date(RealTime* psRTC)
{
    uint8_t buffer[7] = {0};
    
    if (i2c_read_multi_bytes(RTC_3078_ADDRESS, 0x00, buffer, 7)!= I2C_OK) {
        return I2C_NO_ACK;
    } 
    psRTC->sec = (buffer[0] / 16) * 10 + (buffer[0] % 16);
    psRTC->min = (buffer[1] / 16) * 10 + (buffer[1] % 16);
    if (buffer[2] & 0x80) {//
        buffer[2] = buffer[2] & 0x7f;
        psRTC->hour = (buffer[2] / 16) * 10 + (buffer[2] % 16);
    } else {
        if (buffer[2] & 0x20) {//
            buffer[2] = buffer[2] & 0x1f;
            psRTC->hour = (buffer[2] / 16) * 10 + (buffer[2] % 16) + 12;
        } else {//
            buffer[2] = buffer[2] & 0x1f;
            psRTC->hour = (buffer[2] / 16) * 10 + (buffer[2] % 16);
        }
    }
    // psRTC->week = (buffer[3] / 16) * 10 + (buffer[3] % 16);
    psRTC->day = (buffer[4] / 16) * 10 + (buffer[4] % 16);
    psRTC->mon = (buffer[5] / 16) * 10 + (buffer[5] % 16);
    psRTC->year = (buffer[6] / 16) * 10 + (buffer[6] % 16);
    return I2C_OK;
}

uint8_t rtc_write_date(RealTime* setRTC)	
{                               
    uint8_t buffer[7] = {0};
    rtc_write_lock_on();
    buffer[0] = (setRTC->sec / 10) * 16 + (setRTC->sec % 10);
    buffer[1] = (setRTC->min / 10) * 16 + (setRTC->min % 10);
    buffer[2] = (setRTC->hour / 10) * 16 + (setRTC->hour % 10) + 0x80;//
    buffer[3] = 0;
    buffer[4] = (setRTC->day / 10) * 16 + (setRTC->day % 10);
    buffer[5] = (setRTC->mon / 10) * 16 + (setRTC->mon % 10);
    buffer[6] = (setRTC->year / 10) * 16 + (setRTC->year % 10);
    if (i2c_write_multi_bytes(RTC_3078_ADDRESS, 0x00, buffer, 7)!= I2C_OK) {
        rtc_write_lock_off();
        return I2C_NO_ACK;
    } 
    rtc_write_lock_off();
    return I2C_OK;
}

void PrintfTime(RealTime* rtc)
{
	UsbPrintf("\n20%d/%d/%d %d:%d:%d\n", rtc->year, rtc->mon, rtc->day, rtc->hour, rtc->min, rtc->sec);
}



uint8_t rtc_out_4096(void)
{
    uint8_t buffer[2] = {0xA1, 0x02};
    rtc_write_lock_on();
	if (i2c_write_multi_bytes(RTC_3078_ADDRESS, 0x10, buffer, 2)!= I2C_OK) {
        rtc_write_lock_off();
        return I2C_NO_ACK;
    } 
    rtc_write_lock_off();
	
	return I2C_OK;
}

uint8_t rtc_disable_int(void)
{
    uint8_t buffer[2] = {0x80, 0x00};
    rtc_write_lock_on();
	if (i2c_write_multi_bytes(RTC_3078_ADDRESS, 0x10, buffer, 2)!= I2C_OK) {
        rtc_write_lock_off();
        return I2C_NO_ACK;
    } 
    rtc_write_lock_off();
	
	return I2C_OK;
}
