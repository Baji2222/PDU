#pragma STRING(far)
/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : The handler of HTTP request to QoS function
 *
 * Feature : Response the request related to QoS function
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <salqos.h>
#include <salerror.h>
#include <saltrunk.h>
#include <salsys.h>

#include <web/web_api.h>
#include "web_qos.h"
#include "web_util.h"
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
 * Function: get_qos
 * -----------------
 * Placeholder handler for QoS main page.
 * Currently displays simple text response.
 * Can be extended for main QoS configuration.
 */
int32 get_qos(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "This is qos.cgi");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_qos_portbasedpriority
 * -----------------------------------
 * Displays port-based priority configuration page.
 * Currently placeholder - returns empty HTML.
 * Intended to show port-to-priority mapping settings.
 */
int32 get_qos_portbasedpriority(struct http_request_s *pReq)
{
    //    sal_qos_pri2queue_t pri2qid;
    sal_pri_t pri;
    uint8 port = 0;
    uint8 logicPortStr[MAX_LOGIC_ID_LEN];
    //    uint8       i;
    uint8 queue_num;

    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "</html>\n");
    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_qos_packetscheduling
 * ----------------------------------
 * Displays packet scheduling configuration page.
 * Shows queue weights and scheduling algorithm settings.
 * Currently placeholder - returns empty HTML.
 */
int32 get_qos_packetscheduling(struct http_request_s *pReq)
{
    sal_qos_queue_weights_t qweights;
    uint8 port = 0;
    uint8 queue_num;
    uint8 i;

    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_qos_internalPriority
 * ----------------------------------
 * Displays internal priority selection configuration page.
 * Shows priority decision order for QoS classification.
 * Currently placeholder - returns empty HTML.
 */
int32 get_qos_internalPriority(struct http_request_s *pReq)
{
    uint16 int_pri_number;
    sal_qos_pri_selet_t int_pri;

    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    
    WEB_PRINTF(pReq, "</html>\n");
    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_qos_dscpremapping
 * -------------------------------
 * Displays DSCP priority remapping configuration page.
 * Shows DSCP-to-internal-priority mapping table.
 * Currently placeholder - returns empty HTML.
 */
int32 get_qos_dscpremapping(struct http_request_s *pReq)
{
    uint16 dscp;
    uint8 queue_num, pri;
    sal_dscp_t dscp_val;
    sal_pri_t int_pri;

    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");

    WEB_PRINTF(pReq, "</html>\n");
    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: get_qos_pri2queueid
 * -----------------------------
 * Displays priority-to-queue mapping configuration page.
 * Shows mapping of internal priorities to queue IDs.
 * Currently placeholder - returns empty HTML.
 */
int32 get_qos_pri2queueid(struct http_request_s *pReq)
{
    uint16 pri;
    uint8 queue_num, que;
    sal_qos_pri2queue_t priToQueueMap;

    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "</html>\n");
    WEB_END(pReq);

    return SUCCESS;
}

/*
 * Function: post_qos_portbasedpriority
 * ------------------------------------
 * Processes port-based priority form submission.
 * Extracts selected ports and priority value from POST data.
 * Applies priority setting to selected ports via sal_qos_portPri_set().
 * Refreshes port-based priority page after applying.
 */
int32 post_qos_portbasedpriority(struct http_request_s *pReq)
{
    int32 ret;
    int16 i;
    uint8 port[SAL_MAX_LOGIC_COMBO_PORT];
    uint8 *pValue;
    uint8 settingCnt = 0, priority = 0;

    WEB_VAR(pReq, "portid", &pValue);
    if (pValue)
    {
        port[settingCnt++] = atoi(pValue);
    }
    while (pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue)
        {
            port[settingCnt++] = atoi(pValue);
        }
    }

    WEB_VAR(pReq, "port_priority", &pValue);
    if (pValue)
    {
        priority = atoi(pValue);
    }

    for (i = 0; i < settingCnt; ++i)
    {

        if (!(sal_trunk_logicId_exist(port[i])))
        {
            continue;
        }
#if 1

        if ((ret = sal_qos_portPri_set(port[i], priority)) != SAL_ERR_SUCCESS)
            return ret;
#else
        if ((ret = sal_qos_port2queue_set(port[i], qid)) != SAL_ERR_SUCCESS)
        {
            printerr(pReq, ret);
            return SUCCESS;
        }
#endif
    }

    get_qos_portbasedpriority(pReq);
    return SUCCESS;
}

/*
 * Function: post_qos_queueweight
 * ------------------------------
 * Processes queue weight configuration form submission.
 * Extracts queue IDs and weight value from POST data.
 * Updates WRR weights for selected queues on all ports.
 * Refreshes packet scheduling page after applying.
 */
int32 post_qos_queueweight(struct http_request_s *pReq)
{
    sal_qos_queue_weights_t qweights;
    int32 ret;
    int16 i;
    uint8 port;
    uint8 queueId[SAL_MAX_NUM_OF_QUEUE];
    uint8 *value;

    uint8 entryNum, weight;

    entryNum = 0;
    weight = 0;
    WEB_VAR(pReq, "queueid", &value);
    if (value)
    {
        queueId[entryNum++] = atoi(value);
    }
    while (value != NULL)
    {
        WEB_VAR(pReq, NULL, &value);
        if (value)
        {
            queueId[entryNum++] = atoi(value);
        }
    }

    WEB_VAR(pReq, "weight", &value);
    if (value)
    {
        weight = atoi(value);
    }

    for (port = 0; port < SAL_PORT_ID_CPU; ++port)
    {

        if (!(sal_trunk_logicId_exist(port)))
        {
            continue;
        }

        sal_qos_schedulingQueue_get(port, &qweights);
        for (i = 0; i < entryNum; i++)
        {
            qweights.weights[queueId[i]] = weight;
        }
        if ((ret = sal_qos_schedulingQueue_set(port, &qweights)) != SAL_ERR_SUCCESS)
        {
            printerr(pReq, ret);
            return SUCCESS;
        }
    }

    get_qos_packetscheduling(pReq);
    return SUCCESS;
}

/*
 * Function: post_qos_internalPriority
 * -----------------------------------
 * Processes internal priority selection form submission.
 * Extracts priority source selections and decision value.
 * Updates priority order for selected sources (port, dot1q, ACL, DSCP, etc.).
 * Refreshes internal priority page after applying.
 */
int32 post_qos_internalPriority(struct http_request_s *pReq)
{
    int32 ret;
    int16 i;
    uint16 priSet[8];
    uint8 *pValue;
    uint8 settingCnt = 0;
    uint16 dec;
    sal_qos_pri_selet_t priDec;

    WEB_VAR(pReq, "internalPriority", &pValue);
    if (pValue)
    {
        priSet[settingCnt++] = atoi(pValue);
    }
    while (pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue)
        {
            priSet[settingCnt++] = atoi(pValue);
        }
    }

    WEB_VAR(pReq, "decision", &pValue);
    if (pValue)
    {
        dec = atoi(pValue);
    }

    sal_qos_priSel_get(&priDec);
    for (i = 0; i < settingCnt; i++)
    {
        if (priSet[i] == 0)
        {
            priDec.port_pri = (uint32)dec;
        }
        else if (priSet[i] == 1)
        {
            priDec.dot1q_pri = (uint32)dec;
        }
        else if (priSet[i] == 2)
        {
            priDec.acl_pri = (uint32)dec;
        }
        else if (priSet[i] == 3)
        {
            priDec.dscp_pri = (uint32)dec;
        }
        else if (priSet[i] == 4)
        {
            priDec.cvlan_pri = dec;
        }
        else if (priSet[i] == 5)
        {
            priDec.svlan_pri = (uint32)dec;
        }
        else if (priSet[i] == 6)
        {
            priDec.dmac_pri = (uint32)dec;
        }
        else if (priSet[i] == 7)
        {
            priDec.smac_pri = (uint32)dec;
        }
        else
        {
            get_qos_internalPriority(pReq);
            return SUCCESS;
        }
    }

    if ((ret = sal_qos_priSel_set(&priDec)) != SAL_ERR_SUCCESS)
    {
        printerr(pReq, ret);
        return SUCCESS;
    }
    get_qos_internalPriority(pReq);
    return SUCCESS;
}

/*
 * Function: post_qos_dscpremapping
 * --------------------------------
 * Processes DSCP priority remapping form submission.
 * Extracts DSCP values and priority to map.
 * Updates DSCP-to-internal-priority mapping for selected DSCP values.
 * Refreshes DSCP remapping page after applying.
 */
int32 post_qos_dscpremapping(struct http_request_s *pReq)
{
    int32 ret;
    int16 i;
    uint32 j;
    uint32 dscp[SAL_MAX_VALUE_OF_DSCP];
    uint8 priValue;
    uint8 settingCnt = 0, qid = 0;
    uint8 *pValue;

    WEB_VAR(pReq, "dscp_value", &pValue);
    if (pValue)
    {
        dscp[settingCnt++] = atoi(pValue);
    }
    while (pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue)
        {
            dscp[settingCnt++] = atoi(pValue);
        }
    }

    WEB_VAR(pReq, "dscp_priority", &pValue);
    if (pValue)
    {
        priValue = atoi(pValue);
    }

    for (i = 0; i < settingCnt; ++i)
    {
        for (j = 0; j < SAL_MAX_VALUE_OF_DSCP; j++)
        {
            if (dscp[i] == j)
            {
                if ((ret = sal_qos_dscpPriRemap_set(j, priValue)) != SAL_ERR_SUCCESS)
                {
                    printerr(pReq, ret);
                    return SUCCESS;
                }
            }
        }
    }

    get_qos_dscpremapping(pReq);
    return SUCCESS;
}

/*
 * Function: post_qos_pri2queueid
 * ------------------------------
 * Processes priority-to-queue mapping form submission.
 * Extracts priority values and queue ID to map.
 * Updates priority-to-queue mapping table.
 * Refreshes priority-to-queue page after applying.
 */
int32 post_qos_pri2queueid(struct http_request_s *pReq)
{
    int32 ret;
    int16 i;
    uint8 j;
    uint16 priority[SAL_MAX_NUM_OF_PRIORITY], tmp;
    uint32 queueid;
    uint8 settingCnt = 0, qid = 0;
    uint8 *pValue;
    uint8 queue_num;
    sal_qos_pri2queue_t priToQue;

    WEB_VAR(pReq, "priority_value", &pValue);
    if (pValue)
    {
        tmp = atoi(pValue);
        priority[settingCnt++] = tmp;
    }
    while (pValue != NULL)
    {
        WEB_VAR(pReq, NULL, &pValue);
        if (pValue)
        {
            priority[settingCnt++] = atoi(pValue);
        }
    }

    WEB_VAR(pReq, "priority_to_queueid", &pValue);
    if (pValue)
    {
        queueid = atoi(pValue);
    }
    sal_qos_queueNum_get(0, &queue_num);
    sal_qos_priMap_get(queue_num, &priToQue);

    for (i = 0; i < settingCnt; ++i)
    {
        for (j = 0; j < SAL_MAX_NUM_OF_PRIORITY; j++)
        {
            if (j == priority[i])
            {
                priToQue.pri2queue[j] = queueid;
            }
        }
    }
    if ((ret = sal_qos_priMap_set(queue_num, &priToQue)) != SAL_ERR_SUCCESS)
    {
        printerr(pReq, ret);
        return SUCCESS;
    }

    get_qos_pri2queueid(pReq);
    return SUCCESS;
}



int32 get_rtc(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");
    WEB_PUTS(pReq,
    "<html><head><meta name=viewport content=\"width=device-width,initial-scale=1\">"
    "<title>RTC INFO</title>"
    "<style>"
    
    /* Centered Body layout */
    "body{font-family:Arial,sans-serif;background:#f0f2f5;margin:0;min-height:100vh;display:flex;justify-content:center;align-items:center;padding:20px;box-sizing:border-box;color:#1c1f23}"
    
    /* Container matching the Set pages */
    ".container{background:#fff;border-radius:12px;padding:32px;width:100%;max-width:840px;box-shadow:0 4px 20px rgba(0,0,0,.1);}"
    "h1{font-size:20px;margin:0 0 24px;padding-bottom:12px;border-bottom:3px solid #0f1a2b;text-transform:uppercase;letter-spacing:1px;color:#1c1f23;}"
    
    /* Table styling */
    "table{width:100%;border-collapse:collapse;font-size:16px}"
    "th{text-align:left;padding:14px 12px;background:#f1f3f5;color:#6b7280;font-weight:600;font-size:13px;text-transform:uppercase;letter-spacing:.5px}"
    "th:last-child{text-align:right;padding-right:24px}"
    "td{padding:16px 12px;border-bottom:1px solid #e9ecef;text-transform:uppercase}"
    
    /* COLORS: Make countdown and current time dark blue */
    "td:last-child{text-align:right;font-weight:700;font-size:17px;padding-right:24px;color:#0f1a2b}"
    "#t{color:#0f1a2b;font-weight:700;}" 
    
    "tr:last-child td{border:none}"
    
    /* Tags */
    ".tag{display:inline-block;padding:4px 8px;border-radius:4px;font-size:12px;font-weight:700;margin-left:8px}"
    ".tag.on{background:#d3f9d8;color:#2b8a3e}"
    ".tag.off{background:#ffe3e3;color:#c92a2a}"
    "</style></head><body>"
    
    "<div class=container>"
    "<h1>RTC Time, Date & Alarms</h1>"
    "<table>"
    "<tr><th>Parameter</th><th>Value</th><th style='text-align:right'>Status / Countdown</th></tr>"
    "<tr><td>Current Time</td><td id=t>--:--:--</td><td></td></tr>"
    "<tr><td>Current Date</td><td id=d>--/--/----</td><td></td></tr>"
    "<tr><td>Alarm 1</td><td id=a1>--:--</td><td id=cd1>--</td></tr>"
    "<tr><td>Alarm 2</td><td id=a2>--:--</td><td id=cd2>--</td></tr>"
    "<tr><td>Alarm 3</td><td id=a3>--:--</td><td id=cd3>--</td></tr>"
    "</table>"
    "</div>"
    
    "<script>"
    "async function jget(u){"
    "const r=await fetch(u,{cache:'no-store'});"
    "if(!r.ok)throw new Error(r.status);"
    "return r.json();"
    "}"
    "const p2=n=>String(n).padStart(2,'0');"
    
    "let currH=0, currM=0, currS=0;"
    "let alarms=[null, null, null];"

    "function updateUI(){"
    "  currS++;"
    "  if(currS>=60){currS=0; currM++;}"
    "  if(currM>=60){currM=0; currH++;}"
    "  if(currH>=24){currH=0;}"
    
    "  t.textContent=p2(currH)+':'+p2(currM)+':'+p2(currS);"
    
    "  for(let i=0; i<3; i++){"
    "    if(!alarms[i]) { document.getElementById('cd'+(i+1)).textContent='--'; continue; }"
    "    let a = alarms[i];"
    
    "    let nowTotalMins = (currH * 60) + currM;"
    "    let targetTotalMins = (a.h * 60) + a.m;"
    "    let diff = targetTotalMins - nowTotalMins;"
    "    if(diff < 0) diff += 1440;"
    
    "    let hrs = Math.floor(diff / 60);"
    "    let mins = diff - (hrs * 60);"
    
    "    document.getElementById('cd'+(i+1)).textContent = p2(hrs)+'H '+p2(mins)+'M';"
    "  }"
    "}"

    "async function fetchAll(){"
    "  let retryDelay = 30000;"
    "  try{"
    "    const j=await jget('/get_rtc_time');"
    
    "    /* 1. Check if JSON is missing keys */"
    "    if(j.H===undefined || j.A1H===undefined || j.A3H===undefined || j.S3===undefined) {"
    "      throw new Error('Incomplete JSON');"
    "    }"
    
    "    /* 2. Strict Range Validation (Hours 0-23, Mins 0-59, Outlets 1-8, State 0-1) */"
    "    if(j.A1H > 23 || j.A2H > 23 || j.A3H > 23 ||"
    "       j.A1M > 59 || j.A2M > 59 || j.A3M > 59 ||"
    "       j.O1 < 1 || j.O1 > 8 || j.O2 < 1 || j.O2 > 8 || j.O3 < 1 || j.O3 > 8 ||"
    "       j.S1 > 1 || j.S2 > 1 || j.S3 > 1) {"
    "      throw new Error('Invalid Alarm Data Received');"
    "    }"

    "    let diff = j.S - currS;"
    "    if (diff < -10) diff += 60;"
    "    if (diff < 0 || diff > 3) { currH = j.H; currM = j.M; currS = j.S; }"
    
    "    d.textContent=p2(j.D)+'/'+p2(j.MO)+'/'+j.Y;"
    
    "    alarms[0] = {h: j.A1H, m: j.A1M, o: j.O1, s: j.S1};"
    "    alarms[1] = {h: j.A2H, m: j.A2M, o: j.O2, s: j.S2};"
    "    alarms[2] = {h: j.A3H, m: j.A3M, o: j.O3, s: j.S3};"
    
    "    for(let i=0; i<3; i++){"
    "      let a = alarms[i];"
    "      let tag = (a.s==1) ? '<span class=\"tag on\">OUT '+a.o+' ON</span>' : '<span class=\"tag off\">OUT '+a.o+' OFF</span>';"
    "      document.getElementById('a'+(i+1)).innerHTML = p2(a.h)+':'+p2(a.m)+tag;"
    "    }"
    "    updateUI();"
    "  }catch(e){"
    "    console.log('Data error, retrying immediately:', e);"
    "    retryDelay = 1000; /* Retry immediately */"
    "  }"
    "  setTimeout(fetchAll, retryDelay);"
    "}"

    "fetchAll();"
    "setInterval(updateUI, 1000);"
    
    "</script></body></html>");
    WEB_END(pReq);
    return SUCCESS;
}





int32 set_rtc_time_date(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html><head><meta name=viewport content=\"width=device-width,initial-scale=1\">");
    WEB_PRINTF(pReq, "<title>Set RTC Time & Date</title>");
    
    WEB_PRINTF(pReq, "<style>");
    WEB_PRINTF(pReq, "body{font-family:Arial,sans-serif;background:#f0f2f5;margin:0;min-height:100vh;display:flex;justify-content:center;align-items:center;padding:20px;box-sizing:border-box;}");
    
    /* Container styled identically to the Alarm page */
    WEB_PRINTF(pReq, ".container{background:#fff;border-radius:12px;padding:32px;width:100%%;max-width:480px;box-shadow:0 4px 20px rgba(0,0,0,.1);}");
    WEB_PRINTF(pReq, "h1{font-size:20px;margin:0 0 24px;padding-bottom:12px;border-bottom:3px solid #0f1a2b;text-transform:uppercase;letter-spacing:1px;color:#1c1f23;}");
    
    WEB_PRINTF(pReq, ".box{border:1px solid #e5e7eb;border-radius:8px;padding:24px;}");
    WEB_PRINTF(pReq, "label{display:block;font-size:12px;font-weight:600;color:#6b7280;margin:12px 0 6px;}");
    
    WEB_PRINTF(pReq, "input{width:100%%;padding:12px;border:1px solid #d1d5db;border-radius:6px;font-size:14px;box-sizing:border-box;outline:none;}");
    WEB_PRINTF(pReq, "input:focus{border-color:#00d4ff;box-shadow:0 0 0 2px rgba(0,212,255,0.2);}");
    
    /* Changed button to Blue/Teal theme */
    WEB_PRINTF(pReq, "button{width:100%%;margin-top:24px;padding:14px;background:#0f1a2b;color:#fff;border:0;border-radius:6px;font-size:15px;font-weight:700;cursor:pointer;transition:background 0.2s;}");
    WEB_PRINTF(pReq, "button:hover{background:#1a2f44;}");
    
    WEB_PRINTF(pReq, ".msg{margin-top:16px;font-size:14px;font-weight:600;text-align:center;min-height:20px;}");
    WEB_PRINTF(pReq, "</style></head><body>");

    WEB_PRINTF(pReq, "<div class=container><h1>Set RTC Time & Date</h1><div class=box>");

    WEB_PRINTF(pReq, "<label>Hour (0-23)</label><input id=h type=number min=0 max=23 value=0>");
    WEB_PRINTF(pReq, "<label>Minute (0-59)</label><input id=mi type=number min=0 max=59 value=0>");
    WEB_PRINTF(pReq, "<label>Second (0-59)</label><input id=s type=number min=0 max=59 value=0>");
    WEB_PRINTF(pReq, "<label>Day (1-31)</label><input id=d type=number min=1 max=31 value=1>");
    WEB_PRINTF(pReq, "<label>Month (1-12)</label><input id=mo type=number min=1 max=12 value=1>");
    WEB_PRINTF(pReq, "<label>Year</label><input id=y type=number min=2020 max=2099 value=2026>");
    
    WEB_PRINTF(pReq, "<button type=button onclick=\"f()\">Apply Time & Date</button>");
    WEB_PRINTF(pReq, "<div class=msg id=m></div>");

    WEB_PRINTF(pReq, "</div></div><script>");

    WEB_PRINTF(pReq, "async function f(){");
    WEB_PRINTF(pReq, "var m=document.getElementById('m');");
    WEB_PRINTF(pReq, "var h=document.getElementById('h').value;");
    WEB_PRINTF(pReq, "var mi=document.getElementById('mi').value;");
    WEB_PRINTF(pReq, "var s=document.getElementById('s').value;");
    WEB_PRINTF(pReq, "var d=document.getElementById('d').value;");
    WEB_PRINTF(pReq, "var mo=document.getElementById('mo').value;");
    WEB_PRINTF(pReq, "var y=document.getElementById('y').value;");
    
    WEB_PRINTF(pReq, "var u='/set_rtc_time?H='+h+'&M='+mi+'&S='+s+'&D='+d+'&MO='+mo+'&Y='+y;");
    
    WEB_PRINTF(pReq, "try{");
    WEB_PRINTF(pReq, "const r=await fetch(u,{cache:'no-store'});");
    WEB_PRINTF(pReq, "m.textContent=r.ok?'Time & Date Updated':'Failed ('+r.status+')';");
    WEB_PRINTF(pReq, "m.style.color=r.ok?'#0f1a2b':'#e03131';");
    WEB_PRINTF(pReq, "}catch(e){m.textContent='Error: '+e;m.style.color='#e03131';}");
    WEB_PRINTF(pReq, "}");
    
    WEB_PRINTF(pReq, "</script></body></html>");

    WEB_END(pReq);
    return SUCCESS;
}


int32 set_rtc_alarm(struct http_request_s *pReq)
{
    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html><head><meta name=viewport content=\"width=device-width,initial-scale=1\">");
    WEB_PRINTF(pReq, "<title>Set RTC Alarms</title>");
    
    WEB_PRINTF(pReq, "<style>");
    WEB_PRINTF(pReq, "body{font-family:Arial,sans-serif;background:#f0f2f5;margin:0;min-height:100vh;display:flex;justify-content:center;align-items:center;padding:20px;box-sizing:border-box;}");
    
    WEB_PRINTF(pReq, ".container{background:#fff;border-radius:12px;padding:32px;width:100%%;max-width:480px;box-shadow:0 4px 20px rgba(0,0,0,.1);}");
    
    /* Changed border-bottom to the Blue theme */
    WEB_PRINTF(pReq, "h1{font-size:20px;margin:0 0 24px;padding-bottom:12px;border-bottom:3px solid #0f1a2b;text-transform:uppercase;letter-spacing:1px;color:#1c1f23;}");
    
    WEB_PRINTF(pReq, ".box{border:1px solid #e5e7eb;border-radius:8px;padding:24px;}");
    WEB_PRINTF(pReq, "label{display:block;font-size:12px;font-weight:600;color:#6b7280;margin:12px 0 6px;}");
    
    /* Changed input focus to Cyan */
    WEB_PRINTF(pReq, "input,select{width:100%%;padding:12px;border:1px solid #d1d5db;border-radius:6px;font-size:14px;box-sizing:border-box;outline:none;}");
    WEB_PRINTF(pReq, "input:focus,select:focus{border-color:#00d4ff;box-shadow:0 0 0 2px rgba(0,212,255,0.2);}");
    
    /* Changed Button to Blue theme */
    WEB_PRINTF(pReq, "button{width:100%%;margin-top:24px;padding:14px;background:#0f1a2b;color:#fff;border:0;border-radius:6px;font-size:15px;font-weight:700;cursor:pointer;transition:background 0.2s;}");
    WEB_PRINTF(pReq, "button:hover{background:#1a2f44;}");
    
    WEB_PRINTF(pReq, ".msg{margin-top:16px;font-size:14px;font-weight:600;text-align:center;min-height:20px;}");
    WEB_PRINTF(pReq, "</style></head><body>");

    WEB_PRINTF(pReq, "<div class=container><h1>Set RTC Alarm</h1><div class=box>");

    WEB_PRINTF(pReq, "<label>Select Alarm to Configure</label>");
    WEB_PRINTF(pReq, "<select id=alarmId>");
    WEB_PRINTF(pReq, "<option value=1>Alarm 1</option>");
    WEB_PRINTF(pReq, "<option value=2>Alarm 2</option>");
    WEB_PRINTF(pReq, "<option value=3>Alarm 3</option>");
    WEB_PRINTF(pReq, "</select>");

    WEB_PRINTF(pReq, "<label>Hour (0-23)</label><input id=h type=number min=0 max=23 value=0>");
    WEB_PRINTF(pReq, "<label>Minute (0-59)</label><input id=m type=number min=0 max=59 value=0>");
    
    WEB_PRINTF(pReq, "<label>Outlet</label><select id=out>");
    WEB_PRINTF(pReq, "<option value=1>Outlet 1</option><option value=2>Outlet 2</option>");
    WEB_PRINTF(pReq, "<option value=3>Outlet 3</option><option value=4>Outlet 4</option>");
    WEB_PRINTF(pReq, "<option value=5>Outlet 5</option><option value=6>Outlet 6</option>");
    WEB_PRINTF(pReq, "<option value=7>Outlet 7</option><option value=8>Outlet 8</option>");
    WEB_PRINTF(pReq, "</select>");
    
    WEB_PRINTF(pReq, "<label>Action</label><select id=act>");
    WEB_PRINTF(pReq, "<option value=1>ON</option><option value=0>OFF</option>");
    WEB_PRINTF(pReq, "</select>");

    WEB_PRINTF(pReq, "<button type=button onclick=\"sendAlarm()\">Apply Alarm Settings</button>");
    WEB_PRINTF(pReq, "<div class=msg id=msg></div>");

    WEB_PRINTF(pReq, "</div></div><script>");

    WEB_PRINTF(pReq, "function ci(v,lo,hi){");
    WEB_PRINTF(pReq, "var n=parseInt(String(v).replace(/[^0-9]/g,''),10);");
    WEB_PRINTF(pReq, "return (isFinite(n)&&n>=lo&&n<=hi)?n:null;");
    WEB_PRINTF(pReq, "}");

    WEB_PRINTF(pReq, "function sendAlarm(){");
    WEB_PRINTF(pReq, "var msg=document.getElementById('msg');");
    
    WEB_PRINTF(pReq, "var id=ci(document.getElementById('alarmId').value,1,3);");
    WEB_PRINTF(pReq, "var h=ci(document.getElementById('h').value,0,23);");
    WEB_PRINTF(pReq, "var m=ci(document.getElementById('m').value,0,59);");
    WEB_PRINTF(pReq, "var out=ci(document.getElementById('out').value,1,8);");
    WEB_PRINTF(pReq, "var act=ci(document.getElementById('act').value,0,1);");

    WEB_PRINTF(pReq, "if(id===null||h===null||m===null||out===null||act===null){");
    WEB_PRINTF(pReq, "msg.textContent='Invalid input';msg.style.color='#e03131';return;");
    WEB_PRINTF(pReq, "}");
    
    WEB_PRINTF(pReq, "var url='/set_alarm'+id+'?h='+h+'&m='+m+'&out='+out+'&act='+act;");
    WEB_PRINTF(pReq, "var xhr=new XMLHttpRequest();");
    WEB_PRINTF(pReq, "xhr.open('GET',url,true);");
    WEB_PRINTF(pReq, "xhr.onreadystatechange=function(){");
    WEB_PRINTF(pReq, "if(xhr.readyState==4){");
    WEB_PRINTF(pReq, "if(xhr.status==200){");
    WEB_PRINTF(pReq, "var j=JSON.parse(xhr.responseText);");
    WEB_PRINTF(pReq, "if(j.status==='ok'||j.STATUS==='OK'){");
    WEB_PRINTF(pReq, "msg.textContent='Alarm '+id+' set: '+h+':'+m+' -> Outlet '+out+' '+(act?'ON':'OFF');");
    WEB_PRINTF(pReq, "msg.style.color='#0f1a2b';");
    WEB_PRINTF(pReq, "}else{");
    WEB_PRINTF(pReq, "msg.textContent='Failed: '+(j.msg||xhr.status);msg.style.color='#e03131';");
    WEB_PRINTF(pReq, "}}else{");
    WEB_PRINTF(pReq, "msg.textContent='Error: '+xhr.status;msg.style.color='#e03131';");
    WEB_PRINTF(pReq, "}}};");
    WEB_PRINTF(pReq, "xhr.send();");
    WEB_PRINTF(pReq, "}");
    
    WEB_PRINTF(pReq, "</script></body></html>");

    WEB_END(pReq);
    return SUCCESS;
}