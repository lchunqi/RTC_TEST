#ifndef __DEVICES_H
#define __DEVICES_H

#ifdef __cplusplus
 extern "C" {
#endif



void StartDevicesTask(void const * argument);
void operate_to_devices(uint8_t command);

#ifdef __cplusplus
}
#endif

#endif 