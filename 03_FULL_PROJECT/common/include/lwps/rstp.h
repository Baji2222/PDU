/*
 * Copyright Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Program  : RSTP
 * Abstract : 
 * Author   : 
 * $Id: $
 */

#ifndef __RSTP_H__
#define __RSTP_H__

/*
 * Include Files
 */
#include <lwps/lwps.h>
#include <common_types.h>

/*
 * Symbol Definition
 */

#define RSTP_DEFAULT_MSTI          (0)

/* BPDU definitions */
#define BPDU_TYPE_TCN              (0x80)
#define BPDU_TYPE_STP              (0x00)
#define BPDU_TYPE_RSTP             (0x02)

#define RSTP_MIGRATE_TIME          (3)
#define RSTP_FSM_GO_MASK           (0x80)     /* on fsm variables, if mask!=0, run this state */
#define RSTP_FORCE_PROTO_VERSION   (2)


/* 17.14 */
#define RSTP_DEFAULT_MIGRATE_TIME  (3)
#define RSTP_DEFAULT_HELLO_TIME    (2)
  #define RSTP_DEFAULT_HELLO_TIME_MIN    (1)
  #define RSTP_DEFAULT_HELLO_TIME_MAX    (2)
#define RSTP_DEFAULT_MAX_AGE       (20)
#define RSTP_DEFAULT_FWD_DELAY     (15)
#define RSTP_DEFAULT_TX_HOLD_COUNT (6)

#define RSTP_TIME_PER_SECOND                (256UL)
#define RSTP_TIME_PER_SECOND_SHIFT          (8)
#define RSTP_TIME_SECOND_MASK               (0xFFFFFF00UL)
#define RSTP_BPDU_DEFAULT_MIGRATE_TIME      (RSTP_DEFAULT_MIGRATE_TIME    <<RSTP_TIME_PER_SECOND_SHIFT)
#define RSTP_BPDU_DEFAULT_HELLO_TIME        (RSTP_DEFAULT_HELLO_TIME      <<RSTP_TIME_PER_SECOND_SHIFT)
  #define RSTP_BPDU_DEFAULT_HELLO_TIME_MIN    (RSTP_DEFAULT_HELLO_TIME_MIN<<RSTP_TIME_PER_SECOND_SHIFT)
  #define RSTP_BPDU_DEFAULT_HELLO_TIME_MAX    (RSTP_DEFAULT_HELLO_TIME_MAX<<RSTP_TIME_PER_SECOND_SHIFT)
#define RSTP_BPDU_DEFAULT_MAX_AGE           (RSTP_DEFAULT_MAX_AGE         <<RSTP_TIME_PER_SECOND_SHIFT)
#define RSTP_BPDU_DEFAULT_FWD_DELAY         (RSTP_DEFAULT_FWD_DELAY       <<RSTP_TIME_PER_SECOND_SHIFT)
#define RSTP_BPDU_DEFAULT_TX_HOLD_COUNT     (RSTP_DEFAULT_TX_HOLD_COUNT   <<RSTP_TIME_PER_SECOND_SHIFT)



#define RSTP_DEFAULT_BDG_PRIORITY   (0x8000)
#define RSTP_DEFAULT_PORT_PRIORITY  (0x80)

/* 17.19, In  st_port_var_t.modes */
#define RSTP_PORT_MODE_AGREE       (1UL<< 0)
#define RSTP_PORT_MODE_AGREED      (1UL<< 1)
#define RSTP_PORT_MODE_DISPUTED    (1UL<< 2)
#define RSTP_PORT_MODE_FORWARD     (1UL<< 3)
#define RSTP_PORT_MODE_FORWARDING  (1UL<< 4)
#define RSTP_PORT_MODE_LEARN       (1UL<< 5)
#define RSTP_PORT_MODE_LEARNING    (1UL<< 6)
#define RSTP_PORT_MODE_MCHECK      (1UL<< 7)
#define RSTP_PORT_MODE_NEWINFO     (1UL<< 8)
#define RSTP_PORT_MODE_OPEREDGE    (1UL<< 9)
#define RSTP_PORT_MODE_PORTENABLED (1UL<<10)
#define RSTP_PORT_MODE_PROPOSED    (1UL<<11)
#define RSTP_PORT_MODE_PROPOSING   (1UL<<12)
#define RSTP_PORT_MODE_RCVDMSG     (1UL<<13)
#define RSTP_PORT_MODE_RCVDRSTP    (1UL<<14)
#define RSTP_PORT_MODE_RCVDSTP     (1UL<<15)
#define RSTP_PORT_MODE_RCVDTC      (1UL<<16)
#define RSTP_PORT_MODE_RCVDTCACK   (1UL<<17)
#define RSTP_PORT_MODE_RCVDTCN     (1UL<<18)
#define RSTP_PORT_MODE_REROOT      (1UL<<19)
#define RSTP_PORT_MODE_RESELECT    (1UL<<20)
#define RSTP_PORT_MODE_SELECTED    (1UL<<21)
#define RSTP_PORT_MODE_SENDRSTP    (1UL<<22)
#define RSTP_PORT_MODE_SYNC        (1UL<<23)
#define RSTP_PORT_MODE_SYNCED      (1UL<<24)
#define RSTP_PORT_MODE_TCACK       (1UL<<25)
#define RSTP_PORT_MODE_TCPROP      (1UL<<26)
#define RSTP_PORT_MODE_TICK        (1UL<<27)  /* set per second */
#define RSTP_PORT_MODE_UPDTINFO    (1UL<<28)
//#define RSTP_PORT_MODE_FDBFLUSH    (1UL<<29)  /* not used */
//#define RSTP_PORT_MODE_RCVDBPDU    (1UL<<30)  /* not used, get configuration, TCN, RST BPDU */

/* 17.13 Performance Parameters + 6.4.3 Point-to-Point MAC parameters */
#define RSTP_ADMIN_P2P_MASK        (3<<0)
    #define RSTP_ADMIN_P2P_AUTO        (0<<0)
    #define RSTP_ADMIN_P2P_FORCETRUE   (1<<0)
    #define RSTP_ADMIN_P2P_FORCEFALSE  (2<<0)
#define RSTP_ADMIN_EDGE_PORT       (1<<2)
#define RSTP_AUTO_EDGE             (1<<3)

#define RSTP_BPDU_FLAG_TC           (1<<0)
#define RSTP_BPDU_FLAG_PROPOSAL     (1<<1)
#define RSTP_BPDU_FLAG_PORTROLE_MSK (3<<2)
    #define RSTP_BPDU_FLAG_PORTROLE_UNKNOW       (0<<2)
    #define RSTP_BPDU_FLAG_PORTROLE_ALTER_BACKUP (1<<2)
    #define RSTP_BPDU_FLAG_PORTROLE_ROOT         (2<<2)
    #define RSTP_BPDU_FLAG_PORTROLE_DESIGNATED   (3<<2)

#define RSTP_BPDU_FLAG_LEARNING     (1<<4)
#define RSTP_BPDU_FLAG_FORWARDING   (1<<5)
#define RSTP_BPDU_FLAG_AGREEMENT    (1<<6)
#define RSTP_BPDU_FLAG_TCACK        (1<<7)

/* Recommended Port Path Cost Value Table 17-3 */
#define RSTP_LINK_SPEED_100KB    (200000000UL)
#define RSTP_LINK_SPEED_1MB      (20000000UL)
#define RSTP_LINK_SPEED_10MB     (2000000UL)
#define RSTP_LINK_SPEED_100MB    (200000UL)
#define RSTP_LINK_SPEED_1GB      (20000UL)
#define RSTP_LINK_SPEED_10GB     (2000UL)
#define RSTP_LINK_SPEED_100GB    (200UL)
#define RSTP_LINK_SPEED_1TB      (20UL)
#define RSTP_LINK_SPEED_10TB     (2UL)


typedef struct st_bpdu_s{
    u8_t  dsap;
    u8_t  ssap;
    u8_t  llc;
    u16_t protocol;
    u8_t  version;
    u8_t  bpdu_type;
}st_bpdu_t;

typedef struct st_bpdu_config_s{
    u16_t protocol;
    u8_t  version;
    u8_t  bpdu_type;
    u8_t  flags;
    u16_t root_id_priority;
    u8_t  root_id_mac[6];
    u32_t root_path_cost;
    u16_t bridge_id_priority;
    u8_t  bridge_id_mac[6];
    u16_t port_id;
    u16_t msg_age;
    u16_t max_age;
    u16_t hello_time;
    u16_t fwd_delay;
}st_bpdu_config_t;

typedef struct st_bpdu_rstp_s{
    u16_t protocol;
    u8_t  version;
    u8_t  bpdu_type;
    u8_t  flags;
    u16_t root_id_priority;
    u8_t  root_id_mac[6];
    u32_t root_path_cost;
    u16_t bridge_id_priority;
    u8_t  bridge_id_mac[6];
    u16_t port_id;
    u16_t msg_age;
    u16_t max_age;
    u16_t hello_time;
    u16_t fwd_delay;
    u8_t  version1Length;
}st_bpdu_rstp_t;

typedef struct st_bpdu_tcn_s{
    u16_t protocol;
    u8_t  version;
    u8_t  bpdu_type;
}st_bpdu_tcn_t;

typedef struct st_fsm_timer_s{
    u8_t  edgeDelayWhile; /* 17.17.1 */
    u8_t  fdWhile;        /* 17.17.2 */
    u8_t  helloWhen;      /* 17.17.3 */
    u8_t  mdelayWhile;    /* 17.17.4 */
    u8_t  rbWhile;        /* 17.17.5 */
    u8_t  rcvdInfoWhile;  /* 17.17.6 */
    u8_t  rrWhile;        /* 17.17.7 */
    u8_t  tcWhile;        /* 17.17.8 */
}st_fsm_timer_t;

typedef struct st_prio_vector_s{
    /* RootBridgeID */
    u16_t root_priority;
    u8_t  root_addr[6];

    /* RootPathCost */
    u32_t rpc;

    /* DesignatedBridgeID */
    u16_t designated_priority;
    u8_t  designated_addr[6];

    /* DesignatedPortID */
    u16_t dpID;

    /* BridgePortID */
    u16_t  bpID;
    
}st_prio_vector_t;

/* 17.7 Port Role Definitions */
typedef enum rstp_port_role_e
{
    RSTP_PORT_ROLE_DISABLE = 0,
    RSTP_PORT_ROLE_ROOT,
    RSTP_PORT_ROLE_DESIGNATED,
    RSTP_PORT_ROLE_ALTERNATE,
    RSTP_PORT_ROLE_BACKUP    
}rstp_port_role_t;

/* 17.19.10 Port's Spanning Tree information */
typedef enum rstp_psti_e
{
    RSTP_PSTI_DISABLED = 0,
    RSTP_PSTI_MINE,
    RSTP_PSTI_AGED,
    RSTP_PSTI_RECEIVED
}rstp_psti_t;

/* 17.23 PortReceive state machine */
typedef enum rstp_prx_fsm_e
{
    RSTP_PRX_FSM_DISCARD = 0,
    RSTP_PRX_FSM_RECEIVE
}rstp_prx_fsm_t;

/* 17.24 PortProtocolMigration state machine */
typedef enum rstp_ppm_fsm_e
{
    RSTP_PPM_FSM_CHECKING_RSTP = 0,
    RSTP_PPM_FSM_SELECTING_STP,
    RSTP_PPM_FSM_SENSING
}rstp_ppm_fsm_t;

/* 17.25 BridgeDetection state machine */
typedef enum rstp_bdm_fsm_e
{
    RSTP_BDM_FSM_EDGE = 0,
    RSTP_BDM_FSM_NOTEDGE
}rstp_bdn_fsm_t;

/* 17.26 PortTransmit state machine */
typedef enum rstp_ptx_fsm_e
{
    RSTP_PTX_FSM_TRANSMIT_INIT = 0,
    RSTP_PTX_FSM_TRANSMIT_CONFIG,
    RSTP_PTX_FSM_TRANSMIT_PERIODIC,
    RSTP_PTX_FSM_TRANSMIT_TCN,
    RSTP_PTX_FSM_TRANSMIT_RSTP,
    RSTP_PTX_FSM_IDLE
}rstp_ptx_fsm_t;

/* 17.27 PortInformation state machine */
typedef enum rstp_pim_fsm_e
{
    RSTP_PIM_FSM_DISABLED = 0,
    RSTP_PIM_FSM_AGED,
    RSTP_PIM_FSM_UPDATE,
    RSTP_PIM_FSM_SUPERIOR_DESIGNATED,
    RSTP_PIM_FSM_REPEATED_DESIGNATED,
    RSTP_PIM_FSM_INFERIOR_DESIGNATED,
    RSTP_PIM_FSM_NOT_DESIGNATED,
    RSTP_PIM_FSM_OTHER,
    RSTP_PIM_FSM_CURRENT,
    RSTP_PIM_FSM_RECEIVE
}rstp_pim_fsm_t;

/* 17.28 PortRoleSelection state machine */
typedef enum rstp_prs_fsm_e
{
    RSTP_PRS_FSM_INIT_BRIDGE = 0,
    RSTP_PRS_FSM_ROLE_SELECTION
}rstp_prs_fsm_t;

/* 17.29 PortRoleTransitions state machine */
typedef enum rstp_prt_fsm_e
{
    /* Disabled Port */
    RSTP_PRT_FSM_INIT_PORT = 0,
    RSTP_PRT_FSM_DISABLE_PORT,
    RSTP_PRT_FSM_DISABLED_PORT,
    /* Root Port */
    RSTP_PRT_FSM_ROOT_PROPOSED,
    RSTP_PRT_FSM_ROOT_AGREED,
    RSTP_PRT_FSM_REROOT,
    RSTP_PRT_FSM_ROOT_FORWARD,
    RSTP_PRT_FSM_ROOT_LEARN,
    RSTP_PRT_FSM_REROOTED,
    RSTP_PRT_FSM_ROOT_PORT,
    /* Designated Port */
    RSTP_PRT_FSM_DESIGNATED_PROPOSE,
    RSTP_PRT_FSM_DESIGNATED_SYNCED,
    RSTP_PRT_FSM_DESIGNATED_RETIRED,
    RSTP_PRT_FSM_DESIGNATED_FORWARD,
    RSTP_PRT_FSM_DESIGNATED_LEARN,
    RSTP_PRT_FSM_DESIGNATED_DISCARD,
    RSTP_PRT_FSM_DESIGNATED_PORT,
    /* Alternate/Backup Port */
    RSTP_PRT_FSM_ALTERNATE_PROPOSED,
    RSTP_PRT_FSM_ALTERNATE_AGREED,
    RSTP_PRT_FSM_BLOCK_PORT,
    RSTP_PRT_FSM_BACKUP_PORT,
    RSTP_PRT_FSM_ALTERNATE_PORT
}rstp_prt_fsm_t;

/* 17.30 PortStateTransition state machine */
typedef enum rstp_pst_fsm_e
{
    RSTP_PST_FSM_DISCARDING = 0,
    RSTP_PST_FSM_LEARNING,
    RSTP_PST_FSM_FORWARDING
}rstp_pst_fsm_t;

/* 17.31 TopologyChange state machine */
typedef enum rstp_tcm_fsm_e
{
    RSTP_TCM_FSM_INACTIVE = 0,
    RSTP_TCM_FSM_LEARNING,
    RSTP_TCM_FSM_DETECTED,
    RSTP_TCM_FSM_NOTIFIED_TCN,
    RSTP_TCM_FSM_NOTIFIED_TC,
    RSTP_TCM_FSM_PROPAGATING,
    RSTP_TCM_FSM_ACKNOWLEDGED,
    RSTP_TCM_FSM_ACTIVE
}rstp_tcm_fsm_t;

/* for 17.19.26 */
typedef enum rstp_rcvd_info_e
{
    SuperiorDesignatedInfo,
    RepeatedDesignatedInfo,
    InferiorDesignatedInfo,
    InferiorRootAlternatedInfo,
    OtherInfo
} rstp_rcvd_info_t;

/* 17.19.4 fileds follow BPDU sequence */
typedef struct rstp_times_s {
  u16_t MessageAge;
  u16_t MaxAge;
  u16_t HelloTime;
  u16_t ForwardDelay;
} rstp_times_t;

typedef struct st_port_var_s{
    u32_t flags;
    u32_t portPathCost;          /* 17.19.20 */
    
    /* state machines */
    u8_t  fsm_prx;               /* 17.23 PortReceive state machine */
    u8_t  fsm_ppm;               /* 17.24 PortProtocolMigration state machine */
    u8_t  fsm_bdm;               /* 17.25 BridgeDetection state machine */
    u8_t  fsm_ptx;               /* 17.26 PortTransmit state machine */
    u8_t  fsm_pim;               /* 17.27 PortInformation state machine */
    u8_t  fsm_prt;               /* 17.29 PortRoleTransitions state machine */
    u8_t  fsm_pst;               /* 17.30 PortStateTransition state machine */
    u8_t  fsm_tcm;               /* 17.31 TopologyChange state machine */

    u8_t  infoIs;                /* 17.19.10 */
    u8_t  role;                  /* 17.19.35 */
    u8_t  selectedRole;          /* 17.19.37 */
    u8_t  txCount;               /* 17.19.44 */
    st_fsm_timer_t port_timers;  /* 17.17 */
    
    st_prio_vector_t designatedPriority;  /* 17.19.4 */
    rstp_times_t     designatedTimes;     /* 17.19.5 */
    st_prio_vector_t msgPriority;         /* 17.19.14 */
    rstp_times_t     msgTimes;            /* 17.19.15 */
    st_prio_vector_t portPriority;        /* 17.19.21 */
    rstp_times_t     portTimes;           /* 17.19.22 */

    /* 17.19.26 */
    u8_t  rcvdInfo;
}st_port_var_t;


#define ST_BRIDGE_VAR_BEGIN    (1<<0)
typedef struct st_bridge_var_s{
    u8_t  mode;                           /* 17.18.1 */
    u8_t  fsm_prs;                        /* 17.28 PortRoleSelection state machine, global */

    rstp_times_t     rootTimes;           /* 17.18.7 */
    rstp_times_t     bridgeTimes;         /* 17.18.4 */

    st_prio_vector_t rootPriority;        /* 17.18.6 */
    st_prio_vector_t bridgePriority;      /* 17.18.3 */

}st_bridge_var_t;

#define RSTP_ADMIN_DEFAULT_RPC   (0)
#define RSTP_ADMIN_RSTP_ENABLE   (1<<0)
typedef struct st_admin_s{
    u32_t admin_rpc[COMBO_PORT_NUM];
    u16_t MaxAge;
    u16_t HelloTime;
    u16_t ForwardDelay;
    
        /* 17.13 */
    u16_t bdg_priority;
    u8_t  setting;
    u8_t  port_priority[COMBO_PORT_NUM];

    u8_t  perf_parameter[COMBO_PORT_NUM];
    
    u8_t  force_version;
    u8_t  rstpVersion;
}st_admin_t;



typedef enum rstp_desc_target_e
{
    /* Set descriptor */
    RSTP_SET_DESC_PORT_START = 0,
    RSTP_SET_DESC_PORT_PRIORITY,
    RSTP_SET_DESC_PORT_MCHECK,
    RSTP_SET_DESC_PORT_ADMIN_RPC,    /* RPC: root path cost */
    RSTP_SET_DESC_PORT_P2P,
    RSTP_SET_DESC_PORT_ADMIN_EDGE,
    RSTP_SET_DESC_PORT_END,
    RSTP_SET_DESC_BRIDGE_START,
    RSTP_SET_DESC_BRIDGE_PRIORITY,   /* 0x08 */
    RSTP_SET_DESC_MAX_AGE,
    RSTP_SET_DESC_HELLO_TIME,
    RSTP_SET_DESC_FWD_DELAY,
    RSTP_SET_DESC_FORCE_VER,
    RSTP_SET_DESC_FUNC_ENABLE,
    RSTP_SET_DESC_BRIDGE_END,
    
    /* Get descriptor */
    RSTP_GET_DESC_PORT_START,
    RSTP_GET_DESC_PORT_PRIORITY,     /* 0x10 */
    RSTP_GET_DESC_PORT_PATH_COST,
    RSTP_GET_DESC_PORT_P2P,
    RSTP_GET_DESC_PORT_EDGE,
    RSTP_GET_DESC_PORT_ROLE,
    RSTP_GET_DESC_PORT_REAL_PATH_COST,
    RSTP_GET_DESC_PORT_REAL_P2P,
    RSTP_GET_DESC_PORT_REAL_EDGE,
    RSTP_GET_DESC_PORT_END,          /* 0x18 */
    RSTP_GET_DESC_FUNC_ENABLE,
    RSTP_GET_DESC_FORCE_VER,
    RSTP_GET_DESC_BRIDGE_PRIORITY,
    RSTP_GET_DESC_MAX_AGE,
    RSTP_GET_DESC_HELLO_TIME,
    RSTP_GET_DESC_FWD_DELAY,
    RSTP_GET_DESC_THIS_IS_ROOT,
    RSTP_GET_DESC_ROOT_PORT,         /* 0x20 */
    RSTP_GET_DESC_ROOT_MAX_AGE,
    RSTP_GET_DESC_ROOT_HELLO_TIME,
    RSTP_GET_DESC_ROOT_FWD_DELAY,
    RSTP_GET_DESC_ROOT_PRIORITY,
    RSTP_GET_DESC_ROOT_PATH_COST,
    RSTP_GET_DESC_ROOT_MAC,
    RSTP_GET_DESC_BRIDGE_END
}rstp_desc_target_t;

typedef struct st_desc_s{
    u32_t value;
    u8_t  mac[6];
    u8_t  target;
    u8_t  pn;
}st_desc_t;

/*
 * Macro Definition
 */
#define DBG_LWPS_RSTP_ENABLE    (0)
#if DBG_LWPS_RSTP_ENABLE
    #define DBG_LWPS_RSTP_PRINTF(x)            do { OSAL_PRINTF x;}while(0)
    #define DBG_LWPS_RSTP_PRX_PRINTF(x)        //do { OSAL_PRINTF x;}while(0)
    #define DBG_LWPS_RSTP_PPM_PRINTF(x)        //do { OSAL_PRINTF x;}while(0)
    #define DBG_LWPS_RSTP_BDM_PRINTF(x)        //do { OSAL_PRINTF x;}while(0)
    #define DBG_LWPS_RSTP_PTX_PRINTF(x)        //do { OSAL_PRINTF x;}while(0)
    #define DBG_LWPS_RSTP_PIM_PRINTF(x)        //do { OSAL_PRINTF x;}while(0)
    #define DBG_LWPS_RSTP_PRS_PRINTF(x)        //do { OSAL_PRINTF x;}while(0)
    #define DBG_LWPS_RSTP_PRT_PRINTF(x)        //do { OSAL_PRINTF x;}while(0)
    #define DBG_LWPS_RSTP_PST_PRINTF(x)        //do { OSAL_PRINTF x;}while(0)
    #define DBG_LWPS_RSTP_TCM_PRINTF(x)        //do { OSAL_PRINTF x;}while(0)
#else  /* else of DBG_LWPS_RSTP_ENABLE */
    #define DBG_LWPS_RSTP_PRINTF(x)           //
    #define DBG_LWPS_RSTP_PRX_PRINTF(x)       //
    #define DBG_LWPS_RSTP_PPM_PRINTF(x)       //
    #define DBG_LWPS_RSTP_BDM_PRINTF(x)       //
    #define DBG_LWPS_RSTP_PTX_PRINTF(x)       //
    #define DBG_LWPS_RSTP_PIM_PRINTF(x)       //
    #define DBG_LWPS_RSTP_PRS_PRINTF(x)       //
    #define DBG_LWPS_RSTP_PRT_PRINTF(x)       //
    #define DBG_LWPS_RSTP_PST_PRINTF(x)       //
    #define DBG_LWPS_RSTP_TCM_PRINTF(x)       //
    #define rstp_hdr_dump(x)                  //
#endif /* end of else of DBG_LWPS_RSTP_ENABLE */

#define RSTP_DEC(x) do{if((x)!=0) (x)=(x)-1;}while(0)

#define RSTP_SET_FLAG(x, setlist)  \
    do{ (x) = (setlist); }while(0)

#define RSTP_SET_FLAG_ENABLE(x, setlist)  \
    do{ (x) |= (setlist); }while(0)
        
#define RSTP_SET_FLAG_DISABLE(x, setlist)  \
    do{ (x) &= ~(setlist); }while(0)

#define RSTP_FLAG_CHECK_ENABLE(x, checklist) \
    ((checklist)==((checklist)&(x)))

#define RSTP_FLAG_CHECK_DISABLE(x, checklist) \
    (0==((checklist)&(x)))

#define RSTP_FLAG_CHECK_ENABLE_ANY(x, checklist) \
    ((checklist)&(x))

#define RSTP_STATE_CHECK(x, state) \
    ((state)==((~RSTP_FSM_GO_MASK)&(x)))

#define RSTP_BPDU_PORTROLE_CHECK(x, state) \
    ((state)==((RSTP_BPDU_FLAG_PORTROLE_MSK)&(x)))

#define RSTP_TIME_ROUND_WHOLE_SECOND(x, retval)          \
do{                                                      \
    if( ((x)&0x80) )                                     \
    {                                                    \
        (retval) = (((x)&0xFF00)+RSTP_TIME_PER_SECOND);  \
    }                                                    \
    else                                                 \
    {                                                    \
        (retval) = ((x)&0xFF00);                         \
    }                                                    \
}while(0)

#define RSTP_COPY_PRIORITY_NO_BPID(dst, src) memcpy(dst, src, sizeof(st_prio_vector_t)-sizeof(u16_t))
#define RSTP_COPY_PRIORITY_ALL(dst, src) memcpy(dst, src, sizeof(st_prio_vector_t))
#define RSTP_COPY_TIMES_ALL(dst, src) memcpy(dst, src, sizeof(rstp_times_t))

/*
 * Function Declaration
 */
#if LWPS_RSTP
extern void  rstp_init(void);
extern void  rstp_tick(void);
extern void  rstp_link_change_handler(portmask_type new_pm);
extern u16_t rstp_up(pktBuf_t *pBuf);
extern u16_t rstp_set(st_desc_t *desc);
extern u16_t rstp_get(st_desc_t *desc);
extern void  rstp_print_port(u8_t pn); /* for debug */
extern void rstp_print_bdg(void); /* for debug */


/* state machines */
extern void rstp_port_timer_fsm(void);
extern void rstp_port_receive_fsm(u8_t pn, pktBuf_t *pbuf);
extern void rstp_port_ppm_fsm(u8_t pn);
extern void rstp_bridge_detect_fsm(u8_t pn);
extern void rstp_port_transmit_fsm(u8_t pn);
extern void rstp_port_info_fsm(u8_t pn, pktBuf_t *pbuf);
extern void rstp_port_role_selection_fsm(void);
extern void rstp_port_role_trans_disable_fsm(u8_t pn);
extern void rstp_port_role_trans_root_fsm(u8_t pn);
extern void rstp_port_role_trans_designated_fsm(u8_t pn);
extern void rstp_port_role_trans_alterBackup_fsm(u8_t pn);
extern void rstp_port_state_transition_fsm(u8_t pn);
extern void rstp_topology_change_fsm(u8_t pn);

#endif /* end of LWPS_RSTP */


#endif /* end of __RSTP_H__ */

