#ifndef __RTK_SAL_PORT_ABILITY_H__
#define __RTK_SAL_PORT_ABILITY_H__
#include "local/config.h"
#include "common.h"

/* SNMP dot3StatsDuplexStatus value */
#define PORT_STATUS_DUPLEX_UNKNOWN  (2)
#define PORT_STATUS_DUPLEX_HALF     (2)
#define PORT_STATUS_DUPLEX_FULL     (3)

/* SNMP ifHighSpeed */
#define PORT_STATUS_SPEED_10M       (10UL)
#define PORT_STATUS_SPEED_100M      (100UL)
#define PORT_STATUS_SPEED_1000M     (1000UL)

/* SNMP ifConnectorPresent */
#define PORT_STATUS_LINK_UP         (1)
#define PORT_STATUS_LINK_DOWN       (2)

/* SNMP dot3PauseAdminMode/dot3PauseOperMode */
#define PORT_STATUS_PAUSE_MODE_DISABLE         (1)
#define PORT_STATUS_PAUSE_MODE_XMIT            (2)
#define PORT_STATUS_PAUSE_MODE_RCV             (3)
#define PORT_STATUS_PAUSE_MODE_XMITANDRCV      (4)


extern u8_t get_port_link_status(u8_t if_nummber) large;
extern u32_t get_port_speed(u8_t if_nummber) large;
extern u8_t get_port_duplex(u8_t if_nummber) large;
extern u8_t get_port_txrx_pause(u8_t if_nummber) large;


#define GET_PORT_LINK_STATUS(if_nummber)           \
	get_port_link_status((if_nummber))

#define GET_PORT_SPEED(if_nummber)                 \
	get_port_speed((if_nummber))

#define GET_PORT_DUPLEX(if_nummber)                \
	get_port_duplex((if_nummber))

#define GET_PORT_PAUSE_MODE(if_nummber)                \
	get_port_txrx_pause((if_nummber))

#endif /* end of __RTK_SAL_PORT_ABILITY_H__ */


