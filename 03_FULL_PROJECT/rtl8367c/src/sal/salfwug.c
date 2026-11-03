/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 7002 $
 * $Date: 2009-11-13 14:40:24 +0800 (Fri, 13 Nov 2009) $
 *
 * Purpose : System abstraction layer API for wrapping chip driver
 * Feature : Firmware upgrade related function
 *
 */

/* 
 * Include Files 
 */
#include <salfwug.h>
#include <salerror.h>

#include <tftpd/tftpc.h>

#include <stdio.h>

#include <local/flash_config.h>
#include <common.h>
#include <SPIFlashDrv.h>
#include <image.h>

#include <stdio.h>
#include <runtime_loader_msg.h>

/* 
 * Symbol and Data Type Definition 
 */
 
/* 
 * Data Declaration 
 */
static sal_fwug_status_t      fwug_status;
static uint8                  header_get;
static uint8                  reboot_after_upload;
static uint32                 image_length;

/* 
 * Macro Declaration 
 */
#define IMAGE_HDR_LEN                       20
#define IMAGE_IROM_LEN                      (12 * 1024UL - 2)
#define RUNTIME_IROM_IN_FLASH_ADDR          (4 * 1024 + 2)
#define RUNTIME_BANK_IN_FLASH_ADDR          ((16 + 48 * 2) * 1024UL)
#define FLASH_ERASE(block_addr)             spi_flash_erase(block_addr, SPI_ERASE_BLOCK)
#define FLASH_WRITE(start_addr, buf, len)   spi_flash_write(start_addr, buf, len)
#define FLASH_READ(start_addr, buf, len)    spi_flash_read(start_addr, buf, len)
#define RUN_TIME_BANK_END(bankNumber)       (RUN_TIME_BANK_START  + bankNumber * BANK_SIZE)
#define FLASH_BLOCK_OFFSET(addr)            ((addr) & (FLASH_BLOCK_SIZE - 1))
#define FLASH_MAGICNUMBER_ADDR              ((RUNTIME_HEADER_BANK - 1) * BANK_SIZE + IROM_SIZE + (RUNTIME_HEADER_ADDRESS & 0xFFFF) - IROM_SIZE)

 
/* 
 * Function Declaration 
 */

/* Function Name:
 *      sal_fwug_http_init
 * Description:
 *      Initialize connection for TFTP firmware upgrade
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED - socket allocation fail
 * Note:
 *      None
 */
int32 sal_fwug_http_init()
{
    header_get = FALSE;

    return SAL_ERR_SUCCESS;
}
#if defined(UPGRADE_TFTP)
/* Function Name:
 *      sal_fwug_tftp_init
 * Description:
 *      Initialize connection for TFTP firmware upgrade
 * Input:
 *      servIp    - Server IP
 *      pFileName - Pointer to file name.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_FAILED - socket allocation fail
 * Note:
 *      None
 */
int32 sal_fwug_tftp_init(sal_ipaddr_t servIp, char *pFileName)
{
    uint8 ipAddr[4];
    int32 ret;

    ipAddr[0] = (uint8)((servIp >> 24) & 0xFF);
    ipAddr[1] = (uint8)((servIp >> 16) & 0xFF);
    ipAddr[2] = (uint8)((servIp >> 8)  & 0xFF);
    ipAddr[3] = (uint8)  servIp        & 0xFF;

    header_get = FALSE;

    ret = tftpc_read(ipAddr, pFileName, 0, FLASH_SIZE);

    if(ret < 0)
    {
        fwug_status = SAL_FWUG_STATUS_SOCKET_ALLOCATE_FAIL;
        return SAL_ERR_FAILED;
    }

    fwug_status = SAL_FWUG_STATUS_SEARCHING_SERVER;

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_fwug_tftpStatus_set
 * Description:
 *      Set status of TFTP client
 * Input:
 *      None
 * Output:
 *      status - TFTP client status
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_fwug_status_set(sal_fwug_status_t status)
{
    fwug_status = status;

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_fwug_tftpStatus_get
 * Description:
 *      Get status of TFTP client
 * Input:
 *      None
 * Output:
 *      pStatus - pointer to TFTP client status
 * Return:
 *      SAL_ERR_SUCCES
 * Note:
 *      None
 */
int32 sal_fwug_status_get(sal_fwug_status_t *pStatus)
{
    *pStatus = fwug_status;

    return SAL_ERR_SUCCESS;    
}
#endif
/* Function Name:
 *      sal_fwug_image_write
 * Description:
 *      Write image to chip
 * Input:
 *      pBuf   - pointer to palyload of tftp packet.
 *      length - length of tftp packet
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCES
 * Note:
 *      None
 */
int32 sal_fwug_image_write(uint8 *pBuf, uint16 length)
{
    static uint8         write_enable, write_to_bank;
    uint16               truncateLength;
    static uint32        writeLen, writeStartAddr;
    runTimeImageHeader_t *hdrPtr;

    if(FALSE == header_get)
    {
        if(!check_header(pBuf))
        {
            header_get = TRUE;
            reboot_after_upload = TRUE;
            fwug_status = SAL_FWUG_STATUS_TRANSMITTING;
        
            /* image header correct */
            write_enable = TRUE;

            /* get image length */
            hdrPtr = (runTimeImageHeader_t*)pBuf;
            /* image length excluding header length */
            image_length = hdrPtr->length - sizeof(runTimeImageHeader_t);


            /* move pointer */ 
            pBuf += sizeof(runTimeImageHeader_t);
            length -= sizeof(runTimeImageHeader_t);

            /* erase run time IROM section in flash */
            FLASH_ERASE(RUN_TIME_IROM_START);
            FLASH_ERASE(RUN_TIME_IROM_START + FLASH_BLOCK_SIZE);
            FLASH_ERASE(RUN_TIME_IROM_START + FLASH_BLOCK_SIZE * 2);

            /* initialize write address */
            writeLen = 0;

            write_to_bank = FALSE;
        }
        else
        {
            /* image header error */
            fwug_status = SAL_FWUG_STATUS_IMAGE_ERR;
            reboot_after_upload = FALSE;
            write_enable = FALSE;
            return SAL_ERR_SUCCESS;            
        }
    }

    if(write_enable)
    {
        /* write buf to IROM section in flash */
        if(writeLen + length <= IMAGE_IROM_LEN)
        {
            FLASH_WRITE(RUNTIME_IROM_IN_FLASH_ADDR + writeLen, pBuf, length);
            writeLen += length;
        }
        else
        {
            /* parts of buf need to be wirtten to IROM section,
               and parts of buf need to be written to bank section */
            if(write_to_bank == FALSE)
            {
                /* write front parts of buf to IROM section in flash */
                truncateLength = IMAGE_IROM_LEN - writeLen;
                FLASH_WRITE(RUNTIME_IROM_IN_FLASH_ADDR + writeLen, pBuf, truncateLength);

                /* move pointer */
                pBuf += truncateLength;
                length -= truncateLength;

                /* erase first block of run time bank area in flash */
                FLASH_ERASE(RUN_TIME_BANK_START);
                
                /* write rear parts of buf to bank section */
                FLASH_WRITE(RUNTIME_BANK_IN_FLASH_ADDR, pBuf, length);
                writeLen  = IMAGE_IROM_LEN + length;
                
                write_to_bank = TRUE;
            }
            /* write all of buf to bank in flash */
            else
            {
                writeStartAddr = writeLen + RUNTIME_BANK_IN_FLASH_ADDR - IMAGE_IROM_LEN;

                /* if this block is not erased, erase current block */
                if(FLASH_BLOCK_OFFSET(writeStartAddr) == 0)
                    FLASH_ERASE(writeStartAddr);

                /* if data in buf cross block, erase next block */
                /* erase should be block size aliged */                
                if(FLASH_BLOCK_OFFSET(writeStartAddr) > FLASH_BLOCK_OFFSET(writeStartAddr + length))
                    FLASH_ERASE((writeStartAddr + length) & (~(FLASH_BLOCK_SIZE - 1)));
            
                FLASH_WRITE(writeLen + RUNTIME_BANK_IN_FLASH_ADDR - IMAGE_IROM_LEN, pBuf, length);

                writeLen += length;
           }
        }
    }

	return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_fwug_termination_set
 * Description:
 *      Action for termination of firmware upgrade
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCES
 * Note:
 *      None
 */
int32 sal_fwug_termination_set()
{
    if(reboot_after_upload == TRUE)
	{
	    fwug_status = SAL_FWUG_STATUS_COMPLETE;
        setReg(0x1322, 1);
	}

	return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_fwug_startState_get
 * Description:
 *      Get start state of firmware upgrade
 * Input:
 *      None
 * Output:
 *      *pState - pointer to start state;
 * Return:
 *      SAL_ERR_SUCCES
 * Note:
 *      None
 */
int32 sal_fwug_startState_get(sal_fwug_startState_t *pState)
{
    uint8 start_state;

	loader_start_mode_get(&start_state);

    if(START_MODE_WARM == start_state)
    {
        *pState = SAL_FWUG_STARTSTATE_WARM;    
    }
    else
    {
        *pState = SAL_FWUG_STARTSTATE_COLD;    
    }

    return SAL_ERR_SUCCESS;
}


