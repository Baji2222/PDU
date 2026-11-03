#include "common.h"
#include "asicreg.h"
#include "nic.h"
#include "l2.h"

#include <reg_sfr.h>
#include <string.h>
#include <stdio.h>

		
/* The following two MACRO should be multiple of 8 */
#define PEDMA_DIRECTION(x)            ((x)<<7)
#define PEDMA_PKTBUF_TO_XSRAM     (0)
#define PEDMA_XSRAM_TO_PKTBUF     (1)

// Has Receive Packet: Checks if there are any packets waiting in the RX buffer
// Returns TRUE if RX read descriptor pointers indicate pending packet, FALSE otherwise
uint8 hasRxPkt()
{
	if (getReg(NIC_RXRDL) != 0 || getReg(NIC_RXRDH) !=0)
		return TRUE;
	else
		return FALSE;
}

// Receive Packet: Reads a packet from NIC RX buffer into system memory
// Copies packet data via PEDMA, validates CRC status, and advances RX pointers
int8 rxPkt(uint8 *buf, uint16 len)
{
    uint16 xdata curRxPnt;      //current RX pkt start pointer.
	uint8  cnt = 0;

    curRxPnt = (getReg(NIC_CRXCPRL) | (getReg(NIC_CRXCPRH) << 8)) * 8;

	buf += TAG_SIZE;

    PEDMA_E_ADDR0 = ((uint16)buf & 0xff);
	PEDMA_E_ADDR1 = ((uint16)buf & 0xff00) >> 8;
	PEDMA_P_ADDR0 = curRxPnt & 0xFF;
	PEDMA_P_ADDR1 = (curRxPnt & 0xFF00) >> 8;
	
	PEDMA_LEN = 1;

    while(0 != PEDMA_LEN )
        ;

    pktInfo.pktHdr.length = (buf[6] << 8) | buf[7];
    
    if ( pktInfo.pktHdr.length > ((uint16)len) )
    {
        printf("buffer too small\n");
        return FAILED;
    }

    pktInfo.pktHdr.status = (uint32)buf[0] 
                         | ((uint32)buf[1] << 8) 
                         | ((uint32)buf[2] << 16) 
                         | ((uint32)buf[3] << 24);
	

	 /* drop RX CRC */
	if (pktInfo.pktHdr.status & RXCRCE)
    {
        rxSeq++;
        setReg(NIC_RXCMD, 1);
		printf("nic rec rx err\n");
        return FAILED;
    }
    rxSeq++;

    curRxPnt = (curRxPnt + 8) % RXBUFFERSIZE;
    
	PEDMA_E_ADDR0 = ((uint16)(buf + 8) & 0xff);
	PEDMA_E_ADDR1 = ((uint16)(buf + 8) & 0xff00) >> 8;
	PEDMA_P_ADDR0 = curRxPnt & 0xFF;
	PEDMA_P_ADDR1 = (curRxPnt & 0xFF00) >> 8;
	
	PEDMA_LEN = (pktInfo.pktHdr.length + 7) >> 3;
	 
    while(0 != PEDMA_LEN );
    
    setReg(NIC_RXCMD, 1);
	/*no packet*/
	if(getReg(NIC_RXRDL)==0 && getReg(NIC_RXRDH)==0)
	{
	    EX1=1;
	}
	
    return SUCCESS;
}

//void txPkt(uint32 flag, uint16 len, uint8 *buf)
// Transmit Packet: Sends a packet from system memory to the NIC TX buffer
// Inserts CPU tag if needed, sets up PEDMA transfer, and triggers transmission
void txPkt(pktBuf_t *pBuf)
{	
    uint16 xdata txstart;
    pkt_hdr_t xdata pktHdr;
    uint16 xdata freeSpace;
    uint32 xdata flag;
    uint8 xdata *buf;
    uint16 xdata len;
    uint8 xdata i;
	uint8 xdata off;
    static uint16 old_tmp56 = 0, flag1=0;
    uint16 tmp56=0;

	if(flag1!=0)
    {
        while(old_tmp56 == tmp56)
        {
            tmp56 = getReg(0x1a56);
        }
    }
    flag1 = 1;

    flag = TXL4CSRE | TXL3CSRE | TXCRCRE;

    pBuf->pkt_head = pBuf->l2;

    off = 0;
    if(TX_DPORT_ASSIGN_PMSK == pBuf->tx_dport_mode)
    {
        if( (MAC_ADDR_LEN * 2)!=(pBuf->pkt_cputag - pBuf->pkt_head)){
	        for( i = 0; i < MAC_ADDR_LEN * 2; i++)
   	        {
   	            *(pBuf->pkt_head + i - CPU_TAG_SIZE) = *(pBuf->pkt_head + i);
   	        }
			pBuf->pkt_head -= CPU_TAG_SIZE;
			pBuf->size += CPU_TAG_SIZE;
    	}

        /* insert CPU tag */
        *(pBuf->pkt_head + MAC_ADDR_LEN * 2 + 0) = 0x88;
        *(pBuf->pkt_head + MAC_ADDR_LEN * 2 + 1) = 0x99;
        *(pBuf->pkt_head + MAC_ADDR_LEN * 2 + 2) = 0x04;
		*(pBuf->pkt_head + MAC_ADDR_LEN * 2 + 3) = 0x00;
		*(pBuf->pkt_head + MAC_ADDR_LEN * 2 + 4) = 0x00;		
		*(pBuf->pkt_head + MAC_ADDR_LEN * 2 + 5) = 0x20;		
        *(pBuf->pkt_head + MAC_ADDR_LEN * 2 + 6) = (pBuf->tx_dst_pmsk >> 8) & 0xFF;
        *(pBuf->pkt_head + MAC_ADDR_LEN * 2 + 7) = pBuf->tx_dst_pmsk & 0xFF;
    }

    len = pBuf->size;
    buf = pBuf->pkt_head - FRAME_INFO_SIZE;

    //set TX pkt header
    pktHdr.status = (flag & 0x0001FF00) + txSeq;
    pktHdr.length = len;

    txSeq++;
    //check buffer size
    while (1)
    {
        freeSpace = ((getReg(NIC_TXASRL) | (getReg(NIC_TXASRH) << 8))) << 3;
        if (freeSpace >= (len + 8))     //8 byte alignment
            break;
        //OSAL_PRINTF("in tx wait loop\n");
    }

    txstart = getReg(NIC_CTXCPRL) | (getReg(NIC_CTXCPRH) << 8);
    

    //write packet header to packet buffer
    buf[0] = (pktHdr.status) & 0xFF;
    buf[1] = (pktHdr.status >> 8) & 0xFF;
    buf[2] = (pktHdr.status >> 16)& 0xFF;
    buf[3] = (pktHdr.status >> 24)& 0xFF;
    buf[6] = (pktHdr.length >> 8) & 0xFF;
    buf[7] = (pktHdr.length) & 0xFF;

	PEDMA_E_ADDR0 = (uint16)buf & 0xff;
	PEDMA_E_ADDR1 = ((uint16)buf & 0xff00) >> 8;
	PEDMA_P_ADDR0 = (txstart * 8) & 0xFF;
	PEDMA_P_ADDR1 = (((txstart * 8) & 0xFF00) >> 8) | 0x80;
	
	PEDMA_LEN = (pktHdr.length + 7 + 8) >> 3;
	
	while(0 != PEDMA_LEN );

	old_tmp56 =getReg(0x1a56);

    setReg(NIC_TXCMD, 1);

    /* TX buf initial */
    pBuf->tx_dport_mode    = TX_DPORT_DEFAULT;
    pBuf->tx_trunk_support = TX_TRUNK_IGNORE;
	pBuf->pkt_cputag       = NULL;
}