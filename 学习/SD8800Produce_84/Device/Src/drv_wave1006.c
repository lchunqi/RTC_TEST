/*
 *  drivers  wave1006
 * Copyright (C) 2022 shenzhen wave
 * version:v1.0.0--2023-2-12
 * This program is free software; you can redistribute it and/or modify
 *  Driver for wave1006 RTC 
 */
#include "drv_wave1006.h"
#include "rtc_i2c.h"
#include "power.h"
#include "calculate.h"
#include "rtc.h"
#define BIT(x)  (1 << x)

#define WAVE1006_WP_RESET_SEQ1          0x00
/*enable write protect sequence*/
#define WAVE1006_WP_ENABLE_SEQ1         0x54
#define WAVE1006_WP_ENABLE_SEQ2         0x28
#define WAVE1006_WP_ENABLE_SEQ3         0x5C

/*disable write protect sequence*/
#define WAVE1006_WP_DISABLE_SEQ1        0x70
#define WAVE1006_WP_DISABLE_SEQ2        0x0C
#define WAVE1006_WP_DISABLE_SEQ3        0x38


wave1006_otp_arg_t g_sd3068_config;     //otp_config.c中定义的烧写基本结构体
wave1006_otp_arg_t g_sd8825_config;
// wave1006_otp_arg_t g_sd8800_config;     
wave1006_otp_arg_t g_sd8810_config;
wave1006_otp_arg_t g_sd8804_config;
wave1006_otp_arg_t g_sd8931_config;
wave1006_otp_arg_t g_sd8939_config;
wave1006_otp_arg_t g_sd8200_config;
wave1006_otp_arg_t g_sd8840_config;
wave1006_otp_arg_t g_sd8902_config;
wave1006_otp_arg_t g_sd8800_config = {
	0,         //otp_wp_config
	1,         //test_wp_config
	DEV_SD8800,//device_type_config 
	0,         //hour_12_config          		// no support 12 hour
	1,         //week_encode_config   	  		//00-06 as sunday-monday
	2,         //freq_en_config    	    		//use one bit enbale control
	PKG_SD8800,//package_type_config
	1,         //ae_polarity_config  	 			//1 is alarm enable
	0,         //week_day_alarm_config 			//both week and day can select alarm
	0,         //wada_polarity_config  			//if the "week_day_alarm config" as 1,this bit not use
	1,         //week_alarm_config     			//every bit represent a day
	0x1e,      //alarm_valid_config    			//second -year can alarm
	2,         //ae_disable_config     			//when all alarm is disable, alarm is close
	1,         //timer_freq_source_config		//use 2 bit select timer frequency source 
	1,				 //id_support_config
	0, 				 //timer_pause_support_config	//not support timer pause
  	0, 				 //timer_reset_support_config	//not support timer reset
	1,				 //int_out_sel_config					//use INTS1:0 bits select alarm or freq output
  	0,				 //int_out_rst_config					//int output high when flag clear or int is disable
  	1, 				 //wp_type_config							//use three wp bit
	0, 				 //time_rst_support_config		//not support time power on reset
	0, 				 //group2_alarm_support_config//not support group2 alarm
	0, 				 //reg_addr_reset_config			//read and write stop addr is reset
	1, 				 //time_update_support_config	//not support time update
	112,       //temp_offset_config  				//温度偏移值
	0,				 //digtal_comp_sel_config			//1:load form otp, 0:load form reg 
	0x32,			 //device_addr_config 
	0,				 //alarm_ext_config						//not support alarm extend
	0,				 //timer_ext_config						//not support timer extend
	1,				 //event_support_config 			//support event extend
	1,				 //pps_support_config					//support 1 pps extend
	1,				 //temp_comp_support_config		//1：支持温补   0：不支持温补
	1,				 //digtal_comp_ext_config	    //support digtal extend,digtal_comp_sel_config must this bit has effective
	1,				 //ram_ext_config							//not support ram extend
	1,				 //sub_second_ext_config 			//support sub second extend
	1,				 //charge_ext_config					//1:支持扩展充电  0:不支持扩展充电
	1,				 //bat_val_ext_config
	0,				 //rst_out_support_config 
	0,				 //temp_alarm_ext_config
	1,				 //temp_val_ext_config
	5,  			 //rule_temp_adj_config
  	3,				 //rule_bat_adj_config
	3,				 //osc_cap_sel_config1        //封装电容选择值2024.03.1
	1,    		 //adjust_temp_1_3_ppm_config	//3ppm
	0,				 //en_bat_pulldown_config     //有BAT脚的配置为0,没有BAT脚的配置为1通过内部200k下拉到地
	0x19f7,		 //osc_config_config					//13'b00_010_1001_0111; 
	0,				 //spi_freq_en_config					//not support
	0,				 //vol_sel_config
	1, 				 //freq_src_config						//use 4 bit select frequency
	0, 				 //k32_support_config					// support 32k alone output
	0,				 //fout_open_drain_config
	0,				 //sout_open_drain_config
	0,				 //comp_period_config
	0,				 //rst_time_config
	0,			 	 //adjust_25_config
	0,				 //spi_mode_config
	0,				 //bus_select_config					//i2c
	1, 				 //en_charge_config						//1:支持充电  0:不支持充电
	0x177f, 	   //osc_start_config	  				//13'b00_010_0001_0011
	3,				 //charge_vol_adj_config
	0,				 //timer_rst_out_config    
}; 
uint8_t g_dev_id[40];
wave1006_temp_arg_t g_temp_arg[420]; //-60~150


int g_digtal_adj[420] = {0};
float g_cap_val[420] = { 0};
uint16_t g_offset = 0;


typedef struct  
{
	unsigned char device_type;
	wave1006_test_mode_e test_mode;    
} wave1006_state_t;    		//测试模式选择

wave1006_state_t g_wave1006_state = {0, WAVE1006_NORMAL_MODE};


wave1006_exit_code_e get_device_addr(wave1006_dev_e dev, unsigned char *dev_addr)
{
	switch (dev)
	{
		case DEV_SD3068:
		case DEV_SD8800:
		case DEV_SD8810:
		case DEV_SD8804:	
			*dev_addr = 0x64;
			break;
		case DEV_SD8931:
		case DEV_SD8939:
		case DEV_SD8200://BQ32000
			*dev_addr = 0xd0;
			break;
		default:
			return WAVE1006_EXIT_ARG_ERR;
			break;
	}
	return WAVE1006_EXIT_OK;
}

/*
*Function: otp_prg_power_ctrl
*Description: 
*Input: enbale:otp power enable;vol_sel:otp voltage input,0:5V,1:8.5V
*Output: 
*Return: 
*Others:
*/       //OTP编程电源控制
static void otp_prg_power_ctrl(unsigned char enbale)
{
	if (enbale == 1) {
		rtc_otp_power(8.5); //烧录otp电压
	} else {
		rtc_otp_power(5);  //读otp电压
	}
}
/*Function: wave1006_write_enable_seq
*Description: 
*Input:
*Output: 
*Return: 
*Others:
*/
static wave1006_exit_code_e wave1006_write_enable_seq(wave1006_dev_e dev)
{
	unsigned char buf[2];
	wave1006_exit_code_e ret;
	
	ret = drv_wave1006_read_reg(dev, WAVE1006_REG_CTR1, 2,  buf);
	if (ret != WAVE1006_EXIT_OK)
		return ret;

	buf[1] |=  BIT(7);
	ret = drv_wave1006_write_reg(dev, WAVE1006_REG_CTR2, 1,  &buf[1]);
	if (ret != WAVE1006_EXIT_OK)
		return ret;

	buf[0] |=  BIT(2);
	buf[0] |=  BIT(7);
	ret = drv_wave1006_write_reg(dev, WAVE1006_REG_CTR1, 1,  &buf[0]);
	if (ret != WAVE1006_EXIT_OK)
		return ret;
	
	return WAVE1006_EXIT_OK;
}

/**
 * @brief wave1006 read reg
 * 
 */
wave1006_exit_code_e drv_wave1006_read_reg(wave1006_dev_e dev, unsigned char reg, unsigned char length, unsigned char *buf)
{
	unsigned char dev_addr;

	if (get_device_addr(dev, &dev_addr) != WAVE1006_EXIT_OK) {
		return WAVE1006_EXIT_ARG_ERR;
	} 
	if (rtc_read_multi_bytes(dev_addr, reg, buf, length) != I2C_OK) {
		return WAVE1006_EXIT_NO_ACK;
	}
	return WAVE1006_EXIT_OK;
} 

/**
 * @brief wave1006 write register
 * 
 */
wave1006_exit_code_e drv_wave1006_write_reg(wave1006_dev_e dev, unsigned char reg, unsigned char length, const unsigned char *buf)
{
	unsigned char dev_addr;

	if (get_device_addr(dev, &dev_addr) != WAVE1006_EXIT_OK) 
		return WAVE1006_EXIT_ARG_ERR;
	if (rtc_write_multi_bytes(dev_addr, reg, buf, length) != I2C_OK)
		return WAVE1006_EXIT_NO_ACK;
	return WAVE1006_EXIT_OK;
}


wave1006_exit_code_e wave1006_enter_test_mode(wave1006_test_mode_e mode)     //烧写1006时使用
{
	unsigned char buf[7];

	/*1. set test_flag=1*/
	buf[0] = BIT(2);
	if(drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_EXTEND, 1, &buf[0]) != WAVE1006_EXIT_OK)
	{
		return WAVE1006_EXIT_ARG_ERR;
	}

	/*2.set test mode*/
	buf[0] = mode;
	if(drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START, 1, &buf[0]) != WAVE1006_EXIT_OK)
	{
		return WAVE1006_EXIT_ARG_ERR;
	}
	/*3. set time reg*/
	buf[0] = 0x6c;//sec
	buf[1] = 0x6c;//min
	buf[2] = 0x3c;//hour
	buf[3] = 0x07;//week
	buf[4] = 0x3c;//day
	buf[5] = 0x1c;//month
	buf[6] = 0xcc;//year
	if(drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_SEC, 7, &buf[0]) != WAVE1006_EXIT_OK)
	{
		return WAVE1006_EXIT_ARG_ERR;
	}
	/*check test_flag*/	
	HAL_Delay(100);
	UsbPrintf("Enter test mode success!\r\n");
	return WAVE1006_EXIT_OK;

	// if(drv_wave1006_read_reg(DEV_SD8804, WAVE1006_REG_EXTEND, 1, buf) != WAVE1006_EXIT_OK)
	// {
	// 	return WAVE1006_EXIT_ARG_ERR;
	// }

	// if((buf[0] & BIT(3)) != 0)
	// {
	// 	UsbPrintf("Enter test mode success!\r\n");
	// 	return WAVE1006_EXIT_OK;
	// }
	// else
	// {
	// 	UsbPrintf("Enter test mode faild!\r\n");
	// 	return WAVE1006_EXIT_FAIL;
	// }
}

wave1006_exit_code_e sd3031_enter_test_mode(wave1006_test_mode_e mode)
{
	unsigned char buf[7];
	wave1006_exit_code_e ret;

	wave1006_write_enable_seq(DEV_SD3068);
	/*1. set test_flag=1*/
	buf[0] = BIT(2);
	ret = drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_EXTEND, 1, &buf[0]);
	if (ret != WAVE1006_EXIT_OK)
		return ret;
	
	buf[0] = mode;
	ret = drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START, 1, &buf[0]);
	if (ret != WAVE1006_EXIT_OK)
		return ret;
	/*3. set time reg*/
	buf[0] = 0x6c;//sec
	buf[1] = 0x6c;//min
	buf[2] = 0x3c;//hour
	buf[3] = 0x07;//week
	buf[4] = 0x3c;//day
	buf[5] = 0x1c;//month
	buf[6] = 0xcc;//year
	ret = drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_SEC, 7, &buf[0]);
	if (ret != WAVE1006_EXIT_OK)
		return ret;
	/*wait 40ms*/
//	HAL_Delay((40);
	/*check test_flag*/	
	HAL_Delay(1000);
	
	ret = drv_wave1006_read_reg(DEV_SD3068, WAVE1006_REG_EXTEND, 1, buf);
	if (ret != WAVE1006_EXIT_OK)
		return ret;
	if((buf[0] & BIT(3)) != 0)
	{
		UsbPrintf("Enter test mode success!\r\n");
		return WAVE1006_EXIT_OK;
	}
	else
	{
		UsbPrintf("Enter test mode faild!\r\n");
		return WAVE1006_EXIT_FAIL;
	}
//	g_wave1006_state.test_mode = mode;
}

wave1006_exit_code_e wave1006_get_test_flag(void)
{
	uint8_t buf;
	
	if (drv_wave1006_read_reg(DEV_SD8804, WAVE1006_REG_EXTEND, 1, &buf) != WAVE1006_EXIT_OK) {
		return WAVE1006_EXIT_ARG_ERR;
	}
	
	if ((buf & BIT(3)) == 0) {
		return WAVE1006_EXIT_ARG_ERR;
	} else {
		return WAVE1006_EXIT_OK;
	}
}

wave1006_exit_code_e sd3031_get_test_flag(void)
{
	uint8_t buf;
	
	if (drv_wave1006_read_reg(DEV_SD3068, WAVE1006_REG_EXTEND, 1, &buf) != WAVE1006_EXIT_OK) {
		return WAVE1006_EXIT_ARG_ERR;
	}
	
	if ((buf & BIT(3)) == 0) {
		return WAVE1006_EXIT_ARG_ERR;
	} else {
		return WAVE1006_EXIT_OK;
	}
}

wave1006_exit_code_e wave1006_osc_config_set(wave1006_osc_config_t *config)
{
	unsigned char buf[2];
	wave1006_exit_code_e ret;

	/*RAM24:osc_config[7:0]; RAM25[4:0]:osc_config[12:8]*/
	buf[0] = ((config->osc_vref_adj & 0x01)<< 7) | (config->vol_cure_adj << 5) | (config->osc_res_adjust << 3) | (config->osc_cap12p_25p << 2) | (config->osc_fb << 1) | config->oscf_bdeep;
	buf[1] = (config->leakage << 4) | (config->hosc << 3) | (config->cur_res_adj << 2) | ((config->osc_vref_adj >> 1) & 0x01);
	
	ret = drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 23, 2, &buf[0]);
	if (ret != WAVE1006_EXIT_OK) { return ret;}
	/*if not in test mode,enter test mode*/
//	if ((g_wave1006_state.test_mode & WAVE1006_TEST_OSC) != WAVE1006_TEST_OSC)
//	{
//		ret = wave1006_enter_test_mode(g_wave1006_state.test_mode | WAVE1006_TEST_OSC);
//		if (ret != WAVE1006_EXIT_OK)
//			return ret;
//		g_wave1006_state.test_mode |= WAVE1006_TEST_OSC;
//	}
	if (wave1006_enter_test_mode(WAVE1006_TEST_OSC) != WAVE1006_EXIT_OK) {
		return WAVE1006_EXIT_FAIL;
	}
	return WAVE1006_EXIT_OK;
}

static wave1006_exit_code_e wave1006_write_otp_single(unsigned short addr, unsigned short Data)
{
	unsigned char buf[4];
	wave1006_exit_code_e ret;

	//set addr
	buf[0] = addr & 0xff;
	buf[1] = (addr >> 8) & 0x03;
	//set Data
	buf[2] = Data & 0xff;
	buf[3] = (Data >> 8) & 0x03f;
		ret = drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 2, 4, &buf[0]);
		if (ret != WAVE1006_EXIT_OK)
			return ret;
	//pwe=1
	buf[0] = 0xb2;
		ret = drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
		if (ret != WAVE1006_EXIT_OK)
			return ret;
		//delay 100us
	//pwe=0
	buf[0] = 0xa2;
	ret = drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
	return ret;
}

static wave1006_exit_code_e wave1006_read_otp_single(unsigned short addr, unsigned short *Data)
{
	unsigned char buf[4];
	wave1006_exit_code_e ret;

	/*set addr*/
	buf[0] = addr & 0xff;
	buf[1] = (addr >> 8) & 0x03;
	ret = drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 2, 2, &buf[0]);
	if (ret != WAVE1006_EXIT_OK)
		return ret;
	/*prd=1*/
	buf[0] = 0xc0;
	ret = drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
	if (ret != WAVE1006_EXIT_OK)
		return ret;
	/*prd=0*/
	buf[0] = 0x80;
	ret = drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
	if (ret != WAVE1006_EXIT_OK)
		return ret;
	ret = drv_wave1006_read_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 4, 2, buf);
	*Data = ((buf[1] & 0x3f) << 8) | buf[0];
	return ret;
}


//配置为SD3031烧写为不同型号
wave1006_exit_code_e wave1006_write_otp(wave1006_otp_arg_t const *config_arg,  unsigned char const *id, wave1006_temp_arg_t const *temp_arg, uint8_t rtcNumber)
{
	uint16_t i;
	uint8_t buf1;
	uint8_t buf[5]={0,0,0,0,0};
	uint16_t otp_temp;
	uint32_t check_sum_wr = 0;     //写数据校验缓存
	uint32_t check_sum_rd = 0;		 //读数据校验缓存
	uint16_t wr_pos = 0;

	UsbPrintf("WRITE ALL OTP!\r\n");
	rtc_group_power(rtcNumber, POWER_I2C);//-------------------
	rtc_set_channel(rtcNumber, RTC_INIT_1006);
	HAL_Delay(100);
	buf1 = 0x40;
	if(drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_CTR3, 1, &buf1) != WAVE1006_EXIT_OK)   //如果是有应该先关闭32K输出(默认为关闭SD3068的32K)
	{
		return WAVE1006_EXIT_FAIL;
	}
	if(drv_wave1006_read_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 20, 1, &buf1) != WAVE1006_EXIT_OK)   //选择为SD3031封装(必须在进测试模式之前选择，否则芯片pad改变)
	{
		return WAVE1006_EXIT_FAIL;
	}
	buf1 &= ~0x0f;
	buf1 |= 0x0C;//根据pad配置表，编号12为3031
	if(drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 20, 1, &buf1) != WAVE1006_EXIT_OK)
	{
		return WAVE1006_EXIT_FAIL;
	}
		/*clear RAM2~6*/
	if(drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 1, 5, &buf[0]) != WAVE1006_EXIT_OK)
	{
		return WAVE1006_EXIT_FAIL;
	}
		
	if(wave1006_enter_test_mode(WAVE1006_TEST_PAD | WAVE1006_TEST_OTP) != WAVE1006_EXIT_OK)   //此处一定要注意，没进入测试模式也需要将I0切换掉，或者不能继续运行程序
	{
		return WAVE1006_EXIT_FAIL;
	}
 
    /*switch i2c prot, io6 =5V*/
//	otp_prg_power_ctrl(0); 
 	rtc_set_channel(rtcNumber, RTC_SD3031);//切换pad引脚
	HAL_Delay(10);
    /*vdd=1*/
  buf[0] = 0x80;
	if (drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0])!= WAVE1006_EXIT_OK) {
		UsbPrintf("write reg error in test mode\n");
		return WAVE1006_EXIT_FAIL;
	}
    /*ptm=010*/
  buf[0] = 0x82;
  drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
//    pprog = 1
  buf[0] = 0xa2;
  drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);

	UsbPrintf("/**************************** WRITE OTP ********************************/\r\n");
	//    io6 =8.5V
 	rtc_otp_power(8.5);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET); //打开8.5v的开关

//    start write otp
	otp_temp =  0x3ffc | (config_arg->test_wp_config << 1) | config_arg->otp_wp_config;
	check_sum_wr += otp_temp;
  wave1006_write_otp_single(0, otp_temp);// addr 0
	UsbPrintf("otp addr0:%hx\r\n", otp_temp);

	otp_temp = (config_arg->package_type_config << 10) | (config_arg->freq_en_config << 7) | (config_arg->week_encode_config << 5) | (config_arg->hour_12_config << 4) | 
		config_arg->device_type_config;
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(1, otp_temp);
	UsbPrintf("otp addr1:%hx\r\n", otp_temp);

	otp_temp = ((config_arg->timer_freq_source_config & 0x01) << 13) | (config_arg->ae_disable_config << 11) | (config_arg->alarm_valid_config << 4) | 
		(config_arg->week_alarm_config << 3) | (config_arg->wada_polarity_config << 2) |  (config_arg->week_day_alarm_config << 1) | config_arg->ae_polarity_config;
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(2, otp_temp);
	UsbPrintf("otp addr2:%hx\r\n", otp_temp);

	otp_temp = (config_arg->time_update_support_config << 13) | (config_arg->reg_addr_reset_config << 11) | (config_arg->group2_alarm_support_config << 10) | 
		(config_arg->time_rst_support_config << 9) | (config_arg->wp_type_config << 8) | (config_arg->int_out_rst_config << 7) |  (config_arg->int_out_sel_config << 4) |  
		(config_arg->timer_reset_support_config << 3) | (config_arg->timer_pause_support_config << 2) | (config_arg->id_support_config << 1) | 
		((config_arg->timer_freq_source_config >> 1) & 0x01);
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(3, otp_temp);
	UsbPrintf("otp addr3:%hx\r\n", otp_temp);
	
	otp_temp = ((config_arg->device_addr_config & 0x0f) << 10) | (config_arg->digtal_comp_sel_config << 9) | (1 << 8) | config_arg->temp_offset_config ;
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(4, otp_temp);
	UsbPrintf("otp addr4:%hx\r\n", otp_temp);

	otp_temp = (config_arg->rst_out_support_config << 13) | (config_arg->bat_val_ext_config << 12) | (config_arg->charge_ext_config << 11) | 
		(config_arg->sub_second_ext_config << 10) | (config_arg->ram_ext_config << 9) | (config_arg->digtal_comp_ext_config << 8) | 
		(config_arg->temp_comp_support_config << 7) | (config_arg->pps_support_config << 6)  | (config_arg->event_support_config << 5) | 
		(config_arg->timer_ext_config << 4) | (config_arg->alarm_ext_config << 3) | ((config_arg->device_addr_config >> 4) & 0x07);
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(5, otp_temp);
	UsbPrintf("otp addr5:%hx\r\n", otp_temp);

	otp_temp = ((config_arg->osc_config_config & 0x03) << 12) | (config_arg->en_bat_pulldown_config << 11) | (config_arg->adjust_temp_1_3_ppm_config << 10) |  
		(config_arg->osc_cap_sel_config1 << 8) |  (config_arg->rule_bat_adj_config << 5) |(config_arg->rule_temp_adj_config << 2) |  (config_arg->temp_val_ext_config << 1) | 
		config_arg->temp_alarm_ext_config;
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(6, otp_temp);
	UsbPrintf("otp addr6:%hx\r\n", otp_temp);

	otp_temp = ((config_arg->osc_config_config << 6) & 0x3f00 ) | id[0];
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(7, otp_temp);
	UsbPrintf("otp addr7:%hx\r\n", otp_temp);

	otp_temp = (config_arg->spi_freq_en_config << 13) | (config_arg->osc_config_config & 0x1f00 ) | id[1];
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(8, otp_temp);
	UsbPrintf("otp addr8:%hx\r\n", otp_temp);

	otp_temp = ((config_arg->freq_src_config & 0x03) << 12) | (config_arg->vol_sel_config << 8) | id[2];
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(9, otp_temp);
	UsbPrintf("otp addr9:%hx\r\n", otp_temp);
	
	otp_temp = (config_arg->comp_period_config  << 12) | (config_arg->sout_open_drain_config << 11) | (config_arg->fout_open_drain_config << 10) | 
		(config_arg->k32_support_config << 9) | ((config_arg->freq_src_config & 0x04) << 6) | id[3];
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(10, otp_temp);
	UsbPrintf("otp addra:%hx\r\n", otp_temp);

	otp_temp = ((config_arg->adjust_25_config & 0x0f) << 10) | (config_arg->rst_time_config << 8) | id[4];
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(11, otp_temp);
	UsbPrintf("otp addrb:%hx\r\n", otp_temp);

	otp_temp = (config_arg->bus_select_config << 13) | (1 << 12) | (config_arg->spi_mode_config << 11) | ((config_arg->adjust_25_config << 4) & 0x700) | id[5];
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(12, otp_temp);
	UsbPrintf("otp addrc:%hx\r\n", otp_temp);

	otp_temp = ((config_arg->osc_start_config << 9) & 0x3e00) | (config_arg->en_charge_config << 8) | id[6];
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(13, otp_temp);
	UsbPrintf("otp addrd:%hx\r\n", otp_temp);

	otp_temp = ((config_arg->osc_start_config << 3) & 0x3f00) | id[7];
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(14, otp_temp);
	UsbPrintf("otp addre:%hx\r\n", otp_temp);

	otp_temp = (config_arg->timer_rst_out_config << 13)  | (config_arg->charge_vol_adj_config << 10) | ((config_arg->osc_start_config >> 3) & 0x300) | id[8];
	check_sum_wr += otp_temp;
	wave1006_write_otp_single(15, otp_temp);
	UsbPrintf("otp addrf:%hx\r\n", otp_temp);

	for (i = 16; i < 47; i++)
	{
		otp_temp = 0x3f00 | id[9+i-16];
		check_sum_wr += otp_temp;
		wave1006_write_otp_single(i, otp_temp);
		// UsbPrintf("otp addr%hx:%hx\r\n",i, otp_temp);
	}
	check_sum_wr += 0x3fff;
//	write temp value
	for (i = 48; i < 210 * 4 + 48 ; i=i+2)
	{
		otp_temp = ((temp_arg->temp_adj & 0x07) << 11) | (temp_arg->cap_com << 6) | (temp_arg->cap_in << 3) | temp_arg->cap_out;
		check_sum_wr += otp_temp;
		wave1006_write_otp_single(i, otp_temp);
		// UsbPrintf("otp addr%hx:%hx\r\n",i, otp_temp);

		otp_temp = (temp_arg->temp_val << 5) | (temp_arg->temp_adj >> 3);
		check_sum_wr += otp_temp;
		wave1006_write_otp_single(i + 1, otp_temp);
		// UsbPrintf("otp addr%hx:%hx\r\n",i + 1, otp_temp);
		temp_arg++;    //温补数据指向下一个字节
	}
	wr_pos = i;
	UsbPrintf("/**************************** READ OTP ********************************/\r\n");
//	write otp is complete, switch i2c prot, io6 =5V
	rtc_otp_power(5);  
//  pprog = 0 
    buf[0] = 0x82;
	drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
//    ptm=000
    buf[0] = 0x80;
	drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
//    pa=0, Data 
    buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = 0;
    drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 4, &buf[0]);
	UsbPrintf("wr_pos = %hx\r\n",wr_pos);
//	check write value
	for (i = 0; i < wr_pos; i++)
	{
		wave1006_read_otp_single(i, &otp_temp);
		check_sum_rd += otp_temp;
		// UsbPrintf("read otp addr%hx:%hx\r\n",i, otp_temp);
	}
	
 	rtc_otp_power(3.3);  //芯片断电退出测试模式，只有当OTP保护位烧死才会加载OTP参数,否则只是烧写好，下次不能继续烧写
	HAL_Delay(100);
	if (check_sum_rd != check_sum_wr)
	{
		UsbPrintf("write check is err %lx: cal=%lx\r\n",check_sum_rd, check_sum_wr);
		return WAVE1006_EXIT_FAIL;
	}
	UsbPrintf("write otp is successful! %lx: cal=%lx\r\n",check_sum_rd, check_sum_wr);	
	
	rtc_set_channel(rtcNumber, RTC_INIT_1006);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET); //8.5v关闭的开关
	UsbPrintf("EXIT OK!\r\n");
	return WAVE1006_EXIT_OK;
}


wave1006_exit_code_e wave1006_write_otp_one(char *str)
{
	uint8_t buf[5]={0,0,0,0,0}, buf1;
	uint16_t otp_temp;
	int Addr = 0,Data = 0;
	
	UsbPrintf("OPEN WRITE ONE!\r\n");
	CharToInt(str, 6, &Addr);
	CharToInt(str, strSearch(str, ","), &Data);
	
	rtc_otp_power(5);  //RTC和上拉电阻上电
	HAL_Delay(2000);
	buf1 = 0x40;
	if(drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_CTR3, 1, &buf1) != WAVE1006_EXIT_OK)   //如果是有应该先关闭32K输出(默认为关闭SD3068的32K)
	{
		return WAVE1006_EXIT_FAIL;
	}
	
	if(drv_wave1006_read_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 20, 1, &buf1) != WAVE1006_EXIT_OK)   //选择为SD3031封装(必须在进测试模式之前选择，否则芯片pad改变)
	{
		return WAVE1006_EXIT_FAIL;
	}
	buf1 &= ~0x0f;
	buf1 |= 0x0c;
	if(drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 20, 1, &buf1) != WAVE1006_EXIT_OK)
	{
		return WAVE1006_EXIT_FAIL;
	}
		/*clear RAM2~6*/
	if(drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 1, 5, &buf[0]) != WAVE1006_EXIT_OK)
	{
		return WAVE1006_EXIT_FAIL;
	}
		
	if(wave1006_enter_test_mode(WAVE1006_TEST_PAD | WAVE1006_TEST_OTP) != WAVE1006_EXIT_OK)   //此处一定要注意，没进入测试模式也需要将I0切换掉，或者不能继续运行程序
	{
		return WAVE1006_EXIT_FAIL;
	}
    /*vdd=1*/
  buf[0] = 0x80;
	drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
    /*ptm=010*/
  buf[0] = 0x82;
  drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
    /*pprog = 1*/
  buf[0] = 0xa2;
  drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
    /*io6 =8.5V*/
	otp_prg_power_ctrl(1);
    /*start write otp*/
	wave1006_write_otp_single(Addr, Data);
	/*write otp is complete, switch i2c prot, io6 =5V*/
	otp_prg_power_ctrl(0);    
	/*pprog = 0*/
  buf[0] = 0x82;
	drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
    /*ptm=000*/
  buf[0] = 0x80;
	drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
//    pa=0, Data 
    buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = 0;
  drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 4, &buf[0]);
	/*check write value*/
	wave1006_read_otp_single(Addr, &otp_temp);
	// UsbPrintf("read otp addr%hx:%hx\r\n",Addr, otp_temp);
	
	/*exit test mode*/
	rtc_otp_power(5);  //芯片断电退出测试模式，只有当OTP保护位烧死才会加载OTP参数
	HAL_Delay(200);
	
	UsbPrintf("EXIT I2C_OK!\r\n");
	if (otp_temp != Data)
		return WAVE1006_EXIT_FAIL;
	return WAVE1006_EXIT_OK;
}

wave1006_exit_code_e wave1006_read_otp(void)
{
	uint16_t i;
	uint8_t  buf[5]={0,0,0,0,0},buf1;
	uint16_t otp_temp = 0;

	UsbPrintf("OPEN READ!\r\n");
	rtc_otp_power(5);  //RTC和上拉电阻上电
	HAL_Delay(200);
	buf1 = 0x40;
	if(drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_CTR3, 1, &buf1) != WAVE1006_EXIT_OK)   //如果是有应该先关闭32K输出(默认为关闭SD3068的32K)
	{
		return WAVE1006_EXIT_FAIL;
	}
	
	if(drv_wave1006_read_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 20, 1, &buf1) != WAVE1006_EXIT_OK)   //选择为SD3031封装(必须在进测试模式之前选择，否则芯片pad改变)
	{
		return WAVE1006_EXIT_FAIL;
	}
	buf1 &= ~0x0f;
	buf1 |= 0x0c;
	if(drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 20, 1, &buf1) != WAVE1006_EXIT_OK)
	{
		return WAVE1006_EXIT_FAIL;
	}
		/*clear RAM2~6*/
	if(drv_wave1006_write_reg(DEV_SD8804, WAVE1006_REG_USER_RAM_START + 1, 5, &buf[0]) != WAVE1006_EXIT_OK)
	{
		return WAVE1006_EXIT_FAIL;
	}
		
	if(wave1006_enter_test_mode(WAVE1006_TEST_PAD | WAVE1006_TEST_OTP) != WAVE1006_EXIT_OK)   //此处一定要注意，没进入测试模式也需要将I0切换掉，或者不能继续运行程序
	{
		return WAVE1006_EXIT_FAIL;
	}
	
	otp_prg_power_ctrl(0);    //IO6加5V电压
	             
    /*vdd=1*/
  buf[0] = 0x80;
	drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
    
	/*pprog = 0*/
  buf[0] = 0x82;
	drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
    /*ptm=000*/
  buf[0] = 0x80;
	drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 1, &buf[0]);
    /*pa=0, Data*/
    buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = 0;
  drv_wave1006_write_reg(DEV_SD3068, WAVE1006_REG_USER_RAM_START + 1, 4, &buf[0]);
	UsbPrintf("/**************************** READ ALL OTP ********************************/\r\n");
	/*check write value*/
	for (i = 0; i < 1024; i++)
	{
		wave1006_read_otp_single(i, &otp_temp);
		UsbPrintf("addr%x:%hx\r\n",i, otp_temp);
	}
	
	/*exit test mode*/
	rtc_otp_power(5);  //芯片断电退出测试模式，只有当OTP保护位烧死才会加载OTP参数
	HAL_Delay(200);
	UsbPrintf("EXIT I2C_OK!\r\n");
	return WAVE1006_EXIT_OK;
}

uint8_t sd8800_calcul_cap(uint8_t channel)
{
	int i;
	for (i = 0; i < 420; i++) { 
		g_cap_val[i] = 0; 
		g_digtal_adj[i] = 0;
	}

	if (calculate_wave1006_temp_cap(channel, g_cap_val, g_digtal_adj, &g_offset) == 0) {
		UsbPrintf("calculate cap error\n");
		return 0;
	}
	return 1;

}

uint8_t sd8800_write_otp(uint8_t channel)
{
	int i;
	uint8_t buffer[2] = {0};
	RealTime date = {0};
	int num_adj = 0;

	if (sd8800_calcul_cap(channel) == 0) { return 0;}
	system_get_real_time(&date);
	if (date.year < 24 || date.year > 99 || date.mon < 1 || date.mon > 12 || date.day < 1 || date.day > 31) {
		UsbPrintf("date error, check ID error\n");
		return 0;
	}
	//烧写带温补的OTP    配置为SD3031   烧写保护位		
	g_sd8800_config.temp_offset_config = g_offset;			 //温度偏移值，不同芯片不一样
	g_dev_id[0] = ((date.year / 10) % 10) * 16 + (date.year % 10);
	g_dev_id[1] = ((date.mon / 10) % 10) * 16 + (date.mon % 10);
	g_dev_id[2] = ((date.day / 10) % 10) * 16 + (date.day % 10);
	g_dev_id[3] = 0x03;//板子编号
	g_dev_id[4] = 0x24;
	g_dev_id[5] = 0x02;
	g_dev_id[6] = 0x00;
	g_dev_id[7] = (channel / 10) * 16 + (channel % 10);
	for (i = 8; i < 40; i++) {
		g_dev_id[i] = 0xff;
	}
	
	for (i = 0; i < 420; i++) {
		cap_list_to_uint((uint16_t)g_cap_val[i], buffer);
		g_temp_arg[i].cap_com  = ((buffer[1] & 0x07) << 2) + ((buffer[0] >> 6) & 0x03);
		g_temp_arg[i].cap_in  = (buffer[0] >> 3) & 0x07;
		g_temp_arg[i].cap_out  = buffer[0] & 0x07;
		if (g_digtal_adj[i] < 0) {
			num_adj = (g_digtal_adj[i] / (-3)) - 1;
			num_adj = ~num_adj;
			num_adj &= 0x7f;
			num_adj |= 0xC0;
			g_temp_arg[i].temp_adj = num_adj;//0x00;
		} else {
			g_temp_arg[i].temp_adj = 0x00;
		}
		g_temp_arg[i].temp_val =(i - 120) / 2 * 3.218 + 187.9;   //SD8800特有规则
		// g_temp_arg[i].temp_val = (i - 120);//临时测试
	}
	UsbPrintf("*************** WRITE SD8800 ************\r\n");
	if (wave1006_write_otp(&g_sd8800_config, g_dev_id, g_temp_arg, channel) != WAVE1006_EXIT_OK) {
		return 0;
	}
	return 1;
}

uint8_t sd8800_ACEQ100(uint16_t config, uint16_t rtcAdc, float maxCap)
{
	int i;
	uint8_t buffer[2] = {0};
	RealTime date = {0};
	int num_adj = 0;

	for (i = 0; i < 420; i++) { 
		g_cap_val[i] = 0; 
		g_digtal_adj[i] = 0;
	}
	
	if (calculate_ACEQ100_temp_cap(g_cap_val, g_digtal_adj, config, rtcAdc, maxCap) == 0) {
		UsbPrintf("calculate cap error\n");
		return 0;
	}
	system_get_real_time(&date);
	if (date.year < 24 || date.year > 99 || date.mon < 1 || date.mon > 12 || date.day < 1 || date.day > 31) {
		UsbPrintf("date error, check ID error\n");
		return 0;
	}
	//烧写带温补的OTP    配置为SD3031   烧写保护位		
	g_sd8800_config.temp_offset_config = config;			 //温度偏移值，不同芯片不一样
	g_dev_id[0] = ((date.year / 10) % 10) * 16 + (date.year % 10);
	g_dev_id[1] = ((date.mon / 10) % 10) * 16 + (date.mon % 10);
	g_dev_id[2] = ((date.day / 10) % 10) * 16 + (date.day % 10);
	g_dev_id[3] = ((date.hour / 10) % 10) * 16 + (date.hour % 10);
	g_dev_id[4] = 0x24;
	g_dev_id[5] = 0x26;
	g_dev_id[6] = ((date.min / 10) % 10) * 16 + (date.min % 10);
	g_dev_id[7] = ((date.sec / 10) % 10) * 16 + (date.sec % 10);
	for (i = 8; i < 40; i++) {
		g_dev_id[i] = 0xff;
	}
	ControlPrintf("id:%02X%02X%02X%02X%02X%02X%02X%02X\n", 
	 	g_dev_id[0], g_dev_id[1], g_dev_id[2], g_dev_id[3], g_dev_id[4], g_dev_id[5], g_dev_id[6], g_dev_id[7]);
	for (i = 0; i < 420; i++) {
		cap_list_to_uint((uint16_t)g_cap_val[i], buffer);
		g_temp_arg[i].cap_com  = ((buffer[1] & 0x07) << 2) + ((buffer[0] >> 6) & 0x03);
		g_temp_arg[i].cap_in  = (buffer[0] >> 3) & 0x07;
		g_temp_arg[i].cap_out  = buffer[0] & 0x07;
		if (g_digtal_adj[i] < 0) {
			num_adj = (g_digtal_adj[i] / (-3)) - 1;
			num_adj = ~num_adj;
			num_adj &= 0x7f;
			num_adj |= 0xC0;
			g_temp_arg[i].temp_adj = num_adj;//0x00;
		} else {
			g_temp_arg[i].temp_adj = 0x00;
		}
		g_temp_arg[i].temp_val =(i - 120) / 2 * 3.218 + 187.9;   //SD8800特有规则
		// g_temp_arg[i].temp_val = (i - 120);//临时测试
	}
	UsbPrintf("\n*************** WRITE SD8800 ************\r\n");
	if (wave1006_write_otp(&g_sd8800_config, g_dev_id, g_temp_arg, AECQ100_CHANNEL) != WAVE1006_EXIT_OK) {
		return 0;
	}
	return 1;
}
