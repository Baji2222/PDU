#ifndef __UPGRADE_TFTP_H__
#define __UPGRADE_TFTP_H__

#include <common.h>

#define ARP_REQUEST     (0x0001)
#define ARP_REPLAY      (0x0002)

/* for flash */
#define CONFIG_FLASH_SECTOR_SIZE (CONFIG_SPI_SECTOR_SIZE)
#define FLASH_ERASE_BLOCK        SPI_ERASE_BLOCK
#define FLASH_ERASE_NONBLOCK     SPI_ERASE_NONBLOCK
#define flash_erase              spi_flash_erase
#define flash_init               spi_flash_init
#define flash_write              spi_flash_write

extern void getServerMAC(void);

/* return 0 for success, not 0 for fail */
extern u8_t tftp_transfer(void);

#endif /* end of __UPGRADE_TFTP_H__ */


