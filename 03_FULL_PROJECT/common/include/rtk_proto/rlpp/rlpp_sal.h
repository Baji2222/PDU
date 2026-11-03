#ifndef __RLPP_SAL_H__
#define __RLPP_SAL_H__

#include <common_types.h>
#include <rtk_api/rtk_api.h>
#include <rtk_proto/rlpp/rlpp.h>

typedef enum rlpp_led_e
{
    RLPP_LED_NORMAL = 0,
	RLPP_LED_FORCE
} rlpp_led_t;

extern int32 SalRlppSetPortState(rtk_port_t, rlpp_port_state_t);
extern int32 SalRlppSetLed(rtk_port_t, rtk_enable_t);
extern int32 SalRlppRootRemove();
extern int32 SalRlppQueueFlush(uint8);

#endif

