#ifndef __MANAGER_H
#define __MANAGER_H

#ifdef __cplusplus
 extern "C" {
#endif
typedef enum {
    FREE_MODE = 0,
    FREQ_CONNECT,       //1
    TEMP_TEST,          //2
    CAP_ADJUST,         //3
    CAP_ADJUST_COVER,   //4
    CALCULATE,          //5
    CAP_DOWNLOAD,       //6
    SYNC_RTC_TIME,      //7
    CHECK_ERROR_TIME,   //8
    MACHINE_TEST,       //9
    MACHINE_COVER,      //10
    MACHINE_WRITE       //11
} SystemTestMode;

typedef enum {
    CAP_ADJUST_DEFAULT = 0,
    CAP_ADJUST_ERROR,
    CAP_ADJUST_SUCCESS
} CapAdjustFlag;

typedef struct {
    uint8_t flag;
    float cap;
    float temp;
    float ppm;
    uint8_t count;
} CapAdjust;

void manager_set_mode(uint8_t mode);
uint8_t manager_get_mode(void);
int system_manage_temp(void);
void system_set_int(uint8_t channel);
void system_manage_test_mode(void);
int check_system_temp(void);

#ifdef __cplusplus
}
#endif

#endif 