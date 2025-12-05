#ifndef __TEMP_H
#define __TEMP_H

#ifdef __cplusplus
 extern "C" {
#endif

#define TEMP_1_ADDRESS 0X90
#define TEMP_2_ADDRESS 0X92

float temp_read_value(unsigned char channel);
#ifdef __cplusplus
}
#endif

#endif 