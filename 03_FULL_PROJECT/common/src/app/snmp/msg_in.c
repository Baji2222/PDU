#pragma STRING(far)
#include <string.h>
#include <stdio.h>
#include <local/snmp_config.h>
#include <snmp/opt.h>
#if LWIP_SNMP
#include <snmp/mem.h>
#include <snmp/snmp.h>
#include <snmp/snmp_asn1.h>
#include <snmp/snmp_msg.h>
#include <snmp/snmp_structs.h>
#include <lwps/lwps_api.h>
#include <salflash.h>
#include <string.h>
#include <snmp/snmp_debug.h>
s32_t snmp_version = 0;
far const char snmp_publiccommunity[SNMP_COMMUNITY_STR_LEN + 1] = "public";
far const char snmp_privatecommunity[SNMP_COMMUNITY_STR_LEN + 1] = "private";
char snmp_read_community[SNMP_COMMUNITY_STR_LEN + 1];
char snmp_write_community[SNMP_COMMUNITY_STR_LEN + 1];
struct snmp_msg_pstat msg_input_list[SNMP_CONCURRENT_REQUESTS];
struct vb_ident_list_s xdata vb_ident_list;
struct vb_mem_buf_list_s xdata vb_mem_buf_list;
struct vb_value_list_s xdata vb_value_list;
struct udp_pcb *snmp1_pcb;
void snmp_recv(int32 socketId, uint8 *pData, uint32 length, ip_addr_t ip, uint16 port) reentrant;
static err_t snmp_pdu_header_check(u8_t *p, u16_t ofs, u16_t pdu_len, u16_t *ofs_ret, struct snmp_msg_pstat *m_stat);
static err_t snmp_pdu_dec_varbindlist(u8_t *p, u16_t ofs, u16_t *ofs_ret, struct snmp_msg_pstat *m_stat);
extern int32 lwps_sendto(int32 socketId, uint8 *send_data, int32 length, ip_addr_t ip, uint16 port) large;

extern u8_t portPoe;
extern u8_t portLan;

/* Global JSON string for SNMP responses - Default value, adjustable size */
char global_json_string[256] = "{\"status\": \"initial\"}";

/*
 * Function: snmp_write_community_set
 * ----------------------------------
 * Sets the SNMP write community string.
 * Copies provided community into global write community buffer.
 * Used for SNMPv2c write access control.
 */
void snmp_write_community_set(char *pCommunity)
{
    strncpy(snmp_write_community, pCommunity, SNMP_COMMUNITY_STR_LEN);
}

/*
 * Function: snmp_read_community_set
 * ---------------------------------
 * Sets the SNMP read community string.
 * Copies provided community into global read community buffer.
 * Used for SNMPv2c read access control.
 */
void snmp_read_community_set(char *pCommunity)
{
    strncpy(snmp_read_community, pCommunity, SNMP_COMMUNITY_STR_LEN);
}

/**
 * Starts SNMP Agent.
 * Allocates UDP pcb and binds it to IP_ADDR_ANY port 161.
 */
/*
 * Function: snmp_init
 * -------------------
 * Initializes SNMP agent and starts listening on UDP port 161.
 * Allocates UDP socket, binds to port, and registers receive callback.
 * Initializes message processing state and community strings.
 * Sends coldstart trap if SNMP trap is enabled.
 */
void snmp_init(void)
{
    struct snmp_msg_pstat *msg_ps;
#if LWPS_SNMP_TRAP_ENABLE
    u8_t trap_dip[4] = {LWPS_SNMP_TRAP_IP};
#endif /* end of LWPS_SNMP_TRAP_ENABLE */
    int32 socketId;
    u8_t i;
    snmpHandler.event_connect = NULL;
    snmpHandler.event_disconnect = NULL;
    snmpHandler.event_recv = NULL;
    snmpHandler.event_recvfrom = snmp_recv;
    socketId = lwps_socket(LWPS_L3TYPE_IP, LWPS_L4TYPE_UDP, snmpHandler);

    if (socketId < 0)
    {
        OSAL_PRINTF("snmp: Error - alloc socket failed!\n");
        return;
    }
    if (LWPS_OK != lwps_bind(socketId, ip_addr_any, SNMP_IN_PORT))
    {
        OSAL_PRINTF("snmp: Error - bind failed!\n");
        lwps_close(socketId);
        return;
    }
    msg_ps = &msg_input_list[0];
    for (i = 0; i < SNMP_CONCURRENT_REQUESTS; i++)
    {
        msg_ps->state = SNMP_MSG_EMPTY;
        msg_ps->error_index = 0;
        msg_ps->error_status = SNMP_ES_NOERROR;
        msg_ps->invb.count = 0;
        msg_ps->invb.tail = NULL;
        msg_ps->invb.head = NULL;
        msg_ps->outvb.count = 0;
        msg_ps->outvb.tail = NULL;
        msg_ps->outvb.head = NULL;
        msg_ps++;
    }
    snmp_version = 0;
    memset(snmp_read_community, 0, sizeof(snmp_read_community));
    memset(snmp_write_community, 0, sizeof(snmp_write_community));
#if 1 /*Fetch read & write community from flash*/
      //	sal_flash_snmp_community_get(snmp_read_community, snmp_write_community);
      //	sal_snmp_community_get(snmp_publiccommunity, snmp_privatecommunity);
    strncpy(snmp_read_community, snmp_publiccommunity, strlen(snmp_publiccommunity));
    strncpy(snmp_write_community, snmp_privatecommunity, strlen(snmp_privatecommunity));
#endif
#if LWPS_SNMP_TRAP_ENABLE
    /* The coldstart trap will only be output
       if our outgoing interface is up & configured  */
    sal_flash_snmp_trapIp_get(&trap_dip);
    snmp_trap_dst_ip_set(0, trap_dip);
    snmp_trap_dst_enable(0, 1);
    snmp_coldstart_trap();
    snmp_trap_set_enable();
#endif /* end of LWPS_SNMP_TRAP_ENABLE */
    vb_mem_buf_list.freeIndex = 0;
    vb_ident_list.freeIndex = 0;
    vb_value_list.freeIndex = 0;
}

/*
 * Function: snmp_error_response
 * -----------------------------
 * Sends SNMP error response with specified error code.
 * Frees output varbind list and uses input varbinds as response.
 * Sets error status and index, then sends response.
 */
static void
snmp_error_response(struct snmp_msg_pstat *msg_ps, u8_t error)
{
    snmp_varbind_list_free(&msg_ps->outvb);
    msg_ps->outvb = msg_ps->invb;
    msg_ps->invb.head = NULL;
    msg_ps->invb.tail = NULL;
    msg_ps->invb.count = 0;
    msg_ps->error_status = error;
    msg_ps->error_index = 1 + msg_ps->vb_idx;
    snmp_send_response(msg_ps);
    snmp_varbind_list_free(&msg_ps->outvb);
    msg_ps->state = SNMP_MSG_EMPTY;
}

/*
 * Function: snmp_error_response_v2
 * --------------------------------
 * Sends SNMPv2c error response with exception handling.
 * Handles noSuchInstance, noSuchObject, and tooBig exceptions.
 * Supports partial success with error status in v2c format.
 */
static void snmp_error_response_v2(struct snmp_msg_pstat *msg_ps, u8_t error, u8_t use_invb)
{
    /*in v2c, if read one variable is failed, do not clear other successful variable.
     */
    msg_ps->error_status = error;
    msg_ps->error_index = 1 + msg_ps->vb_idx;
    /*V2C use nosuchinstance and no such object exception. */
    if (error == SNMP_ES_NOSUCHNAME)
    {
        msg_ps->error_status = 0;
        msg_ps->error_index = 0;
    }
    else if (error == SNMP_ES_TOOBIG)
    {
        msg_ps->error_index = 0;
        /*too big error response has a null binding list */
        snmp_varbind_list_free(&msg_ps->outvb);
        snmp_varbind_list_free(&msg_ps->invb);
        snmp_send_response(msg_ps);
        msg_ps->state = SNMP_MSG_EMPTY;
        return;
    }

    if (use_invb == 1)
    {
        snmp_varbind_list_free(&msg_ps->outvb);
        // msg_ps->outvb = msg_ps->invb;
        msg_ps->invb.head = NULL;
        msg_ps->invb.tail = NULL;
        msg_ps->invb.count = 0;
        msg_ps->outvb = msg_ps->invb;
    }
    snmp_send_response(msg_ps);
    snmp_varbind_list_free(&msg_ps->invb);
    snmp_varbind_list_free(&msg_ps->outvb);
    msg_ps->state = SNMP_MSG_EMPTY;
}

/*
 * Function: snmp_ok_response
 * --------------------------
 * Sends successful SNMP response with processed varbinds.
 * Sends response via UDP and frees both input and output varbinds.
 * Sets message state to EMPTY after completion.
 */
static void
snmp_ok_response(struct snmp_msg_pstat *msg_ps)
{
    err_t err_ret;

    err_ret = snmp_send_response(msg_ps);
    if (err_ret == ERR_MEM)
    {
        /* serious memory problem, can't return tooBig */
    }
    else
    {
        DBG_SNMP_PRINTF(("[%s][%d]snmp_msg_event = %" S32_F "\n", __FILE__, (uint16)__LINE__, msg_ps->error_status));
    }
    /* free varbinds (if available) */
    snmp_varbind_list_free(&msg_ps->invb);
    snmp_varbind_list_free(&msg_ps->outvb);
    msg_ps->state = SNMP_MSG_EMPTY;
}

/*
 * Function: parse_json_field
 * --------------------------
 * Parses a named field from JSON string and extracts integer value.
 * Searches for pattern "\"field\":" and extracts following number.
 * Returns 1 on success, -1 if field not found.
 */
static int parse_json_field(const char *json, const char *field, s32_t *value) {
    char pattern[32];
    const char *start;

    // Create pattern like "\"A\":"
    sprintf(pattern,"\"%s\":", field);
    // Find the pattern in the JSON string
    start = strstr(json, pattern);
    if (!start) return -1;

    // Move pointer to the value (after ":")
    start += strlen(pattern);

    // Skip any optional spaces
    while (*start == ' ' || *start == '\t') start++;

    // Extract the integer value
    *value = atoi(start);
    return 1;
}

/**
 * Service an internal or external event for SNMP GET.
 *
 * @param request_id identifies requests from 0 to (SNMP_CONCURRENT_REQUESTS-1)
 * @param msg_ps points to the associated message process state
 */
/*
 * Function: snmp_msg_get_event
 * ----------------------------
 * Handles SNMP GET request processing.
 * Processes custom OIDs for JSON fields (AC, Battery, Voltage, etc.).
 * Handles TCP/UDP statistics OIDs via external function calls.
 * Builds response varbind list with retrieved values.
 */
static void snmp_msg_get_event(u8_t request_id, struct snmp_msg_pstat *msg_ps)
{
    int i, j;
    // Define custom OIDs for GET operations
    // - JSON fields use private enterprise number 1.3.6.1.4.1.12345.1.x (replace 12345 with your IANA-assigned number)
    // - TCP metrics use TCP-MIB (1.3.6.1.2.1.6)
    // - UDP metrics use UDP-MIB (1.3.6.1.2.1.7)
    s32_t custom_oids[13][10] = {
        {1, 3, 6, 1, 4, 1, 63663, 1, 0, 0}, // acStatus (A: AC power status- not available)
				
        {1, 3, 6, 1, 4, 1, 63663, 1, 1, 0}, // batteryStatus (B: Battery status)
				
        {1, 3, 6, 1, 4, 1, 63663, 1, 2, 0}, // batteryPercentage (C: Battery percentage)
        {1, 3, 6, 1, 4, 1, 63663, 1, 3, 0}, // voltageMode (D: Voltage mode-not available)
        {1, 3, 6, 1, 4, 1, 63663, 1, 4, 0}, // voltageValue (E: Voltage value)
				
				{1, 3, 6, 1, 4, 1, 63663, 1, 5, 0},//--> for poe port link status
				{1, 3, 6, 1, 4, 1, 63663, 1, 6, 0},//--> for lan port link status
				
        //{1, 3, 6, 1, 2, 1, 6, 5, 0, 0},     // tcpActiveOpens.0 (TCP active connections)
        //{1, 3, 6, 1, 2, 1, 6, 6, 0, 0},     // tcpPassiveOpens.0 (TCP passive connections)
        {1, 3, 6, 1, 2, 1, 6, 7, 0, 0},     // tcpAttemptFails.0 (TCP attempt fails)
        {1, 3, 6, 1, 2, 1, 6, 8, 0, 0},     // tcpEstabResets.0 (TCP established resets)
        {1, 3, 6, 1, 2, 1, 7, 1, 0, 0},     // udpInDatagrams.0 (UDP datagrams delivered)
        {1, 3, 6, 1, 2, 1, 7, 2, 0, 0},     // udpNoPorts.0 (UDP datagrams with no application)
        {1, 3, 6, 1, 2, 1, 7, 3, 0, 0},     // udpInErrors.0 (UDP datagrams failed other)
        {1, 3, 6, 1, 2, 1, 7, 4, 0, 0}      // udpOutDatagrams.0 (UDP datagrams sent)
    };
    u8_t custom_oid_len[13] = {10, 10, 10, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9}; // Lengths for each OID
    int match = -1;

    // Handle SNMP message states
    switch (msg_ps->state)
    {
    case SNMP_MSG_EXTERNAL_GET_OBJDEF:
    {
        // Retrieve object definition for external node
        struct snmp_name_ptr np;
        struct mib_external_node *en;
        np = msg_ps->ext_name_ptr;
        en = msg_ps->ext_mib_node;
        en->get_object_def_a(request_id, np.ident_len, np.ident, &msg_ps->ext_object_def);

        if (msg_ps->ext_object_def.instance != MIB_OBJECT_NONE)
        {
            msg_ps->state = SNMP_MSG_EXTERNAL_GET_VALUE;
            en->get_value_q(request_id, &msg_ps->ext_object_def);
        }
        else
        {
            en->get_object_def_pc(request_id, np.ident_len, np.ident);
            snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
        }
        return;
    }

    case SNMP_MSG_EXTERNAL_GET_VALUE:
    {
        // Allocate and initialize variable binding for value response
        struct mib_external_node *en = msg_ps->ext_mib_node;
        struct snmp_varbind *vb;
        GET_FREE_VB_MEM_BUF(vb);
        memset(vb, 0x00, sizeof(struct snmp_varbind));
        LWIP_ASSERT("vb != NULL", vb != NULL);

        if (vb == NULL)
        {
            en->get_value_pc(request_id, &msg_ps->ext_object_def);
            DBG_SNMP_PRINTF(("snmp_msg_event: no outvb space\n"));
            snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
            return;
        }

        vb->next = NULL;
        vb->prev = NULL;
        vb->ident = msg_ps->vb_ptr->ident;
        vb->ident_len = msg_ps->vb_ptr->ident_len;
        msg_ps->vb_ptr->ident = NULL;
        msg_ps->vb_ptr->ident_len = 0;
        vb->value_type = msg_ps->ext_object_def.asn_type;
        vb->value_len = msg_ps->ext_object_def.v_len;

        if (vb->value_len > 0)
        {
            GET_FREE_VB_VAL_BUF(vb->value);
            memset(vb->value, 0x00, sizeof(struct vb_value_buf_s));
            LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

            if (vb->value != NULL)
            {
                en->get_value_a(request_id, &msg_ps->ext_object_def, vb->value_len, vb->value);
                snmp_varbind_tail_add(&msg_ps->outvb, vb);
                msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                msg_ps->vb_idx += 1;
            }
            else
            {
                en->get_value_pc(request_id, &msg_ps->ext_object_def);
                DBG_SNMP_PRINTF(("snmp_msg_event: no variable space\n"));
                msg_ps->vb_ptr->ident = vb->ident;
                msg_ps->vb_ptr->ident_len = vb->ident_len;
                snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
            }
        }
        else
        {
            en->get_value_a(request_id, &msg_ps->ext_object_def, 0, NULL);
            vb->value = NULL;
            snmp_varbind_tail_add(&msg_ps->outvb, vb);
            msg_ps->state = SNMP_MSG_SEARCH_OBJ;
            msg_ps->vb_idx += 1;
        }
        return;
    }

    default:
        break;
    }

    // Process variable bindings in SEARCH_OBJ state
    while ((msg_ps->state == SNMP_MSG_SEARCH_OBJ) && (msg_ps->vb_idx < msg_ps->invb.count))
    {
        struct snmp_varbind *vb;

        // Update variable binding pointer based on index
        if (msg_ps->vb_idx == 0)
        {
            msg_ps->vb_ptr = msg_ps->invb.head;
        }
        else
        {
            msg_ps->vb_ptr = msg_ps->vb_ptr->next;
        }

        // Check for matching custom OID
        match = -1;
        for (j = 0; j < 13; j++)
        {
            if (msg_ps->vb_ptr->ident_len == custom_oid_len[j])
            {
                int oid_match = 1;
                for (i = 0; i < custom_oid_len[j]; i++)
                {
                    if (msg_ps->vb_ptr->ident[i] != custom_oids[j][i])
                    {
                        oid_match = 0;
                        break;
                    }
                }
                if (oid_match)
                {
                    match = j;
                    break;
                }
            }
        }

        if (match >= 0)
        {
            // Allocate and initialize variable binding for response
            GET_FREE_VB_MEM_BUF(vb);
            memset(vb, 0x00, sizeof(struct snmp_varbind));
            LWIP_ASSERT("vb != NULL", vb != NULL);

            if (vb == NULL)
            {
                DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate outvb space\n"));
                snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                return;
            }

            vb->next = NULL;
            vb->prev = NULL;
            vb->ident = msg_ps->vb_ptr->ident;
            vb->ident_len = msg_ps->vb_ptr->ident_len;
            msg_ps->vb_ptr->ident = NULL;
            msg_ps->vb_ptr->ident_len = 0;

            // Process based on matched OID
            switch (match)
            {
            case 0: // acStatus (1.3.6.1.4.1.12345.1.1.0)
            {
                s32_t value;
                const char *status_text;
                char result_str[50];
                
                if (parse_json_field(global_json_string, "A", &value))
                {
                    // Convert value to descriptive text
                    if (value == 0) {
                        status_text = "0 - Offline (No AC Power)";
                    } else if (value == 1) {
                        status_text = "1 - Online (AC Power Connected)";
                    } else {
                        status_text = "Unknown AC Status";
                    }
                    
                    strcpy(result_str, status_text);
                    
                    vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR;
                    vb->value_len = strlen(result_str);
                    GET_FREE_VB_VAL_BUF(vb->value);
                    LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                    if (vb->value != NULL)
                    {
                        memcpy(vb->value, result_str, vb->value_len);
                        snmp_varbind_tail_add(&msg_ps->outvb, vb);
                        msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                        msg_ps->vb_idx += 1;
                    }
                    else
                    {
                        DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                        msg_ps->vb_ptr->ident = vb->ident;
                        msg_ps->vb_ptr->ident_len = vb->ident_len;
                        snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                    }
                }
                else
                {
                    snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
                }
                break;
            }
            case 1: // batteryStatus (1.3.6.1.4.1.12345.1.2.0)
            {
                s32_t value;
                const char *status_text;
                char result_str[50];
                
                if (parse_json_field(global_json_string, "B", &value))
                {
                    // Convert value to descriptive text
                    if (value == 0) {
                        status_text = "0 - Inactive (Battery Not Working)";
                    } else if (value == 1) {
                        status_text = "1 - Active (Battery Operational)";
                    } else {
                        status_text = "Unknown Battery Status";
                    }
                    
                    strcpy(result_str, status_text);
                    
                    vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR;
                    vb->value_len = strlen(result_str);
                    GET_FREE_VB_VAL_BUF(vb->value);
                    LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                    if (vb->value != NULL)
                    {
                        memcpy(vb->value, result_str, vb->value_len);
                        snmp_varbind_tail_add(&msg_ps->outvb, vb);
                        msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                        msg_ps->vb_idx += 1;
                    }
                    else
                    {
                        DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                        msg_ps->vb_ptr->ident = vb->ident;
                        msg_ps->vb_ptr->ident_len = vb->ident_len;
                        snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                    }
                }
                else
                {
                    snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
                }
                break;
            }
            case 2: // batteryPercentage (1.3.6.1.4.1.12345.1.3.0)
            {
                s32_t value;
                char result_str[60];
                
                if (parse_json_field(global_json_string, "C", &value))
                {
                    // Format battery percentage with description
                    if (value >= 80) {
                        sprintf(result_str, "%d%% - Fully Charged", (int)value);
                    } else if (value >= 50) {
                        sprintf(result_str, "%d%% - Good Charge", (int)value);
                    } else if (value >= 20) {
                        sprintf(result_str, "%d%% - Low Battery", (int)value);
                    } else if (value > 0) {
                        sprintf(result_str, "%d%% - Critical Battery", (int)value);
                    } else {
                        sprintf(result_str, "0%% - Empty or Not Present");
                    }
                    
                    vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR;
                    vb->value_len = strlen(result_str);
                    GET_FREE_VB_VAL_BUF(vb->value);
                    LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                    if (vb->value != NULL)
                    {
                        memcpy(vb->value, result_str, vb->value_len);
                        snmp_varbind_tail_add(&msg_ps->outvb, vb);
                        msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                        msg_ps->vb_idx += 1;
                    }
                    else
                    {
                        DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                        msg_ps->vb_ptr->ident = vb->ident;
                        msg_ps->vb_ptr->ident_len = vb->ident_len;
                        snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                    }
                }
                else
                {
                    snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
                }
                break;
            }
            case 3: // voltageMode (1.3.6.1.4.1.12345.1.4.0)
            {
                s32_t value;
                if (parse_json_field(global_json_string, "D", &value))
                {
                    vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG;
                    vb->value_len = sizeof(value);
                    GET_FREE_VB_VAL_BUF(vb->value);
                    LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                    if (vb->value != NULL)
                    {
                        memcpy(vb->value, &value, sizeof(value));
                        snmp_varbind_tail_add(&msg_ps->outvb, vb);
                        msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                        msg_ps->vb_idx += 1;
                    }
                    else
                    {
                        DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                        msg_ps->vb_ptr->ident = vb->ident;
                        msg_ps->vb_ptr->ident_len = vb->ident_len;
                        snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                    }
                }
                else
                {
                    snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
                }
                break;
            }
            case 4: // voltageValue (1.3.6.1.4.1.12345.1.5.0)
            {
                s32_t value;
                char result_str[60];
                
                if (parse_json_field(global_json_string, "E", &value))
                {
                    // Format voltage with description
                    if (value >= 1000) {
                        // Assume millivolts, convert to volts
                        int volts = value / 1000;
                        int millivolts = value % 1000;
                        sprintf(result_str, "%d.%03dV - Voltage Reading", volts, millivolts);
                    } else {
                        // Assume already in volts
                        sprintf(result_str, "%dV - Voltage Reading", (int)value);
                    }

                    vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR;
                    vb->value_len = strlen(result_str);
                    GET_FREE_VB_VAL_BUF(vb->value);
                    LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                    if (vb->value != NULL)
                    {
                        memcpy(vb->value, result_str, vb->value_len);
                        snmp_varbind_tail_add(&msg_ps->outvb, vb);
                        msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                        msg_ps->vb_idx += 1;
                    }
                    else
                    {
                        DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                        msg_ps->vb_ptr->ident = vb->ident;
                        msg_ps->vb_ptr->ident_len = vb->ident_len;
                        snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                    }
                }
                else
                {
                    snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
                }
                break;
            }
            case 5: // PoE port link status (1.3.6.1.4.1.63663.1.5.0)
            {
                s32_t count_s32 = (s32_t)portPoe;
                char result_str[60];
                
                // Format PoE port status with description
                if (count_s32 == 0) {
                    sprintf(result_str, "0 - PoE Port Link Down");
                } else if (count_s32 == 1) {
                    sprintf(result_str, "1 - PoE Port Link Up");
                } else {
                    sprintf(result_str, "%d - PoE Port Status", (int)count_s32);
                }

                vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR;
                vb->value_len = strlen(result_str);
                GET_FREE_VB_VAL_BUF(vb->value);
                LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                if (vb->value != NULL)
                {
                    memcpy(vb->value, result_str, vb->value_len);
                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                    msg_ps->vb_idx += 1;
                }
                else
                {
                    DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                    msg_ps->vb_ptr->ident = vb->ident;
                    msg_ps->vb_ptr->ident_len = vb->ident_len;
                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                }
                break;
            }
            case 6:  // LAN port link status (1.3.6.1.4.1.63663.1.6.0)
      
            {
                s32_t count_s32 = (s32_t)portLan;
                char result_str[60];
                
                // Format LAN port status with description
                if (count_s32 == 0) {
                    sprintf(result_str, "0 - LAN Port Link Down");
                } else if (count_s32 == 1) {
                    sprintf(result_str, "1 - LAN Port Link Up");
                } else {
                    sprintf(result_str, "%d - LAN Port Status", (int)count_s32);
                }
                
                vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR;
                vb->value_len = strlen(result_str);
                GET_FREE_VB_VAL_BUF(vb->value);
                LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                if (vb->value != NULL)
                {
                    memcpy(vb->value, result_str, vb->value_len);
                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                    msg_ps->vb_idx += 1;
                }
                else
                {
                    DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                    msg_ps->vb_ptr->ident = vb->ident;
                    msg_ps->vb_ptr->ident_len = vb->ident_len;
                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                }
                break;
            }
            case 7: // tcpAttemptFails.0 (1.3.6.1.2.1.6.7.0)
            {
                uint16 count = tcp_get_attempt_fails();
                s32_t count_s32 = (s32_t)count;
                vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER;
                vb->value_len = sizeof(count_s32);
                GET_FREE_VB_VAL_BUF(vb->value);
                LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                if (vb->value != NULL)
                {
                    memcpy(vb->value, &count_s32, sizeof(count_s32));
                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                    msg_ps->vb_idx += 1;
                }
                else
                {
                    DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                    msg_ps->vb_ptr->ident = vb->ident;
                    msg_ps->vb_ptr->ident_len = vb->ident_len;
                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                }
                break;
            }
            case 8: // tcpEstabResets.0 (1.3.6.1.2.1.6.8.0)
            {
                uint16 count = tcp_get_estab_resets();
                s32_t count_s32 = (s32_t)count;
                vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER;
                vb->value_len = sizeof(count_s32);
                GET_FREE_VB_VAL_BUF(vb->value);
                LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                if (vb->value != NULL)
                {
                    memcpy(vb->value, &count_s32, sizeof(count_s32));
                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                    msg_ps->vb_idx += 1;
                }
                else
                {
                    DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                    msg_ps->vb_ptr->ident = vb->ident;
                    msg_ps->vb_ptr->ident_len = vb->ident_len;
                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                }
                break;
            }
            case 9: // udpInDatagrams.0 (1.3.6.1.2.1.7.1.0)
            {
                uint16 count = udp_get_datagrams_delivered();
                s32_t count_s32 = (s32_t)count;
                vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER;
                vb->value_len = sizeof(count_s32);
                GET_FREE_VB_VAL_BUF(vb->value);
                LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                if (vb->value != NULL)
                {
                    memcpy(vb->value, &count_s32, sizeof(count_s32));
                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                    msg_ps->vb_idx += 1;
                }
                else
                {
                    DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                    msg_ps->vb_ptr->ident = vb->ident;
                    msg_ps->vb_ptr->ident_len = vb->ident_len;
                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                }
                break;
            }
            case 10: // udpNoPorts.0 (1.3.6.1.2.1.7.2.0)
            {
                uint16 count = udp_get_datagrams_no_app();
                s32_t count_s32 = (s32_t)count;
                vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER;
                vb->value_len = sizeof(count_s32);
                GET_FREE_VB_VAL_BUF(vb->value);
                LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                if (vb->value != NULL)
                {
                    memcpy(vb->value, &count_s32, sizeof(count_s32));
                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                    msg_ps->vb_idx += 1;
                }
                else
                {
                    DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                    msg_ps->vb_ptr->ident = vb->ident;
                    msg_ps->vb_ptr->ident_len = vb->ident_len;
                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                }
                break;
            }
            case 11: // udpInErrors.0 (1.3.6.1.2.1.7.3.0)
            {
                uint16 count = udp_get_datagrams_failed_other();
                s32_t count_s32 = (s32_t)count;
                vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER;
                vb->value_len = sizeof(count_s32);
                GET_FREE_VB_VAL_BUF(vb->value);
                LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                if (vb->value != NULL)
                {
                    memcpy(vb->value, &count_s32, sizeof(count_s32));
                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                    msg_ps->vb_idx += 1;
                }
                else
                {
                    DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                    msg_ps->vb_ptr->ident = vb->ident;
                    msg_ps->vb_ptr->ident_len = vb->ident_len;
                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                }
                break;
            }
            case 12: // udpOutDatagrams.0 (1.3.6.1.2.1.7.4.0)
            {
                uint16 count = udp_get_datagrams_sent();
                s32_t count_s32 = (s32_t)count;
                vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER;
                vb->value_len = sizeof(count_s32);
                GET_FREE_VB_VAL_BUF(vb->value);
                LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                if (vb->value != NULL)
                {
                    memcpy(vb->value, &count_s32, sizeof(count_s32));
                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                    msg_ps->vb_idx += 1;
                }
                else
                {
                    DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                    msg_ps->vb_ptr->ident = vb->ident;
                    msg_ps->vb_ptr->ident_len = vb->ident_len;
                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                }
                break;
            }
            }
        }
        else
        {
            // Handle standard OID processing
            struct mib_node *mn;
            struct snmp_name_ptr np;

            if (snmp_iso_prefix_tst(msg_ps->vb_ptr->ident_len, msg_ps->vb_ptr->ident))
            {
                mn = snmp_search_tree((struct mib_node *)&internet, msg_ps->vb_ptr->ident_len - 4,
                                      msg_ps->vb_ptr->ident + 4, &np);

                if (mn != NULL)
                {
                    if (mn->node_type == MIB_NODE_EX)
                    {
                        struct mib_external_node *en = (struct mib_external_node *)mn;
                        msg_ps->state = SNMP_MSG_EXTERNAL_GET_OBJDEF;
                        msg_ps->ext_mib_node = en;
                        msg_ps->ext_name_ptr = np;
                        en->get_object_def_q(en->addr_inf, request_id, np.ident_len, np.ident);
                        return;
                    }
                    else
                    {
                        struct obj_def object_def;
                        msg_ps->state = SNMP_MSG_INTERNAL_GET_OBJDEF;
                        mn->get_object_def(np.ident_len, np.ident, &object_def);

                        if (object_def.instance != MIB_OBJECT_NONE)
                        {
                            GET_FREE_VB_MEM_BUF(vb);
                            memset(vb, 0x00, sizeof(struct snmp_varbind));
                            LWIP_ASSERT("vb != NULL", vb != NULL);

                            if (vb != NULL)
                            {
                                vb->next = NULL;
                                vb->prev = NULL;
                                vb->ident = msg_ps->vb_ptr->ident;
                                vb->ident_len = msg_ps->vb_ptr->ident_len;
                                msg_ps->vb_ptr->ident = NULL;
                                msg_ps->vb_ptr->ident_len = 0;
                                vb->value_type = object_def.asn_type;
                                vb->value_len = object_def.v_len;

                                if (vb->value_len > 0)
                                {
                                    GET_FREE_VB_VAL_BUF(vb->value);
                                    memset(vb->value, 0x00, sizeof(struct vb_value_buf_s));
                                    LWIP_ASSERT("vb->value != NULL", vb->value != NULL);

                                    if (vb->value != NULL)
                                    {
                                        mn->get_value(&object_def, vb->value_len, vb->value);
                                        snmp_varbind_tail_add(&msg_ps->outvb, vb);
                                        msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                                        msg_ps->vb_idx += 1;
                                    }
                                    else
                                    {
                                        DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                                        msg_ps->vb_ptr->ident = vb->ident;
                                        msg_ps->vb_ptr->ident_len = vb->ident_len;
                                        if (msg_ps->snmp_version == 0)
                                            snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                                        else
                                            snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                                    }
                                }
                                else
                                {
                                    vb->value = NULL;
                                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                                    msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                                    msg_ps->vb_idx += 1;
                                }
                            }
                            else
                            {
                                DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate outvb space\n"));
                                if (msg_ps->snmp_version == 0)
                                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                                else
                                    snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                            }
                        }
                        else
                        {
                            if (msg_ps->snmp_version == 0)
                                snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
                            else
                            {
                                GET_FREE_VB_MEM_BUF(vb);
                                if (vb != NULL)
                                {
                                    vb->next = NULL;
                                    vb->prev = NULL;
                                    vb->ident = msg_ps->vb_ptr->ident;
                                    vb->ident_len = msg_ps->vb_ptr->ident_len;
                                    msg_ps->vb_ptr->ident = NULL;
                                    vb->value_type = (SNMP_ASN1_CONTXT | SNMP_ASN1_PRIMIT | SNMP_ASN1_NOSUCH_INSTANCE);
                                    vb->value_len = 1;
                                    GET_FREE_VB_VAL_BUF(vb->value);
                                    if (vb->value != NULL)
                                    {
                                        *(u8_t *)(vb->value) = 0;
                                        snmp_varbind_tail_add(&msg_ps->outvb, vb);
                                        snmp_error_response_v2(msg_ps, SNMP_ES_NOSUCHNAME, 0);
                                    }
                                    else
                                    {
                                        DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                                        msg_ps->vb_ptr->ident = vb->ident;
                                        msg_ps->vb_ptr->ident_len = vb->ident_len;
                                        snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                                    }
                                }
                                else
                                {
                                    DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate outvb space\n"));
                                    snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (msg_ps->snmp_version == 0)
                        snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
                    else
                    {
                        GET_FREE_VB_MEM_BUF(vb);
                        if (vb != NULL)
                        {
                            vb->next = NULL;
                            vb->prev = NULL;
                            vb->ident = msg_ps->vb_ptr->ident;
                            vb->ident_len = msg_ps->vb_ptr->ident_len;
                            msg_ps->vb_ptr->ident = NULL;
                            vb->value_type = (SNMP_ASN1_CONTXT | SNMP_ASN1_PRIMIT | SNMP_ASN1_NOSUCH_INSTANCE);
                            vb->value_len = 1;
                            GET_FREE_VB_VAL_BUF(vb->value);
                            if (vb->value != NULL)
                            {
                                *(u8_t *)(vb->value) = 0;
                                snmp_varbind_tail_add(&msg_ps->outvb, vb);
                                snmp_error_response_v2(msg_ps, SNMP_ES_NOSUCHNAME, 0);
                            }
                            else
                            {
                                DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                                msg_ps->vb_ptr->ident = vb->ident;
                                msg_ps->vb_ptr->ident_len = vb->ident_len;
                                snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                            }
                        }
                        else
                        {
                            DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate outvb space\n"));
                            snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                        }
                    }
                }
            }
        }
    }

    // Send OK response if all variable bindings are processed
    if ((msg_ps->state == SNMP_MSG_SEARCH_OBJ) && (msg_ps->vb_idx == msg_ps->invb.count))
    {
        snmp_ok_response(msg_ps);
    }

    RESET_VB_BUFS();
}

/*
 * Function: update_json_string
 * ----------------------------
 * Updates global JSON string used for SNMP responses.
 * Copies new JSON into global buffer with null termination.
 * Used to update status data from UART/POE processing.
 */
void update_json_string(const char *new_json)
{
    strncpy(global_json_string, new_json, sizeof(global_json_string) - 1);
    global_json_string[sizeof(global_json_string) - 1] = '\0'; // Ensure null termination
}

/* Example usage elsewhere in your code */

/**
 * Service an internal or external event for SNMP GETNEXT.
 *
 * @param request_id identifies requests from 0 to (SNMP_CONCURRENT_REQUESTS-1)
 * @param msg_ps points to the associated message process state
 */
/*
 * Function: snmp_msg_getnext_event
 * --------------------------------
 * Handles SNMP GETNEXT request for walk operations.
 * Processes custom enterprise OIDs sequentially (0-6).
 * Returns next OID in sequence with descriptive text values.
 * Handles endOfMibView for transmission MIB OIDs.
 */
static void
snmp_msg_getnext_event(u8_t request_id, struct snmp_msg_pstat *msg_ps)
{
    DBG_SNMP_PRINTF(("[%s][%d]snmp_msg_getnext_event: msg_ps->state==%" U16_F "\n",
                     __FILE__, (uint16)__LINE__, (u16_t)msg_ps->state));

    if (msg_ps->state == SNMP_MSG_EXTERNAL_GET_OBJDEF)
    {
        struct mib_external_node *en;

        /* get_object_def() answer*/
        en = msg_ps->ext_mib_node;

        /* translate answer into a known lifeform */
        en->get_object_def_a(request_id, 1, &msg_ps->ext_oid.id[msg_ps->ext_oid.len - 1], &msg_ps->ext_object_def);
        if (msg_ps->ext_object_def.instance != MIB_OBJECT_NONE)
        {
            msg_ps->state = SNMP_MSG_EXTERNAL_GET_VALUE;
            en->get_value_q(request_id, &msg_ps->ext_object_def);
        }
        else
        {
            en->get_object_def_pc(request_id, 1, &msg_ps->ext_oid.id[msg_ps->ext_oid.len - 1]);
            /* search failed, object id points to unknown object (nosuchname) */
            snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
        }
    }
    else if (msg_ps->state == SNMP_MSG_EXTERNAL_GET_VALUE)
    {
        struct mib_external_node *en;
        struct snmp_varbind *vb;

        /* get_value() answer */
        en = msg_ps->ext_mib_node;

        vb = snmp_varbind_alloc(&msg_ps->ext_oid,
                                msg_ps->ext_object_def.asn_type,
                                msg_ps->ext_object_def.v_len);
        if (vb != NULL)
        {
            en->get_value_a(request_id, &msg_ps->ext_object_def, vb->value_len, vb->value);
            snmp_varbind_tail_add(&msg_ps->outvb, vb);
            msg_ps->state = SNMP_MSG_SEARCH_OBJ;
            msg_ps->vb_idx += 1;
        }
        else
        {
            en->get_value_pc(request_id, &msg_ps->ext_object_def);
            DBG_SNMP_PRINTF(("snmp_msg_getnext_event: couldn't allocate outvb space\n"));
            if (msg_ps->snmp_version == 0)
                snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
            else
                snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
        }
    }

    while ((msg_ps->state == SNMP_MSG_SEARCH_OBJ) &&
           (msg_ps->vb_idx < msg_ps->invb.count))
    {
        struct mib_node *mn;
        struct snmp_obj_id oid;
        int i, j, match = -1;
        
        if (msg_ps->vb_idx == 0)
        {
            msg_ps->vb_ptr = msg_ps->invb.head;
        }
        else
        {
            msg_ps->vb_ptr = msg_ps->vb_ptr->next;
        }
        
        // Check for custom enterprise OID prefix (1.3.6.1.4.1.63663.1)
        if (msg_ps->vb_ptr->ident_len >= 7 &&
            msg_ps->vb_ptr->ident[0] == 1 &&
            msg_ps->vb_ptr->ident[1] == 3 &&
            msg_ps->vb_ptr->ident[2] == 6 &&
            msg_ps->vb_ptr->ident[3] == 1 &&
            msg_ps->vb_ptr->ident[4] == 4 &&
            msg_ps->vb_ptr->ident[5] == 1 &&
            msg_ps->vb_ptr->ident[6] == 63663) {
            
            // For GETNEXT, find the next OID in sequence
            int next_idx = -1;
            
            // If exact enterprise prefix or prefix.1, return first OID
            if (msg_ps->vb_ptr->ident_len <= 8) {
                next_idx = 0;
            } else {
                // Find current OID index
                u8_t last_idx = msg_ps->vb_ptr->ident[8];
                
                // If valid index and not the last one, return next
                if (last_idx < 6) {
                    next_idx = last_idx + 1;
                }
            }
            
            // If we found a next OID, return it
            if (next_idx >= 0 && next_idx <= 6) {
                struct snmp_varbind *vb;
                s32_t value = 0;
                
                // Allocate varbind for response
                GET_FREE_VB_MEM_BUF(vb);
                if (vb != NULL) {
                    memset(vb, 0x00, sizeof(struct snmp_varbind));
                    vb->next = NULL;
                    vb->prev = NULL;
                    
                    // Set OID
                    vb->ident_len = 10;
                    GET_FREE_VB_IDENT_BUF(vb->ident);
                    if (vb->ident != NULL) {
                        // Set base OID
                        vb->ident[0] = 1;
                        vb->ident[1] = 3;
                        vb->ident[2] = 6;
                        vb->ident[3] = 1;
                        vb->ident[4] = 4;
                        vb->ident[5] = 1;
                        vb->ident[6] = 63663;
                        vb->ident[7] = 1;
                        vb->ident[8] = next_idx;
                        vb->ident[9] = 0;
                        
                        // Set value with descriptive text for GETNEXT (snmpwalk)
                        GET_FREE_VB_VAL_BUF(vb->value);
                        
                        if (vb->value != NULL) {
                            char result_str[60];
                            const char *status_text;
                            
                            // Get appropriate value and format with description
                            switch (next_idx) {
                                case 0: // acStatus
                                    parse_json_field(global_json_string, "A", &value);
                                    if (value == 0) {
                                        status_text = "0 - Offline (No AC Power)";
                                    } else if (value == 1) {
                                        status_text = "1 - Online (AC Power Connected)";
                                    } else {
                                        status_text = "Unknown AC Status";
                                    }
                                    strcpy(result_str, status_text);
                                    break;
                                    
                                case 1: // batteryStatus
                                    parse_json_field(global_json_string, "B", &value);
                                    if (value == 0) {
                                        status_text = "0 - Inactive (Battery Not Working)";
                                    } else if (value == 1) {
                                        status_text = "1 - Active (Battery Operational)";
                                    } else {
                                        status_text = "Unknown Battery Status";
                                    }
                                    strcpy(result_str, status_text);
                                    break;
                                    
                                case 2: // batteryPercentage
                                    parse_json_field(global_json_string, "C", &value);
                                    if (value >= 80) {
                                        sprintf(result_str, "%d%% - Fully Charged", (int)value);
                                    } else if (value >= 50) {
                                        sprintf(result_str, "%d%% - Good Charge", (int)value);
                                    } else if (value >= 20) {
                                        sprintf(result_str, "%d%% - Low Battery", (int)value);
                                    } else if (value > 0) {
                                        sprintf(result_str, "%d%% - Critical Battery", (int)value);
                                    } else {
                                        sprintf(result_str, "0%% - Empty or Not Present");
                                    }
                                    break;
                                    
                                case 3: // voltageMode
                                    parse_json_field(global_json_string, "D", &value);
                                    sprintf(result_str, "%d - Voltage Mode", (int)value);
                                    break;
                                    
                                case 4: // voltageValue
                                    parse_json_field(global_json_string, "E", &value);
                                    if (value >= 1000) {
                                        int volts = value / 1000;
                                        int millivolts = value % 1000;
                                        sprintf(result_str, "%d.%03dV - Voltage Reading", volts, millivolts);
                                    } else {
                                        sprintf(result_str, "%dV - Voltage Reading", (int)value);
                                    }
                                    break;
                                    
                                case 5: // poe port status
                                    value = (s32_t)portPoe;
                                    if (value == 0) {
                                        sprintf(result_str, "0 - PoE Port Link Down");
                                    } else if (value == 1) {
                                        sprintf(result_str, "1 - PoE Port Link Up");
                                    } else {
                                        sprintf(result_str, "%d - PoE Port Status", (int)value);
                                    }
                                    break;
                                    
                                case 6: // lan port status
                                    value = (s32_t)portLan;
                                    if (value == 0) {
                                        sprintf(result_str, "0 - LAN Port Link Down");
                                    } else if (value == 1) {
                                        sprintf(result_str, "1 - LAN Port Link Up");
                                    } else {
                                        sprintf(result_str, "%d - LAN Port Status", (int)value);
                                    }
                                    break;
                            }
                            
                            // Set as string type with description
                            vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR;
                            vb->value_len = strlen(result_str);
                            memcpy(vb->value, result_str, vb->value_len);
                            
                            snmp_varbind_tail_add(&msg_ps->outvb, vb);
                            msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                            msg_ps->vb_idx += 1;
                            continue;
                        }
                    }
                    
                    // Error handling for allocation failures
                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                    continue;
                }
            }
        }
        
        // Check for problematic transmission MIB OID
        if (msg_ps->vb_ptr->ident_len >= 7 && 
            msg_ps->vb_ptr->ident[0] == 1 && 
            msg_ps->vb_ptr->ident[1] == 3 && 
            msg_ps->vb_ptr->ident[2] == 6 && 
            msg_ps->vb_ptr->ident[3] == 1 && 
            msg_ps->vb_ptr->ident[4] == 2 && 
            msg_ps->vb_ptr->ident[5] == 1 && 
            msg_ps->vb_ptr->ident[6] == 10) {
            
            // Return endOfMibView for transmission MIB OIDs to properly terminate walk
            struct snmp_varbind *vb;
            GET_FREE_VB_MEM_BUF(vb);
            if (vb != NULL) {
                memset(vb, 0x00, sizeof(struct snmp_varbind));
                vb->next = NULL;
                vb->prev = NULL;
                vb->ident = msg_ps->vb_ptr->ident;
                vb->ident_len = msg_ps->vb_ptr->ident_len;
                /* ensure this memory is referenced once only */
                msg_ps->vb_ptr->ident = NULL;
                msg_ps->vb_ptr->ident_len = 0;

                vb->value_type = (SNMP_ASN1_CONTXT | SNMP_ASN1_PRIMIT | SNMP_ASN1_ENDOFMIBVIEW);
                vb->value_len = 1;
                
                GET_FREE_VB_VAL_BUF(vb->value);
                if (vb->value != NULL) {
                    memset(vb->value, 0x00, sizeof(struct vb_value_buf_s));
                    *(u8_t *)(vb->value) = 0;
                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    snmp_ok_response(msg_ps);
                } else {
                    DBG_SNMP_PRINTF(("snmp_msg_getnext_event: couldn't allocate variable space\n"));
                    msg_ps->vb_ptr->ident = vb->ident;
                    msg_ps->vb_ptr->ident_len = vb->ident_len;
                    if (msg_ps->snmp_version == 0) {
                        snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                    } else {
                        snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                    }
                }
            } else {
                DBG_SNMP_PRINTF(("snmp_msg_getnext_event: couldn't allocate vb space\n"));
                if (msg_ps->snmp_version == 0) {
                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                } else {
                    snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                }
            }
            RESET_VB_BUFS();
            return;
        }

        if (snmp_iso_prefix_expand(msg_ps->vb_ptr->ident_len, msg_ps->vb_ptr->ident, &oid))
        {
            if (msg_ps->vb_ptr->ident_len > 3)
            {
                /* can offset ident_len and ident */
                mn = snmp_expand_tree((struct mib_node *)&internet,
                                      msg_ps->vb_ptr->ident_len - 4,
                                      msg_ps->vb_ptr->ident + 4, &oid);
            }
            else
            {
                /* can't offset ident_len -4, ident + 4 */
                mn = snmp_expand_tree((struct mib_node *)&internet, 0, NULL, &oid);
            }
        }
        else
        {
            mn = NULL;
        }

        DBG_SNMP_PRINTF(("[%s][%d][mn=0x%04x]\n", __FILE__, (uint16)__LINE__, (uint16)mn));

        if (mn != NULL)
        {
            DBG_SNMP_PRINTF(("[%s][%d][mn->node_type=0x%04x]\n",
                             __FILE__, (uint16)__LINE__, (uint16)mn->node_type));

            if (mn->node_type == MIB_NODE_EX)
            {
                /* external object */
                struct mib_external_node *en = (struct mib_external_node *)mn;

                msg_ps->state = SNMP_MSG_EXTERNAL_GET_OBJDEF;
                /* save en && args in msg_ps!! */
                msg_ps->ext_mib_node = en;
                msg_ps->ext_oid = oid;
                DBG_SNMP_PRINTF(("[%s][%d]\n", __FILE__, (uint16)__LINE__));
                en->get_object_def_q(en->addr_inf, request_id, 1, &oid.id[oid.len - 1]);
            }
            else
            {
                /* internal object */
                struct obj_def object_def;
                struct snmp_varbind *vb;

                msg_ps->state = SNMP_MSG_INTERNAL_GET_OBJDEF;
                DBG_SNMP_PRINTF(("[%s][%d][mn->get_object_def=0x%04x]\n",
                                 __FILE__, (uint16)__LINE__, (uint16)mn->get_object_def));

                mn->get_object_def(1, &oid.id[oid.len - 1], &object_def);

                vb = snmp_varbind_alloc(&oid, object_def.asn_type, object_def.v_len);
                if (vb != NULL)
                {
                    msg_ps->state = SNMP_MSG_INTERNAL_GET_VALUE;
                    mn->get_value(&object_def, object_def.v_len, vb->value);
                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                    msg_ps->vb_idx += 1;
                }
                else
                {
                    DBG_SNMP_PRINTF(("[%s][%d]\n", __FILE__, (uint16)__LINE__));
                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                }
            }
        }
        if (mn == NULL)
        {
            /* mn == NULL, noSuchName */
            if (msg_ps->snmp_version == 0) /*v1 */
                snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
            else
            {
                struct snmp_varbind *vb;
                GET_FREE_VB_MEM_BUF(vb);
                memset(vb, 0x00, sizeof(struct snmp_varbind));
                if (vb != NULL)
                {
                    vb->next = NULL;
                    vb->prev = NULL;
                    vb->ident = msg_ps->vb_ptr->ident;
                    vb->ident_len = msg_ps->vb_ptr->ident_len;
                    /* ensure this memory is refereced once only */
                    msg_ps->vb_ptr->ident = NULL;
                    msg_ps->vb_ptr->ident_len = 0;

                    vb->value_type = (SNMP_ASN1_CONTXT | SNMP_ASN1_PRIMIT | SNMP_ASN1_ENDOFMIBVIEW);
                    vb->value_len = 1;

                    GET_FREE_VB_VAL_BUF(vb->value);
                    memset(vb->value, 0x00, sizeof(struct vb_value_buf_s));

                    if (vb->value != NULL)
                    {
                        *(u8_t *)(vb->value) = 0;
                        snmp_varbind_tail_add(&msg_ps->outvb, vb);
                        snmp_error_response_v2(msg_ps, SNMP_ES_NOSUCHNAME, 0);
                    }
                    else
                    {
                        DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                        msg_ps->vb_ptr->ident = vb->ident;
                        msg_ps->vb_ptr->ident_len = vb->ident_len;
                        snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                    }
                }
                else
                {
                    DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate outvb space\n"));
                    snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                }
            } /*esle*/
        }
    }
    if ((msg_ps->state == SNMP_MSG_SEARCH_OBJ) &&
        (msg_ps->vb_idx == msg_ps->invb.count))
    {
        snmp_ok_response(msg_ps);
    }
    RESET_VB_BUFS();
}

/*
 * Function: snmp_msg_getbulk_event
 * --------------------------------
 * Handles SNMP GETBULK request for bulk walk operations.
 * Processes non-repeater (n) and max-repetitions (m) parameters.
 * Repeats GETNEXT operations for efficient bulk data retrieval.
 */
static void snmp_msg_getbulk_event(struct snmp_msg_pstat *msg_ps)
{
    s32_t n;
    s32_t m;
    s32_t r;
    s32_t i;
    s32_t j;
    s32_t k;
    u8_t vb_idx;
    struct snmp_varbind *vb_ptr;

    n = msg_ps->error_status;
    m = msg_ps->error_index;

    if (msg_ps->invb.count <= n)
        n = msg_ps->invb.count;

    if (msg_ps->invb.count - n > 0)
        r = msg_ps->invb.count - n;
    else
        r = 0;

    DBG_SNMP_PRINTF(("m %ld n %ld r %ld \n", m, n, r));

    /*clear */
    msg_ps->error_status = 0;
    msg_ps->error_index = 0;

    /*first n th variable binding */
    while ((msg_ps->state == SNMP_MSG_SEARCH_OBJ) && (msg_ps->vb_idx < n))
    {
        struct mib_node *mn;
        struct snmp_obj_id oid;

        if (msg_ps->vb_idx == 0)
        {
            msg_ps->vb_ptr = msg_ps->invb.head;
        }
        else
        {
            msg_ps->vb_ptr = msg_ps->vb_ptr->next;
        }
        if (snmp_iso_prefix_expand(msg_ps->vb_ptr->ident_len, msg_ps->vb_ptr->ident, &oid))
        {
            if (msg_ps->vb_ptr->ident_len > 3)
            {
                /* can offset ident_len and ident */
                // mn = snmp_expand_tree((struct mib_node *) &internet,
                //                       msg_ps->vb_ptr->ident_len - 4, msg_ps->vb_ptr->ident + 4, &oid);
                mn = snmp_expand_tree((struct mib_node *)&internet,
                                      msg_ps->vb_ptr->ident_len - 4, msg_ps->vb_ptr->ident + 4, &oid);
            }
            else
            {
                /* can't offset ident_len -4, ident + 4 */
                // mn = snmp_expand_tree((struct mib_node *) &internet, 0, NULL, &oid);
                mn = snmp_expand_tree((struct mib_node *)&internet, 0, NULL, &oid);
            }
        }
        else
        {
            mn = NULL;
        }
        if (mn != NULL)
        {
            if (mn->node_type == MIB_NODE_EX)
            {
            }
            else            {
                /* internal object */
                struct obj_def object_def;
                struct snmp_varbind *vb;

                msg_ps->state = SNMP_MSG_INTERNAL_GET_OBJDEF;
                mn->get_object_def(1, &oid.id[oid.len - 1], &object_def);
                // mib_get_object_def(mn, 1, &oid.id[oid.len - 1], &object_def);
                vb = snmp_varbind_alloc(&oid, object_def.asn_type, object_def.v_len);
                if (vb != NULL)
                {
                    msg_ps->state = SNMP_MSG_INTERNAL_GET_VALUE;
                    mn->get_value(&object_def, object_def.v_len, vb->value);
                    // mib_get_value(mn, &object_def, object_def.v_len, vb->value);

                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                    msg_ps->vb_idx += 1;
                }
                else
                {
                    DBG_SNMP_PRINTF(("snmp_recv couldn't allocate outvb space\n"));
                    if (msg_ps->snmp_version == 0) /*v1 */
                        snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                    else
                        snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                }
            }
        }
        if (mn == NULL)
        {

            if (msg_ps->snmp_version == 0) /*v1 */
                                           /* mn == NULL, noSuchName */
                snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
            else
            {
                /*alloc vb set to no such instance */
                struct snmp_varbind *vb;
                GET_FREE_VB_MEM_BUF(vb); // = (struct snmp_varbind *) mem_malloc(sizeof(struct snmp_varbind));
                if (vb != NULL)
                {
                    vb->next = NULL;
                    vb->prev = NULL;
                    vb->ident = msg_ps->vb_ptr->ident;
                    vb->ident_len = msg_ps->vb_ptr->ident_len;
                    /* ensure this memory is refereced once only */
                    msg_ps->vb_ptr->ident = NULL;
                    msg_ps->vb_ptr->ident_len = 0;

                    vb->value_type = (SNMP_ASN1_CONTXT | SNMP_ASN1_PRIMIT | SNMP_ASN1_ENDOFMIBVIEW);
                    vb->value_len = 1;

                    GET_FREE_VB_VAL_BUF(vb->value); // = mem_malloc(vb->value_len);

                    if (vb->value != NULL)
                    {
                        *(u8_t *)(vb->value) = 0;
                        snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    }
                    else
                    {
                        DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                        msg_ps->vb_ptr->ident = vb->ident;
                        msg_ps->vb_ptr->ident_len = vb->ident_len;
                        // mem_free(vb);
                        snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                    }
                }
                else
                {
                    DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate outvb space\n"));
                    snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                }
                snmp_error_response_v2(msg_ps, SNMP_ES_NOSUCHNAME, 0);
            }
        }
    }

    vb_idx = msg_ps->vb_idx;
    if (vb_idx == 0)
        vb_ptr = msg_ps->invb.head;
    else
    {
        vb_ptr = msg_ps->invb.head;
        for (k = 0; k < vb_idx; k++)
            vb_ptr = vb_ptr->next;
    }

    msg_ps->vb_ptr = vb_ptr;
    // msg_ps->vb_idx = vb_idx;
    // vb_ptr = msg_ps->vb_ptr;
    /*next */
    for (j = 0; j < r; j++)
    {
        // printf("j %ld \n",j);
        // printf("B vb_idx %bd \n",vb_idx);

        for (i = 0; i < m; i++)
        {

            // msg_ps->vb_ptr = vb_ptr;

            // printf("i %ld \n",i);
            // printf(" msg_ps->vb_idx %bd \n", msg_ps->vb_idx);
            // while ((msg_ps->state == SNMP_MSG_SEARCH_OBJ) && (msg_ps->vb_idx < msg_ps->invb.count))
            // while ((msg_ps->state == SNMP_MSG_SEARCH_OBJ) )
            if (msg_ps->state != SNMP_MSG_SEARCH_OBJ)
            {
                break;
            }
            else
            {
                struct mib_node *mn;
                struct snmp_obj_id oid;

                /*if (msg_ps->vb_idx == 0)
                {
                   msg_ps->vb_ptr = msg_ps->invb.head;
                }
                else
                {
                   msg_ps->vb_ptr = msg_ps->vb_ptr->next;
                }*/

                // printf("msg_ps->vb_ptr->ident_len %bd \n",msg_ps->vb_ptr->ident_len);
                //  for (k=0;k<msg_ps->vb_ptr->ident_len;k++)
                //	printf("%ld.",msg_ps->vb_ptr->ident[k]);

                // printf("\n");

                if (snmp_iso_prefix_expand(msg_ps->vb_ptr->ident_len, msg_ps->vb_ptr->ident, &oid))
                {
                    if (msg_ps->vb_ptr->ident_len > 3)
                    {
                        /* can offset ident_len and ident */
                        mn = snmp_expand_tree((struct mib_node *)&internet,
                                              msg_ps->vb_ptr->ident_len - 4, msg_ps->vb_ptr->ident + 4, &oid);
                    }
                    else
                    {
                        /* can't offset ident_len -4, ident + 4 */
                        mn = snmp_expand_tree((struct mib_node *)&internet, 0, NULL, &oid);
                    }
                }
                else
                {
                    mn = NULL;
                }
                if (mn != NULL)
                {
                    if (mn->node_type == MIB_NODE_EX)
                    {
                    }
                    else
                    {
                        /* internal object */
                        struct obj_def object_def;
                        struct snmp_varbind *vb;
                        struct snmp_varbind *vb_next;

                        msg_ps->state = SNMP_MSG_INTERNAL_GET_OBJDEF;
                        mn->get_object_def(1, &oid.id[oid.len - 1], &object_def);
                        // mib_get_object_def(mn, 1, &oid.id[oid.len - 1], &object_def);

                        vb = snmp_varbind_alloc(&oid, object_def.asn_type, object_def.v_len);
                        if (vb != NULL)
                        {
                            msg_ps->state = SNMP_MSG_INTERNAL_GET_VALUE;
                            mn->get_value(&object_def, object_def.v_len, vb->value);
                            // mib_get_value(mn, &object_def, object_def.v_len, vb->value);

                            snmp_varbind_tail_add(&msg_ps->outvb, vb);
                            msg_ps->state = SNMP_MSG_SEARCH_OBJ;

                            /*add vb_next  to input list */
                            vb_next = snmp_varbind_duplicate(vb);
                            if (vb_next != NULL)
                            {
                                vb_next->prev = msg_ps->vb_ptr->prev;
                                vb_next->next = msg_ps->vb_ptr->next;
                                if (vb_next->prev != NULL)
                                {
                                    vb_next->prev->next = vb_next;
                                }
                                if (vb_next->next != NULL)
                                {
                                    vb_next->next->prev = vb_next;
                                }
                                if (msg_ps->vb_idx == 0)
                                    msg_ps->invb.head = vb_next;
                                // if (i == 0)
                                //    vb_ptr = vb_next;
                                if (msg_ps->vb_idx == msg_ps->invb.count - 1)
                                    msg_ps->invb.tail = vb_next;
                                snmp_varbind_free(msg_ps->vb_ptr);

                                msg_ps->vb_ptr = vb_next;
                                // printf("SSS\n");
                                // msg_ps->vb_idx += 1;
                            }
                            else
                            {
                                DBG_SNMP_PRINTF(("snmp_recv couldn't allocate outvb space\n"));
                                // snmp_error_response(msg_ps,SNMP_ES_TOOBIG);
                                if (msg_ps->snmp_version == 0) /*v1 */
                                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                                else /*v2c */
                                    snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                            }
                        }
                        else
                        {
                            DBG_SNMP_PRINTF(("snmp_recv couldn't allocate outvb space\n"));
                            // snmp_error_response(msg_ps,SNMP_ES_TOOBIG);
                            if (msg_ps->snmp_version == 0) /*v1 */
                                snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                            else /*v2c */
                                snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                        }
                    }
                }
                if (mn == NULL)
                {

                    if (msg_ps->snmp_version == 0) /*v1 */
                        /* mn == NULL, noSuchName */
                        snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
                    else
                    {

                        /*v2c */
                        /*alloc vb set to no such instance */
                        struct snmp_varbind *vb;
                        // uint8 xdata ttt;
                        // printf("hehehe \n");

                        GET_FREE_VB_MEM_BUF(vb); // = (struct snmp_varbind *) mem_malloc(sizeof(struct snmp_varbind));
                        if (vb != NULL)
                        {
                            vb->next = NULL;
                            vb->prev = NULL;
                            vb->ident = msg_ps->vb_ptr->ident;
                            vb->ident_len = msg_ps->vb_ptr->ident_len;
                            // for (ttt=0;ttt<vb->ident_len;ttt++)
                            //	{
                            //		printf("vb->ident[%bd] %ld \n",ttt,vb->ident[ttt]);
                            //	}
                            /* ensure this memory is refereced once only */
                            msg_ps->vb_ptr->ident = NULL;
                            msg_ps->vb_ptr->ident_len = 0;

                            vb->value_type = (SNMP_ASN1_CONTXT | SNMP_ASN1_PRIMIT | SNMP_ASN1_ENDOFMIBVIEW);
                            vb->value_len = 1;

                            GET_FREE_VB_VAL_BUF(vb->value); // = mem_malloc(vb->value_len);

                            if (vb->value != NULL)
                            {
                                *(u8_t *)(vb->value) = 0;
                                snmp_varbind_tail_add(&msg_ps->outvb, vb);
                                snmp_error_response_v2(msg_ps, SNMP_ES_NOSUCHNAME, 0);
                            }
                            else
                            {
                                DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate variable space\n"));
                                msg_ps->vb_ptr->ident = vb->ident;
                                msg_ps->vb_ptr->ident_len = vb->ident_len;
                                // mem_free(vb);
                                snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                            }
                        }
                        else
                        {
                            DBG_SNMP_PRINTF(("snmp_msg_event: couldn't allocate outvb space\n"));
                            snmp_error_response_v2(msg_ps, SNMP_ES_TOOBIG, 0);
                        }
                    }
                }
            }
        }

        // printf("out I \n");
        vb_idx++;
        msg_ps->vb_idx = vb_idx;
        if (vb_idx == 0)
            vb_ptr = msg_ps->invb.head;
        else
        {
            vb_ptr = msg_ps->invb.head;
            for (k = 0; k < vb_idx; k++)
                vb_ptr = vb_ptr->next;
        }
        msg_ps->vb_ptr = vb_ptr;
    }

    if ((msg_ps->state == SNMP_MSG_SEARCH_OBJ) && (msg_ps->vb_idx == msg_ps->invb.count))
    {
        snmp_ok_response(msg_ps);
    }

    RESET_VB_BUFS();
}

/**
 * Service an internal or external event for SNMP SET.
 *
 * @param request_id identifies requests from 0 to (SNMP_CONCURRENT_REQUESTS-1)
 * @param msg_ps points to the associated message process state
 */
/*
 * Function: snmp_msg_set_event
 * ----------------------------
 * Handles SNMP SET request processing with two-phase commit.
 * First validates all OIDs (test phase), then applies changes (set phase).
 * Supports custom OID for voltage setting (24, 30, 48, 56V).
 * Returns success/failure JSON response for SNMPv2c.
 */
static void snmp_msg_set_event(u8_t request_id, struct snmp_msg_pstat *msg_ps)
{
    /* Define custom OIDs for SET */
    s32_t custom_set_oids[1][10] = {
        {1, 3, 6, 1, 4, 1, 63663, 1, 9, 0} // OID 1 (voltage)
//        {1, 3, 6, 1, 4, 1, 63663, 2}, // OID 2
//        {1, 3, 6, 1, 4, 1, 63663, 3}, // OID 3
//        {1, 3, 6, 1, 4, 1, 63663, 4}  // OID 4
    };
    u8_t custom_oid_len = 10;
    int match = -1, i, j;
    int valid_voltage = 1;

    struct snmp_varbind *vb = NULL;

    DBG_SNMP_PRINTF(("snmp_msg_set_event: msg_ps->state==%" U16_F "\n", (u16_t)msg_ps->state));

    if (msg_ps->state == SNMP_MSG_EXTERNAL_GET_OBJDEF)
    {
        struct snmp_name_ptr np;
        struct mib_external_node *en = msg_ps->ext_mib_node;
        np = msg_ps->ext_name_ptr;
        en->get_object_def_a(request_id, np.ident_len, np.ident, &msg_ps->ext_object_def);
        if (msg_ps->ext_object_def.instance != MIB_OBJECT_NONE)
        {
            msg_ps->state = SNMP_MSG_EXTERNAL_SET_TEST;
            en->set_test_q(request_id, &msg_ps->ext_object_def);
        }
        else
        {
            en->get_object_def_pc(request_id, np.ident_len, np.ident);
            snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
        }
    }
    else if (msg_ps->state == SNMP_MSG_EXTERNAL_SET_TEST)
    {
        struct mib_external_node *en = msg_ps->ext_mib_node;
        if (msg_ps->ext_object_def.access == MIB_OBJECT_READ_WRITE)
        {
            if ((msg_ps->ext_object_def.asn_type == msg_ps->vb_ptr->value_type) &&
                (en->set_test_a(request_id, &msg_ps->ext_object_def, msg_ps->vb_ptr->value_len, msg_ps->vb_ptr->value) != 0))
            {
                msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                msg_ps->vb_idx += 1;
            }
            else
            {
                en->set_test_pc(request_id, &msg_ps->ext_object_def);
                snmp_error_response(msg_ps, SNMP_ES_BADVALUE);
            }
        }
        else
        {
            en->set_test_pc(request_id, &msg_ps->ext_object_def);
            snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
        }
    }
    else if (msg_ps->state == SNMP_MSG_EXTERNAL_GET_OBJDEF_S)
    {
        struct snmp_name_ptr np;
        struct mib_external_node *en = msg_ps->ext_mib_node;
        np = msg_ps->ext_name_ptr;
        en->get_object_def_a(request_id, np.ident_len, np.ident, &msg_ps->ext_object_def);
        if (msg_ps->ext_object_def.instance != MIB_OBJECT_NONE)
        {
            msg_ps->state = SNMP_MSG_EXTERNAL_SET_VALUE;
            en->set_value_q(request_id, &msg_ps->ext_object_def, msg_ps->vb_ptr->value_len, msg_ps->vb_ptr->value);
        }
        else
        {
            en->get_object_def_pc(request_id, np.ident_len, np.ident);
            snmp_error_response(msg_ps, SNMP_ES_GENERROR);
        }
    }
    else if (msg_ps->state == SNMP_MSG_EXTERNAL_SET_VALUE)
    {
        struct mib_external_node *en = msg_ps->ext_mib_node;
        en->set_value_a(request_id, &msg_ps->ext_object_def, 0, NULL);
        msg_ps->state = SNMP_MSG_INTERNAL_SET_VALUE;
        msg_ps->vb_idx += 1;
    }

    /* Test all values before setting */
    while ((msg_ps->state == SNMP_MSG_SEARCH_OBJ) && (msg_ps->vb_idx < msg_ps->invb.count))
    {
        if (msg_ps->vb_idx == 0)
        {
            msg_ps->vb_ptr = msg_ps->invb.head;
        }
        else
        {
            msg_ps->vb_ptr = msg_ps->vb_ptr->next;
        }

        /* Check for custom OIDs */
        match = -1;
        if (msg_ps->vb_ptr->ident_len == custom_oid_len)
        {
            for (j = 0; j < 4; j++)
            {
                int oid_match = 1;
                for (i = 0; i < custom_oid_len; i++)
                {
                    if (msg_ps->vb_ptr->ident[i] != custom_set_oids[j][i])
                    {
                        oid_match = 0;
                        break;
                    }
                }
                if (oid_match)
                {
                    match = j;
                    break;
                }
            }
        }

        if (match >= 0) // Custom OID matched
        {

            s32_t voltage;
            switch (match)
            {
            case 0: // OID 1 (voltage)
                if (msg_ps->vb_ptr->value_type == (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG) &&
                    msg_ps->vb_ptr->value_len == sizeof(s32_t))
                {
                    voltage = *(s32_t *)msg_ps->vb_ptr->value;
                    if (voltage == 24 || voltage == 30 || voltage == 48 || voltage == 56)
                    {
                        printf("set_POE_VOLT:%ld;\r\n", voltage);
                    }
                    else
                    {
                        valid_voltage = 0;
                    }
                }
                else
                {
                    valid_voltage = 0;
                }
                break;
            case 1:
                printf("Setting OID 1.3.6.1.4.1.9999.2\n");
                break;
            case 2:
                printf("Setting OID 1.3.6.1.4.1.9999.3\n");
                break;
            case 3:
                printf("Setting OID 1.3.6.1.4.1.9999.4\n");
                break;
            }
            if (valid_voltage)
            {
                msg_ps->state = SNMP_MSG_INTERNAL_SET_VALUE;
                msg_ps->vb_idx += 1;
            }
            else
            {
                msg_ps->state = SNMP_MSG_INTERNAL_SET_VALUE;
                msg_ps->vb_idx = msg_ps->invb.count; // Skip remaining varbinds
            }
        }
        else // Standard OID processing
        {
            struct mib_node *mn;
            struct snmp_name_ptr np;
            if (snmp_iso_prefix_tst(msg_ps->vb_ptr->ident_len, msg_ps->vb_ptr->ident))
            {
                mn = snmp_search_tree((struct mib_node *)&internet, msg_ps->vb_ptr->ident_len - 4,
                                      msg_ps->vb_ptr->ident + 4, &np);
                if (mn != NULL)
                {
                    if (mn->node_type == MIB_NODE_EX)
                    {
                        struct mib_external_node *en = (struct mib_external_node *)mn;
                        msg_ps->state = SNMP_MSG_EXTERNAL_GET_OBJDEF;
                        msg_ps->ext_mib_node = en;
                        msg_ps->ext_name_ptr = np;
                        en->get_object_def_q(en->addr_inf, request_id, np.ident_len, np.ident);
                    }
                    else
                    {
                        struct obj_def object_def;
                        msg_ps->state = SNMP_MSG_INTERNAL_GET_OBJDEF;
                        mn->get_object_def(np.ident_len, np.ident, &object_def);
                        if (object_def.instance != MIB_OBJECT_NONE)
                        {
                            if (object_def.access == MIB_OBJECT_READ_WRITE)
                            {
                                if (object_def.asn_type == msg_ps->vb_ptr->value_type)
                                {
                                    if (mn->set_test(&object_def, msg_ps->vb_ptr->value_len, msg_ps->vb_ptr->value) != 0)
                                    {
                                        msg_ps->state = SNMP_MSG_INTERNAL_SET_VALUE;
                                        msg_ps->vb_idx += 1;
                                    }
                                    else
                                    {
                                        if (msg_ps->snmp_version == 0)
                                            snmp_error_response(msg_ps, SNMP_ES_BADVALUE);
                                        else
                                            snmp_error_response_v2(msg_ps, SNMP_ES_wrongValue, 1);
                                    }
                                }
                                else
                                {
                                    if (msg_ps->snmp_version == 0)
                                        snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
                                    else
                                        snmp_error_response_v2(msg_ps, SNMP_ES_wrongType, 1);
                                }
                            }
                            else
                            {
                                if (msg_ps->snmp_version == 0)
                                    snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
                                else
                                    snmp_error_response_v2(msg_ps, SNMP_ES_notWritable, 1);
                            }
                        }
                        else
                        {
                            snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
                        }
                    }
                }
                else
                {
                    snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
                }
            }
            else
            {
                snmp_error_response(msg_ps, SNMP_ES_NOSUCHNAME);
            }
        }
    }

    if ((msg_ps->state == SNMP_MSG_SEARCH_OBJ) && (msg_ps->vb_idx == msg_ps->invb.count))
    {
        msg_ps->vb_idx = 0;
        msg_ps->state = SNMP_MSG_INTERNAL_SET_VALUE;
    }

    /* Set all values "atomically" */
    while ((msg_ps->state == SNMP_MSG_INTERNAL_SET_VALUE) && (msg_ps->vb_idx < msg_ps->invb.count))
    {
        if (msg_ps->vb_idx == 0)
        {
            msg_ps->vb_ptr = msg_ps->invb.head;
        }
        else
        {
            msg_ps->vb_ptr = msg_ps->vb_ptr->next;
        }

        /* Check custom OIDs again for set phase */
        match = -1;
        if (msg_ps->vb_ptr->ident_len == custom_oid_len)
        {
            for (j = 0; j < 4; j++)
            {
                int oid_match = 1;
                for (i = 0; i < custom_oid_len; i++)
                {
                    if (msg_ps->vb_ptr->ident[i] != custom_set_oids[j][i])
                    {
                        oid_match = 0;
                        break;
                    }
                }
                if (oid_match)
                {
                    match = j;
                    break;
                }
            }
        }

        if (match >= 0) // Custom OID matched
        {
            msg_ps->vb_idx += 1; // Assume set is successful if it passed test phase
        }
        else // Standard OID set
        {
            struct mib_node *mn;
            struct snmp_name_ptr np;
            if (snmp_iso_prefix_tst(msg_ps->vb_ptr->ident_len, msg_ps->vb_ptr->ident))
            {
                mn = snmp_search_tree((struct mib_node *)&internet, msg_ps->vb_ptr->ident_len - 4,
                                      msg_ps->vb_ptr->ident + 4, &np);
                if (mn != NULL)
                {
                    if (mn->node_type == MIB_NODE_EX)
                    {
                        struct mib_external_node *en = (struct mib_external_node *)mn;
                        msg_ps->state = SNMP_MSG_EXTERNAL_GET_OBJDEF_S;
                        msg_ps->ext_mib_node = en;
                        msg_ps->ext_name_ptr = np;
                        en->get_object_def_q(en->addr_inf, request_id, np.ident_len, np.ident);
                    }
                    else
                    {
                        struct obj_def object_def;
                        msg_ps->state = SNMP_MSG_INTERNAL_GET_OBJDEF_S;
                        mn->get_object_def(np.ident_len, np.ident, &object_def);
                        msg_ps->state = SNMP_MSG_INTERNAL_SET_VALUE;
                        mn->set_value(&object_def, msg_ps->vb_ptr->value_len, msg_ps->vb_ptr->value);
                        msg_ps->vb_idx += 1;
                    }
                }
            }
        }
    }

    if ((msg_ps->state == SNMP_MSG_INTERNAL_SET_VALUE) && (msg_ps->vb_idx == msg_ps->invb.count))
    {
        GET_FREE_VB_MEM_BUF(vb);
        if (vb != NULL)
        {
            vb->next = NULL;
            vb->prev = NULL;
            vb->ident = msg_ps->invb.head->ident; // Echo first OID
            vb->ident_len = msg_ps->invb.head->ident_len;
            vb->value_type = SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR;

            /* Check voltage validity for custom OID 1 */
            valid_voltage = 1;
            if (match == 0)
            {
                if (msg_ps->vb_ptr->value_type == (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG) &&
                    msg_ps->vb_ptr->value_len == sizeof(s32_t))
                {
                    s32_t voltage = *(s32_t *)msg_ps->vb_ptr->value;
                    if (voltage != 24 && voltage != 30 && voltage != 48 && voltage != 56)
                        valid_voltage = 0;
                }
                else
                {
                    valid_voltage = 0;
                }
            }

            if (valid_voltage)
            {
                vb->value_len = strlen("{\"success\": true}");
                GET_FREE_VB_VAL_BUF(vb->value);
                if (vb->value != NULL)
                {
                    memcpy(vb->value, "{\"success\": true}", vb->value_len);
                    msg_ps->outvb = msg_ps->invb;
                    msg_ps->invb.head = NULL;
                    msg_ps->invb.tail = NULL;
                    msg_ps->invb.count = 0;
                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    snmp_ok_response(msg_ps);
                }
                else
                {
                    DBG_SNMP_PRINTF(("snmp_msg_set_event: couldn't allocate value space\n"));
                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                }
            }
            else
            {
                vb->value_len = strlen("{\"success\": false, \"reason\": \"voltage invalid\"}");
                GET_FREE_VB_VAL_BUF(vb->value);
                if (vb->value != NULL)
                {
                    memcpy(vb->value, "{\"success\": false, \"reason\": \"voltage invalid\"}", vb->value_len);
                    msg_ps->outvb = msg_ps->invb;
                    msg_ps->invb.head = NULL;
                    msg_ps->invb.tail = NULL;
                    msg_ps->invb.count = 0;
                    snmp_varbind_tail_add(&msg_ps->outvb, vb);
                    snmp_ok_response(msg_ps);
                }
                else
                {
                    DBG_SNMP_PRINTF(("snmp_msg_set_event: couldn't allocate value space\n"));
                    snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
                }
            }
        }
        else
        {
            DBG_SNMP_PRINTF(("snmp_msg_set_event: couldn't allocate varbind space\n"));
            snmp_error_response(msg_ps, SNMP_ES_TOOBIG);
        }
    }
}

/**
 * Handle one internal or external event.
 * Called for one async event. (recv external/private answer)
 *
 * @param request_id identifies requests from 0 to (SNMP_CONCURRENT_REQUESTS-1)
 */
/*
 * Function: snmp_msg_event
 * ------------------------
 * Dispatches SNMP message events to appropriate handlers.
 * Routes GET, GETNEXT, GETBULK, and SET requests.
 * Called from snmp_recv after initial message parsing.
 */
void snmp_msg_event(u8_t request_id)
{
    struct snmp_msg_pstat *msg_ps;

    DBG_SNMP_PRINTF(("[%s][%d][request_id=%d]\n", __FILE__, (uint16)__LINE__,
                     (uint16)request_id));

    if (request_id < SNMP_CONCURRENT_REQUESTS)
    {
        msg_ps = &msg_input_list[request_id];

        if (msg_ps->rt == SNMP_ASN1_PDU_GET_NEXT_REQ)
        {
            snmp_msg_getnext_event(request_id, msg_ps);
        }
        else if (msg_ps->rt == SNMP_ASN1_PDU_GET_REQ)
        {
            snmp_msg_get_event(request_id, msg_ps);
        }
        else if (msg_ps->rt == SNMP_ASN1_PDU_GET_BULK_REQ)
        {
            snmp_msg_getbulk_event(msg_ps);
        }
        else if (msg_ps->rt == SNMP_ASN1_PDU_SET_REQ)
        {
            snmp_msg_set_event(request_id, msg_ps);
        }
    }
}


/* lwIP UDP receive callback function */
/*
 * Function: snmp_recv
 * -------------------
 * Receives and processes SNMP messages from UDP port 161.
 * Parses incoming SNMP PDU, validates community, and decodes varbinds.
 * Allocates message processing state and triggers event handler.
 */
void snmp_recv(int32 socketId, u8_t *pData, uint32 length, ip_addr_t ip, uint16 port) reentrant
{
    struct snmp_msg_pstat *msg_ps;
    u8_t req_idx;
    u8_t trap_dip[4] = {0};
    int i = 0;

    // printf("\n snmp triggered  \n");
    snmp_trap_dst_ip_get(0, &trap_dip);

    /* Traverse input message process list, look for SNMP_MSG_EMPTY */
    msg_ps = &msg_input_list[0];
    req_idx = 0;

    while ((req_idx < SNMP_CONCURRENT_REQUESTS) && (msg_ps->state != SNMP_MSG_EMPTY))
    {
        req_idx++;
        msg_ps++;
    }
    if (req_idx != SNMP_CONCURRENT_REQUESTS)
    {
        err_t err_ret;
        u16_t payload_len;
        u16_t varbind_ofs = 0;

        /* Accepting request */
        msg_ps->socketId = socketId;
        IPADDR_COPY(msg_ps->sip.addr, ip.addr);
        msg_ps->sp = port;
        payload_len = length;

        /* Check total length, version, community, PDU type */
        err_ret = snmp_pdu_header_check(pData, 0, payload_len, &varbind_ofs, msg_ps);

        if (((msg_ps->rt == SNMP_ASN1_PDU_GET_REQ) ||
             (msg_ps->rt == SNMP_ASN1_PDU_GET_NEXT_REQ) ||
             (msg_ps->rt == SNMP_ASN1_PDU_SET_REQ)) &&
                ((msg_ps->error_status == SNMP_ES_NOERROR) &&
                 (msg_ps->error_index == 0)) ||
            (msg_ps->rt == SNMP_ASN1_PDU_GET_BULK_REQ))
        {
            err_ret = err_ret;
        }
        else
        {
            err_ret = ERR_ARG;
        }

        if (err_ret == ERR_OK)
        {
            /* Decode variable bindings */
            err_ret = snmp_pdu_dec_varbindlist(pData, varbind_ofs, &varbind_ofs, msg_ps);

            if ((err_ret == ERR_OK) && (msg_ps->invb.count > 0))
            {
                if (msg_ps->rt != SNMP_ASN1_PDU_GET_BULK_REQ)
                {
                    msg_ps->error_status = SNMP_ES_NOERROR;
                    msg_ps->error_index = 0;
                }
                /* Find object for each variable binding */
                msg_ps->state = SNMP_MSG_SEARCH_OBJ;
                msg_ps->vb_idx = 0;

                /* Handle input event */
                snmp_msg_event(req_idx);
            }
            else
            {
                DBG_SNMP_PRINTF(("snmp_pdu_dec_varbindlist() failed\n"));
                // Make sure to reset state and clean up resources
                msg_ps->state = SNMP_MSG_EMPTY;
                RESET_VB_BUFS();
            }
            if (err_ret == ERR_ARG)
            {
                // Reset state and clean up resources
                msg_ps->state = SNMP_MSG_EMPTY;
                RESET_VB_BUFS();
            }
        }
        else
        {
            DBG_SNMP_PRINTF(("[%s][%d]snmp_pdu_header_check() failed\n", __FILE__, (uint16)__LINE__));
            // Reset state and clean up resources
            msg_ps->state = SNMP_MSG_EMPTY;
            RESET_VB_BUFS();
        }
    }
}

/*
 * Function: _snmp_community_match
 * -------------------------------
 * Validates SNMP community string for access control.
 * Checks read community for GET/GETNEXT, write community for SET.
 * Triggers authentication trap if community fails.
 */
static u8_t
_snmp_community_match(u8_t pdu_type, char *community)
{
    if (pdu_type == SNMP_ASN1_PDU_GET_REQ || pdu_type == SNMP_ASN1_PDU_GET_NEXT_REQ)
    {
        if ((strncmp(snmp_read_community, community, SNMP_COMMUNITY_STR_LEN) == 0) ||
            (strncmp(snmp_write_community, community, SNMP_COMMUNITY_STR_LEN) == 0))
        {
            return TRUE;
        }
    }
    else if (pdu_type == SNMP_ASN1_PDU_SET_REQ)
    {
        if (strncmp(snmp_write_community, community, SNMP_COMMUNITY_STR_LEN) == 0)
        {
            return TRUE;
        }
    }

#if LWPS_SNMP_TRAP_ENABLE
    snmp_authfail_trap();
#endif

    return FALSE;
}

/**
 * Checks and decodes incoming SNMP message header, logs header errors.
 *
 * @param p points to pbuf chain of SNMP message (UDP payload)
 * @param ofs points to first octet of SNMP message
 * @param pdu_len the length of the UDP payload
 * @param ofs_ret returns the ofset of the variable bindings
 * @param m_stat points to the current message request state return
 * @return
 * - ERR_OK SNMP header is sane and accepted
 * - ERR_ARG SNMP header is either malformed or rejected
 */
/*
 * Function: snmp_pdu_header_check
 * -------------------------------
 * Decodes and validates SNMP message header from raw data.
 * Parses version, community, PDU type, request ID, error status/index.
 * Returns ERR_OK if header is valid, ERR_ARG otherwise.
 */
// lwps_handler_t  snmpHandler;

static err_t
// snmp_pdu_header_check(struct pbuf *p, u16_t ofs, u16_t pdu_len, u16_t *ofs_ret, struct snmp_msg_pstat *m_stat)
snmp_pdu_header_check(u8_t *p, u16_t ofs, u16_t pdu_len, u16_t *ofs_ret, struct snmp_msg_pstat *m_stat)
{

    err_t derr;
    u16_t len, ofs_base;
    u8_t len_octets;
    u8_t type;
    s32_t version;

    ofs_base = ofs;
    snmp_asn1_dec_type(p, ofs, &type);

    derr = snmp_asn1_dec_length(p, ofs + 1, &len_octets, &len);

    if ((derr != ERR_OK) ||
        (pdu_len != (1 + len_octets + len)) ||
        (type != (SNMP_ASN1_UNIV | SNMP_ASN1_CONSTR | SNMP_ASN1_SEQ)))
    {
        return ERR_ARG;
    }
    ofs += (1 + len_octets);
    snmp_asn1_dec_type(p, ofs, &type);
    derr = snmp_asn1_dec_length(p, ofs + 1, &len_octets, &len);
    if ((derr != ERR_OK) || (type != (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG)))
    {
        /* can't decode or no integer (version) */
        return ERR_ARG;
    }
    derr = snmp_asn1_dec_s32t(p, ofs + 1 + len_octets, len, &version);
    if (derr != ERR_OK)
    {
        /* can't decode */
        return ERR_ARG;
    }
    if (version < 0 || version > 1)
    {
        /* only support version 0 (SNMPv1) and version 1 (SNMPv2c) */
        return ERR_ARG;
    }
    m_stat->snmp_version = version;
    snmp_version = version;

    ofs += (1 + len_octets + len);
    snmp_asn1_dec_type(p, ofs, &type);

    derr = snmp_asn1_dec_length(p, ofs + 1, &len_octets, &len);
    if ((derr != ERR_OK) || (type != (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR)))
    {
        /* can't decode or no octet string (community) */
        return ERR_ARG;
    }

    derr = snmp_asn1_dec_raw(p, ofs + 1 + len_octets, len, SNMP_COMMUNITY_STR_LEN, m_stat->community);
    if (derr != ERR_OK)
    {
        return ERR_ARG;
    }

    len = ((len < (SNMP_COMMUNITY_STR_LEN)) ? (len) : (SNMP_COMMUNITY_STR_LEN));
    m_stat->community[len] = 0;
    m_stat->com_strlen = len;

    ofs += (1 + len_octets + len);

    snmp_asn1_dec_type(p, ofs, &type);

    derr = snmp_asn1_dec_length(p, ofs + 1, &len_octets, &len);

    if (derr != ERR_OK)
    {
        return ERR_ARG;
    }

    switch (type)
    {
    case (SNMP_ASN1_CONTXT | SNMP_ASN1_CONSTR | SNMP_ASN1_PDU_GET_REQ):
        /* GetRequest PDU */
        if (!_snmp_community_match(SNMP_ASN1_PDU_GET_REQ, m_stat->community))
            return ERR_ARG;
        derr = ERR_OK;
        break;
    case (SNMP_ASN1_CONTXT | SNMP_ASN1_CONSTR | SNMP_ASN1_PDU_GET_NEXT_REQ):
        /* GetNextRequest PDU */
        if (!_snmp_community_match(SNMP_ASN1_PDU_GET_NEXT_REQ, m_stat->community))
        {
            if (m_stat->snmp_version == 0) /*v1 */
                snmp_error_response(m_stat, SNMP_ES_NOSUCHNAME);
            else /*v2c*/
                snmp_error_response_v2(m_stat, SNMP_ES_notWritable, 1);
            return ERR_ARG;
        }
        derr = ERR_OK;
        break;
    case (SNMP_ASN1_CONTXT | SNMP_ASN1_CONSTR | SNMP_ASN1_PDU_GET_RESP):
        /* GetResponse PDU */
        derr = ERR_ARG;
        break;
    case (SNMP_ASN1_CONTXT | SNMP_ASN1_CONSTR | SNMP_ASN1_PDU_SET_REQ):
        /* SetRequest PDU */
        if (!_snmp_community_match(SNMP_ASN1_PDU_SET_REQ, m_stat->community))
            return ERR_ARG;
        derr = ERR_OK;
        break;
    case (SNMP_ASN1_CONTXT | SNMP_ASN1_CONSTR | SNMP_ASN1_PDU_TRAP):
        /* Trap PDU */
        derr = ERR_ARG;
        break;
    case (SNMP_ASN1_CONTXT | SNMP_ASN1_CONSTR | SNMP_ASN1_PDU_GET_BULK_REQ):
        /* GetBulkRequest PDU */
        derr = ERR_OK;
        break;
    default:
        derr = ERR_ARG;
        break;
    }
    if (derr != ERR_OK)
    {
        /* unsupported input PDU for this agent (no parse error) */
        return ERR_ARG;
    }
    m_stat->rt = type & 0x1F;
    ofs += (1 + len_octets);
    if (len != (pdu_len - (ofs - ofs_base)))
    {
        /* decoded PDU length does not equal actual payload length */
        return ERR_ARG;
    }
    snmp_asn1_dec_type(p, ofs, &type);
    derr = snmp_asn1_dec_length(p, ofs + 1, &len_octets, &len);
    if ((derr != ERR_OK) || (type != (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG)))
    {
        /* can't decode or no integer (request ID) */
        return ERR_ARG;
    }
    derr = snmp_asn1_dec_s32t(p, ofs + 1 + len_octets, len, &m_stat->rid);

    if (derr != ERR_OK)
    {
        /* can't decode */
        return ERR_ARG;
    }
    ofs += (1 + len_octets + len);
    snmp_asn1_dec_type(p, ofs, &type);
    derr = snmp_asn1_dec_length(p, ofs + 1, &len_octets, &len);
    if ((derr != ERR_OK) || (type != (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG)))
    {
        /* can't decode or no integer (error-status) */
        return ERR_ARG;
    }
    /* must be noError (0) for incoming requests.
       log errors for mib-2 completeness and for debug purposes */
    derr = snmp_asn1_dec_s32t(p, ofs + 1 + len_octets, len, &m_stat->error_status);
    if (derr != ERR_OK)
    {
        /* can't decode */
        return ERR_ARG;
    }

    ofs += (1 + len_octets + len);
    snmp_asn1_dec_type(p, ofs, &type);
    derr = snmp_asn1_dec_length(p, ofs + 1, &len_octets, &len);
    if ((derr != ERR_OK) || (type != (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG)))
    {
        /* can't decode or no integer (error-index) */
        return ERR_ARG;
    }
    /* must be 0 for incoming requests.
       decode anyway to catch bad integers (and dirty tricks) */
    derr = snmp_asn1_dec_s32t(p, ofs + 1 + len_octets, len, &m_stat->error_index);
    if (derr != ERR_OK)
    {
        /* can't decode */
        return ERR_ARG;
    }
    ofs += (1 + len_octets + len);
    *ofs_ret = ofs;
    return ERR_OK;
}

/*
 * Function: snmp_pdu_dec_varbindlist
 * ----------------------------------
 * Decodes variable bindings from SNMP PDU payload.
 * Parses OID and value pairs into varbind list structure.
 * Handles multiple ASN.1 types (integer, counter, string, null, etc.).
 */
static err_t
snmp_pdu_dec_varbindlist(u8_t *p, u16_t ofs, u16_t *ofs_ret, struct snmp_msg_pstat *m_stat)
{
    err_t derr;
    u16_t len, vb_len;
    u8_t len_octets;
    u8_t type;
    int i = 0;
    /* variable binding list */
    snmp_asn1_dec_type(p, ofs, &type);
    derr = snmp_asn1_dec_length(p, ofs + 1, &len_octets, &vb_len);
    if ((derr != ERR_OK) ||
        (type != (SNMP_ASN1_UNIV | SNMP_ASN1_CONSTR | SNMP_ASN1_SEQ)))
    {
        return ERR_ARG;
    }
    ofs += (1 + len_octets);

    /* start with empty list */
    m_stat->invb.count = 0;
    m_stat->invb.head = NULL;
    m_stat->invb.tail = NULL;

    while (vb_len > 0)
    {
        struct snmp_obj_id oid, oid_value;
        struct snmp_varbind *vb;

        snmp_asn1_dec_type(p, ofs, &type);
        derr = snmp_asn1_dec_length(p, ofs + 1, &len_octets, &len);
        if ((derr != ERR_OK) ||
            (type != (SNMP_ASN1_UNIV | SNMP_ASN1_CONSTR | SNMP_ASN1_SEQ)) ||
            (len <= 0) || (len > vb_len))
        {
            /* free varbinds (if available) */
            snmp_varbind_list_free(&m_stat->invb);
            return ERR_ARG;
        }
        ofs += (1 + len_octets);
        vb_len -= (1 + len_octets);

        snmp_asn1_dec_type(p, ofs, &type);
        derr = snmp_asn1_dec_length(p, ofs + 1, &len_octets, &len);
        if ((derr != ERR_OK) || (type != (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OBJ_ID)))
        {
            /* can't decode object name length */
            /* free varbinds (if available) */
            snmp_varbind_list_free(&m_stat->invb);
            return ERR_ARG;
        }
        derr = snmp_asn1_dec_oid(p, ofs + 1 + len_octets, len, &oid);

        if (derr != ERR_OK)
        {
            /* can't decode object name */
            /* free varbinds (if available) */
            snmp_varbind_list_free(&m_stat->invb);
            return ERR_ARG;
        }

        ofs += (1 + len_octets + len);
        vb_len -= (1 + len_octets + len);

        snmp_asn1_dec_type(p, ofs, &type);

        derr = snmp_asn1_dec_length(p, ofs + 1, &len_octets, &len);

        if (derr != ERR_OK)
        {
            /* can't decode object value length */
            /* free varbinds (if available) */
            snmp_varbind_list_free(&m_stat->invb);
            return ERR_ARG;
        }

        switch (type)
        {
        case (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG):
            vb = snmp_varbind_alloc(&oid, type, sizeof(s32_t));
            if (vb != NULL)
            {
                s32_t *vptr = vb->value;

                derr = snmp_asn1_dec_s32t(p, ofs + 1 + len_octets, len, vptr);
                snmp_varbind_tail_add(&m_stat->invb, vb);
            }
            else
            {
                derr = ERR_ARG;
            }
            break;
        case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER64):
            /*TODO:  counter64 decode support. Currently, we just change it to COUNT32 */
        case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER):
        case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_GAUGE):
        case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_TIMETICKS):
            vb = snmp_varbind_alloc(&oid, type, sizeof(u32_t));
            if (vb != NULL)
            {
                u32_t *vptr = vb->value;

                derr = snmp_asn1_dec_u32t(p, ofs + 1 + len_octets, len, vptr);
                snmp_varbind_tail_add(&m_stat->invb, vb);
            }
            else
            {
                derr = ERR_ARG;
            }
            break;
        case (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR):
        case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_OPAQUE):
            vb = snmp_varbind_alloc(&oid, type, len);
            if (vb != NULL)
            {
                derr = snmp_asn1_dec_raw(p, ofs + 1 + len_octets, len, vb->value_len, vb->value);
                snmp_varbind_tail_add(&m_stat->invb, vb);
            }
            else
            {
                derr = ERR_ARG;
            }
            break;
        case (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_NUL):

            vb = snmp_varbind_alloc(&oid, type, 0);
            if (vb != NULL)
            {
                snmp_varbind_tail_add(&m_stat->invb, vb);
                derr = ERR_OK;
            }
            else
            {
                derr = ERR_ARG;
            }
            break;
        case (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OBJ_ID):
            derr = snmp_asn1_dec_oid(p, ofs + 1 + len_octets, len, &oid_value);
            if (derr == ERR_OK)
            {
                vb = snmp_varbind_alloc(&oid, type, oid_value.len * sizeof(s32_t));
                if (vb != NULL)
                {
                    u8_t i = oid_value.len;
                    s32_t *vptr = vb->value;

                    while (i > 0)
                    {
                        i--;
                        vptr[i] = oid_value.id[i];
                    }
                    snmp_varbind_tail_add(&m_stat->invb, vb);
                    derr = ERR_OK;
                }
                else
                {
                    derr = ERR_ARG;
                }
            }
            break;
        case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_IPADDR):
            if (len == 4)
            {
                /* must be exactly 4 octets! */
                vb = snmp_varbind_alloc(&oid, type, 4);
                if (vb != NULL)
                {
                    derr = snmp_asn1_dec_raw(p, ofs + 1 + len_octets, len, vb->value_len, vb->value);
                    snmp_varbind_tail_add(&m_stat->invb, vb);
                }
                else
                {
                    derr = ERR_ARG;
                }
            }
            else
            {
                derr = ERR_ARG;
            }
            break;
        default:
            derr = ERR_ARG;
            break;
        }
        if (derr != ERR_OK)
        {
            /* free varbinds (if available) */
            snmp_varbind_list_free(&m_stat->invb);
            return ERR_ARG;
        }
        ofs += (1 + len_octets + len);
        vb_len -= (1 + len_octets + len);
    }

    *ofs_ret = ofs;
    return ERR_OK;
}

/*
 * Function: snmp_varbind_duplicate
 * --------------------------------
 * Creates a duplicate copy of a varbind structure.
 * Allocates new memory for ident and value arrays.
 * Used for GETBULK operations to copy varbind entries.
 */
struct snmp_varbind *snmp_varbind_duplicate(struct snmp_varbind *src_vb)
{
    struct snmp_varbind *vb;
    GET_FREE_VB_MEM_BUF(vb); // = (struct snmp_varbind *) mem_malloc(sizeof(struct snmp_varbind));
    if (vb != NULL)
    {
        vb->next = src_vb->next;
        vb->prev = src_vb->prev;
        vb->ident_len = src_vb->ident_len;
        if (vb->ident_len > 0)
        {
            GET_FREE_VB_IDENT_BUF(vb->ident); // = (s32_t *) mem_malloc(sizeof(s32_t) * vb->ident_len);
            if (vb->ident == NULL)
            {
                //   mem_free(vb);
                return NULL;
            }
            memcpy(vb->ident, src_vb->ident, sizeof(s32_t) * vb->ident_len);
        }
        else
        {
            /* i == 0, pass zero length object identifier */
            vb->ident = NULL;
        }
        vb->value_type = src_vb->value_type;
        vb->value_len = src_vb->value_len;
        if (vb->value_len > 0)
        {
            /* allocate raw bytes for our object value */
            GET_FREE_VB_VAL_BUF(vb->value); // = mem_malloc(vb->value_len);
            // LWIP_ASSERT("vb->value != NULL",vb->value != NULL);
            if (vb->value == NULL)
            {
                if (vb->ident != NULL)
                {
                    //     mem_free(vb->ident);
                }
                //  mem_free(vb);
                return NULL;
            }
            else
            {
                memcpy(vb->value, src_vb->value, vb->value_len);
            }
        }
        else
        {
            /* ASN1_NUL type, or zero length ASN1_OC_STR */
            vb->value = NULL;
        }
    }

    return vb;
}

/*
 * Function: snmp_varbind_alloc
 * ----------------------------
 * Allocates a new varbind structure with OID and value buffer.
 * Copies OID into newly allocated ident array.
 * Allocates value buffer if length > 0.
 * Returns pointer to allocated varbind or NULL on failure.
 */
struct snmp_varbind *
snmp_varbind_alloc(struct snmp_obj_id *oid, u8_t type, u8_t len)
{
    struct snmp_varbind *vb;

    //  vb = (struct snmp_varbind *)mem_malloc(sizeof(struct snmp_varbind));
    GET_FREE_VB_MEM_BUF(vb);
    memset(vb, 0x00, sizeof(struct snmp_varbind));
    LWIP_ASSERT("vb != NULL", vb != NULL);
    if (vb != NULL)
    {
        u8_t i;

        vb->next = NULL;
        vb->prev = NULL;
        i = oid->len;
        vb->ident_len = i;
        if (i > 0)
        {
            /* allocate array of s32_t for our object identifier */
            //      vb->ident = (s32_t*)mem_malloc(sizeof(s32_t) * i);
            GET_FREE_VB_IDENT_BUF(vb->ident); // = &(vb_ident_list.vbIdentBuf[freeIdx]);
            memset(vb->ident, 0x00, sizeof(struct vb_ident_buf_s));
            LWIP_ASSERT("vb->ident != NULL", vb->ident != NULL);
            if (vb->ident == NULL)
            {
                //        mem_free(vb);
                return NULL;
            }
            while (i > 0)
            {
                i--;
                vb->ident[i] = oid->id[i];
            }
        }
        else
        {
            /* i == 0, pass zero length object identifier */
            vb->ident = NULL;
        }
        vb->value_type = type;
        vb->value_len = len;
        if (len > 0)
        {
            /* allocate raw bytes for our object value */
            //      vb->value = mem_malloc(len);
            GET_FREE_VB_VAL_BUF(vb->value); // = &(vb_value_list.vbValueBuf[freeIdx]);
            memset(vb->value, 0x00, sizeof(struct vb_value_buf_s));
            LWIP_ASSERT("vb->value != NULL", vb->value != NULL);
            if (vb->value == NULL)
            {
                if (vb->ident != NULL)
                {
                    //        mem_free(vb->ident);
                }
                //    mem_free(vb);
                return NULL;
            }
        }
        else
        {
            /* ASN1_NUL type, or zero length ASN1_OC_STR */
            vb->value = NULL;
        }
    }
    return vb;
}

/*
 * Function: snmp_varbind_free
 * ---------------------------
 * Frees memory allocated for varbind structure.
 * Currently disabled (commented out) for memory optimization.
 * Placeholder for proper varbind cleanup.
 */
void snmp_varbind_free(struct snmp_varbind *vb)
{
#if 0
  if (vb->value != NULL )
  {
    mem_free(vb->value);
  }
  if (vb->ident != NULL )
  {
    mem_free(vb->ident);
  }
  mem_free(vb);
#endif
}

/*
 * Function: snmp_varbind_list_free
 * --------------------------------
 * Frees entire varbind list from root node.
 * Traverses list from tail to head freeing each varbind.
 * Resets list count, head, and tail pointers.
 */
void snmp_varbind_list_free(struct snmp_varbind_root *root)
{
    struct snmp_varbind *vb, *prev;

    vb = root->tail;
    while (vb != NULL)
    {
        prev = vb->prev;
        snmp_varbind_free(vb);
        vb = prev;
    }
    root->count = 0;
    root->head = NULL;
    root->tail = NULL;
}

/*
 * Function: snmp_varbind_tail_add
 * -------------------------------
 * Adds a varbind to the tail of the list.
 * Handles empty list case by setting head and tail.
 * Updates next/prev pointers and increments count.
 */
void snmp_varbind_tail_add(struct snmp_varbind_root *root, struct snmp_varbind *vb)
{
    if (root->count == 0)
    {
        /* add first varbind to list */
        root->head = vb;
        root->tail = vb;
    }
    else
    {
        DBG_SNMP_PRINTF(("[%s][%d][vb=0x%04x]\n",
                         __FILE__, (uint16)__LINE__, (uint16)vb));
        /* add nth varbind to list tail */
        root->tail->next = vb;
        vb->prev = root->tail;
        root->tail = vb;
    }
    root->count += 1;
}

#if 0 /* uncalled function */
struct snmp_varbind*
snmp_varbind_tail_remove(struct snmp_varbind_root *root)
{
  struct snmp_varbind* vb;

  if (root->count > 0)
  {
    /* remove tail varbind */
    vb = root->tail;
    root->tail = vb->prev;
    vb->prev->next = NULL;
    root->count -= 1;
  }
  else
  {
    /* nothing to remove */
    vb = NULL;
  }
  return vb;
}
#endif

#endif /* LWIP_SNMP */