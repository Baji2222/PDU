#pragma STRING(far)

#include <string.h>
#include <stdio.h>

#include <salvlan.h>
#include <salflash.h>
#include <saltrunk.h>
#include <salportmask.h>
#include <salutil.h>
#include <salsys.h>

#include <web/web_api.h>
#include "web_vlan.h"
#include "web_util.h"
// below added by kalyan
#include "acl.h"
#include "rtk_error.h"
#include "salacl.h"
#include <rtk_api/rtk_switch.h>
#include <syslog/syslogdefs.h>
#include <flash_config.h>
#include <flash_api.h>


#ifdef RTL8367C
extern uint16 phyAllPortMask;
#endif


//u8_t vlanCount = 0; // Current count of VLAN configurations
u8_t ruleCount = 9;

u16_t vlanIds;          // Array for VLAN IDs
uint8   ipStr[16];
uint8   gatewayStr[16];

//extern int32 sal_flash_userData_set(uint32 addr , uint16 length, uint8 *pData);
//extern int32 sal_flash_userData_get(uint32 addr , uint16 length, uint8 *pData);
extern int32 sal_flash_vlan_id_set(uint32 addr , uint16 length, uint8 *pData);
extern int32 sal_flash_vlan_id_get(uint32 addr , uint16 length, uint8 *pData);

/*
 * Function: replace_ports
 * -----------------------
 * Replaces port numbers in VLAN display strings (3->1, 4->2).
 * Handles port range patterns like "3-4" -> "1-2".
 * Used to normalize VLAN display for consistent UI.
 * Modifies string in-place, skips if string is "-".
 */
void replace_ports(char *str) {
	
	char temp[64];
    char *src = str;
    char *dst = temp;
    if (strcmp(str, "-") == 0) {
        // leave as is
        return;
    }

    while (*src) {
        if (strncmp(src, "3-4", 3) == 0) {
            strcpy(dst, "1-2");
            dst += 3;
            src += 3;
        } else if (strncmp(src, "1-5", 3) == 0) {
            strcpy(dst, "1-2");
            dst += 3;
            src += 3;
        } else if (*src == '3') {
            *dst++ = '1';
            src++;
        } else if (*src == '4') {
            *dst++ = '2';
            src++;
        } else {
            *dst++ = *src++;
        }
    }

    *dst = '\0';
    strcpy(str, temp);
}

/*
 * Function: add_gateway_over_vlan_acl_rule
 * ----------------------------------------
 * Adds ACL rule to redirect gateway traffic to CPU based on VLAN.
 * Uses DIP (Destination IP) and VLAN CTAG as match criteria.
 * Allocates fields dynamically and configures redirect action.
 * Used for VLAN-based routing and management access.
 */
int add_gateway_over_vlan_acl_rule(uint16 rule_num, uint16 vlan_id, uint32 gateway_ip,uint32 subnetmask) 
{
    rtk_filter_cfg_t cfg;
    rtk_filter_action_t act;
    rtk_filter_field_t *field1 = NULL;
    rtk_filter_field_t *field2 = NULL;
    rtk_filter_number_t ruleNum;
    uint32 ret;
	
		sal_ipaddr_t    gateway = 0;
		sal_ipaddr_t    ip = 0;
    sal_ipaddr_t    netmask = 0;
	
		sal_sys_ip_get(&ip, &netmask);
		sal_sys_gateway_get(&gateway);
		
		
    field1 = (rtk_filter_field_t *)malloc(sizeof(rtk_filter_field_t));
    if (!field1) {
        printf("Memory allocation failed for field1 (DIP)\n");
        return RT_ERR_FAILED;
    }
    memset(field1, 0, sizeof(rtk_filter_field_t));

    field2 = (rtk_filter_field_t *)malloc(sizeof(rtk_filter_field_t));
    if (!field2) {
        printf("Memory allocation failed for field2 (VLAN)\n");
        free(field1);
        return RT_ERR_FAILED;
    }
    memset(field2, 0, sizeof(rtk_filter_field_t));

    memset(&cfg, 0x00, sizeof(rtk_filter_cfg_t));
    memset(&act, 0x00, sizeof(rtk_filter_action_t));

    field1->fieldType = FILTER_FIELD_IPV4_DIP; 
    field1->filter_pattern_union.dip.dataType = FILTER_FIELD_DATA_MASK;
    field1->filter_pattern_union.dip.value = gateway_ip;
    field1->filter_pattern_union.dip.mask = subnetmask; 
    ret = sal_filter_igrAcl_field_add(&cfg, field1);
    if (ret != RT_ERR_OK) {
        printf("Error adding DIP field: %lu\n", ret);
        goto cleanup;
    }

    field2->fieldType = FILTER_FIELD_CTAG;
    field2->filter_pattern_union.ctag.vid.dataType = FILTER_FIELD_DATA_MASK;
    field2->filter_pattern_union.ctag.vid.value = vlan_id;
    field2->filter_pattern_union.ctag.vid.mask = 0xFFF; 
    ret = sal_filter_igrAcl_field_add(&cfg, field2);
    if (ret != RT_ERR_OK) {
        printf("Error adding VLAN field: %lu\n", ret);
        goto cleanup;
    }

    cfg.activeport.value.bits[0] = phyAllPortMask;
    cfg.activeport.mask.bits[0] = phyAllPortMask;
    cfg.invert = FALSE;

    act.actEnable[FILTER_ENACT_REDIRECT] = TRUE;
    act.filterPortmask.bits[0] = (1UL << salPortIdCpu);

    ret = sal_filter_igrAcl_cfg_add(rule_num, &cfg, &act, &ruleNum);
    if (ret != RT_ERR_OK) {
        printf("Failed to add Gateway Rule %u (VLAN %u -> CPU): ret=%lu\n", 
               rule_num, vlan_id, ret);
    } else {
        printf("Gateway Rule %u added (VLAN %u -> CPU)\n", rule_num, vlan_id);
    }

cleanup:
    free(field1);
    free(field2);
    return ret;
}

/*
 * Function: add_ip_over_vlan_acl_rule
 * -----------------------------------
 * Adds ACL rule to redirect source IP traffic to CPU based on VLAN.
 * Uses SIP (Source IP) and VLAN CTAG as match criteria.
 * Allocates fields dynamically and configures redirect action.
 * Used for VLAN-based management access from specific IPs.
 */
int add_ip_over_vlan_acl_rule(uint16 rule_num, uint16 vlan_id, uint32 ip_address,uint32 subnetmask)
{
	
	
    rtk_filter_cfg_t cfg;
    rtk_filter_action_t act;
    rtk_filter_field_t *field1 = NULL;
    rtk_filter_field_t *field2 = NULL;
    rtk_filter_number_t ruleNum;
    uint32 ret;
	
		
		sal_ipaddr_t    ip = 0;
    sal_ipaddr_t    netmask = 0;
		sal_sys_ip_get(&ip, &netmask);
	
	  printf("Inside ACL Line 1\n ");
	  field1 = (rtk_filter_field_t *)malloc(sizeof(rtk_filter_field_t));
    if (!field1)
    {
        printf("Memory allocation failed for field1\n");
        return RT_ERR_FAILED;
    }
    memset(field1, 0, sizeof(rtk_filter_field_t));

    field2 = (rtk_filter_field_t *)malloc(sizeof(rtk_filter_field_t));
    if (!field2)
    {
        printf("Memory allocation failed for field2\n");
        free(field1);
        return RT_ERR_FAILED;
    }
    memset(field2, 0, sizeof(rtk_filter_field_t));

    memset(&cfg, 0x00, sizeof(rtk_filter_cfg_t));
    memset(&act, 0x00, sizeof(rtk_filter_action_t));

   printf("Inside ACL Line 4\n ");
    field1->fieldType = FILTER_FIELD_IPV4_SIP;
    field1->filter_pattern_union.sip.dataType = FILTER_FIELD_DATA_MASK;
    field1->filter_pattern_union.sip.value = ip_address;
    field1->filter_pattern_union.sip.mask = subnetmask; 
		   printf("Inside ACL Line 4.1\n ");
    ret = sal_filter_igrAcl_field_add(&cfg, field1);
    if (ret != RT_ERR_OK)
    {
			printf("Error adding field1: %lu\n", ret);
        free(field1);
        free(field2);
        return ret;
    }
    printf("Inside ACL Line 5\n ");
    field2->fieldType = FILTER_FIELD_CTAG;
    field2->filter_pattern_union.ctag.vid.dataType = FILTER_FIELD_DATA_MASK;
    field2->filter_pattern_union.ctag.vid.value = vlan_id;
    field2->filter_pattern_union.ctag.vid.mask = 0xFFF; 
    ret = sal_filter_igrAcl_field_add(&cfg, field2);
    if (ret != RT_ERR_OK)
    {
        free(field1);
        free(field2);
        return ret;
    }
     printf("Inside ACL Line 6\n ");
    cfg.activeport.value.bits[0] = phyAllPortMask;
		printf("Inside ACL Line 7\n ");
    cfg.activeport.mask.bits[0] = phyAllPortMask;
    cfg.invert = FALSE;
   printf("Inside ACL Line 8\n ");
    act.actEnable[FILTER_ENACT_REDIRECT] = TRUE;
    act.filterPortmask.bits[0] = (1UL << salPortIdCpu);
   printf("Inside ACL Line 9\n ");
    ret = sal_filter_igrAcl_cfg_add(rule_num, &cfg, &act, &ruleNum);
    if (RT_ERR_OK != ret) {
       printf("\n Failed to add Rule 6 By kalyan: Source IP 192.168.1.114 and VLAN ID 6 redirect to CPU, ret=%lu\n", ret);
        } else {
          printf("\n Rule 6 Updated successfully By kalyan with VID : %u, ruleNum=%u  SubnetMAsk : 0x%08x \n ", vlan_id , ruleNum , subnetmask);
         }
   
     printf("Inside ACL Line 10\n ");
    free(field1);
    free(field2);

    return ret;
}

// Modify the VLAN display section with C89-style declarations
/*
 * Function: get_vlan_static
 * -------------------------
 * Displays static VLAN configuration page with table and forms.
 * Shows VLAN list with member/tagged/untagged ports.
 * Provides form to add/modify VLAN entries.
 * Handles VLAN 1 special display and prevents deletion.
 */
int32 get_vlan_static(struct http_request_s *pReq, uint16 usVlanId)
{
    sal_flash_vlanEntry_t flashVlanEntry;
    sal_flash_vlanDb_t flashVlanDb;
    sal_lPortmask_t lportmask;
    sal_lPortmask_t ltagmask;
    sal_lPortmask_t luntagMask;
    sal_pPortmask_t untagmask;
    int16 radioIndex;
    uint16 slot = 0;
    uint16 portBreakId = 0xffff, portNum;
    uint16 radioId, radioBreakId = 0, radioNum;
    uint16 vlanNum;
    uint8 portmaskStr[128];
    uint8 tagMaskStr[128];
    uint8 untagMaskStr[128];
    uint8 logicPortStr[MAX_LOGIC_ID_LEN];
    uint8 bDrawInTwoLine = FALSE;
    uint8 port;
    Tbool bFound;
    Tbool bTaggedPort;
    Tbool bUntaggedPort;
    /* Variables for tracking unique VLANs */
    uint16 seenVlanIds[MAX_STATIC_VLAN];
    uint16 seenCount;
    uint16 i;
    uint8 isDuplicate;

    /* Load VLAN database from flash first */
    sal_flash_vlan_db_get(&flashVlanDb);
    
    /* Check if the requested VLAN ID exists */
    bFound = FALSE;
    if (usVlanId != 0) {
        for (slot = 0; ((slot < MAX_STATIC_VLAN) && (flashVlanDb.sortedSlot[slot] >= 0)); ++slot) {
            sal_flash_vlan_sttcEntry_get(flashVlanDb.sortedSlot[slot], &flashVlanEntry);
            if (flashVlanEntry.vlan_id == usVlanId) {
                bFound = TRUE;
                break;
            }
        }
    }

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    // Replace old HTML with minimal modern UI
    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Static VLAN Table</title>\n");
    
    // Minimal CSS
    WEB_PRINTF(pReq, "<style>\n");
    WEB_PRINTF(pReq, "body{font-family:Arial,sans-serif;background:#f4f7fa;margin:0;padding:20px;}\n");
    WEB_PRINTF(pReq, "h1,h2{text-align:center;color:#333;}\n");
    WEB_PRINTF(pReq, ".card{background:white;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,.1);padding:20px;max-width:900px;margin:0 auto 20px;}\n");
    WEB_PRINTF(pReq, "table{width:100%%;border-collapse:collapse;margin-bottom:15px;}\n");
    WEB_PRINTF(pReq, "th{background:#f0f0f0;padding:8px;}\n");
    WEB_PRINTF(pReq, "td{padding:8px;border-bottom:1px solid #eee;text-align:center;}\n");
    WEB_PRINTF(pReq, "input[type=text]{padding:8px;border:1px solid #ddd;border-radius:4px;width:100%%;box-sizing:border-box;}\n");
    WEB_PRINTF(pReq, ".btn{background:#1e88e5;color:white;border:none;padding:8px 16px;border-radius:4px;cursor:pointer;margin-right:8px;}\n");
    WEB_PRINTF(pReq, ".btn-sm{padding:4px 8px;font-size:12px;}\n");
    WEB_PRINTF(pReq, ".form-row{display:flex;margin-bottom:10px;gap:15px;}\n");
    WEB_PRINTF(pReq, ".form-group{flex:1;}\n");
    WEB_PRINTF(pReq, "label{display:block;margin-bottom:5px;font-weight:bold;}\n");
    WEB_PRINTF(pReq, ".radio-group td{text-align:center;}\n");
    WEB_PRINTF(pReq, "a{color:#1e88e5;text-decoration:none;}\n");
    WEB_PRINTF(pReq, "a:hover{text-decoration:underline;}\n");
    WEB_PRINTF(pReq, ".divider{border-top:1px solid #ddd;margin:20px 0;}\n");
    WEB_PRINTF(pReq, "</style>\n");
    
    // Keep original JavaScript
    WEB_PRINTF(pReq, "<script>\n");
    WEB_PRINTF(pReq, "function vlanstatic_submit(){\n");
    WEB_PRINTF(pReq, "var acVlanId=document.getElementById('vlanID').value;\n");
    WEB_PRINTF(pReq, "if(parseInt(acVlanId)>4094||parseInt(acVlanId)<=0)alert(\"Invalid Vlan ID (1-4094)\");\n");
    WEB_PRINTF(pReq, "else document.formVlanStatic.submit();\n");
    WEB_PRINTF(pReq, "}\n");
    WEB_PRINTF(pReq, "function selAll(radioIndex,portStart,portEnd){\n");
    WEB_PRINTF(pReq, "for(i=portStart;i<=portEnd;i++)document.getElementById(\"vlanPortRadio_\"+i+\"_\"+radioIndex).checked=true;\n");
    WEB_PRINTF(pReq, "}\n");
    WEB_PRINTF(pReq, "function vlanstatic_selAll(vlanNum){\n");
    WEB_PRINTF(pReq, "for(i=1;i<vlanNum;i++)document.getElementById(\"vlan_\"+i).checked=true;\n");
    WEB_PRINTF(pReq, "}\n");
    WEB_PRINTF(pReq, "</script>\n");
    WEB_PRINTF(pReq, "</head>\n");
    
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<h1>Static VLAN Configuration</h1>\n");
    
    // VLAN Add/Modify Card
    WEB_PRINTF(pReq, "<div class=\"card\">\n");
    WEB_PRINTF(pReq, "<h2>Add/Modify VLAN</h2>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/vlan.cgi?page=static\" name=\"formVlanStatic\" id=\"formVlanStatic\">\n");
    
    // VLAN ID and Name
    WEB_PRINTF(pReq, "<div class=\"form-row\">\n");
    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"vlanID\">VLAN ID (1-4094)</label>\n");
    if (bFound == 1 && usVlanId != 0)
    {
        WEB_PRINTF(pReq, "<input type=\"text\" name=\"vid\" maxlength=\"4\" id=\"vlanID\" value=\"%d\">\n",
                   (uint16)flashVlanEntry.vlan_id);
    }
    else
    {
        WEB_PRINTF(pReq, "<input type=\"text\" name=\"vid\" maxlength=\"4\" id=\"vlanID\">\n");
    }
    WEB_PRINTF(pReq, "</div>\n");
    
    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"vlanName\">VLAN Name</label>\n");
    if (bFound == 1 && usVlanId != 0)
    {
        WEB_PRINTF(pReq, "<input type=\"text\" name=\"name\" maxlength=\"16\" id=\"vlanName\" value=\"%s\">\n",
                   flashVlanEntry.name);
    }
    else
        WEB_PRINTF(pReq, "<input type=\"text\" name=\"name\" maxlength=\"16\" id=\"vlanName\">\n");
    WEB_PRINTF(pReq, "</div>\n");
    WEB_PRINTF(pReq, "</div>\n");
    
    // Port Membership Table
    WEB_PRINTF(pReq, "<table class=\"radio-group\">\n");
    WEB_PRINTF(pReq, "<tr>\n");
    WEB_PRINTF(pReq, "<th>Port</th>\n");
    WEB_PRINTF(pReq, "<th>Select All</th>\n");
    
    portNum = 0;
    for (port = 0; port < 2; ++port)
    {
        if (!(sal_trunk_logicId_exist(port + 2)))
        {
            continue;
        }

        sal_trunk_logicId_toStr(logicPortStr, port + 2);
        WEB_PRINTF(pReq, "<th>%d</th>\n", (uint16)(port + 1));
        portNum++;
    }
    radioNum = portNum;

    WEB_PRINTF(pReq, "</tr>\n");

    for (radioIndex = 0; radioIndex < 3; radioIndex++)
    {
        WEB_PRINTF(pReq, "<tr>\n");
        if (radioIndex == 0)
            WEB_PRINTF(pReq, "<td>Untagged</td>\n");
        else if (radioIndex == 1)
            WEB_PRINTF(pReq, "<td>Tagged</td>\n");
        else
            WEB_PRINTF(pReq, "<td>Not Member</td>\n");

        WEB_PRINTF(pReq, "<td><button type=\"button\" class=\"btn btn-sm\" onclick=\"selAll(%d, %d, %d)\">All</button></td>\n",
                   (uint16)radioIndex, (uint16)0, (uint16)(radioNum - 1));
        
        radioId = 0;
        memcpy(&untagmask, &flashVlanEntry.p_portMask, sizeof(sal_pPortmask_t));
        sal_pPortMask_delPorts(&untagmask, &flashVlanEntry.p_tagMask);
        sal_trunk_physicMask_toLogicMask(&ltagmask, &flashVlanEntry.p_tagMask);
        sal_trunk_physicMask_toLogicMask(&luntagMask, &untagmask);

        if (bFound == 1 && usVlanId != 0)
        {
            for (port = 0; port < 2; ++port)
            {
                if (!(sal_trunk_logicId_exist(port + 2)))
                {
                    continue;
                }

                WEB_PRINTF(pReq, "<td><input type=\"radio\" name=\"vlanPort_%d\" id=\"vlanPortRadio_%d_%d\" value=\"%d\"",
                           (uint16)(port + 2), (uint16)radioId, (uint16)radioIndex, (uint16)radioIndex);

                if (SAL_ERR_SUCCESS == sal_lPortMask_chkPort(&ltagmask, port + 2))
                    bTaggedPort = TRUE;
                else
                    bTaggedPort = FALSE;

                if (SAL_ERR_SUCCESS == sal_lPortMask_chkPort(&luntagMask, port + 2))
                    bUntaggedPort = TRUE;
                else
                    bUntaggedPort = FALSE;

                if ((radioIndex == 0 && bUntaggedPort == 1) || (radioIndex == 1 && bTaggedPort == 1))
                    WEB_PRINTF(pReq, " checked></td>\n");
                else if (radioIndex == 2 && bTaggedPort == 0 && bUntaggedPort == 0)
                    WEB_PRINTF(pReq, " checked></td>\n");
                else
                    WEB_PRINTF(pReq, "></td>\n");

                radioId++;
                if (port == portBreakId)
                {
                    radioBreakId = radioId - 1;
                    break;
                }
            }
        }
        else
        {
            for (port = 0; port < 2; ++port)
            {
                if (!(sal_trunk_logicId_exist(port + 2)))
                {
                    continue;
                }

                if (2 == radioIndex)
                {
                    WEB_PRINTF(pReq, "<td><input type=\"radio\" name=\"vlanPort_%d\" id=\"vlanPortRadio_%d_%d\" value=\"%d\" checked></td>\n",
                               (uint16)(port + 2), (uint16)radioId, (uint16)radioIndex, (uint16)radioIndex);
                }
                else
                {
                    WEB_PRINTF(pReq, "<td><input type=\"radio\" name=\"vlanPort_%d\" id=\"vlanPortRadio_%d_%d\" value=\"%d\"></td>\n",
                               (uint16)(port + 2), (uint16)radioId, (uint16)radioIndex, (uint16)radioIndex);
                }
                radioId++;
                if (port == portBreakId)
                {
                    radioBreakId = radioId - 1;
                    break;
                }
            }
        }
        WEB_PRINTF(pReq, "</tr>\n");
    }
    WEB_PRINTF(pReq, "</table>\n");
    
    WEB_PRINTF(pReq, "<button type=\"button\" class=\"btn\" onclick=\"vlanstatic_submit()\">Add / Modify</button>\n");
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "</div>\n");
    
    // VLAN List Card
    WEB_PRINTF(pReq, "<div class=\"card\">\n");
    WEB_PRINTF(pReq, "<h2>VLAN List</h2>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/vlan.cgi?page=getRmvVlanEntry\" name=\"formVlanStatus\">\n");
    WEB_PRINTF(pReq, "<table>\n");
    WEB_PRINTF(pReq, "<tr>\n");
    WEB_PRINTF(pReq, "<th>VLAN ID</th>\n");
    WEB_PRINTF(pReq, "<th>VLAN Name</th>\n");
    WEB_PRINTF(pReq, "<th>Member Ports</th>\n");
    WEB_PRINTF(pReq, "<th>Tagged Ports</th>\n");
    WEB_PRINTF(pReq, "<th>Untagged Ports</th>\n");
    WEB_PRINTF(pReq, "<th>Delete</th>\n");
    WEB_PRINTF(pReq, "</tr>\n");

    /* Print static VLAN entries from flash */
    vlanNum = 0;
    
    /* Initialize seen VLANs tracking */
    seenCount = 0;
    memset(seenVlanIds, 0, sizeof(seenVlanIds));
    
    /* Special handling to always show VLAN 1 */
    bFound = FALSE;
    
    /* First pass - check if VLAN 1 exists in the database */
    for (slot = 0; ((slot < MAX_STATIC_VLAN) && (flashVlanDb.sortedSlot[slot] >= 0)); ++slot)
    {
        sal_flash_vlan_sttcEntry_get(flashVlanDb.sortedSlot[slot], &flashVlanEntry);
        if (flashVlanEntry.vlan_id == 1)
        {
            bFound = TRUE;
            break;
        }
    }
    
    /* Display VLAN 1 first if it exists */
    if (bFound)
    {
        /* Process VLAN 1 */
        sal_trunk_physicMask_toLogicMask(&lportmask, &flashVlanEntry.p_portMask);
        if (SAL_ERR_SUCCESS == sal_lPortMask_isZero(&lportmask))
        {
            strcpy(portmaskStr, "-");
        }
        else
        {
            lmask_to_comma(portmaskStr, &lportmask);
        }

        sal_trunk_physicMask_toLogicMask(&ltagmask, &flashVlanEntry.p_tagMask);
        if (SAL_ERR_SUCCESS == sal_lPortMask_isZero(&ltagmask))
        {
            strcpy(tagMaskStr, "-");
        }
        else
        {
            lmask_to_comma(tagMaskStr, &ltagmask);
        }

        memcpy(&untagmask, &flashVlanEntry.p_portMask, sizeof(sal_pPortmask_t));
        sal_pPortMask_delPorts(&untagmask, &flashVlanEntry.p_tagMask);
        sal_trunk_physicMask_toLogicMask(&luntagMask, &untagmask);

        if (SAL_ERR_SUCCESS == sal_lPortMask_isZero(&luntagMask))
        {
            strcpy(untagMaskStr, "-");
        }
        else
        {
            lmask_to_comma(untagMaskStr, &luntagMask);
        }
				replace_ports(portmaskStr);
				replace_ports(tagMaskStr);
				replace_ports(untagMaskStr);
				
        WEB_PRINTF(pReq, "<tr>\n");

        WEB_PRINTF(pReq, "<td><a href=\"/vlan.cgi?page=getVlanEntry&pickVlanId=%d\">%d</a></td>\n",
                   (uint16)flashVlanEntry.vlan_id, (uint16)flashVlanEntry.vlan_id);
        WEB_PRINTF(pReq, "<td>%s</td>\n", flashVlanEntry.name);
        WEB_PRINTF(pReq, "<td>%s</td>\n", portmaskStr);
        WEB_PRINTF(pReq, "<td>%s</td>\n", tagMaskStr);
        WEB_PRINTF(pReq, "<td>%s</td>\n", untagMaskStr);
        
        WEB_PRINTF(pReq, "<td><input type=\"checkbox\" name=\"remove_%d\" id=\"vlan_%d\" disabled></td>\n",
                (uint16)flashVlanEntry.vlan_id, (uint16)vlanNum);
        
        WEB_PRINTF(pReq, "</tr>\n");
        vlanNum++;
        
        /* Mark VLAN 1 as seen */
        seenVlanIds[seenCount] = 1;
        seenCount++;
    }

    /* Process all other VLANs */
    for (slot = 0; ((slot < MAX_STATIC_VLAN) && (flashVlanDb.sortedSlot[slot] >= 0)); ++slot)
    {
        sal_flash_vlan_sttcEntry_get(flashVlanDb.sortedSlot[slot], &flashVlanEntry);
        
        /* Skip VLAN 0 entries (invalid) */
        if (flashVlanEntry.vlan_id == 0)
            continue;
            
        /* Skip VLAN 1 as we've already processed it */
        if (flashVlanEntry.vlan_id == 1)
            continue;
            
        /* Check if we've already seen this VLAN ID */
        isDuplicate = FALSE;
        for (i = 0; i < seenCount; i++) 
        {
            if (seenVlanIds[i] == flashVlanEntry.vlan_id) 
            {
                isDuplicate = TRUE;
                break;
            }
        }
        
        /* Skip duplicates */
        if (isDuplicate)
            continue;
            
        /* Add to seen list */
        seenVlanIds[seenCount] = flashVlanEntry.vlan_id;
        seenCount++;

        sal_trunk_physicMask_toLogicMask(&lportmask, &flashVlanEntry.p_portMask);
        if (SAL_ERR_SUCCESS == sal_lPortMask_isZero(&lportmask))
        {
            strcpy(portmaskStr, "-");
        }
        else
        {
            lmask_to_comma(portmaskStr, &lportmask);
        }

        sal_trunk_physicMask_toLogicMask(&ltagmask, &flashVlanEntry.p_tagMask);
        if (SAL_ERR_SUCCESS == sal_lPortMask_isZero(&ltagmask))
        {
            strcpy(tagMaskStr, "-");
        }
        else
        {
            lmask_to_comma(tagMaskStr, &ltagmask);
        }

        memcpy(&untagmask, &flashVlanEntry.p_portMask, sizeof(sal_pPortmask_t));
        sal_pPortMask_delPorts(&untagmask, &flashVlanEntry.p_tagMask);
        sal_trunk_physicMask_toLogicMask(&luntagMask, &untagmask);

        if (SAL_ERR_SUCCESS == sal_lPortMask_isZero(&luntagMask))
        {
            strcpy(untagMaskStr, "-");
        }
        else
        {
            lmask_to_comma(untagMaskStr, &luntagMask);
        }
        replace_ports(portmaskStr);
        replace_ports(tagMaskStr);
        replace_ports(untagMaskStr);
            
        WEB_PRINTF(pReq, "<tr>\n");

        WEB_PRINTF(pReq, "<td><a href=\"/vlan.cgi?page=getVlanEntry&pickVlanId=%d\">%d</a></td>\n",
                (uint16)flashVlanEntry.vlan_id, (uint16)flashVlanEntry.vlan_id);
        WEB_PRINTF(pReq, "<td>%s</td>\n", flashVlanEntry.name);
        WEB_PRINTF(pReq, "<td>%s</td>\n", portmaskStr);
        WEB_PRINTF(pReq, "<td>%s</td>\n", tagMaskStr);
        WEB_PRINTF(pReq, "<td>%s</td>\n", untagMaskStr);
        
        if (flashVlanEntry.vlan_id != 1)
        {
            /* All VLANs, except VLAN 1, may be removed */
            WEB_PRINTF(pReq, "<td><input type=\"checkbox\" name=\"remove_%d\" id=\"vlan_%d\"></td>\n",
                       (uint16)flashVlanEntry.vlan_id, (uint16)vlanNum);
        }
        else
        {
            WEB_PRINTF(pReq, "<td><input type=\"checkbox\" name=\"remove_%d\" id=\"vlan_%d\" disabled></td>\n",
                       (uint16)flashVlanEntry.vlan_id, (uint16)vlanNum);
        }
        WEB_PRINTF(pReq, "</tr>\n");
        vlanNum++;
    }
    
    // Display a message if no valid VLANs found
    if (vlanNum == 0) {
        WEB_PRINTF(pReq, "<tr><td colspan=\"6\">No VLANs configured</td></tr>\n");
    }
    WEB_PRINTF(pReq, "</table>\n");
    
    WEB_PRINTF(pReq, "<button type=\"submit\" class=\"btn\" name=\"Delete\">Delete</button>\n");
    WEB_PRINTF(pReq, "<button type=\"button\" class=\"btn\" onclick=\"vlanstatic_selAll(%d)\">Select All</button>\n", (uint16)vlanNum);
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "</div>\n");

    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_vlan_portbased
 * ----------------------------
 * Displays VLAN port-based configuration page.
 * Shows form to set PVID, frame type, and port selection.
 * Displays current port status table with PVID and frame type.
 * Modern compact UI with form styling.
 */
int32 get_vlan_portbased(struct http_request_s *pReq)
{
    sal_vlan_t pvid = 0;
    uint8 port = 0;
    uint8 logicPortStr[MAX_LOGIC_ID_LEN];
    uint8 frameType;
    uint8 frameTypeStr[128];

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html><head><title>VLAN Port</title><style>\n");
    // Ultra-compact CSS
    WEB_PRINTF(pReq, "body{font:14px Arial;background:#f4f7fa;margin:0;padding:15px}");
    WEB_PRINTF(pReq, "h1{text-align:center;margin:0 0 15px}");
    WEB_PRINTF(pReq, ".c{background:#fff;border-radius:5px;box-shadow:0 1px 3px rgba(0,0,0,.1);padding:15px;max-width:700px;margin:0 auto 15px}");
    WEB_PRINTF(pReq, ".r{display:flex;gap:10px}");
    WEB_PRINTF(pReq, ".o{flex:1}");
    WEB_PRINTF(pReq, "label{display:block;font-weight:bold;margin-bottom:3px}");
    WEB_PRINTF(pReq, "input,select{width:100%%;padding:6px;border:1px solid #ddd;border-radius:4px;box-sizing:border-box}");
    WEB_PRINTF(pReq, "select[multiple]{height:120px}");
    WEB_PRINTF(pReq, ".b{background:#1e88e5;color:#fff;border:0;padding:8px 15px;border-radius:4px;cursor:pointer}");
    WEB_PRINTF(pReq, "table{width:100%%;border-collapse:collapse}");
    WEB_PRINTF(pReq, "th{background:#f0f0f0;text-align:center;padding:5px}");
    WEB_PRINTF(pReq, "td{padding:5px;border-bottom:1px solid #eee;text-align:center}");
    WEB_PRINTF(pReq, ".h{font-weight:bold;margin:0 0 10px}");
    WEB_PRINTF(pReq, ".g{margin:0 0 10px}");
    WEB_PRINTF(pReq, "</style></head>\n");
    
    WEB_PRINTF(pReq, "<body><h1>VLAN Port Configuration</h1>\n");
    
    // Port Settings Card
    WEB_PRINTF(pReq, "<div class=\"c\">\n");
    WEB_PRINTF(pReq, "<div class=\"h\">Port Settings</div>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/vlan.cgi?page=port_based\">\n");
    
    WEB_PRINTF(pReq, "<div class=\"r\">\n");
    
    // Port Selection
    WEB_PRINTF(pReq, "<div class=\"o\">\n");
    WEB_PRINTF(pReq, "<label for=\"p\">Select Ports:</label>\n");
    WEB_PRINTF(pReq, "<select id=\"p\" name=\"ports\" multiple size=\"6\">\n");
    for (port = 0; port < 2; ++port)
    {
        if (!(sal_trunk_logicId_exist(port + 2)))
        {
            continue;
        }

        sal_trunk_logicId_toStr(logicPortStr, port);
        WEB_PRINTF(pReq, "<option value=\"%d\">%s</option>\n", (uint16)(port + 2), logicPortStr);
    }
    WEB_PRINTF(pReq, "</select>\n");
    WEB_PRINTF(pReq, "</div>\n");
    
    // Port Settings
    WEB_PRINTF(pReq, "<div class=\"o\">\n");
    
    // PVID
    WEB_PRINTF(pReq, "<div class=\"g\">\n");
    WEB_PRINTF(pReq, "<label for=\"v\">PVID:</label>\n");
    WEB_PRINTF(pReq, "<input type=\"text\" id=\"v\" name=\"pvid\" maxlength=\"4\">\n");
    WEB_PRINTF(pReq, "</div>\n");
    
    // Frame Type
    WEB_PRINTF(pReq, "<div class=\"g\">\n");
    WEB_PRINTF(pReq, "<label for=\"f\">Frame Type:</label>\n");
    WEB_PRINTF(pReq, "<select id=\"f\" name=\"vlan_accept_frame_type\">\n");
    WEB_PRINTF(pReq, "<option value=\"%d\">All</option>\n", (uint16)SAL_ACCEPT_FRAME_TYPE_ALL);
    WEB_PRINTF(pReq, "<option value=\"%d\">Tag-only</option>\n", (uint16)SAL_ACCEPT_FRAME_TYPE_TAG_ONLY);
    WEB_PRINTF(pReq, "<option value=\"%d\">Untag-only</option>\n", (uint16)SAL_ACCEPT_FRAME_TYPE_UNTAG_ONLY);
    WEB_PRINTF(pReq, "</select>\n");
    WEB_PRINTF(pReq, "</div>\n");
    
    WEB_PRINTF(pReq, "</div>\n"); // form-col end
    WEB_PRINTF(pReq, "</div>\n"); // form-row end
    
    // Submit button
    WEB_PRINTF(pReq, "<div style=\"text-align:center;margin-top:15px\">\n");
    WEB_PRINTF(pReq, "<input type=\"submit\" class=\"b\" value=\"Apply\">\n");
    WEB_PRINTF(pReq, "</div>\n");
    
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "</div>\n"); // card end
    
    // Port Status Card
    WEB_PRINTF(pReq, "<div class=\"c\">\n");
    WEB_PRINTF(pReq, "<div class=\"h\">Port Status</div>\n");
    WEB_PRINTF(pReq, "<table>\n");
    WEB_PRINTF(pReq, "<tr>\n");
    WEB_PRINTF(pReq, "<th>Port</th>\n");
    WEB_PRINTF(pReq, "<th>PVID</th>\n");
    WEB_PRINTF(pReq, "<th>Frame Type</th>\n");
    WEB_PRINTF(pReq, "</tr>\n");

    for (port = 0; port < 2; ++port)
    {
        if (!(sal_trunk_logicId_exist(port + 2)))
        {
            continue;
        }

        sal_trunk_logicId_toStr(logicPortStr, port + 2);
        sal_vlan_portPvid_get(port + 2, &pvid);

        sal_vlan_port_AcceptFrameType_get(port + 2, &frameType);
        switch (frameType)
        {
        case SAL_ACCEPT_FRAME_TYPE_ALL:
            strcpy(frameTypeStr, "All");
            break;
        case SAL_ACCEPT_FRAME_TYPE_TAG_ONLY:
            strcpy(frameTypeStr, "Tag-only");
            break;
        case SAL_ACCEPT_FRAME_TYPE_UNTAG_ONLY:
            strcpy(frameTypeStr, "Untag-only");
            break;
        default:
            strcpy(frameTypeStr, "Unknown Mode");
            break;
        }
        WEB_PRINTF(pReq, "<tr>\n");
        WEB_PRINTF(pReq, "<td>%d</td>\n", (int16)(port + 1));
        WEB_PRINTF(pReq, "<td>%d</td>\n", (int16)pvid);
        WEB_PRINTF(pReq, "<td>%s</td>\n", frameTypeStr);
        WEB_PRINTF(pReq, "</tr>\n");
    }

    WEB_PRINTF(pReq, "</table>\n");
    WEB_PRINTF(pReq, "</div>\n"); // card end

    WEB_PRINTF(pReq, "</body></html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_vlan_ip
 * ---------------------
 * Displays VLAN IP configuration page.
 * Shows form to set VLAN ID, IP, subnet mask, gateway.
 * Displays current VLAN IP status table.
 * Reads VLAN ID from flash and shows current configuration.
 */
int32 get_vlan_ip(struct http_request_s *pReq)
{
    int i = 0;
	  uint8   get_subnetmask[16];
		uint8   getvlan_id[5];
	  u8_t newdevice =0;
	  sal_ipaddr_t    gateway = 0;
		sal_ipaddr_t    ip = 0;
    sal_ipaddr_t    netmask = 0;
	
		if(sal_flash_vlan_id_get(FLSH_CONFIG_DUMMY_START , 4 ,getvlan_id) == SAL_ERR_SUCCESS)
		{
			printf("Read VLAN ID inside flash Memory : %s\r\n" , getvlan_id);
			getvlan_id[4] = '\0';  // Null-terminate
			printf("Vlan ID : %s\r\n",getvlan_id);
		}
		
		sal_sys_ip_get(&ip, &netmask);
    ip_to_str(ipStr, (uint8 *)&ip);
		
		printf("IP address from get function: %s",ipStr);

    sal_sys_gateway_get(&gateway);
        
    ip_to_str(gatewayStr, (uint8 *)&gateway);
		ip_to_str(get_subnetmask, (uint8 *)&netmask);

    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html><head><title>VLAN IP</title><style>\n");
    WEB_PRINTF(pReq, "body{font:14px Arial;padding:15px}");
    WEB_PRINTF(pReq, "table{border-collapse:collapse;width:100%%}");
    WEB_PRINTF(pReq, "th,td{border:1px solid #ddd;padding:8px;text-align:left}");
    WEB_PRINTF(pReq, "th{background:#f2f2f2}");
    WEB_PRINTF(pReq, "input{padding:5px;margin:5px}");
    WEB_PRINTF(pReq, "</style></head>\n");
    
    WEB_PRINTF(pReq, "<body><h2>VLAN IP Configuration</h2>\n");
    
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/vlan.cgi?page=vlan_ip\">\n");
    WEB_PRINTF(pReq, "<p>VLAN ID: <input type=\"text\" name=\"vlanId\" size=\"10\"></p>\n");
    WEB_PRINTF(pReq, "<p>IP Address: <input type=\"text\" name=\"ipAddress\" size=\"15\"></p>\n");
    WEB_PRINTF(pReq, "<p>Subnet Mask: <input type=\"text\" name=\"subnetMask\" size=\"15\"></p>\n");
    WEB_PRINTF(pReq, "<p>Gateway: <input type=\"text\" name=\"gateway\" size=\"15\"></p>\n");
    WEB_PRINTF(pReq, "<p><input type=\"submit\" value=\"Apply\"></p>\n");
    WEB_PRINTF(pReq, "</form>\n");
    
    WEB_PRINTF(pReq, "<h3>Current Status</h3>\n");
    WEB_PRINTF(pReq, "<table>\n");
    WEB_PRINTF(pReq, "<tr><th>VLAN ID</th><th>IP Address</th><th>Subnet Mask</th><th>Gateway</th></tr>\n");

    if(strncmp(getvlan_id,"NEW ",4)!=0)
    {
        WEB_PRINTF(pReq, "<tr><td>%d</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", 
                   vlanIds, ipStr, get_subnetmask, gatewayStr);
    }
    else
    {
        WEB_PRINTF(pReq, "<tr><td colspan=\"4\">No VLAN IP configurations</td></tr>\n");
    }

    WEB_PRINTF(pReq, "</table>\n");
    
    WEB_PRINTF(pReq, "</body></html>\n");

    WEB_END(pReq);
    return SUCCESS;
}

extern void setReg(uint16, uint16);

///*
// * Function: post_vlan_static
// * --------------------------
// * Processes static VLAN add/modify form submission.
// * Extracts VLAN ID, name, and port membership from POST.
// * Handles tagged/untagged port selection via radio buttons.
// * Calls sal_vlan_sttcEntry_add() to create/update VLAN.
// */
int32 post_vlan_static(struct http_request_s *pReq)
{
    sal_lPortmask_t portmask;
    sal_lPortmask_t tagmask;
    sal_vlan_t vid = 0;
    int32 ret;
    int iValue;
    uint8 vlanNameStr[MAX_VLAN_NAME_LEN + 1];
    uint8 portIdStr[32];
    uint8 port = 0;
    uint8 radioValue = 0;
    uint8 *pValue;

    sal_lPortMask_delAll(&portmask);
    sal_lPortMask_delAll(&tagmask);

    WEB_VAR(pReq, "name", &pValue);
    if (pValue)
    {
        strcpy(vlanNameStr, pValue);
    }

    WEB_VAR(pReq, "vid", &pValue);
    if (pValue)
    {
        iValue = atoi(pValue);
        if ((iValue < 1) || (iValue > 4094))
        {
            printerr(pReq, SAL_ERR_VLAN_ID_INVALID);
            return SUCCESS;
        }
        vid = iValue;
    }

    for (port = 0; port < 2; ++port)
    {
        sprintf(portIdStr, "vlanPort_%d", (uint16)(port + 2));
        WEB_VAR(pReq, portIdStr, &pValue);

        if (pValue)
        {
            radioValue = atoi(pValue);
            if (radioValue == 0 || radioValue == 1) /*member port list*/
                sal_lPortMask_addPort(&portmask, port + 2);
            if (radioValue == 1) /*tagged port list*/
                sal_lPortMask_addPort(&tagmask, port + 2);
        }
    }

    if (vid == 1)
    {
        /* vid 1 should be called Default_VLAN*/
        // memcpy(vlanNameStr,"Default_VLAN",sizeof("Default_VLAN"));
        memset(vlanNameStr, 0, sizeof(vlanNameStr));
    }

    if ((ret = sal_vlan_sttcEntry_add(vid, vlanNameStr, &portmask, &tagmask)) != SAL_ERR_SUCCESS)
    {
        printerr(pReq, ret);
        return SUCCESS;
    }

    get_vlan_static(pReq, 0);
    return SUCCESS;
}

/*
 * Function: post_vlanstaticDelEntry
 * ---------------------------------
 * Processes VLAN deletion from form submission.
 * Scans request for "remove_X" checkbox names.
 * Extracts VLAN ID from checkbox name and deletes entry.
 * Handles both single and multiple VLAN deletions.
 */
int32 post_vlanstaticDelEntry(struct http_request_s *pReq)
{
    int32 ret;
    uint16 vid = 0;
    uint16 i;
    uint8 *vlanCharStart, *vlanCharEnd;
    uint8 *pValue;
    uint8 removeStr[32], *vlanChar;

    sprintf(removeStr, "remove_");
    pReq->Var_Temp = 0;

    /* Search Variables */
    for (i = pReq->Var_Temp; i < pReq->Var_Count; i++)
    {
        if (0 == strncmp(removeStr, (pReq->pMethod + pReq->Var[i].name_offset), strlen(removeStr)))
        {
            vid = 0;

            vlanCharStart = (pReq->pMethod + pReq->Var[i].name_offset) + strlen(removeStr) + 0;
            vlanCharEnd = (pReq->pMethod + pReq->Var[i].name_offset) + strlen(removeStr) + 3;
            for (vlanChar = vlanCharStart; vlanChar <= vlanCharEnd; vlanChar++)
            {
                if (*vlanChar >= '0' && *vlanChar <= '9')
                    vid = vid * 10 + (*vlanChar - '0');
            }

            if ((ret = sal_vlan_sttcEntry_remove(vid)) != SAL_ERR_SUCCESS)
            {
                printerr(pReq, ret);
                return SUCCESS;
            }
        }
    }

    WEB_VAR(pReq, "pickVlanId", &pValue);
    if (pValue)
    {
        vid = atoi(pValue);
        if ((vid < 1) || (vid > 4094))
        {
            printerr(pReq, SAL_ERR_VLAN_ID_INVALID);
            return SUCCESS;
        }

        get_vlan_static(pReq, vid);
        return 2;
    }

    get_vlan_static(pReq, 0);
    return SUCCESS;
}

/*
 * Function: post_vlan_portbased
 * -----------------------------
 * Processes VLAN port-based configuration form submission.
 * Extracts PVID, frame type, and selected ports from POST.
 * Applies PVID, tag mode, and frame type to each selected port.
 * Refreshes port-based VLAN page after applying.
 */
int32 post_vlan_portbased(struct http_request_s *pReq)
{
    sal_lPortmask_t portmask;
    int32 ret;
    uint16 vid = 0;
    uint8 port = 0;
    uint8 mode = SAL_VLAN_TAG_MODE_ORIGINAL, frameType = SAL_ACCEPT_FRAME_TYPE_ALL;
    uint8 *pValue;

    sal_lPortMask_delAll(&portmask);

    WEB_VAR(pReq, "pvid", &pValue);
    if (pValue)
    {
        vid = atoi(pValue);
        if ((vid < 1) || (vid > 4094))
        {
            printerr(pReq, SAL_ERR_VLAN_ID_INVALID);
            return SUCCESS;
        }
    }

    WEB_VAR(pReq, "vlan_accept_frame_type", &pValue);
    if (pValue)
    {
        frameType = atoi(pValue);
        if (frameType != SAL_ACCEPT_FRAME_TYPE_ALL && frameType != SAL_ACCEPT_FRAME_TYPE_TAG_ONLY && frameType != SAL_ACCEPT_FRAME_TYPE_UNTAG_ONLY)
        {
            printerr(pReq, SAL_ERR_PARAMETER);
            return SUCCESS;
        }
    }

    WEB_VAR(pReq, "ports", &pValue);
    if (pValue)
    {
        sal_lPortMask_addPort(&portmask, atoi(pValue));
    }

    while (pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue)
        {
            sal_lPortMask_addPort(&portmask, atoi(pValue));
        }
    }

    for (port = 0; port < 2; ++port)
    {
        if (!(sal_trunk_logicId_exist(port + 2)))
        {
            continue;
        }

        if (SAL_ERR_SUCCESS == sal_lPortMask_chkPort(&portmask, port + 2))
        {
            if ((ret = sal_vlan_portPvid_set(port + 2, vid)) != SAL_ERR_SUCCESS)
            {
                printerr(pReq, ret);
                return SUCCESS;
            }

            if ((ret = sal_vlan_tagMode_set(port + 2, mode)) != SAL_ERR_SUCCESS)
            {
                printerr(pReq, ret);
                return SUCCESS;
            }

            if ((ret = sal_vlan_port_AcceptFrameType_set(port + 2, frameType)) != SAL_ERR_SUCCESS)
            {
                printerr(pReq, ret);
                return SUCCESS;
            }
        }
    }

    get_vlan_portbased(pReq);
    return SUCCESS;
}

/*
 * Function: post_vlan_ip
 * ----------------------
 * Processes VLAN IP configuration form submission.
 * Extracts VLAN ID, IP, subnet mask, gateway from POST.
 * Validates all fields (IP format, subnet mask, same subnet).
 * Saves VLAN ID to flash and configures switch IP/gateway.
 * Creates ACL rules for IP and gateway traffic to CPU.
 */
int32 post_vlan_ip(struct http_request_s *pReq)
{
    uint8   *pValue;
    uint32  ip = 0;
    uint32  gateway_ip = 0;
    uint32  subnet_mask = 0;  
    uint8   subnetMaskStr[16];
	 // uint8   vlan_Id[4];
    uint16    ip_rule_index = ruleCount;
    uint16    gateway_rule_index = ruleCount + 2;
	  uint32  temp = subnet_mask;
    uint32  inverted = ~temp;
	  uint8 xdata buf[4];

    // Get and validate VLAN ID
    if (WEB_VAR(pReq, "vlanId", &pValue) == SUCCESS && pValue) {
      vlanIds = (u16_t)atoi((char *)pValue);
			//sprintf(vlan_Id,"%04d",vlanIds);
//			if(sal_flash_vlan_id_set(FLSH_CONFIG_DUMMY_START , 4 ,pValue) == SAL_ERR_SUCCESS)
//				{
//					printf("Write VLAN ID inside flash Memory : %s\r\n" , pValue);
//				}
				

    /* read device type, fw version and hw version */    
    FLASH_READ((uint32)(FLSH_CONFIG_DUMMY_START), buf, sizeof(buf));
    
    /* keep device type, hw version and replace fw version*/
    memset(&buf[0], 0, sizeof(buf));
    memcpy(&buf[0], pValue, 4);
    
    FLASH_ERASE_SECTOR(FLSH_CONFIG_DUMMY_START);
    FLASH_WRITE((uint32)FLSH_CONFIG_DUMMY_START, buf, 4);
			
        if(vlanIds < 1 || vlanIds > 4094) {
            printmsg(pReq, "Invalid VLAN ID (1-4094)");
            return SUCCESS;
        }
    }

    // Get and validate IP Address
    memset(ipStr, 0, sizeof(ipStr));
    WEB_VAR(pReq, "ipAddress", &pValue);
    if (pValue) {
      memcpy(ipStr, pValue, sizeof(ipStr));
//			if(sal_flash_userData_set(FLSH_VLAN_IP_ADDRESS , 16 ,ipStr) == SAL_ERR_SUCCESS)
//				{
//					printf("Write VLAN IP inside flash Memory : %s\r\n" , ipStr);
//				}
        if (str_to_ip(&ip, ipStr) != 0) {
            printmsg(pReq, "Invalid IP Address format");
            return SUCCESS;
        }
    }

    // Get and validate Gateway
    memset(gatewayStr, 0, sizeof(gatewayStr));
    WEB_VAR(pReq, "gateway", &pValue);
    if (pValue) {
      memcpy(gatewayStr, pValue, sizeof(gatewayStr));
//			if(sal_flash_userData_set(FLSH_VLAN_GATEWAY_ADDRESS , 16 ,gatewayStr) == SAL_ERR_SUCCESS)
//				{
//					printf("Write VLAN Gateway inside flash Memory : %s\r\n" , gatewayStr);
//				}
        if (str_to_ip(&gateway_ip, gatewayStr) != 0) {
            printmsg(pReq, "Invalid Gateway format");
            return SUCCESS;
        }
    }
     
    // Get and validate Subnet Mask
    memset(subnetMaskStr, 0, sizeof(subnetMaskStr));
    WEB_VAR(pReq, "subnetMask", &pValue);
    if (pValue) {
        memcpy(subnetMaskStr, pValue, sizeof(subnetMaskStr));
        if (str_to_ip(&subnet_mask, subnetMaskStr) != 0) {
            printmsg(pReq, "Invalid Subnet Mask format");
            return SUCCESS;
        }
        
        // Validate subnet mask (must be a valid netmask)
        
        if ((inverted & (inverted + 1)) != 0) {
            printmsg(pReq, "Invalid Subnet Mask. Must be a valid netmask (e.g., 255.255.255.0)");
            return SUCCESS;
        }
        
//        // Store subnet mask in flash
//        if(sal_flash_userData_set(FLSH_VLAN_SUBNET_ADDRESS, 16, subnetMaskStr) == SAL_ERR_SUCCESS) {
//            printf("Write SubnetMask inside flash Memory: %s\r\n", subnetMaskStr);
//        }
    } else {
        printmsg(pReq, "Subnet Mask is required");
        return SUCCESS;
    }

    // Validate that IP and Gateway are in the same subnet (if gateway is provided)
    if (gateway_ip != 0 && subnet_mask != 0) {
        if ((gateway_ip & subnet_mask) != (ip & subnet_mask)) {
            printmsg(pReq, "IP Address and Gateway must be in the same subnet");
            return SUCCESS;
        }
    }

    // Validate that IP is not a network or broadcast address
    if (subnet_mask != 0) {
        uint32 network_addr = ip & subnet_mask;
        uint32 broadcast_addr = network_addr | (~subnet_mask);
        
        if (ip == network_addr) {
            printmsg(pReq, "IP Address cannot be the network address");
            return SUCCESS;
        }
        
        if (ip == broadcast_addr) {
            printmsg(pReq, "IP Address cannot be the broadcast address");
            return SUCCESS;
        }
    }

    // Configure Gateway
    if(sal_sys_gateway_set(gateway_ip) != RT_ERR_OK) {
        printmsg(pReq, "Gateway configuration failed");
        return SUCCESS;
    }

    // Configure Switch IP
    if(sal_sys_ip_set(ip, subnet_mask) != RT_ERR_OK) {
        printmsg(pReq, "Switch IP configuration failed");
        return SUCCESS;
    }

    // Add IP-based ACL rule
    if(add_ip_over_vlan_acl_rule(ip_rule_index, vlanIds, ip, subnet_mask) != RT_ERR_OK) {
			printmsg(pReq, "IP ACL rule creation failed");
			printf("IP ACL rule creation failed : %u <--> 0X%02x <--> %ld\r\n",ip_rule_index,ip_rule_index,ip_rule_index);
			printf("Gateway ACL rule creation failed : %u <--> 0X%02x <-->%ld\r\n",gateway_rule_index,gateway_rule_index,gateway_rule_index);
       // vlanCount = 0;
        return SUCCESS;
    }

    // Add Gateway-based ACL rule
    if(add_gateway_over_vlan_acl_rule(gateway_rule_index, vlanIds, gateway_ip, subnet_mask) != RT_ERR_OK) {
			printmsg(pReq, "Gateway ACL rule creation failed");
			//printf("Gateway ACL rule creation failed : %u <--> 0X%02x \r\n",gateway_rule_index,gateway_rule_index);
       // vlanCount = 0;
        return SUCCESS;
    }
		
    ruleCount += 4;  // Increment for next use
    //vlanCount = 1;   // Successfully configured

    get_vlan_ip(pReq);
    return SUCCESS;
}


