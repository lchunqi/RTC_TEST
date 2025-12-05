#include "spi_soft.h"
#include "usart.h"

typedef struct {
	GPIO_TypeDef* busGPIOB;
    uint16_t pinCS;
    uint16_t pinCLK;
	uint16_t pinDI;
    uint16_t pinDO;
    uint8_t  pinDI_flag;

}SPIGpio;
static SPIGpio  g_spi = {0};

void SPI_GPIO_Init(  uint16_t Row_pin , uint16_t pinCS )
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};

    g_spi.busGPIOB  = SPI_GPIO;
    g_spi.pinCS     = SPI_CS_PIN  << pinCS;
    g_spi.pinCLK    = SPI_CLK_PIN << Row_pin;
    g_spi.pinDO     = SPI_DO_PIN  << Row_pin;
    g_spi.pinDI_flag= 0;

	GPIO_InitStruct.Pin = g_spi.pinCS ;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(g_spi.busGPIOB, &GPIO_InitStruct);
		
	GPIO_InitStruct.Pin = g_spi.pinCLK ;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(g_spi.busGPIOB, &GPIO_InitStruct);


	GPIO_InitStruct.Pin = g_spi.pinDO;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(g_spi.busGPIOB, &GPIO_InitStruct);
		
    if(Row_pin==2)//RTC_DI
    {
        g_spi.pinDI_flag=1;
        GPIO_InitStruct.Pin = GPIO_PIN_14;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    }else{
        g_spi.pinDI_flag = 0;
        g_spi.pinDI = SPI_DI_PIN  << Row_pin;
        GPIO_InitStruct.Pin = g_spi.pinDI ;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(g_spi.busGPIOB, &GPIO_InitStruct);

    }

}

void SPI_CS_HIGH()
{
	HAL_GPIO_WritePin(g_spi.busGPIOB , g_spi.pinCS, GPIO_PIN_SET);	
}

void SPI_CS_LOW()
{
	HAL_GPIO_WritePin(g_spi.busGPIOB , g_spi.pinCS, GPIO_PIN_RESET);
}

void SPI_CLK_HIGH() 
{
	HAL_GPIO_WritePin(g_spi.busGPIOB , g_spi.pinCLK ,GPIO_PIN_SET);
}

void SPI_CLK_LOW()
{
	HAL_GPIO_WritePin(g_spi.busGPIOB , g_spi.pinCLK , GPIO_PIN_RESET);
}

void SPI_DI_HIGH() 
{
    if(g_spi.pinDI_flag==1) 
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
    }
    else{
        HAL_GPIO_WritePin(g_spi.busGPIOB , g_spi.pinDI ,GPIO_PIN_SET);
    }
}

void SPI_DI_LOW()
{
    if(g_spi.pinDI_flag==1) 
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
    }
    else{
        HAL_GPIO_WritePin(g_spi.busGPIOB , g_spi.pinDI , GPIO_PIN_RESET);
    }
}

static void SPI_DO(uint8_t state)
{

    if(state) 
    {
        HAL_GPIO_WritePin(g_spi.busGPIOB, g_spi.pinDO, GPIO_PIN_SET); 
    }else{
        HAL_GPIO_WritePin(g_spi.busGPIOB, g_spi.pinDO, GPIO_PIN_RESET);   
    }

}

uint8_t SPI_Read_DI(void)
{
    if(g_spi.pinDI_flag==1) 
    {
        return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14);
    }else{
        return HAL_GPIO_ReadPin(g_spi.busGPIOB,g_spi.pinDI);
    }
    
}

static inline void SPI_Delay(int delay)
{
    while (delay--);
}

static void SPI_Send_one_byte(uint8_t data)
{
    int i;
    uint8_t dat=0x80;
    for(i=0;i<8;i++)
    {
        SPI_CLK_LOW();
        SPI_Delay(SPI_delay);

        SPI_DO(data & dat);
        data = data << 1;

        SPI_CLK_HIGH();
        SPI_Delay(SPI_delay);
        
    }
}

static void SPI_Receive_one_byte(uint8_t *data)
{
    int i;
    uint8_t dat=0x80;
    *data=0;
    for ( i = 0; i < 8; i++)
    {
        SPI_CLK_LOW();
        SPI_Delay(SPI_delay);

        if(SPI_Read_DI())
        {
            *data |= dat;
        }  
        dat >>= 1 ;

        SPI_CLK_HIGH();
        SPI_Delay(SPI_delay);
    }

}

void SPI_Write_multi_byte(uint8_t reg_add, uint8_t *data,uint8_t size)
{
    uint8_t i;

    SPI_CLK_HIGH();
    SPI_CS_HIGH();

    SPI_Delay(SPI_delay);
    reg_add &= 0x7f;

    SPI_Send_one_byte(reg_add);
    for(i=0;i<size;i++)
    {
        SPI_Send_one_byte(*(data+i));
        //SPI_Delay(SPI_delay); 
    }

    SPI_CLK_HIGH();
    SPI_CS_LOW();

}
uint8_t SPI_Read_multi_byte(uint8_t reg_add, uint8_t *data,uint8_t size)
{
    uint8_t i,dat=0;

    SPI_CLK_HIGH();
    SPI_CS_HIGH();

    SPI_Delay(SPI_delay);
    reg_add |= 0x80;
    SPI_Send_one_byte(reg_add);
    for(i=0;i<size;i++)
    {
        SPI_Receive_one_byte(&dat);
        *(data+i)=dat;

    }
    
    SPI_CLK_HIGH();
    SPI_CS_LOW();

    if(*data==0xFF)
    {
        return 0;
    }else{
        return 1;
    }
    
}

uint8_t SPI_Connect(uint8_t REG)
{
	uint8_t buf[8]={0},Count_OK=0,count=0;
	uint8_t CS_i=0,j=0;
	
	for( j = 0; j < 3; j++)  
	{
		for ( CS_i = 0; CS_i < 8; CS_i++) 
		{
            SPI_GPIO_Init(j,CS_i);
			HAL_Delay(10);	
			if (SPI_Read_multi_byte( REG, buf, 7)== 1) 
			{
				UsbPrintf("1\t");
				count++;
				Count_OK++;
				if (count % 8 == 0) 
				{ 
					UsbPrintf("\n");
				}
			} else {
				UsbPrintf("0\t");
				count++;
				if (count % 8 == 0) 
				{ 
					UsbPrintf("\n");
				}
			}
		}
		CS_i=0;
	}	
	return Count_OK;
}


uint8_t SPI_ReadTime(uint8_t i,uint8_t j)
{
	uint8_t buf[8]={0};
    if( i<=0 || i>3 || j<=0 || j>8)
    {
        UsbPrintf("--input error !\r\n");
        return 0;
    }else{
        SPI_GPIO_Init( i-1, j-1);
        if (SPI_Read_multi_byte(0x00, buf, 7)== 1) 
        {
            UsbPrintf("20%02X-%02X-%02X W:%02X %02X:%02X:%02X \r\n",buf[6],buf[5],buf[4],buf[3],buf[2],buf[1],buf[0] );
        }else{
            UsbPrintf("--No Connect !\r\n");
            return 0;
        }
    }
	return 1;
}

void Close8840_FREQ(void)
{
	uint8_t i=0,j=0;
    uint8_t SD8840_Close_Freq=0X00;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_SET);
    for( j = 0; j < 3; j++ )  
	{
		for ( i = 0; i < 8; i++ ) 
		{
            SPI_GPIO_Init( j,i );
            SPI_Write_multi_byte(0x0D,&SD8840_Close_Freq,1);
            SPI_Write_multi_byte(0x0F,&SD8840_Close_Freq,1);
        }
    }
}


void RTC_8840_TEMP(float* sysTemp,uint8_t* rtcTemp)
{
	uint8_t CONT=0x10,T=0;
	SPI_Write_multi_byte(0x57, &CONT, 1);
    HAL_Delay(3);
    if(SPI_Read_multi_byte(0x56, &T, 1)== 1) 
    {
        *rtcTemp=T;
        *sysTemp = temp_read_value(1);
        UsbPrintf("-- sysTemp = %.1f C , rtcTemp = %d C \n", *sysTemp , *rtcTemp );
    }
}








