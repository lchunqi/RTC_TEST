#include "config.h"

/**
-------评估板V7.0-------------
功能：主函数入口--main函数运行
修改：sjm
版本：V7.0
参数：-
返回值：-
时间：2025.06.04
**/
RCC_ClocksTypeDef get_rcc_clock;//获取系统时钟数据
int main(void)
{
	RCC_GetClocksFreq(&get_rcc_clock);//查看时钟频率
	OLED_Init();//oled初始化
	IIC_Init();//iic初始化
	SPI_init();//spi初始化
	sd8902_config();//sd8902引脚初始化
	Usart_Init();//串口初始化
	Timer_Init();//定时器初始化
	LED_GPIO_Config();//led初始化
	Key_Init();//按键初始化
	LED_Control(0);//led灭
	sd25xx_charge_on();//打开标准模块充电功能
	
//	OLED_DrawLine(0, 0, 127, 63);    //测试用
//	OLED_DrawLine(127, 0, 0, 63);
//	OLED_Update();
	OLED_ShowImage(33, 0, 80, 60, wave_logo);//显示logo
	OLED_ShowString(101,55,"V6.5",6);//显示版本号
	OLED_Update();//刷新oled
	Delay_ms(800);
	OLED_Clear();//清屏
			
//	Usart_Printf("sd2405时间显示\r\n");
	while (1)
	{	
/*********************************************************/	
	
		Menu_Run();//菜单运行
		
		COMMAND();//串口指令

		Bus_ReceData();//上位机指令

/*********************************************************/

	
/*********************测试用***************************/
//Usart_Printf("20%d年%d月%d日 %d时%d分%d秒 星期%d\n",sd8902_buff[6],sd8902_buff[5],sd8902_buff[4],sd8902_buff[2],sd8902_buff[1],sd8902_buff[0],sd8902_buff[3]);
//Usart_Printf("1----20%d年%d月%d日 %d时%d分%d秒 星期%d\n",buf[6],buf[5],buf[4],buf[2],buf[1],buf[0],buf[3]);
//Delay_ms(1000);
//	sd2201_time_copy();	
//  Delay_ms(1000);
//  wave(1);
//  sd85xx_time_show();
//  t=RTC_ReadByte(0x00,IIC_TEST);
//  OLED_ShowNum(20,20,t,2,8);
//  Updata_Display();
		
		
/******************************************************/

	}
}
