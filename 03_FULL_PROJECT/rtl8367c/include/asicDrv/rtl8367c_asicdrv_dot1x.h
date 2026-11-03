#ifndef _RTL8367C_ASICDRV_DOT1X_H_
#define _RTL8367C_ASICDRV_DOT1X_H_

#include <rtl8367c_asicdrv.h>

enum DOT1X_UNAUTH_BEHAV
{
    DOT1X_UNAUTH_DROP = 0,
    DOT1X_UNAUTH_TRAP,
    DOT1X_UNAUTH_GVLAN,
    DOT1X_UNAUTH_END
};

extern ret_t rtl8367c_setAsic1xPBEnConfig(uint32 port, uint32 enabled);
extern ret_t rtl8367c_getAsic1xPBEnConfig(uint32 port, uint32 *pEnabled);
extern ret_t rtl8367c_setAsic1xPBAuthConfig(uint32 port, uint32 auth);
extern ret_t rtl8367c_getAsic1xPBAuthConfig(uint32 port, uint32 *pAuth);
extern ret_t rtl8367c_setAsic1xPBOpdirConfig(uint32 port, uint32 opdir);
extern ret_t rtl8367c_getAsic1xPBOpdirConfig(uint32 port, uint32 *pOpdir);
extern ret_t rtl8367c_setAsic1xMBEnConfig(uint32 port, uint32 enabled);
extern ret_t rtl8367c_getAsic1xMBEnConfig(uint32 port, uint32 *pEnabled);
extern ret_t rtl8367c_setAsic1xMBOpdirConfig(uint32 opdir);
extern ret_t rtl8367c_getAsic1xMBOpdirConfig(uint32 *pOpdir);
extern ret_t rtl8367c_setAsic1xProcConfig(uint32 port, uint32 proc);
extern ret_t rtl8367c_getAsic1xProcConfig(uint32 port, uint32 *pProc);
extern ret_t rtl8367c_setAsic1xGuestVidx(uint32 index);
extern ret_t rtl8367c_getAsic1xGuestVidx(uint32 *pIndex);
extern ret_t rtl8367c_setAsic1xGVOpdir(uint32 enabled);
extern ret_t rtl8367c_getAsic1xGVOpdir(uint32 *pEnabled);
extern ret_t rtl8367c_setAsic1xTrapPriority(uint32 priority);
extern ret_t rtl8367c_getAsic1xTrapPriority(uint32 *pPriority);


#endif /*_RTL8367C_ASICDRV_DOT1X_H_*/

