#include <common_types.h>
#include <local/config.h>
#include <common.h>

typedef void (*isr_handler_f)() ;

#define SWITCH_ISR_ADDR    (0x1003)
#define TIMER0_ISR_ADDR    (0x100B)
#define TIMER0_ISR_ADDR_H  0x10
#define TIMER0_ISR_ADDR_L  0x0B
#define NIC_ISR_ADDR       (0x1013)
#define NIC_ISR_ADDR_H     0x10
#define NIC_ISR_ADDR_L     0x1B

extern bit      loader_isr_enable;
isr_handler_f data s_isr _at_ 0x7D;

#if defined (FIBER_PORT_70MB)
uint16 timer3000_global_flag0 = 0;
uint16 timer3000_global_flag1 = 0;
#endif

#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
uint16 timer500_global_flag = 0;
#endif

#if 0
void switch_isr() interrupt 0
{
	isr_handler_f s_isr = SWITCH_ISR_ADDR;
	s_isr();
	/* Jump to runtime switch_isr */
}
#endif

void timer_0_isr() interrupt 1
{
#if defined (FIBER_PORT_70MB)
    static uint16 timeCount3000ms = 0;
#endif    
#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
    static uint8 timeCount500ms = 0;
#endif

    if(!loader_isr_enable)
    {
#if 1
        /* Jump to runtime timer_0_isr */    
	    s_isr = TIMER0_ISR_ADDR;
		s_isr();
#else
		#pragma asm
		MOV   R2, #(TIMER0_ISR_ADDR_H)
		MOV   R1, #(TIMER0_ISR_ADDR_L)
		LCALL ?C?ICALL
        #pragma endasm
        
#endif
    }
    else
    {
        /* Interrupt Handler for loader */
        TH0 = _th0;
        TL0 = _tl0;
        
        timeCount++;
        
        if ( timeCount == CONFIG_TIMER_DIVISOR )
        {
            timeCount = 0;
            event_list |= TIMER_SEC_EVENT;
        }

#if defined(FIBER_PORT_4)|| defined (FIBER_PORT_6)
        timeCount500ms++;
        
        /* 500ms event */
        if(timeCount500ms == 100)
        {
            timeCount500ms = 0;
            timer500_global_flag = 1;
        }        
#endif        
#if defined (FIBER_PORT_70MB)
        timeCount3000ms++;
        
        /* 3000ms event */
        if(timeCount3000ms == 600)
        {
            timeCount3000ms = 0;
            timer3000_global_flag0 = 1;
            timer3000_global_flag1 = 1;
        }        
#endif            
    }
}

void nic_isr() interrupt 2
{
#if 0
    if(!loader_isr_enable)
    {
        /* Jump to runtime nic_isr */    
#if 1
        isr_handler_f s_isr = NIC_ISR_ADDR;
        s_isr();
#else

		#pragma asm
		MOV   R2, #(NIC_ISR_ADDR_H)
		MOV   R1, #(NIC_ISR_ADDR_L)
		LCALL ?C?ICALL
        #pragma endasm
        
#endif
    }
    else
    {
        /* Interrupt Handler for loader */
    }
#endif
    EX1 =0;
    /*clear NIC IMS*/
	INDACC_ADDR_H=0x1A;
	INDACC_ADDR_L=0x0E;
	INDACC_WDATA_L=0X80;
	INDACC_CMD=0X03;
	
	INDACC_ADDR_H=0x12; 
	INDACC_ADDR_L=0x02;
	INDACC_WDATA_L=0X80;
	INDACC_WDATA_L=0X80;
	INDACC_CMD=0X03;

    
    
}

void loader_isr_set(uint8 enable)
{
    loader_isr_enable = (enable)?TRUE:FALSE;
}

#if 0
/* this function is defined to invoke calling ?C?ICALL segment */
/* in this way, ?C?ICALL will be declared so that ?C?ICALL will 
   be available in this .c */
void dummy_func()
{
    isr_handler_f s_isr;
	s_isr();
}
#endif
