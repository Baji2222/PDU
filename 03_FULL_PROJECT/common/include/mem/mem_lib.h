/*
* Copyright c                  Realtek Semiconductor Corporation, 2008  
* All rights reserved.
* 
* Abstract : RTL8213M Media Converter, memory library 
* 
*$Id: mem_lib.h,v 1.2 2008/04/24 05:23:10 yajin_zhou Exp $
*/

#ifndef __MEM_LIB_H__
#define __MEM_LIB_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <local/snmp_config.h>
#include <common.h>

#if LWPS_SNMP&LWPS_SNMP_RMON
#define MEM_SIZE  0x2500 //0x2800       /*10k malloc memory */
#else
#ifdef RTL8367B
#define MEM_SIZE  0x0800        /*2k malloc memory */
#else
#define MEM_SIZE  0x1800        /*6k malloc memory */
#endif  
#endif


#define mem_free(x) free(x)
#define mem_malloc(x) malloc(x)
#define mem_realloc(x, size) realloc(x,size)
extern void mem_init(void);
extern u8_t* malloc_tx_buf(u32_t len);
#endif /* end of __MEM_LIB_H__ */


