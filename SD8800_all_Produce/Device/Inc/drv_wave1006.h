#ifndef __DRV_WAVE1006_H
#define __DRV_WAVE1006_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

/*device addr*/
#define WAVE1006_DEV_ADR                0x64
/*time reg*/
#define WAVE1006_REG_SEC                0x00
#define WAVE1006_REG_MIN                0x01
#define WAVE1006_REG_HOUR               0x02
#define WAVE1006_REG_WDAY               0x03
#define WAVE1006_REG_MDAY               0x04
#define WAVE1006_REG_MON                0x05
#define WAVE1006_REG_YEAR               0x06
/*alarm reg*/
#define WAVE1006_REG_ALARM_SEC          0x07
#define WAVE1006_REG_ALARM_MIN          0x08
#define WAVE1006_REG_ALARM_HOUR         0x09
#define WAVE1006_REG_ALARM_WEEK         0x0A
#define WAVE1006_REG_ALARM_DAY          0x0B
#define WAVE1006_REG_ALARM_MONTH        0x0C
#define WAVE1006_REG_ALARM_YEAR         0x0D
#define WAVE1006_REG_ALARM_OE           0x0E
/* control reg*/
#define WAVE1006_REG_CTR1               0x0F
#define WAVE1006_REG_CTR2               0x10
#define WAVE1006_REG_CTR3               0x11
#define WAVE1006_REG_TTF                0x12
#define WAVE1006_REG_TD0                0x13
#define WAVE1006_REG_TD1                0x14
#define WAVE1006_REG_TD2                0x15
/*temperature reg*/
#define WAVE1006_REG_TEMP               0x16
/*i2c control reg*/
#define WAVE1006_REG_I2C_CTRL           0x17
/*charge reg*/
#define WAVE1006_REG_CHARGE             0x18
/*extend control reg*/
#define WAVE1006_REG_CTR4               0x19
#define WAVE1006_REG_CTR5               0x1A
/*battery voltage reg*/
#define WAVE1006_REG_BAT_VAL            0x1B
/*temperature low/hign alarm reg*/    
#define WAVE1006_REG_TEMP_AL            0x1C
#define WAVE1006_REG_TEMP_AH            0x1D
/*history max/min temperature reg*/
#define WAVE1006_REG_HIS_L              0x1E
#define WAVE1006_REG_HIS_H              0x1F
/*history temperature lowest  time ram*/
#define WAVE1006_REG_HIS_L_MIN          0x20
#define WAVE1006_REG_HIS_L_HOUR         0x21
#define WAVE1006_REG_HIS_L_WEEK         0x22
#define WAVE1006_REG_HIS_L_DAY          0x23
#define WAVE1006_REG_HIS_L_MON          0x24
#define WAVE1006_REG_HIS_L_YEAR         0x25
/*history temperature highest  time ram*/
#define WAVE1006_REG_HIS_H_MIN          0x26
#define WAVE1006_REG_HIS_H_HOUR         0x27
#define WAVE1006_REG_HIS_H_WEEK         0x28
#define WAVE1006_REG_HIS_H_DAY          0x29
#define WAVE1006_REG_HIS_H_MON          0x2A
#define WAVE1006_REG_HIS_H_YEAR         0x2B
/*user ram reg 2cH -71H*/
#define WAVE1006_REG_USER_RAM_START     0x2C
#define WAVE1006_REG_USER_RAM_END       0x71
/*device id 72H-79H*/
#define WAVE1006_REG_DEVICE_ID          0x72

/*communicate check reg*/
#define WAVE1006_REG_CHECK              0xFB
/*write protect reg*/
#define WAVE1006_REG_WP                 0xFC

#define WAVE1006_REG_EXTEND             0xFD
/*sub second*/
#define WAVE1006_REG_SUB_SEC1           0xFE
#define WAVE1006_REG_SUB_SEC2           0xFF

/*SD3068 reg define*/
/*time reg*/
#define SD3068_REG_SEC                0x00
#define SD3068_REG_MIN                0x01
#define SD3068_REG_HOUR               0x02
#define SD3068_REG_WDAY               0x03
#define SD3068_REG_MDAY               0x04
#define SD3068_REG_MON                0x05
#define SD3068_REG_YEAR               0x06
/*alarm reg*/
#define SD3068_REG_ALARM_SEC          0x07
#define SD3068_REG_ALARM_MIN          0x08
#define SD3068_REG_ALARM_HOUR         0x09
#define SD3068_REG_ALARM_WEEK         0x0A
#define SD3068_REG_ALARM_DAY          0x0B
#define SD3068_REG_ALARM_MONTH        0x0C
#define SD3068_REG_ALARM_YEAR         0x0D
#define SD3068_REG_ALARM_OE           0x0E

typedef enum
{
    DEV_SD3068 = 0,
    DEV_SD8825, //RX8025T
    DEV_SD8800, //RX8900
    DEV_SD8810,//RX8010
    DEV_SD8804, //RA8804
    DEV_SD8931,//DS3231
    DEV_SD8939,//DS1339
    DEV_SD8902,//DS1302
    DEV_SD8200,//BQ32000
    DEV_SD8840,//RA4000 
}wave1006_dev_e;

typedef enum 
{
	WAVE1006_NORMAL_MODE = 0x00,
	WAVE1006_TEST_LOGIC  = 0x01,
	WAVE1006_TEST_HOT_BG = 0x02,
	WAVE1006_TEST_OSC    = 0x04,
	WAVE1006_TEST_VTEST  = 0x08,
	WAVE1006_TEST_PAD    = 0x10,
	WAVE1006_TEST_OTP    = 0x20,
	WAVE1006_TEST_ADC    = 0x40,
	WAVE1006_TEST_CONFIG = 0x80 
}wave1006_test_mode_e;

typedef struct 
{
	unsigned char leakage : 1; //1
	unsigned char hosc : 1; //1
	unsigned char cur_res_adj : 2; //2
	unsigned char osc_vref_adj : 2;//2bit
	unsigned char vol_cure_adj : 2;//2bit
	unsigned char osc_res_adjust : 2;//2bit
	unsigned char osc_cap12p_25p : 1;//1bit
	unsigned char osc_fb : 1;//1bit
	unsigned char oscf_bdeep : 1; //1bit
}wave1006_osc_config_t;

/*otp config argument define */
typedef struct 
{
	unsigned char otp_wp_config : 1;
	unsigned char test_wp_config : 1;
	unsigned char device_type_config : 4;
	unsigned char hour_12_config : 1;
	unsigned char week_encode_config : 2;
	unsigned char freq_en_config : 3;
	unsigned char package_type_config : 4;
	unsigned char ae_polarity_config : 1;
	unsigned char week_day_alarm_config : 1;
	unsigned char wada_polarity_config : 1;
	unsigned char week_alarm_config : 1;
	unsigned char alarm_valid_config : 7;
	unsigned char ae_disable_config : 2;
	unsigned char timer_freq_source_config : 2;
	unsigned char id_support_config : 1;
	unsigned char timer_pause_support_config : 1;
	unsigned char timer_reset_support_config;
	unsigned char int_out_sel_config : 3;
	unsigned char int_out_rst_config : 1;
	unsigned char wp_type_config : 1;
	unsigned char time_rst_support_config : 1;
	unsigned char group2_alarm_support_config : 1;
	unsigned char reg_addr_reset_config : 2;
	unsigned char time_update_support_config : 1;
	unsigned char temp_offset_config : 8;
	unsigned char digtal_comp_sel_config : 1;
	unsigned char device_addr_config : 7;
	unsigned char alarm_ext_config : 1;
	unsigned char timer_ext_config : 1;
	unsigned char event_support_config : 1;
	unsigned char pps_support_config : 1;
	unsigned char temp_comp_support_config : 1;
	unsigned char digtal_comp_ext_config : 1;
	unsigned char ram_ext_config : 1;
	unsigned char sub_second_ext_config : 1;
	unsigned char charge_ext_config : 1;
	unsigned char bat_val_ext_config : 1;
	unsigned char rst_out_support_config : 1;
	unsigned char temp_alarm_ext_config : 1;
	unsigned char temp_val_ext_config : 1;
	unsigned char rule_temp_adj_config : 3;
	unsigned char rule_bat_adj_config : 3;
	unsigned char osc_cap_sel_config1 : 2;
	unsigned char adjust_temp_1_3_ppm_config : 1;
	unsigned char en_bat_pulldown_config : 1;
	unsigned short osc_config_config : 13;
	unsigned char spi_freq_en_config : 1;
	unsigned char vol_sel_config : 4;
	unsigned char freq_src_config : 3;
	unsigned char k32_support_config : 1;
	unsigned char fout_open_drain_config : 1;
	unsigned char sout_open_drain_config : 1;
	unsigned char comp_period_config : 2;
	unsigned char rst_time_config : 2;
	unsigned char adjust_25_config : 7;
	unsigned char spi_mode_config : 1;
	unsigned char bus_select_config : 1;
	unsigned char en_charge_config : 1;
	unsigned short osc_start_config : 13;
	unsigned char charge_vol_adj_config : 3;
	unsigned char timer_rst_out_config : 1;
}wave1006_otp_arg_t;

typedef struct  
{
	unsigned char cap_com : 5;//cap common value
	unsigned char cap_in : 3; // cap in value
	unsigned char cap_out : 3; //cap out value
	unsigned char temp_adj : 8; //temperature cpmpension value
	unsigned short temp_val : 9; //temperatue value
}wave1006_temp_arg_t;

/*rtc driver error code define*/
typedef enum 
{
	WAVE1006_EXIT_OK = 0,
	WAVE1006_EXIT_NO_ACK = -1,
	WAVE1006_EXIT_ARG_ERR = -2,
	WAVE1006_EXIT_FAIL = -3,
}wave1006_exit_code_e;
/*rtc flag define*/
typedef enum
{
	WAVE1006_OSF_FLAG = 0,
	WAVE1006_ALARM_TIME_FLAG,	//alarm time flag
	WAVE1006_COUNT_DWON_FLAG, //count down flag
	WAVE1006_BLF_FLAG,      
	WAVE1006_PMF_FLAG,
	WAVE1006_RTCF_FLAG
}wave1006_flag_e;

/*12 or 24 hour define*/
typedef enum
{
	WAVE1006_HOUR_12 = 0,
	WAVE1006_HOUR_24 = 1,
}wave1006_hour_type_e;

/*am or pm define,only use for*/
typedef enum
{
	WAVE1006_AM = 0,
	WAVE1006_PM,
	WAVE1006_AM_PM_NULL = 0 //if set 24 hour
}wave1006_am_pm_e;

/*time struct*/
typedef struct 
{
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char week;
	unsigned char day;
	unsigned char month;
	unsigned char year; 
	wave1006_hour_type_e hour_type;      
	wave1006_am_pm_e am_pm;//if hour_type set as SD3078_HOUR_24, am_pm is invalid   
}wave1006_time_t;

/*time alarm extend struct*/
typedef struct 
{
	unsigned char sec_a;
	unsigned char min_a;
	unsigned char hour_a;
	unsigned char week_a;
	unsigned char day_a;
	unsigned char mon_a;
	unsigned char year_a;

	unsigned char ie_a;/*interrupt enable ,bit0:disable,1:enable, bit1: group2*/
	unsigned char int_period;/*0:int outpute 0,1:int output pulse*/
	unsigned char enable_a;/*alarm enable;bit[6]:year,bit[5]:month,bit[4]:day,bit[3]:week,bit[2]:hour,bit[1]:minute,bit[0]:second*/
}wave1006_time_alarm_t;

/*charge control struct*/
typedef struct 
{
	unsigned char chage_en; /*0:disable,1:enable*/
	unsigned char resistance; /*00:10k;01:5k;10:2k;11:open*/
}wave1006_charge_t;

/*user ram write/read struct*/
typedef struct 
{
	unsigned char st_addr;   
	unsigned char end_addr;  
	unsigned char *buf;   
}wave1006_ram_t;

/*temperature lowest/highest alarm set*/
typedef struct 
{
	unsigned char oe;				/*temperature alarm set,0:disable,1:enable*/
	signed char temp; 
}wave1006_temp_alarm_t;

typedef enum 
{
	PKG_RX8025_RXD8900 = 0,
	PKG_RX8900_3225 = 1,
	PKG_RX8804 = 2,
	PKG_RX8010 = 3,
	PKG_DS3231 = 4,
	PKG_DS1339 = 5,
	PKG_DS1302 = 6,
	PKG_BQ32000 = 7,
	PKG_SD8800 = 8,
	PKG_SD8804 = 9,
	PKG_SD3068 = 10,
	PKG_SD3078A = 11,
	PKG_SD3031 = 12,
	PKG_SD3302 = 13,
	PKG_SD8840 = 14,
}package_type_config_e;

wave1006_exit_code_e wave1006_get_test_flag(void);
wave1006_exit_code_e sd3031_get_test_flag(void);
wave1006_exit_code_e wave1006_write_otp(wave1006_otp_arg_t const *config_arg,  unsigned char const *id, wave1006_temp_arg_t const *temp_arg, uint8_t rtcNumber);
wave1006_exit_code_e drv_wave1006_read_reg(wave1006_dev_e dev, unsigned char reg, unsigned char length, unsigned char *buf);
wave1006_exit_code_e drv_wave1006_write_reg(wave1006_dev_e dev, unsigned char reg, unsigned char length, const unsigned char *buf);
wave1006_exit_code_e get_device_addr(wave1006_dev_e dev, unsigned char *dev_addr);
wave1006_exit_code_e wave1006_enter_test_mode(wave1006_test_mode_e mode);
wave1006_exit_code_e sd3031_enter_test_mode(wave1006_test_mode_e mode);
wave1006_exit_code_e wave1006_write_otp_one(char *str);
wave1006_exit_code_e wave1006_read_otp(void);

uint8_t wave1006_cap_adj(char *str);
uint8_t wave1006_config_otp(char *str);
uint8_t wave1006_temp_comp(void);
uint8_t wave1006_write_cap_to_otp(void);
uint8_t rtc_calculate_cap(uint8_t channel);
#if (VERSION_SELECT == VERSION_SD8804)
uint8_t sd8804_write_otp(uint8_t channel);
#else
uint8_t sd8800_write_otp(uint8_t channel);
#endif

#ifdef __cplusplus
}
#endif

#endif