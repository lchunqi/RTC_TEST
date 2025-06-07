/************************************版权申明********************************************
**                             广州大彩光电科技有限公司
**                             http://www.gz-dc.com
**-----------------------------------文件信息--------------------------------------------
** 文件名称:   hmi_driver.c
** 修改时间:   2018-05-18
** 文件说明:   用户MCU串口驱动函数库
** 技术支持：  Tel: 020-82186683  Email: hmi@gz-dc.com Web:www.gz-dc.com
--------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------*/
#include "lcd_driver.h"
#include "control.h"
#include "string.h"
#include "stdio.h"

#define LCD_UART_START          0XEE
#define LCD_UART_END            0XFFFCFFFF
#define LCD_BUFFER_LENGTH       256

extern UART_HandleTypeDef huart2;
typedef struct {
uint16_t length;
uint8_t buffer[LCD_BUFFER_LENGTH];
} LcdBuffer;
LcdBuffer g_lcdUart = {0};

void clear_buffer(void)
{
    g_lcdUart.length = 0;
    memset(g_lcdUart.buffer, 0x00, LCD_BUFFER_LENGTH);
}

void buffer_add_start(void)
{
    g_lcdUart.length = 0;
    g_lcdUart.buffer[g_lcdUart.length++] = LCD_UART_START;
}

void buffer_add_end(void)
{
    if (g_lcdUart.length + 4 < LCD_BUFFER_LENGTH) {
        g_lcdUart.buffer[g_lcdUart.length++] = 0xFF;
        g_lcdUart.buffer[g_lcdUart.length++] = 0xFC;
        g_lcdUart.buffer[g_lcdUart.length++] = 0xFF;
        g_lcdUart.buffer[g_lcdUart.length++] = 0xFF;
    }
}
void buffer_add_1_byte(uint8_t data)
{
    if (g_lcdUart.length + 1 < LCD_BUFFER_LENGTH) {
        g_lcdUart.buffer[g_lcdUart.length++] = data;
    }
}
void buffer_add_2_byte(uint16_t data)
{
    if (g_lcdUart.length + 2 < LCD_BUFFER_LENGTH) {
        g_lcdUart.buffer[g_lcdUart.length++] = data >> 8;
        g_lcdUart.buffer[g_lcdUart.length++] = data % 256;
    }
}
void buffer_add_4_byte(uint32_t data)
{
    if (g_lcdUart.length + 4 < LCD_BUFFER_LENGTH) {
        g_lcdUart.buffer[g_lcdUart.length++] = (data >> 24) % 256;
        g_lcdUart.buffer[g_lcdUart.length++] = (data >> 16) % 256;
        g_lcdUart.buffer[g_lcdUart.length++] = (data >> 8) % 256;
        g_lcdUart.buffer[g_lcdUart.length++] = data % 256;
    }
}

void buffer_add_string(char* str)
{
    while (*str) {
        if (g_lcdUart.length >= LCD_BUFFER_LENGTH) { break; }
        g_lcdUart.buffer[g_lcdUart.length++] = *str;
        str++;
    }
}
/*!
*  \brief  自动调节背光亮度
*  \param  enable 使能
*  \param  bl_off_level 待机亮度
*  \param  bl_on_level  激活亮度
*  \param  bl_on_time  未触摸时间 xs
*/
void SetPowerSaving(uint8_t enable, uint8_t bl_off_level, uint8_t bl_on_level, uint8_t  bl_on_time)
{
    buffer_add_start();
    buffer_add_1_byte(0x77);
    buffer_add_1_byte(enable);
    buffer_add_1_byte(bl_off_level);
    buffer_add_1_byte(bl_on_level);
    buffer_add_1_byte(bl_on_time);
    buffer_add_end();
    HAL_UART_Transmit(&huart2, g_lcdUart.buffer, g_lcdUart.length, 1000);
}

/*!
*  \brief      设置当前画面
*  \param  screen_id 画面ID
*/
void SetScreen(uint16_t screen_id)
{
    buffer_add_start();
    buffer_add_2_byte(0xB100);
    buffer_add_2_byte(screen_id);
    buffer_add_end();
    HAL_UART_Transmit(&huart2, g_lcdUart.buffer, g_lcdUart.length, 1000);
}
/*!
*  \brief     设置文本值
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  str 文本值
*/
void SetTextValue(uint16_t screen_id, uint16_t control_id, char *str)
{ 
    buffer_add_start();
    buffer_add_2_byte(0xB110);
    buffer_add_2_byte(screen_id);
    buffer_add_2_byte(control_id);
    buffer_add_string(str);
    buffer_add_end();
    HAL_UART_Transmit(&huart2, g_lcdUart.buffer, g_lcdUart.length, 1000);
}
/*!
*  \brief     设置控件背景色
*  \details  支持控件：进度条、文本
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  color 背景色
*/
void SetControlBackColor(uint16_t screen_id, uint16_t control_id, uint16_t color)
{
    buffer_add_start();
    buffer_add_2_byte(0xB118);
    buffer_add_2_byte(screen_id);
    buffer_add_2_byte(control_id);
    buffer_add_2_byte(color);
    buffer_add_end();
    HAL_UART_Transmit(&huart2, g_lcdUart.buffer, g_lcdUart.length, 1000);
}

#define MAX_FUNCTION_NUMBER         9
void clear_function_select(void)
{
    uint8_t i;
    for (i = 0; i < MAX_FUNCTION_NUMBER; i++) {
        SetControlBackColor(1, i + 1, DEFAULT_FREE_COLOR);
    }
}

void function_one_select(uint16_t control_id)
{
    static uint16_t lastControl = 0;
    if (lastControl != 0) {
        SetControlBackColor(1, lastControl, DEFAULT_FREE_COLOR);
    }
    SetControlBackColor(1, control_id, DEFAULT_SELECT_COLOR);
    lastControl = control_id;

}

void function_one_select1(uint16_t control_id)
{
    static uint16_t lastControl = 0;
    if (lastControl != 0) {
        SetControlBackColor(2, lastControl, DEFAULT_FREE_COLOR);
    }
    SetControlBackColor(2, control_id, DEFAULT_SELECT_COLOR);
    lastControl = control_id;

}

void excel_add_one_line(uint8_t screen_id, uint8_t control_id, ExcelMessage message)
{
    char str[100] = {0};
    snprintf(str, 100, "%d;%.1f;%d;%d;%d;%s;%s;", message.board + 1, message.systemTemp, message.okNumber, message.chipName, message.chipTemp, message.state, message.message);
    buffer_add_start();
    buffer_add_2_byte(0xB152);
    buffer_add_2_byte(screen_id);
    buffer_add_2_byte(control_id);
    buffer_add_string(str);
    buffer_add_end();
    HAL_UART_Transmit(&huart2, g_lcdUart.buffer, g_lcdUart.length, 1000);
}

void excel_clear(uint8_t screen_id, uint8_t control_id)
{
    buffer_add_start();
    buffer_add_2_byte(0xB153);
    buffer_add_2_byte(screen_id);
    buffer_add_2_byte(control_id);
    buffer_add_end();
    HAL_UART_Transmit(&huart2, g_lcdUart.buffer, g_lcdUart.length, 1000);
}

void excel_change_one_line(uint8_t screen_id, uint8_t control_id, ExcelMessage message)
{
    char str[100] = {0};
    snprintf(str, 100, "%d;%.1f;%d;%d;%d;%s;%s;", message.board + 1, message.systemTemp, message.okNumber, message.chipName, message.chipTemp, message.state, message.message);
    buffer_add_start();
    buffer_add_2_byte(0xB157);
    buffer_add_2_byte(screen_id);
    buffer_add_2_byte(control_id);
    buffer_add_2_byte(message.board);
    buffer_add_string(str);
    buffer_add_end();
    HAL_UART_Transmit(&huart2, g_lcdUart.buffer, g_lcdUart.length, 1000);
}

void lcd_init_message_excel(void)
{
    int i;
    ExcelMessage message = {0};

    excel_clear(EXCEL_OF_SCREEN, EXCEL_TEXT_ID);
    for (i = 0; i < EXCEL_V_NUMBER; i++) {
        message.board = i;
        excel_add_one_line(EXCEL_OF_SCREEN, EXCEL_TEXT_ID, message);
    }
}