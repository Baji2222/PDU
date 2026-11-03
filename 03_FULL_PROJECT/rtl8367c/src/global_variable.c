#include <local/config.h>
#include <reg_sfr.h>

#include <common.h>
#include <asicreg.h>
#include <image.h>

/* Global */

#define    STR_VARA_LEN    (12)

/* Event Flags */
volatile uint8 idata event_list;


/* For Timer */
volatile uint32 data current_time;
volatile uint32 data up_time;
volatile uint8  idata _th0;
volatile uint8  idata _tl0;
         uint32 data system_clock;
         uint8  data timeCount;

/* For NIC */
volatile pkt_info_t idata pktInfo;
volatile uint8      idata rxSeq;
volatile uint8      idata txSeq;

/* For Secondary Stack reserved area */
uint8    secondary_stack[SECOND_STACK_SZ] _at_ (0x8000-SECOND_STACK_SZ);

/* For SPI Driver entry point */
spi_flash_driver_entry_t *spi_flash_driver_entry _at_ 0x7fc0;


/* For LWPS/Update firmware */
         pktBuf_t xdata pktbuf _at_ 0x0010; //this address can't be 0x0000
         uint8    xdata p_buffer[P_BUFFER_SIZE];
		 uint8    xdata p_tx_buffer[P_BUFFER_SIZE];
         uint8    xdata *pkt_head;
         uint8    xdata this_mac[6] = {
         	THIS_MAC
         };
         uint8    xdata this_ip[4] = {
         	THIS_IP
         };
		 ip_addr_t xdata default_gateway = {
         	{DEFAULT_GATEWAY}
         };
         uint8    xdata netmask[4] = {
         	NETMASK
         };

         uint8    xdata target_mac[6];
         uint8    xdata target_ip[4] = {
         	SERVER_IP
         };
         uint8    xdata firmVer[]  = {FIRMWARE_VER};
         uint8    xdata chipVer[]  = {"v0.0"};
		 uint8    xdata firmDate[] = {__DATE__};

         uint8    xdata dhcp_enable = FALSE;

         uint8    xdata chipId[STR_VARA_LEN] = {"RTL8367N"};
         uint8    xdata chipName[STR_VARA_LEN] = {"RTL8367N"};

/* For Reentrant Function */
         uint8    xdata stack_buffer[512] _at_ 0x1E00;

/* For Loader */
         uint8     code  irom_loader[4096] _at_ 0x0000;
         /* Only one far const variable, at RUNTIME_HEADER_ADDRESS */
         far runTimeImageHeader_t const rt_header = {
		 	0xFFFFFFFFUL, 0xFFFFFFFFUL, 0xFFFFFFFFUL,
			0xFFFFFFFFUL, 0xFFFFFFFFUL
		 }; /* _at_ RUNTIME_HEADER_ADDRESS is not available*/

         /* reserved for loader isr.c to control isr address */
         static uint8 bdata bBase _at_ 0x20;
         static uint8 data loader_fun_ptr[3] _at_ 0x7D;
         
         