/* 
 * Copyright (C) 2009 Realtek Semiconductor Corp.  
 * All Rights Reserved. 
 * 
 * This program is the proprietary software of Realtek Semiconductor 
 * Corporation and/or its licensors, and only be used, duplicated,  
 * modified or distributed under the authorized license from Realtek.  
 * 
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER  
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.  
 * 
 * $Revision: 6401 $ 
 * $Date: 2009-10-14 16:03:12 +0800 (Wed, 14 Oct 2009) $ 
 * 
 * Purpose : System abstraction layer API for wrapping chip driver
 * Feature : System related
 * 
 */

/* 
 * Include Files 
 */
#include <lwps/pktbuf.h>
#include <saltype.h>
#include <salerror.h>
#include <saltrunk.h>
#include <salnic.h>
#include <salvlan.h>
#include <nic.h>
#include <vlan.h>

#include <stdio.h>

/* 
 * Symbol and Data Type Definition 
 */
 
/* 
 * Data Declaration 
 */
static uint16     current_vid;
static sal_port_t current_spa;

/* 
 * Macro Declaration 
 */

/* 
 * Function Declaration 
 */

/* Function Name:
 *      sal_nic_rxPort_get
 * Description:
 *      Get source port number of last received packet
 * Input:
 *      None
 * Output:
 *      *pSrcPort - pointer to the received port number.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED - Cannot get source port number from last packet.
 * Note:
 *      None
 */
int32 sal_nic_rxPort_get(void *pktStrPtr, sal_port_t *pSrcPort)
{
    if(SAL_NIC_NULL_SPA == current_spa)
        return SAL_ERR_FAILED;

    /* kill warning message */
    if(pktStrPtr){}

    *pSrcPort = current_spa;

    return SAL_ERR_SUCCESS;
}


/* Function Name:
 *      sal_nic_rxPort_set
 * Description:
 *      Set source port number of last received packet. This function is only called by eth.c.
 * Input:
 *      pBuf - poionter to packet structure
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED - Cannot get source port number from last packet.
 * Note:
 *      None
 */
int32 sal_nic_rxPort_set(void *pBuf)
{
    pktBuf_t *p;

    p = (pktBuf_t*)pBuf;

    if(p->pkt_cputag)
        current_spa = *(p->pkt_cputag + 7);
    else
        current_spa = SAL_NIC_NULL_SPA;

#if defined(LWS_8G_2F)
    if (current_spa == 8)
        current_spa = EXT_PORT0;
    if (current_spa == 9)
        current_spa = EXT_PORT1;            
#elif defined(LWS_5G_1F)
    if (current_spa == 6)
        current_spa = EXT_PORT0;
#else
#endif
    return SAL_ERR_SUCCESS;
}


/* Function Name:
 *      sal_nic_tx_init
 * Description:
 *      Initialize tx related structure. TX buffer are also allocated in this function.
 * Input:
 *      pktStrPtr - pointer to the packet structure.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 sal_nic_tx_init(void *pktStrPtr)
{
    pktBuf_t *pBuf;

    if(NULL == pktStrPtr)
        return SAL_ERR_NULL_POINTER;

    pBuf = pktStrPtr;
    pBuf->pkt_head = p_tx_buffer;
    pBuf->pkt_head += TAG_SIZE+FRAME_INFO_SIZE;
    pBuf->l2 = pBuf->pkt_head;

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_nic_tx
 * Description:
 *      Send packet
 * Input:
 *      pktStrPtr - pointer to the packet structure.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
int32 sal_nic_tx(void *pktStrPtr)
{
    uint8           i, tmpPort;
    sal_port_t      rPort;
    sal_pPortmask_t txPort;
    pktBuf_t        *pBuf;

    pBuf = (pktBuf_t*)pktStrPtr;
    txPort.bits[0] = 0;

    if(pBuf->tx_dst_pmsk == 0)
    {
        /* TX buf initial */
        pBuf->tx_dport_mode    = TX_DPORT_DEFAULT;
        pBuf->tx_trunk_support = TX_TRUNK_IGNORE;
        pBuf->pkt_cputag       = NULL;
        
        return  SAL_ERR_SUCCESS;
    }
    if(TX_DPORT_ASSIGN_PMSK == pBuf->tx_dport_mode && TX_TRUNK_REPRESENT_PORT == pBuf->tx_trunk_support)
    {
        for(i = 0; i < SAL_PORT_ID_CPU;i++)
        {
            if((1UL << i) & pBuf->tx_dst_pmsk)
            {
#if defined(LWS_8G_2F)
    		    if (i == EXT_PORT0){
                    tmpPort = 8;
    		    }
    		    else if (i == EXT_PORT1){
                    tmpPort = 9;	        
    		    }
    		    else{
                    tmpPort = i;
    		    }
#elif defined(LWS_5G_1F) 
    		    if (i == EXT_PORT0){
                    tmpPort = 6;
    		    }
    		    else{
                    tmpPort = i;
    		    }    		    
#else
		        tmpPort = i;
#endif
                sal_trunk_representPort_get((sal_port_t)tmpPort, &rPort);            
                txPort.bits[0] |= (1UL << rPort);
            }
        }
    }

    pBuf->tx_dst_pmsk = txPort.bits[0];

    /* Add inner VLAN tag,
     *  - Assume the pBuf->pkt_vlantag pointer to inner vlan start 
     *  - or pBuf->pkt_vlantag == NULL if there is no vlan tag in pBuf 
     */
    if(((pBuf->inner_tag_mode & TX_VLAN_ASSIGN_VID) == TX_VLAN_ASSIGN_VID) &&
        (pBuf->inner_tag != SAL_NIC_NULL_VID))
    {
        if(NULL == pBuf->pkt_vlantag)
        {
            for( i = 0; i < MAC_ADDR_LEN * 2; i++)
            {
                *(pBuf->l2 + i - VLAN_TAG_SIZE) = *(pBuf->l2 + i);
            }
            pBuf->l2 -= VLAN_TAG_SIZE;
            pBuf->size += VLAN_TAG_SIZE;
            pBuf->pkt_vlantag = pBuf->l2 + MAC_ADDR_LEN * 2;
            pBuf->pkt_head = pBuf->l2;
        }

        /* insert VLAN tag */
        *(pBuf->pkt_vlantag + 0) = 0x81;
        *(pBuf->pkt_vlantag + 1) = 0x00;
        *(pBuf->pkt_vlantag + 2) = ((pBuf->inner_tag & 0x0f00) >> 8);
        *(pBuf->pkt_vlantag + 3) = pBuf->inner_tag & 0xff;
    }

    txPkt(pBuf);

    return  SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_nic_rxVid_get
 * Description:
 *      Get VID of currently received packet
 * Input:
 *      pVid - Pointer to VID
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
int32 sal_nic_rxVid_get(uint16 *pVid)
{
    if(SAL_NIC_NULL_VID == current_vid)
    {
        *pVid = SAL_NIC_NULL_VID;
        return SAL_ERR_FAILED;
    }    

    *pVid = current_vid;

    return SAL_ERR_SUCCESS;
}

/* Function Name:
 *      sal_nic_rxVid_set
 * Description:
 *      Set VID of currently received packet to memory.
 * Input:
 *      pBuf - pointer to packet structure
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_FAILED
 * Note:
 *      None
 */
int32 sal_nic_rxVid_set(void *pBuf)
{
    pktBuf_t *p;
    sal_vlan_t  pvid = 0;
    //sal_lPortmask_t LPortMask;
    //sal_lPortmask_t LTagMask;
    //sal_fid_t FId;
    //sal_stg_t Msti;

    //rtk_vlan_t vid;
//    rtk_portmask_t  member_mask;
//    rtk_portmask_t  untag_mask;
//    rtk_fid_t       fid;
    rtk_vlan_cfg_t vlanCfg;

    p = (pktBuf_t*)pBuf;

    if(p->pkt_vlantag)
        current_vid = *(p->pkt_vlantag + 3) | ((*(p->pkt_vlantag + 2) & 0xF) << 8);
    else
    {
        sal_vlan_portPvid_get(current_spa, &pvid);
        //sal_vlan_entry_get(pvid, &LPortMask, &LTagMask, &FId, &Msti);
        rtk_vlan_get(pvid, &vlanCfg);
        //if(LPortMask.bits[0] & (1 << current_spa))
        if(vlanCfg.mbr.bits[0]& (1UL << current_spa))
        {
            current_vid = pvid;
        }
        else
        {
        current_vid = SAL_NIC_NULL_VID;
        }
    }
//printf("sal_nic_rxVid_set current_vid=%u pvid=%u current_spa=%u member_mask.bits[0]=%lu \n",current_vid,pvid, (uint16)current_spa,vlanCfg.mbr.bits[0]);        
    
    return SAL_ERR_SUCCESS;
}


