#pragma STRING (far)
/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * Purpose : The handler of HTTP request to user function
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <salflash.h>
#include <salsys.h>

#include <web/web_api.h>
#include "web_user.h"
#include <syslog/syslogdefs.h>

/* 1 = the status handler sends GET_STATUS itself (recommended).
   0 = old behaviour: another function sends it when it sees start_pdu_status.
   If you keep 0, nothing here sends GET_STATUS. */
#define SEND_STATUS_IN_HANDLER  1

/* After this many polls without a reply, send GET_STATUS again */
#define STATUS_RESEND_POLLS     4

uint8 Get_Response[200];   /* UART line can be up to 180 chars */

/* Flags used by other files. Kept so the project still links. */
uint8 start_pdu_status = 0;
extern uint8 start_rtc_data;
extern uint8 start_alarm_data;

/* 1 = GET_STATUS sent, reply not received yet */
static uint8 status_waiting    = 0;
static uint8 status_wait_polls = 0;

/* Function Declaration */
void send_str_web(uint8* tempData);
void Fetching_Set_Response(char* tempData);


/*
 * get_user: user account page (username / password change)
 */
int32 get_user(struct http_request_s *pReq)
{
    uint8 username[SAL_FLSH_USR_USERNAME_LEN + 1];
    uint8 password[SAL_FLSH_USR_PASSWORD_LEN + 1];

    sal_flash_sysAccount_get(username, password);

    WEB_BEGIN(pReq);

    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: text/html");

    WEB_PRINTF(pReq, "<html>\n");
    WEB_PRINTF(pReq, "<head>\n");
    WEB_PRINTF(pReq, "<title>User Account Setting</title>\n");

    WEB_PRINTF(pReq, "<style>\n");
    WEB_PRINTF(pReq, "body{font-family:Arial,sans-serif;background:#f4f7fa;margin:0;padding:20px;}\n");
    WEB_PRINTF(pReq, "h1{text-align:center;color:#333;margin-bottom:20px;}\n");
    WEB_PRINTF(pReq, ".card{background:white;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,.1);padding:20px;max-width:500px;margin:0 auto;}\n");
    WEB_PRINTF(pReq, ".form-group{margin-bottom:15px;}\n");
    WEB_PRINTF(pReq, "label{display:block;margin-bottom:5px;font-weight:bold;color:#555;}\n");
    WEB_PRINTF(pReq, "input[type=text],input[type=password]{width:100%%;padding:8px;border:1px solid #ddd;border-radius:4px;box-sizing:border-box;}\n");
    WEB_PRINTF(pReq, ".btn{background:#1e88e5;color:white;border:none;padding:10px 15px;border-radius:4px;cursor:pointer;font-size:14px;}\n");
    WEB_PRINTF(pReq, ".btn:hover{background:#1565c0;}\n");
    WEB_PRINTF(pReq, "</style>\n");

    WEB_PRINTF(pReq, "<script>\n");
    WEB_PRINTF(pReq, "function ConfirmAccount(){\n");
    WEB_PRINTF(pReq, "var form=document.accountform;\n");
    WEB_PRINTF(pReq, "if(form.mpass.value!=form.mpass2.value){alert(\"Passwords do not match.\");return false;}\n");
    WEB_PRINTF(pReq, "return true;\n");
    WEB_PRINTF(pReq, "}\n");
    WEB_PRINTF(pReq, "</script>\n");
    WEB_PRINTF(pReq, "</head>\n");

    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<h1>User Account Setting</h1>\n");
    WEB_PRINTF(pReq, "<div class=\"card\">\n");
    WEB_PRINTF(pReq, "<form method=\"post\" name=\"accountform\" action=\"user.cgi\" onSubmit=\"return ConfirmAccount()\">\n");

    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"username\">New Username</label>\n");
    WEB_PRINTF(pReq, "<input type=\"text\" id=\"username\" name=\"mname\" maxlength=\"16\" value=\"%s\">\n", username);
    WEB_PRINTF(pReq, "</div>\n");

    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"password\">New Password</label>\n");
    WEB_PRINTF(pReq, "<input type=\"password\" id=\"password\" name=\"mpass\" maxlength=\"%d\">\n", (uint16)SAL_FLSH_USR_PASSWORD_LEN);
    WEB_PRINTF(pReq, "</div>\n");

    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"confirmPassword\">Confirm Password</label>\n");
    WEB_PRINTF(pReq, "<input type=\"password\" id=\"confirmPassword\" name=\"mpass2\" maxlength=\"%d\">\n", (uint16)SAL_FLSH_USR_PASSWORD_LEN);
    WEB_PRINTF(pReq, "</div>\n");

    WEB_PRINTF(pReq, "<div style=\"text-align:center;margin-top:20px;\">\n");
    WEB_PRINTF(pReq, "<input type=\"submit\" class=\"btn\" value=\"Apply Changes\">\n");
    WEB_PRINTF(pReq, "<input type=\"hidden\" name=\"cmd\" value=\"passwd\">\n");
    WEB_PRINTF(pReq, "</div>\n");

    WEB_PRINTF(pReq, "</form>\n");
    WEB_PRINTF(pReq, "</div>\n");
    WEB_PRINTF(pReq, "</body>\n");
    WEB_PRINTF(pReq, "</html>\n");

    WEB_END(pReq);

    return SUCCESS;
}


/*
 * post_user: saves username / password.
 * FIX: start from the current values, so a missing field is never
 * saved as uninitialised memory; copies are length-limited.
 */
int32 post_user(struct http_request_s *pReq)
{
    uint8  *pValue;
    uint8  username[SAL_FLSH_USR_USERNAME_LEN + 1];
    uint8  password[SAL_FLSH_USR_PASSWORD_LEN + 1];

    sal_flash_sysAccount_get(username, password);

    WEB_VAR(pReq, "mname", &pValue);
    if (pValue) {
        strncpy((char*)username, (char*)pValue, SAL_FLSH_USR_USERNAME_LEN);
        username[SAL_FLSH_USR_USERNAME_LEN] = '\0';
    }

    WEB_VAR(pReq, "mpass", &pValue);
    if (pValue) {
        strncpy((char*)password, (char*)pValue, SAL_FLSH_USR_PASSWORD_LEN);
        password[SAL_FLSH_USR_PASSWORD_LEN] = '\0';
    }

    sal_flash_sysAccount_set(username, password);

    syslog_add_new_buf_entry(SYSLOG_MSG_PASSWORD_CHANGED, 0);

    get_user(pReq);

    return SUCCESS;
}


/*
 * get_pdu_status
 * Returns the last good status frame (Get_Response).
 * FIX: sends GET_STATUS itself, but only when the previous request has
 * been answered (send_str_web() clears status_waiting). If no reply
 * comes for STATUS_RESEND_POLLS polls, the command is sent again.
 * This keeps MCU2's UART buffer from filling with queued polls.
 * It no longer clears start_rtc_data / start_alarm_data, so a status
 * poll cannot cancel an RTC or alarm request.
 */
int32 get_pdu_status(struct http_request_s *pReq)
{
#if SEND_STATUS_IN_HANDLER
    if (!status_waiting || ++status_wait_polls >= STATUS_RESEND_POLLS)
    {
        printf("GET_STATUS\r\n");
        status_waiting    = 1;
        status_wait_polls = 0;
    }
#else
    start_pdu_status = 1;      /* old path: another function sends it */
#endif

    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: application/json");
    WEB_PRINTF(pReq, "%s", Get_Response);
    WEB_END(pReq);
    return SUCCESS;
}


/*
 * send_str_web
 * Called by process_uart_string() for every VALID status frame.
 * Bounded copy into Get_Response, then marks the request as answered.
 */
void send_str_web(uint8* tempData)
{
    strncpy((char*)Get_Response, (char*)tempData, sizeof(Get_Response) - 1);
    Get_Response[sizeof(Get_Response) - 1] = '\0';

    status_waiting    = 0;
    status_wait_polls = 0;
}


/*
 * set_port_handler: one handler for all ports (1-8),
 * port number is taken from the URL path /set_portX
 */
int32 set_port_handler(struct http_request_s *pReq)
{
    int state = -1, i = 0;
    int port_num = -1;
    char *path = NULL;
    char state_str[8] = {0};
    char *qmark = NULL;

    if (pReq->pPath) {
        path = (char*)pReq->pPath;
    } else if (pReq->path) {
        path = (char*)pReq->path;
    }

    if (!path) {
        WEB_BEGIN(pReq);
        WEB_STATUS(pReq, HTTP_STATUS_BAD_REQUEST);
        WEB_HEADER(pReq, "Content-Type: text/plain");
        WEB_PRINTF(pReq, "Error: No path");
        WEB_END(pReq);
        return FAILED;
    }

    /* /set_portX -> X */
    if (strncmp(path, "/set_port", 9) == 0) {
        port_num = atoi(path + 9);
    }

    /* Method 1: query string in path */
    qmark = strchr(path, '?');
    if (qmark) {
        char *state_ptr = strstr(qmark, "state=");
        if (state_ptr) {
            state = atoi(state_ptr + 6);
        }
    }

    /* Method 2: variables already parsed by the server */
    if (state == -1 && pReq->pVar_Temp) {
        char *var_buffer = (char*)pReq->pVar_Temp;

        for (i = 0; i < pReq->Var_Count; i++) {
            char *name = var_buffer + pReq->Var[i].name_offset;
            char *value = var_buffer + pReq->Var[i].value_offset;
            int value_len = pReq->Var[i].value_length;

            if (strcmp(name, "state") == 0) {
                int len = (value_len < 7) ? value_len : 7;
                strncpy(state_str, value, len);
                state_str[len] = '\0';
                state = atoi(state_str);
                break;
            }
        }
    }

    /* Method 3: POST body */
    if (state == -1 && pReq->pBody) {
        char *state_ptr = strstr((char*)pReq->pBody, "state=");
        if (state_ptr) {
            state = atoi(state_ptr + 6);
        }
    }

    if (state != 0 && state != 1) {
        WEB_BEGIN(pReq);
        WEB_STATUS(pReq, HTTP_STATUS_BAD_REQUEST);
        WEB_HEADER(pReq, "Content-Type: application/json");
        WEB_PRINTF(pReq, "{\"error\":\"Invalid state (must be 0 or 1)\",\"received\":%d}", state);
        WEB_END(pReq);
        return FAILED;
    }

    if (port_num < 1 || port_num > 8) {
        WEB_BEGIN(pReq);
        WEB_STATUS(pReq, HTTP_STATUS_BAD_REQUEST);
        WEB_HEADER(pReq, "Content-Type: application/json");
        WEB_PRINTF(pReq, "{\"error\":\"Invalid port number (must be 1-8)\",\"received\":%d}", port_num);
        WEB_END(pReq);
        return FAILED;
    }

    /* Send command to MCU2 */
    printf("SET_OUTLET:%d STATE:%d\r\n", port_num, state);

    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: application/json");
    WEB_PRINTF(pReq, "{\"port\":%d,\"state\":%d,\"status\":\"ok\"}\r\n", port_num, state);
    WEB_END(pReq);

    return SUCCESS;
}