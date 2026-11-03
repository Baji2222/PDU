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
#include <snmp_config.h>


#include <web/web_api.h>
#include "web_ip.h"
#include "web_util.h"   

extern sal_snmp_community_set(char * pReadComm, char * pWriteComm);
#if LWPS_SNMP_TRAP_ENABLE
extern void snmp_trap_dst_ip_set(u8_t dst_idx, u8_t *dst);
extern void snmp_trap_dst_ip_get(u8_t dst_idx, u8_t *dst);
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
 * Function: get_snmpsec
 * ---------------------
 * Displays SNMP security configuration page.
 * Shows community strings (Read Only, Read Write).
 * Displays system information (description, contact, location).
 * Shows trap IP address configuration.
 * Modern clean UI with grouped sections.
 */
int32 get_snmpsec(struct http_request_s *pReq)
{
    sal_ipaddr_t    ip = 0;
    uint8           ipStr[16] = { 0 } ;
    uint8           description[32] = {0};
    uint8           location[32] = {0};
    uint8           contact[32] = {0};
    uint8           rdonlycommunity[16] = {0};
    uint8           rdwrcommunity[16] = {0};

    sal_flash_snmp_trapIp_get(&ip);
    ip_to_str(ipStr, (uint8 *)&ip);
    snmp_trap_dst_ip_get(0, (u8_t *)&ip);
    ip_to_str(ipStr, (uint8 *)&ip);
    sal_sys_description_get(description);
    sal_sys_location_get(location);
    sal_sys_contact_get(contact);
	sal_flash_snmp_community_get(rdonlycommunity, rdwrcommunity);

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>SNMP Setting</title>\n");
    
    // Minimal CSS
    WEB_PRINTF(pReq, "<style>\n");
    WEB_PRINTF(pReq, "body{font-family:Arial,sans-serif;background:#f4f7fa;margin:0;padding:20px;}\n");
    WEB_PRINTF(pReq, "h1{text-align:center;color:#333;margin-bottom:20px;}\n");
    WEB_PRINTF(pReq, ".card{background:white;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,.1);padding:20px;max-width:600px;margin:0 auto;}\n");
    WEB_PRINTF(pReq, ".form-group{margin-bottom:15px;}\n");
    WEB_PRINTF(pReq, "label{display:block;margin-bottom:5px;font-weight:bold;color:#555;}\n");
    WEB_PRINTF(pReq, "input[type=text]{width:100%%;padding:8px;border:1px solid #ddd;border-radius:4px;box-sizing:border-box;}\n");
    WEB_PRINTF(pReq, ".btn{background:#1e88e5;color:white;border:none;padding:10px 15px;border-radius:4px;cursor:pointer;font-size:14px;}\n");
    WEB_PRINTF(pReq, ".btn:hover{background:#1565c0;}\n");
    WEB_PRINTF(pReq, ".form-row{display:flex;gap:20px;margin-bottom:15px;}\n");
    WEB_PRINTF(pReq, ".form-row .form-group{flex:1;}\n");
    WEB_PRINTF(pReq, ".section-title{font-size:16px;color:#555;margin:20px 0 15px;padding-bottom:8px;border-bottom:1px solid #eee;}\n");
    WEB_PRINTF(pReq, "</style>\n");
    WEB_PRINTF(pReq, "</head>\n");    
    
    // Modern body
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<div class=\"card\">\n");
    WEB_PRINTF(pReq, "<form method=\"post\" name=\"snmpsec\" action=\"/snmpsecurity.cgi?page=snmpsec\">\n");
    
    // Community Settings section
    WEB_PRINTF(pReq, "<h2 class=\"section-title\">Community Settings</h2>\n");
    WEB_PRINTF(pReq, "<div class=\"form-row\">\n");

    // Read Only Community
    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"rdonlycommunity\">Read Only Community</label>\n");
    WEB_PRINTF(pReq, "<input type=\"text\" id=\"rdonlycommunity\" name=\"rdonlycommunity\" value=\"%s\" maxlength=\"15\">\n", rdonlycommunity);
    WEB_PRINTF(pReq, "</div>\n");
    
    // Read Write Community
    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"rdwrcommunity\">Read Write Community</label>\n");
    WEB_PRINTF(pReq, "<input type=\"text\" id=\"rdwrcommunity\" name=\"rdwrcommunity\" value=\"%s\" maxlength=\"15\">\n", rdwrcommunity);
    WEB_PRINTF(pReq, "</div>\n");
    WEB_PRINTF(pReq, "</div>\n");
    
    // System Information section
    WEB_PRINTF(pReq, "<h2 class=\"section-title\">System Information</h2>\n");
    
    // System Description
    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"description\">System Description</label>\n");
    WEB_PRINTF(pReq, "<input type=\"text\" id=\"description\" name=\"description\" value=\"%s\" maxlength=\"30\">\n", description);
    WEB_PRINTF(pReq, "</div>\n");
    
    // System Contact & Location (in a row)
    WEB_PRINTF(pReq, "<div class=\"form-row\">\n");
    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"contact\">System Contact</label>\n");
    WEB_PRINTF(pReq, "<input type=\"text\" id=\"contact\" name=\"contact\" value=\"%s\" maxlength=\"30\">\n", contact);
    WEB_PRINTF(pReq, "</div>\n");
    
    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"location\">System Location</label>\n");
    WEB_PRINTF(pReq, "<input type=\"text\" id=\"location\" name=\"location\" value=\"%s\" maxlength=\"30\">\n", location);
    WEB_PRINTF(pReq, "</div>\n");
    WEB_PRINTF(pReq, "</div>\n");
    
    // Trap Settings section
    WEB_PRINTF(pReq, "<h2 class=\"section-title\">Trap Settings</h2>\n");
    
    // Trap IP
    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"ip\">Trap IP Address</label>\n");
    WEB_PRINTF(pReq, "<input type=\"text\" id=\"ip\" name=\"ip\" value=\"%s\" maxlength=\"15\">\n", ipStr);
    WEB_PRINTF(pReq, "</div>\n");
    
    // Submit button
    WEB_PRINTF(pReq, "<div style=\"text-align:center;margin-top:25px;\">\n");
    WEB_PRINTF(pReq, "<input type=\"submit\" class=\"btn\" value=\"Apply Changes\">\n");
    WEB_PRINTF(pReq, "<input type=\"hidden\" name=\"cmd\" value=\"ip\">\n");
    WEB_PRINTF(pReq, "</div>\n");
    
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "</div>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: post_snmpsec
 * ----------------------
 * Processes SNMP security configuration form submission.
 * Extracts community strings, system info, and trap IP from POST.
 * Validates IP format using str_to_ip() function.
 * Saves all SNMP settings to flash and updates system configuration.
 */
int32 post_snmpsec(struct http_request_s *pReq)
{
    uint8           *pValue;
    uint8           ipStr[16] = { 0 } ;
    sal_ipaddr_t    ip = 0;
    uint8           description[32] = {0};
    uint8           location[32] = {0};
    uint8           contact[32] = {0};
    uint8           rdonlycommunity[16] = {0};
    uint8           rdwrcommunity[16] = {0};

//    printf("\npost_snmpsec\n");

//    memset(ipStr, 0, sizeof(ipStr));
//    memset(description, 0, sizeof(description));
//    memset(location, 0, sizeof(location));
//    memset(contact, 0, sizeof(contact));
 //   memset(rdonlycommunity, 0, sizeof(rdonlycommunity));
 //   memset(rdwrcommunity, 0, sizeof(rdwrcommunity));

    WEB_VAR(pReq, "ip", &pValue);
    if (pValue) {
        memcpy(ipStr, pValue, sizeof(ipStr));
    }
        
    WEB_VAR(pReq, "description", &pValue);
    if (pValue) {
        memcpy(description, pValue, 30);
    }
        
    WEB_VAR(pReq, "contact", &pValue);
    if (pValue) {
        memcpy(contact, pValue, 30);
    }
        
    WEB_VAR(pReq, "location", &pValue);
    if (pValue) {
        memcpy(location, pValue, 30);
    }

    WEB_VAR(pReq, "rdonlycommunity", &pValue);
    if (pValue) {
        memcpy(rdonlycommunity, pValue, 16);
    }

    WEB_VAR(pReq, "rdwrcommunity", &pValue);
    if (pValue) {
        memcpy(rdwrcommunity, pValue, 16);
    }

    if (str_to_ip(&ip, ipStr) != 0) {
//        printmsg(pReq, "Invalid IP Address.");
        return SUCCESS;
    }
    
    sal_sys_description_set(description, 1); 
    sal_sys_location_set(location, 1); 
    sal_sys_contact_set(contact, 1); 

    sal_sys_rdonly_community_set(rdonlycommunity, 1); 
    sal_sys_rdwr_community_set(rdwrcommunity, 1); 

    sal_snmp_community_set(rdonlycommunity, rdwrcommunity);

    //Set the IP address here
    snmp_trap_dst_ip_set(0, (u8_t *)&ip);
    sal_flash_snmp_trapIp_set(ip);
    sal_flash_cfg_save();
   
    get_snmpsec(pReq);

    return SUCCESS;
}
#endif