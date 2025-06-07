#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum {
    I2C_OK      =    0,
    I2C_START_FAIL  ,
    I2C_NO_ACK      ,
}i2c_exit_code_e;

i2c_exit_code_e i2c_write_multi_bytes(unsigned char dev_addr, unsigned char reg_addr, const unsigned char *buf, unsigned short size);
i2c_exit_code_e i2c_read_multi_bytes(unsigned char dev_addr,unsigned char reg_addr, unsigned char *buf, unsigned short size);
i2c_exit_code_e i2c_dev_detect(unsigned char dev_addr);

#ifdef __cplusplus
}
#endif

#endif 