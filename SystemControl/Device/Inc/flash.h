#ifndef __FLASH_H
#define __FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif


#include "main.h"

#define FLASH_USER_START_ADDR       0x08010000UL   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR         FLASH_END   /* End @ of user Flash area */

#define FLASH_UPDATE_STATE_ADDR     0x0800C000UL
#define SYSTEM_USB_UPDATE           0x00000031UL
#define SYSTEM_WIFI_UPDATE          0x00000032UL
#define SYSTEM_RUN_BOOTLOADER       0x000000FFUL

#define FLASH_VERSION_ADDR           0X0800C100UL
#define VERSION_NAME_LENGTH         32
#define FLASH_BSN_ADDR              0X0800C140UL
#define BSN_NAME_LENGTH             16

typedef enum {
    CLEAR_ALL = 0,
    CLEAR_ONLY_BSN,
    CLEAR_ONLY_VERSION,
    CLEAR_ONLY_UPDATE,
} ClearFlagMode;

uint32_t FLASH_ReadWord(uint32_t Addr);
void ClearUpdateStateFlash(void);
void GetStringFromFlash(char* target, uint16_t length, uint32_t addr);
void SaveStringToFlash(char* s, uint32_t addr);
void ClearStateFlash(uint8_t mode);
void SetUpdateStateFlash(uint32_t name);

#ifdef __cplusplus
}
#endif

#endif 
