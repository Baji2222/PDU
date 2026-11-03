#include <local/config.h>
#include <lwps/lwps.h>
#include <reg_sfr.h>
#include <stdio.h>

#include "common.h"
#include "asicreg.h"
#include "nic.h"
#include "packets.h"
#include "SPIFlashDrv.h"
#include "NORFlashDrv.h"

#define ARP_REQUEST     (0x0001)
#define ARP_REPLAY      (0x0002)


/* for flash */
#define CONFIG_FLASH_SECTOR_SIZE (CONFIG_SPI_SECTOR_SIZE)
#define FLASH_ERASE_BLOCK        SPI_ERASE_BLOCK
#define FLASH_ERASE_NONBLOCK     SPI_ERASE_NONBLOCK

static uint8 xdata tftp_RRQ[] = {
	0x00, 0x01, /* RRQ op code*/
	'f', 'i', 'n', 'a', 'l', '.', 'b', 'i', 'n', 0x00,
	'o', 'c', 't', 'e', 't', 0x00
};

static uint8 xdata tftp_ACK[] = {
	0x00, 0x04, /* ACK op code*/
	0xFF, 0xFF  /* Block Number */
};

static uint8 xdata udp_sent_template[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	THIS_MAC,
	0x08, 0x00,            /* ether type */
	0x45, 0x00,
	0xFF, 0xFF,            /* Total length */
	0x80, 0x91,            /* Identification */
	0x00, 0x00, 0x80, 0x11,
	0xFF, 0xFF,            /* Header checksum */
	THIS_IP,
	SERVER_IP,
	0x23, 0x79,            /* Source Port */
	0x00, 0x45,            /* Destination Port, 69 */
	0xFF, 0xFF,            /* length */
	0xFF, 0xFF             /* checksum */
};


static uint8 xdata arp_reply[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* DA */
	THIS_MAC,                           /* SA */
	0x08, 0x06, 0x00, 0x01, 
	0x08, 0x00, 0x06, 0x04, 0x00, 0x02, 
	THIS_MAC,
	THIS_IP,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF
};

static uint8 xdata arp_request[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* DA */
	THIS_MAC,                           /* SA */
	0x08, 0x06, 0x00, 0x01, 
	0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 
	THIS_MAC,
	THIS_IP,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	SERVER_IP
};


uint8 this_memcmp(uint8 *s1, uint8 *s2, uint32 len) large{
	uint32 xdata i;
	uint8  xdata value;
	for(i=0;i<len;i++){
		if( 0!=(value = (s1[i]-s2[i])) ){
			return value;
		}
	}
	return 0;
}


void send_arp() large{
	uint16 xdata pktLen = sizeof(arp_request)/sizeof(uint8);
	uint16 xdata i;
	pktBuf_t xdata pBuf;

	pBuf.tx_dport_mode = TX_DPORT_DEFAULT;
	
	for(i=0;i<pktLen;i++){
		pkt_head[i] = arp_request[i];
	}
    
    pBuf.pkt_head = pkt_head;
	pBuf.size = pktLen;
	txPkt(&pBuf);
	//print_buf(pkt_head, pktLen, "arp:");
}


void getServerMAC() large{
	uint32 xdata pktLen = sizeof(arp_request)/sizeof(uint8);
	uint32 xdata run_times = 0;
	uint16 xdata i;
	uint16 xdata tmp;
	int8   xdata failReason;
	
	send_arp();
	while (1)  
	{
		run_times++;
		if ( hasRxPkt() )
		{
		    if ((failReason = rxPkt(p_buffer, P_BUFFER_SIZE))!= SUCCESS )
			{
			    setReg(0x13A1, failReason);
				tmp = getReg(0x13A7);
				tmp++;
				setReg(0x13A7, tmp);
			}

			pktLen = pktInfo.pktHdr.length;
			setReg(0x13A2, pktLen);

		    if (failReason != SUCCESS )
			{
			}
			if( (0x08==pkt_head[0x0C]) && (0x06==pkt_head[0x0D]) && \
			    (0x00==this_memcmp(pkt_head+0x20, this_mac, 6))
			){
				for(i=0;i<6;i++){
					target_mac[i] = pkt_head[i+6];
				}
				break;
			}
		}
		if( run_times>=0x00010000UL ){
			OSAL_PRINTF("resent arp\n");
			run_times = 0;
			send_arp();
		}
	}
}

void tftp_send_ack(uint16 port_number, uint16 block_number) large{
	uint32 xdata pktLen = sizeof(udp_sent_template)/sizeof(uint8);
	uint16 xdata ip_len;
	uint16 xdata udp_len;
	uint16 xdata i;
	pktBuf_t xdata pBuf;

	pBuf.tx_dport_mode = TX_DPORT_DEFAULT;
	
	/* Send Request */
	for(i=6;i<pktLen;i++){
		pkt_head[i] = udp_sent_template[i];
	}
	
	for(i=0;i<6;i++){
		pkt_head[i] = target_mac[i];
	}
	
	for(i=0;i<6;i++){
		pkt_head[i] = target_mac[i];
	}
	ip_len = 28 + sizeof(tftp_ACK);
	pkt_head[0x10] = (ip_len>>8)&0xFF;
	pkt_head[0x11] = (ip_len>>0)&0xFF;
	
	udp_len = 8 + sizeof(tftp_ACK);
	pkt_head[0x26] = (udp_len>>8)&0xFF;
	pkt_head[0x27] = (udp_len>>0)&0xFF;
	
	pkt_head[0x24] = (port_number>>8)&0xFF;
	pkt_head[0x25] = (port_number>>0)&0xFF;
	
	for(i=0; i<(sizeof(tftp_ACK)-2); i++){
		pkt_head[i+pktLen] = tftp_ACK[i];
	}
	pkt_head[i+pktLen]   = (block_number>>8)&0xFF;
	pkt_head[i+pktLen+1] = (block_number>>0)&0xFF;
	    
    pBuf.pkt_head = pkt_head;
	pBuf.size = pktLen+sizeof(tftp_ACK);
	txPkt(&pBuf);
}

void tftp_transfer(void) large{
	uint32 xdata pktLen = sizeof(udp_sent_template)/sizeof(uint8);
	uint32 xdata image_size = 0;
	uint16 xdata i;
	uint16 xdata tmp;
	uint16 xdata ip_len;
	uint16 xdata udp_len;
	uint16 xdata server_port;
	uint16 xdata block_number;
	uint16 xdata *uint16_ptr;
	int8   xdata failReason;
	pktBuf_t xdata pBuf;
	uint8  ret;

	pBuf.tx_dport_mode = TX_DPORT_DEFAULT;
	
	/* Send Request */
	for(i=6;i<pktLen;i++){
		pkt_head[i] = udp_sent_template[i];
	}
	
	for(i=0;i<6;i++){
		pkt_head[i] = target_mac[i];
	}
	
	for(i=0;i<6;i++){
		pkt_head[i] = target_mac[i];
	}
	ip_len = 28 + sizeof(tftp_RRQ);
	pkt_head[0x10] = (ip_len>>8)&0xFF;
	pkt_head[0x11] = (ip_len>>0)&0xFF;
	
	udp_len = 8 + sizeof(tftp_RRQ);
	pkt_head[0x26] = (udp_len>>8)&0xFF;
	pkt_head[0x27] = (udp_len>>0)&0xFF;
	
	for(i=0; i<(sizeof(tftp_RRQ)); i++){
		pkt_head[i+pktLen] = tftp_RRQ[i];
	}

    pBuf.pkt_head = pkt_head;
	pBuf.size = pktLen+sizeof(tftp_RRQ);
	txPkt(&pBuf);
	
	/* Get tftp data */
	OSAL_PRINTF("[");
	while (1)  
	{
		if ( hasRxPkt() )
		{
		    if ((failReason = rxPkt(p_buffer, P_BUFFER_SIZE))!= SUCCESS )
			{
			    setReg(0x13A1, failReason);
				tmp = getReg(0x13A7);
				tmp++;
				setReg(0x13A7, tmp);
			}

			pktLen = pktInfo.pktHdr.length;
			setReg(0x13A2, pktLen);

		    if (failReason != SUCCESS ){}
		    
		    //print_buf(pkt_head, pktLen, "Recv_Buf:");
			
			if( (0x08==pkt_head[0x0C]) && (0x00==pkt_head[0x0D]) && \
			    (0x00==this_memcmp(pkt_head, this_mac, 6))       && \
			    (0x11==pkt_head[0x17]) && (0x23==pkt_head[0x24]) && \
			    (0x79==pkt_head[0x25]) && (0x03==pkt_head[0x2B])
			){
				if( 39!=(block_number%40) ){
		    		OSAL_PRINTF("#");
		    	}else{
		    		OSAL_PRINTF("#\n ");
		    	}
				block_number = (pkt_head[0x2C]<<8) | pkt_head[0x2D];
				server_port  = (pkt_head[0x22]<<8) | pkt_head[0x23];
				tmp = (pkt_head[0x26]<<8) | (pkt_head[0x27]);
				ret = flash_write(image_size, &pkt_head[0x2E], (tmp - 12));
				image_size += (tmp - 12);
				tftp_send_ack(server_port, block_number);
				if( tmp<524 ){
					uint16_ptr = (uint16*)&image_size;
					//OSAL_PRINTF("Len=0x%04x, image_size=0x%04x%04x\n", tmp, *(uint16_ptr), *(uint16_ptr+1));
					OSAL_PRINTF("]\n");
					OSAL_PRINTF("The Total Size is 0x%04x%04x\n", *(uint16_ptr), *(uint16_ptr+1));
					break;
				}
			}
		}
	}
}

void update_firmware(void) large
{
	uint32 xdata pktCount = 0;
	uint32 xdata i;
	uint32 xdata size;
	uint16 xdata pktLen = 0;
	
	OSAL_PRINTF("===============  Update Firmware Start  ===============\n");
	IE = 0;
	EA = 0;
	
	//SYNC_CTRL = 3;
		
	pkt_head = (p_buffer+TAG_SIZE+FRAME_INFO_SIZE);

	getServerMAC();
	
	OSAL_PRINTF("[ServerMac = ");
	for(i=0;i<6;i++){
		OSAL_PRINTF("0x%02x ", (uint16)target_mac[i]);
	}
	OSAL_PRINTF("]\n");
	

	flash_init();
	
#if defined(FLASH_TYPE_SPI)
	size = CONFIG_UPDATE_FIRMWARE_MAX_SIZE;
	OSAL_PRINTF("[Flash Erase Start, from 0 to 0x%08lX]\n", size);
	for(i=0;i<CONFIG_UPDATE_FIRMWARE_MAX_SIZE;i+=CONFIG_FLASH_SECTOR_SIZE){
		flash_erase(i, FLASH_ERASE_BLOCK);
	}
	OSAL_PRINTF("[Flash Erase Done ]\n");
#elif defined(FLASH_TYPE_NOR)
	OSAL_PRINTF("[Flash Erase Start, Whole Chip]\n");
	flash_erase();
	OSAL_PRINTF("[Flash Erase Done ]\n");
#endif /* end of elif FLASH_TYPE_NOR */
	
	tftp_transfer();

	setReg(0x13A0, 0xFFFF);
	OSAL_PRINTF("===============  Update Firmware End    ===============\n\n");
}
