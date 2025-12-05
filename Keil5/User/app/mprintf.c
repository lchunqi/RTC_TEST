#include "config.h"

uint8_t LongA[256];
uint8_t LongB[256];

/* Function	- MPrint_Help
 * Brief		- None
 * Parameter- None
 * Return		- None
 * Editor		- 						
 * Date			- 
 */
void MPrint_Help(void)
{
	Usart_Printf("使用方法:@+需要执行的指令\n");
	Usart_Printf("help:....................Print command.\n");
	Usart_Printf("version:.................Print current version.\n");
	Usart_Printf("time:....................Print time and data.\n");
	Usart_Printf("ctr:.....................Print control register.\n");
	Usart_Printf("ram:.....................Print RAM.\n");
	Usart_Printf("testmodel:...............Print tes model data.\n");
	Usart_Printf("id:......................Print id.\n");
	Usart_Printf("retain:..................Print data reserve.\n");
	Usart_Printf("config:..................Print config register.\n");
	Usart_Printf("wenbu:...................Print digital compensation.\n");
	Usart_Printf("capin:...................Print input capacitance.\n");
	Usart_Printf("capout:..................Print output capacitance.\n");
	Usart_Printf("configX:.................Print config register number.\n");
	Usart_Printf("wenbuOTP:................Print digital compensation and OTP addr.\n");
	Usart_Printf("capinOTP:................Print input capacitance and OTP addr.\n");
	Usart_Printf("capoutOTP:...............Print output capacitance and OTP addr.\n");
}

/* Function	- MPrint_Version
 * Brief		- None
 * Parameter- None
 * Return		- None
 * Editor		- 						
 * Date			- 
 */
void MPrint_Version(void)
{
	Usart_Printf("Current version:V6.4\n");
	Usart_Printf("Last edit time: 2024.04.08 19:00\n");
	Usart_Printf("Last compile time: %s %s\n\n",__DATE__,__TIME__);
}

/* Function	- MPrint_TimeDate
 * Brief		- None
 * Parameter- None
 * Return		- None
 * Editor		- 						
 * Date			- 
 */
void MPrint_TimeDate(uint8_t *p_dat)
{
	Usart_Printf("当前时间: 20%02u/%02u/%02u %u ",p_dat[6]%100,p_dat[5],p_dat[4],p_dat[3]%7);
	
	Usart_Printf("%02u:%02u:%02u\n\n",p_dat[2],p_dat[1],p_dat[0]);
}

/* Function	- MPrint_CmdFail
 * Brief		- None
 * Parameter- None
 * Return		- None
 * Editor		- 						
 * Date			- 
 */
void MPrint_CmdFail(void)
{
	Usart_Printf("Error:Search command fail!\n\n");
}

/* Function	- MPrint_AckFail
 * Brief		- None
 * Parameter- None
 * Return		- None
 * Editor		- 						
 * Date			- 
 */
void MPrint_AckFail(void)
{
	Usart_Printf("Error:Wait ack fail!\n\n");
}

/**
功能：打印rtc时间
修改：sjm
参数：IIC_x IIC总线
返回值：返回RTC_OK
时间：2025.06.04
**/
rtc_exit_code MPrint_time(i2c_device_bus IIC_x)
{
	uint16_t buf[7];

	if(RTC_read_addr(RTC_TIME,buf,7,IIC_x)!=RTC_OK)
	{
		return RTC_FAIL;
	}	
		
	buf[0]=bcd_to_dec(buf[0]);
	buf[1]=bcd_to_dec(buf[1]);
	buf[2]=bcd_to_dec(buf[2] & 0x7F);
	buf[3]=bcd_to_dec(buf[3]);	
	buf[4]=bcd_to_dec(buf[4]);
	buf[5]=bcd_to_dec(buf[5]);	
	buf[6]=bcd_to_dec(buf[6]);
	
	Usart_Printf("当前时间:%d年%02d月%02d日 %02d时%02d分%02d秒 星期%02d\n",buf[6],buf[5],buf[4],buf[2],buf[1],buf[0],buf[3]);

	return RTC_OK;
}

/**
功能：打印ctr命令
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void MPrint_ctr(void)
{
	uint8_t i,j,k,ctr_addr,ctr_len;
	
	ctr_addr=0x00;
	
	if(Check_RTCtype(IIC_TEST)==FALSE)
		ctr_len=32;
	else
		ctr_len=44;
	
	Usart_Printf("CTR:\n");
	Usart_Printf("\n");
	
	if(OTP_ReadBytes(ctr_addr,LongA,ctr_len,IIC_TEST)==FALSE)
	{
		MPrint_CmdFail();
		return;
	}
	
	for(i = 0; i <= ctr_len / 16; i++)  
	{  
		if(i == ctr_len / 16)  
		{  
			k = ctr_len % 16;  
			if(k == 0)  
				return;  
		}  
		else  
			k = 16;  

		Usart_Printf("ADDR |");  
		
		for(j = 0; j < k; j++)  
		{  
			// 直接执行 CMD_CTR 的逻辑  
			Usart_Printf(" %02XH |", ctr_addr + i * 16 + j);  
		}  
		
		Usart_Printf("\nDATA |");  
		
		for(j = 0; j < k; j++)  
		{  
			// 直接执行 CMD_CTR 的逻辑  
			Usart_Printf(" %02X  |", LongA[i * 16 + j]);  
		}  
		
		Usart_Printf("\n\n");
	}

}

/**
功能：打印ram数据
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void MPrint_ram(void)
{
	uint8_t i,j,k,ram_addr,ram_len;
		
	if(Check_RTCtype(IIC_TEST)==FALSE)
	{
		ram_addr=0x14;
		ram_len=12;
	}
	else
	{
		ram_addr=0x2C;
		ram_len=70;
	}

	Usart_Printf("RAM:\n");
	Usart_Printf("\n");
	
	if(OTP_ReadBytes(ram_addr,LongA,ram_len,IIC_TEST)==FALSE)
	{
		MPrint_CmdFail();
		return;
	}
	
	for(i = 0; i <= ram_len / 16; i++)  
	{  
		if(i == ram_len / 16)  
		{  
			k = ram_len % 16;  
			if(k == 0)  
				return;  
		}  
		else  
			k = 16;  

		Usart_Printf("ADDR |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %02XH |", ram_addr + i * 16 + j);  
		}  
		
		Usart_Printf("\nDATA |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %02X  |", LongA[i * 16 + j]);  
		}  
		
		Usart_Printf("\n\n");
	}

}

/**
功能：打印testmodel数据
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void MPrint_testmodel(void)
{
	uint8_t i,j,k,testmodel_addr,testmodel_len;
	
	testmodel_addr=0x3F;
	
	testmodel_len=9;
	
	Usart_Printf("TEST MODEL:\n");
	Usart_Printf("\n");
	
	if(OTP_ReadBytes(testmodel_addr,LongA,testmodel_len,IIC_TEST)==FALSE)
	{
		MPrint_CmdFail();
		return;
	}
	
	for(i = 0; i <= testmodel_len / 16; i++)  
	{  
		if(i == testmodel_len / 16)  
		{  
			k = testmodel_len % 16;  
			if(k == 0)  
				return;  
		}  
		else  
			k = 16;  

		Usart_Printf("ADDR |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %02XH |", testmodel_addr + i * 16 + j);  
		}  
		
		Usart_Printf("\nDATA |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %02X  |", LongA[i * 16 + j]);  
		}  
		
		Usart_Printf("\n\n");
	}
	
}

/**
功能：打印id
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void MPrint_id(void)
{
	uint8_t i,j,k,id_addr,id_len;
	
	id_addr=0x72;
	
	id_len=8;
	
	Usart_Printf("ID:\n");
	Usart_Printf("\n");
	
	if(OTP_ReadBytes(id_addr,LongA,id_len,IIC_TEST)==FALSE)
	{
		MPrint_CmdFail();
		return;
	}
	
	for(i = 0; i <= id_len / 16; i++)  
	{  
		if(i == id_len / 16)  
		{  
			k = id_len % 16;  
			if(k == 0)  
				return;  
		}  
		else  
			k = 16;  

		Usart_Printf("ADDR |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %02XH |", id_addr + i * 16 + j);  
		}  
		
		Usart_Printf("\nDATA |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %02X  |", LongA[i * 16 + j]);  
		}
		
		Usart_Printf("\n\n");
	}
	
}

/**
功能：打印未使用段数据
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void MPrint_retain(void)
{
	uint8_t i,j,k,retain_addr,retain_len;
	
	retain_addr=0x7A;
	
	retain_len=232;
	
	Usart_Printf("RETAIN:\n");
	Usart_Printf("\n");
	
	if(OTP_ReadBytes(retain_addr,LongA,retain_len,IIC_TEST)==FALSE)
	{
		MPrint_CmdFail();
		return;
	}
	
	for(i = 0; i <= retain_len / 16; i++)  
	{  
		if(i == retain_len / 16)  
		{  
			k = retain_len % 16;  
			if(k == 0)  
				return;  
		}  
		else  
			k = 16;  

		Usart_Printf("ADDR |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %03XH |", retain_addr + i * 16 + j);  
		}  
		
		Usart_Printf("\nDATA |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf("  %02X  |", LongA[i * 16 + j]);  
		}

		Usart_Printf("\n\n");
		
	}
	
}

/**
功能：打印config命令
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void MPrint_config(void)
{
	uint8_t i,j,k,config_len;
	uint32_t config_addr;
	
	config_addr=0x162;
	
	config_len=16;
	
	Usart_Printf("CONFIG:\n");
	Usart_Printf("\n");
	
	if(OTP_ReadBytes(config_addr,LongA,config_len,IIC_TEST)==FALSE)
	{
		MPrint_CmdFail();
		return;
	}
	
	for(i = 0; i <= config_len / 16; i++)  
	{  
		if(i == config_len / 16)  
		{  
			k = config_len % 16;  
			if(k == 0)  
				return;  
		}  
		else  
			k = 16;  

		Usart_Printf("ADDR |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %03XH |", config_addr + i * 16 + j);  
		}  
		
		Usart_Printf("\nDATA |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf("  %02X  |", LongA[i * 16 + j]);  
		}
		
		Usart_Printf("\n\n");
	}
	
}

/**
功能：打印温补数据
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void MPrint_wenbu(void)
{
	uint8_t i,j,k;
	uint32_t wenbu_addr,wenbu_len;
	
	wenbu_addr=0x172;
	
	wenbu_len=256;
	
	Usart_Printf("WENBU ADDR:\n");
	Usart_Printf("\n");
	
	if(OTP_ReadBytes(wenbu_addr,LongA,wenbu_len,IIC_TEST)==FALSE)
	{
		MPrint_CmdFail();
		return;
	}
	
	for(i = 0; i <= wenbu_len / 16; i++)  
	{  
		if(i == wenbu_len / 16)  
		{  
			k = wenbu_len % 16;  
			if(k == 0)  
				return;  
		}  
		else  
			k = 16;  

		Usart_Printf("ADDR |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %03XH |", wenbu_addr + i * 16 + j);  
		}  

		Usart_Printf("\nDATA |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf("  %02X  |", LongA[i * 16 + j]);
		}
		
		Usart_Printf("\n\n");
	}
	
}

/**
功能：打印电容输入配置数据
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void MPrint_capin(void)
{
	uint8_t i,j,k;
	uint32_t capin_addr,capin_len;
	
	capin_addr=0x272;
	
	capin_len=256;
	
	Usart_Printf("CAPIN ADDR:\n");
	Usart_Printf("\n");
	
	if(OTP_ReadBytes(capin_addr,LongA,capin_len,IIC_TEST)==FALSE)
	{
		MPrint_CmdFail();
		return;
	}
	
	for(i = 0; i <= capin_len / 16; i++)  
	{  
		if(i == capin_len / 16)  
		{  
			k = capin_len % 16;  
			if(k == 0)  
				return;  
		}  
		else  
			k = 16;  

		Usart_Printf("ADDR |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %03XH |", capin_addr + i * 16 + j);  
		}  
		
		Usart_Printf("\nDATA |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf("  %02X  |", LongA[i * 16 + j]);
		}
		
		Usart_Printf("\n\n");
	}
	
}


/**
功能：打印电容输出数据
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void MPrint_capout(void)
{
	uint8_t i,j,k;
	uint32_t capout_addr,capout_len;
	
	capout_addr=0x372;
	
	capout_len=256;
	
	Usart_Printf("CAPOUT ADDR:\n");
	Usart_Printf("\n");
	
	if(OTP_ReadBytes(capout_addr,LongA,capout_len,IIC_TEST)==FALSE)
	{
		MPrint_CmdFail();
		return;
	}
	
	for(i = 0; i <= capout_len / 16; i++)  
	{  
		if(i == capout_len / 16)  
		{  
			k = capout_len % 16;  
			if(k == 0)  
				return;  
		}  
		else  
			k = 16;  

		Usart_Printf("ADDR |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %03XH |", capout_addr + i * 16 + j);  
		}  
		
		Usart_Printf("\nDATA |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf("  %02X  |", LongA[i * 16 + j]);
		}
		
		Usart_Printf("\n\n");
	}
	
}

/**
功能：打印configx数据
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void MPrint_configx(void)
{
	uint8_t i,j,k;
	uint32_t configx_addr,configx_len;
	
	configx_addr=0x162;
	
	configx_len=16;
	
	Usart_Printf("CONFIGX:\n");
	Usart_Printf("\n");
	
	if(OTP_ReadBytes(configx_addr,LongA,configx_len,IIC_TEST)==FALSE)
	{
		MPrint_CmdFail();
		return;
	}
	
	for(i = 0; i <= configx_len / 16; i++)  
	{  
		if(i == configx_len / 16)  
		{  
			k = configx_len % 16;  
			if(k == 0)  
				return;  
		}  
		else  
			k = 16;  

		Usart_Printf("ADDR |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf("cfig%X|",(uint32_t)j);  
		}  

		Usart_Printf("\nDATA |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %02X  |", LongA[i * 16 + j]);
		}
		
		Usart_Printf("\n\n");
	}
	
}

/**
功能：打印温补otp数据
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void MPrint_wenbuotp(void)
{
	uint8_t i,j,k;
	uint32_t wenbuotp_addr,wenbuotp_len;
	
	wenbuotp_addr=0x172;
	
	wenbuotp_len=256;
	
	Usart_Printf("WENBUOTP OTPADDR:\n");
	Usart_Printf("\n");
	
	if(OTP_ReadBytes(wenbuotp_addr,LongA,wenbuotp_len,IIC_TEST)==FALSE)
	{
		MPrint_CmdFail();
		return;
	}
	
	for(i = 0; i <= wenbuotp_len / 16; i++)  
	{  
		if(i == wenbuotp_len / 16)  
		{  
			k = wenbuotp_len % 16;  
			if(k == 0)  
				return;  
		}  
		else  
			k = 16;  

		Usart_Printf("ADDR |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %03XH |", wenbuotp_addr + i * 16 + j - 0x72); 
		}  

		Usart_Printf("\nDATA |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf("  %02X  |", LongA[i * 16 + j]);
		}
		
		Usart_Printf("\n\n");
	}
	
}

/**
功能：打印电容输入otp数据
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void MPrint_capinotp(void)
{
	uint8_t i,j,k;
	uint32_t capinotp_addr,capinotp_len;
	
	capinotp_addr=0x272;
	
	capinotp_len=256;
	
	Usart_Printf("CAPINOTP OTPADDR:\n");
	Usart_Printf("\n");
	
	if(OTP_ReadBytes(capinotp_addr,LongA,capinotp_len,IIC_TEST)==FALSE)
	{
		MPrint_CmdFail();
		return;
	}
	
	for(i = 0; i <= capinotp_len / 16; i++)  
	{  
		if(i == capinotp_len / 16)  
		{  
			k = capinotp_len % 16;  
			if(k == 0)  
				return;  
		}  
		else  
			k = 16;  

		Usart_Printf("ADDR |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %03XH |", capinotp_addr + i * 16 + j - 0x72); 
		}  

		Usart_Printf("\nDATA |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf("  %02X  |", LongA[i * 16 + j]);
		}

		Usart_Printf("\n\n");
	}
	
}

/**
功能：打印电容输出opt数据
修改：sjm
参数：-
返回值：-
时间：2025.06.04
**/
void MPrint_capoutotp(void)
{
	uint8_t i,j,k;
	uint32_t capoutotp_,capinotp_len;
	
	capoutotp_=0x372;
	
	capinotp_len=256;
	
	Usart_Printf("CAPOUTOTP OTPADDR:\n");
	Usart_Printf("\n");
	
	if(OTP_ReadBytes(capoutotp_,LongA,capinotp_len,IIC_TEST)==FALSE)
	{
		MPrint_CmdFail();
		return;
	}
	
	for(i = 0; i <= capinotp_len / 16; i++)  
	{  
		if(i == capinotp_len / 16)  
		{  
			k = capinotp_len % 16;  
			if(k == 0)  
				return;  
		}  
		else  
			k = 16;  

		Usart_Printf("ADDR |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf(" %03XH |", capoutotp_ + i * 16 + j - 0x72); 
		}  
		
		Usart_Printf("\nDATA |");  
		
		for(j = 0; j < k; j++)  
		{  
			Usart_Printf("  %02X  |", LongA[i * 16 + j]);
		}
		
		Usart_Printf("\n\n");
	}
	
}




