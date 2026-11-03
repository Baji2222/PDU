/*
 * Copyright Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * Program  : IGMP snooping program
 * Abstract : 
 * Author   : 
 * $Id: $
 */

#ifndef __IGMP_SNOOPING_H__
#define __IGMP_SNOOPING_H__

/*
 * Include Files
 */
#include <local/config.h>

#if LWPS_IGMP
#include <lwps/lwps.h>
#include <lwps/igmp_group_db.h>
#include <common_types.h>

/*
 * Symbol Definition
 */

typedef struct igmp_hdr_s {
    uint8  type;                   /* type*/
    uint8  maxRespTime;            /*maximum response time,unit:0.1 seconds*/
    uint16 checksum;                   
    ip_addr_t groupAddr;
} igmp_hdr_t;

typedef struct igmp_mld_hdr_s {
    uint8  type;            /* type*/
    uint8  cod;           
    uint16 checksum;
    uint16 maxRespDelay;   /* maximum response delay */
    uint16 reserved;      
    ipv6_addr_t groupAddr;
} igmp_mld_hdr_t;

typedef struct igmpv3Query_s {
    uint8 type;                 /*query type*/
    uint8 maxRespCode;          /*maximum response code*/
    uint16 checksum;            /*IGMP checksum*/
    uint32 groupAddr;           /*multicast group address*/
    uint8 rsq;                  /* 4bit: reserved, 1bit: suppress router-side processing, 3bit: querier's robustness variable*/
    uint8 qqic;                 /* querier's query interval code */
    uint16 numOfSrc;            /* number of sources */
    ip_addr_t srcList;          /* first entry of source list */    
}igmpv3Query_t;

typedef enum igmp_group_type_e {
    MODE_IS_INCLUDE         = 1,
    MODE_IS_EXCLUDE         = 2,
    CHANGE_TO_INCLUDE_MODE  = 3,
    CHANGE_TO_EXCLUDE_MODE  = 4,
    ALLOW_NEW_SOURCES       = 5,
    BLOCK_OLD_SOURCES       = 6
} igmp_group_type_t;

typedef struct groupRecord_s
{
    uint8   type;               /* Record Type */
    uint8   auxLen;             /* auxiliary data length, in uints of 32 bit words*/
    uint16  numOfSrc;           /* number of sources */
    ip_addr_t  groupAddr;       /* group address being reported */
//    ip_addr_t  srcList;         /* first entry of source list */
    
}groupRecord_t;

typedef struct igmpv3Report_s {
    uint8  type;                /* Report Type */
    uint8  reserved1;             
    uint16 checksum;            /* IGMP checksum*/
    uint16 reserved2;
    uint16 numOfRecords;        /* number of Group records */
    groupRecord_t recordList;   /* first entry of group record list*/
}igmpv3Report_t;

typedef enum igmp_type_e {
    IGMP_TYPE_MEMBERSHIP_QUERY     = 0x11,  /* Source Quench                */
    IGMPV1_TYPE_MEMBERSHIP_REPORT  = 0x12,  /* Host Membership Report       */
    IGMPV2_TYPE_MEMBERSHIP_REPORT  = 0x16,  /* Redirect (change route)      */
    IGMPV2_TYPE_MEMBERSHIP_LEAVE   = 0x17,  /*                              */
    IGMPV3_TYPE_MEMBERSHIP_REPORT  = 0x22   /*                              */
} igmp_type_t;

typedef enum mld_type_e {
    MLDV1_TYPE_MEMBERSHIP_QUERY   = 130,  /* Source Quench                */
    MLDV1_TYPE_MEMBERSHIP_REPORT  = 131,  /* Host Membership Report       */
    MLDV1_TYPE_MEMBERSHIP_DONE    = 132   /* Leave     */
} mld_type_t;

/*
 * Macro Definition
 */
#define DBG_LWPS_IGMP_ENABLE    (0)
#if DBG_LWPS_IGMP_ENABLE
#define DBG_LWPS_IGMP_PRINTF   OSAL_PRINTF
#else  /* else of DBG_LWPS_IGMP_ENABLE */
#define DBG_LWPS_IGMP_PRINTF
#define igmp_hdr_dump(x)
#endif /* end of else of DBG_LWPS_IGMP_ENABLE */

#define DBG_LWPS_MLD_ENABLE    (0)
#if DBG_LWPS_MLD_ENABLE
#define DBG_LWPS_MLD_PRINTF   OSAL_PRINTF
#else  /* else of DBG_LWPS_MLD_ENABLE */
#define DBG_LWPS_MLD_PRINTF
#define mld_hdr_dump(x)
#endif /* end of else of DBG_LWPS_MLD_ENABLE */


/*
 * Data Declaration
 */

/*
 * Function Declaration
 */
extern int32 igmp_init(void) large;
extern int32 igmp_up(pktBuf_t *pBuf) large;
extern int32 mld_up(pktBuf_t *pBuf) large;
//extern void print_igmp_all( igmp_print_type_t type) large;

#else  /* else of LWPS_IGMP */

#define igmp_init()

#endif /* end of else of LWPS_IGMP */

#endif /* end of __IGMP_SNOOPING_H__ */

