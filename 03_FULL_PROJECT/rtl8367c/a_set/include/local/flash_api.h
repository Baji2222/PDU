#ifndef __FLASH_API_H_
#define __FLASH_API_H_

#include <SPIFlashDrv.h>

#define FLASH_WRITE(addr, buf, len)            spi_flash_write((addr), (buf), (len))
#define FLASH_READ(addr, buf, len)             spi_flash_read((addr), (buf), (len))
#define FLASH_ERASE_SECTOR(addr)               spi_flash_erase((addr), SPI_ERASE_BLOCK)

#define FLASH_API_PRINTF    
//#define FLASH_API_PRINTF    printf

#endif

