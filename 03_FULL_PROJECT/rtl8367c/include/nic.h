#ifndef __NIC_H_
#define	__NIC_H_


#include "common.h"
#include "reg_sfr.h"
#include "asicreg.h"
#include "packets.h"
#include "common.h"

#include <local/config.h>
#include <lwps/pktbuf.h>

#define NICBUFSTART 0xC000
/* Rx status Word */
#define FRAGMENT 	(0x1UL<<28)
#define OVERSIZE		(0x1UL<<27)
#define RXSP			(0x1UL<<26)
#define RXPRI_offset	24
#define RXPRI_mask  (0x3UL<<24)
#define TYPE_offset	20
#define Type_mask	(0xFUL<<20)
#define ACL_offset	16
#define ACL_maks	(0xFUL<<16)
#define TAPRSN_offset	13
#define TAPRSN_maks	(0x7<<13)
#define VLAN		(1<<12)
#define PPPOE	(1<<11)
#define RXL4CSE	(1<<10)
#define RXL3CSE	(1<<9)
#define RXCRCE		(1<<8)
#define RXPKTSN_mask 0xFF

/*TX control word*/
#define NLEARN	(1UL<<16)
#define TXDSP_offset		14
#define TXDSP_mask		(3<<14)
#define TXPRI_offset		11
#define TXPRI_mask		(7<<11)
#define TXL4CSRE			(1<<10)
#define TXL3CSRE			(1<<9)
#define TXCRCRE			(1<<8)
#define TXPKTSN_mask 	0xFF

#define RXSTOP 	0x1FF	//it's 8 byte alignment. So the actual size is the number * 8
#define TXSTOP 	0x3FF
#define MEMEND 	0x5FF

#define RXSEQERR 0x11
#define RXCRCERR 0x10

extern uint16 getReg(uint16 addr);
extern void setReg(uint16 addr, uint16 value);
//extern void nic_init() large;
extern uint8 hasRxPkt();
extern int8 rxPkt(uint8 *buf, uint16 len);
//extern void txPkt(uint32 flag, uint16 len, uint8 *buf);
extern void txPkt(pktBuf_t*);

extern volatile pkt_info_t idata pktInfo;
extern uint8 xdata * xdata szBuf;

#endif /* end of __DRIVER_H_ */
