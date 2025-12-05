#include "main.h"
#include "power.h"
#include "rtc_i2c.h"
#include "rtc.h"
#include "current.h"

extern DAC_HandleTypeDef hdac;
void rtc_group_vdd(uint8_t mode) 
{
    if (mode == 0) {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);//3.3V 
    } else {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);//5V 
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

uint8_t rtc_otp_power(float vol)//配置dac电压
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
        case POWER_OFF:  //全部关闭电源
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13, GPIO_PIN_SET);//切换模拟开关至全部悬空状态,使得VCE关闭
            rtc_horizontal_bat_control(0, ALL_CLOSE);
            rtc_horizontal_vdd_control(0, ALL_CLOSE);
            rtc_vertical_control(0, ALL_CLOSE);
        break;
        case POWER_I2C:  //I2C通信模式，也就是正常模式
            rtc_horizontal_bat_control(0, ALL_CLOSE);//bat电流关闭
            rtc_horizontal_vdd_control(0, ALL_OPEN);
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);//VDD打开
            rtc_vertical_control(0, ALL_OPEN);//全部列打开
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);//VCE关闭
            rtc_otp_power(3.3);//输出5V，防止电压倒灌到寄生二极管
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);//INT拉高
        break;
        case POWER_DOWNLOAD://OTP烧录模式
            rtc_horizontal_bat_control(0, ALL_CLOSE);//bat电流关闭
            // rtc_horizontal_vdd_control(0, ALL_OPEN);
            // rtc_vertical_control(0, ALL_OPEN);//全部列打开
            rtc_horizontal_vdd_control((channel - 1) / NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);
            rtc_vertical_control((channel - 1) % NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);//选择列开关
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);//VDD切换到VDD_SW
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);//VCE打开
        break;
        case POWER_BAT_I:    //BAT电流测试模式
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);//VDD切换到VDD_SW
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);//INT 拉低
            //选择行开关
            rtc_horizontal_bat_control((channel - 1) / NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);
            rtc_horizontal_vdd_control(0, ALL_CLOSE);//VDD通路关闭
            //选择列开关
            rtc_vertical_control((channel - 1) % NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13, GPIO_PIN_SET);//切换模拟开关至全部悬空状态,使得VCE关闭
            HAL_GPIO_WritePin(GPIOC, 0x03ff, GPIO_PIN_RESET);//SCL 拉低
        break;
        case POWER_VDD_I:    //VDD电流测试模式
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);//开关选择2.5V
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);//INT 拉低
            //选择行开关
            rtc_horizontal_vdd_control((channel - 1) / NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);
            rtc_horizontal_bat_control(0, ALL_CLOSE);//battery通路关闭
            //选择列开关
            rtc_vertical_control((channel - 1) % NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13, GPIO_PIN_SET);//切换模拟开关至全部悬空状态,使得VCE关闭
            HAL_GPIO_WritePin(GPIOC, 0x03ff, GPIO_PIN_RESET);//SCL 拉低
        break;
        case POWER_CAP_TEST:  //温箱内部测试，只打开1个芯片的供电
            rtc_horizontal_bat_control(0, ALL_CLOSE);//bat电流关闭
            rtc_horizontal_vdd_control((channel - 1) / NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);//VDD打开
            rtc_vertical_control((channel - 1) % NUMBER_VERTICAL, ONE_OPEN_OTHER_CLOSE);//选择列开关
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);//VCE关闭
            rtc_otp_power(3.3);//输出5V，防止电压倒灌到寄生二极管
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);//INT拉高
        break;
        default:break;
    }
}