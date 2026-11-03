#ifndef __RUNTIME_LOADER_MSG_H__
#define __RUNTIME_LOADER_MSG_H__

#include <common.h>

#define START_MODE_WARM 0
#define START_MODE_COLD 1

extern void runtime_ip_get(uint8 *pIp, uint8 *pNetmask, uint8 *pGateway);
extern void runtime_ip_set(uint8 *pIp, uint8 *pNetmask, uint8 *pGateway);
extern void runtime_start_mode_get(uint8* start_mode);
extern void loader_start_mode_get(uint8* start_mode);
extern void runtime_start_mode_set(uint8 start_mode);
extern void loader_start_mode_set(uint8 start_mode);

#endif

