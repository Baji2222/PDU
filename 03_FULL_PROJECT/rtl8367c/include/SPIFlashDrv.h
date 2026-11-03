#ifndef __SPIFLASHDRV_H_
#define __SPIFLASHDRV_H_

#include "asicreg.h"
#include <local/config.h>
#include <common_types.h>
#include <reg_sfr.h>

// self-check for conflict 
#if (defined(CONFIG_FOR_RUNTIME) && defined(CONFIG_FOR_LOADER))
    #error "conflict options: both CONFIG_FOR_RUNTIME and CONFIG_FOR_LOADER were defined!"
#endif
#if (!defined(CONFIG_FOR_RUNTIME) && !defined(CONFIG_FOR_LOADER))
    #error "conflict options: both CONFIG_FOR_RUNTIME and CONFIG_FOR_LOADER were NOT defined!"
#endif
#if (defined(CONFIG_FOR_LOADER) && defined(CONFIG_STANDALONE))
    #error "conflict options: CONFIG_STANDALONE shouldn't be defined while CONFIG_FOR_LOADER defined"
#endif


#define CONFIG_TO_USE_SCKU_SPI_DRIVER
#define CONFIG_CHECK_CHIP_SIZE
#define CONFIG_CHECK_PROTECT_REGION

/* Configuration Definition*/
#define CONFIG_SPI_QIO         (0)
#define CONFIG_SPI_FLASH_START (0x8000UL)
#define CONFIG_SPI_BANK_SIZE   (48*1024UL)
#define CONFIG_SPI_FIRST_BANK  (16*1024UL)
#define CONFIG_SPI_SECTOR_SIZE ( 4*1024UL)
#define CONFIG_SPI_PAGE_SIZE   (256UL)
#define CONFIG_SPI_PAGE_MASK   (0xFFUL)

#define CONFIG_SPI_DUAL_FAST   (1<<0)
#define CONFIG_SPI_SERIAL_FAST (1<<1)
//#define CONFIG_SPI_SERIAL_NORM (1<<2)
//insert new IO mode here
#define CONFIG_SPI_IO_MAX_MODE (1<<2)

/* 
 * macro utilities 
 */
/* SFR */
#define SPI_S_ACCESS_BYTE(n)    ((n)<<0)
#define SPI_S_ACCESS_READ       (0<<3)
#define SPI_S_ACCESS_WRITE      (1<<3)
#define SPI_S_ACCESS_SKIP_ADR   (1<<4)
#define SPI_IO_CONF_DEFAULT     (0)
#define SPI_IO_CONF_MHZ_MASK    (0x07)
#define SPI_IO_CONF_MHZ(val)    ((val)&SPI_IO_CONF_MHZ_MASK)
	#define SPI_IO_CONF_20_8MHZ (0)
	#define SPI_IO_CONF_41_6MHZ (1)
	#define SPI_IO_CONF_50MHZ   (2)
	#define SPI_IO_CONF_75MHZ   (3)
	#define SPI_IO_CONF_100MHZ  (4)
#define SPI_IO_CONF_IOMODE_MASK (0x1F<<3)
#define SPI_IO_CONF_IOMODE(val) (((val)<<3)&SPI_IO_CONF_IOMODE_MASK)
	#define SPI_IO_CONF_IO_SSS  (0x00)
	#define SPI_IO_CONF_IO_SSD  (0x01)
	#define SPI_IO_CONF_IO_SSQ  (0x02)
	#define SPI_IO_CONF_IO_SDD  (0x03)
	#define SPI_IO_CONF_IO_SDQ  (0x04)
	#define SPI_IO_CONF_IO_SQQ  (0x05)
	#define SPI_IO_CONF_IO_DDD  (0x0B)
	#define SPI_IO_CONF_IO_DDQ  (0x0C)
	#define SPI_IO_CONF_IO_DQQ  (0x0D)
	#define SPI_IO_CONF_IO_QQQ  (0x13)
	
#define SPI_S_CONF_DUMMY(val)         (val)
	#define SPI_S_CONF_DUMMY_00cycle  (0)
	#define SPI_S_CONF_DUMMY_02cycle  (2)
	#define SPI_S_CONF_DUMMY_04cycle  (4)
	#define SPI_S_CONF_DUMMY_06cycle  (6)
	#define SPI_S_CONF_DUMMY_08cycle  (8)
	#define SPI_S_CONF_DUMMY_10cycle  (10)
	#define SPI_S_CONF_DUMMY_12cycle  (12)
	#define SPI_S_CONF_DUMMY_14cycle  (14)
	
#define SPI_ERASE_BLOCK         (1)
#define SPI_ERASE_NONBLOCK      (0)

/* Debug */
//#define SPI_ERROR_MSG(x) OSAL_PRINTF x
#define SPI_ERROR_MSG(x) 

/* In SPI Status Register */
/* Write In Progress */
#define SPI_FLASH_WIP_SHIFT (0)
#define SPI_FLASH_WIP (1<<(SPI_FLASH_WIP_SHIFT))

/* Write Enable Latch*/
#define SPI_FLASH_WEL_SHIFT (1)
#define SPI_FLASH_WEL (1<<(SPI_FLASH_WEL_SHIFT))   


/* spi_iostatus */
#define IOSTATUS_CIO_MASK (0x00000038)
#define CMD_IO_MASK CMD_IO4
#define CMD_R_ADDR_MASK R_ADDR_IO4
#define CMD_W_ADDR_MASK W_ADDR_IO4
#define CMD_R_DATA_MASK R_DATA_IO4
#define CMD_W_DATA_MASK W_DATA_IO4
#define GET_CMD_IO(val)    (((val)&CMD_IO_MASK)>>6)
#define GET_R_ADDR_IO(val) (((val)&CMD_R_ADDR_MASK)>>9)
#define GET_W_ADDR_IO(val) (((val)&CMD_W_ADDR_MASK)>>12)
#define GET_R_DATA_IO(val)   (((val)&CMD_R_DATA_MASK)>>15)
#define GET_W_DATA_IO(val)   (((val)&CMD_W_DATA_MASK)>>18)

/* SPI Commands*/
typedef enum{
	SPI_C_QPP    = 0xfa,  /*pseudo command, Quad Page Program*/
	SPI_C_MREAD  = 0xfb,  /*pseudo command, Multi-IO Read*/
	SPI_C_EMIO   = 0xfd,  /*pseudo command, enter Multi-IO Mode*/
	SPI_C_RSTQIO = 0xFF,  /*SST Reset Quad IO*/
	SPI_C_READ   = 0x03,
	SPI_C_FREAD  = 0x0B,
	SPI_C_RDID   = 0x9F,
	SPI_C_4RDID  = 0xAF,  /*SST Quad ReadID*/
	SPI_C_WREN   = 0x06,
	SPI_C_WRDI   = 0x04,
	SPI_C_BE     = 0xD8,   
	SPI_C_CE     = 0xC7,
	SPI_C_SE     = 0x20,
	SPI_C_PP     = 0x02,
	SPI_C_RDSR   = 0x05,
	SPI_C_WRSR   = 0x01,
	SPI_C_WBPR   = 0x42,  /*SST Write Block-Protection Register*/
	SPI_C_DP     = 0xB9,
	SPI_C_RES    = 0xAB,
	SPI_C_AAI    = 0xAD   /*Auto Address Increment*/
}spi_cmdType_t;


/* IO Status */
typedef enum{
	IO1=(1<<0), /*IO#: chip supports IO width*/
	IO2=(1<<1),
	IO4=(1<<2),
	CIO1=(1<<3),/*IO#: current IO width mode*/
	CIO2=(1<<4),
	CIO4=(1<<5),
	CMD_IO1=((1)<<6),
	CMD_IO2=((2)<<6),
	CMD_IO4=((3)<<6),
	CMD_IO_Reserved=(1<<8),
	R_ADDR_IO1=((1)<<9),
	R_ADDR_IO2=((2)<<9),
	R_ADDR_IO4=((3)<<9),
	R_ADDR_IO_Reserved=(1<<11),
	W_ADDR_IO1=((1)<<12),
	W_ADDR_IO2=((2)<<12),
	W_ADDR_IO4=((3)<<12),
	W_ADDR_IO_Reserved=(1<<14),
	R_DATA_IO1=((1)<<15),
	R_DATA_IO2=((2)<<15),
	R_DATA_IO4=((3)<<15),
	R_DATA_IO_Reserved=(1<<17),
	W_DATA_IO1=((1)<<18),
	W_DATA_IO2=((2)<<18),
	W_DATA_IO4=((3)<<18),
	W_DATA_IO_Reserved=(1<<20),
	HAVE_EQ_CMD=(1<<29),
	QE_BIT=(1<<30),
	MODE_EN=(1<<31)    //enhance mode enable
}spi_iostatus;

/* SPI device structure */
typedef struct spi_chip_info_s{
	uint32  chip_id;
	uint32  chip_size;
	uint8   *chip_name;
	uint8   spi_io_config_r; // for 8051 sfr (read)
	uint8   spi_io_config_w; // for 8051 sfr (write)
	uint8   spi_ri_conf;
	uint8   spi_ri_cmd;
	uint8   spi_s_conf;
	uint8   spi_s_rcmd;
	uint8   spi_s_wcmd;
  #if CONFIG_SPI_QIO
	uint8	qio_read;     //qio read command
	uint8	qio_mode;     //enhance mode format
	uint8	qio_read_dummy; //dummy bytes(dummy_cycle/2)
	uint8	qio_pp;       //page program command  
	uint8	qio_eq;       //enter quad mode command
	uint8	qio_eq_dummy; //enter quad mode dummy
	uint8	qio_es;       //enter serial mode command
	
	uint8   qio_wqe_cmd;  //write quad enable bit command
	uint8	qio_qeb_loc;  //quad enable bit location
	uint8	qio_status_len; // bytes of status register which owns quad enable bit
	uint8   qio_io_config_r; // for 8051 sfr (read)
	uint8   qio_io_config_w; // for 8051 sfr (write)
  #endif /* end of CONFIG_SPI_QIO */
}spi_chip_info_t;


/*
 *  SPI flash chip ID
 */
 
/*   List of supported single I/O chip    */
/*  Spanson Flash  */
#define S25FL004A 0x00010212
#define S25FL016A 0x00010214
#define S25FL064A 0x00010216  /*supposed support*/
#define S25FL128P 0x00012018  /*only S25FL128P0XMFI001, Uniform  64KB secotr*/
                       /*not support S25FL128P0XMFI011, Uniform 256KB secotr*/
                       /*because #define SPI_BLOCK_SIZE 65536  */

/*  MXIC Flash  */
#define MX25L4005   0x00C22013
#define MX25L8005   0x00C22014
#define MX25L1605D  0x00C22015
#define MX25L3205D  0x00C22016  /*supposed support*/
#define MX25L6405D  0x00C22017
#define MX25L12805D 0x00C22018


/*  SST Flash  */
#define SST25VF032B 0x00BF254A


/*   List of supported Multi I/O chip    */
/*  Spanson Flash   */
/*  MXIC Flash      */
#define MX25L1635D  0x00C22415  /*supposed support*/
#define MX25L3235D  0x00C25E16  /*supposed support*/
/*  SST Flash       */
#define SST26VF016  0x00BF2601
#define SST26VF032  0x00BF2602
/*  WindBond Flash  */
#define W25Q40BV    0x00EF4013
#define W25Q80      0x00EF4014
#define W25Q16      0x00EF4015
#define W25Q32      0x00EF4016
/* Spansion Flash*/
#define S25FL032P   0x00010215

#define UNKNOW_SPIFLASH 0x00FFFFFF

#if FLASH_TYPE_SPI
    /* function prototype */
    #define spi_flash_driver_info() {OSAL_PRINTF("spi_flash_driver_entry=%p\n", spi_flash_driver_entry);}
    extern uint8 spi_flash_erase(uint32 erase_block_adr, uint8 block) large;
    extern uint8 spi_flash_write(uint32 start_adr, uint8 *buf, uint16 len) large;
    extern uint8 spi_flash_read(uint32 start_adr, uint8 *buf, uint16 len) large;
    #define set_IOmode(x)
#else  /* else of FLASH_TYPE_SPI */
    #define set_IOmode(x)
#endif /* end of else of FLASH_TYPE_SPI */


#ifdef  CONFIG_TO_USE_SCKU_SPI_DRIVER
    /* Entry point of SPI flash driver */
    typedef uint8 (spi_flash_driver_entry_t(uint16 arg));
    extern spi_flash_driver_entry_t *spi_flash_driver_entry;
    
    #if (defined(CONFIG_FOR_LOADER) || defined(CONFIG_STANDALONE))
        extern uint8 __spi_flash_drv_init(void) large;
        #define spi_flash_init() __spi_flash_drv_init()
    #else
        #define spi_flash_init() 
    #endif
    
    // debug only
    extern uint8 spi_flash_test(uint32 adr) large; 
    extern void dump_xdata_buf(uint8 *buf, uint16 len) large;
    #ifdef CONFIG_USE_FLASH_VIEWER
        extern uint8 spi_flash_viewer() large;
    #endif
    
    #define spi_chip0_chip_size         (CONFIG_DEFAULT_FLASH_SIZE)
    #define spi_chip0_chip_name         "Default"
    #define spi_chip0_spi_io_config_r   (SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SDD))
    #define spi_chip0_spi_io_config_w   (SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS))
#ifdef FLASH_MODE_DUAL    
    #define spi_chip0_spi_ri_conf       (SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_04cycle))
    #define spi_chip0_spi_ri_cmd        (0xBB)
    #define spi_chip0_spi_s_conf        (SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_04cycle))
    #define spi_chip0_spi_s_rcmd        (0xBB)
#else    
    #define spi_chip0_spi_ri_conf       (SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_08cycle))
    #define spi_chip0_spi_ri_cmd        (0x0B)
    #define spi_chip0_spi_s_conf        (SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_08cycle))
    #define spi_chip0_spi_s_rcmd        (0x0B)
#endif    
    #define spi_chip0_spi_s_wcmd        (M_SPI_C_PP)
    #define FLASH_PROTECT_REGION        (sizeof(flash_protect_area)/sizeof(uint32))
    
    #define SPI_FLASH_OP_READ           (1)
    #define SPI_FLASH_OP_WRITE          (2)
    #define SPI_FLASH_OP_ERASE          (3)
    #define SPI_FLASH_OP_ERASE_BLOCKING (4)
    #define SPI_FLASH_OP_INIT           (0xff)
    struct spi_write_flash_argument_t {
        uint32  f_addr;
        uint8   op;
        uint8   len;
        uint16  x_addr;
    };
    
    extern code uint32 flash_protect_area[];
    extern code uint8 num_flash_protected_area;
#endif  //CONFIG_TO_USE_SCKU_SPI_DRIVER


#endif /* end of __SPIFLASHDRV_H_ */
