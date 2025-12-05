#include "main.h"
#include "flash.h"
#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

uint8_t g_printfBuffer[LOG_MAX_BUFFER] = {0};
uint32_t g_nextPage = 0; 

int log_list_init(void)
{
    uint32_t list_addr = LOG_LIST_ADDR;
    uint8_t buffer[PAGE_SIZE] = {0};
    uint16_t i, j;
    g_nextPage = 0;
    if (flash_read_data(list_addr, buffer, PAGE_SIZE) != 1) {
        UsbPrintf("flash init fail\n");
        return -1;
    }
    memset(buffer, 0, PAGE_SIZE);
    for (i = 0; i < LOG_MAX_PAGE - 1; i += PAGE_SIZE) {
        flash_read_data(list_addr, buffer, PAGE_SIZE);
        for (j = 0; j < PAGE_SIZE; j++) {
            if (buffer[j] == 'w') {
                g_nextPage = i + j + 1;
            }
        }
        list_addr += PAGE_SIZE;
        memset(buffer, 0, PAGE_SIZE);
    }
	UsbPrintf("log use pages %d/%d\n", g_nextPage, LOG_MAX_PAGE);
    return g_nextPage;
}

uint32_t get_log_record_pages(void)
{
    return g_nextPage + 1;
}

void log_list_write(uint32_t pageNumber)
{
    uint8_t buffer = 'w';

    flash_write_data(LOG_LIST_ADDR + pageNumber, &buffer, 1);
}

void log_buffer_write(void)
{
    static uint16_t logLength = 0;
    uint16_t bufferLength = 0;

    bufferLength = strlen((char*)g_printfBuffer);
    if (logLength  + bufferLength >  PAGE_SIZE) {
        if (g_nextPage >= (LOG_MAX_PAGE - 1))  { 
            return;
        }
        g_nextPage++; 
        logLength = 0;
    } 
    flash_write_data(LOG_START_ADDR + (g_nextPage * PAGE_SIZE) + logLength, g_printfBuffer, bufferLength);
    // HAL_Delay(2);//
    log_list_write(g_nextPage);
    logLength += bufferLength;
}

void log_printf(char* fmt,...)
{
	va_list ap;
	memset(g_printfBuffer, 0, LOG_MAX_BUFFER);
	va_start(ap, fmt);//
	vsnprintf((char*)g_printfBuffer, LOG_MAX_BUFFER, fmt, ap);//
	va_end(ap);
    log_buffer_write();
}