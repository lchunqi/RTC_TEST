#include "menu.h"


//void init_timer() {
//    TIM_TimeBaseInitTypeDef TIM_InitStruct;
//    NVIC_InitTypeDef NVIC_InitStruct;

//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

//    TIM_InitStruct.TIM_Period = 500 - 1; // 500ms
//    TIM_InitStruct.TIM_Prescaler = 7200 - 1; // 84MHz / 8400 = 10kHz
//    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
//    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
//    TIM_TimeBaseInit(TIM2, &TIM_InitStruct);

//    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

//    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
//    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStruct);

//    TIM_Cmd(TIM2, ENABLE);
//}

//void TIM2_IRQHandler(void) {
//    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
//        sd2505showtime();
//        sd2506_messgae_line();
//        OLED_Update();

//        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//    }
//}


void sd2506_messgae(void)
{
	
//	while(1)
//	{
//		sd2505showtime();
//		sd2506_messgae_line();
//		OLED_Update();
//		if(KEY_BACK == 0)
//		{
//			Delay_ms(10);
//			return;
//		}
//	}
	
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd2505showtime();
		sd2506_messgae_line();
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

void SD_Series_64addr_time(void)
{
	uint8_t back_press_time = 0;
	uint8_t enter_press_time = 0;
	
	while(1)
	{
		sd2xxx_and_std();
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
		
		if(Key_up == 0)
		{
			Delay_ms(10); // 消抖
			enter_press_time += 50; // 每次循环递增50ms
			if(enter_press_time >= 100)
			{
				// 长按200ms，退出函数
				while(Key_up == 0) // 等待松手
				{
					sd2xxx_time_copy();
					OLED_Update();
				}
			} 
		}
		else
		{
			enter_press_time = 0; // 重置计时器
		}

	}
	
}

void SD3XXX_and_std(void)
{
	uint8_t back_press_time = 0;
	uint8_t enter_press_time = 0;
	uint8_t down_press_time = 0;
	uint8_t key_state = 0;
	
	while(1)
	{
		sd3xxx_and_std();
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
/******************上按键************************/
		if(Key_up == 0)
		{
			Delay_ms(10); // 消抖
			enter_press_time += 50; // 每次循环递增50ms
			if(enter_press_time >= 100)
			{
				sd3xxx_time_copy();
				OLED_Update();
			}
		}
		else
		{
			enter_press_time = 0; // 重置计时器
		}
/******************下按键************************/
    if (Key_down == 0)
	{
        Delay_ms(10); // 消抖
        down_press_time += 50; // 每次循环递增50ms
        if (down_press_time >= 100)
		{
            if (key_state == 0) 
			{
                sd3xxx_charge_on(); // 执行sd3xxx_charge_on()函数
                key_state = 1; // 更新按键状态为已按下
            } else {
                sd3xxx_charge_off(); // 执行sd3xxx_charge_off()函数
                key_state = 0; // 更新按键状态为未按下
            }
            down_press_time = 0; // 重置计时器
        }
    } else {
        down_press_time = 0; // 重置计时器
		
		}
	
	}
	
}


void SD2XXX_Series(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd2xxx_time_show();
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

void SD3XXX_Series(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd3xxx_time_show();
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

void SD85XX_Series(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd85xx_time_show();
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

void SD85XX_and_std(void)
{
	uint8_t back_press_time = 0;
	uint8_t enter_press_time = 0;
	
	while(1)
	{
		sd85xx_and_std();
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
		
		if(Key_up == 0)
		{
			Delay_ms(10); // 消抖
			enter_press_time += 50; // 每次循环递增50ms
			if(enter_press_time >= 100)
			{
				// 长按200ms，退出函数
				while(Key_up == 0) // 等待松手
				{
					sd85xx_time_copy();
					OLED_Update();
				}
			} 
		}
		else
		{
			enter_press_time = 0; // 重置计时器
		}

	}
}

void SD8800_Series(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8800_time_show();
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

void SD8800_and_std(void)
{
	uint8_t back_press_time = 0;
	uint8_t enter_press_time = 0;
	uint8_t down_press_time = 0;
	uint8_t key_state = 0;
	
	while(1)
	{
		sd8800_and_std();
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
		
		if(Key_up == 0)
		{
			Delay_ms(10); // 消抖
			enter_press_time += 50; // 每次循环递增50ms
			if(enter_press_time >= 100)
			{
				// 长按200ms，退出函数
				while(Key_up == 0) // 等待松手
				{
					sd8800_time_copy();
					OLED_Update();
				}
			} 
		}
		else
		{
			enter_press_time = 0; // 重置计时器
		}
/******************下按键************************/
    if (Key_down == 0)
	{
        Delay_ms(10); // 消抖
        down_press_time += 50; // 每次循环递增50ms
        if (down_press_time >= 100)
		{
            if (key_state == 0) 
			{
                sd8200_charge_on(); // 执行sd3xxx_charge_on()函数
                key_state = 1; // 更新按键状态为已按下
            } else {
                sd8200_charge_off(); // 执行sd3xxx_charge_off()函数
                key_state = 0; // 更新按键状态为未按下
            }
            down_press_time = 0; // 重置计时器
        }
    } else {
        down_press_time = 0; // 重置计时器
		
		}
	}
}

void SD8804_Series(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8804_time_show();
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

void SD8804_and_std(void)
{
	uint8_t back_press_time = 0;
	uint8_t enter_press_time = 0;
	
	while(1)
	{
		sd8804_and_std();
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
		
		if(Key_up == 0)
		{
			Delay_ms(10); // 消抖
			enter_press_time += 50; // 每次循环递增50ms
			if(enter_press_time >= 100)
			{
				// 长按200ms，退出函数
				while(Key_up == 0) // 等待松手
				{
					sd8804_time_copy();
					OLED_Update();
				}
			} 
		}
		else
		{
			enter_press_time = 0; // 重置计时器
		}

	}
}

void SD8810_Series(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8810_time_show();
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

void SD8810_and_std(void)
{
	uint8_t back_press_time = 0;
	uint8_t enter_press_time = 0;
	
	while(1)
	{
		sd8810_and_std();
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
		
		if(Key_up == 0)
		{
			Delay_ms(10); // 消抖
			enter_press_time += 50; // 每次循环递增50ms
			if(enter_press_time >= 100)
			{
				// 长按200ms，退出函数
				while(Key_up == 0) // 等待松手
				{
					sd8810_time_copy();
					OLED_Update();
				}
			} 
		}
		else
		{
			enter_press_time = 0; // 重置计时器
		}

	}
}

void SD8825_Series(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8825_time_show();
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

void SD8825_and_std(void)
{
	uint8_t back_press_time = 0;
	uint8_t enter_press_time = 0;
	
	while(1)
	{
		sd8825_and_std();
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
		
		if(Key_up == 0)
		{
			Delay_ms(10); // 消抖
			enter_press_time += 50; // 每次循环递增50ms
			if(enter_press_time >= 100)
			{
				// 长按200ms，退出函数
				while(Key_up == 0) // 等待松手
				{
					sd8825_time_copy();
					OLED_Update();
				}
			} 
		}
		else
		{
			enter_press_time = 0; // 重置计时器
		}

	}
}

void SD8840_Series(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8840_time_show();
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

void SD8840_and_std(void)
{
	uint8_t back_press_time = 0;
	uint8_t enter_press_time = 0;
	
	while(1)
	{
		sd8840_and_std();
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
		
		if(Key_up == 0)
		{
			Delay_ms(10); // 消抖
			enter_press_time += 50; // 每次循环递增50ms
			if(enter_press_time >= 100)
			{
				// 长按200ms，退出函数
				while(Key_up == 0) // 等待松手
				{
					sd8840_time_copy();
					OLED_Update();
				}
			} 
		}
		else
		{
			enter_press_time = 0; // 重置计时器
		}

	}
}

void SD8200_Series(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8200_time_show();
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

void SD8200_and_std(void)
{
	uint8_t back_press_time = 0;
	uint8_t enter_press_time = 0;
	uint8_t down_press_time = 0;
	uint8_t key_state = 0;

	
	while(1)
	{
		sd8200_and_std();
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
		
		if(Key_up == 0)
		{
			Delay_ms(10); // 消抖
			enter_press_time += 50; // 每次循环递增50ms
			if(enter_press_time >= 100)
			{
				// 长按200ms，退出函数
				while(Key_up == 0) // 等待松手
				{
					sd8200_time_copy();
					OLED_Update();
				}
			} 
		}
		else
		{
			enter_press_time = 0; // 重置计时器
		}
/******************下按键************************/
    if (Key_down == 0)
	{
        Delay_ms(10); // 消抖
        down_press_time += 50; // 每次循环递增50ms
        if (down_press_time >= 100)
		{
            if (key_state == 0) 
			{
                sd8200_charge_on(); // 执行sd3xxx_charge_on()函数
                key_state = 1; // 更新按键状态为已按下
            } else {
                sd8200_charge_off(); // 执行sd3xxx_charge_off()函数
                key_state = 0; // 更新按键状态为未按下
            }
            down_press_time = 0; // 重置计时器
        }
    } else {
        down_press_time = 0; // 重置计时器
		
		}
	}
}


void SD8931_Series(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8931_time_show();
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

void SD8931_and_std(void)
{
	uint8_t back_press_time = 0;
	uint8_t enter_press_time = 0;
	uint8_t down_press_time = 0;
	uint8_t key_state = 0;

	while(1)
	{
		sd8931_and_std();
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
		
		if(Key_up == 0)
		{
			Delay_ms(10); // 消抖
			enter_press_time += 50; // 每次循环递增50ms
			if(enter_press_time >= 100)
			{
				// 长按200ms，退出函数
				while(Key_up == 0) // 等待松手
				{
					sd8931_time_copy();
					OLED_Update();
				}
			} 
		}
		else
		{
			enter_press_time = 0; // 重置计时器
		}
/******************下按键************************/
    if (Key_down == 0)
	{
        Delay_ms(10); // 消抖
        down_press_time += 50; // 每次循环递增50ms
        if (down_press_time >= 100)
		{
            if (key_state == 0) 
			{
                sd8931_charge_on(); // 执行sd3xxx_charge_on()函数
                key_state = 1; // 更新按键状态为已按下
            } else {
                sd8931_charge_off(); // 执行sd3xxx_charge_off()函数
                key_state = 0; // 更新按键状态为未按下
            }
            down_press_time = 0; // 重置计时器
        }
    } else {
        down_press_time = 0; // 重置计时器
		
		}	
		
	}
}

void SD8939_Series(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8939_time_show();
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

void SD8939_and_std(void)
{
	uint8_t back_press_time = 0;
	uint8_t enter_press_time = 0;
	
	while(1)
	{
		sd8939_and_std();
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
		
		if(Key_up == 0)
		{
			Delay_ms(10); // 消抖
			enter_press_time += 50; // 每次循环递增50ms
			if(enter_press_time >= 100)
			{
				// 长按200ms，退出函数
				while(Key_up == 0) // 等待松手
				{
					sd8939_time_copy();
					OLED_Update();
				}
			} 
		}
		else
		{
			enter_press_time = 0; // 重置计时器
		}

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
	uint8_t back_press_time = 0;
	uint8_t enter_press_time = 0;
	uint8_t down_press_time = 0;
	uint8_t key_state = 0;
	
	while(1)
	{
		sd8902_and_std();
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
		
		if(Key_up == 0)
		{
			Delay_ms(10); // 消抖
			enter_press_time += 50; // 每次循环递增50ms
			if(enter_press_time >= 100)
			{
				// 长按200ms，退出函数
				while(Key_up == 0) // 等待松手
				{
					sd8902_time_copy();
					OLED_Update();
				}
			} 
		}
		else
		{
			enter_press_time = 0; // 重置计时器
		}
/******************下按键************************/
    if (Key_down == 0)
	{
        Delay_ms(10); // 消抖
        down_press_time += 50; // 每次循环递增50ms
        if (down_press_time >= 100)
		{
            if (key_state == 0) 
			{
                sd8902_charge_on(); // 执行sd3xxx_charge_on()函数
                key_state = 1; // 更新按键状态为已按下
            } else {
                sd8902_charge_off(); // 执行sd3xxx_charge_off()函数
                key_state = 0; // 更新按键状态为未按下
            }
            down_press_time = 0; // 重置计时器
        }
    } else {
        down_press_time = 0; // 重置计时器
		
		}		

	}
}

void null(void)
{


}



void f32768_8563_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8563_f32768(IIC_TEST);
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
			sd8563_f32768_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	
}

void f1024_8563_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8563_f1024(IIC_TEST);
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
			sd8563_f1024_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}

}

void f32_8563_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8563_f32(IIC_TEST);
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
			sd8563_f32_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f1_8563_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8563_f1(IIC_TEST);
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
			sd8563_f1_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f32768_8939_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8939_f32768(IIC_TEST);
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
			sd8939_f32768_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}

}

void f8192_8939_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8939_f8192(IIC_TEST);
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
			sd8939_f8192_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}

}

void f4096_8939_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8939_f4096(IIC_TEST);
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
			sd8939_f4096_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}

}

void f1_8939_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8939_f1(IIC_TEST);
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
			sd8939_f1_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}

}

void f32768_8825_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8825_f32768(IIC_TEST);
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
			sd8825_f32768_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f1024_8825_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8825_f1024(IIC_TEST);
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
			sd8825_f1024_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f1_8825_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8825_f1(IIC_TEST);
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
			sd8825_f1_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f32_8902_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8902_f32768();
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
			sd8902_f32768_off();
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}


}

void f32768_8804_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8804_f32768(IIC_TEST);
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
			sd8804_f32768_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f1024_8804_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8804_f1024(IIC_TEST);
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
			sd8804_f1024_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f1_8804_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8804_f1(IIC_TEST);
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
			sd8804_f1_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f32768_8810_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8810_f32768(IIC_TEST);
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
			sd8810_f32768_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f1024_8810_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8810_f1024(IIC_TEST);
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
			sd8810_f1024_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f1_8810_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8810_f1(IIC_TEST);
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
			sd8810_f1_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f512_8200_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8200_f512(IIC_TEST);
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
			sd8200_f512_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}

}

void f1_8200_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8200_f1(IIC_TEST);
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
			sd8200_f1_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}


}

void f32768_8840_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8840_f32768();
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
			sd8840_f32768_off();
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}

}

void f1024_8840_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8840_f1024();
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
			sd8840_f1024_off();
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}


}

void f1_8840_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8840_f1();
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
			sd8840_f1_off();
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}


}

void f8192_8931_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8931_f8192(IIC_TEST);
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
			sd8931_f8192_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f4096_8931_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8931_f4096(IIC_TEST);
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
			sd8931_f4096_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}

}

void f1024_8931_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8931_f1024(IIC_TEST);
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
			sd8931_f1024_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f1_8931_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8931_f1(IIC_TEST);
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
			sd8931_f1_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f32768_8931_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8931_f32768(IIC_TEST);
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
			sd8931_f32768_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f32768_8800_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8800_f32768(IIC_TEST);
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
			sd8800_f32768_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}

}

void f1024_8800_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8800_f1024(IIC_TEST);
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
			sd8800_f1024_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f1_8800_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		sd8800_f1(IIC_TEST);
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
			sd8800_f1_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
}

void f32768_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f32768(IIC_TEST);
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
			rtc_f32768_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	
}
void f4096_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f4096(IIC_TEST);
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
			rtc_f4096_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	
}

void f1024_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f1024(IIC_TEST);
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
			rtc_f1024_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	
}

void f64_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f64(IIC_TEST);
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
			rtc_f64_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	
}

void f32_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f32(IIC_TEST);
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
			rtc_f32_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	

}

void f16_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f16(IIC_TEST);
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
			rtc_f16_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}

}

void f8_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f8(IIC_TEST);
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
			rtc_f8_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	
}

void f4_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f4(IIC_TEST);
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
			rtc_f4_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	

}

void f2_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f2(IIC_TEST);
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
			rtc_f2_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	
}

void f1_Out(void)
{
		uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f1(IIC_TEST);
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
			rtc_f1_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	
}

void f1_2_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f1_2(IIC_TEST);
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
			rtc_f1_2_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	
}

void f1_4_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f1_4(IIC_TEST);
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
			rtc_f1_4_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	
}

void F1_8_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f1_8(IIC_TEST);
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
			rtc_f1_8_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	

}

void F1_16_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f1_16(IIC_TEST);
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
			rtc_f1_16_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	

}

void f1s_Out(void)
{
	uint8_t back_press_time = 0;
	
	while(1)
	{
		rtc_f1s(IIC_TEST);
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
			rtc_f1s_off(IIC_TEST);
			return;
		}
		else
		{
			back_press_time = 0; // 重置计时器
		}
	}
	

}


