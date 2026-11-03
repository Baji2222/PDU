#include <stdio.h>
#include <lwps\lwps.h>
#include <local/flash_config.h>
#include "SPIFlashDrv.h"


code uint32 flash_protect_area[] = { CONFIG_FLASH_PROTECT };
code uint8 num_flash_protected_area=FLASH_PROTECT_REGION;
static uint8 __spi_flash_op_wrapper(uint8 *buf, uint32 start_adr, uint16 len, uint8 op) {
    struct spi_write_flash_argument_t arg;
    uint16 clen;
/*
    if (start_adr&0xff) {
        OSAL_PRINTF("        __spi_flash_op_wrapper: f_addr=0x%08lx, len=%d, op=%bd\n", 
            start_adr, len, op);
    }
*/
    arg.op=op;
    arg.f_addr=start_adr;
    if ((op==SPI_FLASH_OP_READ)||(op==SPI_FLASH_OP_WRITE)) {
        arg.x_addr=(((uint16)buf)&0xffff);
        while (len>0) {
            clen=(len>256)?256:len;
            arg.len=(clen&0xff);
            (*spi_flash_driver_entry)(((uint16)&arg)&0xffff);
            arg.f_addr += clen;
            arg.x_addr += clen;
            len -= clen;
        }
        return TRUE;
    } else {
        // op=SPI_FLASH_OP_ERASE/SPI_FLASH_OP_ERASE_BLOCKING/SPI_FLASH_OP_INIT
        return (*spi_flash_driver_entry)(((uint16)&arg)&0xffff);
    }
}
// SPI Flash Erase: Erases a sector/block at specified address with blocking/non-blocking option
// Performs alignment, chip size, and protected region validation before erasing
uint8 spi_flash_erase(uint32 erase_block_adr, uint8 blocking) large {
/*
    OSAL_PRINTF("      spi_flash_erase: entry=%p, addr=0x%08lx, blocking=%bd\n", 
        spi_flash_driver_entry, erase_block_adr, blocking); 
*/

    /* Error Check */
    #ifdef CONFIG_CHECK_CHIP_SIZE
        if( (erase_block_adr)>(spi_chip0_chip_size) ){
            SPI_ERROR_MSG(("adr[0x%08lX] is larger than ChipSize[0x%08lX]!\n", 
                    erase_block_adr, (spi_chip0_chip_size)));
            
            return FALSE;
        }
    #endif
    if( 0!= (erase_block_adr%CONFIG_SPI_SECTOR_SIZE) ){
        SPI_ERROR_MSG(("erase_block_adr is not aligned!\n"));
        return FALSE;
    }
    #ifdef CONFIG_CHECK_PROTECT_REGION
    {   uint8 i;
        for (i=0; i<FLASH_PROTECT_REGION; i+=2){
            if (erase_block_adr<flash_protect_area[i]) continue;
            if (erase_block_adr>=flash_protect_area[i+1]) continue;
            return FALSE;
        }
    }
    #endif
    return __spi_flash_op_wrapper(NULL, erase_block_adr, 0, 
        (blocking==SPI_ERASE_NONBLOCK)?SPI_FLASH_OP_ERASE:SPI_FLASH_OP_ERASE_BLOCKING);
}
// SPI Flash Write: Writes data to flash at specified address with automatic page alignment handling
// Validates chip size limits and protected regions before performing write operation
uint8 spi_flash_write(uint32 start_adr, uint8 *buf, uint16 len) large {
    uint32 xdata align_adr = 0, pre_size = 0;
/*
    if (start_adr&0xff) {
        OSAL_PRINTF("      spi_flash_write: entry=%p, addr=0x%08lx, len=%d\n", 
            spi_flash_driver_entry, start_adr, len); 
    }
*/
    #ifdef CONFIG_CHECK_CHIP_SIZE
        if( (start_adr+len)>spi_chip0_chip_size ){
            SPI_ERROR_MSG(("[adr+len=0x%08lX] is larger than ChipSize[0x%08lX]!\n", 
                start_adr+len, spi_chip0_chip_size));
            return FALSE;
        }
    #endif

    #ifdef CONFIG_CHECK_PROTECT_REGION
    { uint8 i;
        for (i=0; i<FLASH_PROTECT_REGION; i+=2){
            if ((start_adr+len) <= flash_protect_area[i]) continue;
            if (start_adr>=flash_protect_area[i+1]) continue;
            SPI_ERROR_MSG(("flash write area is protected!\n"));
            return FALSE;
        }
    }
    #endif
    
    /* align page size */
    align_adr = start_adr&(~CONFIG_SPI_PAGE_MASK);
    pre_size = CONFIG_SPI_PAGE_SIZE-(start_adr-align_adr);
    
    if( (0!=start_adr%CONFIG_SPI_PAGE_SIZE) &&  ( len>pre_size ) ){
        __spi_flash_op_wrapper(buf, start_adr, pre_size, SPI_FLASH_OP_WRITE);
        start_adr += pre_size;
        buf += pre_size;
        len -= pre_size;
    }
    __spi_flash_op_wrapper(buf, start_adr, len, SPI_FLASH_OP_WRITE);
    
    return TRUE;
}

// SPI Flash Read: Reads data from flash at specified address into provided buffer
// Validates chip size limits before performing read operation
uint8 spi_flash_read(uint32 start_adr, uint8 *buf, uint16 len) large {
    #ifdef CONFIG_CHECK_CHIP_SIZE
        if ((start_adr+len)>spi_chip0_chip_size) {
            SPI_ERROR_MSG(("start_adr+len is larger than chip_size!\n"));
            return FALSE;
        }
    #endif
    __spi_flash_op_wrapper(buf, start_adr, len, SPI_FLASH_OP_READ);
    return TRUE;
}