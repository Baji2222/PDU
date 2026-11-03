#pragma STRING (far)

#include <local/config.h>
#include <local/flash_api.h>

#include <stdio.h>

#include <common_types.h>
#include <common.h>
#include <utility/firmware_upgrade.h>




static uint32 currentPtr;

static uint8  upgradeReboot = FALSE;
static uint8  upgradeStatus = UPGRADE_NOT_START;
static uint32 imgLen;

#define UPGRADE_WRITE_TO_BACKUP_SPACE   0
#define UPGRADE_WRITE_TO_COVER_SPACE    1

/* this function should be placed in IROM */
void upgrade_init(uint32 len)
{
    currentPtr = 0;
    imgLen = len;
	upgradeStatus = UPGRADE_PROGRESSING;

	flash_api_set_status(UPGRADE_FLSH_UNDONE);
	
FWUPG_PRINTF("%s %u - firmware upgrade init\n", __FILE__, (uint16)__LINE__);
    /* note: backup space is not erased here */
	/*       backup space needs to be erased after firmware update */

	flash_api_erase_cover_space();
	
}

int16 upgrade_write_img(uint8 *buf, uint16 len)
{
    uint8 ret;
	uint16 writeLen;
	static uint8 state = UPGRADE_WRITE_TO_BACKUP_SPACE;
	static uint16 count = 0;

	OSAL_PRINTF("*");
	count++;
	if ( count % 64 == 0 )
		printf("\n");

//printf("%s %u - currentPtr: %04X%04X\n", __FILE__, (uint16)__LINE__, (uint16)(currentPtr>>16), (uint16)(currentPtr & 0xFFFF));
//printf("%s %u - currentPtr: %04X%04X\n", __FILE__, (uint16)__LINE__, (uint16)(UPGRADE_BIN_COVER_SPACE_ADDR>>16), (uint16)(UPGRADE_BIN_COVER_SPACE_ADDR & 0xFFFF));
//printf("%s %u - write %u bytes at %04x%04x\n", __FILE__, (uint16)__LINE__, (uint16)len, (uint16)((currentPtr + UPGRADE_FLSH_BACKUP_SPACE_ADDR) >> 16), (uint16)((currentPtr + UPGRADE_FLSH_BACKUP_SPACE_ADDR) & 0xFFFF));

    if ( state == UPGRADE_WRITE_TO_BACKUP_SPACE )
	{
        if ( currentPtr + len >= UPGRADE_BIN_COVER_SPACE_ADDR )
	    {
FWUPG_PRINTF("%s %u - corss bound\n", __FILE__, (uint16)__LINE__);
		    writeLen = UPGRADE_BIN_COVER_SPACE_ADDR - currentPtr;
	        ret = FLASH_WRITE(currentPtr + UPGRADE_FLSH_BACKUP_SPACE_ADDR, buf, writeLen);
			
			if ( ret != SUCCESS )
			    return ret;
			
			buf += writeLen;
			len = len - writeLen;
			currentPtr = UPGRADE_BIN_COVER_SPACE_ADDR;

			state = UPGRADE_WRITE_TO_COVER_SPACE;
		}		
		else
        {
FWUPG_PRINTF("%s %u - in bound\n", __FILE__, (uint16)__LINE__);        
		    FLASH_WRITE(currentPtr + UPGRADE_FLSH_BACKUP_SPACE_ADDR, buf, len);
		    currentPtr += len;

			if ( ret != SUCCESS )
			    return ret;

		}
	}

	if ( state == UPGRADE_WRITE_TO_COVER_SPACE )
	{
        if ( currentPtr >= imgLen || currentPtr >= UPGRADE_FLSH_COVER_SPACE_ADDR + UPGRADE_FLSH_COVER_SPACE_SIZE)
        {
            FWUPG_PRINTF("%s():%u invalid write cmd!\n", __FILE__, (uint16)__LINE__);
	        return;
        }
		FLASH_WRITE(currentPtr + UPGRADE_FLSH_COVER_SPACE_ADDR, buf, len);
		currentPtr += len;

        if ( ret != SUCCESS )
		    return ret;
	}

   return SUCCESS;
}    

void upgrade_set_status(uint8 status)
{
    upgradeStatus = status;
}

void upgrade_get_status(uint8 *status)
{
     *status = upgradeStatus;
}

uint8 upgrade_upload_done()
{
    if ( currentPtr >= imgLen )
	    return TRUE;

	if ( currentPtr >= UPGRADE_FLSH_COVER_SPACE_SIZE + UPGRADE_FLSH_CODE_SPACE_SIZE)
	    return TRUE;

    if ( upgradeStatus == UPGRADE_SUCCESS)
    {
OSAL_PRINTF("%s %u - upgrade is done\n", __FILE__, (uint16)__LINE__);			
	    return TRUE;
    }

	return FALSE;
}

void upgrade_terminate()
{
    flash_api_upgrade_terminate(imgLen);
}

void upgrade_set_reboot_flag(uint8 reboot)
{
    upgradeReboot = reboot;
}

void upgrade_get_reboot_flag(uint8 *reboot)
{
    *reboot = upgradeReboot;
}

void upgrade_set_img_len(uint32 len)
{
    imgLen = len;
}

void upgrade_get_img_len(uint32 *len)
{
    *len = imgLen;
}

