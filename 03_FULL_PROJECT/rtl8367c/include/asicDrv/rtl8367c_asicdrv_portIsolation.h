#ifndef _RTL8367C_ASICDRV_PORTISOLATION_H_
#define _RTL8367C_ASICDRV_PORTISOLATION_H_

#include <rtl8367c_asicdrv.h>

extern ret_t rtl8367c_setAsicPortIsolationPermittedPortmask(uint32 port, uint32 permitPortmask);
extern ret_t rtl8367c_getAsicPortIsolationPermittedPortmask(uint32 port, uint32 *pPermitPortmask);
extern ret_t rtl8367c_setAsicPortIsolationEfid(uint32 port, uint32 efid);
extern ret_t rtl8367c_getAsicPortIsolationEfid(uint32 port, uint32 *pEfid);

#endif /*_RTL8367C_ASICDRV_PORTISOLATION_H_*/
