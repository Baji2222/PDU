#ifndef __NORFLASHDRV_H_
#define __NORFLASHDRV_H_

#include <var_sfr.h>
#include <local/config.h>
#include "common.h"

#ifdef FLASH_TYPE_NOR

#if 0
/*  BIT Registers  */
sbit SPI_CTRL_BUSY   = CACHE_CRTL^0;
sbit CACHE_CLEAN     = CACHE_CRTL^1;
sbit SPI_CMD_ISSUE   = CACHE_CRTL^2;
#endif

extern void   eraseFlash();
extern uint8  nor_flash_read(uint32 start_adr, uint8 *buf, uint32 len);
extern void   nor_flash_write(uint32 addr, uint8 *buf, uint32 len);
extern void   writeFlash(uint32 addr, uint8 value);
extern void   nor_flash_reset();

#endif /* end of FLASH_TYPE_NOR */

#endif /* end of __NOR_FLASH_DRIVER_H__ */

