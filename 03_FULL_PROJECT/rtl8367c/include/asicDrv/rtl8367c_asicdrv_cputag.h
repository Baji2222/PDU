#ifndef _RTL8367C_ASICDRV_CPUTAG_H_
#define _RTL8367C_ASICDRV_CPUTAG_H_

#include <rtl8367c_asicdrv.h>

enum CPUTAG_INSERT_MODE
{
    CPUTAG_INSERT_TO_ALL = 0,
    CPUTAG_INSERT_TO_TRAPPING,
    CPUTAG_INSERT_TO_NO,
    CPUTAG_INSERT_END
};

extern ret_t rtl8367c_setAsicCputagEnable(uint32 enabled);
extern ret_t rtl8367c_getAsicCputagEnable(uint32 *pEnabled);
extern ret_t rtl8367c_setAsicCputagTrapPort(uint32 port);
extern ret_t rtl8367c_getAsicCputagTrapPort(uint32 *pPort);
extern ret_t rtl8367c_setAsicCputagPortmask(uint32 portmask);
extern ret_t rtl8367c_getAsicCputagPortmask(uint32 *pPmsk);
extern ret_t rtl8367c_setAsicCputagInsertMode(uint32 mode);
extern ret_t rtl8367c_getAsicCputagInsertMode(uint32 *pMode);
extern ret_t rtl8367c_setAsicCputagPriorityRemapping(uint32 srcPri, uint32 newPri);
extern ret_t rtl8367c_getAsicCputagPriorityRemapping(uint32 srcPri, uint32 *pNewPri);
extern ret_t rtl8367c_setAsicCputagPosition(uint32 postion);
extern ret_t rtl8367c_getAsicCputagPosition(uint32* pPostion);
extern ret_t rtl8367c_setAsicCputagMode(uint32 mode);
extern ret_t rtl8367c_getAsicCputagMode(uint32 *pMode);
extern ret_t rtl8367c_setAsicCputagRxMinLength(uint32 mode);
extern ret_t rtl8367c_getAsicCputagRxMinLength(uint32 *pMode);

#endif /*#ifndef _RTL8367C_ASICDRV_CPUTAG_H_*/

