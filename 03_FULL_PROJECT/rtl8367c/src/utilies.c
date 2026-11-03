#include <local/config.h>
#include <reg_sfr.h>

//#include "SPIFlashDrv.h"
//#include "NORFlashDrv.h"
#include <stdio.h>

#include "common.h"
#include "asicreg.h"

/* Functions */
#if 0
// Get Register: Reads a 16-bit value from the specified register address using indirect access
// Disables interrupts during access and waits for status completion before returning data
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

// Set Register: Writes a 16-bit value to the specified register address using indirect access
// Disables interrupts during access and waits for status completion before returning
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
#endif


#if 0
// Get Flash Data: Reads a single byte from flash memory at the specified address
// Calls flash_read function to retrieve data from SPI/NOR flash
uint8 getFlashData(uint32 addr){
	uint8 xdata value;
	
	//flash_read(addr, &value, 1);
	return(value);
}
#endif