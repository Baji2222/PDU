#ifndef __COMMON_H__
#define __COMMON_H__

#include <common_types.h>

#include <nic.h>
#include <SPIFlashDrv.h>

/* SFR */

/* CLI command list */
#define PROJECT_NAME          "RTL8370S"
#define CLI_COMMAND_CLEAR     ('c')
#define CLI_COMMAND_HELP      ('h')
#define CLI_COMMAND_EXIT      ('q')
#define CLI_COMMAND_UPDATE    ('u')

//#ifndef MAC_ADDR_LEN
//#define MAC_ADDR_LEN          (6)
//#endif

enum eventFlag{
	NULL_EVENT = 0,
	NIC_RX_EVENT,
	TIMER_SEC_EVENT
};

//#define OSAL_PRINTF  //
#define OSAL_PRINTF  printf

/* define this for rtl8370s and relative models */
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

extern void   all_init(void) large;
extern void   loader_init(void) large;
extern void   nic_init(void) large;
extern void   uart_init(void) large;
extern void   check_boot(uint8) large;
extern uint8  check_header(u8_t *hdr_adr);
extern void   loader_main();
extern void   loader_isr_set(uint8);


extern void   update_firmware(void) large;
extern void   event_dispatch(void);
extern void   setReg(uint16 address, uint16 value);
extern void   change_clock(uint16 xdata value) large;
//extern void   print_buf(uint8 *buf, uint32 len, char *msg) large;


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
  /* move p_buffer from lwps.h to here*/

extern pktBuf_t xdata pktbuf;
extern uint8    xdata *pkt_head;
extern uint8    xdata p_buffer[P_BUFFER_SIZE];
extern uint8    xdata p_tx_buffer[P_BUFFER_SIZE];
  /* For timer */
extern volatile uint32 data current_time;
extern          uint32 data system_clock;
extern volatile uint8  data _th0;
extern volatile uint8  data _tl0;
extern          uint8  data timeCount;

  /* For NIC */
extern volatile uint8  idata rxSeq;
extern volatile uint8  idata txSeq;
extern          uint8  xdata *p_head;

  /* for Event Flags */
extern volatile uint8 idata event_list;

    /* for SAL */
extern uint8     xdata this_mac[6];
extern uint8     xdata this_ip[4];
extern ip_addr_t xdata default_gateway;
extern uint8     xdata target_mac[6];
extern uint8     xdata target_ip[4];
extern uint8     xdata netmask[4];

    /* for firmware upgrade buffer */
extern uint8    xdata stack_buffer[2048];

#endif /* End of __COMMON_H__ */

