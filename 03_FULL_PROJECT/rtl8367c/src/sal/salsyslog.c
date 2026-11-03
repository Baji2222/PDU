#pragma STRING(far)
#include <saltype.h>
#include <salflash.h>
#include <salerror.h>

#include <syslog/syslogdefs.h>

#if SAL_SYSLOG

/* Function Name:
 *      sal_syslog_serverIp_set
 * Description:
 *      Set IP address of destination of syslog server.
 * Input:
 *      serverIp - Destination IP Address.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_syslog_serverIp_set(sal_ipaddr_t serverIp)
{
    uint8 iparr[4];

    iparr[0] = (uint8)((serverIp & 0xFF000000) >> 24);
    iparr[1] = (uint8)((serverIp & 0xFF0000) >> 16);    
    iparr[2] = (uint8)((serverIp & 0xFF00) >> 8);    
    iparr[3] = (uint8)( serverIp & 0xFF);    
    
    return sal_flash_syslog_serverIp_set(serverIp);
}


/* Function Name:   
 *      sal_snmp_trapIp_get
 * Description:
 *      Get IP address of destination of trap packet.
 * Input:
 *      None
 * Output:
 *      pTrapIp - Pointer to Destination IP Address.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 sal_syslog_serverIp_get(sal_ipaddr_t *pServerIp)
{
    return sal_flash_syslog_serverIp_get(pServerIp);
}

/* Function Name:
 *      sal_syslog_serverPort_set
 * Description:
 *      Set port of destination of syslog server.
 * Input:
 *      serverPort - Destination port.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_syslog_serverPort_set(uint16 serverPort)
{
    return sal_flash_syslog_serverPort_set(serverPort);
}

/* Function Name:
 *      sal_syslog_serverPort_get
 * Description:
 *      Get port of destination of syslog server.
 * Input:
 *      None
 * Output:
 *      serverPort - Destination port.
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_syslog_serverPort_get(uint16 *pServerPort)
{
    return sal_flash_syslog_serverPort_get(pServerPort);
}


//kalyan syslog
sal_enable_t syslog_enable = SAL_DISABLED;

/* Function Name:
 *      sal_syslog_state_set
 * Description:
 *      Set syslog Enable state.
 * Input:
 *      enable - syslog enable state.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_syslog_state_set(sal_enable_t enable)
{

    syslog_enable = enable;
 //   snmp_trap_set_enable();
    return sal_flash_syslog_state_set(enable);
}


/* Function Name:
 *      sal_syslog_state_get
 * Description:
 *      Get state of syslog.
 * Input:
 *      None
 * Output:
 *      enable - status of syslog.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 sal_syslog_state_get(sal_enable_t *enable)
{
    return sal_flash_syslog_state_get(enable);
}



/* Function Name:
 *      sal_syslog_buf_head_idx_set
 * Description:
 *      Set head index of syslog bufer.
 * Input:
 *      head index.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_syslog_buf_head_idx_set(uint16 headIdx)
{
	syslog_head_idx_set(headIdx);
    return sal_flash_syslog_buf_head_index_set(headIdx);
}



/* Function Name:
 *      sal_syslog_buf_head_idx_get
 * Description:
 *      Get head index of syslog bufer.
 * Input:
 *      head index.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_syslog_buf_head_idx_get(uint16* pHeadIdx)
{
    return sal_flash_syslog_buf_head_index_get(pHeadIdx);
}


/* Function Name:
 *      sal_syslog_buf_head_idx_set
 * Description:
 *      Set head index of syslog bufer.
 * Input:
 *      head index.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */

int32 sal_syslog_buf_tail_idx_set(uint16 tailIdx)
{
	syslog_tail_idx_set(tailIdx);
    return sal_flash_syslog_buf_tail_index_set(tailIdx);
}


/* Function Name:
 *      sal_syslog_buf_tail_idx_get
 * Description:
 *      Get tail index of syslog bufer.
 * Input:
 *      tail index.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_syslog_buf_tail_idx_get(uint16* pTailIdx)
{
	syslog_tail_idx_get(pTailIdx);
    return sal_flash_syslog_buf_tail_index_get(pTailIdx);
}



/* Function Name:
 *      sal_syslog_config_restore
 * Description:
 *      Restore Syslog configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */

int32 sal_syslog_config_restore(void)
{
    sal_flash_syslog_t syslog;
    sal_enable_t enable;
	uint16 headIndex, tailIndex;
	
    sal_flash_syslog_state_get(&enable);
    sal_syslog_state_set(enable);

    sal_flash_syslog_serverPort_get(&syslog.sysLogServerPort);
    sal_syslog_serverPort_set(syslog.sysLogServerPort);

    sal_flash_syslog_serverIp_get(&syslog.sysLogServerIp);
    sal_syslog_serverIp_set(syslog.sysLogServerIp);

	sal_flash_syslog_buf_head_index_get(&headIndex);
	sal_syslog_buf_head_idx_set(headIndex);

	sal_flash_syslog_buf_tail_index_get(&tailIndex);
	sal_syslog_buf_tail_idx_set(tailIndex);
	printf("\n sal_syslog_config_restore, head: %d, tail:%d\n", headIndex, tailIndex);

    return SAL_ERR_SUCCESS;
}

#endif

