/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : The handler of HTTP request to MAC function
 *
 * Feature : Response the request related to MAC function
 *
 */
#pragma STRING (far)

/*
 * Include Files
 */
#include <string.h>
#include <stdio.h>

#include <salflash.h>
#include <salutil.h>
#include <saltrunk.h>
#include <salflash.h>
#include <sall2.h>
#include <salsys.h>

#include <web/web_api.h>
#include "web_mac.h"
#include "web_util.h"

/*
 * Symbol Definition
 */
#define SET_GET_FLASH TRUE
#define ENTRY_STATIC             (1)

/*
 * Data Declaration
 */



/*
 * Function Declaration
 */

/*
 * Function: get_mac
 * -----------------
 * Placeholder handler for MAC page.
 * Currently displays simple text response.
 * Can be extended for main MAC configuration.
 */
int32 get_mac(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "This is mac.cgi");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_mac_forwardingtable
 * ---------------------------------
 * Displays MAC address forwarding table from SRAM.
 * Reads all SRAM entries and filters CPU port entries.
 * Shows MAC, VLAN ID, type (Static/Dynamic), and port mapping.
 * Includes button to clear dynamic entries.
 */
int32 get_mac_forwardingtable(struct http_request_s *pReq)
{
    uint32              indx = 0;
    sal_l2_type_t       type;
    uint32              entryNum = 1;
    sal_mac_t           mac;
    uint8               macStr[128];
    sal_vlan_t          fid;
    sal_lPortmask_t     lportmask;
    sal_pPortmask_t     portmask; 
    uint8               portmaskStr[128];
    uint8               bStatic;
    uint8               staticStr[128];
    
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>MAC Address Information</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>MAC Address Information</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/mac.cgi?page=fwd_tbl\">\n");
    WEB_PRINTF(pReq, "<br>\n");
    WEB_PRINTF(pReq, "<table border=\"1\">\n");
    WEB_PRINTF(pReq, "  <tr>\n");
    WEB_PRINTF(pReq, "    <th width=\"35\">No.</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"185\">MAC Address</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"80\">VLAN ID</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"165\">Type</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"190\">Port</th>\n");
    WEB_PRINTF(pReq, "  </tr>\n");
    
    for (indx = 0; indx < MAX_MAC_SRAM_ENTRY; indx++)
    {
        if(SUCCESS != sal_l2_sramEntry_get(indx, &mac, &fid, &bStatic, &type, &portmask))
            continue;

        if (SAL_L2_UCAST != type)
        {
            continue;
        }

        /* Exclude MAC Addresses learnt by CPU PORT */
        if(portmask.bits[0] & (1 << SAL_PORT_ID_CPU))
        {
            continue;
        }

        mac_to_str(macStr, mac.octet);

        if (1 == bStatic)
            strcpy(staticStr,  "Static");
        else
            strcpy(staticStr,  "Dynamic");

        sal_lPortMask_delAll(&lportmask);

        sal_lPortMask_copyUL(&lportmask, &portmask.bits[0]);

        strcpy(portmaskStr, "");

        lmask_to_comma(portmaskStr, &lportmask);

        WEB_PRINTF(pReq, "  <tr>\n");
        WEB_PRINTF(pReq, "    <td align=\"center\">%d</td>\n", (uint16)entryNum);
        WEB_PRINTF(pReq, "    <td align=\"center\">%s</td>\n", macStr);
        WEB_PRINTF(pReq, "    <td align=\"center\">%d</td>\n", (uint16)fid);
        WEB_PRINTF(pReq, "    <td align=\"center\">%s</td>\n", staticStr);
        WEB_PRINTF(pReq, "    <td align=\"center\">%s</td>\n", portmaskStr);
        WEB_PRINTF(pReq, "  </tr>\n");

        entryNum++;
    }

    WEB_PRINTF(pReq, "</table>\n");

    WEB_PRINTF(pReq, "<br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\"Clear Dynamic Entries\">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"mactblclr\">\n");
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "\n");

    WEB_PRINTF(pReq, "</center>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

extern void sal_mac_dbg_dump(void);

/*
 * Function: get_mac_search
 * ------------------------
 * Displays MAC address search page with input form.
 * Allows user to enter MAC address and VLAN ID for search.
 * Provides search button to find specific MAC entry.
 * Results are displayed on separate result page.
 */
int32 get_mac_search(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>MAC Addresses Searching</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");

    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>MAC Addresses Searching</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/mac.cgi?page=search\">\n");

    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" width=\"150\">MAC Address</th>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" width=\"90\">VLAN ID</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\"><input type=\"text\" name=\"mac\" value=\"00:00:00:00:00:00\"></td>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\"><input type=\"text\" size=\"5\" name=\"vlan\"> (1~4094)</td>\n");
    WEB_PRINTF(pReq, "    </tr>\n");
    WEB_PRINTF(pReq, "  </table>\n");

    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\" Search \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"macsearch\">\n");
    WEB_PRINTF(pReq, "</form>\n");

    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "<p>\n");
    WEB_PRINTF(pReq, "\n");

    WEB_PRINTF(pReq, "</center>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: _get_mac_search_res
 * -----------------------------
 * Displays MAC search result page with found entry details.
 * Shows MAC address, VLAN ID, type, and port information.
 * Reuses the search form for additional searches.
 * Helper function called from post_mac_search.
 */
int32 _get_mac_search_res(struct http_request_s *pReq, sal_l2_ucastAddr_t* pSearch_res)
{
    uint8       macStr[128];
    uint8       staticStr[128];
    uint8       logicPortStr[MAX_LOGIC_ID_LEN];
    sal_port_t  port;
            
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>MAC Addresses Searching</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");

    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>MAC Addresses Searching</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/mac.cgi?page=search\">\n");

    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" width=\"150\">MAC Address</th>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" width=\"90\">VLAN ID</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\"><input type=\"text\" name=\"mac\" value=\"00:00:00:00:00:00\"></td>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\"><input type=\"text\" size=\"5\" name=\"vlan\"> (1~4094)</td>\n");
    WEB_PRINTF(pReq, "    </tr>\n");
    WEB_PRINTF(pReq, "  </table>\n");

    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\" Add \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"macsearch\">\n");
    WEB_PRINTF(pReq, "</form>\n");

    WEB_PRINTF(pReq, "<hr>\n");

    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/mac.cgi?page=staticdel\">\n");
    WEB_PRINTF(pReq, "<table border=\"1\">\n");
    WEB_PRINTF(pReq, "  <tr>\n");
    WEB_PRINTF(pReq, "    <th width=\"185\">MAC Address</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"80\">VLAN ID</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"165\">Type</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"190\">Port</th>\n");
    WEB_PRINTF(pReq, "  </tr>\n");
    
    mac_to_str(macStr, pSearch_res->mac.octet);

    if (TRUE == pSearch_res->isStatic)
        strcpy(staticStr,  "Static");
    else
        strcpy(staticStr,  "Dynamic");

    sal_trunk_physicId_toLogicId(&port, pSearch_res->srcPort);
    sal_trunk_logicId_toStr(logicPortStr, port);

    WEB_PRINTF(pReq, "  <tr>\n");
    WEB_PRINTF(pReq, "    <td align=\"center\">%s</td>\n", macStr);
    WEB_PRINTF(pReq, "    <td align=\"center\">%d</td>\n", (uint16)pSearch_res->vid);
    WEB_PRINTF(pReq, "    <td align=\"center\">%s</td>\n", staticStr);
    WEB_PRINTF(pReq, "    <td align=\"center\">%s</td>\n", logicPortStr);
    WEB_PRINTF(pReq, "  </tr>\n");
    
    WEB_PRINTF(pReq, "</table>\n");
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "<p>\n");
    WEB_PRINTF(pReq, "\n");

    WEB_PRINTF(pReq, "</center>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_mac_staticmac
 * ---------------------------
 * Displays static MAC address configuration page.
 * Shows form to add static MAC with VLAN and port selection.
 * Lists existing static MAC entries with delete checkboxes.
 * Supports source MAC blocking checkbox for security.
 */
int32 get_mac_staticmac(struct http_request_s *pReq)
{
    sal_flash_l2_sttcEntry_t    entry;
    sal_lPortmask_t             portmask;   
    uint32  indx = 0;
    uint32  entryNum = 1;
    uint16  port;
    uint16  fid;
    uint8   macStr[128];
    uint8   portmaskStr[128];
    uint8   logicPortStr[MAX_LOGIC_ID_LEN];
    uint8   emptyMac[ETHER_ADDR_LEN];

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>Static MAC Addresses</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");

    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>Static MAC Setting</legend>\n");
    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/mac.cgi?page=static\">\n");

    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" width=\"150\">MAC Address</th>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" width=\"90\">VLAN ID</th>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" width=\"90\">Port</th>\n");
    WEB_PRINTF(pReq, "      <th align=\"center\" width=\"90\">Source MAC Blocking</th>\n");    
    WEB_PRINTF(pReq, "    </tr>\n");

    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\"><input type=\"text\" name=\"mac\" value=\"00:00:00:00:00:00\"></td>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\"><input type=\"text\" size=\"5\" name=\"vlan\"> (1~4094)</td>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\">\n");
    WEB_PRINTF(pReq, "        <select name=\"src\" size=\"6\">\n");
    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {
        if (!(sal_trunk_logicId_exist(port)))
        {
            continue;
        }

        sal_trunk_logicId_toStr(logicPortStr, port);
        WEB_PRINTF(pReq, "          <option value=\"%d\">%s\n", (uint16)port, logicPortStr);
    }

    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      <td align=\"center\"><input type=\"checkbox\" name=\"src_blk\"></td>\n");
    WEB_PRINTF(pReq, "      </td>\n");
    WEB_PRINTF(pReq, "    </tr>\n");
    WEB_PRINTF(pReq, "  </table>\n");

    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\" Add \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"macstatic\">\n");
    WEB_PRINTF(pReq, "</form>\n");

    WEB_PRINTF(pReq, "<hr>\n");

    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/mac.cgi?page=staticdel\">\n");
    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "    <th width=\"35\">No.</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"190\">MAC Address</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"80\">VLAN ID</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"190\">Port</th>\n");
    WEB_PRINTF(pReq, "    <th width=\"80\">Source MAC Blocking</th>\n");    
    WEB_PRINTF(pReq, "    <th width=\"2\">Select</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    memset(emptyMac, 0, ETHER_ADDR_LEN);

    for (indx = 0; indx < SAL_MAX_STATIC_MAC; indx++)
    {

        if (SAL_ERR_SUCCESS != sal_flash_l2_sttcEntry_get((uint16) indx, &entry))
        {
            continue;
        }

        if (memcmp(entry.mac.octet, emptyMac, ETHER_ADDR_LEN) == 0)
        {
            continue;
        }

        fid = entry.fid;

        mac_to_str(macStr, entry.mac.octet);

//        printf("Physical portmask: %lx\n", entry.pPortMask.bits[0]);
        strcpy(portmaskStr, "");        
        sal_trunk_physicMask_toLogicMask(&portmask, &entry.pPortMask);
//        printf("Logical portmask : %lx\n", portmask.bits[0]);
        lmask_to_comma(portmaskStr, &portmask);

        WEB_PRINTF(pReq, "  <tr>\n");
        WEB_PRINTF(pReq, "      <td align=\"center\">%d</td>\n", (uint16)entryNum);
        WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", macStr);
        WEB_PRINTF(pReq, "      <td align=\"center\">%d</td>\n", (uint16)fid);
        WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", portmaskStr);
        WEB_PRINTF(pReq, "      <td align=\"center\">%s</td>\n", entry.saBlock?"Blocked":"-");        
        WEB_PRINTF(pReq, "      <td align=\"center\"><input type=\"checkbox\" name=\"del\" value=\"%s_%d\"></td>\n", macStr, (uint16)fid);
        WEB_PRINTF(pReq, "  </tr>\n");

        entryNum++;
    }

    WEB_PRINTF(pReq, "  </table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" value=\" Delete \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"macstatictbl\">\n");
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "<p>\n");
    WEB_PRINTF(pReq, "\n");

    WEB_PRINTF(pReq, "</center>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: post_mac_del
 * ----------------------
 * Processes static MAC entry deletion from form submission.
 * Parses checkbox values containing "MAC_FID" format.
 * Deletes each selected MAC entry from hardware and flash.
 * Refreshes static MAC page after deletion.
 */
int32
post_mac_del(struct http_request_s *pReq)
{
    sal_mac_t   mac;
    int32       ret;
    uint16      fid = 0;
    uint8       macStr[128], tmp[64];
    uint8       *pValue;

    WEB_VAR(pReq, "del", &pValue);
    if (pValue) {
        strcpy(tmp, "");
        sprintf(tmp, "%s", pValue);
        strcpy(macStr, strtok (pValue, "_"));
        fid = atoi(strstr(tmp, "_") + 1);
        str_to_mac(mac.octet, macStr, 0);
        if ((ret = sal_l2_entry_del(&mac, fid)) != SAL_ERR_SUCCESS)
        {
            printerr(pReq, ret);
            return SUCCESS;
        }
#if SET_GET_FLASH
        sal_flash_l2_sttcEntry_del(&mac, fid);
#endif
    }        
           
    while(pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue) {
            strcpy(tmp, "");
            sprintf(tmp, "%s", pValue);
            strcpy(macStr, strtok (pValue, "_"));
            fid = atoi(strstr(tmp, "_") + 1);
            str_to_mac(mac.octet, macStr, 0);
            if ((ret = sal_l2_entry_del(&mac, fid)) != SAL_ERR_SUCCESS)
            {
                printerr(pReq, ret);
                return SUCCESS;
            }
#if SET_GET_FLASH
            sal_flash_l2_sttcEntry_del(&mac, fid);
#endif
        }
    }

    get_mac_staticmac(pReq);


    sal_mac_dbg_dump();
    return SUCCESS;
}

/*
 * Function: post_mac_add
 * ----------------------
 * Processes static MAC entry addition from form submission.
 * Validates MAC format, VLAN ID range (1-4094), and port selection.
 * Checks for multicast MAC (rejected) and duplicate entries.
 * Adds entry to hardware and flash, handles slot allocation.
 */
int32
post_mac_add(struct http_request_s *pReq)
{
    sal_pPortmask_t portmask;
    sal_port_t      port;
    sal_mac_t       mac;
    uint16          cvid = 0;
    uint8           macStr[ETHER_ADDR_LEN], src_blk;
    uint8           *pValue;
    

#if SET_GET_FLASH
    uint32         slot = 0;
    sal_flash_l2_sttcEntry_t Entry;
#endif

#if SET_GET_FLASH
    memset(&Entry, 0, sizeof(sal_flash_l2_sttcEntry_t));
#endif


    WEB_VAR(pReq, "mac", &pValue);
    if (pValue) {
        if (str_to_mac(macStr, pValue, 0) != 0)
        {
            printmsg(pReq, "Invalid MAC Address !!");
            return SUCCESS;
        }

        if (macStr[0] & 0x01)
        {
            printmsg(pReq, "Multicast MAC is not supported !!");
            return SUCCESS;
        }
    }

    WEB_VAR(pReq, "vlan", &pValue);
    if (pValue) {
        cvid = atoi(pValue);
        if ((cvid < 1) || (cvid > 4094))
        {
            printmsg(pReq, "Invalid Vlan ID !!");
            return SUCCESS;
        }
    }

    if (WEB_VAR(pReq, "src", &pValue) == FAILED)
    {
        printmsg(pReq, "Invalid port number !!");
        return SUCCESS;
    }

    if (pValue) {
        port = atoi(pValue);
        sal_trunk_logicId_toPhysicMask(&portmask, port);
    }

    WEB_VAR(pReq, "src_blk", &pValue);
    if(pValue)
        src_blk = TRUE;
    else
        src_blk = FALSE;


#if SET_GET_FLASH

    if (SAL_ERR_SUCCESS == sal_flash_l2_matchedSlot_get(&slot, (sal_mac_t *)macStr, cvid))
    {
        memcpy(mac.octet, macStr, ETHER_ADDR_LEN);
        sal_flash_l2_sttcEntry_del(&mac, cvid);
    }
    else // not found a matched entry
    {
        if (SAL_ERR_SUCCESS != sal_flash_l2_firstAvailableSlot_get(&slot))
        {
            printmsg(pReq, "MAC Table is full !!");
            return SUCCESS;
        }
    }

    memcpy(Entry.mac.octet, macStr, ETHER_ADDR_LEN);
    Entry.fid       = cvid;
    Entry.auth      = FALSE;
    Entry.saBlock   = src_blk;
    Entry.daBlock   = FALSE;
    Entry.valid     = 1;
    sal_trunk_logicId_toPhysicMask(&Entry.pPortMask, port);

    memcpy(mac.octet, macStr, ETHER_ADDR_LEN);
    
    if (SAL_ERR_SUCCESS == sal_l2_entry_add(
                                &Entry.mac,
                                cvid,
                                ENTRY_STATIC, Entry.auth, Entry.saBlock, Entry.daBlock,
                                &Entry.pPortMask))
    {
        sal_flash_l2_sttcEntry_add(slot, &Entry);
    }
    else
    {
        printmsg(pReq, "Could not add the MAC entry !!");
        return SUCCESS;
    }

#else
    memcpy(mac.octet, macStr, ETHER_ADDR_LEN);
    sal_l2_entry_add(&mac, cvid, TRUE, FALSE, FALSE, FALSE, &portmask);
#endif


    get_mac_staticmac(pReq);

    sal_mac_dbg_dump();
    return SUCCESS;
}

/*
 * Function: post_mac_search
 * -------------------------
 * Processes MAC address search request from form submission.
 * Validates MAC format and VLAN ID range.
 * Calls sal_l2_addr_get() to search for the MAC entry.
 * Displays search result via _get_mac_search_res() helper.
 */
int32 post_mac_search(struct http_request_s *pReq)
{
    sal_l2_ucastAddr_t  search_res;
    sal_mac_t           mac;
    sal_vlan_t          fid;

    int32       ret; 
    uint8       *pValue;
    uint8       macStr[ETHER_ADDR_LEN];

    WEB_VAR(pReq, "mac", &pValue);
    if (pValue) {
        if (str_to_mac(macStr, pValue, 0) != 0)
        {
            printmsg(pReq, "Invalid MAC Address !!");
            return SUCCESS;
        }

        if (macStr[0] & 0x01)
        {
            printmsg(pReq, "Multicast MAC is not supported !!");
            return SUCCESS;
        }
    }

    WEB_VAR(pReq, "vlan", &pValue);
    if (pValue) {
        fid = atoi(pValue);
        if ((fid < 1) || (fid > 4094))
        {
            printmsg(pReq, "Invalid Vlan ID !!");
            return SUCCESS;
        }
    }

    memcpy(mac.octet, macStr, ETHER_ADDR_LEN);

    printf("Post Mac address: %02bx: %02bx: %02bx: %02bx: %02bx: %02bx\n", \
        macStr[0], macStr[1], macStr[2], macStr[3], macStr[4], macStr[5]);

    if ((ret = sal_l2_addr_get(&mac, fid, &search_res)) != SAL_ERR_SUCCESS)
    {
        printerr(pReq, ret);       
        return SUCCESS;
    }

    _get_mac_search_res(pReq, &search_res);
    return SUCCESS;
}

/*
 * Function: post_mac_tblclear
 * ---------------------------
 * Clears all dynamic MAC entries from the forwarding table.
 * Calls sal_l2_table_clear() with SAL_L2_CLR_DYNAMIC flag.
 * Static MAC entries are preserved.
 * Refreshes MAC forwarding table page after clear.
 */
int32
post_mac_tblclear(struct http_request_s *pReq)
{
    int32 ret;
    if ((ret = sal_l2_table_clear(SAL_L2_CLR_DYNAMIC)) != SAL_ERR_SUCCESS)
    {
        printerr(pReq, ret);
        return SUCCESS;
    }


    get_mac_forwardingtable(pReq);
    return SUCCESS;
}

/*
 * Function: get_mac_constraint
 * ----------------------------
 * Displays MAC learning constraint configuration page.
 * Shows current learn-over action (Drop/Flooding).
 * Allows setting per-port entry limits with enable/disable.
 * Displays current entry limits for all ports.
 */
int32 get_mac_constraint(struct http_request_s *pReq)
{
    uint8   port = 0;
    uint8   logicPortStr[MAX_LOGIC_ID_LEN];
    int32   ret;        
    uint32  limits;
    sal_l2_limitLearnCntAction_t act;
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>MAC Constraint Setting</title>\n");
    WEB_PRINTF(pReq, "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n");
    WEB_PRINTF(pReq, "<script type=\"text/javascript\">\n");
    WEB_PRINTF(pReq, "  function chkEnable(value)\n");
    WEB_PRINTF(pReq, "  {\n");
    WEB_PRINTF(pReq, "      if ( value == '1' ) {\n");
    WEB_PRINTF(pReq, "          document.getElementById('limit').value = \'\';\n");
    WEB_PRINTF(pReq, "          document.getElementById('limit').disabled = false;\n");
    WEB_PRINTF(pReq, "      }\n");
    WEB_PRINTF(pReq, "      else { \n");
    WEB_PRINTF(pReq, "          document.getElementById('limit').value = \'Unlimited\';\n");
    WEB_PRINTF(pReq, "          document.getElementById('limit').disabled = true;\n");
    WEB_PRINTF(pReq, "      }\n");
    WEB_PRINTF(pReq, "  }\n");
    WEB_PRINTF(pReq, "</script>\n");
    WEB_PRINTF(pReq, "</head>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<body>\n");


        WEB_PRINTF(pReq, "<center>\n");
        WEB_PRINTF(pReq, "\n");
        WEB_PRINTF(pReq, "<fieldset>\n");
        WEB_PRINTF(pReq, "<legend>MAC Constraint Action Setting</legend>\n");
        WEB_PRINTF(pReq, "<form method=\"post\" action=\"/mac_constraint.cgi?page=set_action\">\n");

        WEB_PRINTF(pReq, "<br>\n");
        WEB_PRINTF(pReq, "<table border=\"1\">\n");
        WEB_PRINTF(pReq, "  <tr>\n");
        WEB_PRINTF(pReq, "    <th width=\"200\">Learn over Action</th>\n");

        WEB_PRINTF(pReq, "      <td width=\"150\">\n");
        WEB_PRINTF(pReq, "        <select name=\"select_action\" style=\"width:150\">\n");

        if ((ret = sal_l2_limitLearningCntAction_get(SAL_WHOLE_SYSTEM, &act)) != SAL_ERR_SUCCESS)
            printerr(pReq, ret);

        switch(act)
        {
        case SAL_LIMIT_LEARN_CNT_ACTION_DROP:
            WEB_PRINTF(pReq, "      <option value=\"0\" selected>Drop \n");                
            WEB_PRINTF(pReq, "      <option value=\"1\">Flooding \n");
            break;
            
        case SAL_LIMIT_LEARN_CNT_ACTION_FORWARD:
            WEB_PRINTF(pReq, "      <option value=\"0\" >Drop \n");                        
            WEB_PRINTF(pReq, "      <option value=\"1\" selected>Flooding \n");
            break;
            
        default:
            printmsg(pReq, "Learnover: wrong!\n");
            return SUCCESS;
        }
        
        WEB_PRINTF(pReq, "        </select>\n");
        WEB_PRINTF(pReq, "      </td>\n");

        WEB_PRINTF(pReq, "  </tr>\n");
        WEB_PRINTF(pReq, "</table>\n");
        WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
        WEB_PRINTF(pReq, "  <input type=\"submit\" value=\"   Apply   \">\n");
        WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"sel_act\">\n");
        WEB_PRINTF(pReq, "</form>\n");
        WEB_PRINTF(pReq, "</fieldset>\n");
        WEB_PRINTF(pReq, "</center>\n");


    WEB_PRINTF(pReq, "<center>\n");
    WEB_PRINTF(pReq, "\n");
    WEB_PRINTF(pReq, "<fieldset>\n");
    WEB_PRINTF(pReq, "<legend>MAC Constraint Setting</legend>\n");

    WEB_PRINTF(pReq, "<form method=\"post\" action=\"/mac_constraint.cgi\">\n");
    WEB_PRINTF(pReq, "  <table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th>Port</th>\n");
    WEB_PRINTF(pReq, "      <th>State</th>\n");
    WEB_PRINTF(pReq, "      <th nowrap>Entry Limits</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <td>\n");
    WEB_PRINTF(pReq, "        <select name=\"portid\" multiple size=\"6\">\n");

    for (port = 0; port < SAL_PORT_ID_CPU; ++port) {
        if (!(sal_trunk_logicId_exist(port))) {
            continue;
        }

        sal_trunk_logicId_toStr(logicPortStr, port);
        WEB_PRINTF(pReq, "          <option value=\"%d\">%s\n", (uint16)port, logicPortStr);
    }
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "      <td>\n");
    WEB_PRINTF(pReq, "        <select name=\"state\" onload=\"chkEnable(this.value)\" onchange=\"chkEnable(this.value)\">\n");
    WEB_PRINTF(pReq, "          <option value=\"%d\">Disable\n", (uint16)DISABLE);
    WEB_PRINTF(pReq, "          <option value=\"%d\">Enable\n", (uint16)ENABLE);
    WEB_PRINTF(pReq, "        </select>\n");
    WEB_PRINTF(pReq, "      </td>\n");

    WEB_PRINTF(pReq, "      <td><input type=\"text\" size=\"6\" name=\"limit\" id=\"limit\" value=\'Unlimited\' disabled = true>"
        "(0-%lu)</td>\n",rtk_switch_maxLutAddrNumber_get());
    WEB_PRINTF(pReq, "    </tr>\n");

    WEB_PRINTF(pReq, "  </table>\n");
    WEB_PRINTF(pReq, "  <br style=\"line-height:50%%\">\n");
    WEB_PRINTF(pReq, "  <input type=\"submit\" name=\"submit\" value=\"   Apply   \">\n");
    WEB_PRINTF(pReq, "  <input type=\"hidden\" name=\"cmd\" value=\"mac_constraint\">\n");
    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "<hr>");
    WEB_PRINTF(pReq, "<br>\n");

    WEB_PRINTF(pReq, "<table border=\"1\">\n");
    WEB_PRINTF(pReq, "    <tr>\n");
    WEB_PRINTF(pReq, "      <th width=\"90\">Port</th>\n");
    WEB_PRINTF(pReq, "      <th width=\"120\">Entry Limits</th>\n");
    WEB_PRINTF(pReq, "    </tr>\n");

    for (port = 0; port < SAL_PORT_ID_CPU; ++port) {
        if (!(sal_trunk_logicId_exist(port))) {
            continue;
        }

        sal_trunk_logicId_toStr(logicPortStr, port);

        if ((ret = sal_l2_limitLearningCnt_get(port, &limits)) != SAL_ERR_SUCCESS)
            printerr(pReq, ret);
        WEB_PRINTF(pReq, "  <tr>\n");
        WEB_PRINTF(pReq, "    <td>%s</td>\n", logicPortStr);

        WEB_PRINTF(pReq, "    <td>");

        if (limits == rtk_switch_maxLutAddrNumber_get()) 
            WEB_PRINTF(pReq, "Unlimited");
        else
            WEB_PRINTF(pReq, "%ld", limits);

        WEB_PRINTF(pReq, "    </td>\n");

        WEB_PRINTF(pReq, "    <td>");
        WEB_PRINTF(pReq, "    </td>\n");
        WEB_PRINTF(pReq, "  </tr>\n");
    }

    WEB_PRINTF(pReq, "</table>\n");
    WEB_PRINTF(pReq, "<br>\n");
    WEB_PRINTF(pReq, "</fieldset>\n");
    WEB_PRINTF(pReq, "</center>\n");
    
    WEB_PRINTF(pReq, "<br>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: post_mac_constraint
 * -----------------------------
 * Processes MAC learning constraint form submission.
 * Extracts selected ports, enable/disable state, and limit value.
 * Sets entry limit for each selected port using sal_l2_limitLearningCnt_set().
 * Unlimited limit is represented by MAX_LUT_ADDR_NUMBER.
 * Refreshes constraint page after applying settings.
 */
int32 post_mac_constraint(struct http_request_s *pReq)
{
    int32   ret;
    int16   i;
    uint8   *pValue;
    uint8   port[SAL_MAX_LOGIC_COMBO_PORT];
    uint8   settingCnt;     // the total port numbers to set
    Tbool   enable;
    uint32  limits;
    settingCnt = 0;
    enable = DISABLE;

    WEB_VAR(pReq, "portid", &pValue);
    if (pValue) {
        port[settingCnt++] = atoi(pValue);
    }

    while(pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue) {
            port[settingCnt++] = atoi(pValue);
        }
    }

    WEB_VAR(pReq, "state", &pValue);
    if (pValue) {
        enable = atoi(pValue);
    }

    if (enable != DISABLE)
    {
        WEB_VAR(pReq, "limit", &pValue);
        if (pValue) {
            limits = atol(pValue);

            if (limits < 0 || limits > rtk_switch_maxLutAddrNumber_get())
            {
                printmsg(pReq, "Invalid Entry limits !!");
                return SUCCESS;
            }
        }
    }

    for (i = 0; i < settingCnt; ++i) {

        if (!(sal_trunk_logicId_exist(port[i]))) {
            continue;
        }
        
        if (enable == DISABLE)
        {
            limits = rtk_switch_maxLutAddrNumber_get();
        }
       
        if ((ret = sal_l2_limitLearningCnt_set(port[i], limits)) != SAL_ERR_SUCCESS) 
        {
            printerr(pReq, ret);
            return SUCCESS;
        }
    }

        
    get_mac_constraint(pReq);
            return SUCCESS;
        }
        
/*
 * Function: post_mac_constraint_action
 * ------------------------------------
 * Processes learn-over action setting from form submission.
 * Extracts selected action (Drop/Flooding) from POST data.
 * Calls sal_l2_limitLearningCntAction_set() to apply globally.
 * Refreshes constraint page after applying action.
 */
int32 post_mac_constraint_action(struct http_request_s *pReq)
{
    int32   ret;
    uint8   *pValue;
    sal_l2_limitLearnCntAction_t  act;

    WEB_VAR(pReq, "select_action", &pValue);
    if (pValue) {
        act = atol(pValue);
    }
    if ((ret = sal_l2_limitLearningCntAction_set(SAL_WHOLE_SYSTEM, act)) != SAL_ERR_SUCCESS) 
    {
        printerr(pReq, ret);
        return SUCCESS;
    }


    get_mac_constraint(pReq);
    return SUCCESS;
}