#ifndef __WIFI_H
#define __WIFI_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"


typedef enum {
    WIFI_NOT_CONNECT = 0,
    WIFI_CONNECTING,
    WIFI_BEFOR_TCP,
    WIFI_CONNECTED_CMD,
    WIFI_CONNECTED_FREE,
}WifiConnectState;

void StartWifiTask(void const * argument);
void wifi_heart(void);
void connect_wifi(void);
void wifi_usart_receive(void);
#ifdef __cplusplus
}
#endif

#endif 