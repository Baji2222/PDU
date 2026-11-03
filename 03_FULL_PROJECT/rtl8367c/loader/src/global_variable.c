#include <local/lwps_config.h>
#include <local/config.h>
#include <local/flash_config.h>
#include <reg_sfr.h>

#include "common.h"
#include "asicreg.h"

/* Global */
/* For Secondary Stack reserved area */
unsigned char scondary_stack[SECOND_STACK_SZ] _at_ (0x8000-SECOND_STACK_SZ);

/* For SPI Driver entry point */
spi_flash_driver_entry_t *spi_flash_driver_entry _at_ 0x17fe;

volatile uint8 idata event_list;

/* For Timer */
volatile uint32 data current_time;
volatile uint8  data _th0;
volatile uint8  data _tl0;
         uint32 data system_clock;
         uint8  data timeCount;

/* For NIC */
volatile pkt_info_t idata pktInfo;
volatile uint8      idata rxSeq;
volatile uint8      idata txSeq;

/* For LWPS/Update firmware */
         pktBuf_t xdata pktbuf _at_ 0x0010; //this address can't be 0x0000
         uint8    xdata p_buffer[P_BUFFER_SIZE] _at_ 0x1000;
		 uint8    xdata p_tx_buffer[P_BUFFER_SIZE] _at_ 0x2000;
         uint8    xdata *pkt_head;
         uint8    xdata this_mac[6] = DFLT_SYS_MAC_ADDRESS;
         uint8    xdata this_ip[4] = DFLT_SYS_IP_IP;
         uint8    xdata netmask[4] = DFLT_SYS_IP_NETMASK;
         ip_addr_t xdata default_gateway = {DFLT_SYS_IP_GATEWAY};
         

/* For Reentrant Function */
         uint8    xdata stack_buffer[2048] _at_ 0x2800;

/* loader and runtime shared variable */
         uint8    bdata bBase _at_ 0x20;
         sbit     loader_isr_enable = bBase ^ 0;

