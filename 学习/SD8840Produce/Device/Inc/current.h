#ifndef __CURRENT_H
#define __CURRENT_H

#ifdef __cplusplus
 extern "C" {
#endif

#define CURRENT_VDD_MODE    0
#define CURRENT_VBAT_MODE   1

unsigned char CS1237Init(void);
float CS1237Current(void);
unsigned char CS1237ReadConfig(void);
void CS1237WriteConfig(unsigned char tem);
//void current_set_channel(unsigned char mode, unsigned char number);

#ifdef __cplusplus
}
#endif

#endif 