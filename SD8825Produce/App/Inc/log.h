#ifndef __LOG_H
#define __LOG_H

#ifdef __cplusplus
 extern "C" {
#endif

#define LOG_LIST_ADDR       0x010000                 //the second block

int log_list_init(void);
void log_list_write(uint32_t pageNumber);
uint32_t get_log_record_pages(void);
#ifdef __cplusplus
}
#endif

#endif 