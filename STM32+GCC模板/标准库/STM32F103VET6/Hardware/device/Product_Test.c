#include "Product_Test.h"



void SD8840_SecCountDown(void)
{
	uint8_t buf,buf1;
	//选择1Hz INT中断输出
	buf=0x02;
	spi_write_multi_bytes(0x0D,&buf,1);
	//清除倒计时标志位
	buf=0x00;
	spi_write_multi_bytes(0x0E,&buf,1);
	//设置倒计时计数值:1s
	buf=0x01;
	spi_write_multi_bytes(0x0B,&buf,1);
	//允许INT输出
	buf=0x10;
	spi_write_multi_bytes(0x0F,&buf,1);
	//回读CTR1位并置DE位为1
	spi_read_multi_bytes(0x0D,&buf1,1);
	buf = buf1 | 0x10;
	//启动倒计时
	spi_write_multi_bytes(0x0D,&buf,1);
	
}


spi_exit_code SD8840_PrintfTime(void)
{
	uint8_t i;
	uint8_t spi_time_buf[7];
	uint8_t spi_show_time[7];
	spi_read_multi_bytes(0x00,spi_time_buf,7);
	for(i=0;i<7;i++)
	{
		spi_show_time[i]=bcd_to_dec(spi_time_buf[i]);
	}
	Usart_Printf("%d年%d月%d日 星期%d %d时%d分%d秒\n",spi_show_time[6],spi_show_time[5],spi_show_time[4],spi_show_time[3],spi_show_time[2],spi_show_time[1],spi_show_time[0]);
	return SPI_OK;
}





