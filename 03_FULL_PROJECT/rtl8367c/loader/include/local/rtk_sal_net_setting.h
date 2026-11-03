#ifndef __RTK_SAL_NET_SETTING_H__
#define __RTK_SAL_NET_SETTING_H__

#include <local/config.h>
#include <common_types.h>
#include <api/rtk_error.h>
#include <mem/mem_lib.h>

#define GET_ETHERNET_THIS_MAC(mac)         \
	rtk_sal_get_thismac( (mac) )

#define GET_TX_BUF(len)         \
	malloc_tx_buf( (len) )

#define SET_TRUCK_PORT(group, portmask)

extern u8_t rtk_sal_get_thismac(u8_t *mac_addr);
extern u8_t rtk_sal_wait_trunk_ready(u16_t trunk_id, portmask_type pm);
extern u8_t rtk_sal_set_trunk_member(u16_t trunk_id, portmask_type pm);


#endif  /* end of __RTK_SAL_NET_SETTING_H__ */

