#ifndef __TEMP_H__
#define __TEMP_H__

#ifdef __cplusplus
 extern "C" {
#endif
#include "main.h"

#define TEMP_1_ADDRESS 0X90
#define TEMP_2_ADDRESS 0X92

typedef enum {
    _I2C_OK      =    0,
    _I2C_START_FAIL   ,
    _I2C_NO_ACK       ,
    _I2C_Slave_FAIL   ,

}i2c_exit_code_e;

i2c_exit_code_e i2c_write_multi_bytes(unsigned char dev_addr, unsigned char reg_addr, const unsigned char *buf, unsigned short size);
i2c_exit_code_e i2c_read_multi_bytes(unsigned char dev_addr,unsigned char reg_addr, unsigned char *buf, unsigned short size);
//i2c_exit_code_e i2c_dev_detect(unsigned char dev_addr);

void  TEMP_GPIO_Init(void);

float  temp_read_value(unsigned char channel);



#ifdef __cplusplus
}
#endif

#endif 