#ifndef __RTK_SAL_H__
#define __RTK_SAL_H__

#include <common_types.h>
#include <lwps/pktbuf.h>

#define RTK_SAL_ERR_SUCCESS (0)
#define RTK_SAL_ERR_FAILED  (-1)

extern int32 SalSysGetMac(uint8 *mac);
extern int32 SalGetRxPort(uint8 *port, pktBuf_t *pBuf);

#endif

