#ifndef __TASK_H
#define __TASK_H

#ifdef __cplusplus
 extern "C" {
#endif
typedef struct {
    uint32_t recordTime;
    float freq;
} FrequencyData;

#define FREQ_TEST_DEFAULT 0
#define FREQ_TEST_STARTING  1
void system_manage(void);
void system_set_real_time(RealTime time);
void rtc_freq_test(int mode);
int system_machine_test_one(uint8_t number, uint8_t mode);
int system_machine_write_one(uint8_t number, uint8_t mode);
int command_freq_test(uint8_t number);
void command_cap_list_test(uint8_t init, int startNumber);
void set_int_test(uint8_t state, uint8_t start, uint8_t end);
#ifdef __cplusplus
}
#endif

#endif 