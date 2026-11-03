#pragma STRING (far)

#include <local/lwps_config.h>

#include <stdio.h>
#include <common.h>
#include <upgrade_tftp.h>
#include <lwps/pktbuf.h>
#include <image.h>

uint8 xdata tftp_RRQ[] = {
    0x00, 0x01, /* RRQ op code*/
    'f', 'i', 'n', 'a', 'l', '.', 'b', 'i', 'n', 0x00,
    'o', 'c', 't', 'e', 't', 0x00
};

uint8 xdata tftp_ACK[] = {
    0x00, 0x04, /* ACK op code*/
    0xFF, 0xFF  /* Block Number */
};

uint8 xdata udp_sent_template[] = {
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


uint8 xdata arp_reply[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* DA */
    THIS_MAC,                           /* SA */
    0x08, 0x06, 0x00, 0x01, 
    0x08, 0x00, 0x06, 0x04, 0x00, 0x02, 
    THIS_MAC,
    THIS_IP,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF
};

uint8 xdata arp_request[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* DA */
    THIS_MAC,                           /* SA */
    0x08, 0x06, 0x00, 0x01, 
    0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 
    THIS_MAC,
    THIS_IP,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    SERVER_IP
};

#if 0 /* print_buf */
static void print_buf(uint8 *buf, uint32 len, char *msg){
    uint16 i;
    int line_size = 8;
    
    if( NULL!=msg ){
        OSAL_PRINTF("%s\n", msg);
    }
    OSAL_PRINTF("[Begin:0x%04x_0x0000]    0     1     2     3     4     5     6     7", (uint16)buf);
    for(i=0;i<len;i++, buf++){
        if( 0==(i%8) ){
            OSAL_PRINTF("\n      [0x%04x_0x%04x]:", (uint16)buf, i);
        }
        OSAL_PRINTF("0x%02x  ", (uint16)(*buf));
    }
    OSAL_PRINTF("\n");
}
#endif /* end of print_buf */

static uint8 this_memcmp(uint8 *s1, uint8 *s2, uint32 len){
    uint32 i;
    uint8 value;
    for(i=0;i<len;i++){
        if( 0!=(value = (s1[i]-s2[i])) ){
            return value;
        }
    }
    return 0;
}

static void send_arp(void) large{
    uint32 pktLen;
    pktBuf_t pb;
    uint16 i;

    pktLen = sizeof(arp_request)/sizeof(uint8);
    
    for(i=0;i<pktLen;i++){
        pkt_head[i] = arp_request[i];
    }
    pb.l2 = pb.pkt_head = pkt_head;
    pb.tx_dport_mode = TX_DPORT_DEFAULT;
    pb.size = pktLen;
    //print_buf(p_buffer, pktLen, "Send Arp Tx");
    txPkt(&pb);
}

void getServerMAC(void){
    uint32 pktLen = sizeof(arp_request)/sizeof(uint8);
    uint32 run_times = 0;
    uint16 i, tmp;
    int8 failReason;

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

void tftp_send_ack(uint16 port_number, uint16 block_number){
    uint32 pktLen = sizeof(udp_sent_template)/sizeof(uint8);
    pktBuf_t pb;
    uint16 ip_len, udp_len;
    uint16 i;
    
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
    
    //OSAL_PRINTF("block_number=%d\n", block_number);
    pb.l2 = pb.pkt_head = pkt_head;
    pb.tx_dport_mode = TX_DPORT_DEFAULT;
    pb.size = pktLen+sizeof(tftp_ACK);
    txPkt(&pb);
}

static uint32 erased_addr;
/*
void flash_erase_write(uint32 addr, uint8 *buf, uint16 len) large {
    uint32 end_addr;
    if (addr>=CONFIG_TOTAL_IROM_SIZE) {
        // write to banked area
        if (addr>=(erased_addr+CONFIG_FLASH_SECTOR_SIZE)) {
            erased_addr=addr&(CONFIG_FLASH_SECTOR_SIZE-1);
            flash_erase(erased_addr, FLASH_ERASE_BLOCK);
        }
        end_addr=addr+len;
        while(end_addr>=(erased_addr+CONFIG_FLASH_SECTOR_SIZE)) {
            erased_addr+=CONFIG_FLASH_SECTOR_SIZE;
            flash_erase(erased_addr, FLASH_ERASE_BLOCK);
        }
    }
    flash_write(addr, buf, len);
}
*/
#define flash_erase_write flash_write
u8_t tftp_transfer(void){
    uint32 pktLen = sizeof(udp_sent_template)/sizeof(uint8);
    uint32 program_addr = RUN_TIME_IROM_START + 2;
    uint32 image_size = 0;
    uint16 i, tmp, payload_len, rt_header_length;
    uint16 ip_len, udp_len;
    uint16 server_port, block_number;
    uint16 *uint16_ptr;
	uint8  writable;
    int8   failReason;
	
	pktBuf_t pb;
    
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
    pb.l2 = pb.pkt_head = pkt_head;
    pb.tx_dport_mode = TX_DPORT_DEFAULT;
    pb.size = pktLen+sizeof(tftp_RRQ);
    txPkt(&pb);
    
    /* Get tftp data */
    erased_addr=0;
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
                payload_len = ((pkt_head[0x26]<<8) | (pkt_head[0x27]))-12;

				rt_header_length = sizeof(runTimeImageHeader_t);

                if( ( program_addr              < (CONFIG_TOTAL_IROM_SIZE)) &&
                    ((program_addr+payload_len) > (CONFIG_TOTAL_IROM_SIZE))   )
                {
                	if( TRUE==writable ){
                        tmp = (CONFIG_TOTAL_IROM_SIZE - program_addr);
                        flash_erase_write(program_addr, &pkt_head[0x2E], tmp);
                        program_addr = LOADER_BANK_NUM*BANK_SIZE + CONFIG_TOTAL_IROM_SIZE;
                        flash_erase_write(program_addr, &pkt_head[0x2E+tmp], payload_len-tmp);
                        program_addr += payload_len-tmp ;
                	}
                    
                }else{
                    if( 0==image_size ){

                        if ( 0!=check_header(&pkt_head[0x2E]) ){
			    writable = FALSE;
                        }else{
			    writable = TRUE;
                        }
			
			if( TRUE==writable ){
			    flash_erase_write(program_addr, &pkt_head[0x2E+rt_header_length], \
				    payload_len-rt_header_length);
                            program_addr += payload_len-rt_header_length;
                        }
                    }else{
			if( TRUE==writable ){
                            flash_erase_write(program_addr, &pkt_head[0x2E], payload_len);
                            program_addr += payload_len;
                        }
                    }
                }
                
                image_size += payload_len;
                tftp_send_ack(server_port, block_number);
                if( payload_len<512 ){
                    uint16_ptr = (uint16*)&image_size;
                    //OSAL_PRINTF("Len=0x%04x, image_size=0x%04x%04x\n", payload_len, *(uint16_ptr), *(uint16_ptr+1));
                    OSAL_PRINTF("]\n");
                    OSAL_PRINTF("The Total Size is 0x%04x%04x\n", *(uint16_ptr), *(uint16_ptr+1));
                    break;
                }
            }
        }
    }
	if( TRUE==writable ){
		return 0;
	}else{
		OSAL_PRINTF("RunTime Image Error !!!\n");
	    return -1;
	}
}

