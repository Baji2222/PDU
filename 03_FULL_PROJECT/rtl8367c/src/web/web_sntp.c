#pragma STRING (far)
/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : The handler of HTTP request to IP function
 *
 * Feature : Response the request related to IP function
 *
 */

/*
 * Include Files
 */
#include <string.h>
#include <stdio.h>

#include <salutil.h>
#include <salsys.h>
#include <salsntp.h>
#include <salflash.h>

#include <web/web_api.h>
#include "web_ip.h"
#include "web_util.h"   

#if SAL_SNTP
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
 * Function: get_sntp
 * ------------------
 * Displays SNTP (Simple Network Time Protocol) configuration page.
 * Shows current SNTP enable/disable state and server IP address.
 * Uses JavaScript to enable/disable IP input field based on state.
 * Serves as the GET handler for SNTP configuration.
 */
int32 get_sntp(struct http_request_s *pReq)
{
//    sal_ipaddr_t    ip = 0;
//    sal_enable_t    sntp_state;
//    uint8           ipStr[16];

//    sal_sntp_ip_get(&ip);
//    sal_sntp_state_get(&sntp_state);

//    ipStr[0] = '\0';        
//    ip_to_str(ipStr, (uint8 *)&ip);
//    
//    WEB_BEGIN(pReq);

//    WEB_STATUS(pReq, HTTP_STATUS_OK);
//    WEB_HEADER(pReq, "Content-Type: text/html");

//    WEB_PRINTF(pReq, "<html>\n");
//    WEB_PRINTF(pReq, "\n");
//    WEB_PRINTF(pReq, "<head>\n");
//    WEB_PRINTF(pReq, "<title>SNTP Address Setting</title>\n");
//    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
//    WEB_PRINTF(pReq, "<script type=\"text/javascript\">\n");
//    WEB_PRINTF(pReq, "  function chkEnable(value)\n");
//    WEB_PRINTF(pReq, "  {\n");
//    WEB_PRINTF(pReq, "      if ( value == '0' ) {\n");
//    WEB_PRINTF(pReq, "          document.getElementById('ip').value = \'%s\';\n", ipStr);
//    WEB_PRINTF(pReq, "          document.getElementById('ip').disabled = true;\n");
//    WEB_PRINTF(pReq, "      }\n");
//    WEB_PRINTF(pReq, "      else { \n");
//    WEB_PRINTF(pReq, "          document.getElementById('ip').value = \'%s\';\n", ipStr);
//    WEB_PRINTF(pReq, "          document.getElementById('ip').disabled = false;\n");
//    WEB_PRINTF(pReq, "      }\n");
//    WEB_PRINTF(pReq, "  }\n");
//    WEB_PRINTF(pReq, "</script>\n");
//    WEB_PRINTF(pReq, "</head>\n");    
//    WEB_PRINTF(pReq, "\n");
//    WEB_PRINTF(pReq, "<center>\n");
//    WEB_PRINTF(pReq, "\n");
//    WEB_PRINTF(pReq, "<fieldset>\n");
//    WEB_PRINTF(pReq, "<legend>SNTP Setting</legend>\n");
//    WEB_PRINTF(pReq, "<form method=\"post\" action=\"sntp.cgi\" onSubmit=\"return ConfirmBox(this.form)\">\n");
//    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
//    WEB_PRINTF(pReq, "    <tr>\n");
//    WEB_PRINTF(pReq, "      <th>SNTP State</th>\n");
//    WEB_PRINTF(pReq, "      <td>\n");
//    WEB_PRINTF(pReq, "        <select name=\"sntp_state\" onload=\"chkEnable(this.value)\" onchange=\"chkEnable(this.value)\">\n");
//    if(TRUE == sntp_state)    
//    {
//        WEB_PRINTF(pReq, "          <option value=\"0\">Disable\n");
//        WEB_PRINTF(pReq, "          <option value=\"1\" selected>Enable\n");
//    }
//    else
//    {
//        WEB_PRINTF(pReq, "          <option value=\"0\" selected>Disable\n");
//        WEB_PRINTF(pReq, "          <option value=\"1\">Enable\n");
//    }
//    WEB_PRINTF(pReq, "        </select>\n");
//    WEB_PRINTF(pReq, "      </td>\n");    
//    WEB_PRINTF(pReq, "    </tr>\n");
//    WEB_PRINTF(pReq, "    <tr>\n");
//    WEB_PRINTF(pReq, "      <th>Server IP</th>\n");
//    WEB_PRINTF(pReq, "      <td style=\"text-align:left;\"><input type=\"text\" size=\"16\" name=\"ip\" id=\"ip\" value=\"%s\" maxlength=\"15\"></td>\n", ipStr);
//    WEB_PRINTF(pReq, "    </tr>\n");
//    WEB_PRINTF(pReq, "  </table>\n");
//    WEB_PRINTF(pReq, "  <br>\n");
//    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\"   Apply   \">\n");
//    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"ip\">\n");
//    WEB_PRINTF(pReq, "</form>\n");
//    WEB_PRINTF(pReq, "</fieldset>\n");
//    WEB_PRINTF(pReq, "</center>\n");
//    WEB_PRINTF(pReq, "</body>\n");
//    WEB_PRINTF(pReq, "</html>\n");

//    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: post_sntp
 * -------------------
 * Processes SNTP configuration form submission.
 * Extracts SNTP state and server IP from POST data.
 * Validates IP format using str_to_ip() function.
 * Applies SNTP settings via sal_sntp_ip_set() and sal_sntp_state_set().
 * Redirects back to GET handler to refresh the page.
 */
int32 post_sntp(struct http_request_s *pReq)
{
    uint8           *pValue;
    sal_ipaddr_t    ip;
    uint8           ipStr[16];
    uint8           sntp_state;

    WEB_VAR(pReq, "sntp_state", &pValue);
    if (pValue) 
    {
        sntp_state = atoi(pValue);
    }
    
    if(TRUE == sntp_state)
    {
        memset(ipStr, 0, sizeof(ipStr));
        WEB_VAR(pReq, "ip", &pValue);
        if (pValue) {
            memcpy(ipStr, pValue, sizeof(ipStr));
        }
        
        if (str_to_ip(&ip, ipStr) != 0) {
            printmsg(pReq, "Invalid IP Address.");
            return SUCCESS;
        }

        sal_sntp_ip_set(ip);
    }
    sal_sntp_state_set(sntp_state);    

   
    get_sntp(pReq);

    return SUCCESS;
}
#endif