#include "mprintf.h"


/* Function	- MPrint_Help
 * Brief		- None
 * Parameter- None
 * Return		- None
 * Editor		- 						
 * Date			- 
 */
void MPrint_Help(void)
{
	Usart_Printf("help:....................Print command.(使用方法:@+指令)\n");
	Usart_Printf("version:.................Print current version.\n");
	Usart_Printf("time:....................Print time and data.\n");
	Usart_Printf("ctr:.....................Print control register.\n");
	Usart_Printf("ram:.....................Print RAM.\n");
	Usart_Printf("testmode:................Print tes mode data.\n");
	Usart_Printf("id:......................Print id.\n");
	Usart_Printf("retain:..................Print data reserve.\n");
	Usart_Printf("config:..................Print config register.\n");
	Usart_Printf("wenbu:...................Print digital compensation.\n");
	Usart_Printf("capin:...................Print input capacitance.\n");
	Usart_Printf("capout:..................Print output capacitance.\n");
	Usart_Printf("configX:.................Print config register number.\n");
	Usart_Printf("wenbuOTP:................Print digital compensation and OTP addr.\n");
	Usart_Printf("capinOTP:................Print input capacitance and OTP addr.\n");
	Usart_Printf("capoutOTP:...............Print output capacitance and OTP addr.\n\n");
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
