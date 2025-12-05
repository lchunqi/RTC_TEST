#include "config.h"

RTC_Series_State sdxx_state = RTC_Series_Display; // 初始化状态  
RTC_and_std_State sdxx_std_state = RTC_and_std_Display; // 初始化状态  
RTC_Freq_State sdxx_freq_state = RTC_Freq_Display; // 初始化状态  


/**********************以下都是都是实现具体功能的函数*******************************/
/**
功能：实现具体功能的函数：芯片走时测试，对比走时测试，频率数据
--void SDXXXX_Series(void);//单独测试
--void SDXXXX_and_std(void);//对比测试
--void fXXX_Out(void);//频率输出

修改：sjm
参数：-
返回值：-
**/
void sd2506_messgae(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd2505showtime();
			sd2506_messgae_line();
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }
}

void SD20XXX_and_std(void)
{
	switch (sdxx_std_state) 
	{  
		  case RTC_and_std_Display:  
			// 显示时间  
			sd2xxx_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10);
				if(Key_up == 0) 
				{
					sd2xxx_time_copy();
				}
			}
		  
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }
	
	
}

void SD3XXX_and_std(void)
{
	static uint8_t key_state = 0;
	
	switch (sdxx_std_state) 
	{
		  case RTC_and_std_Display:  
			// 显示时间  
			sd3xxx_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10); 
				if(Key_up == 0)
				{
					sd2xxx_time_copy();
				}
			}
			
			if(Key_down == 0) 
			{
				Delay_ms(10);
				if(Key_down == 0)
				{
					if (key_state == 0) 
					{	
						sd3xxx_charge_on(); // 执行sd3xxx_charge_on()函数
						key_state = 1; // 更新按键状态为已按下
					} else 
					{
						sd3xxx_charge_off(); // 执行sd3xxx_charge_off()函数
						key_state = 0; // 更新按键状态为未按下
					}
				}
			}
		  
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }
	
}


void SD2XXX_Series(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd2xxx_time_show();
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }
	
}





void SD3XXX_Series(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd3xxx_time_show();
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }
}

void SD85XX_Series(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd85xx_time_show();
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }
	
}

void SD85XX_and_std(void)
{
	switch (sdxx_std_state) 
	{
		  case RTC_and_std_Display:  
			// 显示时间  
			sd85xx_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10); 
				if(Key_up == 0)
				{
					sd85xx_time_copy();
				}
			}
			
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }
	
}

void SD8800_Series(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd8800_time_show();
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }	
	
}

void SD8800_and_std(void)
{
  static uint8_t key_state = 0;

	switch (sdxx_std_state) 
	{
		  case RTC_and_std_Display:  
			// 显示时间  
			sd8800_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10); 
				if(Key_up == 0)
				{
					sd8800_time_copy();
				}
			}
			
			if(Key_down == 0) 
			{
				Delay_ms(10);
				if(Key_down == 0)
				{
					if (key_state == 0) 
					{	
						sd8800_charge_on(); // 执行sdxxx_charge_on()函数
						key_state = 1; // 更新按键状态为已按下
					} else 
					{
						sd8800_charge_off(); // 执行sdxxx_charge_off()函数
						key_state = 0; // 更新按键状态为未按下
					}
				}
			}
		  
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }
	
	
	
}

void SD8804_Series(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd8804_time_show();
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }
	
}

void SD8804_and_std(void)
{
	switch (sdxx_std_state) 
	{
		  case RTC_and_std_Display:  
			// 显示时间  
			sd8804_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10); 
				if(Key_up == 0)
				{
					sd8804_time_copy();
				}
			}
		  
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }
	
}

void SD8810_Series(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd8810_time_show();
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }	
}

void SD8810_and_std(void)
{
	switch (sdxx_std_state) 
	{
		  case RTC_and_std_Display:  
			// 显示时间  
			sd8810_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10); 
				if(Key_up == 0)
				{
					sd8810_time_copy();
				}
			}
		  
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }
	
}

void SD8825_Series(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd8825_time_show();
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }
}

void SD8825_and_std(void)
{
	switch (sdxx_std_state) 
	{
		  case RTC_and_std_Display:  
			// 显示时间  
			sd8825_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10); 
				if(Key_up == 0)
				{
					sd8825_time_copy();
				}
			}
		  
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }	
}

void SD8840_Series(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd8840_time_show();
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }

	
}

void SD8840_and_std(void)
{
	switch (sdxx_std_state) 
	{
		  case RTC_and_std_Display:  
			// 显示时间  
			sd8840_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10); 
				if(Key_up == 0)
				{
					sd8840_time_copy();
				}
			}
		  
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }
	
}

void SD8200_Series(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd8200_time_show();
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }

}

void SD8200_and_std(void)
{
  static uint8_t key_state = 0;

	switch (sdxx_std_state) 
	{
		  case RTC_and_std_Display:  
			// 显示时间  
			sd8200_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10); 
				if(Key_up == 0)
				{
					sd8200_time_copy();
				}
			}
			
			if(Key_down == 0) 
			{
				Delay_ms(10);
				if(Key_down == 0)
				{
					if (key_state == 0) 
					{	
						sd8200_charge_on(); // 执行sdxxx_charge_on()函数
						key_state = 1; // 更新按键状态为已按下
					} else 
					{
						sd8200_charge_off(); // 执行sdxxx_charge_off()函数
						key_state = 0; // 更新按键状态为未按下
					}
				}
			}
		  
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }	
}


void SD8931_Series(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd8931_time_show();
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }

	
}

void SD8931_and_std(void)
{
  static uint8_t key_state = 0;

	switch (sdxx_std_state) 
	{
		  case RTC_and_std_Display:  
			// 显示时间  
			sd8931_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10); 
				if(Key_up == 0)
				{
					sd8931_time_copy();
				}
			}
			
			if(Key_down == 0) 
			{
				Delay_ms(10);
				if(Key_down == 0)
				{
					if (key_state == 0) 
					{	
						sd8931_charge_on(); // 执行sdxxx_charge_on()函数
						key_state = 1; // 更新按键状态为已按下
					} else 
					{
						sd8931_charge_off(); // 执行sdxxx_charge_off()函数
						key_state = 0; // 更新按键状态为未按下
					}
				}
			}
		  
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }
	
}

void SD8939_Series(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd8939_time_show();
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }

	
}

void SD8939_and_std(void)
{
	switch (sdxx_std_state) 
	{
		  case RTC_and_std_Display:  
			// 显示时间  
			sd8939_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10); 
				if(Key_up == 0)
				{
					sd8939_time_copy();
				}
			}
		  
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }
	
}


void SD8902_Series(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8902_time_show();
		OLED_Update();
		if(Key_back == 0)
		{
			Delay_ms(10); // 消抖
			back_press_time += 50; // 每次循环递增50ms
			if(back_press_time >= 100)
			{
				// 长按200ms，退出函数
				while(Key_back == 0); // 等待松手
				break;
			}
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void SD8902_and_std(void)
{
	static uint8_t key_state = 0;

	switch (sdxx_std_state) 
	{
		  case RTC_and_std_Display:  
			// 显示时间  
			sd8902_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10); 
				if(Key_up == 0)
				{
					sd8902_time_copy();
				}
			}
			
			if(Key_down == 0) 
			{
				Delay_ms(10);
				if(Key_down == 0)
				{
					if (key_state == 0) 
					{	
						sd8902_charge_on(); // 执行sdxxx_charge_on()函数
						key_state = 1; // 更新按键状态为已按下
					} else 
					{
						sd8902_charge_off(); // 执行sdxxx_charge_off()函数
						key_state = 0; // 更新按键状态为未按下
					}
				}
			}
		  
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }
	
}

void SD2201_Series(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd2201_time_show(IIC_TEST);
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }	
}

void SD2201_and_std(void)
{
	switch (sdxx_std_state) 
	{
		  case RTC_and_std_Display:  
			// 显示时间  
			sd2201_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10); 
				if(Key_up == 0)
				{
					sd2201_time_copy();
				}
			}
		  
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }

}


void null(void)
{


}



void f32768_8563_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8563_f32768(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8563_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1024_8563_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8563_f1024(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8563_f1024_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f32_8563_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8563_f32(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8563_f32_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1_8563_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8563_f1(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8563_f1_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f32768_8939_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8939_f32768(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8939_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f8192_8939_Out(void)
{
	switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8939_f8192(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8939_f8192_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }

	
}

void f4096_8939_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8939_f4096(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8939_f4096_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1_8939_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8939_f1(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8939_f1_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }

}

void f32768_8825_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8825_f32768(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8825_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1024_8825_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8825_f1024(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8825_f1024_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1_8825_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8825_f1(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8825_f1_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f32_8902_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8902_f32768();  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8902_f32768_off();  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f32768_8804_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8804_f32768(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8804_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1024_8804_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8804_f1024(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8804_f1024_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1_8804_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8804_f1(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8804_f1_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f32768_8810_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8810_f32768(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8810_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1024_8810_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8810_f1024(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8810_f1024_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1_8810_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8810_f1(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8810_f1_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f512_8200_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8200_f512(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8200_f512_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }	
}

void f1_8200_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8200_f1(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8200_f1_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f32768_8840_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8840_f32768();  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8840_f32768_off();  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1024_8840_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8840_f1024();  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8840_f1024_off();  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }

}

void f1_8840_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8840_f1();  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8840_f1_off();  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }

}

void f8192_8931_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8931_f8192(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8931_f8192_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f4096_8931_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8931_f4096(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8931_f4096_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1024_8931_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8931_f1024(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8931_f1024_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1_8931_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8931_f1(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8931_f1_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f32768_8931_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8931_f32768(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8931_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f32768_8800_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8800_f32768(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8800_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1024_8800_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8800_f1024(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8800_f1024_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1_8800_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8800_f1(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8800_f1_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f32768_2201_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd2201_f32768(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd2201_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f16_2201_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd2201_f16(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd2201_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f8_2201_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd2201_f8(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd2201_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f4_2201_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd2201_f4(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd2201_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f2_2201_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd2201_f2(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd2201_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }

}

void f1_2201_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd2201_f1(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd2201_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}


void f32768_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            rtc_f32768(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                rtc_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}


void f4096_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f4096(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
				rtc_f4096_off(IIC_TEST);  // 停止频率显示
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}
	
}

void f1024_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f1024(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
				rtc_f1024_off(IIC_TEST);  // 停止频率显示
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}
}

void f64_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f64(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
				rtc_f64_off(IIC_TEST);  // 停止频率显示
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}
	
}

void f32_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f32(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态 
				rtc_f32_off(IIC_TEST);  // 停止频率显示				
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}

}

void f16_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f16(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
				rtc_f16_off(IIC_TEST);  // 停止频率显示
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}

}

void f8_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f8(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
				rtc_f8_off(IIC_TEST);  // 停止频率显示
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}
}

void f4_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f4(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态
				rtc_f4_off(IIC_TEST);  // 停止频率显示
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}

}

void f2_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f2(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态
				rtc_f2_off(IIC_TEST);  // 停止频率显示
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}
	
}

void f1_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f1(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态 
				rtc_f1_off(IIC_TEST);  // 停止频率显示
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}
	
}

void f1_2_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f1_2(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
				rtc_f1_2_off(IIC_TEST);  // 停止频率显示
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}
	
}

void f1_4_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f1_4(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
				rtc_f1_4_off(IIC_TEST);  // 停止频率显示
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}
}

void F1_8_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f1_8(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
				rtc_f1_8_off(IIC_TEST);  // 停止频率显示
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}
}

void F1_16_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f1_16(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
				rtc_f1_16_off(IIC_TEST);  // 停止频率显示
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}

}

void f1s_Out(void)
{
	switch (sdxx_freq_state) 
	{  
		  case RTC_Freq_Display:  
			// 显示频率 
			rtc_f1s(IIC_TEST);
			Updata_Display();
		  
			if (Key_back == 0) 
			{  
				sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态 
				rtc_f1s_off(IIC_TEST);  // 停止频率显示
			  }
		  break;  

		  case RTC_Freq_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_freq_state = RTC_Freq_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	}
}

void SD8573_Series(void)
{
	switch (sdxx_state) 
	{  
        case RTC_Series_Display:  
			// 显示时间  
			sd8573_time_show();
			Updata_Display();
            
            if (Key_back == 0) 
			{  
              sdxx_state = RTC_Series_WaitBack; // 切换状态  
            }
        break;  

        case RTC_Series_WaitBack:  
            if (Key_back != 0) 
			{  
                sdxx_state = RTC_Series_Display; // 重置状态  
                return; // 返回菜单  
            }  
        break;  
    }	
}

void SD8573_and_std(void)
{
	switch (sdxx_std_state) 
	{
		  case RTC_and_std_Display:  
			// 显示时间  
			sd8573_and_std();
			Updata_Display();
		  
			if(Key_up == 0)
			{
				Delay_ms(10); 
				if(Key_up == 0)
				{
					sd8573_time_copy();
				}
			}
		  
			if (Key_back == 0) 
			{  
				sdxx_std_state = RTC_and_std_WaitBack; // 切换状态  
			}
			
		  break;  

		  case RTC_and_std_WaitBack:  
			if (Key_back != 0) 
			{  
				  sdxx_std_state = RTC_and_std_Display; // 重置状态 
				  return; // 返回菜单  
			  }  
		  break;  
	  }

}

void f32768_8573_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8573_f32768(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8573_f32768_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f13684_8573_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8573_f13684(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8573_f13684_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f8192_8573_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8573_f8192(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8573_f8192_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f4096_8573_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8573_f4096(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8573_f4096_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f2048_8573_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8573_f2048(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8573_f2048_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1024_8573_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8573_f1024(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8573_f1024_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

void f1_8573_Out(void)
{
    switch (sdxx_freq_state)   
    {  
        case RTC_Freq_Display:  
            // 显示频率   
            sd8573_f1(IIC_TEST);  
            Updata_Display();  
		
            if (Key_back == 0)   
            {  
                sdxx_freq_state = RTC_Freq_WaitBack; // 切换状态  
                sd8573_f1_off(IIC_TEST);  // 停止频率显示  
            } 
            break;  

        case RTC_Freq_WaitBack:  
            if (Key_back != 0)   
            {  
                sdxx_freq_state = RTC_Freq_Display; // 重置状态   
                return; // 返回菜单  
            }
            break;  
    }
	
}

