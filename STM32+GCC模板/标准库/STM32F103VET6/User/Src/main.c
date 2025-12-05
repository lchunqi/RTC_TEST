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
#include "sd2201.h"
#include "Product_Test.h"


/**
  * �����ᶨ�壺
  * ���Ͻ�Ϊ(0, 0)��
  * ��������ΪX�ᣬȡֵ��Χ��0~127
  * ��������ΪY�ᣬȡֵ��Χ��0~63
  * 
  *       0             X��           127 
  *      .------------------------------->
  *    0 |
  *      |
  *      |
  *      |
  *  Y�� |
  *      |
  *      |
  *      |
  *   63 |
  *      v
  * 
  */

RCC_ClocksTypeDef get_rcc_clock;//��ȡϵͳʱ������

int main(void)
{

	RCC_GetClocksFreq(&get_rcc_clock);//�鿴ʱ��Ƶ��
	OLED_Init();
	IIC_Init();
	SPI_init();
	sd8902_config();
	Usart_Init();
	LED_GPIO_Config();
	Key_Init();
	LED_Control(0);
	
//	OLED_DrawLine(0, 0, 127, 63);    //������
//	OLED_DrawLine(127, 0, 0, 63);
//	OLED_Update();
	
	OLED_ShowImage(33, 0, 80, 60, wave_logo);
	OLED_ShowString(101,55,"V6.4",6);
	
	OLED_Update();
	Delay_ms(800);	
	OLED_Clear();
	// Usart_Printf("sd2405ʱ����ʾ\r\n");
	while (1)
	{	
/*********************************************************/
		Menu_Run();
		
		COMMAND();

		Bus_ReceData();

/*********************************************************/

	
/*********************������***************************/
//Usart_Printf("20%d��%d��%d�� %dʱ%d��%d�� ����%d\n",sd8902_buff[6],sd8902_buff[5],sd8902_buff[4],sd8902_buff[2],sd8902_buff[1],sd8902_buff[0],sd8902_buff[3]);
//Usart_Printf("1----20%d��%d��%d�� %dʱ%d��%d�� ����%d\n",buf[6],buf[5],buf[4],buf[2],buf[1],buf[0],buf[3]);


// SD2201_ReadTimeDate(IIC_TEST);
//Delay_ms(1000);

/******************************************************/

	}
}
