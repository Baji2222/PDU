/*
 * Copyright Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * Program  : ICMP (Internet Control Message Protocol) of Light-weight Protocol Stack
 * Abstract : 
 * Author   : 
 * $Id: $
 */


#ifndef	__LWPS_ICMP_H__
#define	__LWPS_ICMP_H__

#include "local/config.h"

#if LWPS_ICMP

/*
 * Include Files
 */
#include <lwps/lwps.h>

/*
 * Symbol Definition
 */
typedef	struct icmp_hdr_s {
    uint8  type;       /* Type */
    uint8  cod;
//    uint8  code;       /* reserved keyword in 8051 C language */
    uint16 chksum;     /* Checksum */

    uint16 identifier; /* Id Number */
    uint16 seq_number; /* Sequence */
} icmp_hdr_t;

typedef enum icmp_type_e {
    ICMP_TYPE_ECHOREPLY = 0,            /* Echo Reply                   */
    ICMP_TYPE_DESTUNREACH = 3,          /* Destination Unreachable      */
    ICMP_TYPE_SOURCEQUENCH = 4,         /* Source Quench                */
    ICMP_TYPE_REDIRECT = 5,             /* Redirect (change route)      */
    ICMP_TYPE_ECHO = 8,                 /* Echo Request                 */
    ICMP_TYPE_TIMEEXCEEDED = 11,        /* Time Exceeded                */
    ICMP_TYPE_PARAMETERPROB = 12,       /* Parameter Problem            */
    ICMP_TYPE_TIMESTAMP = 13,           /* Timestamp Request            */
    ICMP_TYPE_TIMESTAMPREPLY = 14,      /* Timestamp Reply              */
    ICMP_TYPE_INFOREQUEST = 15,         /* Information Request          */
    ICMP_TYPE_INFOREPLY = 16,           /* Information Reply            */
    ICMP_TYPE_ADDRESS = 17,             /* Address Mask Request         */
    ICMP_TYPE_ADDRESSREPLY = 18         /* Address Mask Reply           */
} icmp_type_t;

/*
 * Macro Definition
 */

#define DBG_LWPS_ICMP_ENABLE    (0)
#if DBG_LWPS_ICMP_ENABLE
#define DBG_LWPS_ICMP_PRINTF   OSAL_PRINTF
#else  /* else of DBG_LWPS_ICMP_ENABLE */
#define DBG_LWPS_ICMP_PRINTF    
#endif /* end of else of DBG_LWPS_ICMP_ENABLE */

/*
 * Function Declaration
 */
extern int32 icmp_init(void) large;
extern int32 icmp_up(pktBuf_t *pBuf) large;

#else  /* else of LWPS_ICMP */
#define icmp_init()

#endif /* end of else of LWPS_ICMP */

#endif	/* __LWPS_ICMP_H__ */

