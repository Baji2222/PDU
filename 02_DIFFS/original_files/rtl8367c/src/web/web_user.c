#pragma STRING (far)
/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : The handler of HTTP request to user function
 *
 * Feature : Response the request related to user function
 *
 */

/*
 * Include Files
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <salflash.h>
#include <salsys.h>

#include <web/web_api.h>
#include "web_user.h"
#include <syslog/syslogdefs.h>

uint8 Get_Response[75];
/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

/*
 * Function Declaration
 */
void send_str_web(uint8* tempData);
void Fetching_Set_Response(char* tempData);
/*
 * Function: get_user
 * ------------------
 * Displays user account management page for username/password changes.
 * Shows current username and password fields with confirmation.
 * Uses JavaScript to validate password match before submission.
 * Modern clean UI with form styling and validation.
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
    
    // Minimal CSS
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
    
    // Simplified JavaScript validation
    WEB_PRINTF(pReq, "<script>\n");
    WEB_PRINTF(pReq, "function ConfirmAccount(){\n");
    WEB_PRINTF(pReq, "var form=document.accountform;\n");
    WEB_PRINTF(pReq, "if(form.mpass.value!=form.mpass2.value){alert(\"Passwords do not match.\");return false;}\n");
    WEB_PRINTF(pReq, "return true;\n");
    WEB_PRINTF(pReq, "}\n");
    WEB_PRINTF(pReq, "</script>\n");
    WEB_PRINTF(pReq, "</head>\n");
    
    // Modern body
    WEB_PRINTF(pReq, "<body>\n");
    WEB_PRINTF(pReq, "<h1>User Account Setting</h1>\n");
    WEB_PRINTF(pReq, "<div class=\"card\">\n");
    WEB_PRINTF(pReq, "<form method=\"post\" name=\"accountform\" action=\"user.cgi\" onSubmit=\"return ConfirmAccount()\">\n");
    
    // Username field
    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"username\">New Username</label>\n");
    WEB_PRINTF(pReq, "<input type=\"text\" id=\"username\" name=\"mname\" maxlength=\"16\" value=\"%s\">\n", username);
    WEB_PRINTF(pReq, "</div>\n");
    
    // Password field
    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"password\">New Password</label>\n");
    WEB_PRINTF(pReq, "<input type=\"password\" id=\"password\" name=\"mpass\" maxlength=\"%d\">\n", (uint16)SAL_FLSH_USR_PASSWORD_LEN);
    WEB_PRINTF(pReq, "</div>\n");
    
    // Confirm Password field
    WEB_PRINTF(pReq, "<div class=\"form-group\">\n");
    WEB_PRINTF(pReq, "<label for=\"confirmPassword\">Confirm Password</label>\n");
    WEB_PRINTF(pReq, "<input type=\"password\" id=\"confirmPassword\" name=\"mpass2\" maxlength=\"%d\">\n", (uint16)SAL_FLSH_USR_PASSWORD_LEN);
    WEB_PRINTF(pReq, "</div>\n");
    
    // Submit button
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
 * Function: post_user
 * -------------------
 * Processes user account form submission.
 * Extracts username and password from POST data.
 * Calls sal_flash_sysAccount_set() to save credentials.
 * Logs password change event to syslog.
 */
int32 post_user(struct http_request_s *pReq)
{
    uint8  *pValue;
    uint8  username[SAL_FLSH_USR_USERNAME_LEN + 1];
    uint8  password[SAL_FLSH_USR_PASSWORD_LEN + 1];

    /* get variables */
    WEB_VAR(pReq, "mname", &pValue);
    if (pValue) {
        //memcpy(username, pValue, SAL_FLSH_USR_USERNAME_LEN);
        strcpy(username, pValue);
    }
    
    WEB_VAR(pReq, "mpass", &pValue);
    if (pValue) {
        //memcpy(password, pValue, SAL_FLSH_USR_PASSWORD_LEN);
        strcpy(password, pValue);
    }

    /* Save user account */
    sal_flash_sysAccount_set(username, password);

	syslog_add_new_buf_entry(SYSLOG_MSG_PASSWORD_CHANGED, 0);
		
    get_user(pReq);

    return SUCCESS;
}

uint8 start_pdu_status = 0;
extern uint8 start_rtc_data;
extern uint8 start_alarm_data;

int32 get_pdu_status(struct http_request_s *pReq)
{
		start_alarm_data = 0;
		start_rtc_data = 0;
    start_pdu_status = 1;
		//printf("GET_STATUS\r\n");
    WEB_BEGIN(pReq);
    WEB_STATUS(pReq, HTTP_STATUS_OK);
    WEB_HEADER(pReq, "Content-Type: application/json");
    WEB_PRINTF(pReq, "%s",Get_Response);
    WEB_END(pReq);
    return SUCCESS;
}
/*
 * Function: send_str_web
 * ----------------------
 * Stores string data for web response in global buffer.
 * Copies tempData to Get_Response for later use.
 * Called by UART processing to pass data to web interface.
 */
void send_str_web(uint8* tempData)
{
		strcpy((char*)Get_Response, tempData);
}

//void Fetching_Set_Response(char* tempData)
//{
//		strcpy(Set_Response, tempData);
//}


/*
 * Function: set_port_handler
 * --------------------------
 * Single handler for all ports (1-8) - extracts port number from URL path
 */

int32 set_port_handler(struct http_request_s *pReq)
{
    int state = -1, counts = 0,i=0;
    int port_num = -1;
    char *path = NULL;
    char state_str[8] = {0};
    char *qmark = NULL;
    int retry_count = 0;
    int max_retries = 50; // 50 * 10ms = 500ms timeout
    
    // Get the path - try pPath first, then path array
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
    
    // Extract port number from path: /set_portX -> X
    if (strncmp(path, "/set_port", 9) == 0) {
        port_num = atoi(path + 9);
    }
    
    // Method 1: Try parsing from query string in path
    qmark = strchr(path, '?');
    if (qmark) {
        char *state_ptr = strstr(qmark, "state=");
        if (state_ptr) {
            state = atoi(state_ptr + 6);
        }
    }
    
    // Method 2: Try pVar_Temp (if the server parsed it already)
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
    
    // Method 3: Try pBody (for POST requests)
    if (state == -1 && pReq->pBody) {
        char *state_ptr = strstr((char*)pReq->pBody, "state=");
        if (state_ptr) {
            state = atoi(state_ptr + 6);
        }
    }
    
    // Validate state
    if (state != 0 && state != 1) {
        WEB_BEGIN(pReq);
        WEB_STATUS(pReq, HTTP_STATUS_BAD_REQUEST);
        WEB_HEADER(pReq, "Content-Type: application/json");
        WEB_PRINTF(pReq, "{\"error\":\"Invalid state (must be 0 or 1)\",\"received\":%d}", state);
        WEB_END(pReq);
        return FAILED;
    }
    
    // Validate port number
    if (port_num < 1 || port_num > 8) {
        WEB_BEGIN(pReq);
        WEB_STATUS(pReq, HTTP_STATUS_BAD_REQUEST);
        WEB_HEADER(pReq, "Content-Type: application/json");
        WEB_PRINTF(pReq, "{\"error\":\"Invalid port number (must be 1-8)\",\"received\":%d}", port_num);
        WEB_END(pReq);
        return FAILED;
    }
    
    // Send command to MCU
    printf("SET_OUTLET:%d STATE:%d\r\n", port_num, state);


		WEB_BEGIN(pReq);
		WEB_STATUS(pReq, HTTP_STATUS_OK);
		WEB_HEADER(pReq, "Content-Type: application/json");
		WEB_PRINTF(pReq, "{\"port\":%d,\"state\":%d,\"status\":\"ok\"}\r\n",port_num,state);
		//WEB_PRINTF(pReq, "%s",Set_Response);
		WEB_END(pReq);

    
    return SUCCESS;
}







