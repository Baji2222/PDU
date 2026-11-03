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
 * $Revision: 7002 $
 * $Date: 2009-11-13 14:40:24 +0800 (Fri, 13 Nov 2009) $
 *
 * Purpose : System abstraction layer API for wrapping chip driver
 * Feature : ACL 
 *
 */
#ifndef __SAL_ACL_H__
#define __SAL_ACL_H__

#include <saltype.h>
#ifdef RTL8367C
#include <acl.h>
#else
#include <rtk_api.h>
#endif
/* Function Name:
 *      sal_filter_igrAcl_cfg_delAll
 * Description:
 *      Delete all ACL entries from ASIC
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS              - OK
 *      SAL_ERR_FAILED          - FAILED
 *      SAL_ERR_SMI             - SMI access error
 * Note:
 *      This function delete all ACL configuration from ASIC.
 */
extern rtk_api_ret_t sal_filter_igrAcl_cfg_delAll(void);

/* Function Name:
 *      sal_filter_igrAcl_init 
 * Description:
 *      ACL initialization function
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS              - OK
 *      SAL_ERR_FAILED          - FAILED
 *      SAL_ERR_SMI             - SMI access error
 *      SAL_ERR_NULL_POINTER    - Pointer pFilter_field or pFilter_cfg point to NULL.
 * Note:
 *      This function enable and intialize ACL function
 */
extern rtk_api_ret_t sal_filter_igrAcl_init(void);

/* Function Name:
 *      sal_filter_igrAcl_field_add
 * Description:
 *      Add comparison rule to an ACL configuration
 * Input:
 *      pFilter_cfg | The ACL configuration that this function will add comparison rule
 *      pFilter_field | The comparison rule that will be added.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS              - OK
 *      SAL_ERR_FAILED          - FAILED
 *      SAL_ERR_SMI             - SMI access error
 *      SAL_ERR_NULL_POINTER    - Pointer pFilter_field or pFilter_cfg point to NULL.
 *      SAL_ERR_INPUT - Invalid input parameters. 
 * Note:
 *      This function add a comparison rule (*pFilter_field) to an ACL configuration (*pFilter_cfg). 
 *      Pointer pFilter_cfg points to an ACL configuration structure, this structure keeps multiple ACL 
 *      comparison rules by means of linked list. Pointer pFilter_field will be added to linked 
 *      list keeped by structure that pFilter_cfg points to.
 */
extern rtk_api_ret_t sal_filter_igrAcl_field_add(rtk_filter_cfg_t* pFilter_cfg, rtk_filter_field_t* pFilter_field);

/* Function Name:
 *      sal_filter_igrAcl_cfg_add
 * Description:
 *      Add an ACL configuration to ASIC
 * Input:
 *      filter_id - Start index of ACL configuration.
 *      pFilter_cfg - The ACL configuration that this function will add comparison rule
 *      pFilter_action - Action(s) of ACL configuration.
 * Output:
 *      ruleNum - number of rules written in acl table
 * Return:
 *      SAL_ERR_SUCCESS              - OK
 *      SAL_ERR_FAILED          - FAILED
 *      SAL_ERR_SMI             - SMI access error
 *      SAL_ERR_NULL_POINTER    - Pointer pFilter_field or pFilter_cfg point to NULL.
 *      SAL_ERR_INPUT - Invalid input parameters. 
 *      SAL_ERR_ENTRY_INDEX - Invalid filter_id .
 *      SAL_ERR_NULL_POINTER - Pointer pFilter_action or pFilter_cfg point to NULL.
 *      SAL_ERR_FILTER_INACL_ACT_NOT_SUPPORT - Action is not supported in this chip.
 *      SAL_ERR_FILTER_INACL_RULE_NOT_SUPPORT - Rule is not supported.
 *      SAL_ERR_SVLAN_ENTRY_NOT_FOUND - svlan entry not exist.
 *      SAL_ERR_TBL_FULL - Table full.
 * Note:
 *      This function store pFilter_cfg, pFilter_action into ASIC. The starting
 *      index(es) is filter_id.
 */
extern rtk_api_ret_t sal_filter_igrAcl_cfg_add(rtk_filter_id_t filter_id, rtk_filter_cfg_t* pFilter_cfg, rtk_filter_action_t* pFilter_action, rtk_filter_number_t *ruleNum);

/* Function Name:
 *      sal_filter_igrAcl_cfg_del
 * Description:
 *      Delete an ACL configuration from ASIC
 * Input:
 *      filter_id | Start index of ACL configuration.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS              - OK
 *      SAL_ERR_FAILED          - FAILED
 *      SAL_ERR_SMI             - SMI access error
 *      SAL_ERR_FILTER_ENTRYIDX - Invalid filter_id.
 * Note:
 *      This function delete a group of ACL rules starting from filter_id.
 */
extern rtk_api_ret_t sal_filter_igrAcl_cfg_del(rtk_filter_id_t filter_id);

/* Function Name:
 *      sal_filter_igrAcl_cfg_get
 * Description:
 *      Get one ingress acl configuration from ASIC.
 * Input:
 *      filter_id - Start index of ACL configuration.
 * Output:
 *      pFilter_cfg - buffer pointer of ingress acl data
 *      pFilter_action - buffer pointer of ingress acl action
 * Return:
 *      SAL_ERR_SUCCESS              - OK
 *      SAL_ERR_FAILED          - FAILED
 *      SAL_ERR_SMI             - SMI access error
 *      SAL_ERR_NULL_POINTER - Pointer pFilter_action or pFilter_cfg point to NULL.
 *      SAL_ERR_FILTER_ENTRYIDX - Invalid entry index.
 * Note:
 *      This function get ACL configuration from ASIC.  
 */
extern rtk_api_ret_t sal_filter_igrAcl_cfg_get(rtk_filter_id_t filter_id, rtk_filter_cfg_raw_t *pFilter_cfg, rtk_filter_action_t *pAction);

/* Function Name:
 *      sal_filter_igrAcl_unmatchAction_set
 * Description:
 *      Set action to packets when no ACL configuration match
 * Input:
 *      port - Port id.
 *      action - Action.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS              - OK
 *      SAL_ERR_FAILED          - FAILED
 *      SAL_ERR_SMI             - SMI access error
 *      SAL_ERR_PORT_ID - Invalid port id.
 *      SAL_ERR_INPUT - Invalid input parameters.
 * Note:
 *      This function sets action of packets when no ACL configruation matches.
 */
extern rtk_api_ret_t sal_filter_igrAcl_unmatchAction_set(rtk_port_t port, rtk_filter_unmatch_action_t action);

/* Function Name:
 *      sal_filter_igrAcl_unmatchAction_get
 * Description:
 *      Get action to packets when no ACL configuration match
 * Input:
 *      port - Port id.
 * Output:
 *      pAction - Action.
 * Return:
 *      SAL_ERR_SUCCESS              - OK
 *      SAL_ERR_FAILED          - FAILED
 *      SAL_ERR_SMI             - SMI access error
 *      SAL_ERR_PORT_ID         - Invalid port id.
 *      SAL_ERR_INPUT           - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
extern rtk_api_ret_t sal_filter_igrAcl_unmatchAction_get(rtk_port_t port, rtk_filter_unmatch_action_t* pAction);

/* Function Name:
 *      sal_filter_igrAcl_state_set
 * Description:
 *      Set state of ingress ACL.
 * Input:
 *      port - Port id.
 *      state - Ingress ACL state.
 * Output:
 *      None
 * Return:
 *      SAL_ERR_SUCCESS              - OK
 *      SAL_ERR_FAILED          - FAILED
 *      SAL_ERR_SMI             - SMI access error
 *      SAL_ERR_PORT_ID         - Invalid port id.
 *      SAL_ERR_INPUT           - Invalid input parameters.
 * Note:
 *      This function sets ACL state of port.
 */
extern rtk_api_ret_t sal_filter_igrAcl_state_set(rtk_port_t port, rtk_filter_state_t state);

/* Function Name:
 *      sal_filter_igrAcl_state_get
 * Description:
 *      Get state of ingress ACL.
 * Input:
 *      port - Port id.
 * Output:
 *      pState - Ingress ACL state.
 * Return:
 *      SAL_ERR_SUCCESS              - OK
 *      SAL_ERR_FAILED          - FAILED
 *      SAL_ERR_SMI             - SMI access error
 *      SAL_ERR_PORT_ID         - Invalid port id.
 *      SAL_ERR_INPUT           - Invalid input parameters.
 * Note:
 *      This function gets ACL state of port.
 */
extern rtk_api_ret_t sal_filter_igrAcl_state_get(rtk_port_t port, rtk_filter_state_t* pState);


#endif/*__SAL_ACL_H__*/

