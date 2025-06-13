#include "main.h"
#include "power.h"
#include "rtc_i2c.h"
#include "rtc.h"
#include "current.h"

extern DAC_HandleTypeDef hdac;
void rtc_group_vdd(uint8_t mode) 
{
    if (mode == 0) {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_RESET);//3.3V 
    } else {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_SET);//5V 
    }
}

void rtc_vertical_control(uint8_t lineNum, uint8_t mode)
{
    if (mode == ALL_CLOSE) {
        HAL_GPIO_WritePin(GPIOD, 0x03ff, GPIO_PIN_SET);
    } else if (mode == ALL_OPEN) {
        HAL_GPIO_WritePin(GPIOD, 0x03ff, GPIO_PIN_RESET);
    } else if (mode == ONE_OPEN_OTHER_CLOSE) {
        rtc_vertical_control(lineNum, ALL_CLOSE);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0 << lineNum, GPIO_PIN_RESET);
    } else {
        rtc_vertical_control(lineNum, ALL_OPEN);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0 << lineNum, GPIO_PIN_SET);
    }
}

void rtc_horizontal_vdd_control(uint8_t lineNum, uint8_t mode)
{
    if (mode == ALL_CLOSE) {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11| GPIO_PIN_12 |GPIO_PIN_13, GPIO_PIN_SET);
    } else if (mode == ALL_OPEN) {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11| GPIO_PIN_12 |GPIO_PIN_13, GPIO_PIN_RESET);
    } else if (mode == ONE_OPEN_OTHER_CLOSE) {
        rtc_horizontal_vdd_control(lineNum, ALL_CLOSE);
        if (lineNum <= 5) {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8 << lineNum, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2 << (lineNum - 6), GPIO_PIN_RESET);
        }
    } else {
        rtc_horizontal_vdd_control(lineNum, ALL_OPEN);
        if (lineNum <= 5) {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8 << lineNum, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2 << (lineNum - 6), GPIO_PIN_SET);
        }
    }
}

void rtc_horizontal_bat_control(uint8_t lineNum, uint8_t mode)
{
    if (mode == ALL_CLOSE) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_SET);
    } else if (mode == ALL_OPEN) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_RESET);
    } else if (mode == ONE_OPEN_OTHER_CLOSE) {
        rtc_horizontal_bat_control(lineNum, ALL_CLOSE);
        if (lineNum <= 7) {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2 << lineNum, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 << (lineNum - 8), GPIO_PIN_RESET);
        }
    } else {
        rtc_horizontal_bat_control(lineNum, ALL_OPEN);
        if (lineNum <= 7) {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2 << lineNum, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 << (lineNum - 8), GPIO_PIN_SET);
        }
    }
}

uint8_t rtc_otp_power(float vol)//����dac��ѹ
{
    HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, vol / L2722_GAIN / 3.3 * 4096);
    HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);
    return 1;
}

void rtc_group_power(uint8_t channel, uint8_t mode)
{
    if (channel < 1) { channel = 1; }
    if (channel > 100) { channel = 100; }
    switch (mode) {
        case POWER_OFF:  //ֻ�ܿ���ĳ��оƬ�������������ͣ�����оƬ����Ϊ����
            rtc_horizontal_bat_control(0, ALL_CLOSE);
            rtc_horizontal_vdd_control(0, ALL_CLOSE);
            rtc_vertical_control(0, ALL_CLOSE);
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);//INT ����
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);//
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);//SDA ����
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);//CLK FREQ ����
            set_switch_channel(channel - 1);//ģ�⿪��ѡ��ĳһ��оƬ
        break;
        case POWER_I2C:  //I2Cͨ��ģʽ��Ҳ��������ģʽ
#if (VERSION_SELECT == VERSION_SD8801)    
            rtc_horizontal_vdd_control(0, ALL_OPEN);
            rtc_horizontal_bat_control(0, ALL_CLOSE);//batteryͨ·�ر�
#else 
            rtc_horizontal_bat_control(0, ALL_OPEN);
            rtc_horizontal_vdd_control(0, ALL_CLOSE);
#endif
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);//VDD��
            rtc_vertical_control(0, ALL_OPEN);//ȫ���д�
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);//VCE�ر�
            rtc_otp_power(3.3);//���3.3V
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);//INT����
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);//
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);//SDA ����
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);//CLK FREQ ����
            set_switch_channel(channel - 1);
        break;
        case POWER_BAT_I:    //BAT��������ģʽ  V1.2�汾���Բ���bat������V1.3�Ժ��ܲ⣬�����л�8800��8801��VDD���ţ���8800��VDD
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);//����ѡ��2.5V
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);//INT ����

            rtc_horizontal_bat_control((channel - 1) / NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);
            rtc_horizontal_vdd_control(0, ALL_CLOSE);
            rtc_vertical_control((channel - 1) % NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);//INT ����
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);//
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);//SDA ����
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);//CLK FREQ ����
            set_switch_channel(channel - 1);//ģ�⿪��ѡ��ĳһ��оƬ
        break;
        case POWER_VDD_I:    //VDD��������ģʽ            �����л�8800��8801��VDD���ţ���8801��VDD
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);//����ѡ��2.5V
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);//INT ����

            rtc_horizontal_vdd_control((channel - 1) / NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);
            rtc_horizontal_bat_control(0, ALL_CLOSE);//batteryͨ·�ر�
            rtc_vertical_control((channel - 1) % NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);//INT ����
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);//
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);//SDA ����
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);//CLK FREQ ����
            set_switch_channel(channel - 1);//ģ�⿪��ѡ��ĳһ��оƬ
        break;
        case POWER_CAP_TEST:  //�����ڲ����ԣ�ֻ��1��оƬ�Ĺ���
#if (VERSION_SELECT == VERSION_SD8801)    
            rtc_horizontal_vdd_control((channel - 1) / NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);
            rtc_horizontal_bat_control(0, ALL_CLOSE);//batteryͨ·�ر�
#else 
            rtc_horizontal_bat_control((channel - 1) / NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);
            rtc_horizontal_vdd_control(0, ALL_CLOSE);
#endif
            rtc_vertical_control((channel - 1) % NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);//ѡ���п���
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);//VDD��
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);//VCE�ر�
            rtc_otp_power(3.3);//���3.3V����ֹ��ѹ���ൽ����������
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);//INT����
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);//
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);//SDA ����
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);//CLK FREQ ����
            set_switch_channel(channel - 1);
        break;
        default:break;
    }
}