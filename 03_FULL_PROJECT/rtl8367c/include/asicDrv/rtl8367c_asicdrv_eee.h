#ifndef _RTL8367C_ASICDRV_EEE_H_
#define _RTL8367C_ASICDRV_EEE_H_

#include <rtl8367c_asicdrv.h>

#define EEE_OCP_PHY_ADDR    (0xA5D0)

extern ret_t rtl8367c_setAsicEee100M(uint32 port, uint32 enable);
extern ret_t rtl8367c_getAsicEee100M(uint32 port, uint32 *enable);
extern ret_t rtl8367c_setAsicEeeGiga(uint32 port, uint32 enable);
extern ret_t rtl8367c_getAsicEeeGiga(uint32 port, uint32 *enable);


#endif /*_RTL8367C_ASICDRV_EEE_H_*/
