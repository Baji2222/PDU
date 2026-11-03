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

#include <salutil.h>
#include <salsys.h>
#include <salflash.h>

#include <web/web_api.h>
#include "web_ip.h"
#include "web_util.h"
#include <syslog/syslogdefs.h>

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
 * Function: get_ip
 * ----------------
 * Displays IP configuration page with DHCP and static settings.
 * Retrieves current IP, netmask, gateway from system.
 * Shows DHCP state and pre-fills IP fields accordingly.
 * Uses JavaScript to enable/disable IP fields based on DHCP selection.
 * Displays warning confirmation when changing IP addresses.
 */
int32 get_ip(struct http_request_s *pReq)
{
    sal_ipaddr_t    ip = 0;
    sal_ipaddr_t    netmask = 0;
    sal_ipaddr_t    gateway = 0;
    uint8           ipStr[16];
    uint8           netmaskStr[16];
    uint8           gatewayStr[16];
    sal_enable_t    dhcp_state;

    ipStr[0] = '\0';
    netmaskStr[0] = '\0';
    gatewayStr[0] = '\0';

    sal_sys_ip_get(&ip, &netmask);
    
    ip_to_str(ipStr, (uint8 *)&ip);
    ip_to_str(netmaskStr, (uint8 *)&netmask);

    sal_sys_gateway_get(&gateway);
        
    ip_to_str(gatewayStr, (uint8 *)&gateway);

    sal_sys_dhcp_get(&dhcp_state);

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    // Replace with minimal modern styling
    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>IP Address Setting</title>\n");
    WEB_PRINTF(pReq, "<style>\n");
    WEB_PRINTF(pReq, "body{font-family:Arial,sans-serif;background:#f4f7fa;margin:0;padding:20px;}\n");
    WEB_PRINTF(pReq, "h1{text-align:center;color:#333;}\n");
    WEB_PRINTF(pReq, ".card{background:white;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,.1);padding:20px;max-width:600px;margin:0 auto;}\n");
    WEB_PRINTF(pReq, "label{display:block;margin:8px 0;font-weight:bold;}\n");
    WEB_PRINTF(pReq, "input,select{width:100%%;padding:8px;margin-bottom:15px;border:1px solid #ddd;border-radius:4px;box-sizing:border-box;}\n");
    WEB_PRINTF(pReq, "input:disabled{background:#f5f5f5;}\n");
    WEB_PRINTF(pReq, ".btn{background:#1e88e5;color:white;border:none;padding:10px;border-radius:4px;cursor:pointer;width:100%%;}\n");
    WEB_PRINTF(pReq, "</style>\n");
    
    // Simplified JavaScript
    WEB_PRINTF(pReq, "<script>\n");
    WEB_PRINTF(pReq, "function chkEnable(value){\n");
    WEB_PRINTF(pReq, "var d=(value=='1');\n");
    WEB_PRINTF(pReq, "document.getElementById('ip').disabled=d;\n");
    WEB_PRINTF(pReq, "document.getElementById('netmask').disabled=d;\n");
    WEB_PRINTF(pReq, "document.getElementById('gateway').disabled=d;\n");
    WEB_PRINTF(pReq, "}\n");
    WEB_PRINTF(pReq, "function ConfirmBox(){return confirm('Change IP address will lose connection, continue?');}\n");
    WEB_PRINTF(pReq, "</script>\n");
    WEB_PRINTF(pReq, "</head>\n");
    
    // Minimal HTML structure
    WEB_PRINTF(pReq, "<body onload=\"chkEnable(document.getElementById('dhcp').value)\">\n");
    WEB_PRINTF(pReq, "<h1>IP Address Setting</h1>\n");
    WEB_PRINTF(pReq, "<div class=\"card\">\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"ip.cgi\" onSubmit=\"return ConfirmBox()\">\n");
    
    WEB_PRINTF(pReq, "<label>DHCP Client:</label>\n");
    WEB_PRINTF(pReq, "<select name=\"dhcp_state\" id=\"dhcp\" onchange=\"chkEnable(this.value)\">\n");
    if(TRUE == dhcp_state)    
    {
        WEB_PRINTF(pReq, "<option value=\"0\">Disable</option>\n");
        WEB_PRINTF(pReq, "<option value=\"1\" selected>Enable</option>\n");
    }
    else
    {
        WEB_PRINTF(pReq, "<option value=\"0\" selected>Disable</option>\n");
        WEB_PRINTF(pReq, "<option value=\"1\">Enable</option>\n");
    }
    WEB_PRINTF(pReq, "</select>\n");
    
    WEB_PRINTF(pReq, "<label for=\"ip\">IP Address:</label>\n");
    WEB_PRINTF(pReq, "<input type=\"text\" name=\"ip\" id=\"ip\" value=\"%s\" maxlength=\"15\" %s>\n", 
               ipStr, (TRUE == dhcp_state) ? "disabled" : "");
    
    WEB_PRINTF(pReq, "<label for=\"netmask\">Subnet Mask:</label>\n");
    WEB_PRINTF(pReq, "<input type=\"text\" name=\"netmask\" id=\"netmask\" value=\"%s\" maxlength=\"15\" %s>\n", 
               netmaskStr, (TRUE == dhcp_state) ? "disabled" : "");
    
    WEB_PRINTF(pReq, "<label for=\"gateway\">Default Gateway:</label>\n");
    WEB_PRINTF(pReq, "<input type=\"text\" name=\"gateway\" id=\"gateway\" value=\"%s\" maxlength=\"15\" %s>\n", 
               gatewayStr, (TRUE == dhcp_state) ? "disabled" : "");
    
    WEB_PRINTF(pReq, "<input type=\"submit\" class=\"btn\" value=\"Apply Changes\">\n");
    WEB_PRINTF(pReq, "<input type=\"hidden\" name=\"cmd\" value=\"ip\">\n");
    
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "</div>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: post_ip
 * -----------------
 * Processes IP configuration form submission.
 * Extracts DHCP state, IP, netmask, gateway from POST data.
 * Validates IP format using str_to_ip() function.
 * Checks that gateway is in same subnet as IP address.
 * Logs IP change event to syslog and applies new settings.
 */
int32 post_ip(struct http_request_s *pReq)
{
    uint8   *pValue;
    uint32  ip = 0;
    uint32  netmask = 0;
    uint32  gateway = 0;
    uint8   ipStr[16];
    uint8   netmaskStr[16];
    uint8   gatewayStr[16];
    uint8   dhcp_state;

    WEB_VAR(pReq, "dhcp_state", &pValue);
    if (pValue) 
    {
        dhcp_state = atoi(pValue);
        sal_sys_dhcp_set(dhcp_state);
    }
    
    if(FALSE == dhcp_state)
    {
        memset(ipStr, 0, sizeof(ipStr));
        WEB_VAR(pReq, "ip", &pValue);
        if (pValue) {
            memcpy(ipStr, pValue, sizeof(ipStr));
        }
        memset(netmaskStr, 0, sizeof(netmaskStr));
        WEB_VAR(pReq, "netmask", &pValue);
        if (pValue) {
            memcpy(netmaskStr, pValue, sizeof(netmaskStr));
        }
        
        if (str_to_ip(&ip, ipStr) != 0) {
            printmsg(pReq, "Invalid IP Address.");
            return SUCCESS;
        }
        if (str_to_ip(&netmask, netmaskStr) != 0) {
            printmsg(pReq, "Invalid Netmask.");
            return SUCCESS;
        }

        memset(gatewayStr, 0, sizeof(gatewayStr));
        WEB_VAR(pReq, "gateway", &pValue);
        if (pValue) {
            memcpy(gatewayStr, pValue, sizeof(gatewayStr));
            if (str_to_ip(&gateway, gatewayStr) != 0) {
                printmsg(pReq, "Invalid Gateway.");
            return SUCCESS;
        }
        if(gateway != 0)
        {
        if ((gateway & netmask) != (ip & netmask)) {
            printmsg(pReq, "IP and Gateway address must be in the same subnet.");
            return SUCCESS;
            }
        }
        }
		syslog_add_new_buf_entry(SYSLOG_MSG_MANAGEMENT_IP_CHANGED, 0);

        sal_sys_ip_set(ip, netmask);
        sal_sys_gateway_set(gateway);
	
		
    }
    get_ip(pReq);

    return SUCCESS;
}