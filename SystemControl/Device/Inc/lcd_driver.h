#ifndef __LCD_DRIVER_H
#define __LCD_DRIVER_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#define DEFAULT_SCREEN_ID       0
#define DEFAULT_OFF_LIGHT       40
#define DEFAULT_ON_LIGHT        120
#define DEFAULT_SELECT_COLOR    0XCFE6
#define DEFAULT_FREE_COLOR      0XDEFB

#define YELLOW_COLOR            0X01FF
#define MAX_FUNCTION_NUMBER         9

#define WIFI_TEXT_ID            12
#define VOLTAGE_TEXT_ID         13
#define FUNCTION_SELECT_ID      2
#define EXCEL_OF_SCREEN         3
#define EXCEL_TEXT_ID           6
#define EXCEL_V_NUMBER          10
#define EXCEL_H_NUMBER          7

typedef struct {
    uint8_t board;
    float systemTemp;
    uint8_t okNumber;
    uint8_t chipName;
    int chipTemp;
    char state[20];
    char message[20];
}ExcelMessage;

typedef enum {
    DEFAULT_START = 0,
    SCREEN_FUNCTION_SELECT,
    SCREEN_VOLTAGE_SELECT,
    SCREEN_MASSAGE_SHOW,
    SCREEN_MACHINE_TEST,
    SCREEN_INPUT_NUMBER 
} ScreenName;

/*!
*  \brief  自动调节背光亮度
*  \param  enable 使能
*  \param  bl_off_level 待机亮度
*  \param  bl_on_level  激活亮度
*  \param  bl_on_time  未触摸时间 xs
*/
void SetPowerSaving(uint8_t enable, uint8_t bl_off_level, uint8_t bl_on_level, uint8_t  bl_on_time);

/*!
*  \brief      设置当前画面
*  \param  screen_id 画面ID
*/
void SetScreen(uint16_t screen_id);
/*!
*  \brief     设置文本值
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  str 文本值
*/
void SetTextValue(uint16_t screen_id, uint16_t control_id, char *str);
/*!
*  \brief     设置控件背景色
*  \details  支持控件：进度条、文本
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  color 背景色
*/
void SetControlBackColor(uint16_t screen_id,uint16_t control_id, uint16_t color);

void clear_function_select(void);

void function_one_select(uint16_t control_id);
void function_one_select1(uint16_t control_id);
void lcd_init_message_excel(void);
void excel_change_one_line(uint8_t screen_id, uint8_t control_id, ExcelMessage message);
void excel_clear(uint8_t screen_id, uint8_t control_id);
void excel_add_one_line(uint8_t screen_id, uint8_t control_id, ExcelMessage message);
void show_chip_id(void);


#ifdef __cplusplus
}
#endif

#endif 