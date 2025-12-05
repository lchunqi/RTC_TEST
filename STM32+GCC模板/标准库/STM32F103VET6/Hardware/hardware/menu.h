#ifndef __MENU_H
#define __MENU_H


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f10x.h"
#include "OLED.h"
#include "Key.h"
#include "rtc.h"
#include "spi.h"
#include "sd3068.h"
#include "sd8563.h"
#include "sd8931.h"
#include "sd8200.h"
#include "sd8939.h"
#include "sd8810.h"
#include "sd8804.h"
#include "sd8800.h"
#include "sd8825.h"


typedef struct
{
	uint16_t current;
	uint16_t up;//向上翻索引号
	uint16_t down;//向下翻索引号
	uint16_t enter;//确认索引号
	uint16_t back;//返回索引号
	void (*current_operation)();
} key_table;

extern key_table table[];
extern uint16_t func_index; //初始界面

void Menu_Run(void);


void First_Ui_1(void);
void First_Ui_2(void);

void Second_Ui_1_1(void);//1_1:一级界面下的第一个分选项
void Second_Ui_1_2(void);
void Second_Ui_1_3(void);
void Second_Ui_1_4(void);
void Second_Ui_1_5(void);
void Second_Ui_1_6(void);
void Second_Ui_1_7(void);

void sd2506_messgae(void);

void Third_2Ui_1_1(void);//2Ui_1_1:二级界面下的第一个分选项的分支
void Third_2Ui_1_2(void);
void Third_2Ui_1_3(void);
void Third_2Ui_1_4(void);
void Third_2Ui_1_5(void);
void Third_2Ui_1_6(void);
void Third_2Ui_1_7(void);

void Third_2Ui_2_1(void);
void Third_2Ui_2_2(void);
void Third_2Ui_2_3(void);
void Third_2Ui_2_4(void);
void Third_2Ui_2_5(void);
void Third_2Ui_2_6(void);
void Third_2Ui_2_7(void);

void Third_2Ui_3_1(void);//8563/4/5/8/73/78/89
void Third_2Ui_3_2(void);
void Third_2Ui_3_3(void);
void Third_2Ui_3_4(void);
void Third_2Ui_3_5(void);
void Third_2Ui_3_6(void);
void Third_2Ui_3_7(void);

void Third_2Ui_4_1(void);//8800/04/10/25/30/40
void Third_2Ui_4_2(void);
void Third_2Ui_4_3(void);
void Third_2Ui_4_4(void);
void Third_2Ui_4_5(void);
void Third_2Ui_4_6(void);

void Third_2Ui_5_1(void);//8190/8198/8200/8208/8902/8908/8931/8939/
void Third_2Ui_5_2(void);
void Third_2Ui_5_3(void);
void Third_2Ui_5_4(void);
void Third_2Ui_5_5(void);
void Third_2Ui_5_6(void);
void Third_2Ui_5_7(void);
void Third_2Ui_5_8(void);

void Third_2Ui_6_1(void);
void Third_2Ui_6_2(void);
void Third_2Ui_6_3(void);
void Third_2Ui_6_4(void);
void Third_2Ui_6_5(void);
void Third_2Ui_6_6(void);
void Third_2Ui_6_7(void);

void Third_2Ui_7_1(void);
void Third_2Ui_7_2(void);
void Third_2Ui_7_3(void);
void Third_2Ui_7_4(void);
void Third_2Ui_7_5(void);
void Third_2Ui_7_6(void);
void Third_2Ui_7_7(void);
void Third_2Ui_7_8(void);


void Fourth_3Ui_1_1(void);
void Fourth_3Ui_1_2(void);
void Fourth_3Ui_1_3(void);

void SD2XXX_Series(void);//单独测试
void SD_Series_64addr_time(void);//对比测试

void SD3XXX_Series(void);//单独测试
void SD3XXX_and_std(void);//对比测试

void f32768(void);
void f4096(void);
void f1024(void);
void f64(void);
void f32(void);
void f16(void);
void f8(void);
void f4(void);
void f2(void);
void f1(void);
void f1_2(void);
void f1_4(void);
void F1_8(void);
void F1_16(void);
void f1s(void);

/*
1,8573需要修改时间寄存器：8573秒地址：0x04
2,所有1006型号温度和vbat,id码需要重新计算
3,8200需要查看规格书
*/
void SD85XX_Series(void);//单独测试
void SD85XX_and_std(void);//对比测试

void SD8800_Series(void);//单独测试
void SD8800_and_std(void);//对比测试

void SD8804_Series(void);//单独测试
void SD8804_and_std(void);//对比测试

void SD8810_Series(void);//单独测试
void SD8810_and_std(void);//对比测试

void SD8825_Series(void);//单独测试
void SD8825_and_std(void);//对比测试

void SD8840_Series(void);//单独测试
void SD8840_and_std(void);//对比测试

void SD8200_Series(void);//单独测试
void SD8200_and_std(void);//对比测试

void SD8931_Series(void);//单独测试
void SD8931_and_std(void);//对比测试

void SD8939_Series(void);//单独测试
void SD8939_and_std(void);//对比测试

void SD8902_Series(void);//单独测试
void SD8902_and_std(void);//对比测试

//void Alarm(void);
//void Countdown(void);

void f32768_Out(void);
void f4096_Out(void);
void f1024_Out(void);
void f64_Out(void);
void f32_Out(void);
void f16_Out(void);
void f8_Out(void);
void f4_Out(void);
void f2_Out(void);
void f1_Out(void);
void f1_2_Out(void);
void f1_4_Out(void);
void F1_8_Out(void);
void F1_16_Out(void);
void f1s_Out(void);
//////////////////////////////////
void f32768_8563(void);
void f1024_8563(void);
void f32_8563(void);
void f1_8563(void);

void f32768_8563_Out(void);
void f1024_8563_Out(void);
void f32_8563_Out(void);
void f1_8563_Out(void);
////////////////////////////////////
void f32768_8939(void);
void f8192_8939(void);
void f4096_8939(void);
void f1_8939(void);

void f32768_8939_Out(void);
void f8192_8939_Out(void);
void f4096_8939_Out(void);
void f1_8939_Out(void);
/////////////////////////////////////
void f32768_8825(void);
void f1024_8825(void);
void f1_8825(void);

void f32768_8825_Out(void);
void f1024_8825_Out(void);
void f1_8825_Out(void);
/////////////////////////////////////
void f32_8902(void);

void f32_8902_Out(void);
/////////////////////////////////////
void f32768_8804(void);
void f1024_8804(void);
void f1_8804(void);

void f32768_8804_Out(void);
void f1024_8804_Out(void);
void f1_8804_Out(void);
/////////////////////////////////////
void f32768_8810(void);
void f1024_8810(void);
void f1_8810(void);

void f32768_8810_Out(void);
void f1024_8810_Out(void);
void f1_8810_Out(void);
/////////////////////////////////////
void f512_8200(void);
void f1_8200(void);

void f512_8200_Out(void);
void f1_8200_Out(void);
/////////////////////////////////////
void f32768_8840(void);
void f1024_8840(void);
void f1_8840(void);

void f32768_8840_Out(void);
void f1024_8840_Out(void);
void f1_8840_Out(void);
/////////////////////////////////////
void f32768_8931(void);
void f8192_8931(void);
void f4096_8931(void);
void f1024_8931(void);
void f1_8931(void);

void f32768_8931_Out(void);
void f8192_8931_Out(void);
void f4096_8931_Out(void);
void f1024_8931_Out(void);
void f1_8931_Out(void);
/////////////////////////////////////
void f32768_8800(void);
void f1024_8800(void);
void f1_8800(void);

void f32768_8800_Out(void);
void f1024_8800_Out(void);
void f1_8800_Out(void);

void null(void);



#endif

