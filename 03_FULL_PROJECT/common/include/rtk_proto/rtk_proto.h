#ifndef __RTK_PROTO_H__
#define __RTK_PROTO_H__

#include <common_types.h>
#include <lwps/lwps.h>
//#include <lwps/pktbuf.h>
//#include <local/lwps_config.h>

#ifndef RTK_PROTO_ERR_OK
#define RTK_PROTO_ERR_OK      0
#endif

#ifndef RTK_PROTO_ERR_FAILED
#define RTK_PROTO_ERR_FAILED  (-1)
#endif

#ifndef MAC_ADDR_LEN
#define MAC_ADDR_LEN          (6)
#endif

#ifndef FCS_LEN
#define FCS_LEN          		(4)
#endif

typedef enum rtk_proto_id_e
{
    RTK_PROTO_ID_RRCPV1 = 1,
	RTK_PROTO_ID_CPUTAG = 4,
	RTK_PROTO_ID_RRCPV3 = 0x22,
	RTK_PROTO_ID_RLPP   = 0x25
} rtk_proto_id_t;

#if RTK_PROTO_RRCPV1
extern int32 rrcpv1_up(pktBuf_t *pBuf);
#endif

#if RTK_PROTO_RLPP
extern int32 rlpp_up(pktBuf_t *pBuf);
#endif

//#if RTK_PROTO_RRCPV3R
extern int32 rrcpv3r_up(pktBuf_t *pBuf);
//#endif


#endif

