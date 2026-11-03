/*
* Copyright c                  Realtek Semiconductor Corporation, 2008  
* All rights reserved.
* 
* Abstract : RTL8213M Media Converter, 8051 register define
*$Id: reg320.h,v 1.2 2008/03/20 08:03:40 ralph_yu Exp $
*/

#ifndef __BASIC_SFR_H__
#define __BASIC_SFR_H__

/*  BYTE Registers  */
sfr P0     = 0x80;
sfr P1     = 0x90;
sfr P2     = 0xA0;
sfr P3     = 0xB0;
sfr PSW    = 0xD0;
sfr ACC    = 0xE0;
sfr B      = 0xF0;
sfr SP     = 0x81;
sfr DPL    = 0x82;
sfr DPH    = 0x83;
sfr PCON   = 0x87;
sfr TCON   = 0x88;
sfr TMOD   = 0x89;
sfr TL0    = 0x8A;
sfr TL1    = 0x8B;
sfr TH0    = 0x8C;
sfr TH1    = 0x8D;
sfr IE     = 0xA8;
sfr IP     = 0xB8;
sfr SCON0  = 0x98;
sfr SBUF0  = 0x99;


/*  BIT Registers  */
/*  PSW */
sbit CY    = PSW^7;
sbit AC    = PSW^6;
sbit F0    = PSW^5;
sbit RS1   = PSW^4;
sbit RS0   = PSW^3;
sbit OV    = PSW^2;
sbit FL    = PSW^1;
sbit P     = PSW^0;

/*  TCON  */
sbit TF1   = TCON^7;
sbit TR1   = TCON^6;
sbit TF0   = TCON^5;
sbit TR0   = TCON^4;
sbit IE1   = TCON^3;
sbit IT1   = TCON^2;
sbit IE0   = TCON^1;
sbit IT0   = TCON^0;

/*  IE  */
sbit EA    = IE^7;
sbit ES1   = IE^6;
sbit ET2   = IE^5;
sbit ES0   = IE^4;
sbit ET1   = IE^3;
sbit EX1   = IE^2;
sbit ET0   = IE^1;
sbit EX0   = IE^0;

/*  IP  */
sbit PS1   = IP^6;
sbit PT2   = IP^5;
sbit PS0   = IP^4;
sbit PT1   = IP^3;
sbit PX1   = IP^2;
sbit PT0   = IP^1;
sbit PX0   = IP^0;

/*  P1  */
sbit INT5  = P1^7;
sbit INT4  = P1^6;
sbit INT3  = P1^5;
sbit INT2  = P1^4;
sbit TXD1  = P1^3;
sbit RXD1  = P1^2;
sbit T2EX  = P1^1;
sbit T2    = P1^0;

/*  P3  */
sbit RD    = P3^7;
sbit WR    = P3^6;
sbit T1    = P3^5;
sbit T0    = P3^4;
sbit INT1  = P3^3;
sbit INT0  = P3^2;
sbit TXD0  = P3^1;
sbit RXD0  = P3^0;

/*  SCON0  */
sbit SM0   = SCON0^7; /* alternative SM0_FE_0 */
sbit SM1   = SCON0^6; /* alternative SM1_0 */
sbit SM2   = SCON0^5; /* alternative SM2_0 */
sbit REN   = SCON0^4; /* alternative REN_0 */
sbit TB8   = SCON0^3; /* alternative TB8_0 */
sbit RB8   = SCON0^2; /* alternative RB8_0 */
sbit TI    = SCON0^1; /* alternative TI_0  */
sbit RI    = SCON0^0; /* alternative RI_0  */

#endif /* __BASIC_SFR_H__ */

