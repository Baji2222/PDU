#include <stdio.h>
#include <common.h>

#include <common_types.h>
#include <local/flash_config.h>
#include <local/flash_api.h>
#include <SPIFlashDrv.h>

#define BASIC_SECTOR_SIZE    (4UL * 1024UL)
#define BASIC_SECTOR_MASK    (BASIC_SECTOR_SIZE - 1UL)

#define UPGRADE_FLSH_DONE   0
#define UPGRADE_FLSH_UNDONE 1

/* address and len must be 4KB alignmnet */
void _flash_api_erase_section(uint32 addr, uint32 len)
{
    uint16 block = (len+BASIC_SECTOR_MASK)/BASIC_SECTOR_SIZE;
	uint16 i;

	for(i=0;i<block;i++){
		FLASH_ERASE_SECTOR(addr);
		addr += BASIC_SECTOR_SIZE;
	}
}

void flash_api_set_status(uint8 undone)
{
    /* kill warning message */
	undone = 0;
}

void flash_api_erase_cover_space()
{
    //FLASH_API_PRINTF("erase cover space\n");
    _flash_api_erase_section(UPGRADE_FLSH_COVER_SPACE_ADDR, UPGRADE_FLSH_COVER_SPACE_SIZE);
}

void flash_api_upgrade_terminate(uint32 imgLen)
{
    uint32 writeCount;
	uint16 len, tmp;
	uint8 ret;

    CKCON = 1;

	for(len=0;len<10000;len++);

    /* set imgLen 4KB alignment */
	imgLen = imgLen & (~BASIC_SECTOR_MASK);
    imgLen += BASIC_SECTOR_SIZE;

    EA = 0;
    
	/* disable NIC */
    tmp = getReg(0x130c);
	tmp &= 0xFBFF;
	setReg(0x130c, tmp);

    /*  erase code space in flash */
    _flash_api_erase_section(UPGRADE_FLSH_CODE_SPACE_ADDR, imgLen);

	/* copy data from backup space to code space */
    writeCount = 0;

	while(1)
	{
        //FLASH_API_PRINTF("%s %u - writeCount: %04X%04X\n", __FILE__, (uint16)__LINE__, (uint16)(writeCount >> 16), (uint16)(writeCount & 0xFFFF));
        //printf("%s %u - writeCount: %04X%04X\n", __FILE__, (uint16)__LINE__, (uint16)(writeCount >> 16), (uint16)(writeCount & 0xFFFF));
        len = (UPGRADE_FLSH_BACKUP_SPACE_SIZE - writeCount < sizeof(stack_buffer))?(UPGRADE_FLSH_BACKUP_SPACE_SIZE- writeCount):sizeof(stack_buffer);

		FLASH_READ(writeCount + UPGRADE_FLSH_BACKUP_SPACE_ADDR, p_buffer, len);
		ret = FLASH_READ(writeCount + UPGRADE_FLSH_CODE_SPACE_ADDR, p_buffer, len);
		writeCount += len;

		if ( writeCount >= UPGRADE_FLSH_BACKUP_SPACE_SIZE )
		    break;
    }

	/* erase bakcup space in flash */
    _flash_api_erase_section(UPGRADE_FLSH_BACKUP_SPACE_ADDR, UPGRADE_FLSH_CODE_SPACE_SIZE);

    flash_api_set_status(UPGRADE_FLSH_DONE);

    /* reset */
	setReg(0x1322, 1);
}

