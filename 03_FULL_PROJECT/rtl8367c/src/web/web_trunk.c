#pragma STRING (far)

#include <stdio.h>

#include <saltrunk.h>
#include <salflash.h>
#include <salutil.h>
#include <local/web_config.h>
#include <salsys.h>

#include <web/web_api.h>
#include <web/http_debug.h>
#include "web_util.h"
#include "web_trunk.h"

/*
 * Function: get_trunk
 * -------------------
 * Displays trunk configuration page with group settings.
 * Shows form to create/modify trunk groups with port selection.
 * Lists existing trunk groups with delete checkboxes.
 * Includes JavaScript for "Select All" functionality.
 */
int32 get_trunk(struct http_request_s *pReq)
{
    sal_pPortmask_t portmask;
    sal_lPortmask_t lportmask;
    int16           i;
    uint8           trunkId;
    uint8           trunkNum;
    uint8           portMaskStr[128];
    uint8           logicPortStr[MAX_LOGIC_ID_LEN];

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Trunking Information</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "<script type=\"text/javascript\">\n");
    WEB_PRINTF(pReq, "function trunk_selAll(trunkNum)\n");
    WEB_PRINTF(pReq, "{\n");
    WEB_PRINTF(pReq, "  for ( i = 0; i < trunkNum; i++)\n");
    WEB_PRINTF(pReq, "      document.getElementById(\"trunk_\"+i).checked = true;\n");
    WEB_PRINTF(pReq, "}\n");
    WEB_PRINTF(pReq, "function chkEnable(value)\n");
    WEB_PRINTF(pReq, "{\n");
    WEB_PRINTF(pReq, "    if ( value == '1' ) {\n");
    WEB_PRINTF(pReq, "        document.getElementById('lacp_mode').value = \"0\";\n");
    WEB_PRINTF(pReq, "        document.getElementById('lacp_mode').disabled = false;\n");
    WEB_PRINTF(pReq, "    }\n");
    WEB_PRINTF(pReq, "    else { \n");
    WEB_PRINTF(pReq, "        document.getElementById('lacp_mode').value = \"0\";\n");
    WEB_PRINTF(pReq, "        document.getElementById('lacp_mode').disabled = true;\n");
    WEB_PRINTF(pReq, "    }\n");
    WEB_PRINTF(pReq, "}\n");
    WEB_PRINTF(pReq, "</script>\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");

    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>Trunk Group Setting</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/trunk.cgi?page=group\">\n");
    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th>Group ID</th>\n");
    WEB_PRINTF(pReq, "      <th>Ports</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\">\n");
    WEB_PRINTF(pReq, "        <select name=\"id\" >\n");
#if defined(LWS_5G)    
    WEB_PRINTF(pReq, "          <option value=\"%d\" selected id=trunk_sel1>Trunk%d\n", (uint16)1, (uint16)1);
#elif defined(LWS_5G_1F)    
    WEB_PRINTF(pReq, "          <option value=\"%d\" selected id=trunk_sel1>Trunk%d\n", (uint16)1, (uint16)1);
    WEB_PRINTF(pReq, "          <option value=\"%d\" id=trunk_sel2>Trunk%d\n", (uint16)2, (uint16)2);
#elif defined(LWS_8G)  
    WEB_PRINTF(pReq, "          <option value=\"%d\" selected id=trunk_sel1>Trunk%d\n", (uint16)1, (uint16)1);
    WEB_PRINTF(pReq, "          <option value=\"%d\" id=trunk_sel2>Trunk%d\n", (uint16)2, (uint16)2);
#elif defined(LWS_8G_2F)
    WEB_PRINTF(pReq, "          <option value=\"%d\" selected id=trunk_sel1>Trunk%d\n", (uint16)1, (uint16)1);
    WEB_PRINTF(pReq, "          <option value=\"%d\" id=trunk_sel2>Trunk%d\n", (uint16)2, (uint16)2);
    WEB_PRINTF(pReq, "          <option value=\"%d\" id=trunk_sel3>Trunk%d\n", (uint16)3, (uint16)3);
#else
#endif
    
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\">\n");
    WEB_PRINTF(pReq, "        <select name=\"ports\" multiple size=\"6\" id=portsel>\n");
    for (i = 0; i < SAL_MAX_PHY_PORT; ++i) {
    
        #if 0
        if (!(sal_trunk_logicId_exist(i))) {
            continue;
        }
        #endif
        sal_trunk_logicId_toStr(logicPortStr, i);
#if defined(LWS_8G_2F)
        if(i == 8)
            WEB_PRINTF(pReq, "          <option value=%d>%s\n", (uint16)16, logicPortStr);
        else if(i == 9)
            WEB_PRINTF(pReq, "          <option value=%d>%s\n", (uint16)17, logicPortStr);
        else
            WEB_PRINTF(pReq, "          <option value=%d>%s\n", i, logicPortStr);
#elif defined(LWS_5G_1F)  
        if(i == 5)
            WEB_PRINTF(pReq, "          <option value=%d>%s\n", (uint16)16, logicPortStr);
        else
            WEB_PRINTF(pReq, "          <option value=%d>%s\n", i, logicPortStr);
#else
        WEB_PRINTF(pReq, "          <option value=%d>%s\n", i, logicPortStr);
#endif
    }
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\">\n");    
    WEB_PRINTF(pReq, "    </tr>\n");
    WEB_PRINTF(pReq, "  </table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\"Add / Modify\">\n"); 
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"trunk\">\n");
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "<hr>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/trunk.cgi?page=group_remove\">\n");
    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th width=\"90\">Group ID</th>\n");
    WEB_PRINTF(pReq, "      <th width=\"200\">Ports</th>\n");
    WEB_PRINTF(pReq, "      <th  width=\"90\">Select</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");
    trunkNum = 0;
   
    for (trunkId = 0; trunkId < SAL_MAX_STATIC_TRUNK; ++trunkId) {
        sal_trunk_port_get(trunkId, NULL, &portmask, NULL);        
        if (SAL_ERR_SUCCESS == sal_pPortMask_isZero(&portmask)) {
            continue;
        }

        sal_lPortMask_copyPMask(&lportmask, &portmask);
        lmask_to_comma(portMaskStr, &lportmask);

        WEB_PRINTF(pReq, "    <tr>\n");
        WEB_PRINTF(pReq, "      <td align=\"center\">Trunk%bd</td>\n", trunkId+1);

        WEB_PRINTF(pReq, "      <td align=\"center\" style=\"width:200px;\">%s</td>\n", portMaskStr);
         
        WEB_PRINTF(pReq, "      <td align=\"center\"><input type=\"checkbox\" name=\"remove_%bu\" id=trunk_%bu></td>\n", 
            trunkId, trunkNum);
        WEB_PRINTF(pReq, "    </tr>\n");
        trunkNum ++;
    }

    WEB_PRINTF(pReq, "  </table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" name=\"Remove\" value=\"    Delete    \">\n");
    WEB_PRINTF(pReq, "  <input type=\"button\" value=\" Select All \" onclick=\"trunk_selAll(%bu)\">\n", trunkNum);
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"group_remove\">\n");
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "</center>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: post_trunk
 * --------------------
 * Processes trunk group creation/modification form submission.
 * Extracts trunk ID and selected ports from POST data.
 * Handles both Port and Trunk selections from multiple select.
 * Calls sal_trunk_port_set() to apply trunk configuration.
 */
int32 post_trunk (struct http_request_s *pReq)
{
    sal_pPortmask_t portmask;
    int32           ret;
    uint8           *pValue;
    uint8           trunkId = 0;
    
    sal_pPortMask_delAll(&portmask);

    WEB_VAR(pReq, "id", &pValue);
    if (pValue) {
        trunkId = atoi(pValue) - 1 ;
    }

    WEB_VAR(pReq, "ports", &pValue);
    if (pValue) {
        sal_pPortMask_addPort(&portmask, atoi(pValue));
    }

    while(pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue) {
            sal_pPortMask_addPort(&portmask, atoi(pValue));
        }
    }

    if ((ret = sal_trunk_port_set(trunkId, FALSE, &portmask, &portmask)) != SAL_ERR_SUCCESS)
    {
        printerr(pReq, ret);
        return SUCCESS;
    }

    
    get_trunk(pReq);

    return SUCCESS;
}

/*
 * Function: post_trunk_remove
 * ---------------------------
 * Processes trunk group deletion from form submission.
 * Scans all trunk IDs for checkbox named "remove_X".
 * Calls sal_trunk_port_del() for each selected trunk.
 * Refreshes trunk page after deletion.
 */
int32 post_trunk_remove (struct http_request_s *pReq)
{
    int32           ret;
    uint8           trunkId = 0;
    uint8           *pValue;
    uint8           removeStr[32];

    for (trunkId = 0; trunkId < SAL_MAX_STATIC_TRUNK; ++trunkId) {
        sprintf(removeStr, "remove_%bu", trunkId);
        WEB_VAR(pReq, removeStr, &pValue);
        if (pValue) {
            if ((ret = sal_trunk_port_del(trunkId)) != SAL_ERR_SUCCESS)
            {
                printerr(pReq, ret);
                return SUCCESS;
            }
        }
    }


    get_trunk(pReq);

    return SUCCESS;
}