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
#include <salerror.h>
#include <salnic.h>
#include <nic.h>

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
        return SAL_ERR_FAILED;

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

    p = (pktBuf_t*)pBuf;

    if(p->pkt_vlantag)
        current_vid = *(p->pkt_vlantag + 3) | ((*(p->pkt_vlantag + 2) & 0xF) << 8);
    else
        current_vid = SAL_NIC_NULL_VID;
    
    return SAL_ERR_SUCCESS;
}


