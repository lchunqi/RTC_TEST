#ifndef __POWER_H
#define __POWER_H

#ifdef __cplusplus
 extern "C" {
#endif
#define L2722_GAIN  4.1
typedef enum {
    POWER_OFF = 0,  //全部关闭电源
    POWER_I2C,      //I2C通信模式，也就是正常模式
    POWER_BAT_I,    //BAT电流测试模式
    POWER_VDD_I,    //VDD电流测试模式
} RtcPowerMode;

typedef enum {
    ALL_CLOSE = 0,
    ALL_OPEN,
    ONE_OPEN_OTHER_CLOSE,
    ONE_CLOSE_OTHER_OPEN,
} RtcGroupState;

void rtc_group_power(uint8_t channel, uint8_t mode);
uint8_t rtc_otp_power(float vol);//配置dac电压

#ifdef __cplusplus
}
#endif

#endif 