#ifndef _RTL8367C_ASICDRV_LUT_H_
#define _RTL8367C_ASICDRV_LUT_H_

#include <rtl8367c_asicdrv.h>

#define RTL8367C_LUT_AGETIMERMAX        (7)
#define RTL8367C_LUT_AGESPEEDMAX        (3)
#define RTL8367C_LUT_LEARNLIMITMAX      (0x1040)
#define RTL8367C_LUT_ADDRMAX            (0x103F)
#define RTL8367C_LUT_IPMCGRP_TABLE_MAX  (0x3F)
#define RTL8367C_LUT_ENTRY_SIZE         (6)
#define RTL8367C_LUT_BUSY_CHECK_NO      (10)

#define RTL8367C_LUT_TABLE_SIZE         (6)

enum RTL8367C_LUTHASHMETHOD{

    LUTHASHMETHOD_SVL=0,
    LUTHASHMETHOD_IVL,
    LUTHASHMETHOD_END,
};


enum RTL8367C_LRNOVERACT{

    LRNOVERACT_FORWARD=0,
    LRNOVERACT_DROP,
    LRNOVERACT_TRAP,
    LRNOVERACT_END,
};

enum RTL8367C_LUTREADMETHOD{

    LUTREADMETHOD_MAC =0,
    LUTREADMETHOD_ADDRESS,
    LUTREADMETHOD_NEXT_ADDRESS,
    LUTREADMETHOD_NEXT_L2UC,
    LUTREADMETHOD_NEXT_L2MC,
    LUTREADMETHOD_NEXT_L3MC,
    LUTREADMETHOD_NEXT_L2L3MC,
    LUTREADMETHOD_NEXT_L2UCSPA,
};

enum RTL8367C_FLUSHMODE
{
    FLUSHMDOE_PORT = 0,
    FLUSHMDOE_VID,
    FLUSHMDOE_FID,
    FLUSHMDOE_END,
};

enum RTL8367C_FLUSHTYPE
{
    FLUSHTYPE_DYNAMIC = 0,
    FLUSHTYPE_BOTH,
    FLUSHTYPE_END,
};


typedef struct LUTTABLE{

    ipaddr_t sip;
    ipaddr_t dip;
    ether_addr_t mac;
    uint16 ivl_svl:1;
    uint16 cvid_fid:12;
    uint16 fid:4;
    uint16 efid:3;

    uint16 nosalearn:1;
    uint16 da_block:1;
    uint16 sa_block:1;
    uint16 auth:1;
    uint16 lut_pri:3;
    uint16 sa_en:1;
    uint16 fwd_en:1;
    uint16 mbr:11;
    uint16 spa:4;
    uint16 age:3;
    uint16 l3lookup:1;
    uint16 igmp_asic:1;
    uint16 igmpidx:8;

    uint16 lookup_hit:1;
    uint16 lookup_busy:1;
    uint16 address:13;

    uint16 l3vidlookup:1;
    uint16 l3_vid:12;

    uint16 wait_time;

}rtl8367c_luttb;

extern ret_t rtl8367c_setAsicLutIpMulticastLookup(uint32 enabled);
extern ret_t rtl8367c_getAsicLutIpMulticastLookup(uint32* pEnabled);
extern ret_t rtl8367c_setAsicLutIpMulticastVidLookup(uint32 enabled);
extern ret_t rtl8367c_getAsicLutIpMulticastVidLookup(uint32* pEnabled);
extern ret_t rtl8367c_setAsicLutAgeTimerSpeed(uint32 timer, uint32 speed);
extern ret_t rtl8367c_getAsicLutAgeTimerSpeed(uint32* pTimer, uint32* pSpeed);
extern ret_t rtl8367c_setAsicLutCamTbUsage(uint32 enabled);
extern ret_t rtl8367c_getAsicLutCamTbUsage(uint32* pEnabled);
extern ret_t rtl8367c_getAsicLutCamType(uint32* pType);
extern ret_t rtl8367c_setAsicLutLearnLimitNo(uint32 port, uint32 number);
extern ret_t rtl8367c_getAsicLutLearnLimitNo(uint32 port, uint32* pNumber);
extern ret_t rtl8367c_setAsicSystemLutLearnLimitNo(uint32 number);
extern ret_t rtl8367c_getAsicSystemLutLearnLimitNo(uint32 *pNumber);
extern ret_t rtl8367c_setAsicLutLearnOverAct(uint32 action);
extern ret_t rtl8367c_getAsicLutLearnOverAct(uint32* pAction);
extern ret_t rtl8367c_setAsicSystemLutLearnOverAct(uint32 action);
extern ret_t rtl8367c_getAsicSystemLutLearnOverAct(uint32 *pAction);
extern ret_t rtl8367c_setAsicSystemLutLearnPortMask(uint32 portmask);
extern ret_t rtl8367c_getAsicSystemLutLearnPortMask(uint32 *pPortmask);
extern ret_t rtl8367c_setAsicL2LookupTb(rtl8367c_luttb *pL2Table);
extern ret_t rtl8367c_getAsicL2LookupTb(uint32 method, rtl8367c_luttb *pL2Table);
extern ret_t rtl8367c_getAsicLutLearnNo(uint32 port, uint32* pNumber);
extern ret_t rtl8367c_setAsicLutIpLookupMethod(uint32 type);
extern ret_t rtl8367c_getAsicLutIpLookupMethod(uint32* pType);
extern ret_t rtl8367c_setAsicLutForceFlush(uint32 portmask);
extern ret_t rtl8367c_getAsicLutForceFlushStatus(uint32 *pPortmask);
extern ret_t rtl8367c_setAsicLutFlushMode(uint32 mode);
extern ret_t rtl8367c_getAsicLutFlushMode(uint32* pMode);
extern ret_t rtl8367c_setAsicLutFlushType(uint32 type);
extern ret_t rtl8367c_getAsicLutFlushType(uint32* pType);
extern ret_t rtl8367c_setAsicLutFlushVid(uint32 vid);
extern ret_t rtl8367c_getAsicLutFlushVid(uint32* pVid);
extern ret_t rtl8367c_setAsicLutFlushFid(uint32 fid);
extern ret_t rtl8367c_getAsicLutFlushFid(uint32* pFid);
extern ret_t rtl8367c_setAsicLutDisableAging(uint32 port, uint32 disabled);
extern ret_t rtl8367c_getAsicLutDisableAging(uint32 port, uint32 *pDisabled);
extern ret_t rtl8367c_setAsicLutIPMCGroup(uint32 index, ipaddr_t group_addr, uint32 vid, uint32 pmask, uint32 valid);
extern ret_t rtl8367c_getAsicLutIPMCGroup(uint32 index, ipaddr_t *pGroup_addr, uint32 *pVid, uint32 *pPmask, uint32 *pValid);
extern ret_t rtl8367c_setAsicLutLinkDownForceAging(uint32 enable);
extern ret_t rtl8367c_getAsicLutLinkDownForceAging(uint32 *pEnable);
extern ret_t rtl8367c_setAsicLutFlushAll(void);
extern ret_t rtl8367c_getAsicLutFlushAllStatus(uint32 *pBusyStatus);
extern ret_t rtl8367c_setAsicLutIpmcFwdRouterPort(uint32 enable);
extern ret_t rtl8367c_getAsicLutIpmcFwdRouterPort(uint32 *pEnable);

#endif /*_RTL8367C_ASICDRV_LUT_H_*/

