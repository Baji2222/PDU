#ifndef __ASICREG_H_
#define __ASICREG_H_


#define DW8051_IMS        (0x1119)

#define CHIP_RST          (0x1322)
	#define CHIP_RST_NIC  (1<<5)
	#define CHIP_RST_8051 (1<<4)
	#define CHIP_RST_CFG  (1<<2)
	#define CHIP_RST_SW   (1<<1)
	#define CHIP_RST_CHIP (1<<0)
#define DW8051_STATUS_REG (0x1336)
#define DW8051_DEBUG_REG  (0x1346)

#define NIC_RXRDL         (0x1A04)
#define NIC_RXRDH         (0x1A05)
#define NIC_TXASRL        (0x1A08)
#define NIC_TXASRH        (0x1A09)
#define NIC_RXCMD         (0x1A0C)
#define NIC_TXCMD         (0x1A0D)
#define NIC_RXCR0         (0x1A14)
#define NIC_RXCR1         (0x1A15)
#define NIC_TXCR          (0x1A16)
#define NIC_GCR           (0x1A17)
#define NIC_TXSTOPL       (0x1A44)
#define NIC_TXSTOPH       (0x1A45)
#define NIC_RXSTOPL       (0x1A46)
#define NIC_RXSTOPH       (0x1A47)
#define NIC_CRXCPRL       (0x1a50)
#define NIC_CRXCPRH       (0x1a51)
#define NIC_CTXCPRL       (0x1a52)
#define NIC_CTXCPRH       (0x1a53)

#define PORT_ABAILITY_BASE (0x1352)



/* Switch Registers */
#define DW8051_RDY                 (0x1336)
	#define DW8051_RDY_RATE(val)   ((val<<4)&0x70)
	#define DW8051_RDY_RATE_MASK   (0x07<<4)

    /* RealChip */
#define DW8051_RDY_200MHz     (0x00)
#define DW8051_RDY_125MHz     (0x01)
#define DW8051_RDY_100MHz     (0x02)
#define DW8051_RDY_625MHz     (0x04)
#define DW8051_RDY_20_8MHz    (0x05)

#endif

