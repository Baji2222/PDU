#include "../NORFlashDrv.h"

#ifdef FLASH_TYPE_NOR
#include <stdio.h>
#include <common_types.h>

/* For NOR Flash */
#define AUTO_SELECT_ADDR1       (0xAAAUL)
#define AUTO_SELECT_ADDR2       (0x555UL)
#define AUTO_SELECT_ADDR3       (0xAAAUL)
#define AUTO_SELECT_DATA1       (0xAA)
#define AUTO_SELECT_DATA2       (0x55)
#define AUTO_SELECT_DATA3       (0x90)

#define READ_SILICON_ID_ADDR    (0x00)
#define READ_DEVICE_ID_ADDR     (0x02)

#define NOR_RESET_ADDR          (0x00)
#define NOR_RESET_DATA          (0xF0)

#define NOR_ERASE_ADDR1       (0xAAAUL)
#define NOR_ERASE_ADDR2       (0x555UL)
#define NOR_ERASE_ADDR3       (0xAAAUL)
#define NOR_ERASE_ADDR4       (0xAAAUL)
#define NOR_ERASE_ADDR5       (0x555UL)
#define NOR_ERASE_DATA1       (0xAA)
#define NOR_ERASE_DATA2       (0x55)
#define NOR_ERASE_DATA3       (0x80)
#define NOR_ERASE_DATA4       (0xAA)
#define NOR_ERASE_DATA5       (0x55)
#define NOR_ERASE_DATA6       (0x30)

#define NOR_WRITE_ADDR1       (0xAAAUL)
#define NOR_WRITE_ADDR2       (0x555UL)
#define NOR_WRITE_ADDR3       (0xAAAUL)
#define NOR_WRITE_DATA1       (0xAA)
#define NOR_WRITE_DATA2       (0x55)
#define NOR_WRITE_DATA3       (0xA0)

#define NOR_ERASE_CHIP_ADDR1       (0xAAAUL)
#define NOR_ERASE_CHIP_ADDR2       (0x555UL)
#define NOR_ERASE_CHIP_ADDR3       (0xAAAUL)
#define NOR_ERASE_CHIP_ADDR4       (0xAAAUL)
#define NOR_ERASE_CHIP_ADDR5       (0x555UL)
#define NOR_ERASE_CHIP_ADDR6       (0xAAAUL)
#define NOR_ERASE_CHIP_DATA1       (0xAA)
#define NOR_ERASE_CHIP_DATA2       (0x55)
#define NOR_ERASE_CHIP_DATA3       (0x80)
#define NOR_ERASE_CHIP_DATA4       (0xAA)
#define NOR_ERASE_CHIP_DATA5       (0x55)
#define NOR_ERASE_CHIP_DATA6       (0x10)



/* Utilities for sfr */
#define SPI_S_ACCESS_READ       (0x01)
#define SPI_S_ACCESS_WRITE      (0x08)


extern uint16 getReg(uint16 address);

extern void setReg(uint16 address, uint16 value);


void nor_flash_reset(){
	SPI_S_ACCESS = SPI_S_ACCESS_WRITE;
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_RESET_ADDR>>16);
	SPI_S_ADDR1 = (uint8)((NOR_RESET_ADDR>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_RESET_ADDR>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_RESET_DATA;
	SPI_CMD_ISSUE = 1;
	
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
}

#if 1
void enter_auto_select(){
	SPI_S_ACCESS = SPI_S_ACCESS_WRITE;
	
	/* ADDR1 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( AUTO_SELECT_ADDR1>>16);
	SPI_S_ADDR1 = (uint8)((AUTO_SELECT_ADDR1>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((AUTO_SELECT_ADDR1>> 0)&0x0000FF);
	SPI_S_DATA0 = AUTO_SELECT_DATA1;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR2 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( AUTO_SELECT_ADDR2>>16);
	SPI_S_ADDR1 = (uint8)((AUTO_SELECT_ADDR2>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((AUTO_SELECT_ADDR2>> 0)&0x0000FF);
	SPI_S_DATA0 = AUTO_SELECT_DATA2;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR3 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( AUTO_SELECT_ADDR3>>16);
	SPI_S_ADDR1 = (uint8)((AUTO_SELECT_ADDR3>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((AUTO_SELECT_ADDR3>> 0)&0x0000FF);
	SPI_S_DATA0 = AUTO_SELECT_DATA3;
	SPI_CMD_ISSUE = 1;
	
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
}
#endif

/* Q7 valid value is 0/1(for program) or 2(for erase) */
void checkReady(uint32 addr, uint8 Q7){
	uint8 value;
	uint8 Q7_shift = (Q7<<7)&0xFF;
	
	while( 1==SPI_CTRL_BUSY );
	
	SPI_S_ACCESS = SPI_S_ACCESS_READ;
	
	while(1){
		SPI_S_ADDR2 = (uint8)( addr>>16);
		SPI_S_ADDR1 = (uint8)((addr>> 8)&0x0000FF);
		SPI_S_ADDR0 = (uint8)((addr>> 0)&0x0000FF);
		value       = SPI_S_DATA0;
		SPI_CMD_ISSUE = 1;
		while( 1==SPI_CTRL_BUSY );
		
		if( 2==Q7 && 0xFF==value ){ // for erase
			break;
		}else if( 0==Q7 || 1==Q7 ){                      // for program
			if( (value&0x80)==Q7_shift ){
				break;
			}
		}else{
		}
	}
	
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
}

void writeFlash(uint32 addr, uint8 value)
{
	SPI_S_ACCESS = SPI_S_ACCESS_WRITE;
    /* ADDR1 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_WRITE_ADDR1>>16);
	SPI_S_ADDR1 = (uint8)((NOR_WRITE_ADDR1>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_WRITE_ADDR1>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_WRITE_DATA1;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR2 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_WRITE_ADDR2>>16);
	SPI_S_ADDR1 = (uint8)((NOR_WRITE_ADDR2>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_WRITE_ADDR2>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_WRITE_DATA2;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR3 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_WRITE_ADDR3>>16);
	SPI_S_ADDR1 = (uint8)((NOR_WRITE_ADDR3>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_WRITE_ADDR3>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_WRITE_DATA3;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR4 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( addr>>16);
	SPI_S_ADDR1 = (uint8)((addr>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((addr>> 0)&0x0000FF);
	SPI_S_DATA0 = value;
	SPI_CMD_ISSUE = 1;
	
	checkReady(addr, (value>>7) );
	
}

void nor_flash_write(uint32 addr, uint8 *buf, uint32 len){
	uint32 i;
	for(i=0;i<len;i++){
		writeFlash(addr+i, buf[i]);
	}
}

#if 1
uint8 __nor_flash_read(uint32 addr)
{
    uint8 flashData;
    
    nor_flash_reset();

    SPI_S_ACCESS = SPI_S_ACCESS_READ;

	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( addr>>16);
	SPI_S_ADDR1 = (uint8)((addr>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((addr>> 0)&0x0000FF);
	
	SPI_CMD_ISSUE = 1;
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );

	flashData   = SPI_S_DATA0;
	return flashData;
}
uint8 nor_flash_read(uint32 start_adr, uint8 *buf, uint32 len){
	uint32 i;
	
	for(i=0;i<len;i++){
		buf[i] = __nor_flash_read(start_adr+i);
	}
	return TRUE;
}
#endif

#if 1
void eraseFlashSector(uint32 sector)
{
	SPI_S_ACCESS = SPI_S_ACCESS_WRITE;
	
	/* ADDR1 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_ERASE_ADDR1>>16);
	SPI_S_ADDR1 = (uint8)((NOR_ERASE_ADDR1>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_ERASE_ADDR1>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_ERASE_DATA1;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR2 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_ERASE_ADDR2>>16);
	SPI_S_ADDR1 = (uint8)((NOR_ERASE_ADDR2>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_ERASE_ADDR2>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_ERASE_DATA2;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR3 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_ERASE_ADDR3>>16);
	SPI_S_ADDR1 = (uint8)((NOR_ERASE_ADDR3>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_ERASE_ADDR3>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_ERASE_DATA3;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR4 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_ERASE_ADDR4>>16);
	SPI_S_ADDR1 = (uint8)((NOR_ERASE_ADDR4>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_ERASE_ADDR4>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_ERASE_DATA4;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR5 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_ERASE_ADDR5>>16);
	SPI_S_ADDR1 = (uint8)((NOR_ERASE_ADDR5>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_ERASE_ADDR5>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_ERASE_DATA5;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR6 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( sector>>16);
	SPI_S_ADDR1 = (uint8)((sector>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((sector>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_ERASE_DATA6;
	SPI_CMD_ISSUE = 1;
	
	checkReady(sector, 2 );
}
#endif

void eraseFlash(void)
{
    SPI_S_ACCESS = SPI_S_ACCESS_WRITE;
	
	/* ADDR1 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_ERASE_CHIP_ADDR1>>16);
	SPI_S_ADDR1 = (uint8)((NOR_ERASE_CHIP_ADDR1>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_ERASE_CHIP_ADDR1>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_ERASE_DATA1;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR2 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_ERASE_CHIP_ADDR2>>16);
	SPI_S_ADDR1 = (uint8)((NOR_ERASE_CHIP_ADDR2>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_ERASE_CHIP_ADDR2>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_ERASE_DATA2;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR3 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_ERASE_CHIP_ADDR3>>16);
	SPI_S_ADDR1 = (uint8)((NOR_ERASE_CHIP_ADDR3>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_ERASE_CHIP_ADDR3>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_ERASE_DATA3;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR4 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_ERASE_CHIP_ADDR4>>16);
	SPI_S_ADDR1 = (uint8)((NOR_ERASE_CHIP_ADDR4>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_ERASE_CHIP_ADDR4>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_ERASE_DATA4;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR5 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_ERASE_CHIP_ADDR5>>16);
	SPI_S_ADDR1 = (uint8)((NOR_ERASE_CHIP_ADDR5>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_ERASE_CHIP_ADDR5>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_ERASE_DATA5;
	SPI_CMD_ISSUE = 1;
	
	/* ADDR6 */
	/* Check Controller Ready */
	while( 1==SPI_CTRL_BUSY );
	SPI_S_ADDR2 = (uint8)( NOR_ERASE_CHIP_ADDR6>>16);
	SPI_S_ADDR1 = (uint8)((NOR_ERASE_CHIP_ADDR6>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((NOR_ERASE_CHIP_ADDR6>> 0)&0x0000FF);
	SPI_S_DATA0 = NOR_ERASE_CHIP_DATA6;
	SPI_CMD_ISSUE = 1;
	
	checkReady(0x0, 2 );
}

#if 0
void readID(void){
	uint16 adr;
	uint8 SID, DID;
	
	enter_auto_select();
	
	SPI_S_ACCESS = SPI_S_ACCESS_READ;
	
	/* Read Silicon ID */
	SPI_S_ADDR2 = (uint8)( READ_SILICON_ID_ADDR>>16);
	SPI_S_ADDR1 = (uint8)((READ_SILICON_ID_ADDR>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((READ_SILICON_ID_ADDR>> 0)&0x0000FF);
	SPI_CMD_ISSUE = 1;
	while( 1==SPI_CTRL_BUSY );
	SID = SPI_S_DATA0;
	
	/* Read Device ID */
	SPI_S_ADDR2 = (uint8)( READ_DEVICE_ID_ADDR>>16);
	SPI_S_ADDR1 = (uint8)((READ_DEVICE_ID_ADDR>> 8)&0x0000FF);
	SPI_S_ADDR0 = (uint8)((READ_DEVICE_ID_ADDR>> 0)&0x0000FF);
	SPI_CMD_ISSUE = 1;
	while( 1==SPI_CTRL_BUSY );
	DID = SPI_S_DATA0;
	
	OSAL_PRINTF("NOR flash silicon ID = 0x%02x\n", (uint16)SID);
	OSAL_PRINTF("NOR flash  device ID = 0x%02x\n", (uint16)DID);
}
#endif

#endif /* end of FLASH_TYPE_NOR */
