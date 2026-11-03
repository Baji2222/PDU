#include <stdio.h>

#include <local/config.h>
#include <common.h>
#include <reg_sfr.h>
#include <asicreg.h>
#include <lwps/lwps_api.h>
#include <dhcpc/dhcpc.h>

#include <rtl8367c_reg.h>

#include <utility/firmware_upgrade.h>

#include <salflash.h>
#include <salgpio.h>

#if WEB_ENABLE
#include <sal/salsys.h>
#endif

#if LWPS_SNMP
#include <snmp/snmp.h>
#endif /* end of LWPS_SNMP */

#if RTK_PROTO_RLPP
#include <rtk_proto/rlpp/rlpp.h>
#include <salled.h>
#endif


#if LWPS_LLDP
#include <lldp/tx_sm.h>
#include <lldp/rx_sm.h>
#endif

#if LWPS_RSTP
#include <lwps/rstp.h>
#define RSTP_TICK() rstp_tick()
#define RSTP_LINK_CHANGE_HANDLER(x) rstp_link_change_handler(x)
#else
#define RSTP_TICK()
#define RSTP_LINK_CHANGE_HANDLER(x)
#endif


#include <handler.h>
#include <salport.h>

#define AUTO_SAVE_TIMER         120

#define FACTORY_RESET_INTVAL    3
#define IP_USER_RESET_INTVAL    3

#if LWPS_IGMP
#include <saligmp.h>
#endif

extern uint16 r_mdc_mdio(uint8 PHY_ADDR,uint8 REG_ADDR);
extern void w_mdc_mdio(uint8 PHY_ADDR,uint8 REG_ADDR,uint16 Odata) ;
extern uint8 i2c_read_byte(uint8 slave_addr, uint8 reg_addr);

#if RTK_PROTO_RRCPV3
uint16 delayRebootFlag = 0;
#endif

// Switch ISR: Empty interrupt handler for switch interrupts
// Serves as placeholder for switch interrupt service routine
void switch_isr() interrupt 0
{
	;
}



#if defined (FIBER_PORT_70MB)
uint16 timer3000_global_flag0 = 0;
uint16 timer3000_global_flag1 = 0;

extern int8 setSerdesReg(uint16 sds_index, uint16 sds_reg, uint16 sds_page, uint16 regData);
#endif

#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
uint16 timer500_global_flag = 0;
#endif

// Timer 0 ISR: Handles timer0 interrupt for system timekeeping and periodic events
// Generates 100ms, 500ms, 3000ms and 1-second events for various system tasks
void timer_0_isr() interrupt 1
{
    static uint8 timeCount100ms = 0;
#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
    static uint8 timeCount500ms = 0;
#endif
#if defined (FIBER_PORT_70MB)
    static uint16 timeCount3000ms = 0;
#endif    
    TH0 = _th0;
    TL0 = _tl0;
    
    timeCount++;
    timeCount100ms++;
#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
    timeCount500ms++;
#endif
#if defined (FIBER_PORT_70MB)
    timeCount3000ms++;
#endif    
    /* 100ms event */
    if(timeCount100ms == 20)
    {
        timeCount100ms = 0;
        event_list |= TIMER_100MS_EVENT;
    }
    
#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
    /* 500ms event */
    if(timeCount500ms == 100)
    {
        timeCount500ms = 0;
        timer500_global_flag = 1;
    }
#endif
#if defined(FIBER_PORT_70MB)
        /* 3000ms event */
        if(timeCount3000ms == 600)
        {
            timeCount3000ms = 0;
            timer3000_global_flag0 = 1;
            timer3000_global_flag1 = 1;
        }
#endif


    if ( timeCount == CONFIG_TIMER_DIVISOR )
    {
        timeCount = 0;
        current_time++;
        up_time++;
        event_list |= TIMER_SEC_EVENT;
    }

}

// NIC ISR: Handles Network Interface Controller receive interrupt
// Clears interrupt status and sets NIC_RX_EVENT flag for packet processing
void nic_isr() interrupt 2
{
	EX1 = 0;
	/* clear NIC IMS */
    INDACC_ADDR_H  = 0x1A;
    INDACC_ADDR_L  = 0x0E;
    INDACC_WDATA_L = 0x80;
    INDACC_CMD     = 0x03;
    while( INDACC_STATUS != 0);

	event_list |= NIC_RX_EVENT;

}
                       
// NIC RX DSR: Deferred service routine for processing received network packets
// Reads packets from NIC buffer and passes them to protocol handlers
void nic_rx_dsr(void){       
	uint16 xdata times = 0;
    int32  xdata ret;
	uint8  *u8_ptr;
	                         
	while( (times<CONFIG_MAX_RX_PER_INTR)            && \
	       (TRUE==hasRxPkt())                        && \
	       (SUCCESS==rxPkt(p_buffer, P_BUFFER_SIZE))    \
	)
	{
		u8_ptr            = p_buffer + TAG_SIZE;
		/* size is not include crc length */
	    pktbuf.size       = (u8_ptr[6] << 8) | u8_ptr[7];
		pktbuf.frame_info = u8_ptr;
		pktbuf.pkt_head   = pkt_head;
		pktbuf.l2         = pkt_head;


        /* call user-defined L2 handler */
        if(eth_up(&pktbuf) == LWPS_ERR_L2_NDEF)
            handler_net_l2(&pktbuf);
        
		ret = lwps_event_handler();
		times++;
	}
	
	if( times<CONFIG_MAX_RX_PER_INTR ){
		//event_list &= (~NIC_RX_EVENT);
		//EX1 = 1;
	}
	
}

// Timer 100ms DSR: Handles GPIO button debounce and factory reset detection
// Monitors GPIO for press events and triggers reset or factory reset based on press duration
void timer_100ms_dsr()
{
#define GPIO_DEBOUNCE_TIME_MAX  5   /* debounce time: 500ms */
#define GPIO_REBOOT_TIME        10  /* 1s */
#define GPIO_INPUT_NORMAL       0
#define GPIO_INPUT_PRESSED      1
#define GPIO_INPUT_RELEASED     2
#define GPIO_PRESSED            0
#define GPIO_RELEASED           1

#if 1//LWPS_RESET

    static uint8    gpio_state = GPIO_INPUT_NORMAL;
    uint8           current_input;
    static uint16   pressedTime;  /* unit: 100ms */
    static uint8    debounceTime;
#endif
    event_list &= (~TIMER_100MS_EVENT);
#if 1//LWPS_RESET
    sal_gpio_input_get(SAL_GPIO_ONCHIP_39, &current_input);    

    if(current_input)
        current_input = 1;

    switch(gpio_state)
    {
    case GPIO_INPUT_NORMAL:
        /* detect the pressed event, start to count pressed time */
        if(GPIO_PRESSED == current_input)
        {
            gpio_state = GPIO_INPUT_PRESSED;
            pressedTime = 0;
        }
        break;
    case GPIO_INPUT_PRESSED:
        pressedTime++;
        /* detect the release event, start to count debounce time */
        if(GPIO_RELEASED == current_input)
        {
            gpio_state = GPIO_INPUT_RELEASED;
            debounceTime = 0;
        }
        /* pressed for more than 3s */
        if(pressedTime > GPIO_RESET_TIME)
        {
            sal_flash_cfg_reset();
            sal_sys_system_reboot();
        }
        break;
    case GPIO_INPUT_RELEASED:
        pressedTime++;
        /* continue to count debounce time */
        if(GPIO_RELEASED == current_input)
        {
            debounceTime++;
            /* debounce time end, check result */
            if(debounceTime >= GPIO_DEBOUNCE_TIME_MAX)
            {
    gpio_state = GPIO_INPUT_NORMAL;

                /* reverse pressed time to the moment that GPIO is released */
                pressedTime -= GPIO_DEBOUNCE_TIME_MAX;

                /* pressed under 1s */
                if(pressedTime < GPIO_REBOOT_TIME)
                {
                    setReg(0x1322, 1);
                }
            }
        }
        else
        {
            gpio_state = GPIO_INPUT_PRESSED;
        }
        break;
    }
#endif
}

extern void check_port_setting();

// Timer Second DSR: Handles all 1-second periodic system tasks
// Processes DHCP, SNMP, ARP, TCP, UDP, IGMP, RLPP, RSTP and other protocol timers
void timer_sec_dsr(void){
#if LWPS_SNTPC    
	static uint8 times = 0;    
#endif
#if RTK_PROTO_RRCPV3
    static uint8 rrcpResetTime = 0;
#endif
#if RTK_PROTO_RLPP
    uint8   port = 0;
    static uint8   flag = 0;
    sal_loop_port_status_t  loopStatus;
    static uint8 status[COMBO_PORT_NUM] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#endif

	event_list &= (~TIMER_SEC_EVENT);
#if LWPS_SNTPC        
    if( times>=10 )
    {
        sntpc_update_time();
        times = 0;
    }
    times++;
#endif    
	
#if LWPS_SNMP

  #if LWPS_SNMP_TRAP_ENABLE
    snmp_timer_tick();
  #endif /*end of LWPS_SNMP_TRAP_ENABLE */
  
#endif /* end of LWPS_SNMP */
	
	//OSAL_PRINTF("CT=%lu\n", current_time);
	etharp_timer();
	tcp_timer();
	udp_timer();

#if LWPS_IGMP	
	igmpdb_timer(1);
#endif

#if RTK_PROTO_RLPP
    pktbuf.l2 = p_buffer + TAG_SIZE;
    pktbuf.pkt_head = pktbuf.l2;
    rlpp_tick();

    //If port is in RLPP block state, force LED ON / OFF every one second.
    for(port=0; port < SAL_PORT_ID_CPU; port+=1)
    {
        if(sal_loop_portRange_check(port) != 0)
            continue;
    	sal_loop_portStatus_get(port, &loopStatus);
    	if (SAL_LOOP_STATUS_RLPP == loopStatus)
    	{
    	    status[port] = 1;
    	    if(flag == 0)
    	    {
    	        sal_led_forceMode_set(port, SAL_LED_FORCE_OFF);
    	    }
    	    else
    	    {
    	        sal_led_forceMode_set(port, SAL_LED_FORCE_ON);
    	    }
    	}
    	else
    	{
    	    if(status[port] == 1)
    	    {
                sal_led_forceMode_set(port, SAL_LED_FORCE_NORMAL);
            }
            status[port] = 0;
    	}
    }
    if(flag == 0)
        flag = 1;
    else if(flag == 1)
        flag = 0;
    else
    {
    }


#endif    
#if LWPS_DHCPC
    dhcpc_timer(1000);
#endif

    RSTP_TICK();

#if LWPS_SNTPC    
    sntpc_routine();
#endif

    handler_timer();
    
    check_port_setting();
#if RTK_PROTO_RRCPV3
        if(delayRebootFlag == 1)
        {
            rrcpResetTime++;
            if(rrcpResetTime > 15) //delay 15 seconds
                setReg(0x1322,1); //reboot
        }
#endif    
    
}

// Link Status Change: Handles link status change events for all protocols
// Notifies IGMP, RSTP, SNMP, and RLPP about link state changes
uint8 link_status_change(portmask_type currUpPmsk, portmask_type prevUpPmsk)
{
#if LWPS_IGMP
	sal_enable_t enable;

    sal_igmp_state_get(&enable);
    if(enable == ENABLE)
    {
	igmp_group_linkStatus_change(currUpPmsk, prevUpPmsk);
    }
#endif /* end of LWPS_IGMP */
	
#if LWPS_RSTP
	RSTP_LINK_CHANGE_HANDLER(currUpPmsk);
#endif

#if LWPS_SNMP
  #if LWIP_SNMP &&LWPS_SNMP_TRAP_ENABLE
    snmp_linkChange_cb(currUpPmsk);
  #endif /* end of LWIP_SNMP&&LWPS_SNMP_TRAP_ENABLE */
#endif

#if RTK_PROTO_RLPP
    RLPP_link_change_handler(currUpPmsk,prevUpPmsk);
#endif

    /* kill warning message */
    if(currUpPmsk & prevUpPmsk){}

    return SUCCESS;
}

// Get Current Link Status: Reads link status from all switch ports
// Returns port mask of all currently active/linked ports
void getCurrLinkStatus(portmask_type *pmsk)
{
    uint8  i;
	uint16 tmp;
	uint32 tmpmsk;

    tmpmsk = 0;

	for(i = 0; i < SAL_MAX_PHY_PORT; i++)
	{
#if defined(LWS_5G_1F)   	
        if(i == 5)
	        tmp = getReg(RTL8367C_REG_PORT0_STATUS + i + 1);
	    else
            tmp = getReg(RTL8367C_REG_PORT0_STATUS + i);
#else
        tmp = getReg(RTL8367C_REG_PORT0_STATUS + i);
#endif
		if ( (tmp & RTL8367C_PORT0_STATUS_LINK_STATE_MASK) != 0 )
		{
#if defined(LWS_8G_2F)
            if (i == 8){
                tmpmsk |= (1UL << EXT_PORT0);
            }
            else if (i == 9){
                tmpmsk |= (1UL << EXT_PORT1);
            }   
            else
            {
                tmpmsk |= (1UL << i);
            }
#elif defined(LWS_5G_1F)       
            if (i == 5){
                tmpmsk |= (1UL << EXT_PORT0);
            }   
            else{
                tmpmsk |= (1UL << i);
            }   
#else
            tmpmsk |= (1UL << i);
#endif
		}
	}
	*pmsk = tmpmsk;
}

#if defined(FIBER_PORT_4)
// Toggle Bit Error Reset: Toggles fiber port bit error reset for port 4
// Writes to SERDES registers to reset bit error detection circuit
void toggle_bit_error_reset()
{
    setReg(0x6602, 0x0004);
    setReg(0x6601, 0x0400);
    setReg(0x6600, 0x00c0);
    setReg(0x6602, 0     );
    setReg(0x6601, 0x0400);
    setReg(0x6600, 0x00c0);
}

// Fiber Port 4 Event: Monitors fiber link status and handles link down recovery
// Forces MAC link down and toggles bit error reset when link is lost
void fiber_event(void)
{
    uint16 tmp;
    static uint8 flag_500ms = 0;
    uint16 tmp2, i;
    static uint8 flag = 0;
    
    tmp = getReg(0x6201);
    if(tmp & (1 << 2)) //linkup
    {
        if(flag == 1)
        {
            tmp2 = getReg(0x13ea);   //get fiber ability
            tmp2 &= ~(1 << 4);       //set MAC4 link down and bypass other ability
            tmp2 |= (1 << 12);       //enable MAC ability force mode
            setReg(0x1316, tmp2);    //force MAC4 link down
            for(i=0;i<0x6fff;i++);    

            tmp2 &= ~(1 << 12);      //disable MAC ability force mode
            setReg(0x1316, tmp2);    //release force MAC4 link down
            flag = 0;
        }
        
        flag_500ms = 0;
    }
    else //linkdown
    {
        if(flag_500ms == 0)
        {
            tmp2 = getReg(0x1356);   //get MAC4 current status
            tmp2 &= ~(1 << 4);       //set MAC4 link down
            tmp2 |= (1 << 12);       //enable MAC ability force mode
            setReg(0x1316, tmp2);    //force MAC4 link down
            flag = 1;
                
            //toggle bit error reset
            toggle_bit_error_reset();
            
            timer500_global_flag = 0;

            flag_500ms = 1;
        }
        else
        {
            if(timer500_global_flag == 1)//500ms
            {
                tmp2 = getReg(0x1356);   //get MAC4 current status
                tmp2 &= ~(1 << 4);       //set MAC4 link down
                tmp2 |= (1 << 12);       //enable MAC ability force mode                
                setReg(0x1316, tmp2);    //force MAC4 link down
                flag = 1;
                    
                //toggle bit error reset
                toggle_bit_error_reset();
                
                timer500_global_flag = 0;

                flag_500ms = 1;
            }                
        }    
    }
}
#endif

#if defined(FIBER_PORT_6)
// Toggle Bit Error Reset for 5UTP+1Fiber: Resets bit error for fiber port 6 with link down sequence
// Forces link down, toggles bit error reset, then releases force down
void toggle_bit_error_reset_5UTP_1FIBER()
{
    uint16 i;

    //force linkdown fiber port(MAC 6)
    setReg(0x134a, 0x2);
    setReg(0x6602, 0x7146);
    setReg(0x6601, 0x3);
    setReg(0x6600, 0x00c0);
    for(i=0;i<100;i++); //delay
    
    //toggle bit error reset
    setReg(0x6602, 0x0004);
    setReg(0x6601, 0x0400);
    setReg(0x6600, 0x00c0);
    setReg(0x6602, 0     );
    setReg(0x6601, 0x0400);
    setReg(0x6600, 0x00c0);
    
    //release force linkdown fiber port(MAC 6)
    setReg(0x134a, 0);
    setReg(0x6602, 0x7106);
    setReg(0x6601, 0x3);
    setReg(0x6600, 0x00c0);
}

// Fiber Port 6 Event: Monitors fiber port 6 link status and triggers recovery on link down
// Performs bit error reset sequence at 500ms intervals when link is lost
void fiber_event(void)
{
    uint16 tmp;
    static uint8 flag_500ms = 0;
    
    tmp = getReg(0x6201);
    tmp = getReg(0x6201);
    if(tmp & (1 << 2)) //linkup
    {
        flag_500ms = 0;
    }
    else //linkdown
    {
        if(flag_500ms == 0)
        {
            //toggle bit error reset
            toggle_bit_error_reset_5UTP_1FIBER();
            
            timer500_global_flag = 0;

            flag_500ms = 1;
        }
        else
        {
            if(timer500_global_flag == 1)//500ms
            {
                //toggle bit error reset
                toggle_bit_error_reset_5UTP_1FIBER();
                
                timer500_global_flag = 0;

                flag_500ms = 1;
            }                
        }    
    }
}
#endif

#if defined (FIBER_PORT_70MB)
// Fiber 0 Event for 70MB: Monitors SDS0 link and performs recovery on link down
// Toggles RX enable and force link down with 3-second interval polling
void fiber0_event_70MB(void)
{
    uint16 tmp, i;
    static uint8 flag_3000ms0 = 0;
    uint16 togglenum0=0;

    //monitor sds0	 
    tmp = getReg(0x6201);
    tmp = getReg(0x6201);
    if(tmp & (1 << 2)) //linkup
    {
        flag_3000ms0 = 0;
    }
    else //linkdown
    {
        if(flag_3000ms0 == 0)
        {
            //toggle bit error reset
            //patch rx_en reset sds1
             togglenum0++;
             setReg(0xba4, togglenum0); 
             setReg(0x13c4, 0x66);                // force SDS0/Port 9 link down
             setReg(0x13c4, 0x1066);              //toggle force setting
             while(!(getReg(0x1d9d) & 0x200)){;}  //wait until traffic clear
             for(i=0;i<400;i++);                 //delay (0.3ms)
             setReg(0x133d,0x02);               //stop gli clk
             for(i=0;i<400;i++);                 //delay (0.3ms)
            
             setSerdesReg(0,0,0x0,0x1400);       //toggle rx_en_on,tx_en_on
             for(i = 0; i < 12480; i++);        //delay (12ms)
             setSerdesReg(0,0,0x0,0x1403);
             for(i = 0; i < 6240; i++);        //delay (6ms)
            
             setReg(0x133d,0x0);                //release gli clk
             for(i=0;i<400;i++);                 //delay (0.3ms)
             setReg(0x13c4, 0x66);                //release force 

            
            timer3000_global_flag0 = 0;

            flag_3000ms0 = 1;
        }
        else
        {
            if(timer3000_global_flag0 == 1)//3000ms
            {
                //toggle bit error reset
                //patch rx_en reset sds1
                 togglenum0++;
                 setReg(0xba4, togglenum0); 
                 setReg(0x13c4, 0x66);                // force SDS0/Port 9 link down
                 setReg(0x13c4, 0x1066);              //toggle force setting
                 while(!(getReg(0x1d9d) & 0x200)){;}  //wait until traffic clear
                 for(i=0;i<400;i++);                 //delay (0.3ms)
                 setReg(0x133d,0x02);               //stop gli clk
                 for(i=0;i<400;i++);                 //delay (0.3ms)
                
                 setSerdesReg(0,0,0x0,0x1400);       //toggle rx_en_on,tx_en_on
                 for(i = 0; i < 12480; i++);        //delay (12ms)
                 setSerdesReg(0,0,0x0,0x1403);
                 for(i = 0; i < 6240; i++);        //delay (6ms)
                
                 setReg(0x133d,0x0);                //release gli clk
                 for(i=0;i<400;i++);                 //delay (0.3ms)
                 setReg(0x13c4, 0x66);                //release force 

                
                timer3000_global_flag0 = 0;

                flag_3000ms0 = 1;
            }                
        }    
    }
}

// Fiber 1 Event for 70MB: Monitors SDS1 link and performs recovery on link down
// Similar to fiber0 but for second SDS channel with independent control
void fiber1_event_70MB(void)
{
    uint16 tmp, i;
    static uint8 flag_3000ms1 = 0;
    uint16 togglenum1=0;

    //monitor sds1	 
    tmp = getReg(0x6211);
    tmp = getReg(0x6211);
    if(tmp & (1 << 2)) //linkup
    {
        flag_3000ms1 = 0;
    }
    else //linkdown
    {
        if(flag_3000ms1 == 0)
        {
            //toggle bit error reset
            //patch rx_en reset sds1
             togglenum1++;
             setReg(0xba3, togglenum1);
            
             setReg(0x1311, 0x66);                // force SDS1/Port 8 link down
             setReg(0x1311, 0x1066);              //toggle force setting
             while(!(getReg(0x1d9d) & 0x100)){;}  //wait until traffic clear
             for(i=0;i<400;i++);                 //delay (0.3ms)
             setReg(0x133d,0x02);               //stop gli clk
             for(i=0;i<400;i++);                 //delay (0.3ms)
            
             setSerdesReg(1,0,0x0,0x1400);       //toggle rx_en_on ,tx_en_on    
             for(i = 0; i < 12480; i++);        //delay(12ms) 
             setSerdesReg(1,0,0x0,0x1403);
             for(i = 0; i < 6240; i++);        //delay (6ms)
            
             setReg(0x133d,0x0);                //release gli clk
             for(i=0;i<400;i++);                 //delay (0.3ms)
             setReg(0x1311, 0x66);                //release force 


            
            timer3000_global_flag1 = 0;

            flag_3000ms1 = 1;
        }
        else
        {
            if(timer3000_global_flag1 == 1)//3000ms
            {
                //toggle bit error reset
                //patch rx_en reset sds1
                 togglenum1++;
                 setReg(0xba3, togglenum1);
                
                 setReg(0x1311, 0x66);                // force SDS1/Port 8 link down
                 setReg(0x1311, 0x1066);              //toggle force setting
                 while(!(getReg(0x1d9d) & 0x100)){;}  //wait until traffic clear
                 for(i=0;i<400;i++);                 //delay (0.3ms)
                 setReg(0x133d,0x02);               //stop gli clk
                 for(i=0;i<400;i++);                 //delay (0.3ms)
                
                 setSerdesReg(1,0,0x0,0x1400);       //toggle rx_en_on ,tx_en_on    
                 for(i = 0; i < 12480; i++);        //delay(12ms) 
                 setSerdesReg(1,0,0x0,0x1403);
                 for(i = 0; i < 6240; i++);        //delay (6ms)
                
                 setReg(0x133d,0x0);                //release gli clk
                 for(i=0;i<400;i++);                 //delay (0.3ms)
                 setReg(0x1311, 0x66);                //release force 


                
                timer3000_global_flag1 = 0;

                flag_3000ms1 = 1;
            }                
        }    
    }    
}
#endif

/* The following are DSR */
// Event Dispatch: Main event dispatcher that processes all pending system events
// Handles timer events, NIC RX, link status changes, UART commands, and fiber events
void event_dispatch(void){
	portmask_type        currLinkUpPmsk;
	static portmask_type prevLinkUpPmsk = 0;
	
//    uint16               pmsk;
#if defined(FIBER_PORT_70MB) || defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
    static uint16 flag = 0;
#endif    
	if( TIMER_SEC_EVENT&event_list ){  /* Highest Priority */
		timer_sec_dsr();
		
	}
	nic_rx_dsr();
		
    if( TIMER_100MS_EVENT&event_list ){
        timer_100ms_dsr();
    }
    
#if LWPS_DHCPC   
    dhcpc_routine();
#endif

    /* check if link status has changed */
	//currLinkUpPmsk = 0;
    getCurrLinkStatus(&currLinkUpPmsk);
    if ( (prevLinkUpPmsk != currLinkUpPmsk) )
    {
		link_status_change(currLinkUpPmsk, prevLinkUpPmsk);
    }
#if 0
    /* get and clear link up port mask then call related handler */
    pmsk = getReg(0x1107);
    if(pmsk)
    {
        setReg(0x1107, pmsk);
        handler_linkup(pmsk);
    }

    /* get and clear link down port mask then call related handler */
    pmsk = getReg(0x1106);
    if(pmsk)
    {
        setReg(0x1106, pmsk);
        handler_linkdown(pmsk);
    }    
#endif
    sys_uart_cmd();

	prevLinkUpPmsk = currLinkUpPmsk;
	
#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
    flag++;
    if(flag == 1500)
    {
	    fiber_event();
	}
#endif	
#if defined(FIBER_PORT_70MB)
    flag++;
    if(flag == 3000)
    {
	    fiber0_event_70MB();
	}
    if(flag == 6000)
    {
	    fiber1_event_70MB();
	    flag = 0;
	}	
#endif	

}