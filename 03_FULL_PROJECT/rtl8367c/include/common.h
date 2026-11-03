#ifndef __COMMON_H__
#define __COMMON_H__

#include <common_types.h>
#include <lwps/lwps.h>

#include <nic.h>
#include <SPIFlashDrv.h>
#include <NORFlashDrv.h>


/* SFR */

/* CLI command list */


#define TIMER_SEC_EVENT     (1<<0)
#define NIC_RX_EVENT        (1<<1)
#define TIMER_100MS_EVENT   (1<<2)

//#define OSAL_PRINTF  //
#define OSAL_PRINTF  printf

/* define this for rtl8367c and relative models */
typedef struct frame_info_s {
	/* byte-0 */
	uint8 rxpktsn;
	/* byte-1 */
	uint8 crce:1;
	uint8 l3cse:1;
	uint8 l4cse:1;
	uint8 pppoe:1;
	uint8 vlan:1;
	uint8 reserved1:3;
	/* byte-2 */
	uint8 reserved2:4;
	uint8 type:4;
	/* byte-3 */
	uint8 reserved3:4;
	uint8 oversize:1;
	uint8 fragment:1;
	uint8 reserved4:2;
	/* byte-4-7 */
	uint32 length;
}frame_info_t;

typedef struct cputag_s {
	/* byte-0,1 */
	uint16 etherType;
	/* byte-2 */
	uint8 protocol;
	/* byte-3 */
	uint8 reason;
	/* byte-4 */
	uint8 efid:1;
	uint8 enhancefid:3;
	uint8 priority_select:1;
	uint8 priority:3;
	/* byte-5 */
	uint8 keep:1;
	uint8 vsel:1;
	uint8 disable_learning:1;
	uint8 vidx:1;
	/* byte-6,7 */
	uint16 portmask;
}cputag_t;


/* extern functions */

extern void   all_init(void);
extern void   nic_init(void);
extern void   timer_init(void);
extern void   uart_init(void);
extern void   mem_init(void);
#if MIB_ENABLE
extern void   mib_init(void);
#endif
#if LWPS_SNMP
extern void   snmp_init(void);
#endif /* end of LWPS_SNMP */

#if LWPS_SYSLOG
extern void   syslog_init(void);
#endif /* end of LWPS_SYSLOG */

extern void   print_menu(void) large;
extern void   clearScreen(void) large;
extern void   cliGetMacAddr(void) large;
extern void   update_firmware(void) large;
extern void   event_dispatch(void);
extern void   setReg(uint16 address, uint16 value);
extern void   change_clock(uint16 xdata value) large;
extern void   set_8051RDY(void) large;
extern void   print_buf(uint8 *buf, uint32 len, char *msg) large;
//extern uint8  vic_rand8(void);
extern uint8  cmd_select(void) large;
extern uint8  getFlashData(uint32 addr);
extern char   getchar_nb(void);
extern uint16 getReg(uint16 address);
extern void sys_uart_cmd();

#if defined(FLASH_TYPE_SPI)
  #define flash_read   spi_flash_read
  #define flash_write  spi_flash_write
  #define flash_erase  spi_flash_erase
  #define flash_init   spi_flash_init
#elif defined(FLASH_TYPE_NOR)
  #define flash_read nor_flash_read
  #define flash_write  nor_flash_write
  #define flash_erase  eraseFlash
  #define flash_init()
#endif


/* extern variables */
  /* move p_buffer from lwps.h to here */
extern          uint8  xdata p_buffer[P_BUFFER_SIZE];
extern          uint8  xdata p_tx_buffer[P_BUFFER_SIZE];
  /* For timer */
extern volatile uint32 data current_time;
extern volatile uint32 data up_time;
extern          uint32 data system_clock;
extern volatile uint8  idata _th0;
extern volatile uint8  idata _tl0;
extern          uint8  data timeCount;
extern volatile uint8  data global_debug;

  /* For NIC */
extern volatile uint8  idata rxSeq;
extern volatile uint8  idata txSeq;
extern          uint8  xdata *p_head;

  /* for Event Flags */
extern volatile uint8 idata event_list;

    /* for SAL */
extern uint8     xdata    this_mac[6];
extern uint8     xdata    this_ip[4];
extern uint8     xdata    netmask[4];
extern ip_addr_t xdata    default_gateway;
extern uint8     xdata    dhcp_enable;

extern uint8    xdata firmVer[];
extern uint8    xdata chipVer[];		 
extern uint8    xdata firmDate[];
extern uint8    xdata chipId[];
extern uint8    xdata chipName[];

    /* for firmware upgrade buffer */
extern uint8    stack_buffer[512];

#endif /* End of __COMMON_H__ */

