/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Temp file for new SAL structure.
 *
 * Feature : 
 *
 */
#ifndef __SAL_HEADER_TMP_H__
#define __SAL_HEADER_TMP_H__

#include <saltype.h>
#include <sal_rtk_api_extention.h>
#include <sall2.h>
#include <salflash.h>
#include <salloop.h>

#if SAL_RTL8316D
extern int32 sal_port_iso_get(sal_lPortmask_t* pstPortMask);
extern int32 sal_port_iso_set(sal_lPortmask_t *pstPortMask);
extern int32 SalFlshPortSetIsolationMask(sal_lPortmask_t *isolationMask);
extern int32 SalFlshPortGetIsolationMask(sal_lPortmask_t *isolationMask);
#endif

/****************************salfwd.h **********************************/
#if SAL_RTL8316D
extern int32 sal_rate_igrBandwidthCtrl_en_set(uint8 port, uint32 enable);
extern int32 sal_rate_igrBandwidthCtrl_en_get(uint8 port, uint32* enable);
extern int32 sal_rate_egrBandwidthCtrl_en_set(uint8	port, uint32 enable);
extern int32 sal_rate_egrBandwidthCtrl_en_get(uint8	port, uint32* enable);
#endif

/****************************salflash.h ******************************/
#if SAL_RTL8316D
extern int32 SalFlshPortSetIsolationMask(sal_lPortmask_t *isolationMask);
extern int32 SalFlshPortGetIsolationMask(sal_lPortmask_t *isolationMask);

extern int32 sal_flash_rate_igrBandwidthEnable_set(uint8 port, uint8 enable);
extern int32 sal_flash_rate_igrBandwidthEnable_get (uint8 port, uint8* enable);
extern int32 sal_flash_rate_egrBandwidthEnable_set(uint8 port, uint8 enable);
extern int32 sal_flash_rate_egrBandwidthEnable_get (uint8 port, uint8* enable);
#endif	

#endif

