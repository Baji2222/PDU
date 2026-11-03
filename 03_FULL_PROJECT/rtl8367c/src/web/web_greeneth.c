/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : The handler of HTTP request to green ethernet function
 *
 * Feature : Response the related request of green ethernet function
 *
 */
#pragma STRING (far)

/*
 * Include Files
 */
#include <stdlib.h>

#include <salerror.h>
#include <salgreeneth.h>
#include <salsys.h>
#include <web/http_types.h>
#include <web/web_api.h>

#include <local/web_config.h>

#if __WEB_GREENETH__

/*
 * Function Declaration
 */

/*
 * Function: get_greeneth
 * ----------------------
 * Displays Green Ethernet configuration page.
 * Reads current Green Ethernet enable/disable state.
 * Renders HTML with dropdown showing current state.
 * Highlights selected option (Enable/Disable) in dropdown.
 */
int32 get_greeneth(struct http_request_s *pReq)
{
    sal_enable_t gr_enable;
    
//    WEB_BEGIN(pReq);

//    WEB_STATUS(pReq, HTTP_STATUS_OK);
//    WEB_HEADER(pReq, "Content-Type: text/html");

//    WEB_PRINTF(pReq, "<html>\n");
//    WEB_PRINTF(pReq, "<head>\n");
//    WEB_PRINTF(pReq, "<title>Green Ethernet Setting</title>\n");
//    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
//    WEB_PRINTF(pReq, "</head>\n");
//    WEB_PRINTF(pReq, "\n");
//    WEB_PRINTF(pReq, "<body>\n");
//    WEB_PRINTF(pReq, "<center>\n");
//    WEB_PRINTF(pReq, "\n");
//    WEB_PRINTF(pReq, "<fieldset>\n");
//    WEB_PRINTF(pReq, "<legend>Green Ethernet Setting</legend>\n");
//    WEB_PRINTF(pReq, "<form method=\"post\" name=\"greeneth\" action=\"/greeneth.cgi\">\n");

//    WEB_PRINTF(pReq, "<br>\n");
//    WEB_PRINTF(pReq, "<table border=\"1\">\n");
//    WEB_PRINTF(pReq, "  <tr>\n");
//    WEB_PRINTF(pReq, "    <th width=\"200\">Green Ethernet</th>\n");

//    WEB_PRINTF(pReq, "      <td width=\"150\">\n");
//    WEB_PRINTF(pReq, "        <select name=\"state\" style=\"width:150\">\n");

//    sal_greenEth_enable_get(&gr_enable);

//    if(SAL_ENABLED == gr_enable)
    {
//        WEB_PRINTF(pReq, "      <option value=\"0\" >Disable \n");                
//        WEB_PRINTF(pReq, "      <option value=\"1\" selected >Enable \n");
    }
//    else
    {
//        WEB_PRINTF(pReq, "      <option value=\"0\" selected >Disable \n");                
//        WEB_PRINTF(pReq, "      <option value=\"1\" >Enable \n");    
    }
    
//    WEB_PRINTF(pReq, "        </select>\n");
//    WEB_PRINTF(pReq, "      </td>\n");

//    WEB_PRINTF(pReq, "  </tr>\n");
//    WEB_PRINTF(pReq, "</table>\n");
//    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
//    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\"   Apply   \">\n");
//    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"green\">\n");
//    WEB_PRINTF(pReq, "</form>\n");
//    WEB_PRINTF(pReq, "</fieldset>\n");
//    WEB_PRINTF(pReq, "</center>\n");
//    WEB_PRINTF(pReq, "</body>\n");
//    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: post_greeneth
 * -----------------------
 * Processes Green Ethernet configuration form submission.
 * Extracts 'state' value from POST data (0=Disable, 1=Enable).
 * Calls sal_greenEth_enable_set() to apply the setting.
 * Redirects back to GET handler to refresh the page.
 */
int32 post_greeneth(struct http_request_s *pReq)
{
    uint8           *pValue;
    uint8           enable;

    WEB_VAR(pReq, "state", &pValue);
    if (pValue) {
        enable = atoi(pValue);
    }

    if(enable)
        sal_greenEth_enable_set(SAL_ENABLED);
    else
        sal_greenEth_enable_set(SAL_DISABLED);

    
    get_greeneth(pReq);

    return SUCCESS;
}

#endif