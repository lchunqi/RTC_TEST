#ifndef __POWER_H
#define __POWER_H

#ifdef __cplusplus
 extern "C" {
#endif
#define L2722_GAIN  4.1
typedef enum {
    POWER_OFF = 0,  //ȫ���رյ�Դ
    POWER_I2C,      //I2Cͨ��ģʽ��Ҳ��������ģʽ
    POWER_BAT_I,    //BAT��������ģʽ
    POWER_VDD_I,    //VDD��������ģʽ
    POWER_TEST,     //��¼��Ĳ���
} RtcPowerMode;

typedef enum {
    ALL_CLOSE = 0,
    ALL_OPEN,
    ONE_OPEN_OTHER_CLOSE,
    ONE_CLOSE_OTHER_OPEN,
} RtcGroupState;

void rtc_group_power(uint8_t channel, uint8_t mode);
uint8_t rtc_otp_power(float vol);//����dac��ѹ

#ifdef __cplusplus
}
#endif

#endif 