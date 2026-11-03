#pragma STRING(far)
#include <local/config.h>
#include <reg_sfr.h>
#include <stdio.h>

#include <common.h>
#include <asicreg.h>
#include <lwps/pktbuf.h>
#include <rtk_switch.h>

#include <lib/lib_time.h>
#include <mem/mem_lib.h>

#include <lwps/lwps_api.h>
#include <web/httpd.h>

#include <salflash.h>
#include <salfwug.h>
#include <salsys.h>

#include <runtime_loader_msg.h>

#include <local/web_config.h>
#include <handler.h>
#include <port.h>

extern uint8    xdata *pkt_head;
extern pktBuf_t xdata pktbuf;
extern void set_8051RDY() large;

#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
void speed_change_reset()
{
    uint16 tmp;
    
    //SDS page 0 reg 3 bit 6=1
    setReg(0x6601,0x0003);
    setReg(0x6600,0x0080);
    tmp =getReg(0x6602);
    tmp |= 1 << 6;    
    setReg(0x6602,tmp);
    setReg(0x6601,0x0003);
    setReg(0x6600,0x00c0);
    
    //SDS page 0 reg 3 bit 6=0
    setReg(0x6601,0x0003);
    setReg(0x6600,0x0080);
    tmp =getReg(0x6602);
    tmp &= ~(1 << 6);    
    setReg(0x6602,tmp);
    setReg(0x6601,0x0003);
    setReg(0x6600,0x00c0);

    ////CDR reset
    //SDS page 0 reg 0 bit 1,0 = 0
    setReg(0x6601,0x0000);
    setReg(0x6600,0x0080);
    tmp =getReg(0x6602);
    tmp &= ~(1 << 1);    
    tmp &= ~(1 << 0);  
    setReg(0x6602,tmp);
    setReg(0x6601,0x0000);
    setReg(0x6600,0x00c0);

    //SDS page 0 reg 0 bit 1,0 = 1
    setReg(0x6601,0x0000);
    setReg(0x6600,0x0080);
    tmp =getReg(0x6602);
    tmp |= 1 << 1;       
    tmp |= 1 << 0;    
    setReg(0x6602,tmp);
    setReg(0x6601,0x0000);
    setReg(0x6600,0x00c0);
}
#endif

#if defined(FIBER_PORT_4)
void fiber_init()
{
    uint16 regData;
    uint16 tmp;

    //port set combo fiber
    setReg(0x6602, 0x04D7);
    setReg(0x6601, 0x0480);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0xF994);
    setReg(0x6601, 0x0481);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0x31A2);
    setReg(0x6601, 0x0482);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0x6960);
    setReg(0x6601, 0x0483);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0x9728);
    setReg(0x6601, 0x0484);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0x9D85);
    setReg(0x6601, 0x0423);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0xD810);
    setReg(0x6601, 0x0424);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0x400);
    setReg(0x6601, 0x2F);
    setReg(0x6600, 0x00C0);

#if 1//// disable ASIC bit error reset and enable down speed
    setReg(0x6602, 0x0f20);
    setReg(0x6601, 0x0001);
    setReg(0x6600, 0x00c0);
#endif

#if 1 ////  one_giga auto detect auto_force
    setReg(0x6601,0x0004);
    setReg(0x6600,0x0080);
    tmp =getReg(0x6602);
    tmp |= 1 << 1;        
    tmp |= 1 << 2;
    setReg(0x6602,tmp);
    setReg(0x6601,0x0004);
    setReg(0x6600,0x00c0);
#endif

#if 1 //MAC 4 change to auto
    setReg(0x13eb,0x15b9);
    setReg(0x13e7,0x7);
    setReg(0x6200,0x1140);
#endif

    speed_change_reset();
}
#endif

#if defined (FIBER_PORT_6)
void fiber_auto_detect()
{
    uint16 tmp;
    
    setReg(0x6602, 0x04D7);
    setReg(0x6601, 0x0480);
    setReg(0x6600, 0x00C0);
    
    setReg(0x1d11, 0x1740);
//    setReg(0x6200,0x1140);
    tmp = getReg(0x1d41);
    tmp |= 1<< 5;
    tmp |= 1<< 7;    
    setReg(0x1d41, tmp);
    setReg(0x13EB, 0x15BB);
    setReg(0x13E7, 0x7);      //release chip top serdes mode(default fiber auto)
    setReg(0x6200,0x1140);
    speed_change_reset();
}

void enable_down_speed()
{
    //  enable  cfg_frc_dwspd
    setReg(0x6602, 0x0f20);
    setReg(0x6601, 0x0001);
    setReg(0x6600, 0x00c0);
}

void auto_giga_with_linkpartner_force_1G()
{
    uint16 tmp;
    
    //  one_giga auto detect auto_force
    setReg(0x6601,0x0004);
    setReg(0x6600,0x0080);
    tmp =getReg(0x6602);
    tmp |= 1 << 1;        
    tmp |= 1 << 2;
    setReg(0x6602,tmp);
    setReg(0x6601,0x0004);
    setReg(0x6600,0x00c0);
}

void fiber_init()
{
    uint16 tmp;
    uint16 regData;
    
    //port set combo fiber
    setReg(0x6602, 0x04D7);
    setReg(0x6601, 0x0480);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0xF994);
    setReg(0x6601, 0x0481);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0x31A2);
    setReg(0x6601, 0x0482);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0x6960);
    setReg(0x6601, 0x0483);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0x9728);
    setReg(0x6601, 0x0484);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0x9D85);
    setReg(0x6601, 0x0423);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0xD810);
    setReg(0x6601, 0x0424);
    setReg(0x6600, 0x00C0);

    setReg(0x6602, 0x400);
    setReg(0x6601, 0x2F);
    setReg(0x6600, 0x00C0);

    fiber_auto_detect();
    enable_down_speed();
    auto_giga_with_linkpartner_force_1G();

}
#endif

#if defined(FIBER_PORT_70MB)
int8 setSerdesReg(uint16 sds_index, uint16 sds_reg, uint16 sds_page, uint16 regData)
{
    uint16 sds_addr = 0, sds_cmd = 0;
    uint16 busyFlag,checkCounter;
        
    sds_addr = (sds_page << 5) | sds_reg;
    sds_cmd  = sds_index | 0x00C0; 
    
    setReg(0x6602, regData);
    setReg(0x6601, sds_addr);
    setReg(0x6600, sds_cmd);

    checkCounter = 100;
	while(checkCounter)
	{
    	busyFlag = getReg(0x6600);
		if(busyFlag & 0x0100U)
		{
			checkCounter --;
			if(0 == checkCounter)
                return FAILED;
		}
		else
		{
			checkCounter = 0;
		}
	} 
    
    return SUCCESS;      
}

void pup_phy()
{
    
    setReg(0x6200, 0x1340);   //power up sds 0
    setReg(0x6210, 0x1340);   //power up sds 1

}
void base_patch() {
// patch sds	
    //cfg sds mode to reset mode
	setReg(0x1d92, 0x1f1f);	

	//REG_RX_OFFSET_AUTO_K force
	setSerdesReg(1,1,0x21,0xEC91);
	setSerdesReg(1,5,0x24,0x5825);
	//setSerdesReg(1,0,2,0x1140);
   //patch FIB100 Device Link up/down 3times
   	setSerdesReg(1,13,0,0x4616);
   //enable down speed
	setSerdesReg(1,1,0,0xf20);

	//REG_RX_OFFSET_AUTO_K force
	setSerdesReg(0,0,0x28,0x942c);
	setSerdesReg(0,0,0x24,0x942c);
	setSerdesReg(0,5,0x21,0x8dc3);
	//Patch for Hisgmii
	setSerdesReg(0,9,0x21,0x3931);
	//setSerdesReg(0,0,2,0x1140);
   //patch FIB100 Device Link up/down 3times
   	setSerdesReg(0,13,0,0x4616);
   //enable down speed
	setSerdesReg(0,1,0,0xf20);

   //enable sds1/0				  
	setReg(0x1d92, 0x707);   
}
#endif

void uart_init()
{
    uint16 tmp;
    uint32 divisor;

    IE = 0;
    /* enable global interrupt */
    EA = 1;
    T2CON = 0x34;
    SCON0 = 0x50;

    setReg(0x1d32, 0x0004); //Enable 67C UART
    
    tmp = (getReg(0x1336) >> 4) & 0x7;
    switch(tmp)
    {
    case DW8051_RDY_200MHz:
        system_clock = (200000UL * 1000);
        break;
    case DW8051_RDY_125MHz:
        system_clock = (125000UL * 1000);
        break;
    case DW8051_RDY_100MHz:
        system_clock = (100000UL * 1000);
        break;
    case DW8051_RDY_625MHz:
        system_clock = (62500UL * 1000);
        break;
    case DW8051_RDY_20_8MHz:
        system_clock = (20800UL * 1000);
        break;
    default:
        system_clock = (20800UL * 1000);
    }
    divisor = 65536 - system_clock/(32UL * BAUD_RATE);

    RCAP2L = divisor & 0xFF;
    RCAP2H = (divisor & 0xFF00) >> 8;

    setReg(0x13a0, (RCAP2H << 8) | RCAP2L);
        
    TI = 1;
    RI = 0;

    /* enable flash */
    tmp = getReg(0x130c);
    tmp |= 0x1000;
    setReg(0x130c, tmp);
}
 

void nic_init()
{
    uint16 tmp;
 
    rxSeq = 0;
    txSeq = 0;
	
    /* reset nic */
    tmp = getReg(0x1322);
    tmp = tmp | (1 << 5);
    setReg(0x1322, tmp);
    /* set nic ready */
    tmp = getReg(0x130c);
    tmp |= 0x400;
    setReg(0x130c, tmp);
    /* set rx stop */
    setReg(0x1a46, (RXBUFFERSIZE/8 - 1) & 0xFF);
    setReg(0x1a47, (RXBUFFERSIZE / 8 - 1) >> 8);
    /* set tx stop  */
    setReg(0x1a44, ((RXBUFFERSIZE + TXBUFFERSIZE)/8 - 1) & 0xFF);
    setReg(0x1a45, ((RXBUFFERSIZE + TXBUFFERSIZE)/8 - 1) >> 8);
     /* set rxfst */
    setReg(0x1a48, 4);
 
    /* set mtu */
    setReg(0x1a17, 0x0000);


	/* set rx drop */
	setReg(0x1a5C, 1);
    // setReg(0x1a5C, 0);
    /*enable TX */
    setReg(NIC_TXCR, 1);
  
    /* enable rx and discard crc err */
    setReg(NIC_RXCR1, 0x03);
   //setReg(NIC_RXCR1, 0x1D);

}

void switch_init(void) large{

    uint16 i;
    /* Set initial state */
    rtk_switch_init();

    i = getReg(0x130C);
    i |= 0x50;
    setReg(0x130C, i);    
    
}


#if 0
void clear_ACL(void)
{
   uint16 i;

    /* clear template*/
    for(i=0x600;i<=0x613;i++)
        setReg(i,0);
	
   
	/*clear ACL care bit*/
	for(i=0;i<0x40;i++)
	{
        setReg(0x0501,i);
	    setReg(0x0510,0);
	    setReg(0x0511,0);
	    setReg(0x0512,0);
	    setReg(0x0513,0);
	    setReg(0x0514,0);
	    setReg(0x0515,0);
	    setReg(0x0516,0);
	    setReg(0x0517,0);
	    setReg(0x0518,0);
	    setReg(0x0519,0);
	    setReg(0x0500,0x0009);
	}
    /*clear ACL rules*/
	for(i=0x40;i<0x80;i++)
	{
        setReg(0x0501,i);
	    setReg(0x0510,0);
	    setReg(0x0511,0);
	    setReg(0x0512,0);
	    setReg(0x0513,0);
	    setReg(0x0514,0);
	    setReg(0x0515,0);
	    setReg(0x0516,0);
	    setReg(0x0517,0);
	    setReg(0x0518,0);
	    setReg(0x0519,0);
	    setReg(0x0500,0x0009);
	}
}

void cpu_patch()
{
#define ARP_ETHER_TYPE   (0x0806)
#define IP_ETHER_TYPE    (0x0800)
#define RTK_ETHER_TYPE   (0x8899)
#define DHCP_L4_SRC_PORT (0x0043)
#define DHCP_L4_DST_PORT (0x0044)
    
    uint16 i, tmp;

    /* Add cpu port into isoloation output port */
    for(i = 0x8a2; i < 0x8a9; i++)
        setReg(i,getReg(i)|0x80);
   
   /* Add cpu port into vlan 1 */
   setReg(0x510, 0x80ff);
   setReg(0x511, 0);
   setReg(0x512, 0xc000);
   setReg(0x513, 0x3fff);
   setReg(0x501, 1);
   setReg(0x500, 0x13);
   
   /* exclude cpu port from flooding portmask */
   setReg(0x890, 0x7F);
   setReg(0x891, 0x7F);
   setReg(0x892, 0x7F);
   	/* Add static entry in L2 for cpu port */
    //setReg(0x502, 0x1498);
	setReg(0x510, this_mac[5] | (this_mac[4] << 8)); 
	setReg(0x511, this_mac[3] | (this_mac[2] << 8)); 
	setReg(0x512, this_mac[1] | (this_mac[0] << 8));
	setReg(0x513, 0x2001);
	setReg(0x514, 0x7f00);
	setReg(0x515, 0x0020);
	setReg(0x500, 0x000c);
	

  
    /* set ACL enable */
    setReg(0x6d5,0xff);




	 /*set ACL template 0 as DMAC0 DMAC1 DMAC2 ETHERTYPE SMAC0 SMAC1 SMAC2*/
	setReg(0x0600,0x0201);
	setReg(0x0601,0x0703);
	setReg(0x0602,0x0504);
	setReg(0x0603,0x0006);
	
	
	/* Add ACL rule 0 to fowrard packet with ARP and DA = Switch MAC to cpu port */
	
	setReg(0x0501,0x0000);
	setReg(0x0510,0x80ff);
	setReg(0x0511,~((this_mac[4] << 8) | this_mac[5])) ;
	setReg(0x0512, ~((this_mac[2] << 8) | this_mac[3]));
	setReg(0x0513,~((this_mac[0] << 8) | this_mac[1]));
	setReg(0x0514,0xf7f9);
	setReg(0x0515,0x0000);
	setReg(0x0516,0x0000);
	setReg(0x0517,0x0000);
	setReg(0x0518,0x0000);
	setReg(0x0500,0x0009);
	
	setReg(0x0501,0x0040);
	setReg(0x0510,0x7f00);
	setReg(0x0511,(this_mac[4] << 8) | this_mac[5]);
	setReg(0x0512,(this_mac[2] << 8) | this_mac[3]);
	setReg(0x0513,(this_mac[0] << 8) | this_mac[1]);
	setReg(0x0514,0x0806);
	setReg(0x0515,0x0000);
	setReg(0x0516,0x0000);
	setReg(0x0517,0x0000);
	setReg(0x0518,0x0000);
	setReg(0x0519,0x0001);
	setReg(0x0500,0x0009);
	
	setReg(0x0501,0x0000);
	setReg(0x0510,0x0000);
	setReg(0x0511,0x6000);
	setReg(0x0512,0x0000);
	setReg(0x0500,0x000a);
	setReg(0x0614,0x0018);
	
	/* Add ACL rule 1 to mirror ARP to 8051 */
	setReg(0x0501,0x0001);
	setReg(0x0510,0x80ff);
	setReg(0x0511,0x0000);
	setReg(0x0512,0x0000);
	setReg(0x0513,0x0000);
	setReg(0x0514,0xf7f9);
	setReg(0x0515,0x0000);
	setReg(0x0516,0x0000);
	setReg(0x0517,0x0000);
	setReg(0x0518,0x0000);
	setReg(0x0500,0x0009);
	
	setReg(0x0501,0x0041);
	setReg(0x0510,0x7f00);
	setReg(0x0511,0x0000);
	setReg(0x0512,0x0000);
	setReg(0x0513,0x0000);
	setReg(0x0514,0x0806);
	setReg(0x0515,0x0000);
	setReg(0x0516,0x0000);
	setReg(0x0517,0x0000);
	setReg(0x0518,0x0000);
	setReg(0x0519,0x0001);
	setReg(0x0500,0x0009);
	
	setReg(0x0614,0x1818);//RULE0+RULE1
	setReg(0x0501,0x0001);
	setReg(0x0510,0x0000);
	setReg(0x0511,0x2002);
	setReg(0x0512,0x0000);
	setReg(0x0500,0x000a);
	
	
	
	
	
	/* Add ACL rule 2 to block IP packet with SA equals to switch MAC */ 
	
	setReg(0x0501,0x0002);
	setReg(0x0510,0x80ff);
	setReg(0x0511,0x0000);
	setReg(0x0512,0x0000);
	setReg(0x0513,0x0000);
	setReg(0x0514,0xf7ff);
	setReg(0x0515,~((this_mac[4] << 8) | this_mac[5]));
	setReg(0x0516,~((this_mac[2] << 8) | this_mac[3]));
	setReg(0x0517,~((this_mac[0] << 8) | this_mac[1]));
	setReg(0x0518,0x0000);
	setReg(0x0500,0x0009);
	
	setReg(0x0501,0x0042);
	setReg(0x0510,0x7f00);
	setReg(0x0511,0x0000);
	setReg(0x0512,0x0000);
	setReg(0x0513,0x0000);
	setReg(0x0514,0x0800);
	setReg(0x0515,(this_mac[4] << 8) | this_mac[5]);
	setReg(0x0516,(this_mac[2] << 8) | this_mac[3]);
	setReg(0x0517,(this_mac[0] << 8) | this_mac[1]);
	setReg(0x0518,0x0000);
	setReg(0x0519,0x0001);
	setReg(0x0500,0x0009);
	
	setReg(0x0501,0x0002);
	setReg(0x0510,0x0000);
	setReg(0x0511,0x4000);
	setReg(0x0512,0x0000);
	setReg(0x0500,0x000a);
	setReg(0x0615,0x0010);




	/* set MAC in MAC register */
	tmp = this_mac[4];
	tmp = this_mac[5] | (tmp << 8);
	setReg(0x1209, tmp);
	tmp = this_mac[2];
	tmp = this_mac[3] | (tmp << 8);
	setReg(0x120A, tmp);
	tmp = this_mac[0];
	tmp = this_mac[1] | (tmp << 8);
    setReg(0x120B, tmp);

    /* When queue number = 4, map priority 6 and priority 7 to queue 3 */
    setReg(0x90b, 0x3322);

    /* LED setting */
    /* Parallel mode, group 0:speed1000/ACT, group 1:speed100(10)/ACT, group 2:off */
    setReg(0x1b26, 3);
    setReg(0x1B00, 0x14F4);
	setReg(0x1B03, 0x0096);
}
#endif

void interrupt_init(void)
{
    IE  = 0;
    EIE = 0;
    EA  = 1;
}

void timer_init(void)
{
    uint16 xdata tmp;

    current_time = JAN_1970;
    timeCount = 0;

    TMOD = 0x01;                /* enable timer 0 */
    tmp  = (uint16) (system_clock/(12*CONFIG_TIMER_DIVISOR));
    tmp  = 0xFFFF - tmp + 1;
    _th0 = (tmp>> 8)&0xFF;
    _tl0 = (tmp>> 0)&0xFF;
    TH0  = _th0;
    TL0  = _tl0;

    tmp += (0x0DU); /* magic number, for Software reload */
    _th0 = (tmp>> 8)&0xFF;
    _tl0 = (tmp>> 0)&0xFF;

    //OSAL_PRINTF("Divisor=0x%04x\n", tmp);

    TCON  = 0x10;                //enable timer0 and timer 1
    CKCON = CKCON & 0xC7;       //timer clock is system clock/12

    ET0   = 1;                    // enable timer0 interrupt
}

void lwps_init(void)
{
    eth_init();
    etharp_init();
    ip_init();
    icmp_init();
    tcp_init();
    udp_init();
    lwps_api_init();
}

void all_init(void)
{
    uint32 i;

    change_clock(DW8051_CLK);
    CKCON = 0x0; //Stretch value is 0
    DATA_BANK = 0;
    mem_init();
    uart_init();
    nic_init();
    flash_init(); //spi_flash_driver_info();
	
    for(i=0;i<0xffff;i++) //add delay for spi flash to finish initial
        ;
    
    EX1 = 1;
    pkt_head = (p_buffer+TAG_SIZE+FRAME_INFO_SIZE);
}

void loader_init()
{
    sal_mac_t   mac;
    sal_fwug_startState_t start_state;

    pkt_head = (p_buffer+TAG_SIZE+FRAME_INFO_SIZE);    

#if defined(FIBER_PORT_70MB)
    base_patch();
#endif
  
    interrupt_init();
	change_clock(DW8051_CLK);	 
	uart_init();  

    sal_fwug_startState_get(&start_state);

    if(SAL_FWUG_STARTSTATE_COLD == start_state)
    {
        switch_init();    
    }

	set_8051RDY();
	
#if defined(FIBER_PORT_70MB)
    pup_phy();
#endif
    
#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
    fiber_init();
#endif

	//clear_ACL();
    lwps_init();
	
    httpd_init();
#if defined(UPGRADE_TFTP)
    /* TFTP init */
    tftpc_init();
    sal_fwug_status_set(SAL_FWUG_STATUS_NORMAL);
#endif    
    /* init ip */
    if(SAL_FWUG_STARTSTATE_WARM == start_state)
    {
        runtime_ip_get(this_ip, netmask, default_gateway.addr);
    }
    else
    {
        sal_flash_config_init();
    }


    /* check whether non-volatile MAC address is valid */
    sal_flash_sysMac_get(&mac);
    
    /* if it is invalid, use non-volatile MAC as MAC address */
    if(mac.octet[0] != 0xFF || mac.octet[1] != 0xFF || mac.octet[2] != 0xFF ||
        mac.octet[3] != 0xFF || mac.octet[4] != 0xFF || mac.octet[5] != 0xFF)
    {
        printf("load MAC from nvcfg\n");
        sal_sys_mac_set(&mac);
    }
    /* else, MAC address will following configuration, so do nothing here */


    loader_isr_set(TRUE);
    timer_init();  

    /* start phy */
	rtk_port_phyEnableAll_set(ENABLED);
	
	handler_init();

    printf("  IP:%d.%d.%d.%d\n", (uint16)this_ip[0], (uint16)this_ip[1], (uint16)this_ip[2], (uint16)this_ip[3]);
    printf("Mask:%d.%d.%d.%d\n", (uint16)netmask[0], (uint16)netmask[1], (uint16)netmask[2], (uint16)netmask[3]);
    printf("  GW:%d.%d.%d.%d\n", (uint16)default_gateway.addr[0], (uint16)default_gateway.addr[1], (uint16)default_gateway.addr[2], (uint16)default_gateway.addr[3]);
    printf(" MAC:%02bX.%02bX.%02bX.%02bX.%02bX.%02bX\n", this_mac[0], this_mac[1], this_mac[2], this_mac[3], this_mac[4], this_mac[5]);
    
//    setReg(0x1d3f, 0x0000); //Force deglitch disble
}

