#ifndef __SD2506_H
#define __SD2506_H

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct 
{
  uint8_t year;
  uint8_t mon;
  uint8_t day;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint16_t tick;
}RealTime;

#define RTC_3078_ADDRESS    0x64

uint8_t rtc_write_lock_on(void);
uint8_t rtc_write_lock_off(void);
uint8_t rtc_write_date(RealTime* setRTC);
uint8_t rtc_read_date(RealTime* psRTC);
void PrintfTime(RealTime* rtc);
uint8_t rtc_read_temp(uint8_t* temp);
uint8_t rtc_set_bat_charge(uint8_t mode);
uint8_t rtc_out_4096(void);
uint8_t rtc_disable_int(void);
uint8_t rtc_read_vbat(int* vbat);

#ifdef __cplusplus
}
#endif

#endif 