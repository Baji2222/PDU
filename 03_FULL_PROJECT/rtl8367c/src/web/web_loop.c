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
#include <string.h>

#include <salloop.h>
#include <salerror.h> 
#include <local/web_config.h>
#include "salportmask.h"

#include <salutil.h>
#include <saltrunk.h>
#include <salsys.h>
#include <salloop.h>

#include <web/web_api.h>
#include "web_loop.h"
#include "web_util.h"
    
#if __WEB_LOOP__
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
 * Function: get_loop
 * ------------------
 * Displays Loop Prevention/Detection configuration page.
 * Shows current loop mode (Off/RLPP/RLDP/STP) with dropdown selection.
 * Displays interval time and recover time input fields.
 * For RLPP mode, shows port enable/disable and loop state table.
 * Uses JavaScript to refresh the page for status updates.
 */
int32 get_loop(struct http_request_s *pReq)
    {
        int32 ret;
        sal_loop_mode_t mode;
        uint16  interval_time;
        uint32  recover_time;
        //sal_lPortmask_t stLPortMask;
        char    acLogicId[MAX_LOGIC_ID_LEN];
        uint16   ucPortId, bEnable, bState;
        //sal_enable_t bLink;
        sal_lPortmask_t         allPortMask;
        
        WEB_BEGIN(pReq);
    
        WEB_STATUS(pReq, HTTP_STATUS_OK);
        WEB_HEADER(pReq, "Content-Type: text/html");
    
        WEB_PRINTF(pReq, "<html>\n");
        WEB_PRINTF(pReq, "<head>\n");
        WEB_PRINTF(pReq, "<title>Loop Prevention Setting</title>\n");
        WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
        WEB_PRINTF(pReq, "<script type=\"text/javascript\">\n");
        WEB_PRINTF(pReq, "  function refreshFrame() {\n");
        WEB_PRINTF(pReq, "    document.location.replace(document.location.href);}\n");
        WEB_PRINTF(pReq, "</script>\n");
        WEB_PRINTF(pReq, "</head>\n");
        WEB_PRINTF(pReq, "\n");
        WEB_PRINTF(pReq, "<body>\n");
        WEB_PRINTF(pReq, "<center>\n");
        WEB_PRINTF(pReq, "\n");
        WEB_PRINTF(pReq, "<fieldset>\n");
        WEB_PRINTF(pReq, "<legend>Loop Prevention Setting</legend>\n");
        WEB_PRINTF(pReq, "<form method=\"post\" name=\"loop\" action=\"/loop.cgi\">\n");
    
        WEB_PRINTF(pReq, "<br>\n");
        WEB_PRINTF(pReq, "<table border=\"1\">\n");
        WEB_PRINTF(pReq, "  <tr>\n");
        WEB_PRINTF(pReq, "    <th width=\"200\">Loop function</th>\n");
    
        WEB_PRINTF(pReq, "      <td width=\"150\">\n");
        WEB_PRINTF(pReq, "        <select name=\"func_type\" style=\"width:150\">\n");
    
        if ((ret = sal_loop_mode_get(&mode)) != SAL_ERR_SUCCESS) 
        {
            printerr(pReq, ret);
            return SUCCESS;
        }
    
        switch(mode)
        {
        case SAL_LOOP_MODE_RLPP:
            WEB_PRINTF(pReq, "      <option value=\"0\" >Off \n");                
            WEB_PRINTF(pReq, "      <option value=\"1\" selected >Loop Prevention \n");
            WEB_PRINTF(pReq, "      <option value=\"2\" >Loop Detection \n");
    
#if __WEB_STP__
            WEB_PRINTF(pReq, "      <option value=\"3\" >Spanning Tree\n");
#endif
    
            break;
        case SAL_LOOP_MODE_RLDP:
            WEB_PRINTF(pReq, "      <option value=\"0\" >Off \n");                        
            WEB_PRINTF(pReq, "      <option value=\"1\" >Loop Prevention \n");
            WEB_PRINTF(pReq, "      <option value=\"2\" selected>Loop Detection \n");
    
#if __WEB_STP__
            WEB_PRINTF(pReq, "      <option value=\"3\" >Spanning Tree\n");
#endif
    
            break;
        case SAL_LOOP_MODE_DISABLE:
            WEB_PRINTF(pReq, "      <option value=\"0\" selected >Off \n");                        
            WEB_PRINTF(pReq, "      <option value=\"1\" >Loop Prevention \n");
            WEB_PRINTF(pReq, "      <option value=\"2\" >Loop Detection \n");
                   
#if __WEB_STP__
            WEB_PRINTF(pReq, "      <option value=\"3\" >Spanning Tree\n");
#endif
    
            break; 
    
#if __WEB_STP__
        case SAL_LOOP_MODE_STP:
            WEB_PRINTF(pReq, "      <option value=\"0\" >Off \n");                        
            WEB_PRINTF(pReq, "      <option value=\"1\" >Loop Prevention \n");
            WEB_PRINTF(pReq, "      <option value=\"2\" >Loop Detection \n");  
            WEB_PRINTF(pReq, "      <option value=\"3\" selected >Spanning Tree\n");
            break;
#endif
    
        default:
            printmsg(pReq, "Loop hander: LOOP_HANDLER_OFF!\n");
            return SUCCESS;
        }
        
        WEB_PRINTF(pReq, "        </select>\n");
        WEB_PRINTF(pReq, "      </td>\n");
    
        WEB_PRINTF(pReq, "  </tr>\n");
    
        WEB_PRINTF(pReq, "    <tr>\n");
        WEB_PRINTF(pReq, "      <th>Time Interval (1~32767)</th>\n");
        sal_loop_interval_time_get(&interval_time);
        WEB_PRINTF(pReq, "      <td><input type=\"text\" size= \"8\" maxlength=\"7\" name=\"interval_time\" value=\"%d\">sec</td>\n", interval_time);
        WEB_PRINTF(pReq, "    </tr>\n");
        WEB_PRINTF(pReq, "    <tr>\n");
        WEB_PRINTF(pReq, "      <th>Recover Time (0 or 4~1000000)</th>\n");
        sal_loop_recover_time_get(&recover_time);
        WEB_PRINTF(pReq, "      <td><input type=\"text\" size= \"8\" maxlength=\"7\" name=\"recover_time\" value=\"%lu\">sec</td>\n", recover_time);
        WEB_PRINTF(pReq, "    </tr>\n");
    
        WEB_PRINTF(pReq, "</table>\n");
        WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
        WEB_PRINTF(pReq, "  <input type=\"submit\" value=\"   Apply   \">\n");
        WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"loop\">\n");
        WEB_PRINTF(pReq, "</form>\n");
    
        
        if(mode == SAL_LOOP_MODE_RLPP)
        {
            WEB_PRINTF(pReq, "<hr>\n");
            WEB_PRINTF(pReq, "<form method=\"post\" action=\"/loop_port.cgi\">\n");
            WEB_PRINTF(pReq, "  <table border=\"1\">\n");
            WEB_PRINTF(pReq, "    <tr>\n");
            WEB_PRINTF(pReq, "      <th>Port</th>\n");
            WEB_PRINTF(pReq, "      <th>State</th>\n");
            WEB_PRINTF(pReq, "    </tr>\n");
            WEB_PRINTF(pReq, "    <tr>\n");
            WEB_PRINTF(pReq, "      <td align=\"center\">\n");
            WEB_PRINTF(pReq, "        <select name=\"portid\" multiple size=\"6\" >\n");
#if 1
            sal_lPortMask_addAll(&allPortMask);

            for (ucPortId = 0; ucPortId < SAL_PORT_ID_CPU; ++ucPortId)
            {
                if (SAL_ERR_FAILED == sal_lPortMask_chkPort(&allPortMask, ucPortId))
                {
                    continue;
                }
                sal_trunk_logicId_toStr_rlpp(acLogicId, ucPortId);
                WEB_PRINTF(pReq, "          <option value=\"%d\">%s\n", ucPortId, acLogicId);
                
            }

#else
            for (ucPortId = 0; ucPortId < SAL_PORT_ID_CPU; ++ucPortId) {
                if (!(sal_trunk_logicId_exist(ucPortId)))
                {
                    continue;
                }

                sal_trunk_logicId_toStr_rlpp(acLogicId, ucPortId);
                WEB_PRINTF(pReq, "          <option value=\"%d\">%s\n", ucPortId, acLogicId);
            }    
#endif
            WEB_PRINTF(pReq, "      </td>\n");
    
            WEB_PRINTF(pReq, "      <td align=\"center\">\n");
            WEB_PRINTF(pReq, "        <select name=\"portEnable\">\n");
            WEB_PRINTF(pReq, "          <option value= 0 >Disable\n");
            WEB_PRINTF(pReq, "          <option value= 1 >Enable\n");
            WEB_PRINTF(pReq, "        </select>\n");
            WEB_PRINTF(pReq, "      </td>\n");
            
            WEB_PRINTF(pReq, "    </tr>\n");
            WEB_PRINTF(pReq, "  </table>\n");
    
            WEB_PRINTF(pReq, "<br>\n");
            WEB_PRINTF(pReq, "<table border=\"1\">\n");
            WEB_PRINTF(pReq, "  <tr>\n");
            WEB_PRINTF(pReq, "    <th width=\"150\">Port</th>\n");
            WEB_PRINTF(pReq, "    <th width=\"150\">Loop State</th>\n");
            WEB_PRINTF(pReq, "    <th width=\"150\">Loop Status</th>\n");
            WEB_PRINTF(pReq, "  </tr>\n");

            sal_lPortMask_addAll(&allPortMask);

            for (ucPortId = 0; ucPortId < SAL_PORT_ID_CPU; ++ucPortId)
            {
                if (SAL_ERR_FAILED == sal_lPortMask_chkPort(&allPortMask, ucPortId))
                {
                    continue;
                }
                sal_trunk_logicId_toStr_rlpp(acLogicId, ucPortId);
                sal_loop_port_enable_get(ucPortId, &bEnable);
                sal_loop_port_state_get(ucPortId, &bState);
                //sal_port_link_get(ucPortId, &bLink);
                //sal_port_link_get(ucPortId, &bLink);
                
                WEB_PRINTF(pReq, "  <tr>\n");
                WEB_PRINTF(pReq, "    <td>%s</td>\n", acLogicId);
                WEB_PRINTF(pReq, "    <td>");
                if (bEnable) {
                    WEB_PRINTF(pReq, "Enabled");
                }
                else {
                    WEB_PRINTF(pReq, "Disabled");
                }
                WEB_PRINTF(pReq, "</td>\n");
                WEB_PRINTF(pReq, "    <td>");
                switch (bState)
                {
                    case SAL_RLPP_DISABLE_STATE:
                        WEB_PRINTF(pReq, "Disable");
                        break;
                    case SAL_RLPP_BLOCKING_STATE:
                        WEB_PRINTF(pReq, "BLOCKING");
                        break; 
                    case SAL_RLPP_LISTENING_STATE:
                        WEB_PRINTF(pReq, "LISTENING");
                        break;
                    case SAL_RLPP_FORWARDING_STATE:
                        WEB_PRINTF(pReq, "FORWARDING");
                        break;
                  
                }
                WEB_PRINTF(pReq, "</td>\n");
                WEB_PRINTF(pReq, "  </tr>\n");
            }
    
            WEB_PRINTF(pReq, "</table>\n");
            
            WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
            WEB_PRINTF(pReq, "  <input type=\"submit\" style=\"width:80\" value=\"Apply\">\n"); 
            WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"rlp\">\n");
            WEB_PRINTF(pReq, "</form>\n");
        }
        WEB_PRINTF(pReq, "</fieldset>\n");
        WEB_PRINTF(pReq, "</center>\n");
        WEB_PRINTF(pReq, "</body>\n");
        WEB_PRINTF(pReq, "</html>\n");
    
        WEB_END(pReq);
    
        return SUCCESS;
    }

/*
 * Function: post_loop
 * -------------------
 * Processes Loop Prevention/Detection form submission.
 * Extracts interval time, recover time, and loop mode from POST data.
 * Validates and applies new loop mode setting via sal_loop_mode_set().
 * Returns to GET handler to refresh the page with updated state.
 */
int32 post_loop(struct http_request_s *pReq)
{
    int32           ret;
    uint8           *pValue;
    sal_loop_mode_t mode;
    uint16 int_time, rec_time;
    sal_loop_mode_t mode_old;

    WEB_VAR(pReq, "interval_time", &pValue);
    if (pValue) {
        int_time = atoi(pValue);
    }
    sal_loop_interval_time_set(int_time);

    WEB_VAR(pReq, "recover_time", &pValue);
    if (pValue) {
        rec_time = atoi(pValue);
    }
    sal_loop_recover_time_set(rec_time);

    WEB_VAR(pReq, "func_type", &pValue);
    if (pValue) {
        mode = atoi(pValue);
    }
    
    sal_loop_mode_get(&mode_old);
    if(mode_old == mode)
    {
        get_loop(pReq);
        return SAL_ERR_SUCCESS;
    }
        
    if ((ret = sal_loop_mode_set(mode)) != SAL_ERR_SUCCESS) 
    {
        printerr(pReq, ret);
        return SUCCESS;
    }

        get_loop(pReq);

    return SUCCESS;
}

/*
 * Function: post_loop_port
 * ------------------------
 * Processes port-level loop prevention configuration.
 * Extracts selected port IDs and enable/disable state from POST data.
 * Applies port enable setting to all selected ports via sal_loop_port_enable_set().
 * Returns to GET handler to refresh the page with updated port states.
 */
int32 post_loop_port (http_request_t *pReq)
{
    uint8  *pValue;
    uint16   ucPortId[MAX_LOGIC_COMBO_PORT];
    uint16   ucPortNum = 0, portEnable = 0, i;
    
    
    WEB_VAR(pReq, "portid", &pValue);
    if (pValue) {
        ucPortId[ucPortNum++] = atoi(pValue);
    }

    while(pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue) {
            ucPortId[ucPortNum++] = atoi(pValue);
        }
    }

    WEB_VAR(pReq, "portEnable", &pValue);
    if (pValue) {
        portEnable = atoi(pValue);        
    }

    for (i = 0; i < ucPortNum; ++i) {
        sal_loop_port_enable_set(ucPortId[i], portEnable);
        
    }

    get_loop(pReq);

    return SUCCESS;
}

#endif