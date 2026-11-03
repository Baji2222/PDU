#pragma STRING(far)

/*
 * Copyright c                  Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * Abstract :
 *$Id:
 */
#include <string.h>
#include <stdio.h>

#include <local/web_config.h>
#include <sal/salsys.h>

#include <web/web_api.h>
#include "html_api.h"
#include "html_unauth.h"
#include "web_index.h"
#include "web_menu.h"
#include "web_panel.h"
#include "web_info.h"
#include "web_image.h"
#include "web_hidden.h"
#include "web_file.h"
#include "web_trunk.h"
#include "web_snmpsec.h"
#include "web_syslogsec.h"
#include "web_syslogbuf.h"
#include "web_port.h"
#include "web_vlan.h"
#include "web_qos.h"
#include "web_ip.h"
#include "web_user.h"
#include "web_fwd.h"
#include "web_mac.h"
#include "web_sys.h"
#include "web_loop.h"
#if __WEB_IGMP__
#include "web_igmp.h"
#endif
#include "web_eee.h"
#if __WEB_CFG_BACKUP__
#include "web_cfg.h"
#endif

#if __WEB_UPLOAD__
#include "web_fwug.h"
#endif

#if __WEB_GREENETH__
#include "web_greeneth.h"
#endif

#include "sal/salflash.h"
//extern int32 set_port1(struct http_request_s *pReq);
extern int32 set_port_handler(struct http_request_s *pReq);
//extern int32 set_port3(struct http_request_s *pReq);
//extern int32 set_port4(struct http_request_s *pReq);
//extern int32 set_port5(struct http_request_s *pReq);
//extern int32 set_port6(struct http_request_s *pReq);
//extern int32 set_port7(struct http_request_s *pReq);
//extern int32 set_port8(struct http_request_s *pReq);
extern int32 set_port(struct http_request_s *pReq);
extern int32 get_pdu_status(struct http_request_s *pReq);
extern int32 get_pdu_names(struct http_request_s *pReq);   /* FIX: route was missing */
extern int32 set_pdu_name(struct http_request_s *pReq);    /* FIX: route was missing */
extern int32 get_pdu(struct http_request_s *pReq);
extern int32 get_mac_setup(struct http_request_s *pReq);
extern int32 get_rtc_data(struct http_request_s *pReq);
extern int32 get_rtc(struct http_request_s *pReq);
extern int32 set_rtc_alarm(struct http_request_s *pReq);
extern int32 set_rtc_time_date(struct http_request_s *pReq);
extern int32 get_rtc_time(struct http_request_s *pReq);
extern int32 get_alarms(struct http_request_s *pReq);
extern int32 set_rtc_time_handler(struct http_request_s *pReq);
extern int32 set_alarm_handler(struct http_request_s *pReq, int alarm_idx);
/*
 * Function: set_mac
 * Description: Handles setting MAC address if current MAC is the default one (01:02:03:04:05:06)
 */
// Set MAC: Handles MAC address configuration via web interface
// Parses MAC string from POST data and updates system MAC if default
int32 set_mac(struct http_request_s *pReq)
{
    uint8 *pValue;
    sal_mac_t current_mac;
    sal_mac_t new_mac;
    uint8 mac_str[18];
    uint8 i, j, val;
    uint8 is_default_mac = TRUE;
    
    /* Get current MAC address */
    sal_sys_mac_get(&current_mac);
    
    /* Check if current MAC is the default MAC (01:02:03:04:05:06) */
    if (!(current_mac.octet[0] == 0xA4 && 
          current_mac.octet[1] == 0xDA && 
          current_mac.octet[2] == 0x22 && 
          current_mac.octet[3] == 0x52 && 
          current_mac.octet[4] == 0x00 && 
          current_mac.octet[5] == 0x00)) {
        is_default_mac = FALSE;
    }
    
    /* Handle POST request */
    WEB_VAR(pReq, "mac", &pValue);
    
    if (pValue && is_default_mac) {
        /* Convert MAC string (format: XX:XX:XX:XX:XX:XX) to bytes */
        memset(mac_str, 0, sizeof(mac_str));
        strncpy(mac_str, pValue, sizeof(mac_str) - 1);
        
        /* Parse MAC address */
        for (i = 0, j = 0; i < 6; i++, j += 3) {
            if ((mac_str[j] >= '0') && (mac_str[j] <= '9'))
                val = (mac_str[j] - '0') << 4;
            else if ((mac_str[j] >= 'a') && (mac_str[j] <= 'f'))
                val = (mac_str[j] - 'a' + 10) << 4;
            else if ((mac_str[j] >= 'A') && (mac_str[j] <= 'F'))
                val = (mac_str[j] - 'A' + 10) << 4;
            else
                val = 0;
            
            if ((mac_str[j+1] >= '0') && (mac_str[j+1] <= '9'))
                val |= (mac_str[j+1] - '0');
            else if ((mac_str[j+1] >= 'a') && (mac_str[j+1] <= 'f'))
                val |= (mac_str[j+1] - 'a' + 10);
            else if ((mac_str[j+1] >= 'A') && (mac_str[j+1] <= 'F'))
                val |= (mac_str[j+1] - 'A' + 10);
            
            new_mac.octet[i] = val;
        }
        
        /* Set the new MAC address */
        sal_sys_mac_set(&new_mac);
        sal_flash_sysMac_set(&new_mac);
        
        /* Return success response */
        WEB_BEGIN(pReq);
        WEB_STATUS(pReq, HTTP_STATUS_OK);
        WEB_HEADER(pReq, "Content-Type: text/plain");
        WEB_HEADER(pReq, "Access-Control-Allow-Origin: *");
        WEB_HEADER(pReq, "Access-Control-Allow-Methods: GET, POST, OPTIONS");
        WEB_HEADER(pReq, "Access-Control-Allow-Headers: Content-Type");
        WEB_PRINTF(pReq, "MAC_SET_SUCCESS");
        WEB_END(pReq);
        
        return SUCCESS;
    } else {
        /* Return error response */
        WEB_BEGIN(pReq);
        WEB_STATUS(pReq, HTTP_STATUS_OK);
        WEB_HEADER(pReq, "Content-Type: text/plain");
        WEB_HEADER(pReq, "Access-Control-Allow-Origin: *");
        WEB_HEADER(pReq, "Access-Control-Allow-Methods: GET, POST, OPTIONS");
        WEB_HEADER(pReq, "Access-Control-Allow-Headers: Content-Type");
        WEB_PRINTF(pReq, "MAC_SET_NOT_ALLOWED");
        WEB_END(pReq);
        
        return SUCCESS;
    }
}

// Default Reply ASP Send: Routes HTTP requests to appropriate handlers
// Dispatches GET and POST requests to specific page handler functions
int8 default_reply_asp_send(struct http_request_s *pReq, uint8 *pPath)
{
    uint8 *pValue = NULL;

    if (pReq == NULL || pPath == NULL)
        return FAILED;

    switch (pReq->method)
    {
    case HTTP_METHOD_GET:
        if ((strcmp(pPath, "Unauth") == 0))
        {
            return get_unauth(pReq);
        }
        else if ((strcmp(pReq->path, "/") == 0) || (strcmp(pPath, "/index.cgi") == 0))
        {
            return get_index(pReq);
        }
        else if (strcmp(pReq->path, "/hidden.cgi") == 0)
        {
            return get_hidden(pReq);
        }
        else if (strcmp(pReq->path, "/goback.cgi") == 0)
        {
            return get_goback(pReq);
        }
        else if (strcmp(pReq->path, "/alert.cgi") == 0)
        {
            WEB_VAR(pReq, "alertmsg", &pValue);

            return get_alert(pReq, (uint8 *)pValue);
        }
        /* MAC Address Setup Page */
        else if (strcmp(pReq->path, "/mac-setup") == 0 || strcmp(pReq->path, "/mac-setup.html") == 0)
        {
            return get_mac_setup(pReq);
        }
        /* images */
        else if (strcmp(pReq->path, "/logo.png") == 0)
        {
            return get_image(pReq, WEB_IMAGE_LOGO_PNG);
        }
        else if (strcmp(pReq->path, "/RJ45_up_1.png") == 0)
        {
            return get_image(pReq, WEB_IMAGE_RJ45_UP_1_PNG);
        }
        else if (strcmp(pReq->path, "/RJ45_up_0.png") == 0)
        {
            return get_image(pReq, WEB_IMAGE_RJ45_UP_0_PNG);
        }
        else if (strcmp(pReq->path, "/RJ45_down_1.png") == 0)
        {
            return get_image(pReq, WEB_IMAGE_RJ45_DOWN_1_PNG);
        }
        else if (strcmp(pReq->path, "/RJ45_down_0.png") == 0)
        {
            return get_image(pReq, WEB_IMAGE_RJ45_DOWN_0_PNG);
        }
        else if (strcmp(pReq->path, "/RJ45_loop_rldp_up.png") == 0)
        {
            return get_image(pReq, WEB_IMAGE_RJ45_LOOP_RLDP_UP_PNG);
        }
        else if (strcmp(pReq->path, "/RJ45_loop_rlpp_up.png") == 0)
        {
            return get_image(pReq, WEB_IMAGE_RJ45_LOOP_RLPP_UP_PNG);
        }
        else if (strcmp(pReq->path, "/document.gif") == 0)
        {
            return get_image(pReq, WEB_IMAGE_DOCUMENT_GIF);
        }
        else if (strcmp(pReq->path, "/folder_opened.gif") == 0)
        {
            return get_image(pReq, WEB_IMAGE_FOLDER_OPENED_GIF);
        }
        else if (strcmp(pReq->path, "/folder_closed.gif") == 0)
        {
            return get_image(pReq, WEB_IMAGE_FOLDER_CLOSED_GIF);
        }
        else if (strcmp(pReq->path, "/plus.gif") == 0)
        {
            return get_image(pReq, WEB_IMAGE_PLUS_GIF);
        }
        else if (strcmp(pReq->path, "/minus.gif") == 0)
        {
            return get_image(pReq, WEB_IMAGE_MINUS_GIF);
        }
        else if (strcmp(pReq->path, "/transparent.gif") == 0)
        {
            return get_image(pReq, WEB_IMAGE_TRANSPARENT_GIF);
        }
        /* files */
        else if (strcmp(pReq->path, "/style.css") == 0)
        {
            return get_file(pReq, WEB_FILE_STYLE_CSS);
        }
        else if (strcmp(pReq->path, "/xp_progress.js") == 0)
        {
            return get_file(pReq, WEB_FILE_XP_PROGRESS_JS);
        }
        else if (strcmp(pReq->path, "/tree.js") == 0)
        {
            return get_file(pReq, WEB_FILE_TREE_JS);
        }
        /* menu.cgi */
        else if (strcmp(pReq->path, "/menu.cgi") == 0)
        {
            return get_menu(pReq);
        }
        /* panel.cgi */
        else if (strcmp(pReq->path, "/panel.cgi") == 0)
        {
            return get_panel(pReq);
        }
        /* info.cgi */
        else if (strcmp(pPath, "/info.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);
            /* Cable Diagnostic */
            if (pValue && strcmp("cable_diag", pValue) == 0)
            {
#if __WEB_CABLE_DIAGNOSTIC__
                return get_cable_diag(pReq);
#else
                return SUCCESS;
#endif
            }
            /* Information */
            else
            {
                return get_info(pReq);
            }
        }
        /* ip.cgi */
        else if (strcmp(pReq->path, "/ip.cgi") == 0)
        {
            return get_ip(pReq);
        }
#if __WEB_EEE_ENABLE__
        /* eee.cgi */
        else if (strcmp(pReq->path, "/eee.cgi") == 0)
        {
            return get_eee(pReq);
        }
#endif

#if __WEB_GREENETH__
        /* greeneth.cgi */
        else if (strcmp(pReq->path, "/greeneth.cgi") == 0)
        {
            return get_greeneth(pReq);
        }
#endif

        /* user.cgi - Complete routing */
        else if (strcmp(pReq->path, "/user.cgi") == 0)
        {
            return get_user(pReq);
        }
        else if (strcmp(pReq->path, "/poe.cgi") == 0)
        {
            return get_pdu(pReq);
        }
        else if (strcmp(pReq->path, "/get_pdu_status") == 0)
        {
            return get_pdu_status(pReq);
        }
        else if (strcmp(pReq->path, "/get_pdu_names") == 0)
        {
            return get_pdu_names(pReq);
        }
        else if (strcmp(pReq->path, "/set_pdu_name") == 0)
        {
            return set_pdu_name(pReq);
        }
				
        else if (strcmp(pReq->path, "/set_port1") == 0)
        {
            return set_port_handler(pReq);
        }
        else if (strcmp(pReq->path, "/set_port2") == 0)
        {
            return set_port_handler(pReq);
        }
        else if (strcmp(pReq->path, "/set_port3") == 0)
        {
            return set_port_handler(pReq);
        }
        else if (strcmp(pReq->path, "/set_port4") == 0)
        {
            return set_port_handler(pReq);
        }
        else if (strcmp(pReq->path, "/set_port5") == 0)
        {
            return set_port_handler(pReq);
        }
        else if (strcmp(pReq->path, "/set_port6") == 0)
        {
            return set_port_handler(pReq);
        }
        else if (strcmp(pReq->path, "/set_port7") == 0)
        {
            return set_port_handler(pReq);
        }
        else if (strcmp(pReq->path, "/set_port8") == 0)
        {
            return set_port_handler(pReq);
        }

				/* ============================================================
				 * RTC endpoints - used by JS fetch() on the RTC page
				 * ============================================================ */

				/* --- GET /get_rtc_time  ->  {"h","m","s","d","mo","y"} --- */
				else if (strcmp(pReq->path, "/get_rtc_time") == 0 ||
								 strcmp(pPath,       "/get_rtc_time") == 0)
				{
						return get_rtc_time(pReq);
				}

				/* --- GET /get_alarms  ->  {"a1h","a1m","a2h","a2m","a3h","a3m"} --- */
				else if (strcmp(pReq->path, "/get_alarms") == 0 ||
								 strcmp(pPath,       "/get_alarms") == 0)
				{
						return get_alarms(pReq);
				}

				/* --- POST /set_rtc_time --- */
				else if (strcmp(pReq->path, "/set_rtc_time") == 0 ||
								 strcmp(pPath,       "/set_rtc_time") == 0)
				{
						return set_rtc_time_handler(pReq);
				}

				/* --- POST /set_alarm1 --- */
				else if (strcmp(pReq->path, "/set_alarm1") == 0 ||
								 strcmp(pPath,       "/set_alarm1") == 0)
				{
						return set_alarm_handler(pReq, 1);
				}

				/* --- POST /set_alarm2 --- */
				else if (strcmp(pReq->path, "/set_alarm2") == 0 ||
								 strcmp(pPath,       "/set_alarm2") == 0)
				{
						return set_alarm_handler(pReq, 2);
				}

				/* --- POST /set_alarm3 --- */
				else if (strcmp(pReq->path, "/set_alarm3") == 0 ||
								 strcmp(pPath,       "/set_alarm3") == 0)
				{
						return set_alarm_handler(pReq, 3);
				}
				
        /* port.cgi */
        else if (strcmp(pReq->path, "/port.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);

            /* Bandwidth Control */
            if (pValue && strcmp("bw_ctrl", pValue) == 0)
            {
                return get_port_bandwidthcontrol(pReq);
            }
            /* Port Statictics */
            else if (pValue && strcmp("stats", pValue) == 0)
            {
                return get_port_statistics(pReq);
            }
            /* Port Mirroring */
            else if (pValue && strcmp("mirroring", pValue) == 0)
            {
                return get_port_mirroring(pReq);
            }
            /* Port Isolation */
            else if (pValue && strcmp("isolation", pValue) == 0)
            {
                return get_port_isolation(pReq);
            }
            else
            {
                return get_port(pReq);
            }
        }
#if __WEB_SNMP__
        else if (strcmp(pReq->path, "/snmpsecurity.cgi") == 0)
        {
            u8_t ret = SUCCESS;
            WEB_VAR(pReq, "page", &pValue);

            ret = get_snmpsec(pReq);
            return SUCCESS;
        }
#endif
#if 1
        /* syslog.cgi */
        else if (strcmp(pReq->path, "/syslog.cgi") == 0)
        {
            u8_t ret = SUCCESS;

            WEB_VAR(pReq, "page", &pValue);
            ret = get_syslogsec(pReq);
            return SUCCESS;
        }
#endif
#if 1
        /* syslogbuf.cgi */
        else if (strcmp(pReq->path, "/syslogbuf.cgi") == 0)
        {
            u8_t ret = SUCCESS;

            WEB_VAR(pReq, "page", &pValue);
            ret = get_syslogbufsec(pReq);
            return SUCCESS;
        }
#endif

        else if (strcmp(pReq->path, "/trunk.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);

            return get_trunk(pReq);
        }
        /* fwd.cgi */
        else if (strcmp(pReq->path, "/fwd.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);

            /* Jumboframe */
            if (pValue && strcmp("jumboframe", pValue) == 0)
            {
                return get_fwd_jumboframe(pReq);
            }
            /* Storm Control */
            else if (pValue && strcmp("storm_ctrl", pValue) == 0)
            {
                return get_fwd_stormcontrol(pReq);
            }
            else
            {
                return get_fwd(pReq);
            }
        }
#if __WEB_LOOP__
        else if (strcmp(pReq->path, "/loop.cgi") == 0)
        {
            return get_loop(pReq);
        }
#endif
        else if (strcmp(pReq->path, "/vlan.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);

            if (pValue && strcmp("get_rtc_config", pValue) == 0)
            {
                return get_rtc(pReq);//get_vlan_static(pReq, 0);
            }
						
            else if (pValue && strcmp("set_rtc_alarm", pValue) == 0)
            {
                return set_rtc_alarm(pReq);//get_vlan_portbased(pReq);
            }
						else if (pValue && strcmp("set_rtc_data_alarm", pValue) == 0)
            {
                return set_rtc_time_date(pReq);//get_vlan_ip(pReq);
            }
        }
				
#if __WEB_IGMP__
        else if (strcmp(pReq->path, "/igmp.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);
            if (pValue && strcmp("dump", pValue) == 0)
            {
                return get_igmp_dump(pReq);
            }
        }
#endif

        /* mac.cgi */
        else if (strcmp(pReq->path, "/mac.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);

            /* MAC Forwarding Table */
            if (pValue && strcmp("fwd_tbl", pValue) == 0)
            {
                return get_mac_forwardingtable(pReq);
            }
            /* Static MAC */
            else if (pValue && strcmp("static", pValue) == 0)
            {
                return get_mac_staticmac(pReq);
            }
            /* MAC address searching */
            else if (pValue && strcmp("search", pValue) == 0)
            {
                return get_mac_search(pReq);
            }
            else
            {
                return get_mac(pReq);
            }
        }
        else if (strcmp(pReq->path, "/mac_constraint.cgi") == 0)
        {
            return get_mac_constraint(pReq);
        }
        /* qos.cgi */
        else if (strcmp(pReq->path, "/qos.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);

            /* Port-based Priority */
            if (pValue && strcmp("port_pri", pValue) == 0)
            {
                return get_qos_portbasedpriority(pReq);
            }
            /* Packet Scheduling */
            else if (pValue && strcmp("pkt_sch", pValue) == 0)
            {
                return get_qos_packetscheduling(pReq);
            }
            /* QoS internal priority selection */
            else if (pValue && strcmp("qos_int_pri", pValue) == 0)
            {
                return get_qos_internalPriority(pReq);
            }
            /* QoS dscp remapping */
            else if (pValue && strcmp("dscp_remap", pValue) == 0)
            {
                return get_qos_dscpremapping(pReq);
            }
            /* QoS priority to queue id mapping */
            else if (pValue && strcmp("pri_to_queueid", pValue) == 0)
            {
                return get_qos_pri2queueid(pReq);
            }
            else
            {
                return get_qos(pReq);
            }
        }

#if __WEB_TFTP_UPGRADE__
        /* tftp.cgi */
        else if (strcmp(pReq->path, "/tftp.cgi") == 0)
        {
            WEB_VAR(pReq, "cmd", &pValue);

            /* tftp_fwupg_process */
            if (pValue && strcmp("fwupg_status", pValue) == 0)
            {
                return get_tftp_fwupg_status(pReq);
            }
            else
            {
                return get_tftp(pReq);
            }
        }
#endif
#if __WEB_HTTP_UPGRADE__
        /* httpupg.cgi */
        else if (strcmp(pReq->path, "/httpupg.cgi") == 0)
        {
            return get_httpupg(pReq);
        }
#endif

#if (!__WEB_HTTP_UPGRADE__ && !__WEB_HTTP_UPGRADE__)
        /* fwug.cgi */
        else if (strcmp(pReq->path, "/fwug.cgi") == 0)
        {
            return get_fwug(pReq);
        }
#endif

#if __WEB_CFG_BACKUP__
        /* config_back.cgi */
        else if (strcmp(pReq->path, "/config_back.cgi") == 0)
        {
            return get_http_config(pReq);
        }
#endif
        /* reset.cgi */
        else if (strcmp(pReq->path, "/reset.cgi") == 0)
        {
            return get_reset(pReq);
        }
        /* save.cgi */
        else if (strcmp(pReq->path, "/save.cgi") == 0)
        {
            return get_save(pReq);
        }
        /* reboot.cgi */
        else if (strcmp(pReq->path, "/reboot.cgi") == 0)
        {
            return get_reboot(pReq);
        }
        else
        {
            WEB_BEGIN(pReq);
            WEB_STATUS(pReq, HTTP_STATUS_NOT_FOUND);
            WEB_HEADER(pReq, "");
            WEB_END(pReq);
        }
        break;
    case HTTP_METHOD_POST:
        /* trunk.cgi */
        if (strcmp(pReq->path, "/trunk.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);

            /* Trunk Group Setting */
            if (pValue && strcmp("group", pValue) == 0)
            {
                return post_trunk(pReq);
            }
            else if (pValue && strcmp("group_remove", pValue) == 0)
            {
                return post_trunk_remove(pReq);
            }
        }
        else if (strcmp(pReq->path, "/set_mac") == 0)
        {
            return set_mac(pReq);
        }
        else if (strcmp(pReq->path, "/ip.cgi") == 0)

        {
            printf("IP Address changed : \n");
            return post_ip(pReq);
        }
#if SAL_SNMP
        else if (strcmp(pReq->path, "/snmpsecurity.cgi") == 0)
        {
            return post_snmpsec(pReq);
        }
#endif
#if 1
        else if (strcmp(pReq->path, "/syslog.cgi") == 0)
        {
            return post_syslogsec(pReq);
        }
#endif
#if 1
        else if (strcmp(pReq->path, "/syslogbuf.cgi") == 0)
        {
            return post_syslogbufsec(pReq);
        }
#endif
#if __WEB_EEE_ENABLE__
        else if (strcmp(pReq->path, "/eee.cgi") == 0)
        {
            return post_eee(pReq);
        }
#endif

#if __WEB_GREENETH__
        else if (strcmp(pReq->path, "/greeneth.cgi") == 0)
        {
            return post_greeneth(pReq);
        }
#endif

        /* user.cgi */
        else if (strcmp(pReq->path, "/user.cgi") == 0)
        {
            return post_user(pReq);
        }
        else if (strcmp(pReq->path, "/user.cgi") == 0)
        {
            return post_user(pReq);
            
        }
        else if (strcmp(pReq->path, "/port.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);

            /* Bandwidth Control */
            if (pValue && strcmp("bwctrl", pValue) == 0)
            {
                return post_bandwidthcontrol(pReq);
            }
            /* Port Isolation */
            else if (pValue && strcmp("isolation", pValue) == 0)
            {
                return post_port_isolation(pReq);
            }
            /* Port Mirroring */
            else if (pValue && strcmp("mirroring", pValue) == 0)
            {
                return post_port_mirroring(pReq);
            }
            else if (pValue && strcmp("delete_mirror", pValue) == 0)
            {
                return post_port_mirroring_delete(pReq);
            }
            else if (pValue && strcmp("stats", pValue) == 0)
            {
                return post_port_statistics_clear(pReq);
            }
#if __WEB_RMON_COUNTER_QUERY__
            else if (pValue && strcmp("stat_rmon", pValue) == 0)
            {
                return post_port_statistics_rmonQuery(pReq);
            }
#endif
            else
            {
                return post_port(pReq);
            }
        }
        /* fwd.cgi */
        else if (strcmp(pReq->path, "/fwd.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);

            /* Bandwidth Control */
            if (pValue && strcmp("jumboframe", pValue) == 0)
            {
                return post_fwd_jumboframe(pReq);
            }
            /* Storm Control */
            else if (pValue && strcmp("storm_ctrl", pValue) == 0)
            {
                return post_fwd_stormcontrol(pReq);
            }
        }
#if __WEB_LOOP__
        else if (strcmp(pReq->path, "/loop.cgi") == 0)
        {
            return post_loop(pReq);
        }
        else if (strcmp(pReq->path, "/loop_port.cgi") == 0)
        {
            return post_loop_port(pReq);
        }
#endif
        /* vlan.cgi */
        else if (strcmp(pReq->path, "/vlan.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);
            /* Static VLAN */
            if (pValue && strcmp("static", pValue) == 0)
            {
                return post_vlan_static(pReq);
            }
            /* Port-based VLAN */
            else if (pValue && strcmp("port_based", pValue) == 0)
            {
                return post_vlan_portbased(pReq);
            }
            else if (pValue && strcmp("getRmvVlanEntry", pValue) == 0)
            {
                return post_vlanstaticDelEntry(pReq);
            }
            else if (pValue && strcmp("vlan_ip", pValue) == 0)
            {
                return post_vlan_ip(pReq);
            }
        }
#if __WEB_IGMP__
        else if (strcmp(pReq->path, "/igmp.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);
            if (pValue && strcmp("enable_igmp", pValue) == 0)
            {
                return post_igmp_state(pReq);
            }
            else if (pValue && strcmp("igmp_static_router", pValue) == 0)
            {
                return post_igmp_static_router(pReq);
            }
        }
#endif
        /* mac.cgi */
        else if (strcmp(pReq->path, "/mac.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);

            if (pValue && strcmp("static", pValue) == 0)
            {
                return post_mac_add(pReq);
            }
            else if (pValue && strcmp("staticdel", pValue) == 0)
            {
                return post_mac_del(pReq);
            }
            else if (pValue && strcmp("fwd_tbl", pValue) == 0)
            {
                return post_mac_tblclear(pReq);
            }
            else if (pValue && strcmp("search", pValue) == 0)
            {
                return post_mac_search(pReq);
            }
        }
        else if (strcmp(pReq->path, "/mac_constraint.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);
            if (pValue && strcmp("set_action", pValue) == 0)
            {
                return post_mac_constraint_action(pReq);
            }
            else
            {
                return post_mac_constraint(pReq);
            }
        }
        /* qos.cgi */
        else if (strcmp(pReq->path, "/qos.cgi") == 0)
        {
            WEB_VAR(pReq, "page", &pValue);

            /* Port-based Priority */
            if (pValue && strcmp("port_pri", pValue) == 0)
            {
                return post_qos_portbasedpriority(pReq);
            }
            /* Packet Scheduling */
            else if (pValue && strcmp("pkt_sch", pValue) == 0)
            {
                // ret = post_qos_packetscheduling(pReq);
            }
            /* QoS internal priority selection */
            else if (pValue && strcmp("qos_int_pri", pValue) == 0)
            {
                return post_qos_internalPriority(pReq);
            }
            /* QoS dscp remapping */
            else if (pValue && strcmp("dscp_remap", pValue) == 0)
            {
                return post_qos_dscpremapping(pReq);
            }
            /* QoS priority to queue id mapping */
            else if (pValue && strcmp("pri_to_queueid", pValue) == 0)
            {
                return post_qos_pri2queueid(pReq);
            }
            else if (pValue && strcmp("que_weight", pValue) == 0)
            {
                return post_qos_queueweight(pReq);
            }
        }
#if __WEB_TFTP_UPGRADE__
        /* tftp.cgi */
        else if (strcmp(pReq->path, "/tftp.cgi") == 0)
        {
            WEB_VAR(pReq, "cmd", &pValue);

            if (pValue && strcmp("fwupg", pValue) == 0)
            {
                return post_tftp_fwupg(pReq);
            }
        }
#endif
#if __WEB_HTTP_UPGRADE__
        else if (strcmp(pReq->path, "/httpupg.cgi") == 0)
        {
            WEB_VAR(pReq, "cmd", &pValue);

            if (pValue && (0 == strcmp(pValue, "fw_upgrade")))
            {
                return post_http_firmupdate(pReq);
            }
            else
            {
                WEB_BEGIN(pReq);

                WEB_STATUS(pReq, HTTP_STATUS_NOT_FOUND);

                WEB_END(pReq);
            }
        }
#endif

#if (!__WEB_HTTP_UPGRADE__ && !__WEB_HTTP_UPGRADE__)
        /* fwug.cgi */
        else if (strcmp(pReq->path, "/fwug.cgi") == 0)
        {
            return post_fwug(pReq);
        }
#endif

#if __WEB_CFG_BACKUP__
        /* config_back.cgi */
        else if (strcmp(pReq->path, "/config_back.cgi") == 0)
        {
            WEB_VAR(pReq, "cmd", &pValue);

            if (pValue && (0 == strcmp(pValue, "conf_restore")))
            {
                return post_http_restore(pReq);
            }
            else
            {
                WEB_BEGIN(pReq);

                WEB_STATUS(pReq, HTTP_STATUS_NOT_FOUND);

                WEB_END(pReq);
            }
        }
#endif
        /* reset.cgi */
        else if (strcmp(pReq->path, "/reset.cgi") == 0)
        {
            return post_factdefault_load(pReq);
        }
        /* save.cgi */
        else if (strcmp(pReq->path, "/save.cgi") == 0)
        {
            // return post_save(pReq);
        }
        /* reboot.cgi */
        else if (strcmp(pReq->path, "/reboot.cgi") == 0)
        {
            return post_reboot(pReq);
        }
        /* NOT FOUND */
        else
        {
            WEB_BEGIN(pReq);
            WEB_STATUS(pReq, HTTP_STATUS_NOT_FOUND);
            WEB_HEADER(pReq, "");
            WEB_END(pReq);
        }
        break;

    default:
        WEB_BEGIN(pReq);
        WEB_STATUS(pReq, HTTP_STATUS_NOT_IMPLEMENTED);
        WEB_HEADER(pReq, "");
        WEB_END(pReq);
        break;
    }

    return FAILED;
}