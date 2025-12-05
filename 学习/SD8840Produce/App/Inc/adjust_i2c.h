#ifndef __ADJUST_I2C_H
#define __ADJUST_I2C_H

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct {
	uint16_t tIN;
	uint16_t tAA;
	uint16_t tBUF;
	uint16_t tLOW;
	uint16_t tHIGH;
	uint16_t tSU_STA;
	uint16_t tHD_STA;
	uint16_t tSU_DAT;
	uint16_t tHD_DAT;
	uint16_t tSU_STO;
	uint16_t tHD_STO;
	uint16_t tRise;
	uint16_t tFall;
}I2cParameter;
#define DELAY_GAIN	18.5f

uint8_t adjust_i2c_read_byte(unsigned char dev_addr,unsigned char reg_addr, unsigned char *buf);
void adjust_i2c_init(void);
#define NOP1()  __NOP()
#define NOP2()  NOP1();NOP1() //2
#define NOP3()  NOP2();NOP1() //3
#define NOP4()  NOP2();NOP2() //4
#define NOP5() 	NOP4();NOP2() //6
#define NOP6()  NOP4();NOP4() //8
#define NOP7()  NOP6();NOP4() //10
#define NOP8()  NOP7();NOP2() //12
#define NOP9()  NOP8();NOP2() //14
#define NOP10() NOP9();NOP2() //16
#ifdef __cplusplus
}
#endif

#endif 