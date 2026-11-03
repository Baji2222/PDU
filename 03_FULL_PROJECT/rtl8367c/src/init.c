
#pragma STRING(far)
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <local/config.h>
#include <reg_sfr.h>
#include <common.h>
#include <asicreg.h>
#include <runtime_loader_msg.h>

//#include <sntpc/sntpc.h>
#include <dhcpc/dhcpc.h>
#include <lwps/lwps_api.h>
#include <lib/lib_time.h>
#include <sal/saltype.h>
#include <salsys.h>
#include <salled.h>
#include <rtk_api/rtk_switch.h>
#include <rtk_api/acl.h>
#include <rtk_api/cpu.h>

#if WEB_ENABLE
#include <web/httpd.h>
#include <web/web_flash.h>
#endif /* end of WEB_ENABLE */

#if RTK_PROTO_RLPP
#include <rtk_proto/rlpp/rlpp.h>
#endif

#if LWPS_LLDP
#include <lldp/lldp.h>
#endif

#include <handler.h>
#include <rtl8367c_asicdrv_storm.h>
#include <rtl8367c_asicdrv_meter.h>
#include <rtl8367c_asicdrv_specialcongest.h>
#include <rtl8367c_asicdrv_vlan.h>
#include <rtl8367c_asicdrv_cputag.h>
#include <salgpio.h>
#include <salflash.h>
#include <rtl8367c_asicdrv_scheduling.h>
#include <rtl8367c_reg.h>
#include <salacl.h>
#include <port.h>

//===========================
#include <flash_config.h>
#include <salflash.h>
#include <salutil.h>

//=======================
#ifdef RTL8367C
extern uint16 phyAllPortMask;
extern uint32 aclAllPortMask;
extern uint8 portIdCput;
#endif


extern u16_t vlanIds;

extern sal_ipaddr_t netmask = 0;
extern int add_ip_over_vlan_acl_rule(uint16 rule_num, uint16 vlan_id, uint32 ip_address,uint32 subnetmask);
extern int32 sal_flash_vlan_id_get(uint32 addr , uint16 length, uint8 *pData);

// UART Initialization: Configures UART with baud rate based on system clock frequency
// Sets up timer2 for baud rate generation and enables UART peripheral
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

// Timer Initialization: Sets up timer0 for system timekeeping with configurable divisor
// Initializes current time to JAN_1970 and configures timer reload values
void timer_init(void) large{
	uint16 xdata tmp;

	current_time = JAN_1970;
	timeCount = 0;

    TMOD = 0x11;                //timer0 and timer1 mode1	
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

// Interrupt Initialization: Disables all interrupts by clearing IE and EIE registers
// Called during system initialization to start with clean interrupt state
void interrupt_init(void) large{
	IE  = 0;
	EIE = 0;
}

// NIC Initialization: Configures Network Interface Controller registers and buffers
// Sets up RX/TX buffer sizes, enables NIC, and configures CPU tag for packet processing
void nic_init(void) large{
	uint16 xdata tmp;

	tmp = getReg(CHIP_RST);
	tmp = tmp | CHIP_RST_NIC;
	setReg(CHIP_RST, tmp);
   	
	rxSeq = 0;
	txSeq = 0;


	tmp = getReg(CHIP_RST);
	tmp = tmp & (~CHIP_RST_NIC);
	setReg(CHIP_RST, tmp);

	/* set NIC ready */
	tmp = getReg(RTL8367C_REG_MISCELLANEOUS_CONFIGURE0);
	tmp = tmp | 0x400;
	setReg(RTL8367C_REG_MISCELLANEOUS_CONFIGURE0, tmp);

	/* enable NIC RX interrupt */
	setReg(0x1A0F, 0x80);
	
    //setReg(0x131d, 0x1075);

	/* set txstop rxstop */
	tmp = (RXBUFFERSIZE / 8) - 1;
	setReg(NIC_RXSTOPL, tmp & 0xFF);
	setReg(NIC_RXSTOPH, tmp >> 8);
    tmp = ((TXBUFFERSIZE + RXBUFFERSIZE)/ 8) - 1;
	setReg(NIC_TXSTOPL, tmp & 0xFF);
	setReg(NIC_TXSTOPH, tmp >> 8);

	/* set rxfst */
	setReg(RTL8367C_REG_NIC_RXFSTR, 4);
	setReg(NIC_GCR, 0x0000);

	/*enable TX */
	setReg(NIC_TXCR, 1);

	setReg(NIC_RXCR1, 0x03); //discarded CRC Error, remove CRC, Rx Enable

	/* enable cpu tag function, and set nic as cpu port */
    //need to add
	setReg(0x121a, 0xbb);
	setReg(0x1219, 0x0080);
	
	pktbuf.tx_dport_mode = TX_DPORT_DEFAULT;
	pktbuf.pkt_cputag = NULL;

}

// LWPS Initialization: Initializes all Lightweight Protocol Stack components
// Calls eth_init, arp_init, ip_init, icmp_init, tcp_init, udp_init and lwps_api_init
void lwps_init(void) large{
	
	
	eth_init();
	etharp_init();
	ip_init();
	icmp_init();
	tcp_init();
	udp_init();
	lwps_api_init();
}

extern void eee_init();

// Switch Initialization: Initializes the Ethernet switch hardware and configures basic settings
// Calls rtk_switch_init and enables additional switch features via register configuration
void switch_init(void) large{
    uint16 i;
    
    /* Set initial state */
    rtk_switch_init();
    
    i = getReg(0x130C);
    i |= 0x50;
    setReg(0x130C, i);    
    
}


uint16 default_vlan_fid = 0;

// CPU Patch: Configures ACL rules and switch settings for CPU port traffic handling
// Sets up ARP, ICMP, DHCP rules and storm control, QoS, and VLAN ingress filtering
void cpu_patch()
{
#define ARP_ETHER_TYPE   (0x0806)
#define IP_ETHER_TYPE    (0x0800)
#define RTK_ETHER_TYPE   (0x8899)
#define DHCP_L4_SRC_PORT (0x0043)
#define DHCP_L4_DST_PORT (0x0044)

    uint16                  i, j, tmp;
    rtk_filter_cfg_t cfg;
    rtk_filter_action_t act;
    rtk_filter_field_t *field1;
	rtk_filter_field_t *field2;
    rtk_filter_number_t ruleNum;
	uint32 ret;

    rtk_cpu_enable_set(ENABLED);
    rtk_cpu_tagPort_set(salPortIdCpu, CPU_INSERT_TO_TRAPPING);
	
    /* Add cpu port into isoloation output port */
    for(i = 0x8a2; i < 0x8a2 + sysMaxPhyPort; i++)
    {
        tmp = getReg(i);
        tmp |= (cpuPortPortmask & 0xffff);
        setReg(i, tmp);
    }

    /* unknown Unicast/multicast/broadcast don't flooding to 8051 port */
    setReg(RTL8367C_UNUCAST_FLOADING_PMSK_REG, phyAllPortMask);
    setReg(RTL8367C_UNMCAST_FLOADING_PMSK_REG, phyAllPortMask);
    setReg(RTL8367C_BCAST_FLOADING_PMSK_REG, phyAllPortMask);

#if 0
   	/* Add static entry in L2 for cpu port */
    //setReg(0x502, 0x1498);
	setReg(0x510, this_mac[5] | (this_mac[4] << 8)); 
	setReg(0x511, this_mac[3] | (this_mac[2] << 8)); 
	setReg(0x512, this_mac[1] | (this_mac[0] << 8));
	setReg(0x513, 0x2001);
	setReg(0x514, 0x7f00);
	setReg(0x515, 0x0020);
	setReg(0x500, 0x000c);
#endif
	memset(&cfg, 0x00, sizeof(rtk_filter_cfg_t));
    memset(&act, 0x00, sizeof(rtk_filter_action_t));
    field1= malloc(sizeof(rtk_filter_field_t));
    memset(field1, 0, sizeof(rtk_filter_field_t));
	field2= malloc(sizeof(rtk_filter_field_t));
    memset(field2, 0, sizeof(rtk_filter_field_t));

    /* Add ACL rule 1 ARP+(DA=switch MAC)->redirect to 8051 with priority 1+ policing(meter 1)*/
    field1->fieldType = FILTER_FIELD_DMAC;
    field1->filter_pattern_union.dmac.dataType        = FILTER_FIELD_DATA_MASK;
    field1->filter_pattern_union.dmac.value.octet[0]  = this_mac[0];
    field1->filter_pattern_union.dmac.value.octet[1]  = this_mac[1];
    field1->filter_pattern_union.dmac.value.octet[2]  = this_mac[2];
    field1->filter_pattern_union.dmac.value.octet[3]  = this_mac[3];
    field1->filter_pattern_union.dmac.value.octet[4]  = this_mac[4];
    field1->filter_pattern_union.dmac.value.octet[5]  = this_mac[5];
    field1->filter_pattern_union.dmac.mask.octet[0]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[1]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[2]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[3]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[4]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[5]   = 0xFF;
    sal_filter_igrAcl_field_add(&cfg, field1);

	field2->fieldType = FILTER_FIELD_ETHERTYPE;
    field2->filter_pattern_union.etherType.dataType = FILTER_FIELD_DATA_MASK;
	field2->filter_pattern_union.etherType.value       = ARP_ETHER_TYPE;
	field2->filter_pattern_union.etherType.mask        =0xffff;
	sal_filter_igrAcl_field_add(&cfg, field2);
    
    cfg.activeport.value.bits[0]= aclAllPortMask;
    cfg.activeport.mask.bits[0] = aclAllPortMask;
    cfg.invert = FALSE;

    /* Action :  */
    act.actEnable[FILTER_ENACT_REDIRECT] = TRUE;
    act.filterPortmask.bits[0] = (1UL << (salPortIdCpu));
    act.actEnable[FILTER_ENACT_PRIORITY] = TRUE;
    act.filterPriority = 1;
    act.actEnable[FILTER_ENACT_POLICING_0] = TRUE;
    act.filterPolicingIdx[0]=1;
	sal_filter_igrAcl_cfg_add(1, &cfg, &act, &ruleNum);	


    /*Rule 2: ICMP + (DA=switch MAC) ' redirect to 8051 with priority 6*/
    memset(&cfg, 0x00, sizeof(rtk_filter_cfg_t));
    memset(&act, 0x00, sizeof(rtk_filter_action_t));
    memset(field1, 0, sizeof(rtk_filter_field_t));
	memset(field2, 0, sizeof(rtk_filter_field_t));

    field1->fieldType = FILTER_FIELD_DMAC;
    field1->filter_pattern_union.dmac.dataType        = FILTER_FIELD_DATA_MASK;
    field1->filter_pattern_union.dmac.value.octet[0]  = this_mac[0];
    field1->filter_pattern_union.dmac.value.octet[1]  = this_mac[1];
    field1->filter_pattern_union.dmac.value.octet[2]  = this_mac[2];
    field1->filter_pattern_union.dmac.value.octet[3]  = this_mac[3];
    field1->filter_pattern_union.dmac.value.octet[4]  = this_mac[4];
    field1->filter_pattern_union.dmac.value.octet[5]  = this_mac[5];
    field1->filter_pattern_union.dmac.mask.octet[0]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[1]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[2]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[3]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[4]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[5]   = 0xFF;
    sal_filter_igrAcl_field_add(&cfg, field1);

	field2->fieldType = FILTER_FIELD_ICMP_TYPE;
    field2->filter_pattern_union.icmpType.dataType = FILTER_FIELD_DATA_MASK;
	field2->filter_pattern_union.icmpType.value       = 8;
	field2->filter_pattern_union.icmpType.mask        =0xffff;
	sal_filter_igrAcl_field_add(&cfg, field2);
    
    cfg.activeport.value.bits[0]    = aclAllPortMask;
    cfg.activeport.mask.bits[0]     = aclAllPortMask;
    cfg.invert = FALSE;

    /* Action :  */
    act.actEnable[FILTER_ENACT_REDIRECT] = TRUE;
    act.filterPortmask.bits[0] = (1UL << (salPortIdCpu));
    act.actEnable[FILTER_ENACT_PRIORITY] = TRUE;
    act.filterPriority = 6;
	act.actEnable[FILTER_ENACT_POLICING_0] = TRUE;
    act.filterPolicingIdx[0]=0;

    sal_filter_igrAcl_cfg_add(2, &cfg, &act, &ruleNum);

	
	/*Rule 3: DHCP ' mirror to 8051 with priority 7+ policing (meter 2)*/
    memset(&cfg, 0x00, sizeof(rtk_filter_cfg_t));
    memset(&act, 0x00, sizeof(rtk_filter_action_t));
    memset(field1, 0, sizeof(rtk_filter_field_t));
	memset(field2, 0, sizeof(rtk_filter_field_t));	
    field1->fieldType = FILTER_FIELD_UDP_SPORT;
    field1->filter_pattern_union.udpSrcPort.dataType = FILTER_FIELD_DATA_MASK;
	field1->filter_pattern_union.udpSrcPort.value = DHCP_L4_SRC_PORT;
	field1->filter_pattern_union.udpSrcPort.mask =0xFFFF;
	sal_filter_igrAcl_field_add(&cfg, field1);
	field2->fieldType = FILTER_FIELD_UDP_DPORT;
    field2->filter_pattern_union.udpDstPort.dataType = FILTER_FIELD_DATA_MASK;
	field2->filter_pattern_union.udpDstPort.value = DHCP_L4_DST_PORT;
	field2->filter_pattern_union.udpDstPort.mask =0xFFFF;
    sal_filter_igrAcl_field_add(&cfg, field2);
    cfg.activeport.value.bits[0]    = aclAllPortMask;
    cfg.activeport.mask.bits[0]     = aclAllPortMask;
    cfg.invert = FALSE;

    /* Action :  */
    act.actEnable[FILTER_ENACT_ADD_DSTPORT] = TRUE;
    act.filterPortmask.bits[0] = (1UL << (salPortIdCpu));
    act.actEnable[FILTER_ENACT_PRIORITY] = TRUE;
    act.filterPriority = 7;
	act.actEnable[FILTER_ENACT_POLICING_0] = TRUE;
    act.filterPolicingIdx[0]=2;
//    act.actEnable[FILTER_ENACT_EGRESSCTAG_TAG] = 1;

    sal_filter_igrAcl_cfg_add(3, &cfg, &act, &ruleNum);


	/*Rule 4: (DA=switch MAC) ' redirect to 8051 with priority 7*/
    memset(&cfg, 0x00, sizeof(rtk_filter_cfg_t));
    memset(&act, 0x00, sizeof(rtk_filter_action_t));
    memset(field1, 0, sizeof(rtk_filter_field_t));

    field1->fieldType = FILTER_FIELD_DMAC;
    field1->filter_pattern_union.dmac.dataType        = FILTER_FIELD_DATA_MASK;
    field1->filter_pattern_union.dmac.value.octet[0]  = this_mac[0];
    field1->filter_pattern_union.dmac.value.octet[1]  = this_mac[1];
    field1->filter_pattern_union.dmac.value.octet[2]  = this_mac[2];
    field1->filter_pattern_union.dmac.value.octet[3]  = this_mac[3];
    field1->filter_pattern_union.dmac.value.octet[4]  = this_mac[4];
    field1->filter_pattern_union.dmac.value.octet[5]  = this_mac[5];
    field1->filter_pattern_union.dmac.mask.octet[0]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[1]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[2]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[3]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[4]   = 0xFF;
    field1->filter_pattern_union.dmac.mask.octet[5]   = 0xFF;

    sal_filter_igrAcl_field_add(&cfg, field1);
    
    cfg.activeport.value.bits[0]    = aclAllPortMask;
    cfg.activeport.mask.bits[0]     = aclAllPortMask;
    cfg.invert = FALSE;

    /* Action :  */
    act.actEnable[FILTER_ENACT_REDIRECT] = TRUE;
    act.filterPortmask.bits[0] = (1UL << (salPortIdCpu));
    act.actEnable[FILTER_ENACT_PRIORITY] = TRUE;
    act.filterPriority = 7;

    sal_filter_igrAcl_cfg_add(4, &cfg, &act, &ruleNum);


	/*Rule 62: ARP ' mirror to 8051 with priority 0 + policing (meter 3)*/
    memset(&cfg, 0x00, sizeof(rtk_filter_cfg_t));
    memset(&act, 0x00, sizeof(rtk_filter_action_t));
    memset(field1, 0x00, sizeof(rtk_filter_field_t));

    field1->fieldType = FILTER_FIELD_ETHERTYPE;
    field1->filter_pattern_union.etherType.dataType        = FILTER_FIELD_DATA_MASK;
    field1->filter_pattern_union.etherType.value = ARP_ETHER_TYPE;
    field1->filter_pattern_union.etherType.mask = 0xffff;

    ret =sal_filter_igrAcl_field_add(&cfg, field1);
    if (RT_ERR_OK != ret)
    {
        printf("\n 11 ret=%lu Rule 62: ARP ' mirror to 8051 with priority 0 + policing (meter 3) \n",ret);
    }

    cfg.activeport.value.bits[0]    = aclAllPortMask;
    cfg.activeport.mask.bits[0]     = aclAllPortMask;
    cfg.invert = FALSE;

    /* Action :  */
    act.actEnable[FILTER_ENACT_ADD_DSTPORT] = TRUE;
    act.filterPortmask.bits[0] = (1UL << (salPortIdCpu));
    act.actEnable[FILTER_ENACT_PRIORITY] = TRUE;
    act.filterPriority = 1;
//	act.actEnable[FILTER_ENACT_POLICING_0] = TRUE;
//    act.filterPolicingIdx[0]=4;
//    act.actEnable[FILTER_ENACT_EGRESSCTAG_TAG] = 1;

    ret = sal_filter_igrAcl_cfg_add(62, &cfg, &act, &ruleNum);
    if (RT_ERR_OK != ret)
    {
        printf("\n 22 ret=%lu Rule 62: ARP ' mirror to 8051 with priority 0 + policing (meter 3) \n", ret);
    }
#if 1

	/*Rule 63: (SA=switch MAC) ' block*/
	memset(&cfg, 0x00, sizeof(rtk_filter_cfg_t));
    memset(&act, 0x00, sizeof(rtk_filter_action_t));
    memset(field1, 0, sizeof(rtk_filter_field_t));

    field1->fieldType = FILTER_FIELD_SMAC;
    field1->filter_pattern_union.smac.dataType        = FILTER_FIELD_DATA_MASK;
    field1->filter_pattern_union.smac.value.octet[0]  = this_mac[0];
    field1->filter_pattern_union.smac.value.octet[1]  = this_mac[1];
    field1->filter_pattern_union.smac.value.octet[2]  = this_mac[2];
    field1->filter_pattern_union.smac.value.octet[3]  = this_mac[3];
    field1->filter_pattern_union.smac.value.octet[4]  = this_mac[4];
    field1->filter_pattern_union.smac.value.octet[5]  = this_mac[5];
    field1->filter_pattern_union.smac.mask.octet[0]   = 0xFF;
    field1->filter_pattern_union.smac.mask.octet[1]   = 0xFF;
    field1->filter_pattern_union.smac.mask.octet[2]   = 0xFF;
    field1->filter_pattern_union.smac.mask.octet[3]   = 0xFF;
    field1->filter_pattern_union.smac.mask.octet[4]   = 0xFF;
    field1->filter_pattern_union.smac.mask.octet[5]   = 0xFF;

    sal_filter_igrAcl_field_add(&cfg, field1);
    
    cfg.activeport.value.bits[0]    = aclAllPortMask;
    cfg.activeport.mask.bits[0]     = aclAllPortMask;
    cfg.invert = FALSE;

    /* Action :  */
    act.actEnable[FILTER_ENACT_REDIRECT] = TRUE;
    act.filterPortmask.bits[0] = 0x0;
    
    sal_filter_igrAcl_cfg_add(63, &cfg, &act, &ruleNum);
    free(field1);
	free(field2);
#endif


	
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
    
#if 0
    /* change to egress flow control mode */
    setReg(0x121d, 0xfc06);
    setReg(0x938, 0x7FFF);
    setReg(0x937, 0xFF);
#endif

    /* enable all storm */
    setReg(0xa40, phyAllPortMask);
    setReg(0xa41, phyAllPortMask);    
    setReg(0xa42, phyAllPortMask);
    setReg(0xa43, phyAllPortMask);    

    /* assign meters to storm */
    //#ifdef LWS_5G_1F
    /* meter allocation:
                bc      mc      unmc    unda
       port0    4       5       6       7
       port1    8       9       10      11
       ...
       port4    20      21      22      23
       port5    x       x       x       x
       port6    24      25      26      27
    */
    
    //#else
    /* meter allocation:
                bc      mc      unmc    unda
       port0    8      9        10      11
       port1    12      13        14      15
       ...
       port4    24      25        26      27
       port5    28      29        30      31
       port6    32      33        34      35
       port7    36      37        38      39   
       port8    40      41        42      43
       port9    44      45        46      47
    */

    for(i = 0; i < INTERFACE_NUMBER;i++)
    {
#ifdef LWS_5G_1F
        if(i == 5)
        {
            i=i+1;//fiber use port 6
            j = 5;
        }
        else
        {
            j = i;
        }
        rtl8367c_setAsicStormFilterBroadcastMeter(i, 4 + j * 4);
        rtl8367c_setAsicStormFilterMulticastMeter(i, 5 + j * 4);
        rtl8367c_setAsicStormFilterUnknownMulticastMeter(i, 6 + j * 4);
        rtl8367c_setAsicStormFilterUnknownUnicastMeter(i, 7 + j * 4);        
#else
        rtl8367c_setAsicStormFilterBroadcastMeter(i, 8 + i * 4);
        rtl8367c_setAsicStormFilterMulticastMeter(i, 9 + i * 4);
        rtl8367c_setAsicStormFilterUnknownMulticastMeter(i, 10 + i * 4);
        rtl8367c_setAsicStormFilterUnknownUnicastMeter(i, 11 + i * 4);        
#endif
    }
    
    /*ACL rule 2*/
	rtl8367c_setAsicShareMeter(0,1,0);
	/*ACL rule 4*/
    rtl8367c_setAsicShareMeter(1,1,0);
	/*ACL rule 61*/
	rtl8367c_setAsicShareMeter(2,1,0);
	/*ACL rule 62*/
	rtl8367c_setAsicShareMeter(3,1,0);

    /* Enable Special congest */
    for(i = 0; i < INTERFACE_NUMBER; i++)
    {
#ifdef LWS_5G_1F
        if(i == 5)
            i=i+1;//fiber use port 6    
#endif            
        rtl8367c_setAsicSpecialCongestModeConfig(i, 2);
    }
    /* enable vlan ingress filtering */
    for(i = 0; i < INTERFACE_NUMBER; i++)
    {
#ifdef LWS_5G_1F
        if(i == 5)
            i=i+1;//fiber use port 6
#endif            
        rtl8367c_setAsicVlanIngressFilter(i, TRUE);
    }

    /*
    1.meter 0~7 is for acl use.
    2.each port uses 4 meters for storm control
    //#define BCAST_METER(port)    ((port) * 4 + 8)
    //#define MCAST_METER(port)    ((port) * 4 + 9)
    //#define UNMC_METER(port)     ((port) * 4 + 10)
    //#define UNDA_METER(port)     ((port) * 4 + 11)
    3.67C's 8051 port apr is from 4*5 + 8 = 28
    4.70B's 8051 port apr is from 4*8 + 8 = 40
    */
#if (defined(LWS_5G) || defined(LWS_5G_1F))//UTP port number is 5, RTL8367C
    /* map 8051 port's queue number to 8 */
    tmp = getReg(RTL8367C_REG_QOS_PORT_QUEUE_NUMBER_CTRL1);
    setReg(RTL8367C_REG_QOS_PORT_QUEUE_NUMBER_CTRL1,tmp & 0x0FFF);

    rtl8367c_setAsicAprEnable(portIdCput, 1); //enable cpu port apr
    rtl8367c_setAsicAprMeter(portIdCput, 0, 5); //set cpu port queue 0 apr meter to index 29
    rtl8367c_setAsicAprMeter(portIdCput, 1, 4); //set cpu port queue 1 apr meter to index 28
    rtl8367c_setAsicAprMeter(portIdCput, 2, 7); //set cpu port queue 2 apr meter to index 31
    rtl8367c_setAsicAprMeter(portIdCput, 3, 7); //set cpu port queue 3 apr meter to index 31
    rtl8367c_setAsicAprMeter(portIdCput, 4, 7); //set cpu port queue 4 apr meter to index 31
    rtl8367c_setAsicAprMeter(portIdCput, 5, 7); //set cpu port queue 5 apr meter to index 31
    rtl8367c_setAsicAprMeter(portIdCput, 6, 7); //set cpu port queue 6 apr meter to index 31
    rtl8367c_setAsicAprMeter(portIdCput, 7, 7); //set cpu port queue 7 apr meter to index 31
    rtl8367c_setAsicShareMeter(28, 1, 0); //meter to index 28 with rate 8kb
    rtl8367c_setAsicShareMeter(29, 1, 0); //meter to index 29 with rate 8kb
#endif        
#if (defined(LWS_8G) || defined(LWS_8G_2F))//UTP port number is 10, RTL8370B
    /* map 8051 port's queue number to 8 */
    tmp = getReg(RTL8367C_REG_QOS_PORT_QUEUE_NUMBER_CTRL2);
    setReg(RTL8367C_REG_QOS_PORT_QUEUE_NUMBER_CTRL2,tmp & 0xF0FF);

    rtl8367c_setAsicAprEnable(portIdCput, 1); //enable cpu port apr
    rtl8367c_setAsicAprMeter(portIdCput, 0, 5); //set cpu port queue 0 apr meter to index 45
    rtl8367c_setAsicAprMeter(portIdCput, 1, 4); //set cpu port queue 1 apr meter to index 44
    rtl8367c_setAsicAprMeter(portIdCput, 2, 7); //set cpu port queue 2 apr meter to index 47
    rtl8367c_setAsicAprMeter(portIdCput, 3, 7); //set cpu port queue 3 apr meter to index 47
    rtl8367c_setAsicAprMeter(portIdCput, 4, 7); //set cpu port queue 4 apr meter to index 47
    rtl8367c_setAsicAprMeter(portIdCput, 5, 7); //set cpu port queue 5 apr meter to index 47
    rtl8367c_setAsicAprMeter(portIdCput, 6, 7); //set cpu port queue 6 apr meter to index 47
    rtl8367c_setAsicAprMeter(portIdCput, 7, 7); //set cpu port queue 7 apr meter to index 47
    rtl8367c_setAsicShareMeter(52, 1, 0); //meter to index 44 with rate 8kb
    rtl8367c_setAsicShareMeter(53, 1, 0); //meter to index 45 with rate 8kb
#endif

//    sal_led_blinkRate_set(1);


}

#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
// Speed Change Reset: Performs reset sequence for fiber port speed change
// Toggles SDS page register bits to reset CDR and force speed change
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
// Fiber Port 4 Initialization: Configures port 4 for fiber mode with auto-negotiation
// Sets up SERDES registers and MAC for fiber operation on port 4
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
// Fiber Port 6 Auto Detect: Configures auto-detection for fiber port 6 mode
// Sets up SERDES registers and enables auto-negotiation for fiber operation
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

// Enable Down Speed: Enables forced down speed configuration for fiber port
// Sets register bit to allow speed downgrade when link partner cannot support 1G
void enable_down_speed()
{
    //  enable  cfg_frc_dwspd
    setReg(0x6602, 0x0f20);
    setReg(0x6601, 0x0001);
    setReg(0x6600, 0x00c0);
}

// Auto Giga with Force 1G: Configures auto-negotiation with force 1G capability
// Sets SERDES register bits for auto-detect with 1G forced mode
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

// Fiber Port 6 Initialization: Complete initialization sequence for fiber port 6
// Combines auto-detect, down speed enable, and auto-negotiation configuration
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
// Set SERDES Register: Writes data to SERDES register with page and index addressing
// Handles busy-wait for register access completion and returns SUCCESS/FAILED status
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

// Power Up PHY: Powers up SERDES interfaces for both SDS channels
// Sets power-up bits in registers for SDS 0 and SDS 1
void pup_phy()
{
    
    setReg(0x6200, 0x1340);   //power up sds 0
    setReg(0x6210, 0x1340);   //power up sds 1

}

// Base Patch: Applies SERDES patches for fiber port 70MB operation
// Configures RX offset, down speed, and HISGMII patches for both SDS channels
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

// All Initialization: Main system initialization function for all hardware and software components
// Initializes clock, memory, interrupts, switch, NIC, UART, flash, timer, LWPS, and ACL
void all_init(void) large{

	//=====================ACL Variables==========================
	
	uint8   getvlan_id[5];
	
	 // sal_ipaddr_t    gateway = 0;
		sal_ipaddr_t    ip = 0;
   // sal_ipaddr_t    netmask = 0;
	
	
	
	//=====================ACL Variables==========================
	char timestamp[] = __DATE__;
	
    uint16      tmp,i;
    sal_mac_t   mac;
#if LWPS_RSTP
    st_desc_t desc;
#endif

#if defined(FIBER_PORT_70MB)
    base_patch();
#endif    

	change_clock(DW8051_CLK);
	CKCON = 0x0; //Stretch value is 0
	event_list = 0;
	//event_list |= NIC_RX_EVENT;
	DATA_BANK = 0;
    pkt_head = (p_buffer+TAG_SIZE+FRAME_INFO_SIZE);
	mem_init();
	interrupt_init();
	switch_init();
	nic_init();    
	uart_init();
	flash_init();   //spi_flash_driver_info();
    for(i=0;i<0xffff;i++) //add delay for spi flash to finish initial
        ;	
	igmp_init();
	timer_init();

    lwps_init();

#if LWPS_SNMP
    printf("\nBefore SNMP_INIT\n");
	snmp_init();
    printf("\nAfter SNMP_INIT\n");
	mib_init();
#endif



#if LWPS_RSTP
    //rstp_init();
    /* disable RSTP */
    //desc.target = RSTP_SET_DESC_FUNC_ENABLE;
    //desc.value = DISABLE;
    //rstp_set(&desc);
#endif

#if LWPS_LLDP
    initializeLLDP();
    lldp_set(FALSE);
#endif

#if LWPS_DHCPC
    dhcpc_init();
#endif /* endof LWPS_DHCPC */

#if SAL_RLPP
    //rlpp_init();
#endif

#if WEB_ENABLE
    httpd_init();
#endif /* end of WEB_ENABLE */
    
    set_8051RDY();

#if defined(FIBER_PORT_70MB)
    pup_phy();
#endif

    sal_filter_igrAcl_init();
    sal_filter_igrAcl_state_set(salPortIdCpu, DISABLED);
    sal_flash_config_init();

    /* check whether non-volatile MAC address is valid */
    sal_flash_sysMac_get(&mac);

    /* if it is invalid, use non-volatile MAC as MAC address */
    if(mac.octet[0] != 0xFF || mac.octet[1] != 0xFF || mac.octet[2] != 0xFF ||
        mac.octet[3] != 0xFF || mac.octet[4] != 0xFF || mac.octet[5] != 0xFF)
    {
        sal_sys_mac_set_cache(&mac);
    }
    /* else, MAC address will following configuration, so do nothing here */

    cpu_patch();

    /* enable phy */
//    rtk_port_phyEnableAll_set(ENABLED);

#if 0//LWPS_RESET
    /* initialize GPIO 2 */
    sal_gpio_init(SAL_GPIO_ONCHIP_2);
    /* set direction of GPIO 2 as input for factory reset */
    sal_gpio_dir_set(SAL_GPIO_ONCHIP_2, SAL_GPIO_DIR_INPUT);
#endif
    handler_init();

    /* enable interrupt */	
    EA  = 1;
    
	/*update this internal message before release*/
	for(i=0;i<(sizeof(timestamp)/2 +1);i++)
	{
		tmp = timestamp[i*2];
		if(sizeof(timestamp)&1)
			tmp = (tmp << 8);
		else
			tmp = (tmp << 8) | timestamp[i*2+1];
		
		setReg(RTL8367C_REG_DUMMY_1731-i,tmp);	
	}

//    setReg(0x1d3f, 0x0000); //Force deglitch disable
#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
fiber_init();
#endif

#if LWPS_SYSLOG
	printf("\nBefore Syslog_INIT\n");
	syslog_init();
	
	printf("\nAfter Syslog_INIT\n");
#endif
//========================== ACL START=============================
	
	if(sal_flash_vlan_id_get(FLSH_CONFIG_DUMMY_START , 4 ,getvlan_id) == 0)
		{
			printf("Read VLAN ID inside flash Memory : %s\r\n" , getvlan_id);
			//memcpy(vlan_id,getvlan_id,4);
			getvlan_id[4] = '\0';  // Null-terminate
			printf("Vlan ID : %s\r\n",getvlan_id);
		}
		sal_sys_ip_get(&ip, &netmask);
		vlanIds = (u16_t)atoi((char *)getvlan_id);
	
		if(strncmp(getvlan_id,"NEW ",4)!=0)
		{
		add_ip_over_vlan_acl_rule(5,vlanIds,ip,netmask);
		}
		else{
			printf("NEW Device Detected - No Vlan IP Configuration Found \r\n");
		}
	  
		//add_ip_over_vlan_acl_rule(5,vlanIds,ip,netmask);
	
	
	//========================== ACL END=============================

}