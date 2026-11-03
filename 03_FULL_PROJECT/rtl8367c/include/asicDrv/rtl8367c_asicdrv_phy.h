#ifndef _RTL8367C_ASICDRV_PHY_H_
#define _RTL8367C_ASICDRV_PHY_H_

#include <rtl8367c_asicdrv.h>

#define RTL8367C_PHY_REGNOMAX           0x1F
#define RTL8367C_PHY_EXTERNALMAX        0x7

#define RTL8367C_PHY_BASE               0x2000
#define RTL8367C_PHY_EXT_BASE           0xA000

#define RTL8367C_PHY_OFFSET             5
#define RTL8367C_PHY_EXT_OFFSET         9

#define RTL8367C_PHY_PAGE_ADDRESS       31


extern ret_t rtl8367c_setAsicPHYReg(uint32 phyNo, uint32 phyAddr, uint32 regData );
extern ret_t rtl8367c_getAsicPHYReg(uint32 phyNo, uint32 phyAddr, uint32* pRegData );
extern ret_t rtl8367c_setAsicPHYOCPReg(uint32 phyNo, uint32 ocpAddr, uint32 ocpData );
extern ret_t rtl8367c_getAsicPHYOCPReg(uint32 phyNo, uint32 ocpAddr, uint32 *pRegData );
extern ret_t rtl8367c_setAsicSdsReg(uint32 sdsId, uint32 sdsReg, uint32 sdsPage, uint32 value);
extern ret_t rtl8367c_getAsicSdsReg(uint32 sdsId, uint32 sdsReg, uint32 sdsPage, uint32 *value);

#endif /*#ifndef _RTL8367C_ASICDRV_PHY_H_*/

