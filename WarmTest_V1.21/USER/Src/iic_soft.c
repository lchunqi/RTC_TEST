/* 
*2024.11.16更新 SD3178 SD3077 温度强制转换 RTC_3178_TEMP/3078
*
*/ 
#include "main.h"
#include "usart.h"
#include "iic_soft.h"
typedef struct {
	GPIO_TypeDef* busSda;
	uint16_t pinSda;
	GPIO_TypeDef* busScl;
	uint16_t pinScl;
}I2cGpio;
static I2cGpio  g_i2c = {0};
void IIC_GPIO_Init(uint16_t pinScl, uint16_t pinSda)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	g_i2c.busScl = I2C_GPIO;
	g_i2c.busSda = I2C_GPIO;

	g_i2c.pinSda = GPIO_PIN_0 << pinSda;
	g_i2c.pinScl = GPIO_PIN_8 << pinScl;

	GPIO_InitStruct.Pin = g_i2c.pinSda;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(g_i2c.busSda, &GPIO_InitStruct);
		
	GPIO_InitStruct.Pin = g_i2c.pinScl;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(g_i2c.busScl, &GPIO_InitStruct);

}
void IIC_SDA_H()
{
	HAL_GPIO_WritePin(g_i2c.busSda, g_i2c.pinSda, GPIO_PIN_SET);
	
}

void IIC_SDA_L()
{
	HAL_GPIO_WritePin(g_i2c.busSda, g_i2c.pinSda, GPIO_PIN_RESET);
}

void IIC_SCL_H() 
{
	HAL_GPIO_WritePin(g_i2c.busScl, g_i2c.pinScl ,GPIO_PIN_SET);
}

void IIC_SCL_L()
{
	HAL_GPIO_WritePin(g_i2c.busScl, g_i2c.pinScl , GPIO_PIN_RESET);
}

void IIC_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_Init={0};
	GPIO_Init.Pin=g_i2c.pinSda;
	GPIO_Init.Pull=GPIO_NOPULL;
	GPIO_Init.Mode=GPIO_MODE_INPUT;
	GPIO_Init.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(g_i2c.busSda,&GPIO_Init);
	
}

void IIC_SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_Init={0};
	GPIO_Init.Pin=g_i2c.pinSda;
	GPIO_Init.Pull=GPIO_PULLUP;
	GPIO_Init.Mode=GPIO_MODE_OUTPUT_OD;
	GPIO_Init.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(g_i2c.busSda,&GPIO_Init);
	
}

uint8_t IIC_Read_SDA(void)
{
	return  HAL_GPIO_ReadPin(g_i2c.busSda,g_i2c.pinSda);
}

static inline  void IIC_delay(int i)
{
	while(i--);
}

//开始信号
IIC_soft IIC_Start(void)
{
	IIC_SDA_OUT();
	IIC_SCL_H();
	IIC_SDA_H();
	
	IIC_delay(I2C_delay);
	if(!IIC_Read_SDA())
	{
		return I2C_Start_FAIL;
	}

	IIC_SDA_L();
	IIC_delay(I2C_delay);
	
	IIC_SCL_L();
	IIC_delay(I2C_delay);
	
	return I2C_OK;
}

//停止信号
void IIC_Stop(void)
{
	IIC_SDA_OUT();
	IIC_SCL_L();
	IIC_SDA_L();
	IIC_delay(I2C_delay);
	IIC_SCL_H();
	
	IIC_delay(I2C_delay);
	IIC_SDA_H();
}

//ACK
void IIC_ACK(void)
{
	IIC_SCL_L();
	IIC_SDA_L();
	
	IIC_delay(I2C_delay);
	IIC_SCL_H();
	
	IIC_delay(I2C_delay);
	IIC_SCL_L();
}

void IIC_NACK(void)
{
	
	IIC_SCL_L();
	IIC_SDA_H();
	
	IIC_delay(I2C_delay);
	IIC_SCL_H();
	
	IIC_delay(I2C_delay);
	IIC_SCL_L();

}
	
IIC_soft IIC_Wait_ACK(void)
{
	IIC_SDA_IN();
	IIC_SCL_H();
	IIC_delay(I2C_delay);
	
	if(IIC_Read_SDA())
	{
		return I2C_NO_ACK;
	}
	
	IIC_SCL_L();
	IIC_delay(I2C_delay);
	
	IIC_SDA_H();
	IIC_SDA_OUT();
	
	return I2C_ACK;
}

//发 1 byte
void IIC_Send_Data(uint8_t data)
{
	uint8_t i=0;
	IIC_SCL_L();
	for(i=0;i<8;i++)
	{
		if(data&0X80)
		{
			IIC_SDA_H();
		}else{
			
			IIC_SDA_L();
		}
		IIC_delay(I2C_delay);
		data <<= 1;
		IIC_SCL_H();
		IIC_delay(I2C_delay);
		IIC_SCL_L();
	}
}

//收 1 byte
uint8_t IIC_Receive_Data(void)
{	
	uint8_t i=0,data=0;
	IIC_SDA_IN();
	IIC_SDA_H();
	for(i=0;i<8;i++)
	{
		IIC_SCL_L();
		data <<= 1;
		IIC_delay(I2C_delay);
		IIC_SCL_H();
		IIC_delay(I2C_delay);
		if(IIC_Read_SDA())
		{
			data |= 0x01;		
		}
	}
	IIC_SCL_L();
	IIC_delay(I2C_delay);
	IIC_SDA_OUT();
	
	return data;
}

IIC_soft IIC_Write_multi_bytes(uint8_t dev_addr, uint8_t reg_addr, const uint8_t *buf, uint8_t size)
{
	
	uint8_t i = 0;
	
	if(IIC_Start() != I2C_OK )//开始信号
	{
		return I2C_Start_FAIL;
	}
	
	IIC_Send_Data(dev_addr);//等待从机应答
	if(IIC_Wait_ACK() != I2C_ACK)
	{
		IIC_Stop();
		return I2C_Start_FAIL;
	}
	
	IIC_Send_Data(reg_addr);//器件地址
	if(IIC_Wait_ACK() != I2C_ACK)
	{
		IIC_Stop();
		return I2C_NO_ACK;
	}
	
	for ( i = 0; i < size; i++)//发数据
  {
		IIC_Send_Data(*(buf+i));
		IIC_Wait_ACK();
	}
	
	IIC_Stop();
	return I2C_OK;
}

IIC_soft IIC_Read_multi_bytes(uint8_t dev_addr,uint8_t reg_addr, uint8_t *buf, uint8_t size)
{
	uint8_t i = 0;
	
	if(IIC_Start() != I2C_OK )//开始信号
	{
		return I2C_Start_FAIL;
	}
	
	IIC_Send_Data(dev_addr );//等待从机应答
	if(IIC_Wait_ACK() != I2C_ACK)
	{
		IIC_Stop();
		return I2C_slave_FAIL;
	}
	
	IIC_Send_Data(reg_addr);//器件地址
	if(IIC_Wait_ACK() != I2C_ACK)
	{
		IIC_Stop();
		return I2C_NO_ACK;
	}
	
	//*重新********//
	IIC_Start();
	IIC_Send_Data(dev_addr | 0X01 );
	IIC_Wait_ACK();
	
	for ( i = 0; i < size; i++)
	{
		*(buf+i) = IIC_Receive_Data();

		if (i != size-1) { IIC_ACK(); }			
	}
	
	IIC_NACK();
	IIC_Stop();

	return I2C_OK;
}

uint8_t I2C_Connect(void)//测试iic的连接
{
	uint8_t buf[8]={0},Count_OK=0,count=0;
	uint8_t SDA_i=0,SCL_i=0;
	for( SDA_i = 0; SDA_i < 3; SDA_i++) 
	{
		for ( SCL_i = 0; SCL_i < 8; SCL_i++) 
		{
			IIC_GPIO_Init(SDA_i,SCL_i);
			HAL_Delay(10);	
			if (IIC_Read_multi_bytes(RTC_8931_ADDRESS, RTC_8931_REG, buf, 7)== I2C_OK || 
			IIC_Read_multi_bytes(RTC_8825_ADDRESS, RTC_8825_REG, buf, 7)== I2C_OK) 
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
		SCL_i=0;
	}	
	
	return Count_OK;
}
uint8_t I2C_ReadTime(uint8_t i,uint8_t j)
{
	uint8_t buf[8]={0};
	if(i < 0 || i > 3 || j < 0 || j > 8)
	{
		UsbPrintf("--Input error !\r\n");
		return 0;
	}else{
		IIC_GPIO_Init( i-1,j-1);
		if(IIC_Read_multi_bytes(RTC_8931_ADDRESS, RTC_8931_REG, buf, 7)== I2C_OK)
		{
			//UsbPrintf("D0\n");
			UsbPrintf("20%02X-%02X-%02X 星期%X %X:%X:%X \r\n",buf[6],buf[5],buf[4],buf[3],buf[2],buf[1],buf[0] );
		}
		else if(IIC_Read_multi_bytes(RTC_8825_ADDRESS, RTC_8825_REG, buf, 7)== I2C_OK)
		{
			//UsbPrintf("64\n");
			UsbPrintf("20%02X-%02X-%02X 星期%X %X:%X:%X \r\n",buf[6],buf[5],buf[4],buf[3],buf[2],buf[1],buf[0] );
		}
		else{
			UsbPrintf("--No Connect !\r\n");
			return 0;
		}
	}
	return I2C_OK;
}
void Close3077_FREQ(void)
{
	uint8_t i=0,j=0;
	uint8_t INTFE=0x80;		
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_SET);
    for( i = 1; i < 4; i++ )  
	{
		for ( j = 1; j < 9; j++ ) 
		{
			IIC_GPIO_Init(i-1,j-1);  
			IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x10, &INTFE, 1);
        }
    }
}
void Close3178_FREQ(void)
{
	uint8_t i=0,j=0;
	uint8_t INTFE=0x80;	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_SET);
    for( i = 1; i < 4; i++ )  
	{
		for ( j = 1; j < 9; j++ ) 
		{
			IIC_GPIO_Init(i-1,j-1);  
			IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x10, &INTFE, 1);
        }
    }
}
void Close8800_FREQ(void)
{
	Control_level(0,0);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_SET);
}
void Close8801_FREQ(void)
{

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_SET);
    
}
void Close8810_FREQ(void)
{
	uint8_t i=0,j=0;
	uint8_t SD8810_CLOSE=0X00;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_SET);
    for( i = 1; i < 4; i++ )  
	{
		for ( j = 1; j < 9; j++ ) 
		{
			IIC_GPIO_Init(i-1,j-1);    
			IIC_Write_multi_bytes(RTC_8810_ADDRESS, 0X1D,&SD8810_CLOSE, 1);
        }
    }
}
void Close8825_FREQ(void)//
{
	Control_level(0,0);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_SET);
}
void Close8931_FREQ(void)//
{
	uint8_t i=0,j=0;
	uint8_t SD8931_CLOSE[2]={0X04,0X00};
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_SET);
    for( i = 1; i < 4; i++ )  
	{
		for ( j = 1; j < 9; j++ ) 
		{
			IIC_GPIO_Init(i-1,j-1);    
			IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0X0E,SD8931_CLOSE, 2);
        }
    }
}
void SD3077_Write_Enable(void)
{
	uint8_t data3=0x80,data4=0x84;
	if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x10, &data3, 1)== I2C_OK)
	{
		if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x0F, &data4, 1)== I2C_OK)
		{
		}else
		{
			UsbPrintf("--Enable ERROR ! \r\n");
		}
	}
}
void SD3077_Write_Disable(void)
{
	uint8_t data1=0x00;
	if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x0F, &data1, 1)== I2C_OK)
	{
		if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x10, &data1, 1)== I2C_OK)
		{
			//UsbPrintf("---Write Disable !\r\n");
		}
		else
		{
			UsbPrintf("--Disable ERROR ! \r\n");
		}
	}
}
void SD3178_Write_Enable(void)
{
	uint8_t data3=0x80,data4=0xFF;
	if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x10, &data3, 1)== I2C_OK)
		{
			if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x0F, &data4, 1)== I2C_OK)
			{
			}
			else
			{
				UsbPrintf("--Enable ERROR ! \r\n");
			}
		}
}
void SD3178_Write_Disable(void)
{
	uint8_t data1=0x7B,data2=0x00;
	if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x0F, &data1, 1)== I2C_OK)
		{
			if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x10, &data2, 1)== I2C_OK)
			{
				//UsbPrintf("---Write Disable !\r\n");
			}
			else
			{
				UsbPrintf("--Disable ERROR ! \r\n");
			}
		}
}
void RTC_3077_TEMP( float* sysTemp ,  uint8_t* rtcTemp )
{	
	uint8_t temp_buf=0,CONT=0x10;
	if(IIC_Write_multi_bytes(RTC_3077_ADDRESS, 0x17, &CONT, 1)== I2C_OK )
	{
		HAL_Delay(3);
		if(IIC_Read_multi_bytes(RTC_3077_ADDRESS, 0x16, &temp_buf, 1)== I2C_OK )
		{
			*rtcTemp = temp_buf;
			*sysTemp = temp_read_value(1);
			UsbPrintf("--sysTemp = %.1f C , rtcTemp = %d C\n",*sysTemp , *rtcTemp );
		}
	}
}
void RTC_3178_TEMP( float* sysTemp ,  uint8_t* rtcTemp )
{	
	uint8_t temp_buf=0,CONT=0x10;
	if(IIC_Write_multi_bytes(RTC_3178_ADDRESS, 0x17, &CONT, 1)== I2C_OK )
	{
		HAL_Delay(3);
		if(IIC_Read_multi_bytes(RTC_3178_ADDRESS, 0x16, &temp_buf, 1)== I2C_OK )
		{
			*rtcTemp = temp_buf;
			*sysTemp = temp_read_value(1);
			UsbPrintf("--sysTemp = %.1f C , rtcTemp = %d C\n",*sysTemp , *rtcTemp );
		}
	}
}
void RTC_8825_TEMP( float* sysTemp ,  uint8_t* rtcTemp )
{
	uint8_t CONT=0x10,T=0;
	if(IIC_Write_multi_bytes(RTC_8825_ADDRESS, 0x57, &CONT, 1)== I2C_OK) 
	{
		HAL_Delay(3);
		if(IIC_Read_multi_bytes(RTC_8825_ADDRESS, 0x56, &T, 1)== I2C_OK) 
		{
			*rtcTemp = T;
			*sysTemp = temp_read_value(1);
			UsbPrintf("--sysTemp = %.1f C , rtcTemp = %d C\n",*sysTemp , *rtcTemp );
		}
	}
}
void RTC_8800_TEMP( float* sysTemp ,  uint8_t* rtcTemp )
{
	uint8_t CONT=0x10,buf=0;
	float 	temp = 0; 
	if(IIC_Write_multi_bytes(RTC_8800_ADDRESS, 0x57, &CONT, 1)== I2C_OK) 
	{
		HAL_Delay(3);
		if(IIC_Read_multi_bytes(RTC_8800_ADDRESS, 0x17, &buf, 1)== I2C_OK) 
		{
			temp = ((uint32_t)buf * 2 - 187.19 ) /3.218;
			if (temp  < 0) {
				*rtcTemp = (uint8_t)(temp + 256);
			} else {
				*rtcTemp = (uint8_t)(temp);
			}
			*sysTemp = temp_read_value(1);
			UsbPrintf("--sysTemp = %.1f C , rtcTemp = %d C \n", *sysTemp , *rtcTemp);
		}
	}
}
void RTC_8804_TEMP( float* sysTemp ,  uint8_t* rtcTemp )
{
	uint8_t CONT=0x10,T=0;
	if(IIC_Write_multi_bytes(RTC_8804_ADDRESS, 0x57, &CONT, 1)== I2C_OK) 
	{
		HAL_Delay(3);
		if(IIC_Read_multi_bytes(RTC_8804_ADDRESS, 0x56, &T, 1)== I2C_OK) 
		{
			*rtcTemp = T;
			*sysTemp = temp_read_value(1);
			UsbPrintf("--sysTemp = %.1f C , rtcTemp = %d C\n",*sysTemp , *rtcTemp );
		}
	}
}
void RTC_8810_TEMP( float* sysTemp ,  uint8_t* rtcTemp )
{
	uint8_t CONT=0x10,T=0;
	if(IIC_Write_multi_bytes(RTC_8825_ADDRESS, 0x57, &CONT, 1)== I2C_OK) 
	{
		HAL_Delay(3);
		if(IIC_Read_multi_bytes(RTC_8825_ADDRESS, 0x56, &T, 1)== I2C_OK) 
		{
			*rtcTemp = T;
			*sysTemp = temp_read_value(1);
			UsbPrintf("--sysTemp = %.1f C , rtcTemp = %d C\n",*sysTemp , *rtcTemp );
		}
	}
}
void RTC_8931_TEMP(float* sysTemp ,  uint8_t* rtcTemp )
{
	uint8_t CONT[2]={0x20,0X04},temp=0;
	if(IIC_Write_multi_bytes(RTC_8931_ADDRESS, 0x0E, CONT, 2)== I2C_OK) 
	{
		HAL_Delay(3);
		if(IIC_Read_multi_bytes(RTC_8931_ADDRESS, 0x11, &temp, 1)== I2C_OK) 
		{
			*rtcTemp = temp;
			*sysTemp = temp_read_value(1);
			UsbPrintf("--sysTemp = %.1f C , rtcTemp = %d C\n",*sysTemp , *rtcTemp );
		}
	}
}






