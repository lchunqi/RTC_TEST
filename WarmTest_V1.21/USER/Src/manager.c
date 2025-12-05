#include "manager.h"
#include "temp.h"
#include "iic_soft.h"
#include "usart.h"

void Manager_Init(void)//模拟开关
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_SET);
  
}

void Set_Port_Number(uint8_t port)//选择对应的端口号
{
    switch(port)
    {
        case 0:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); 
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);            
            break;
        case 1:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);  
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); 
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);  
            break;
        case 2:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); 
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
            break;
        case 3:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); 
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
            break;
        case 4:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); 
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
            break;
        case 5:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); 
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
            break;
        case 6:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); 
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
            break;
        case 7:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); 
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
            break;
    default:
            break;
    }
}

void Enable_IIC4051(uint8_t i ,uint8_t j)
{
    switch (j)//使能模拟开关
		{
			case 1:
			case 4:
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); 
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_SET); 
                if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)==GPIO_PIN_RESET)
                {
                    Set_Port_Number(i-1);
                    UsbPrintf("--INT 1 OUT !\r\n");  
                }else{
                    UsbPrintf("--Enable 1 or 4 Error !\r\n");
                }
				break;
			case 2:
			case 5:
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); 
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_4, GPIO_PIN_SET); 
                if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)==GPIO_PIN_RESET)
                {
                    Set_Port_Number(i-1);
                    UsbPrintf("--INT 2 OUT !\r\n");        
                }else{
                   UsbPrintf("--Enable 2 or 5 Error !\r\n");
                }
				break;
			case 3:
			case 6:
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_0, GPIO_PIN_SET); 
                if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)==GPIO_PIN_RESET)
                {
                    Set_Port_Number(i-1);
                    UsbPrintf("--INT 3 OUT !\r\n");        
                }else{
                   UsbPrintf("--Enable 3 or 6 Error !\r\n");
                }
			break;
			default:
                UsbPrintf("--Enable Error !\r\n"); 
				break;
		}
}

void Enable_SPI4051(uint8_t i,uint8_t j)
{
    switch (j)
		{
			case 1:
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); 
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_SET); 
                if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)==GPIO_PIN_RESET)
                {
                    Set_Port_Number(i-1); 
                    UsbPrintf("--INT OUT 1!\r\n");          
                }else{
                    UsbPrintf("--Enable 1 Error !\r\n");
                }
				break;
			case 2:
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); 
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_4, GPIO_PIN_SET); 
                if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)==GPIO_PIN_RESET)
                {
                    Set_Port_Number(i-1); 
                    UsbPrintf("--INT OUT 2!\r\n");       
                }else{
                    UsbPrintf("--Enable 2 Error !\r\n");
                }
				break;
			case 3:
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_0, GPIO_PIN_SET); 
                if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)==GPIO_PIN_RESET)
                {
                    Set_Port_Number(i-1); 
                    UsbPrintf("--INT OUT 3!\r\n");       
                }else{
                    UsbPrintf("--Enable 3 Error !\r\n");
                }
			break;
			default:
                UsbPrintf("--Enable Error !\r\n");
				break;
		}
}

void Control_level(uint8_t i ,uint8_t level)
{
    switch(i)
    {
        case 1:
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, level);
            break;
        case 2:
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, level);
            break;
        case 3:
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, level);
            break;   
        default:
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_0, GPIO_PIN_RESET);
            break;
    }
}


uint8_t INT_Choose(char* command)
{
    if(strstr(command,"32K"))
	{
		return 32;
	}
	else if(strstr(command,"64HZ"))
	{
		return 64;
	}
    else if(strstr(command,"1024HZ"))
	{
		return 24;
	}
    else if(strstr(command,"4096HZ"))
	{
		return 96;
	}
    else if(strstr(command,"1HZ"))
    {
        return 1;
    }
    else if(strstr(command,"8192HZ"))
    {
        return 92;
    }
    else if(strstr(command,"1min"))
    {
        return 60;
    }
     else if(strstr(command,"1S"))
    {
        return 61;
    }

    return 0;
}












