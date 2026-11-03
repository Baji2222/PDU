#ifndef __RLPP_H_
#define __RLPP_H_

#include <rtk_proto/rtk_proto.h>
#include <common_types.h>
#include <lwps/lwps.h>

#define RLPP_BCAST_PORT_MSK   ((1UL<< SYS_MAX_PHY_PORT) - 1)

#define RLPP_SWPRI_LEN   (3)

#define port_is_enabled(port, pmsk) ((1UL << port) & pmsk)

#define RLPP_PKT_LEN     (60)

typedef enum rlpp_port_state_e
{
    RLPP_DISABLE_STATE = 0,
    RLPP_BLOCKING_STATE,
    RLPP_LISTENING_STATE,
    RLPP_FORWARDING_STATE
} rlpp_port_state_t;

typedef struct rlpp_switch_id_s
{
    uint8      swpri[RLPP_SWPRI_LEN];
    uint8      swmac[MAC_ADDR_LEN];
} rlpp_switch_id_t;

typedef struct rlpp_port_vector_s
{
    rlpp_switch_id_t rid;
	uint16           hopcount;
	rlpp_switch_id_t sid;
	uint8 swppri;
	uint8 swport;    	
} rlpp_port_vector_t;

typedef struct rlpp_port_var_s{
    rlpp_port_vector_t port_vector;
    uint8 slen;
    uint8 selfloop;
    uint8 ppri;
    uint8 hc;
    uint8 pv_ag_enable;
    uint8 pv_ag;
    uint8 pv_sl_enable;
    uint8 sl_ag;
    uint8 port_state;
} rlpp_port_var_t;

typedef struct rlpp_payload_s
{
    uint8              proto_id;
    rlpp_port_vector_t pkt_port_vector;
} rlpp_payload_t;

extern int32 rlpp_up(pktBuf_t *pBuf);
extern int32 rlpp_init();
extern int32 rlpp_tick();
extern int32 rlpp_setState(uint8 enable);
extern int32 rlpp_getPortState(uint8 port, rlpp_port_state_t *state);
extern int32 rlpp_portVar_init(uint8 port);
extern void rlpp_interval_time_set(uint16 time);
extern void rlpp_interval_time_get(uint16 *pTime);
extern void rlpp_recover_time_set(uint32 time);
extern void rlpp_recover_time_get(uint32 *pTime);
extern void rlpp_port_enable_get(uint16 ucPortId, uint16 *pEnable);
extern void rlpp_port_enable_set(uint16 logicPort, uint16 enable);
extern void rlpp_get_portState(uint16 ucPortId, uint16 *pState);
extern void RLPP_link_change_handler(portmask_type currUpPmsk, portmask_type prevUpPmsk);

#define DBG_RLPP_ENABLE    (0)
#if DBG_RLPP_ENABLE
  #define RLPP_PRINT(x)   do { OSAL_PRINTF x;}while(0)
#else /* else of DBG_LWPS_IP_ENABLE */
  #define RLPP_PRINT(x)
//  #define dumpPortVector(x)
#endif /* end of else of DBG_LWPS_IP_ENABLE */

#endif

