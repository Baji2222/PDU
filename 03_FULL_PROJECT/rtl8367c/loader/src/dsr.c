#include <common.h>
#include <lwps/lwps.h>
#include <tftpd/tftpc.h>
#include <handler.h>

#if defined (FIBER_PORT_70MB)
extern uint16 timer3000_global_flag0;
extern uint16 timer3000_global_flag1;

extern int8 setSerdesReg(uint16 sds_index, uint16 sds_reg, uint16 sds_page, uint16 regData);
#endif
#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
extern uint16 timer500_global_flag;
#endif

#if defined(FIBER_PORT_4)
void toggle_bit_error_reset()
{
    setReg(0x6602, 0x0004);
    setReg(0x6601, 0x0400);
    setReg(0x6600, 0x00c0);
    setReg(0x6602, 0     );
    setReg(0x6601, 0x0400);
    setReg(0x6600, 0x00c0);
}

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
             for(i=0;i<3000;i++);                 //delay (0.3ms)
             setReg(0x133d,0x02);               //stop gli clk
             for(i=0;i<3000;i++);                 //delay (0.3ms)
            
             setSerdesReg(0,0,0x0,0x1400);       //toggle rx_en_on,tx_en_on
             for(i = 0; i < 60000; i++);        //delay (12ms)
//             for(i = 0; i < 60000; i++);
             setSerdesReg(0,0,0x0,0x1403);
             for(i = 0; i < 30000; i++);        //delay (6ms)
//             for(i = 0; i < 30000; i++);
            
             setReg(0x133d,0x0);                //release gli clk
             for(i=0;i<3000;i++);                 //delay (0.3ms)
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
                 for(i=0;i<3000;i++);                 //delay (0.3ms)
                 setReg(0x133d,0x02);               //stop gli clk
                 for(i=0;i<3000;i++);                 //delay (0.3ms)
                
                 setSerdesReg(0,0,0x0,0x1400);       //toggle rx_en_on,tx_en_on
                 for(i = 0; i < 60000; i++);        //delay (12ms)
//                 for(i = 0; i < 60000; i++);
                 setSerdesReg(0,0,0x0,0x1403);
                 for(i = 0; i < 30000; i++);        //delay (6ms)
//                 for(i = 0; i < 30000; i++);
                
                 setReg(0x133d,0x0);                //release gli clk
                 for(i=0;i<3000;i++);                 //delay (0.3ms)
                 setReg(0x13c4, 0x66);                //release force 

                
                timer3000_global_flag0 = 0;

                flag_3000ms0 = 1;
            }                
        }    
    }
}

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
             for(i=0;i<3000;i++);                 //delay (0.3ms)
             setReg(0x133d,0x02);               //stop gli clk
             for(i=0;i<3000;i++);                 //delay (0.3ms)
            
             setSerdesReg(1,0,0x0,0x1400);       //toggle rx_en_on ,tx_en_on    
             for(i = 0; i < 60000; i++);        //delay(12ms) 
//             for(i = 0; i < 60000; i++);
             setSerdesReg(1,0,0x0,0x1403);
             for(i = 0; i < 30000; i++);        //delay (6ms)
//             for(i = 0; i < 30000; i++);
            
             setReg(0x133d,0x0);                //release gli clk
             for(i=0;i<3000;i++);                 //delay (0.3ms)
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
                 for(i=0;i<3000;i++);                 //delay (0.3ms)
                 setReg(0x133d,0x02);               //stop gli clk
                 for(i=0;i<3000;i++);                 //delay (0.3ms)
                
                 setSerdesReg(1,0,0x0,0x1400);       //toggle rx_en_on ,tx_en_on    
                 for(i = 0; i < 60000; i++);        //delay(12ms) 
//                 for(i = 0; i < 60000; i++);
                 setSerdesReg(1,0,0x0,0x1403);
                 for(i = 0; i < 30000; i++);        //delay (6ms)
//                 for(i = 0; i < 30000; i++);
                
                 setReg(0x133d,0x0);                //release gli clk
                 for(i=0;i<3000;i++);                 //delay (0.3ms)
                 setReg(0x1311, 0x66);                //release force 


                
                timer3000_global_flag1 = 0;

                flag_3000ms1 = 1;
            }                
        }    
    }    
}
#endif


void timer_sec_dsr(void)
{
    /* one second */
    event_list &= (~TIMER_SEC_EVENT);
#if defined(UPGRADE_TFTP)
    tftpc_timer(1000);
#endif    
    etharp_timer();
    tcp_timer();
    udp_timer();

    handler_timer();
}

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
//	    	pktbuf.size       =pktbuf.size;
        pktbuf.frame_info = u8_ptr;
        pktbuf.pkt_head   = pkt_head;
        pktbuf.l2         = pkt_head;
		

	    if(eth_up(&pktbuf) == LWPS_ERR_L2_NDEF)
	        handler_net_l2(&pktbuf);
        ret = lwps_event_handler();
        times++;
    }
    
}


void event_dispatch(void)
{
#if defined(FIBER_PORT_70MB) || defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
    static uint16 flag = 0;
#endif    

    if( TIMER_SEC_EVENT&event_list ){  /* Highest Priority */
		timer_sec_dsr();
    }
    nic_rx_dsr();
#if defined(UPGRADE_TFTP)    
    tftpc_routine();
#endif    
#if defined(FIBER_PORT_70MB)
    flag++;
    if(flag == 1500)
    {
	    fiber0_event_70MB();
	}
    if(flag == 3000)
    {
	    fiber1_event_70MB();
	    flag = 0;
	}	
#endif	

#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
    flag++;
    if(flag == 1500)
    {
	    fiber_event();
	}
#endif	

}

