#include <stdio.h>
#include "SPIFlashDrv.h"

//#pragma REGPARMS

#define SPI_WINBOND_SIO  (0)
#define SPI_WINBOND_DIO  (1)
#define SPI_WINBOND_QIO  (0)


#ifdef CONFIG_TO_USE_SCKU_SPI_DRIVER
// *********************************************
//              Shan-Chyun Ku's SPI Driver
// *********************************************

#ifdef FLASH_MODE_DUAL
#if (defined(CONFIG_FOR_LOADER) || defined(CONFIG_STANDALONE))
#define XCALL           LCALL
#define XJMP            LJMP

#define M_SPI_C_QPP     0xfa
#define M_SPI_C_MREAD   0xfb
#define M_SPI_C_EMIO    0xfd
#define M_SPI_C_RSTQIO  0xFF
#define M_SPI_C_READ    0x03
#define M_SPI_C_FREAD   0x0B
#define M_SPI_C_RDID    0x9F
#define M_SPI_C_4RDID   0xAF
#define M_SPI_C_WREN    0x06
#define M_SPI_C_WRDI    0x04
#define M_SPI_C_BE      0xD8
#define M_SPI_C_CE      0xC7
#define M_SPI_C_SE      0x20
#define M_SPI_C_PP      0x02
#define M_SPI_C_RDSR    0x05
#define M_SPI_C_WRSR    0x01
#define M_SPI_C_WBPR    0x42
#define M_SPI_C_DP      0xB9
#define M_SPI_C_RES     0xAB
#define M_SPI_C_AAI     0xAD

#define M_IO1           (1<<0)
#define M_IO2           (1<<1)
#define M_IO4           (1<<2)
#define M_CIO1          (1<<3)
#define M_CIO2          (1<<4)
#define M_CIO4          (1<<5)
#define M_CMD_IO1       ((1)<<6)
#define M_CMD_IO2       ((2)<<6)
#define M_CMD_IO4       ((3)<<6)
#define M_CMD_IO_Reserved (1<<8)
#define M_R_ADDR_IO1    ((1)<<9)
#define M_R_ADDR_IO2    ((2)<<9)
#define M_R_ADDR_IO4    ((3)<<9)
#define M_R_ADDR_IO_Reserved (1<<11)
#define M_W_ADDR_IO1    ((1)<<12)
#define M_W_ADDR_IO2    ((2)<<12)
#define M_W_ADDR_IO4    ((3)<<12)
#define M_W_ADDR_IO_Reserved (1<<14)
#define M_R_DATA_IO1    ((1)<<15)
#define M_R_DATA_IO2    ((2)<<15)
#define M_R_DATA_IO4    ((3)<<15)
#define M_R_DATA_IO_Reserved (1<<17)
#define M_W_DATA_IO1    ((1)<<18)
#define M_W_DATA_IO2    ((2)<<18)
#define M_W_DATA_IO4    ((3)<<18)
#define M_W_DATA_IO_Reserved (1<<20)
#define M_HAVE_EQ_CMD   (1<<29)
#define M_QE_BIT        (1<<30)
#define M_MODE_EN       (1<<31)

uint8 __spi_flash_drv_init(void) large;
static void __spi_flash_drv_entry(void) large;

uint8 __spi_flash_drv_init(void) large {
    spi_flash_driver_entry = (spi_flash_driver_entry_t*) &__spi_flash_drv_entry;
    
    SPI_DSEL_CYCLE = 0x09; /* 10 cycles */
    SPI_RI_CMD  = spi_chip0_spi_ri_cmd;
    SPI_RI_CONF = spi_chip0_spi_ri_conf;
    //SPI_S_CONF  = spi_chip0_spi_s_conf;           // be remarked due to override later
    //SPI_S_RCMD  = spi_chip0_spi_s_rcmd;           // be remarked due to override later
    //SPI_IO_CONF = spi_chip0_spi_io_config_r;      // be remarked due to override later

    /* Check Controller Ready */
    while( 1==SPI_CTRL_BUSY );
    
    /* Set sfr for ReadSR */
    SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
    SPI_IO_CONF   = SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS);
    SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(1) | SPI_S_ACCESS_WRITE | SPI_S_ACCESS_SKIP_ADR;
    SPI_S_WCMD    = SPI_C_WRSR;
    SPI_S_DATA0   = 0;                              // new_value=0
    SPI_CMD_ISSUE = 1;
    
    /* Check Controller Ready */
    while( 1==SPI_CTRL_BUSY );
    
#pragma asm
    //__spi_flash_drv_restore_S_mode_reg();
    LCALL   __spi_flash_drv_restore_S_mode_reg;
#pragma endasm
    return SUCCESS;
}

static void __spi_flash_drv_entry(void) large {
    // It should be uint8 __spi_flash_drv_entry(uint16 argx), but I have to 
    // trick C51, which generates the unnecessary to access XDATA
    // now arg is in R6/R7

#pragma asm
//  struct spi_write_flash_argument_t * xdata arg=(struct spi_write_flash_argument_t *)argx;
//  if (arg==NULL) return FALSE;
    MOV     A, R7
    MOV     DPL, A
    MOV     A, R6
    MOV     DPH, A
    ORL     A, R7
    JZ      ?0034

//  SPI_S_CONF  = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
    MOV     SPI_S_CONF, #(SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle))

//  SPI_IO_CONF = spi_chip0_spi_io_config_w;
    MOV     SPI_IO_CONF, #(spi_chip0_spi_io_config_w)

//  uint24 start_adr = arg->f_addr; (R0:MSB/R1/R2:LSB)
    INC     DPTR            // skip first byte of f_addr
    MOVX    A, @DPTR
    MOV     R0, A
    INC     DPTR
    MOVX    A, @DPTR
    MOV     R1, A
    INC     DPTR
    MOVX    A, @DPTR
    MOV     R2, A

// uint8 op = arg->op; (R5)
    INC     DPTR
    MOVX    A, @DPTR
    MOV     R5, A
    
    CJNE    R5, #(SPI_FLASH_OP_ERASE), ?0030
    XJMP    __spi_flash_driver_erase
?0030:
    CJNE    R5, #(SPI_FLASH_OP_ERASE_BLOCKING), ?0031
    XJMP    __spi_flash_driver_erase
?0031:
    CJNE    R5, #(SPI_FLASH_OP_INIT), ?0032
    LJMP    __spi_flash_drv_init
?0032:
    
//  uint8 len=arg->len; (R3)
    INC     DPTR
    MOVX    A, @DPTR
    MOV     R3, A

//  uint8 *buf=(uint8*)arg->x_addr; (DPTR)
    INC     DPTR
    MOVX    A, @DPTR
    MOV     R7, A
    INC     DPTR
    MOVX    A, @DPTR
    MOV     DPL, A
    MOV     A, R7
    MOV     DPH, A

    CJNE    R5, #(SPI_FLASH_OP_WRITE), ?0033
    SJMP    __spi_flash_driver_write
?0033:
    CJNE    R5, #(SPI_FLASH_OP_READ), ?0034
    XJMP    __spi_flash_driver_read
?0034:
    
    //  return FALSE;
    MOV     R7, #FALSE
__spi_flash_drv_restore_S_mode_reg:
//  SPI_IO_CONF =   spi_chip0_spi_io_config_r;
//  SPI_S_RCMD =    spi_chip0_spi_s_rcmd;
//  SPI_S_CONF =    spi_chip0_spi_s_conf; 
    MOV     SPI_IO_CONF, #(spi_chip0_spi_io_config_r)
    MOV     SPI_S_RCMD, #(spi_chip0_spi_s_rcmd)
    MOV     SPI_S_CONF, #(spi_chip0_spi_s_conf)
    RET

__spi_flash_readSR:
//  ****Check Controller Ready 
//  while( 1==SPI_CTRL_BUSY );
?0006:
    JB      SPI_CTRL_BUSY,?0006

    // assume:
    //  SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
    //  SPI_IO_CONF   = (SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS));

//  SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(1) | SPI_S_ACCESS_READ |  SPI_S_ACCESS_SKIP_ADR;
    MOV     SPI_S_ACCESS, #(SPI_S_ACCESS_BYTE(1) | SPI_S_ACCESS_READ |  SPI_S_ACCESS_SKIP_ADR)

//  SPI_S_RCMD    = SPI_C_RDSR;
    MOV     SPI_S_RCMD, #(M_SPI_C_RDSR)

//  SPI_CMD_ISSUE = 1;
    SETB    SPI_CMD_ISSUE

//  ****Check Controller Ready 
//  while( 1==SPI_CTRL_BUSY );
?0008:
    JB      SPI_CTRL_BUSY,?0008
    
//  return SPI_S_DATA0;
    MOV     R7, SPI_S_DATA0
    RET

__spi_flash_waitRdy:
//  while( __spi_flash_readSR()&SPI_FLASH_WIP );
?0009:
    XCALL   __spi_flash_readSR
    MOV     A,R7
    JB      ACC.0,?0009
    RET

__spi_flash_waitWEL:
//  __spi_flash_waitRdy();
    XCALL   __spi_flash_waitRdy

    // assume:
    //  SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
    //  SPI_IO_CONF   = (SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS));

//  SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(0) | SPI_S_ACCESS_WRITE | SPI_S_ACCESS_SKIP_ADR;
    MOV     SPI_S_ACCESS, #(SPI_S_ACCESS_BYTE(0) | SPI_S_ACCESS_WRITE | SPI_S_ACCESS_SKIP_ADR)

//  SPI_S_WCMD    = SPI_C_WREN;
    MOV     SPI_S_WCMD, #(M_SPI_C_WREN)

//  SPI_CMD_ISSUE = 1;
    SETB    SPI_CMD_ISSUE

//  while (!(__spi_flash_readSR()&SPI_FLASH_WEL));
?000A:
    XCALL   __spi_flash_readSR
    MOV     A, R7
    JNB     ACC.(SPI_FLASH_WEL_SHIFT), ?000A
    RET
    
__spi_flash_driver_write:
    // uint24 start_adr in (R0/R1/R2)
    // uint8 len is (R3)
    // uint8 *buf is (DPTR)
//  while (1){
?000D:
//      __spi_flash_waitWEL();
        XCALL   __spi_flash_waitWEL

        // assume:
        //  SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
        //  SPI_IO_CONF   = (SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS));

//      SPI_S_WCMD  = spi_chip0_spi_s_wcmd;
        MOV     SPI_S_WCMD, #(spi_chip0_spi_s_wcmd)

//      clen=4; to_quit=0;  to_quit is in (R5)
//      if (!((len>4)||(len==0))) {
//          clen=len; to_quit++;
//      }
//      SPI_S_ACCESS = clen  | SPI_S_ACCESS_WRITE ;
        MOV     SPI_S_ACCESS, #(0x04|(SPI_S_ACCESS_WRITE))
        MOV     R5, #0
        MOV     A, R3
        JZ      ?000E
	ADD 	A,#0xFB
	JC      ?000E
	MOV     A, #(SPI_S_ACCESS_WRITE)
	ORL     A, R3
        MOV     SPI_S_ACCESS, A
        INC     R5
?000E:

//      SPI_S_ADDR2 = (uint8)( start_adr>>16);
//      SPI_S_ADDR1 = (uint8)((start_adr>> 8)&0x0000FF);
//      SPI_S_ADDR0 = (uint8)((start_adr>> 0)&0x0000FF);
        MOV     A, R0
        MOV     SPI_S_ADDR2, A
        MOV     A, R1
        MOV     SPI_S_ADDR1, A
        MOV     A, R2
        MOV     SPI_S_ADDR0, A

//      SPI_S_DATA0 = *buf++;
//      SPI_S_DATA1 = *buf++;
//      SPI_S_DATA2 = *buf++;
//      SPI_S_DATA3 = *buf++;
        MOVX    A, @DPTR
        MOV     SPI_S_DATA0, A
        INC     DPTR
        MOVX    A, @DPTR
        MOV     SPI_S_DATA1, A
        INC     DPTR
        MOVX    A, @DPTR
        MOV     SPI_S_DATA2, A
        INC     DPTR
        MOVX    A, @DPTR
        MOV     SPI_S_DATA3, A
        INC     DPTR

//      SPI_CMD_ISSUE = 1;
        SETB    SPI_CMD_ISSUE

//      if (to_quit) break;
        MOV     A, R5
        JNZ     ?0010
        
//      len -= 4;
        MOV     A, #(0xFC)
        ADD     A, R3
        MOV     R3, A

//      start_adr +=4;
        MOV     A, #(0x04)
        ADD     A, R2
        MOV     R2, A
        CLR     A
        ADDC    A, R1
        MOV     R1, A
        CLR     A
        ADDC    A, R0
        MOV     R0, A
//  }
    SJMP    ?000D

//    
?0010:
//  __spi_flash_waitRdy();
    XCALL   __spi_flash_waitRdy

//  __spi_flash_drv_restore_S_mode_reg(); 
//  return SUCCESS
    MOV     R7, #SUCCESS
    XJMP    __spi_flash_drv_restore_S_mode_reg;

__spi_flash_driver_read:
    // uint24 start_adr in (R0/R1/R2)
    // uint8 len is (R3)
    // uint8 *buf is (DPTR)

//  __spi_flash_waitRdy();
    XCALL   __spi_flash_waitRdy
    
//  __spi_flash_drv_restore_S_mode_reg();
//      to get SPI_S_RCMD, SPI_S_CONF, and SPI_IO_CONF
    XCALL   __spi_flash_drv_restore_S_mode_reg
    
//  SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(4) | SPI_S_ACCESS_READ ;
    MOV     SPI_S_ACCESS, #(SPI_S_ACCESS_BYTE(4) | SPI_S_ACCESS_READ)
   
//  while(1) {
?0021:
//      SPI_S_ADDR2 = (uint8)( start_adr>>16);
//      SPI_S_ADDR1 = (uint8)((start_adr>> 8)&0x0000FF);
//      SPI_S_ADDR0 = (uint8)((start_adr>> 0)&0x0000FF);
        MOV     A, R0
        MOV     SPI_S_ADDR2, A
        MOV     A, R1
        MOV     SPI_S_ADDR1, A
        MOV     A, R2
        MOV     SPI_S_ADDR0, A
    
//      SPI_CMD_ISSUE = 1;
        SETB    SPI_CMD_ISSUE

//      while( 1==SPI_CTRL_BUSY );
?0022:
        JB      SPI_CTRL_BUSY, ?0022
            
//      *buf++   = SPI_S_DATA0;
//      if ((--len)==0) break;
        MOV     A, SPI_S_DATA0
        MOVX    @DPTR, A
        INC     DPTR
        DEC     R3
        MOV     A, R3
        JZ      ?0020

//      *buf++   = SPI_S_DATA1;
//      if ((--len)==0) break;
        MOV     A, SPI_S_DATA1
        MOVX    @DPTR, A
        INC     DPTR
        DEC     R3
        MOV     A, R3
        JZ      ?0020

//      *buf++   = SPI_S_DATA2;
//      if ((--len)==0) break;
        MOV     A, SPI_S_DATA2
        MOVX    @DPTR, A
        INC     DPTR
        DEC     R3
        MOV     A, R3
        JZ      ?0020

//      *buf++   = SPI_S_DATA3;
//      if ((--len==0)) break;
        MOV     A, SPI_S_DATA3
        MOVX    @DPTR, A
        INC     DPTR
        DEC     R3
        MOV     A, R3
        JZ      ?0020

//      start_adr += 4;
        MOV     A, #(0x04)
        ADD     A, R2
        MOV     R2, A
        CLR     A
        ADDC    A, R1
        MOV     R1, A
        CLR     A
        ADDC    A, R0
        MOV     R0, A

//  }
    SJMP    ?0021

?0020:
    MOV     R7, #SUCCESS
    RET    

__spi_flash_driver_erase:
    // erase_block_adr is in (R0/R1/R2)
    // blocking mode is in (R5)
    
//  __spi_flash_waitWEL();
    XCALL   __spi_flash_waitWEL

    // assume:
    //  SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
    //  SPI_IO_CONF   = (SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS));

//  SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(0) | SPI_S_ACCESS_WRITE ;
    MOV     SPI_S_ACCESS, #(SPI_S_ACCESS_BYTE(0) | SPI_S_ACCESS_WRITE)

//  SPI_S_WCMD    = SPI_C_SE;
    MOV     SPI_S_WCMD, #(M_SPI_C_SE)

//  SPI_S_ADDR2   = (uint8)((erase_block_adr >> 16)&0xff);
    MOV     A,R0
    MOV     SPI_S_ADDR2, A

//  SPI_S_ADDR1   = (uint8)((erase_block_adr >> 8)&0xff);
    MOV     A,R1
    MOV     SPI_S_ADDR1,A

//  SPI_S_ADDR1   = (uint8)((erase_block_adr)&0xff);
    MOV     A,R2
    MOV     SPI_S_ADDR0,A

//  SPI_CMD_ISSUE = 1;
    SETB    SPI_CMD_ISSUE

//  if (op(R5)==SPI_FLASH_OP_ERASE_BLOCKING) __spi_flash_waitRdy();
    MOV     A, R5
    CJNE    A, #(SPI_FLASH_OP_ERASE_BLOCKING), ?000B
    XCALL   __spi_flash_waitRdy
?000B:

//  __spi_flash_drv_restore_S_mode_reg();
    MOV     R7, #(TRUE)
    XJMP    __spi_flash_drv_restore_S_mode_reg
#pragma endasm
}   // entry
#endif  // if (defined(CONFIG_FOR_LOADER) || defined(CONFIG_STANDALONE))
#else //FLASH_MODE_DUAL
#if (defined(CONFIG_FOR_LOADER) || defined(CONFIG_STANDALONE))
#define XCALL           LCALL
#define XJMP            LJMP

#define M_SPI_C_QPP     0xfa
#define M_SPI_C_MREAD   0xfb
#define M_SPI_C_EMIO    0xfd
#define M_SPI_C_RSTQIO  0xFF
#define M_SPI_C_READ    0x03
#define M_SPI_C_FREAD   0x0B
#define M_SPI_C_RDID    0x9F
#define M_SPI_C_4RDID   0xAF
#define M_SPI_C_WREN    0x06
#define M_SPI_C_WRDI    0x04
#define M_SPI_C_BE      0xD8
#define M_SPI_C_CE      0xC7
#define M_SPI_C_SE      0x20
#define M_SPI_C_PP      0x02
#define M_SPI_C_RDSR    0x05
#define M_SPI_C_WRSR    0x01
#define M_SPI_C_WBPR    0x42
#define M_SPI_C_DP      0xB9
#define M_SPI_C_RES     0xAB
#define M_SPI_C_AAI     0xAD

#define M_IO1           (1<<0)
#define M_IO2           (1<<1)
#define M_IO4           (1<<2)
#define M_CIO1          (1<<3)
#define M_CIO2          (1<<4)
#define M_CIO4          (1<<5)
#define M_CMD_IO1       ((1)<<6)
#define M_CMD_IO2       ((2)<<6)
#define M_CMD_IO4       ((3)<<6)
#define M_CMD_IO_Reserved (1<<8)
#define M_R_ADDR_IO1    ((1)<<9)
#define M_R_ADDR_IO2    ((2)<<9)
#define M_R_ADDR_IO4    ((3)<<9)
#define M_R_ADDR_IO_Reserved (1<<11)
#define M_W_ADDR_IO1    ((1)<<12)
#define M_W_ADDR_IO2    ((2)<<12)
#define M_W_ADDR_IO4    ((3)<<12)
#define M_W_ADDR_IO_Reserved (1<<14)
#define M_R_DATA_IO1    ((1)<<15)
#define M_R_DATA_IO2    ((2)<<15)
#define M_R_DATA_IO4    ((3)<<15)
#define M_R_DATA_IO_Reserved (1<<17)
#define M_W_DATA_IO1    ((1)<<18)
#define M_W_DATA_IO2    ((2)<<18)
#define M_W_DATA_IO4    ((3)<<18)
#define M_W_DATA_IO_Reserved (1<<20)
#define M_HAVE_EQ_CMD   (1<<29)
#define M_QE_BIT        (1<<30)
#define M_MODE_EN       (1<<31)

uint8 __spi_flash_drv_init(void) large;
static void __spi_flash_drv_entry(void) large;

uint8 __spi_flash_drv_init(void) large {
    spi_flash_driver_entry = (spi_flash_driver_entry_t*) &__spi_flash_drv_entry;
    
    SPI_DSEL_CYCLE = 0x09; /* 10 cycles */
    SPI_RI_CMD  = spi_chip0_spi_ri_cmd;
    SPI_RI_CONF = spi_chip0_spi_ri_conf;
    //SPI_S_CONF  = spi_chip0_spi_s_conf;           // be remarked due to override later
    //SPI_S_RCMD  = spi_chip0_spi_s_rcmd;           // be remarked due to override later
    //SPI_IO_CONF = spi_chip0_spi_io_config_r;      // be remarked due to override later

    /* Check Controller Ready */
    while( 1==SPI_CTRL_BUSY );
    
    /* Set sfr for ReadSR */
    SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_08cycle);
    SPI_IO_CONF   = SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS);
    SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(1) | SPI_S_ACCESS_WRITE | SPI_S_ACCESS_SKIP_ADR;
    SPI_S_WCMD    = SPI_C_WRSR;
    SPI_S_DATA0   = 0;                              // new_value=0
    SPI_CMD_ISSUE = 1;
    
    /* Check Controller Ready */
    while( 1==SPI_CTRL_BUSY );
    
#pragma asm
    //__spi_flash_drv_restore_S_mode_reg();
    LCALL   __spi_flash_drv_restore_S_mode_reg;
#pragma endasm
    return SUCCESS;
}

static void __spi_flash_drv_entry(void) large {
    // It should be uint8 __spi_flash_drv_entry(uint16 argx), but I have to 
    // trick C51, which generates the unnecessary to access XDATA
    // now arg is in R6/R7

#pragma asm
//  struct spi_write_flash_argument_t * xdata arg=(struct spi_write_flash_argument_t *)argx;
//  if (arg==NULL) return FALSE;
    MOV     A, R7
    MOV     DPL, A
    MOV     A, R6
    MOV     DPH, A
    ORL     A, R7
    JZ      ?0034

//  SPI_S_CONF  = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
    MOV     SPI_S_CONF, #(SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_08cycle))

//  SPI_IO_CONF = spi_chip0_spi_io_config_w;
    MOV     SPI_IO_CONF, #(spi_chip0_spi_io_config_w)

//  uint24 start_adr = arg->f_addr; (R0:MSB/R1/R2:LSB)
    INC     DPTR            // skip first byte of f_addr
    MOVX    A, @DPTR
    MOV     R0, A
    INC     DPTR
    MOVX    A, @DPTR
    MOV     R1, A
    INC     DPTR
    MOVX    A, @DPTR
    MOV     R2, A

// uint8 op = arg->op; (R5)
    INC     DPTR
    MOVX    A, @DPTR
    MOV     R5, A
    
    CJNE    R5, #(SPI_FLASH_OP_ERASE), ?0030
    XJMP    __spi_flash_driver_erase
?0030:
    CJNE    R5, #(SPI_FLASH_OP_ERASE_BLOCKING), ?0031
    XJMP    __spi_flash_driver_erase
?0031:
    CJNE    R5, #(SPI_FLASH_OP_INIT), ?0032
    LJMP    __spi_flash_drv_init
?0032:
    
//  uint8 len=arg->len; (R3)
    INC     DPTR
    MOVX    A, @DPTR
    MOV     R3, A

//  uint8 *buf=(uint8*)arg->x_addr; (DPTR)
    INC     DPTR
    MOVX    A, @DPTR
    MOV     R7, A
    INC     DPTR
    MOVX    A, @DPTR
    MOV     DPL, A
    MOV     A, R7
    MOV     DPH, A

    CJNE    R5, #(SPI_FLASH_OP_WRITE), ?0033
    SJMP    __spi_flash_driver_write
?0033:
    CJNE    R5, #(SPI_FLASH_OP_READ), ?0034
    XJMP    __spi_flash_driver_read
?0034:
    
    //  return FALSE;
    MOV     R7, #FALSE
__spi_flash_drv_restore_S_mode_reg:
//  SPI_IO_CONF =   spi_chip0_spi_io_config_r;
//  SPI_S_RCMD =    spi_chip0_spi_s_rcmd;
//  SPI_S_CONF =    spi_chip0_spi_s_conf; 
    MOV     SPI_IO_CONF, #(spi_chip0_spi_io_config_w)
    MOV     SPI_S_RCMD, #(spi_chip0_spi_s_rcmd)
    MOV     SPI_S_CONF, #(spi_chip0_spi_s_conf)
    RET

__spi_flash_readSR:
//  ****Check Controller Ready 
//  while( 1==SPI_CTRL_BUSY );
?0006:
    JB      SPI_CTRL_BUSY,?0006

    // assume:
    //  SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
    //  SPI_IO_CONF   = (SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS));

//  SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(1) | SPI_S_ACCESS_READ |  SPI_S_ACCESS_SKIP_ADR;
    MOV     SPI_S_ACCESS, #(SPI_S_ACCESS_BYTE(1) | SPI_S_ACCESS_READ |  SPI_S_ACCESS_SKIP_ADR)

//  SPI_S_RCMD    = SPI_C_RDSR;
    MOV     SPI_S_RCMD, #(M_SPI_C_RDSR)

//  SPI_CMD_ISSUE = 1;
    SETB    SPI_CMD_ISSUE

//  ****Check Controller Ready 
//  while( 1==SPI_CTRL_BUSY );
?0008:
    JB      SPI_CTRL_BUSY,?0008
    
//  return SPI_S_DATA0;
    MOV     R7, SPI_S_DATA0
    RET

__spi_flash_waitRdy:
//  while( __spi_flash_readSR()&SPI_FLASH_WIP );
?0009:
    XCALL   __spi_flash_readSR
    MOV     A,R7
    JB      ACC.0,?0009
    RET

__spi_flash_waitWEL:
//  __spi_flash_waitRdy();
    XCALL   __spi_flash_waitRdy

    // assume:
    MOV SPI_S_CONF, #(SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle))
    MOV SPI_IO_CONF, #(SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS))
    //  SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
    //  SPI_IO_CONF   = (SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS));

//  SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(0) | SPI_S_ACCESS_WRITE | SPI_S_ACCESS_SKIP_ADR;
    MOV     SPI_S_ACCESS, #(SPI_S_ACCESS_BYTE(0) | SPI_S_ACCESS_WRITE | SPI_S_ACCESS_SKIP_ADR)

//  SPI_S_WCMD    = SPI_C_WREN;
    MOV     SPI_S_WCMD, #(M_SPI_C_WREN)

//  SPI_CMD_ISSUE = 1;
    SETB    SPI_CMD_ISSUE

//  while (!(__spi_flash_readSR()&SPI_FLASH_WEL));
?000A:
    XCALL   __spi_flash_readSR
    MOV     A, R7
    JNB     ACC.(SPI_FLASH_WEL_SHIFT), ?000A
    RET
    
__spi_flash_driver_write:
    // uint24 start_adr in (R0/R1/R2)
    // uint8 len is (R3)
    // uint8 *buf is (DPTR)
//  while (1){
?000D:
//      __spi_flash_waitWEL();
        XCALL   __spi_flash_waitWEL

        // assume:
        MOV SPI_S_CONF, #(SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle))
        MOV SPI_IO_CONF, #(SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS))
        //  SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
        //  SPI_IO_CONF   = (SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS));

//      SPI_S_WCMD  = spi_chip0_spi_s_wcmd;
        MOV     SPI_S_WCMD, #(spi_chip0_spi_s_wcmd)

//      clen=4; to_quit=0;  to_quit is in (R5)
//      if (!((len>4)||(len==0))) {
//          clen=len; to_quit++;
//      }
//      SPI_S_ACCESS = clen  | SPI_S_ACCESS_WRITE ;
        MOV     SPI_S_ACCESS, #(0x04|(SPI_S_ACCESS_WRITE))
        MOV     R5, #0
        MOV     A, R3
        JZ      ?000E
	ADD 	A,#0xFB
	JC      ?000E
	MOV     A, #(SPI_S_ACCESS_WRITE)
	ORL     A, R3
        MOV     SPI_S_ACCESS, A
        INC     R5
?000E:

//      SPI_S_ADDR2 = (uint8)( start_adr>>16);
//      SPI_S_ADDR1 = (uint8)((start_adr>> 8)&0x0000FF);
//      SPI_S_ADDR0 = (uint8)((start_adr>> 0)&0x0000FF);
        MOV     A, R0
        MOV     SPI_S_ADDR2, A
        MOV     A, R1
        MOV     SPI_S_ADDR1, A
        MOV     A, R2
        MOV     SPI_S_ADDR0, A

//      SPI_S_DATA0 = *buf++;
//      SPI_S_DATA1 = *buf++;
//      SPI_S_DATA2 = *buf++;
//      SPI_S_DATA3 = *buf++;
        MOVX    A, @DPTR
        MOV     SPI_S_DATA0, A
        INC     DPTR
        MOVX    A, @DPTR
        MOV     SPI_S_DATA1, A
        INC     DPTR
        MOVX    A, @DPTR
        MOV     SPI_S_DATA2, A
        INC     DPTR
        MOVX    A, @DPTR
        MOV     SPI_S_DATA3, A
        INC     DPTR

//      SPI_CMD_ISSUE = 1;
        SETB    SPI_CMD_ISSUE

//      if (to_quit) break;
        MOV     A, R5
        JNZ     ?0010
        
//      len -= 4;
        MOV     A, #(0xFC)
        ADD     A, R3
        MOV     R3, A

//      start_adr +=4;
        MOV     A, #(0x04)
        ADD     A, R2
        MOV     R2, A
        CLR     A
        ADDC    A, R1
        MOV     R1, A
        CLR     A
        ADDC    A, R0
        MOV     R0, A
//  }
    SJMP    ?000D

//    
?0010:
//  __spi_flash_waitRdy();
    XCALL   __spi_flash_waitRdy

//  __spi_flash_drv_restore_S_mode_reg(); 
//  return SUCCESS
    MOV     R7, #SUCCESS
    XJMP    __spi_flash_drv_restore_S_mode_reg;

__spi_flash_driver_read:
    // uint24 start_adr in (R0/R1/R2)
    // uint8 len is (R3)
    // uint8 *buf is (DPTR)

//  __spi_flash_waitRdy();
    XCALL   __spi_flash_waitRdy
    
//  __spi_flash_drv_restore_S_mode_reg();
//      to get SPI_S_RCMD, SPI_S_CONF, and SPI_IO_CONF
    XCALL   __spi_flash_drv_restore_S_mode_reg
    
//  SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(4) | SPI_S_ACCESS_READ ;
    MOV     SPI_S_ACCESS, #(SPI_S_ACCESS_BYTE(4) | SPI_S_ACCESS_READ)
   
//  while(1) {
?0021:
//      SPI_S_ADDR2 = (uint8)( start_adr>>16);
//      SPI_S_ADDR1 = (uint8)((start_adr>> 8)&0x0000FF);
//      SPI_S_ADDR0 = (uint8)((start_adr>> 0)&0x0000FF);
        MOV     A, R0
        MOV     SPI_S_ADDR2, A
        MOV     A, R1
        MOV     SPI_S_ADDR1, A
        MOV     A, R2
        MOV     SPI_S_ADDR0, A
    
//      SPI_CMD_ISSUE = 1;
        SETB    SPI_CMD_ISSUE

//      while( 1==SPI_CTRL_BUSY );
?0022:
        JB      SPI_CTRL_BUSY, ?0022
            
//      *buf++   = SPI_S_DATA0;
//      if ((--len)==0) break;
        MOV     A, SPI_S_DATA0
        MOVX    @DPTR, A
        INC     DPTR
        DEC     R3
        MOV     A, R3
        JZ      ?0020

//      *buf++   = SPI_S_DATA1;
//      if ((--len)==0) break;
        MOV     A, SPI_S_DATA1
        MOVX    @DPTR, A
        INC     DPTR
        DEC     R3
        MOV     A, R3
        JZ      ?0020

//      *buf++   = SPI_S_DATA2;
//      if ((--len)==0) break;
        MOV     A, SPI_S_DATA2
        MOVX    @DPTR, A
        INC     DPTR
        DEC     R3
        MOV     A, R3
        JZ      ?0020

//      *buf++   = SPI_S_DATA3;
//      if ((--len==0)) break;
        MOV     A, SPI_S_DATA3
        MOVX    @DPTR, A
        INC     DPTR
        DEC     R3
        MOV     A, R3
        JZ      ?0020

//      start_adr += 4;
        MOV     A, #(0x04)
        ADD     A, R2
        MOV     R2, A
        CLR     A
        ADDC    A, R1
        MOV     R1, A
        CLR     A
        ADDC    A, R0
        MOV     R0, A

//  }
    SJMP    ?0021

?0020:
    MOV     R7, #SUCCESS
    RET    

__spi_flash_driver_erase:
    // erase_block_adr is in (R0/R1/R2)
    // blocking mode is in (R5)
    
//  __spi_flash_waitWEL();
    XCALL   __spi_flash_waitWEL

    // assume:
    MOV SPI_S_CONF, #(SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle))
    MOV SPI_IO_CONF, #((SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS)))  
    //  SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
    //  SPI_IO_CONF   = (SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS));

//  SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(0) | SPI_S_ACCESS_WRITE ;
    MOV     SPI_S_ACCESS, #(SPI_S_ACCESS_BYTE(0) | SPI_S_ACCESS_WRITE)

//  SPI_S_WCMD    = SPI_C_SE;
    MOV     SPI_S_WCMD, #(M_SPI_C_SE)

//  SPI_S_ADDR2   = (uint8)((erase_block_adr >> 16)&0xff);
    MOV     A,R0
    MOV     SPI_S_ADDR2, A

//  SPI_S_ADDR1   = (uint8)((erase_block_adr >> 8)&0xff);
    MOV     A,R1
    MOV     SPI_S_ADDR1,A

//  SPI_S_ADDR1   = (uint8)((erase_block_adr)&0xff);
    MOV     A,R2
    MOV     SPI_S_ADDR0,A

//  SPI_CMD_ISSUE = 1;
    SETB    SPI_CMD_ISSUE

//  if (op(R5)==SPI_FLASH_OP_ERASE_BLOCKING) __spi_flash_waitRdy();
    MOV     A, R5
    CJNE    A, #(SPI_FLASH_OP_ERASE_BLOCKING), ?000B
    XCALL   __spi_flash_waitRdy
?000B:

//  __spi_flash_drv_restore_S_mode_reg();
    MOV     R7, #(TRUE)
    XJMP    __spi_flash_drv_restore_S_mode_reg
#pragma endasm
}   // entry

#endif  // if (defined(CONFIG_FOR_LOADER) || defined(CONFIG_STANDALONE))

#endif









#else   // END OF CONFIG_TO_USE_SCKU_SPI_DRIVER: 

// Using the original SPI-Driver (not scku's version)

#if 0==CONFIG_FLASH_PROBE
/*
 *  Set spi_chips as constant can reduce 
 *  the data segment size (For flash only)
 */
static const spi_chip_info_t xdata spi_chips[] = {
        {
                UNKNOW_SPIFLASH,                            /* chip_id */
                0x00080000U ,                               /* chip_size, 512KB */
                "UNKNOW_SPIFLASH",                          /* chip_name */
                SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS),     /* spi_io_config_r */
                SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS),     /* spi_io_config_w */
                SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_08cycle), /* spi_ri_conf */
                SPI_C_FREAD,                                /* spi_ri_cmd */
                SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_08cycle), /* spi_s_conf */
                SPI_C_FREAD,                                /* spi_s_rcmd */
                SPI_C_PP                                    /* spi_s_wcmd */
        },
        {
                W25Q32,                                     /* chip_id */
                0x00400000U ,                               /* chip_size, 4MB */
                "W25Q32",                                   /* chip_name */
                SPI_IO_CONF_MHZ(SPI_IO_CONF_75MHZ)          /* spi_io_config_r */ \
                    | SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SDD),
                SPI_IO_CONF_MHZ(SPI_IO_CONF_75MHZ)          /* spi_io_config_w */ \
                    | SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS),
                SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_04cycle), /* spi_ri_conf */
                0xBB,                                       /* spi_ri_cmd */
                SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_04cycle), /* spi_s_conf */
                0xBB,                                       /* spi_s_rcmd */
                SPI_C_PP                                    /* spi_s_wcmd */
        },
        {
                MX25L1635D,                                 /* chip_id */
                0x00200000U ,                               /* chip_size, 2MB */
                "MX25L1635D",                                   /* chip_name */
                SPI_IO_CONF_MHZ(SPI_IO_CONF_75MHZ)          /* spi_io_config_r */ \
                    | SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS),
                SPI_IO_CONF_MHZ(SPI_IO_CONF_75MHZ)          /* spi_io_config_w */ \
                    | SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS),
                SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle), /* spi_ri_conf */
                SPI_C_READ,                                 /* spi_ri_cmd */
                SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle), /* spi_s_conf */
                SPI_C_READ,                                 /* spi_s_rcmd */
                SPI_C_PP                                    /* spi_s_wcmd */
        },
        
};
#endif /* end of 0==CONFIG_FLASH_PROBE */

spi_chip_info_t xdata spi_chip0;


/* Redefine spi_chip0 as this_chip .
 * - All erase/read/write work on spi_chip0 in RTL8367C/70 (single chip)
 * - this_chip can be redefine or pass as a function paramter
 *   to select spi_chip0/1/2/...
 */
#define this_chip spi_chip0

static uint8 __spi_flash_wrsr(uint8 new_value) large;

void spi_flash_init(void) large {
        SPI_DSEL_CYCLE = 0x09; /* 10 cycles */
#if 0==CONFIG_FLASH_PROBE
        spi_chip0.chip_size       = CONFIG_DEFAULT_FLASH_SIZE;
        spi_chip0.chip_name       = "Default";
        spi_chip0.spi_io_config_r = SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SDD);
        spi_chip0.spi_io_config_w = SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS);
        spi_chip0.spi_ri_conf     = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_04cycle);
        spi_chip0.spi_ri_cmd      = 0xBB;
        spi_chip0.spi_s_conf      = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_04cycle);
        spi_chip0.spi_s_rcmd      = 0xBB;
        spi_chip0.spi_s_wcmd      = SPI_C_PP;
                        
        SPI_RI_CMD  = spi_chip0.spi_ri_cmd;
        SPI_RI_CONF = spi_chip0.spi_ri_conf;
        SPI_S_CONF  = spi_chip0.spi_s_conf;
        SPI_S_RCMD  = spi_chip0.spi_s_rcmd;
        SPI_IO_CONF = spi_chip0.spi_io_config_r;
#else /* else of 0==CONFIG_FLASH_PROBE */
        int xdata i = 0;
        uint16 xdata size_in_kb;
        uint16 xdata *print_ptr;
        SPI_ERROR_MSG(("[SPI Flash Init]\n"));
        
        /* Check Controller Ready */
        while( 1==SPI_CTRL_BUSY );
                
        /* Read ID */
        SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
        SPI_IO_CONF   = SPI_IO_CONF_MHZ(SPI_IO_CONF_20_8MHZ);
        SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(3) | SPI_S_ACCESS_READ | \
                        SPI_S_ACCESS_SKIP_ADR;
        SPI_S_RCMD    = SPI_C_RDID;
        
        SPI_CMD_ISSUE = 1;
        
        /* Check Controller Ready */
        while( 1==SPI_CTRL_BUSY );
        spi_chip0.chip_id  = ((uint32)SPI_S_DATA0)<<16;
        spi_chip0.chip_id |= ((uint32)SPI_S_DATA1)<< 8;
        spi_chip0.chip_id |= ((uint32)SPI_S_DATA2)<< 0;
        
        spi_chip0.chip_size = 0;
        
        for( i=0;i<sizeof(spi_chips)/sizeof(spi_chip_info_t);i++){
                if( spi_chips[i].chip_id==spi_chip0.chip_id ){
                        spi_chip0.chip_size       = spi_chips[i].chip_size;
                        spi_chip0.chip_name       = spi_chips[i].chip_name;
                        spi_chip0.spi_io_config_r = spi_chips[i].spi_io_config_r;
                        spi_chip0.spi_io_config_w = spi_chips[i].spi_io_config_w;
                        spi_chip0.spi_ri_conf     = spi_chips[i].spi_ri_conf;
                        spi_chip0.spi_ri_cmd      = spi_chips[i].spi_ri_cmd;
                        spi_chip0.spi_s_conf      = spi_chips[i].spi_s_conf;
                        spi_chip0.spi_s_rcmd      = spi_chips[i].spi_s_rcmd;
                        spi_chip0.spi_s_wcmd      = spi_chips[i].spi_s_wcmd;
                        
                        SPI_RI_CMD  = spi_chip0.spi_ri_cmd;
                        SPI_RI_CONF = spi_chip0.spi_ri_conf;
                        SPI_S_CONF  = spi_chip0.spi_s_conf;
                        SPI_S_RCMD  = spi_chip0.spi_s_rcmd;
                        SPI_IO_CONF = spi_chip0.spi_io_config_r;
                        break;
                }
        }
        
        if( 0!=spi_chip0.chip_size ){
                OSAL_PRINTF("Flash Chip = %s\n", spi_chip0.chip_name);
                size_in_kb = (uint16)(spi_chip0.chip_size>>10);
                OSAL_PRINTF("Flash Size = %d KB\n", size_in_kb);
        }else{
                print_ptr = (uint16*)&(spi_chip0.chip_id);
                if( ((spi_chip0.chip_id&0x00FFFFFF)!=0x00000000) ){
                        spi_chip0.chip_size       = spi_chips[0].chip_size;
                        spi_chip0.chip_name       = spi_chips[0].chip_name;
                        spi_chip0.spi_io_config_r = spi_chips[0].spi_io_config_r;
                        spi_chip0.spi_io_config_w = spi_chips[0].spi_io_config_w;
                        spi_chip0.spi_ri_conf     = spi_chips[0].spi_ri_conf;
                        spi_chip0.spi_ri_cmd      = spi_chips[0].spi_ri_cmd;
                        spi_chip0.spi_s_conf      = spi_chips[0].spi_s_conf;
                        spi_chip0.spi_s_rcmd      = spi_chips[0].spi_s_rcmd;
                        spi_chip0.spi_s_wcmd      = spi_chips[0].spi_s_wcmd;
                        spi_chip0.chip_name       = spi_chips[0].chip_name;
                        
                        SPI_RI_CMD  = spi_chip0.spi_ri_cmd;
                        SPI_RI_CONF = spi_chip0.spi_ri_conf;
                        SPI_S_CONF  = spi_chip0.spi_s_conf;
                        SPI_S_RCMD  = spi_chip0.spi_s_rcmd;
                        SPI_IO_CONF = spi_chip0.spi_io_config_r;
                }
                OSAL_PRINTF("Unknown chipID=0x%04x%04x\n", *print_ptr, *(print_ptr+1));
                size_in_kb = (uint16)(spi_chip0.chip_size>>10);
                OSAL_PRINTF("Default Flash Size = %d KB\n", size_in_kb);
        }
#endif /* end of else of 0==CONFIG_FLASH_PROBE */
        
        __spi_flash_wrsr(0);
}


static uint8 __spi_flash_readSR(void) large {
    
        /* Check Controller Ready */
        while( 1==SPI_CTRL_BUSY );
        
        /* Set sfr for ReadSR*/
        SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
        SPI_IO_CONF   = (spi_chip0.spi_io_config_r&SPI_IO_CONF_MHZ_MASK) \
                        | SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS);
        SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(1) | SPI_S_ACCESS_READ | \
                        SPI_S_ACCESS_SKIP_ADR;
        SPI_S_RCMD    = SPI_C_RDSR;
        SPI_CMD_ISSUE = 1;
        
        /* Check Controller Ready */
        while( 1==SPI_CTRL_BUSY );
        
        SPI_S_RCMD    = this_chip.spi_s_rcmd;
        
        return SPI_S_DATA0;
}

static void __spi_flash_waitRdy(void) large{
        while (__spi_flash_readSR()&SPI_FLASH_WIP);
}

static void __spi_flash_waitWEL(void) large{
        uint8 status;
        
        __spi_flash_waitRdy();
        
        /* Set sfr for WriteEnable*/
        SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(0) | SPI_S_ACCESS_WRITE | SPI_S_ACCESS_SKIP_ADR;
        SPI_S_WCMD    = SPI_C_WREN;
        SPI_CMD_ISSUE = 1;
        do{
                status = __spi_flash_readSR();
        }while( !(status&(SPI_FLASH_WEL)) );
}


uint8 __spi_flash_erase_check(uint32 erase_block_adr) large{
        uint8 i;
        
        /* Error Check */
        if( (erase_block_adr)>(this_chip.chip_size) ){
                SPI_ERROR_MSG(("adr[0x%08lX] is larger than ChipSize[0x%08lX]!\n", \
                        erase_block_adr, (this_chip.chip_size)));
                return FALSE;
        }
        if( 0!= (erase_block_adr%CONFIG_SPI_SECTOR_SIZE) ){
                SPI_ERROR_MSG(("erase_block_adr is not aligned!\n"));
                return FALSE;
        }
        
        for (i=0; i<flash_protect_region; i+=2){
                if( erase_block_adr>flash_protect_area[i] && \
                        erase_block_adr<flash_protect_area[i+1]
                ){
                        SPI_ERROR_MSG(("erase[0x%08lx] is protected!\n", erase_block_adr));
                        return FALSE;
                }
        }
        return TRUE;
}
/* erase_block_adr is flash physical address*/
uint8 spi_flash_erase(uint32 erase_block_adr, uint8 block) large{
        
        if( FALSE==__spi_flash_erase_check(erase_block_adr) ){
                return FALSE;
        }
        
        __spi_flash_waitWEL();
        
        SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
        SPI_IO_CONF   = (spi_chip0.spi_io_config_r&SPI_IO_CONF_MHZ_MASK) \
                        | SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS);
        SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(0) | SPI_S_ACCESS_WRITE ;
        SPI_S_WCMD    = SPI_C_SE;
        SPI_S_ADDR2   = (uint8)( erase_block_adr>>16);
        SPI_S_ADDR1   = (uint8)((erase_block_adr>> 8)&0x0000FF);
        SPI_S_ADDR0   = (uint8)((erase_block_adr>> 0)&0x0000FF);
        SPI_CMD_ISSUE = 1;
        
        if( SPI_ERASE_BLOCK==block ){
                __spi_flash_waitRdy();
        }

        SPI_IO_CONF = spi_chip0.spi_io_config_r;
        SPI_S_CONF = spi_chip0.spi_s_conf; // reset S config for DataBank
                
        return TRUE;
}


static uint8 __spi_flash_wrsr(uint8 new_value) large{
        
        /* Check Controller Ready */
        while( 1==SPI_CTRL_BUSY );
        
        /* Set sfr for ReadSR */
        SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
        SPI_IO_CONF   = SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS);
        SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(1) | SPI_S_ACCESS_WRITE | \
                        SPI_S_ACCESS_SKIP_ADR;
        SPI_S_RCMD    = SPI_C_WRSR;
        SPI_S_DATA0   = new_value;
        SPI_CMD_ISSUE = 1;
        
        /* Check Controller Ready */
        while( 1==SPI_CTRL_BUSY );
        
        SPI_S_RCMD    = this_chip.spi_s_rcmd;
        SPI_IO_CONF = spi_chip0.spi_io_config_r;
        SPI_S_CONF = spi_chip0.spi_s_conf; // reset S config for DataBank
        
        return SUCCESS;
}



#define DATA_LEN_PER_ITERATION  (4)
static uint8 __spi_flash_write(uint32 start_adr, uint8 *buf, uint32 write_len) large{
        int len = write_len;
        
        SPI_S_CONF    = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
        SPI_IO_CONF = this_chip.spi_io_config_w;
        while ( len>=DATA_LEN_PER_ITERATION ){
                __spi_flash_waitWEL();
                SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(DATA_LEN_PER_ITERATION) \
                                | SPI_S_ACCESS_WRITE ;
                SPI_S_WCMD    = this_chip.spi_s_wcmd;
                SPI_S_ADDR2 = (uint8)( start_adr>>16);
                SPI_S_ADDR1 = (uint8)((start_adr>> 8)&0x0000FF);
                SPI_S_ADDR0 = (uint8)((start_adr>> 0)&0x0000FF);
                SPI_S_DATA0 = *buf++;
                SPI_S_DATA1 = *buf++;
                SPI_S_DATA2 = *buf++;
                SPI_S_DATA3 = *buf++;
                
                SPI_CMD_ISSUE = 1;
                start_adr +=DATA_LEN_PER_ITERATION;
                len       -=DATA_LEN_PER_ITERATION;
        }
        
        while ( 0!=len ){
                __spi_flash_waitWEL();
                SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(1) | SPI_S_ACCESS_WRITE ;
                SPI_S_WCMD    = this_chip.spi_s_wcmd;
                SPI_S_ADDR2 = (uint8)(start_adr>>16);
                SPI_S_ADDR1 = (uint8)((start_adr>> 8)&0x0000FF);
                SPI_S_ADDR0 = (uint8)((start_adr>> 0)&0x0000FF);
                SPI_S_DATA0 = *buf;
                SPI_CMD_ISSUE = 1;
                buf       +=1;
                start_adr +=1;
                len       -=1;
        }
        
        __spi_flash_waitRdy();
        SPI_IO_CONF = spi_chip0.spi_io_config_r;
        SPI_S_CONF = spi_chip0.spi_s_conf; // reset S config for DataBank
        return SUCCESS;
}

uint8 __spi_flash_write_check(uint32 start_adr, uint32 len) large {
        uint8 i;

        if( (start_adr+len)>this_chip.chip_size ){
                SPI_ERROR_MSG(("[adr+len=0x%08lX] is larger than ChipSize[0x%08lX]!\n", \
                        start_adr+len, this_chip.chip_size));
                //SPI_ERROR_MSG(("[adr=0x%08lX][len=0x%08lX]\n", start_adr, len));
                return FALSE;
        }
        
        for (i=0; i<flash_protect_region; i+=2){
                if( ( start_adr < flash_protect_area[i]      &&       \
                          start_adr > flash_protect_area[i+1]  )      ||  \
                        ((start_adr+len) > flash_protect_area[i] &&       \
                         (start_adr+len) < flash_protect_area[i+1]  )
                ){
                        SPI_ERROR_MSG(("flash write area is protected!\n"));
                        //SPI_ERROR_MSG(("[start=0x%08lX][end=0x%08lX]\n", flash_protect_area[i], flash_protect_area[i+1]));
                        //SPI_ERROR_MSG(("[adr=0x%08lX][len=0x%08lX]\n", start_adr, len));
                        return FALSE;
                }
        }
        return TRUE;
}

/* start_adr is physical address , address 0 is the flash start address */
uint8 spi_flash_write(uint32 start_adr, uint8 *buf, uint32 len) large{
        uint32 xdata align_adr = 0, pre_size = 0;
        
        if( FALSE==__spi_flash_write_check(start_adr, len) ){
                return FALSE;
        }
        
        /* align page size */
        align_adr = start_adr&(~CONFIG_SPI_PAGE_MASK);
        pre_size = CONFIG_SPI_PAGE_SIZE-(start_adr-align_adr);
        
        if( (0!=start_adr%CONFIG_SPI_PAGE_SIZE) && \
            ( len>pre_size )
        ){
                __spi_flash_write(start_adr, buf, pre_size);
                start_adr += pre_size;
                buf += pre_size;
                len -= pre_size;
        }
        
        __spi_flash_write(start_adr, buf, len);
        
        return TRUE;
}

#if CONFIG_SPI_PIO_READ
/* start_adr is physical address , address 0 is the flash start address */
uint8 spi_flash_read(uint32 start_adr, uint8 *buf, uint32 read_len) large{
        int32 xdata len = read_len;
        
        if( (start_adr+len)>this_chip.chip_size ){
                SPI_ERROR_MSG(("start_adr+len is larger than chip_size!\n"));
                return FALSE;
        }
        
        __spi_flash_waitRdy();
        
        SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(4) | SPI_S_ACCESS_READ ;
        SPI_S_RCMD    = this_chip.spi_s_rcmd;
        SPI_IO_CONF   = this_chip.spi_io_config_r;
        SPI_S_CONF    = this_chip.spi_s_conf;

        while( len>=4 ){
                SPI_S_ADDR2 = (uint8)(start_adr>>16);
                SPI_S_ADDR1 = (uint8)((start_adr>> 8)&0x0000FF);
                SPI_S_ADDR0 = (uint8)((start_adr>> 0)&0x0000FF);
                SPI_CMD_ISSUE = 1;
                
                /* Check Controller Ready */
                while( 1==SPI_CTRL_BUSY );
                
                *buf++   = SPI_S_DATA0;
                *buf++   = SPI_S_DATA1;
                *buf++   = SPI_S_DATA2;
                *buf++   = SPI_S_DATA3;
                start_adr += 4;
                len -= 4;
        }
        
        SPI_S_ACCESS  = SPI_S_ACCESS_BYTE(1) | SPI_S_ACCESS_READ ;
        while( 0!=len ){
                SPI_S_ADDR2 = (uint8)(start_adr>>16);
                SPI_S_ADDR1 = (uint8)((start_adr>> 8)&0x0000FF);
                SPI_S_ADDR0 = (uint8)((start_adr>> 0)&0x0000FF);
                SPI_CMD_ISSUE = 1;
                
                /* Check Controller Ready */
                while( 1==SPI_CTRL_BUSY );
                *buf++   = SPI_S_DATA0;
                start_adr += 1;
                len -= 1;
        }
        
        return TRUE;
}

#endif /* end of CONFIG_SPI_PIO_READ */

void set_IOmode(uint8 iomode){

        if( CONFIG_SPI_DUAL_FAST==iomode ){
                SPI_IO_CONF     = SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SDD);
                SPI_RI_CMD      = 0xBB;
                SPI_RI_CONF     = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_04cycle);
                SPI_S_RCMD      = 0xBB;
                SPI_S_CONF      = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_04cycle);
        }else if( CONFIG_SPI_SERIAL_FAST==iomode ){
                SPI_IO_CONF     = SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS);
                SPI_RI_CMD      = 0x0B;
                SPI_RI_CONF     = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_08cycle);
                SPI_S_RCMD      = 0x0B;
                SPI_S_CONF      = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_08cycle);
        }/*else if( CONFIG_SPI_SERIAL_NORM==iomode ){
                SPI_IO_CONF     = SPI_IO_CONF_IOMODE(SPI_IO_CONF_IO_SSS);
                SPI_RI_CMD      = 0x03;
                SPI_RI_CONF     = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
                SPI_S_RCMD      = 0x03;
                SPI_S_CONF      = SPI_S_CONF_DUMMY(SPI_S_CONF_DUMMY_00cycle);
        }*/
}
#endif  // CONFIG_TO_USE_SCKU_SPI_DRIVER: 
