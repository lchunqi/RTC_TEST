#include "main.h"
#include "power.h"
#include "rtc_i2c.h"
#include "rtc.h"
#include "current.h"
#include "flash.h"

extern DAC_HandleTypeDef hdac;

void rtc_group_vdd(uint8_t mode) 
{
    if (mode == 0) {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);//3.3V 
    } else {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);//5V 
    }
}

void rtc_vertical_open_one(uint8_t left)//left: 0 ~ 9
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0 << left, GPIO_PIN_RESET);
}

void rtc_vertical_close_one(uint8_t left)//left: 0 ~ 9
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0 << left, GPIO_PIN_SET);
}

void rtc_vertical_control(uint8_t lineNum, uint8_t mode)
{
    uint8_t i = 0;
    if (mode == ALL_CLOSE) {
        HAL_GPIO_WritePin(GPIOD, 0x03ff, GPIO_PIN_SET);
    } else if (mode == ALL_OPEN) {
        HAL_GPIO_WritePin(GPIOD, 0x03ff, GPIO_PIN_RESET);
    } else if (mode == ONE_OPEN_OTHER_CLOSE) {
        for (i = 0; i <= 9; i++) {
            if (i == lineNum) {
                rtc_vertical_open_one(i);
            } else {
                rtc_vertical_close_one(i);
            }
        }
    } else {
        for (i = 0; i <= 9; i++) {
            if (i == lineNum) {
                rtc_vertical_close_one(i);
            } else {
                rtc_vertical_open_one(i);
            }
        }
    }
}

void rtc_horizontal_vdd_control(uint8_t lineNum, uint8_t mode)
{//VDD_I_S0 ~ VDD_I_S5 �ܹ�6�����������ߣ�Pmos�ܽ��ر�
    uint16_t buff;
    if (mode == ALL_CLOSE) {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11| GPIO_PIN_12 , GPIO_PIN_SET);
    } else if (mode == ALL_OPEN) {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11| GPIO_PIN_12, GPIO_PIN_RESET);
    } else if (mode == ONE_OPEN_OTHER_CLOSE) {
        buff = GPIO_PIN_8 << lineNum;
        rtc_horizontal_vdd_control(0, ALL_CLOSE);
        HAL_GPIO_WritePin(GPIOE, buff, GPIO_PIN_RESET);
    } else {
        buff = GPIO_PIN_8 << lineNum;
        rtc_horizontal_vdd_control(0, ALL_OPEN);
        HAL_GPIO_WritePin(GPIOE, buff, GPIO_PIN_SET);
    }
}



uint8_t rtc_otp_power(float vol)//����dac��ѹ
{
    HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, vol / L2722_GAIN / 3.3 * 4096);
    HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);
    return 1;
}

void rtc_group_power(uint8_t channel, uint8_t mode)// SD8825û��BAT����
{
    if (channel < 1) { channel = 1; }
    if (channel > MAX_CHIP_NUMBER) { channel = MAX_CHIP_NUMBER; }
    switch (mode) {
        case POWER_OFF:  //ȫ���رյ�Դ
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);//VCE_SDAѡ��VCEͨ������VCE���ر�
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 |GPIO_PIN_9 |GPIO_PIN_12 |GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15, GPIO_PIN_SET);//VCE�����������ߣ��Ӷ���Ϊ�͵�ƽ
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);//INT ����
            HAL_GPIO_WritePin(GPIOC, 0x3fff, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOE, 0x001f, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10 |GPIO_PIN_11 |GPIO_PIN_12 |GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15, GPIO_PIN_RESET);
            rtc_horizontal_vdd_control(0, ALL_CLOSE);
            rtc_vertical_control(0, ALL_CLOSE);
        break;
        case POWER_I2C:  //I2Cͨ��ģʽ��Ҳ��������ģʽ
            rtc_horizontal_vdd_control((channel - 1) / 10, ONE_OPEN_OTHER_CLOSE);// rtc_horizontal_vdd_control(0, ALL_OPEN);//ȫ���д�
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);//VDD_TESTģ�⿪���л���VDD_SWͨ��
            rtc_vertical_control((channel - 1) % 10, ONE_OPEN_OTHER_CLOSE);//rtc_vertical_control(0, ALL_OPEN);//ȫ���д�
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);//VCE_SDAѡ��SDAͨ��
            rtc_otp_power(3.3);//���3.3V����ֹ��ѹ���ൽ����������
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);//INT ����
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 |GPIO_PIN_9 |GPIO_PIN_12 |GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15, GPIO_PIN_SET);//VCE�����������ߣ��Ӷ���Ϊ�͵�ƽ
            // rtc_set_channel(channel, RTC_COMMON);
        break;
        case POWER_VDD_I:    //VDD��������ģʽ
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);//����ѡ��2.5V
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);//INT ����
            //ѡ���п���
            rtc_horizontal_vdd_control((channel - 1) / 10, ONE_OPEN_OTHER_CLOSE);
            //ѡ���п���
            rtc_vertical_control((channel - 1) % 10, ONE_OPEN_OTHER_CLOSE);
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);//VCE_SDAѡ��VCEͨ������VCE���ر�
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 |GPIO_PIN_9 |GPIO_PIN_12 |GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15, GPIO_PIN_SET);//VCE�����������ߣ��Ӷ���Ϊ�͵�ƽ
            HAL_GPIO_WritePin(GPIOC, 0x3fff, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOE, 0x001f, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10 |GPIO_PIN_11 |GPIO_PIN_12 |GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15, GPIO_PIN_RESET);
        break;
        case POWER_TEST://��¼��Ĳ���
            rtc_horizontal_vdd_control(0, ALL_OPEN);//ȫ���д�
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);//VDD_TESTģ�⿪���л���VDD_SWͨ��
            rtc_vertical_control(0, ALL_OPEN);//ȫ���д�
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);//VCE_SDAѡ��SDAͨ��
            rtc_otp_power(3.3);//���3.3V����ֹ��ѹ���ൽ����������
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);//INT ����
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 |GPIO_PIN_9 |GPIO_PIN_12 |GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15, GPIO_PIN_SET);//VCE�����������ߣ��Ӷ���Ϊ�͵�ƽ
            // rtc_set_channel(channel, RTC_COMMON);
        break;

        default:break;
    }
}