/*
* Copyright c                  Realtek Semiconductor Corporation, 2008  
* All rights reserved.
* 
* Abstract : RTL8213M Media Converter, 8051 register define
*$Id: reg320.h,v 1.2 2008/03/20 08:03:40 ralph_yu Exp $
*/

#ifndef __VAR_SFR_H__
#define __VAR_SFR_H__

/* Special SFR in DW8051 */
sfr DPL1   = 0x84;
sfr DPH1   = 0x85;
sfr DPS    = 0x86;
sfr CKCON  = 0x8E;
sfr SPC_FNC= 0x8F;
sfr EXIF   = 0x91;
sfr SCON1  = 0xC0;
sfr SBUF1  = 0xC1;
sfr T2CON  = 0xC8;
sfr RCAP2L = 0xCA;
sfr RCAP2H = 0xCB;
sfr TL2    = 0xCC;
sfr TH2    = 0xCD;
sfr EICON  = 0xD8;
sfr EIE    = 0xE8;
sfr EIP    = 0xF8;

/* Special SFR in Realtek */
sfr GPIO_E         = 0x93;
sfr GPIO_O         = 0x94;
sfr GPIO_I         = 0x95;
sfr GPIO2_E        = 0xC4;
sfr GPIO2_O        = 0xC2;
sfr GPIO2_I        = 0xC3;
sfr FLASH_MSB      = 0x96;
sfr INDACC_CMD     = 0xA0;
sfr INDACC_STATUS  = 0xA1;
sfr INDACC_ADDR_H  = 0xA2;
sfr INDACC_ADDR_L  = 0xA3;
sfr INDACC_WDATA_H = 0xA4;
sfr INDACC_WDATA_L = 0xA5;
sfr INDACC_RDATA_H = 0xA6;
sfr INDACC_RDATA_L = 0xA7;
sfr SYNC_CTRL      = 0x8E;
sfr PEDMA_E_ADDR0  = 0xB3;
sfr PEDMA_E_ADDR1  = 0xB4;
sfr PEDMA_P_ADDR0  = 0xB5;
sfr PEDMA_P_ADDR1  = 0xB6;
sfr PEDMA_LEN      = 0xB7;

/* Flash Access Registers */
sfr CACHE_CRTL    = 0x80;
sfr SPI_IO_CONF   = 0x9A;
sfr SPI_RI_CONF   = 0x9B;
sfr SPI_RI_CMD    = 0x9C;
sfr SPI_S_CONF    = 0x9D;
sfr SPI_S_ACCESS  = 0x9E;
sfr SPI_S_ADDR0   = 0xA9;
sfr SPI_S_ADDR1   = 0xAA;
sfr SPI_S_ADDR2   = 0xAB;
sfr SPI_S_DATA0   = 0xAC;
sfr SPI_S_DATA1   = 0xAD;
sfr SPI_S_DATA2   = 0xAE;
sfr SPI_S_DATA3   = 0xAF;
sfr SPI_S_RCMD    = 0xB1;
sfr SPI_S_WCMD    = 0xB2;
sfr SPI_DSEL_CYCLE= 0xBC;

sfr CODE_BANK     = 0x96;
sfr DATA_BANK     = 0x97;

/* Second Stack */
sfr SSPL  = 0xB9;
sfr SSPH  = 0xBA;
sfr SSIO  = 0xBB;

/*  BIT Registers  */
sbit SPI_CTRL_BUSY   = CACHE_CRTL^0;
sbit CACHE_CLEAN     = CACHE_CRTL^1;
sbit SPI_CMD_ISSUE   = CACHE_CRTL^2;

/*  T2CON  */
sbit TF2    = T2CON^7;
sbit EXF2   = T2CON^6;
sbit RCLK   = T2CON^5;
sbit TCLK   = T2CON^4;
sbit EXEN2  = T2CON^3;
sbit TR2    = T2CON^2;
sbit C_T2   = T2CON^1;
sbit CP_RL2 = T2CON^0;

/*  EIE  */
sbit EWDI  = EIE^4;
sbit EX5   = EIE^3;
sbit EX4   = EIE^2;
sbit EX3   = EIE^1;
sbit EX2   = EIE^0;

/*  EIP  */
sbit PWDI  = EIP^4;
sbit PX5   = EIP^3;
sbit PX4   = EIP^2;
sbit PX3   = EIP^1;
sbit PX2   = EIP^0;

/*  SCON1  */
sbit SM01  = SCON1^7; /* alternative SM0_FE_1  */
sbit SM11  = SCON1^6; /* alternative SM1_1  */
sbit SM21  = SCON1^5; /* alternative SM2_1  */
sbit REN1  = SCON1^4; /* alternative REN_1  */
sbit TB81  = SCON1^3; /* alternative TB8_1  */
sbit RB81  = SCON1^2; /* alternative RB8_1  */
sbit TI1   = SCON1^1; /* alternative TI_1   */
sbit RI1   = SCON1^0; /* alternative RI_1   */

#endif /* end of __VAR_SFR_H__ */
