/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : The handler of HTTP request to forwarding function
 *
 * Feature : Response the related request of forwarding function
 *
 */
#pragma STRING (far)

/*
 * Include Files
 */
#include <stdio.h>
#include <stdlib.h>

#include <local/web_config.h>

#include <saligmp.h>
#include <salutil.h>

#include <web/web_api.h>
#include "web_loop.h"
#include "web_util.h"
#include <salsys.h>
#include <saltrunk.h>


#if __WEB_IGMP__  
/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/*
 * Function: get_igmp_dump
 * -----------------------
 * Displays IGMP snooping configuration and status page.
 * Shows IGMP enable/disable checkbox and static/dynamic router ports.
 * Lists all current IGMP group entries with IP, ports, and VLAN ID.
 * Provides forms to enable/disable IGMP and configure static router ports.
 */
int32 get_igmp_dump(struct http_request_s *pReq)
{
    sal_igmp_entry_t igmp_entry;
    uint8            currTail;
    char             ipStr[16];
    char             portMaskStr[128];
    sal_lPortmask_t  portmask;
    sal_enable_t     enable;
    uint16           usPortId;
    char             acLogicId[MAX_LOGIC_ID_LEN];
    bool             bPortCheck;
    sal_lPortmask_t  lPortMask;
    sal_lPortmask_t  lstaticmask, ldynamicmask;

    sal_igmp_state_get(&enable);
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>IGMP</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");


    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>IGMP Enable Setting</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" name=\"igmp\" action=\"/igmp.cgi?page=enable_igmp\">\n");
    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th bgcolor=\"#99ccff\" width=\"180\">Enable</th>\n");
    if (enable == SAL_ENABLED)
        WEB_PRINTF(pReq, "      <td style=\"text-align:left;\" width=\"180\"><input type=\"checkbox\" name=\"enable_igmp\"  checked></td>\n");
    else
        WEB_PRINTF(pReq, "      <td style=\"text-align:left;\" width=\"180\"><input type=\"checkbox\" name=\"enable_igmp\"></td>\n");
    WEB_PRINTF(pReq, "    </tr>\n");
    WEB_PRINTF(pReq, "  </table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\"   Apply   \">\n");
    WEB_PRINTF(pReq, "</form>\n");

    if(enable == SAL_ENABLED)
    {
        WEB_PRINTF(pReq, "<hr>\n");
        WEB_PRINTF(pReq, "<form method=\"post\" action=\"/igmp.cgi?page=igmp_static_router\">\n");
        WEB_PRINTF(pReq, "  <table border=\"0\" style=\"font-family:Sans-serif\">\n");
        WEB_PRINTF(pReq, "    <tr>\n");

        WEB_PRINTF(pReq, "      <td colspan=\"2\" style=\"text-align:left\">\n");
        WEB_PRINTF(pReq, "        <table border=\"1\" style=\"font-size:12px\">\n");
        WEB_PRINTF(pReq, "          <tr>\n");
        WEB_PRINTF(pReq, "            <th width=\"120\">Router Port</th>\n");
    
        for (usPortId = 0; usPortId < SAL_PORT_ID_CPU; ++usPortId)
        {
            if (!(sal_trunk_logicId_exist(usPortId)))
            {
                continue;
            }
    
            sal_trunk_logicId_toStr(acLogicId, usPortId);
            if (usPortId < SAL_MAX_PHY_PORT)
                WEB_PRINTF(pReq, "            <td>%d</td>\n", usPortId+1);
            else
                WEB_PRINTF(pReq, "            <td>%s</td>\n", acLogicId);
        }
    
        WEB_PRINTF(pReq, "          </tr>\n");
        WEB_PRINTF(pReq, "          <tr>\n");
        WEB_PRINTF(pReq, "            <th width=\"120\">static</th>\n");

        sal_igmp_staticRouter_get(&lPortMask);
        sal_trunk_physicMask_toLogicMask(&lstaticmask, &lPortMask);
        for (usPortId = 0; usPortId < SAL_PORT_ID_CPU; ++usPortId)
        {
            if (!(sal_trunk_logicId_exist(usPortId)))
            {
                continue;
            }
            bPortCheck = (SAL_ERR_SUCCESS == sal_lPortMask_chkPort(&lstaticmask, usPortId))? 1:0;
            WEB_PRINTF(pReq, "            <td align=\"center\"><input type=\"checkbox\" name=\"lPort_%u\"%s></td>\n", \
                usPortId, (bPortCheck)? " checked" : "");
        }
        WEB_PRINTF(pReq, "          </tr>\n");

        WEB_PRINTF(pReq, "          <tr>\n");
        WEB_PRINTF(pReq, "            <th width=\"120\">dynamic</th>\n");

        sal_igmp_dynamicRouter_get(&lPortMask);
        sal_trunk_physicMask_toLogicMask(&ldynamicmask, &lPortMask);
        for (usPortId = 0; usPortId < SAL_PORT_ID_CPU; ++usPortId)
        {
            if (!(sal_trunk_logicId_exist(usPortId)))
            {
                continue;
            }
            bPortCheck = (SAL_ERR_SUCCESS == sal_lPortMask_chkPort(&ldynamicmask, usPortId))? 1:0;
            WEB_PRINTF(pReq, "            <td align=\"center\"><input type=\"checkbox\" name=\"lPort_%u\"%s disabled = true></td>\n", \
                usPortId, (bPortCheck)? " checked" : "");
        }
        WEB_PRINTF(pReq, "          </tr>\n");
        WEB_PRINTF(pReq, "        </table>\n");
        WEB_PRINTF(pReq, "      </td>\n");
        WEB_PRINTF(pReq, "    </tr>\n");
        WEB_PRINTF(pReq, "  </table>\n");
        WEB_PRINTF(pReq, "  <br>\n");
        WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"set\">\n");
        WEB_PRINTF(pReq, "  <input type=\"submit\" value=\"Add / Modify\">\n");
        WEB_PRINTF(pReq, "</form>\n");
    }
    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "</center>\n");
    
    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>Dump IGMP entry</legend>\n");    
    WEB_PRINTF(pReq, "<br>\n");
    WEB_PRINTF(pReq, "<table border=\"1\">\n");
    WEB_PRINTF(pReq, "  <tr>\n");
    WEB_PRINTF(pReq, "    <th width=\"150\">IP Address</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"250\">Ports</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"50\">Vid</th>\n");
    WEB_PRINTF(pReq, "  </tr>\n");

    currTail = FALSE;
    
    while(1)
    {
        sal_igmp_nextEntry_get(&igmp_entry, &currTail);

        if(FALSE == currTail)
        {
            ipStr[0] = '\0';
            ip_to_str(ipStr, (uint8 *)&igmp_entry.group_addr);

            sal_lPortMask_copyPMask(&portmask, &igmp_entry.fwd_portmask);
            lmask_to_comma(portMaskStr, &portmask);
        
            WEB_PRINTF(pReq, "    <tr>\n");        
            WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", ipStr);
            WEB_PRINTF(pReq, "      <td align=\"center\" style=\"width:200px;\">%s</td>\n", portMaskStr);
            WEB_PRINTF(pReq, "      <td align=\"center\">%d</td>\n", igmp_entry.vid);            
            WEB_PRINTF(pReq, "    </tr>\n");            
        }
        else
        {
            break;
        }
    }
            
    WEB_PRINTF(pReq, "</table>");
    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "</center>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: post_igmp_state
 * -------------------------
 * Processes IGMP enable/disable form submission.
 * Extracts checkbox state from POST data.
 * Calls sal_igmp_state_set() to enable/disable IGMP snooping.
 * Refreshes IGMP configuration page after applying.
 */
int32 post_igmp_state (struct http_request_s *pReq)
{
    uint8  *pValue;
    sal_enable_t enable_igmp;

    WEB_VAR(pReq, "enable_igmp", &pValue);
    if (pValue)
            enable_igmp = SAL_ENABLED;
        else
            enable_igmp = SAL_DISABLED;
    

    sal_igmp_state_set(enable_igmp);

    get_igmp_dump(pReq);
    return SUCCESS;
}

/*
 * Function: post_igmp_static_router
 * ---------------------------------
 * Processes static router port configuration form submission.
 * Scans all ports for checkbox named "lPort_X".
 * Builds port mask from checked checkboxes.
 * Calls sal_igmp_staticRouter_set() to apply static router ports.
 */
int32 post_igmp_static_router (struct http_request_s *pReq)
{
    uint16   portId = 0;
    sal_lPortmask_t lPortMask;
    uint8   *pValue;
    uint8   strPort[16];    /* `lPort_xxxxxxx` */

    sal_lPortMask_delAll(&lPortMask);
    for (portId = 0; portId < SAL_PORT_ID_CPU; ++portId)
    {
        sprintf(strPort, "lPort_%u", portId);
        WEB_VAR(pReq, strPort, &pValue);
        if (pValue)
        {
            sal_lPortMask_addPort(&lPortMask, portId);
        }
    }
    sal_igmp_staticRouter_set(lPortMask);

    get_igmp_dump(pReq);
    return SUCCESS;

}
#endif