#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "oled.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "menu.h"
#include "usart.h"
#include "iic.h"
#include "spi.h"
#include "rtc.h"
#include "cmd.h"
#include "bus.h"


/**
  * 坐标轴定义：
  * 左上角为(0, 0)点
  * 横向向右为X轴，取值范围：0~127
  * 纵向向下为Y轴，取值范围：0~63
  * 
  *       0             X轴           127 
  *      .------------------------------->
  *    0 |
  *      |
  *      |
  *      |
  *  Y轴 |
  *      |
  *      |
  *      |
  *   63 |
  *      v
  * 
  */

RCC_ClocksTypeDef get_rcc_clock;//获取系统时钟数据

int main(void)
{
	RCC_GetClocksFreq(&get_rcc_clock);//查看时钟频率
	OLED_Init();
	IIC_Init();
	SPI_init();
	sd8902_config();
	Usart_Init();
	LED_GPIO_Config();
	Key_Init();

	
	
	
	LED_Control(0);
//	OLED_DrawLine(0, 0, 127, 63);    //测试用
//	OLED_DrawLine(127, 0, 0, 63);
//	OLED_Update();
	OLED_ShowImage(33, 0, 80, 60, wave_logo);
	OLED_ShowString(101,55,"V6.4",6);
	
	OLED_Update();
	Delay_ms(800);	
	OLED_Clear();
	
//	Usart_Printf("sd2405时间显示\r\n");

	while (1)
	{	
/*********************************************************/
		
		Menu_Run();
		
		COMMAND();

		Bus_ReceData();

/*********************************************************/

	
/*********************测试用***************************/
//		Delay_ms(1000);
//		sd8902_time();	//读取日期和时间
//		Usart_Printf("20%d年%d月%d日 %d时%d分%d秒 星期%d/n",sd8902_buff[6],sd8902_buff[5],sd8902_buff[4],sd8902_buff[2],sd8902_buff[1],sd8902_buff[0],sd8902_buff[3]);
//	sd2405showtime();
//	Time_test_show();
//  sd2506_idshow();
//	SD8902_ReadTimeDate();
//	OLED_Update();
//串口走时测试
//	sd3068time();
//		sd8902();

//uint8_t y=(uint8_t)RTC_ReadVbat(IIC_STD);
//uint8_t x= sd8902_ReadTemp();

//Usart_Printf("y=%uV\r\n",y);
//Usart_Printf("x=%x C\r\n",x);
//Usart_Printf("x=%d C\r\n",x);

// sd8840_ReadTemp();
//Delay_ms(1000);




/******************************************************/

	}
}
