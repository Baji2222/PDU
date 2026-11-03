#include <local/config.h>
#include <reg_sfr.h>

//#include "SPIFlashDrv.h"
//#include "NORFlashDrv.h"
#include <stdio.h>

#include "common.h"
#include "asicreg.h"

/* Functions */
#if 0
uint16 getReg(uint16 address)
{
	uint16 xdata tmp;
	EA  = 0;
    INDACC_ADDR_H = address / 0x100;
    INDACC_ADDR_L = address & 0xFF;
	INDACC_CMD = 0x01;

	while( INDACC_STATUS != 0 ) 
		;

	tmp = (uint16)((uint16)(INDACC_RDATA_H) << ((uint16)8)) | (uint16)INDACC_RDATA_L;
	EA  = 1;
	return tmp;
}

void setReg(uint16 address, uint16 value)
{
    uint16 xdata tmp = 0;
	EA  = 0;
    INDACC_ADDR_H = (uint8)(address / 0x100);
    INDACC_ADDR_L = (uint8)(address & 0xFF);
    INDACC_WDATA_H = (uint8)(value / 0x100);
    INDACC_WDATA_L = (uint8)(value & 0xFF);

    INDACC_CMD = 0x03;

	while( INDACC_STATUS != 0)
		;
	EA  = 1;
	return;
}
#endif /* */

void change_clock(uint16 xdata value) large{
	volatile uint32 xdata i;
	for(i=0;i<0x100;i++);
	if(value < 0x04) //8051 Clock >100Mhz
	{
	    //if 8051 Clock >100Mhz, SPI flash access clock is divided to half 8051 Clock
        setReg(DW8051_RDY, (DW8051_RDY_RATE(value)|(1<<8)) );
	}
	else
    {
        //if 8051 Clock < 100Mhz, SPI flash access clock is equal to 8051 Clock
	setReg(DW8051_RDY, (DW8051_RDY_RATE(value)) );
    }
	for(i=0;i<65535;i++);
	uart_init();
	for(i=0;i<0x100;i++);
}

void set_8051RDY() large{
	volatile uint32 xdata tmp;
	tmp = getReg(DW8051_RDY);

	setReg(DW8051_RDY, tmp |0x1);
}


