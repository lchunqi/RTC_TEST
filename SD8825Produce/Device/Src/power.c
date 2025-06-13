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
{//VDD_I_S0 ~ VDD_I_S5 总共6，将该列拉高，Pmos管将关闭
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



uint8_t rtc_otp_power(float vol)//配置dac电压
{
    HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, vol / L2722_GAIN / 3.3 * 4096);
    HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);
    return 1;
}

void rtc_group_power(uint8_t channel, uint8_t mode)// SD8825没有BAT引脚
{
    if (channel < 1) { channel = 1; }
    if (channel > MAX_CHIP_NUMBER) { channel = MAX_CHIP_NUMBER; }
    switch (mode) {
        case POWER_OFF:  //全部关闭电源
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);//VCE_SDA选择VCE通道，但VCE均关闭
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 |GPIO_PIN_9 |GPIO_PIN_12 |GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15, GPIO_PIN_SET);//VCE控制引脚拉高，从而变为低电平
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);//INT 拉低
            HAL_GPIO_WritePin(GPIOC, 0x3fff, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOE, 0x001f, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10 |GPIO_PIN_11 |GPIO_PIN_12 |GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15, GPIO_PIN_RESET);
            rtc_horizontal_vdd_control(0, ALL_CLOSE);
            rtc_vertical_control(0, ALL_CLOSE);
        break;
        case POWER_I2C:  //I2C通信模式，也就是正常模式
            rtc_horizontal_vdd_control((channel - 1) / 10, ONE_OPEN_OTHER_CLOSE);// rtc_horizontal_vdd_control(0, ALL_OPEN);//全部行打开
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);//VDD_TEST模拟开关切换到VDD_SW通道
            rtc_vertical_control((channel - 1) % 10, ONE_OPEN_OTHER_CLOSE);//rtc_vertical_control(0, ALL_OPEN);//全部列打开
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);//VCE_SDA选择SDA通道
            rtc_otp_power(3.3);//输出3.3V，防止电压倒灌到寄生二极管
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);//INT 拉高
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 |GPIO_PIN_9 |GPIO_PIN_12 |GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15, GPIO_PIN_SET);//VCE控制引脚拉高，从而变为低电平
            // rtc_set_channel(channel, RTC_COMMON);
        break;
        case POWER_VDD_I:    //VDD电流测试模式
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);//开关选择2.5V
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);//INT 拉低
            //选择行开关
            rtc_horizontal_vdd_control((channel - 1) / 10, ONE_OPEN_OTHER_CLOSE);
            //选择列开关
            rtc_vertical_control((channel - 1) % 10, ONE_OPEN_OTHER_CLOSE);
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);//VCE_SDA选择VCE通道，但VCE均关闭
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 |GPIO_PIN_9 |GPIO_PIN_12 |GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15, GPIO_PIN_SET);//VCE控制引脚拉高，从而变为低电平
            HAL_GPIO_WritePin(GPIOC, 0x3fff, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOE, 0x001f, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10 |GPIO_PIN_11 |GPIO_PIN_12 |GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15, GPIO_PIN_RESET);
        break;
        case POWER_TEST://烧录后的测试
            rtc_horizontal_vdd_control(0, ALL_OPEN);//全部行打开
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);//VDD_TEST模拟开关切换到VDD_SW通道
            rtc_vertical_control(0, ALL_OPEN);//全部列打开
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);//VCE_SDA选择SDA通道
            rtc_otp_power(3.3);//输出3.3V，防止电压倒灌到寄生二极管
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);//INT 拉高
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 |GPIO_PIN_9 |GPIO_PIN_12 |GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15, GPIO_PIN_SET);//VCE控制引脚拉高，从而变为低电平
            // rtc_set_channel(channel, RTC_COMMON);
        break;

        default:break;
    }
}