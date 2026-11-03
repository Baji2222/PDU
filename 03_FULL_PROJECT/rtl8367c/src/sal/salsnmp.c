#pragma STRING(far)
#include <saltype.h>
#include <salflash.h>
#include <salerror.h>

#include <snmp/snmp_msg.h>

#if SAL_SNMP

/* Function Name:
 *      sal_snmp_trapIp_set
 * Description:
 *      Set IP address of destination of trap packet.
 * Input:
 *      trapIp - Destination IP Address.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_snmp_trapIp_set(sal_ipaddr_t trapIp)
{
    uint8 iparr[4];

    iparr[0] = (uint8)((trapIp & 0xFF000000) >> 24);
    iparr[1] = (uint8)((trapIp & 0xFF0000) >> 16);    
    iparr[2] = (uint8)((trapIp & 0xFF00) >> 8);    
    iparr[3] = (uint8)( trapIp & 0xFF);    
    
    snmp_trap_dst_ip_set(0, iparr);

    return sal_flash_snmp_trapIp_set(trapIp);
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
int32 sal_snmp_trapIp_get(sal_ipaddr_t *pTrapIp)
{
    u8_t trap_ip[4] = {0};
    //Dont read flash every timeout.
    return sal_flash_snmp_trapIp_get(pTrapIp);
    //snmp_trap_dst_ip_get(0, &trap_ip[0]);
    //memcpy((void *)pTrapIp, trap_ip, 4);
    //turn SUCCESS;
}



/* Function Name:
 *      sal_snmp_community_set
 * Description:
 *      Set read community and read-write community string.
 * Input:
 *      pReadComm   - Pointer to read community string.
 *      pWriteComm  - Pointer to write community string.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 *      SAL_ERR_PARAMETER 
 * Note:
 *      None
 */
int32 sal_snmp_community_set(char *pReadComm, char *pWriteComm)
{
    if(NULL == pReadComm || NULL == pWriteComm)
        return SAL_ERR_NULL_POINTER;

    if(strlen(pReadComm) > SAL_SNMP_COMMUNITY_STR_LEN || strlen(pWriteComm) > SAL_SNMP_COMMUNITY_STR_LEN)
        return SAL_ERR_PARAMETER;

    snmp_read_community_set(pReadComm);
    snmp_write_community_set(pWriteComm);

    return sal_flash_snmp_community_set(pReadComm, pWriteComm);
}


/* Function Name:
 *      sal_snmp_community_set
 * Description:
 *      Get read community and read-write community string.
 * Input:
 *      None 
 * Output:
 *      pReadComm   - Pointer to read community string.
 *      pWriteComm  - Pointer to write community string.
 * Return:
 *      SAL_ERR_SUCCESS
 *      SAL_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 sal_snmp_community_get(char *pReadComm, char *pWriteComm)
{
    if(NULL == pReadComm || NULL == pWriteComm)
        return SAL_ERR_NULL_POINTER;

    return sal_flash_snmp_community_get(pReadComm, pWriteComm);
}

sal_enable_t snmp_enable = SAL_ENABLED;

/* Function Name:
 *      sal_snmp_state_set
 * Description:
 *      Set snmp Enable state.
 * Input:
 *      enable - Snmp enable state.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_snmp_state_set(sal_enable_t enable)
{

    snmp_enable = enable;
    snmp_trap_set_enable();
    return sal_flash_snmp_state_set(enable);
}


/* Function Name:
 *      sal_snmp_state_get
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
int32 sal_snmp_state_get(sal_enable_t *enable)
{
    return sal_flash_snmp_state_get(enable);
}

/* Function Name:
 *      sal_snmp_config_restore
 * Description:
 *      Restore SNMP configuration from flash to system
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS
 * Note:
 *      None
 */
int32 sal_snmp_config_restore(void)
{
    sal_flash_snmp_t snmp;
    sal_enable_t enable;

    sal_flash_snmp_state_get(&enable);
    sal_snmp_state_set(enable);

    sal_flash_snmp_community_get(snmp.read_community, snmp.write_community);
    sal_snmp_community_set(snmp.read_community, snmp.write_community);

    sal_flash_snmp_trapIp_get(&snmp.trapIp);
    sal_snmp_trapIp_set(snmp.trapIp);

    snmp_inc_system_info();
    return SAL_ERR_SUCCESS;
}

#endif

