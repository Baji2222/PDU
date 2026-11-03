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
#include <salflash.h>
#include <salsyslog.h>

#include <web/web_api.h>
#include "web_ip.h"
#include "web_util.h"   

#if 1
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
 * Function: get_syslogsec
 * -----------------------
 * Displays syslog configuration page.
 * Shows syslog enable/disable dropdown, server IP, and UDP port.
 * Reads current settings from flash and pre-populates fields.
 * Uses modern form layout with table-based design.
 */
int32 get_syslogsec(struct http_request_s *pReq)
{
    sal_ipaddr_t    ip = 0;
    uint8           ipStr[16] = { 0 } ;
		sal_enable_t	syslogStatus = 0;
		uint16 udpPort;
    sal_flash_syslog_serverIp_get(&ip);
    ip_to_str(ipStr, (uint8 *)&ip);

//    snmp_trap_dst_ip_get(0, (u8_t *)&ip); //?? Check this Charan
//    ip_to_str(ipStr, (uint8 *)&ip);

	sal_flash_syslog_state_get(&syslogStatus);
	sal_flash_syslog_serverPort_get(&udpPort);
	
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
	WEB_HEADER(pReq, "Content-Type: text/html");

	WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
	WEB_PRINTF(pReq, "<head>\n");
	WEB_PRINTF(pReq, "<title>Syslog Configuration</title>\n");
	WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
	WEB_PRINTF(pReq, "</head>\n");
	WEB_PRINTF(pReq, "\n");
	WEB_PRINTF(pReq, "<body>\n");
	WEB_PRINTF(pReq, "<center>\n");
	WEB_PRINTF(pReq, "\n");
	WEB_PRINTF(pReq, "<fieldset>\n");
	WEB_PRINTF(pReq, "<legend>Syslog Configuration</legend>\n");
//	WEB_PRINTF(pReq, "<form method=\"post\" name=\"syslogsec\" action=\"/syslog.cgi\">\n");
    WEB_PRINTF(pReq, "<form method=\"post\" name=\"syslogsec\" action=\"/syslog.cgi?page=syslogsec\">\n");

	WEB_PRINTF(pReq, "<br>\n");
	WEB_PRINTF(pReq, "<table border=\"1\">\n");
	WEB_PRINTF(pReq, "	<tr>\n");
	WEB_PRINTF(pReq, "	  <th width=\"200\">Syslog Function</th>\n");

	WEB_PRINTF(pReq, "		<td width=\"150\">\n");
	WEB_PRINTF(pReq, "		  <select name=\"state\" style=\"width:150\">\n");

	if(SAL_ENABLED == syslogStatus)
	{
		WEB_PRINTF(pReq, "		<option value=\"0\" >Disable \n");				  
		WEB_PRINTF(pReq, "		<option value=\"1\" selected >Enable \n");
	}
	else
	{
		WEB_PRINTF(pReq, "		<option value=\"0\" selected >Disable \n"); 			   
		WEB_PRINTF(pReq, "		<option value=\"1\" >Enable \n");	 
	}

	WEB_PRINTF(pReq, "		  </select>\n");
	WEB_PRINTF(pReq, "		</td>\n");

	WEB_PRINTF(pReq, "	</tr>\n");

    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <td>Server IP Address</td>\n");
    WEB_PRINTF(pReq, "      <td style=\"text-align:left;\"><input type=\"text\" size=\"16\" name=\"ip\" id=\"ip\" value=\"%s\" maxlength=\"15\"></td>\n", ipStr);
    WEB_PRINTF(pReq, "    </tr>\n");

    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <td>UDP Port (1-65525)</td>\n");
    WEB_PRINTF(pReq, "      <td style=\"text-align:left;\"><input type=\"text\" size=\"6\" name=\"udpPort\" id=\"udpPort\" value=\"%d\" maxlength=\"5\"></td>\n", udpPort);
    WEB_PRINTF(pReq, "    </tr>\n");
    WEB_PRINTF(pReq, "    <tr>\n");

	WEB_PRINTF(pReq, "</table>\n");
	WEB_PRINTF(pReq, "	<br style=\"line-height:50%%\">\n");
	WEB_PRINTF(pReq, "	<input type=\"submit\" value=\"   Apply   \">\n");
	WEB_PRINTF(pReq, "	<input type=\"hidden\" name=\"cmd\" value=\"syslog\">\n");
	WEB_PRINTF(pReq, "</form>\n");
	WEB_PRINTF(pReq, "</fieldset>\n");
	WEB_PRINTF(pReq, "</center>\n");
	WEB_PRINTF(pReq, "</body>\n");
	WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;

}

/*
 * Function: post_syslogsec
 * ------------------------
 * Processes syslog configuration form submission.
 * Extracts IP, UDP port, and enable state from POST data.
 * Validates IP format and port range.
 * Saves all syslog settings to flash and updates system configuration.
 */
int32 post_syslogsec(struct http_request_s *pReq)
{
    uint8           *pValue;
    uint8           ipStr[16] = { 0 } ;
    sal_ipaddr_t    ip = 0;
	uint16 udpPort;
	sal_enable_t	syslogStatus = 0;

    WEB_VAR(pReq, "ip", &pValue);
    if (pValue) {
        memcpy(ipStr, pValue, sizeof(ipStr));
    }
        
    WEB_VAR(pReq, "udpPort", &pValue);
    if (pValue) {
        udpPort = atoi(pValue);
    }

    WEB_VAR(pReq, "state", &pValue);
    if (pValue) {
        syslogStatus = atoi(pValue);
    }
	
    if (str_to_ip(&ip, ipStr) != 0) {
        printmsg(pReq, "Invalid IP Address.");
        return SUCCESS;
    }
    
    //Set the IP address here

 //   snmp_trap_dst_ip_set(0, (u8_t *)&ip);
	sal_sys_syslog_state_set(&syslogStatus,1);
	sal_syslog_state_set(syslogStatus);

	sal_sys_syslog_hostIp_set(&ip, 1);
	sal_sys_syslog_hostUdpPort_set(&udpPort, 1);

    sal_flash_syslog_serverIp_set(ip);
	sal_flash_syslog_serverPort_set(udpPort);
    sal_flash_cfg_save();
   
    get_syslogsec(pReq);

    return SUCCESS;
}
#endif