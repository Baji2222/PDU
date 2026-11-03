#ifndef _RTL8367C_ASICDRV_RMA_H_
#define _RTL8367C_ASICDRV_RMA_H_

#include <rtl8367c_asicdrv.h>

#define RTL8367C_RMAMAX                     0x2F

enum RTL8367C_RMAOP
{
    RMAOP_FORWARD = 0,
    RMAOP_TRAP_TO_CPU,
    RMAOP_DROP,
    RMAOP_FORWARD_EXCLUDE_CPU,
    RMAOP_END
};


typedef struct  rtl8367c_rma_s{

    uint16 operation;
    uint16 discard_storm_filter;
    uint16 trap_priority;
    uint16 keep_format;
    uint16 vlan_leaky;
    uint16 portiso_leaky;

}rtl8367c_rma_t;


extern ret_t rtl8367c_setAsicRma(uint32 index, rtl8367c_rma_t* pRmacfg);
extern ret_t rtl8367c_getAsicRma(uint32 index, rtl8367c_rma_t* pRmacfg);
extern ret_t rtl8367c_setAsicRmaCdp(rtl8367c_rma_t* pRmacfg);
extern ret_t rtl8367c_getAsicRmaCdp(rtl8367c_rma_t* pRmacfg);
extern ret_t rtl8367c_setAsicRmaCsstp(rtl8367c_rma_t* pRmacfg);
extern ret_t rtl8367c_getAsicRmaCsstp(rtl8367c_rma_t* pRmacfg);
extern ret_t rtl8367c_setAsicRmaLldp(uint32 enabled, rtl8367c_rma_t* pRmacfg);
extern ret_t rtl8367c_getAsicRmaLldp(uint32 *pEnabled, rtl8367c_rma_t* pRmacfg);

#endif /*#ifndef _RTL8367C_ASICDRV_RMA_H_*/

