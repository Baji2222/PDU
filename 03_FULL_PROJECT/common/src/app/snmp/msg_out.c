#pragma STRING (far)
/**
 * @file
 * SNMP output message processing (RFC1157).
 *
 * Output responses and traps are build in two passes:
 *
 * Pass 0: iterate over the output message backwards to determine encoding lengths
 * Pass 1: the actual forward encoding of internal form into ASN1
 *
 * The single-pass encoding method described by Comer & Stevens
 * requires extra buffer space and copying for reversal of the packet.
 * The buffer requirement can be prohibitively large for big payloads
 * (>= 484) therefore we use the two encoding passes.
 */

/*
 * Copyright (c) 2006 Axon Digital Design B.V., The Netherlands.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Author: Christiaan Simons <christiaan.simons@axon.tv>
 */

#include <local/config.h>
#include <snmp/opt.h>
#include <mem/mem_lib.h>

#if LWIP_SNMP /* don't build if not configured for use in lwipopts.h */

//#include "lwip/udp.h"
#include <snmp/snmp.h>
#include <snmp/snmp_asn1.h>
#include <snmp/snmp_msg.h>
#include <snmp/snmp_debug.h>
#include <lwps/lwps_api.h>
#include <saltype.h>
#include <syslog/syslogdefs.h>

#define INVALID_U8_VALUE 255

extern uint8 Get_Response[200];   /* FIX: must match web_user.c */
u32_t linkChangeTime[COMBO_PORT_NUM] = {0, };
u8_t snmp_authfail_trap_enable = TRUE;

portmask_type snmp_keep_pm = 0;
//portmask_type snmp_next_pm = 0;
portmask_type snmp_new_linkdown_pm;
portmask_type snmp_new_linkup_pm;

u8_t portPoe = 0; /* Global variable for PoE port (port 3) status: 1 = up, 0 = down */
u8_t portLan = 0; /* Global variable for LAN port (port 4) status: 1 = up, 0 = down */

/** Agent Version constant, 0 = v1 oddity */
extern s32_t   snmp_version;
extern struct vb_mem_buf_list_s xdata vb_mem_buf_list;
extern struct vb_value_list_s  xdata vb_value_list;
extern struct vb_ident_list_s xdata vb_ident_list; 

#if LWPS_SNMP_TRAP_ENABLE

struct snmp_trap_dst
{
  /* destination IP address in network order */
  struct ip_addr_s dip;
  /* set to 0 when disabled, >0 when enabled */
  u8_t enable;
};
struct snmp_trap_dst trap_dst[SNMP_TRAP_DESTINATIONS];
/** TRAP message structure */
struct snmp_msg_trap trap_msg;

u8_t  linkUpDownTrapEnable[COMBO_PORT_NUM] = {1,1,1,1,1,1,1,1};

#define SNMP_TRAP_TYPE_MAXIMAL      6   //4
#define SNMP_TRAP_TYPE_ADD          1
#define SNMP_TRAP_TYPE_COLDSTART    (SNMP_GENTRAP_COLDSTART+SNMP_TRAP_TYPE_ADD)
#define SNMP_TRAP_TYPE_AUTHFAIL     (SNMP_GENTRAP_AUTHFAIL+SNMP_TRAP_TYPE_ADD)
#define SNMP_TRAP_TYPE_LINKDOWN     (SNMP_GENTRAP_LINKDOWN+SNMP_TRAP_TYPE_ADD)
#define SNMP_TRAP_TYPE_LINKUP       (SNMP_GENTRAP_LINKUP+SNMP_TRAP_TYPE_ADD)
#define SNMP_TRAP_TYPE_BATT_LOW     (SNMP_GENTRAP_ENTERPRISESPC+SNMP_TRAP_TYPE_ADD) 
#define SNMP_TRAP_TYPE_AC_STATUS    (SNMP_GENTRAP_ENTERPRISESPC+SNMP_TRAP_TYPE_ADD+1) 

u8_t snmp_trap_msg_queue[SNMP_TRAP_TYPE_MAXIMAL] = 0;
s8_t snmp_trap_msg_index = -1;
/* 1.3.6.1.2.1.2.2.1.1.x */
s32_t interface_oid[] = {1, 3, 6, 1, 2, 1, 2, 2, 1, 1};
u8_t snmp_trap_enable = FALSE;

static u16_t snmp_trap_header_sum(struct snmp_msg_trap *m_trap, u16_t vb_len);
static u16_t snmp_trap_header_enc(struct snmp_msg_trap *m_trap, u8_t *p);

#endif /* end of LWPS_SNMP_TRAP_ENABLE */

lwps_handler_t  snmpHandler;



static u16_t snmp_resp_header_sum(struct snmp_msg_pstat *m_stat, u16_t vb_len);
static u16_t snmp_varbind_list_sum(struct snmp_varbind_root *root);
void snmp_battery_low_trap(void);
void snmp_ac_status_trap(uint8 ac_status);
u8_t extract_a_value(const char *json_str);
void snmp_add_trap(u8_t trap_type);
static u16_t snmp_resp_header_enc(struct snmp_msg_pstat *m_stat, u8_t *p);
static u16_t snmp_varbind_list_enc(struct snmp_varbind_root *root, u8_t *p, u16_t ofs);
char bufferC[4] = {0}; // Buffer to store the number

// The fixed function from earlier
/*
 * Function: extract_c_value
 * -------------------------
 * Extracts battery percentage value from JSON string.
 * Searches for "C": pattern and returns integer value.
 * Returns -1 if key not found or value invalid.
 */
u8_t extract_c_value(const char *json_str) {
    u8_t value = 0;
    const char *key = "\"C\":";
    char *key_pos = strstr(json_str, key);
    if (key_pos == NULL) {
        return -1; // Key not found
    }

    key_pos += strlen(key);
    while (*key_pos == ' ' || *key_pos == '\t') {
        key_pos++;
    }

    if (*key_pos >= '0' && *key_pos <= '9') {
        
        int i = 0;
        // Copy digits until a non-digit is encountered
        while (*key_pos >= '0' && *key_pos <= '9' && i < sizeof(bufferC) - 1) {
            bufferC[i++] = *key_pos++;
        }
        bufferC[i] = '\0'; // Null-terminate the string
				printf("battery Percentage: %s\r\n",bufferC);
        value = atoi(bufferC);
    } else {
        return -1; // Invalid value
    }

    return value;
}



/**
 * Sends a 'getresponse' message to the request originator.
 *
 * @param m_stat points to the current message request state source
 * @return ERR_OK when success, ERR_MEM if we're out of memory
 *
 * @note the caller is responsible for filling in outvb in the m_stat
 * and provide error-status and index (except for tooBig errors) ...
 */
 
/*
 * Function: snmp_send_response
 * ----------------------------
 * Sends SNMP response message to request originator.
 * Performs two-pass encoding: pass 0 calculates lengths, pass 1 encodes.
 * Handles tooBig errors by retrying with empty varbind list.
 */
err_t
snmp_send_response(struct snmp_msg_pstat *m_stat)
{
  struct snmp_varbind_root  emptyvb = {NULL, NULL, 0, 0, 0};
  int32  socketId;
  u8_t   *p;
  u16_t  tot_len;
  err_t  err;
  u16_t retval;

  DBG_SNMP_PRINTF(("[%s][%d][m_stat->error_status=0x%04x]\n", \
	  	__FILE__, (uint16)__LINE__, (uint16)m_stat->error_status));
  /* pass 0, calculate length fields */
  tot_len = snmp_varbind_list_sum(&m_stat->outvb);
  tot_len = snmp_resp_header_sum(m_stat, tot_len);
 /* try allocating pbuf(s) for complete response */
  p = malloc_tx_buf(1518UL);
  
  if (p == NULL)
  {
    DBG_SNMP_PRINTF(("snmp_snd_response() tooBig\n"));

    /* can't construct reply, return error-status tooBig */
    m_stat->error_status = SNMP_ES_TOOBIG;
    m_stat->error_index = 0;
    /* pass 0, recalculate lengths, for empty varbind-list */
    tot_len = snmp_varbind_list_sum(&emptyvb);
    tot_len = snmp_resp_header_sum(m_stat, tot_len);
    /* retry allocation once for header and empty varbind-list */
  }
  if (p != NULL)
  {
    /* first pbuf alloc try or retry alloc success */
    u16_t ofs;

    /* pass 1, size error, encode packet ino the pbuf(s) */
	DBG_SNMP_PRINTF(("[%s][%d]\n", __FILE__, (uint16)__LINE__));
    ofs = snmp_resp_header_enc(m_stat, p);
    if (m_stat->error_status == SNMP_ES_TOOBIG)
    {
      snmp_varbind_list_enc(&emptyvb, p, ofs);
    }
    else
    {
      DBG_SNMP_PRINTF(("[%s][%d][m_stat->error_status=0x%04x]\n", \
	  	__FILE__, (uint16)__LINE__, (uint16)m_stat->error_status));
	  
      retval = snmp_varbind_list_enc(&m_stat->outvb, p, ofs);
    }
  /** @todo do we need separate rx and tx pcbs for threaded case? */
    /** connect to the originating source */
	DBG_SNMP_PRINTF(("[%s][%d][tot_len=%d]\n", __FILE__, (uint16)__LINE__, \
	    (uint16)tot_len));
	socketId = lwps_socket(LWPS_L3TYPE_IP, LWPS_L4TYPE_UDP, snmpHandler);
	if (socketId < 0)
	{
	  OSAL_PRINTF("[%s][%d]SNMP Connect Error! alloc socket failed!\n", __FILE__, (uint16)__LINE__);
	  lwps_close(socketId);
	  return ERR_CONN;
	}
	
	if (LWPS_OK != lwps_sendto(socketId, p, tot_len, m_stat->sip, m_stat->sp))
	{
	  OSAL_PRINTF("SNMP Send Error! alloc socket failed!\n");
	  lwps_close(socketId);
	  return ERR_CONN;
	}
	
    if (err == ERR_MEM)
    {
      /** @todo release some memory, retry and return tooBig? tooMuchHassle? */
      err = ERR_MEM;
    }
    else
    {
      err = ERR_OK;
    }
    /** disassociate remote address and port with this pcb */

    lwps_close(socketId);

    DBG_SNMP_PRINTF( ("snmp_snd_response() done\n"));
    return err;
  }
  else
  {
    /* first pbuf alloc try or retry alloc failed
       very low on memory, couldn't return tooBig */
    return ERR_MEM;
  }
}


/**
 * Sums response header field lengths from tail to head and
 * returns resp_header_lengths for second encoding pass.
 *
 * @param vb_len varbind-list length
 * @param rhl points to returned header lengths
 * @return the required lenght for encoding the response header
 */
/*
 * Function: snmp_resp_header_sum
 * ------------------------------
 * Calculates total length for SNMP response header.
 * Sums lengths of version, community, PDU header, and varbind list.
 * Stores individual lengths in response header structure.
 */
static u16_t
snmp_resp_header_sum(struct snmp_msg_pstat *m_stat, u16_t vb_len)
{
  u16_t tot_len;
  struct snmp_resp_header_lengths *rhl;

  rhl = &m_stat->rhl;
  tot_len = vb_len;
  snmp_asn1_enc_s32t_cnt(m_stat->error_index, &rhl->erridxlen);
  snmp_asn1_enc_length_cnt(rhl->erridxlen, &rhl->erridxlenlen);
  tot_len += 1 + rhl->erridxlenlen + rhl->erridxlen;

  snmp_asn1_enc_s32t_cnt(m_stat->error_status, &rhl->errstatlen);
  snmp_asn1_enc_length_cnt(rhl->errstatlen, &rhl->errstatlenlen);
  tot_len += 1 + rhl->errstatlenlen + rhl->errstatlen;

  snmp_asn1_enc_s32t_cnt(m_stat->rid, &rhl->ridlen);
  snmp_asn1_enc_length_cnt(rhl->ridlen, &rhl->ridlenlen);
  tot_len += 1 + rhl->ridlenlen + rhl->ridlen;

  rhl->pdulen = tot_len;
  snmp_asn1_enc_length_cnt(rhl->pdulen, &rhl->pdulenlen);
  tot_len += 1 + rhl->pdulenlen;

  rhl->comlen = m_stat->com_strlen;
  snmp_asn1_enc_length_cnt(rhl->comlen, &rhl->comlenlen);
  tot_len += 1 + rhl->comlenlen + rhl->comlen;

  snmp_asn1_enc_s32t_cnt(snmp_version, &rhl->verlen);
  snmp_asn1_enc_length_cnt(rhl->verlen, &rhl->verlenlen);
  tot_len += 1 + rhl->verlen + rhl->verlenlen;

  rhl->seqlen = tot_len;
  snmp_asn1_enc_length_cnt(rhl->seqlen, &rhl->seqlenlen);
  tot_len += 1 + rhl->seqlenlen;

  return tot_len;
}

/**
 * Sums varbind lengths from tail to head and
 * annotates lengths in varbind for second encoding pass.
 *
 * @param root points to the root of the variable binding list
 * @return the required lenght for encoding the variable bindings
 */
/*
 * Function: snmp_varbind_list_sum
 * -------------------------------
 * Calculates total length for varbind list encoding.
 * Iterates through list from tail to head computing lengths.
 * Stores individual varbind lengths for second pass.
 */
static u16_t
snmp_varbind_list_sum(struct snmp_varbind_root *root) large
{
  struct snmp_varbind *vb;
  u32_t *uint_ptr;
  s32_t *sint_ptr;
  u16_t tot_len;

  DBG_SNMP_PRINTF(("[%s][%d]\n", __FILE__, (uint16)__LINE__));

  tot_len = 0;
  vb = root->tail;
  while ( vb != NULL )
  {
    /* encoded value lenght depends on type */
    switch (vb->value_type)
    {
      case (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG):
        sint_ptr = vb->value;
        snmp_asn1_enc_s32t_cnt(*sint_ptr, &vb->vlen);
        break;
	  case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER64):
	  	/*TODO:  counter64 decode support. Currently, we just change it to COUNT32 */
	  	uint_ptr = vb->value;
		snmp_asn1_enc_u64t_cnt(uint_ptr, &vb->vlen);
		break;
      case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER):
      case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_GAUGE):
      case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_TIMETICKS):
        uint_ptr = vb->value;
        snmp_asn1_enc_u32t_cnt(*uint_ptr, &vb->vlen);
        break;
      case (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR):
      case (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_NUL):
      case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_IPADDR):
      case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_OPAQUE):
        vb->vlen = vb->value_len;
        break;
      case (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OBJ_ID):
        sint_ptr = vb->value;
        snmp_asn1_enc_oid_cnt(vb->value_len / sizeof(s32_t), sint_ptr, &vb->vlen);
        break;
      default:
        /* unsupported type */
        vb->vlen = 0;
        break;
    };
    /* encoding length of value length field */
    snmp_asn1_enc_length_cnt(vb->vlen, &vb->vlenlen);
    snmp_asn1_enc_oid_cnt(vb->ident_len, vb->ident, &vb->olen);
    snmp_asn1_enc_length_cnt(vb->olen, &vb->olenlen);

    vb->seqlen = 1 + vb->vlenlen + vb->vlen;
    vb->seqlen += 1 + vb->olenlen + vb->olen;
    snmp_asn1_enc_length_cnt(vb->seqlen, &vb->seqlenlen);

    /* varbind seq */
    tot_len += 1 + vb->seqlenlen + vb->seqlen;

    vb = vb->prev;
  }

  /* varbind-list seq */
  root->seqlen = tot_len;
  snmp_asn1_enc_length_cnt(root->seqlen, &root->seqlenlen);
  tot_len += 1 + root->seqlenlen;

  return tot_len;
}

/**
 * Encodes response header from head to tail.
 */
/*
 * Function: snmp_resp_header_enc
 * ------------------------------
 * Encodes SNMP response header into buffer.
 * Writes version, community, PDU type, request ID, error status/index.
 * Returns updated offset after header encoding.
 */
static u16_t
snmp_resp_header_enc(struct snmp_msg_pstat *m_stat, u8_t *p)
{
  u16_t ofs;

  ofs = 0;
  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_CONSTR | SNMP_ASN1_SEQ));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_stat->rhl.seqlen);
  ofs += m_stat->rhl.seqlenlen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_stat->rhl.verlen);
  ofs += m_stat->rhl.verlenlen;
  snmp_asn1_enc_s32t(p, ofs, m_stat->rhl.verlen, snmp_version);
  ofs += m_stat->rhl.verlen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_stat->rhl.comlen);
  ofs += m_stat->rhl.comlenlen;
  snmp_asn1_enc_raw(p, ofs, m_stat->rhl.comlen, m_stat->community);
  ofs += m_stat->rhl.comlen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_CONTXT | SNMP_ASN1_CONSTR | SNMP_ASN1_PDU_GET_RESP));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_stat->rhl.pdulen);
  ofs += m_stat->rhl.pdulenlen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_stat->rhl.ridlen);  
  ofs += m_stat->rhl.ridlenlen;
  snmp_asn1_enc_s32t(p, ofs, m_stat->rhl.ridlen, m_stat->rid);
  ofs += m_stat->rhl.ridlen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_stat->rhl.errstatlen);
  ofs += m_stat->rhl.errstatlenlen;
  snmp_asn1_enc_s32t(p, ofs, m_stat->rhl.errstatlen, m_stat->error_status);
  ofs += m_stat->rhl.errstatlen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_stat->rhl.erridxlen);
  ofs += m_stat->rhl.erridxlenlen;
  snmp_asn1_enc_s32t(p, ofs, m_stat->rhl.erridxlen, m_stat->error_index);
  ofs += m_stat->rhl.erridxlen;

  return ofs;
}

/**
 * Encodes varbind list from head to tail.
 */
/*
 * Function: snmp_varbind_list_enc
 * -------------------------------
 * Encodes varbind list into buffer from head to tail.
 * Writes OID, value type, and value for each varbind.
 * Handles multiple ASN.1 types (integer, counter, string, etc.).
 */
static u16_t
snmp_varbind_list_enc(struct snmp_varbind_root *root, u8_t *p, u16_t ofs)
{
  struct snmp_varbind *vb;
  s32_t *sint_ptr;
  u32_t *uint_ptr;
  u8_t *raw_ptr;

  DBG_SNMP_PRINTF(("[%s][%d]\n", __FILE__, (uint16)__LINE__));
  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_CONSTR | SNMP_ASN1_SEQ));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, root->seqlen);
  ofs += root->seqlenlen;

  vb = root->head;
  while ( vb != NULL )
  {
    snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_CONSTR | SNMP_ASN1_SEQ));
    ofs += 1;
    snmp_asn1_enc_length(p, ofs, vb->seqlen);
    ofs += vb->seqlenlen;

    snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OBJ_ID));
    ofs += 1;
    snmp_asn1_enc_length(p, ofs, vb->olen);
    ofs += vb->olenlen;
    snmp_asn1_enc_oid(p, ofs, vb->ident_len, &vb->ident[0]);
    ofs += vb->olen;

    snmp_asn1_enc_type(p, ofs, vb->value_type);
    ofs += 1;
    snmp_asn1_enc_length(p, ofs, vb->vlen);
    ofs += vb->vlenlen;

    switch (vb->value_type)
    {
      case (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG):
        sint_ptr = vb->value;
        snmp_asn1_enc_s32t(p, ofs, vb->vlen, *sint_ptr);
        break;
	  case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER64):
	  	/*TODO:  counter64 decode support. Currently, we just change it to COUNT32 */
	  	uint_ptr = vb->value;
        snmp_asn1_enc_u64t(p, ofs, vb->vlen, uint_ptr);
		break;
      case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER):
      case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_GAUGE):
      case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_TIMETICKS):
        uint_ptr = vb->value;
        snmp_asn1_enc_u32t(p, ofs, vb->vlen, *uint_ptr);
        break;
      case (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR):
      case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_IPADDR):
      case (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_OPAQUE):
        raw_ptr = vb->value;
        snmp_asn1_enc_raw(p, ofs, vb->vlen, raw_ptr);
        break;
      case (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_NUL):
        break;
      case (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OBJ_ID):
        sint_ptr = vb->value;
        snmp_asn1_enc_oid(p, ofs, vb->value_len / sizeof(s32_t), sint_ptr);
        break;
      default:
        /* unsupported type */
        break;
    };
    ofs += vb->vlen;
    vb = vb->next;
  }
  return ofs;
}

#if LWPS_SNMP_TRAP_ENABLE

/*
 * Function: snmp_trap_set_enable
 * ------------------------------
 * Enables SNMP trap functionality globally.
 * Sets trap enable flag to TRUE for trap transmission.
 */
void snmp_trap_set_enable(void){
    snmp_trap_enable = TRUE;
}

/*
 * Function: snmp_linkUpdownTrap_set_enable
 * ----------------------------------------
 * Enables/disables link up/down traps for specific port.
 * Sets port-specific trap enable flag.
 */
void snmp_linkUpdownTrap_set_enable(u8_t portid,  u8_t enable)
{
     linkUpDownTrapEnable[portid] = enable;
}

/*
 * Function: snmp_linkUpdownTrap_get_enable
 * ----------------------------------------
 * Gets link up/down trap enable status for specific port.
 * Returns port-specific trap enable flag.
 */
void snmp_linkUpdownTrap_get_enable(u8_t portid,  u8_t *pEnable)
{
     *pEnable = linkUpDownTrapEnable[portid];
}

/*
 * Function: snmp_get_snmpenableauthentraps
 * ----------------------------------------
 * Returns authentication trap enable status.
 * Used for SNMPv1/v2c authentication failure trap control.
 */
void snmp_get_snmpenableauthentraps(u8_t *pEnable)
{
    *pEnable = snmp_authfail_trap_enable;
}
    
/*
 * Function: snmp_trap_linkChange
 * ------------------------------
 * Sends link up/down traps for port state changes.
 * Creates varbind with interface index for ports 3 and 4.
 * Updates syslog and link change time for affected ports.
 */
static void snmp_trap_linkChange(u8_t link_state)
{
    /* 1.3.6.1.2.1.2.2.1.1.x */
    s32_t interface_oid_Interface[] = {1, 3, 6, 1, 2, 1, 2, 2, 1, 1};
    struct snmp_varbind *vb;
    portmask_type change_type_pm;
    u8_t i;

    trap_msg.outvb.head = NULL;
    trap_msg.outvb.tail = NULL;
    trap_msg.outvb.count = 0;
    vb = NULL;

    if (link_state == SNMP_GENTRAP_LINKDOWN)
    {
        change_type_pm = snmp_new_linkdown_pm;
    }
    else if (link_state == SNMP_GENTRAP_LINKUP)
    {
        change_type_pm = snmp_new_linkup_pm;
    }
    else /* impossible */
    {
        return;
    }

    /* Check if any relevant ports (3 or 4) have changed */
    for (i = 0; i < INTERFACE_NUMBER; i++)
    {
        if (change_type_pm & (1UL << i))
        {
            if (linkUpDownTrapEnable[i] != 1)
                continue;
            /* Only process ports 3 (index 2) and 4 (index 3) */
            if (i != 2 && i != 3)
                continue;

            if (link_state == SNMP_GENTRAP_LINKDOWN)
                syslog_add_new_buf_entry(SYSLOG_MSG_PORT_LINK_DOWN, (uint32)(i + 1UL));
            else if (link_state == SNMP_GENTRAP_LINKUP)
                syslog_add_new_buf_entry(SYSLOG_MSG_PORT_LINK_UP, (uint32)(i + 1UL));

            GET_FREE_VB_MEM_BUF(vb);
            memset(vb, 0x00, sizeof(struct snmp_varbind));

            if (vb != NULL)
            {
                vb->next = NULL;
                vb->prev = NULL;

                vb->ident_len = (sizeof(interface_oid_Interface) / 4) + 1;
                GET_FREE_VB_IDENT_BUF(vb->ident);
                memset(vb->ident, 0x00, sizeof(struct vb_ident_buf_s));
                if (vb->ident != NULL)
                {
                    memcpy(vb->ident, interface_oid_Interface, sizeof(interface_oid_Interface));
                    vb->ident[vb->ident_len - 1] = (s32_t)(i + 1L); /* OID ends with port number */
                }
                else
                {
                    snmp_varbind_list_free(&trap_msg.outvb);
                    return;
                }

                vb->value_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
                vb->value_len = sizeof(s32_t);
                GET_FREE_VB_VAL_BUF(vb->value);
                memset(vb->value, 0x00, sizeof(struct vb_value_buf_s));
                if (vb->value != NULL)
                {
                    /* Map port 3 (index 2) to value 1, port 4 (index 3) to value 2 */
                    if (i == 2)
                        *((u32_t *)vb->value) = 1UL; /* Port 3 */
                    else if (i == 3)
                        *((u32_t *)vb->value) = 2UL; /* Port 4 */
                    
                    snmp_varbind_tail_add(&trap_msg.outvb, vb);
                }
                else
                {
                    snmp_varbind_free(vb);
                    snmp_varbind_list_free(&trap_msg.outvb);
                    return;
                }
            }
            else
            {
                snmp_varbind_list_free(&trap_msg.outvb);
                return;
            }
        }
    }

    /* Only send trap if variable bindings were added */
    if (trap_msg.outvb.count > 0)
    {
        snmp_send_trap(link_state, NULL, 0);
        snmp_varbind_list_free(&trap_msg.outvb);
        RESET_VB_BUFS();
    }
}

u8_t last_ac_status = INVALID_U8_VALUE;
u8_t last_battery_percentage = INVALID_U8_VALUE;
u8_t enable_trap_flag = 0;


extern uint8 start_pdu_status;
extern uint8 start_rtc_data;
extern uint8 start_alarm_data;
uint8 counter = 0;
/*
 * Function: snmp_timer_tick
 * -------------------------
 * Periodic timer handler for SNMP trap processing.
 * Reads JSON data for battery and AC status.
 * Sends traps on status changes and processes queued traps.
 * Requires ARP resolution before sending traps.
 */
void snmp_timer_tick() {
    static u8_t snmp_trap_test_time = 0;
    u8_t i = 0, current_ac_status = 0, current_battery_percentage = 0;
    current_battery_percentage = extract_c_value(Get_Response);
    current_ac_status = extract_a_value(Get_Response);

		if(start_pdu_status)
		{
				printf("GET_STATUS\r\n");
         start_pdu_status = 0;
		}
		
		if(start_rtc_data)
		{
				printf("GET_RTC\r\n");
        start_rtc_data = 0;

		}
		
    // Validate extracted values (assuming valid range for battery: 0-100, AC: 0 or 1)
    if (current_battery_percentage > 100 || current_ac_status > 1) {
        // printf("Invalid data: battery=%u, ac=%u\n", current_battery_percentage, current_ac_status);
        return; // Skip processing if data is invalid
    }

    if (enable_trap_flag == 1) {
        // Battery low trap logic
      if (last_battery_percentage == INVALID_U8_VALUE || current_battery_percentage != last_battery_percentage) {
            // printf("Sending BATT_LOW trap: battery=%u\n", current_battery_percentage);
            snmp_add_trap(SNMP_TRAP_TYPE_BATT_LOW);
				 last_battery_percentage = current_battery_percentage; // Always update last value
        }
       

        // AC status trap logic
        if (last_ac_status == INVALID_U8_VALUE || current_ac_status != last_ac_status) {
            // printf("Sending AC_STATUS trap: ac=%u\n", current_ac_status);
            snmp_add_trap(SNMP_TRAP_TYPE_AC_STATUS);
            last_ac_status = current_ac_status; // Update last value
        }
    }

    if ((-1 == snmp_trap_msg_index) || (FALSE == snmp_trap_enable)) {
        return;
    }

    snmp_trap_test_time++;

    if (LWPS_OK != lwps_arp_check(trap_dst[0].dip)) {
        if (snmp_trap_test_time >= LWPS_SNMP_TRAP_TESTTIME) {
            // printf("Trap timeout: clearing queue\n");
            for (i = 0; i <= snmp_trap_msg_index; i++) {
                snmp_trap_msg_queue[i] = 0;
            }
            snmp_trap_msg_index = -1;
            snmp_trap_test_time = 0;
        }
        return;
    }

    for (i = 0; i <= snmp_trap_msg_index; i++) {
        switch (snmp_trap_msg_queue[i]) {
            case SNMP_TRAP_TYPE_COLDSTART:
                trap_msg.outvb.head = NULL;
                trap_msg.outvb.tail = NULL;
                trap_msg.outvb.count = 0;
                snmp_send_trap(SNMP_GENTRAP_COLDSTART, NULL, 0);
                syslog_add_new_buf_entry(SYSLOG_MSG_SYSTEM_STARTED, 0);
                break;
            case SNMP_TRAP_TYPE_LINKDOWN:
                snmp_trap_linkChange(SNMP_GENTRAP_LINKDOWN);
                break;
            case SNMP_TRAP_TYPE_LINKUP:
                snmp_trap_linkChange(SNMP_GENTRAP_LINKUP);
                break;
            case SNMP_TRAP_TYPE_BATT_LOW:
                snmp_battery_low_trap();
                break;
            case SNMP_TRAP_TYPE_AC_STATUS:
                snmp_ac_status_trap(current_ac_status);
                break;
        }
        snmp_trap_msg_queue[i] = 0;
    }

    snmp_trap_msg_index = -1;
}

/*
 * Function: snmp_add_trap
 * -----------------------
 * Adds a trap type to the trap message queue.
 * Prevents duplicate traps by checking existing entries.
 * Returns if queue is full or trap already queued.
 */
void snmp_add_trap(u8_t trap_type){
    u8_t i;

    if( -1==snmp_trap_msg_index ){
        snmp_trap_msg_index = 0;
    }

    for(i=0; i<=snmp_trap_msg_index; i++)
    {
        if( snmp_trap_msg_queue[i]==trap_type )
        {
            return;
        }
    }
    
    
    if( (SNMP_TRAP_TYPE_MAXIMAL-1)==snmp_trap_msg_index )
    {
        return; /* no more space to add */
    }
    else
    {
        snmp_trap_msg_index++;
    }

    snmp_trap_msg_queue[snmp_trap_msg_index] = trap_type;
}

/**
 * Sets enable switch for this trap destination.
 * @param dst_idx index in 0 .. SNMP_TRAP_DESTINATIONS-1
 * @param enable switch if 0 destination is disabled >0 enabled.
 */
/*
 * Function: snmp_trap_dst_enable
 * ------------------------------
 * Enables or disables SNMP trap destination.
 * Sets enable flag for specified trap destination index.
 */
void
snmp_trap_dst_enable(u8_t dst_idx, u8_t enable)
{
  if (dst_idx < SNMP_TRAP_DESTINATIONS)
  {
    trap_dst[dst_idx].enable = enable;
  }
}

#if 1 /* Only Link-down/up trap */
/*
 * Function: snmp_authfail_trap
 * ----------------------------
 * Sends authentication failure trap if enabled.
 * Called when SNMP community string validation fails.
 * Uses generic trap type AUTHFAIL.
 */
void
snmp_authfail_trap(void)
{
  u8_t enable;
  snmp_get_snmpenableauthentraps(&enable);
  if (enable == 1)
  {
    trap_msg.outvb.head = NULL;
    trap_msg.outvb.tail = NULL;
    trap_msg.outvb.count = 0;
    snmp_send_trap(SNMP_GENTRAP_AUTHFAIL, NULL, 0);
  }
}
#endif


/**
 * Sets IPv4 address for this trap destination.
 * @param dst_idx index in 0 .. SNMP_TRAP_DESTINATIONS-1
 * @param dst IPv4 address in host order.
 */
/*
 * Function: snmp_trap_dst_ip_set
 * ------------------------------
 * Sets trap destination IP address for specified index.
 * Copies IP address from input into trap destination array.
 */
void
snmp_trap_dst_ip_set(u8_t dst_idx, u8_t *dst)
{
  if (dst_idx < SNMP_TRAP_DESTINATIONS)
  {
    IPADDR_COPY(trap_dst[dst_idx].dip.addr, dst);
    //trap_dst[dst_idx].dip.addr = htonl(dst->addr);
  }
}

/**
 * Gets IPv4 address for this trap destination.
 * @param dst_idx index in 0 .. SNMP_TRAP_DESTINATIONS-1
 * @param dst IPv4 address in host order.
 */
/*
 * Function: snmp_trap_dst_ip_get
 * ------------------------------
 * Gets trap destination IP address for specified index.
 * Copies IP address from trap destination array to output.
 */
void
snmp_trap_dst_ip_get(u8_t dst_idx, u8_t *dst)
{
  if ( (dst_idx < SNMP_TRAP_DESTINATIONS)  && dst )
  {
    IPADDR_COPY(dst, trap_dst[dst_idx].dip.addr);
    //trap_dst[dst_idx].dip.addr = htonl(dst->addr);
  }
}



/**
 * Sends an generic or enterprise specific trap message.
 *
 * @param generic_trap is the trap code
 * @param eoid points to enterprise object identifier
 * @param specific_trap used for enterprise traps when generic_trap == 6
 * @return ERR_OK when success, ERR_MEM if we're out of memory
 *
 * @note the caller is responsible for filling in outvb in the trap_msg
 * @note the use of the enterpise identifier field
 * is per RFC1215.
 * Use .iso.org.dod.internet.mgmt.mib-2.snmp for generic traps
 * and .iso.org.dod.internet.private.enterprises.yourenterprise
 * (sysObjectID) for specific traps.
 */
/*
 * Function: snmp_send_trap
 * ------------------------
 * Sends SNMP trap to all enabled destinations.
 * Performs two-pass encoding and sends via UDP.
 * Supports both generic and enterprise-specific traps.
 */
err_t
snmp_send_trap(s8_t generic_trap, struct snmp_obj_id *eoid, s32_t specific_trap)
{
  struct snmp_trap_dst *td;
  int32  socketId;
  u8_t *p;
  u16_t i,tot_len;
  u16_t ofs;

  DBG_SNMP_PRINTF(("[%s][%d]\n", __FILE__, (uint16)__LINE__));

  for (i=0, td = &trap_dst[0]; i<SNMP_TRAP_DESTINATIONS; i++, td++)
  {
    if ((td->enable != 0) && (td->dip.addr[0] != 0) && (td->dip.addr[1] != 0) &&\
		(td->dip.addr[2] != 0) && (td->dip.addr[3] != 0))
    {
      /* network order trap destination */
	  IPADDR_COPY(trap_msg.dip.addr, td->dip.addr);
      IPADDR_COPY(trap_msg.sip_raw, this_ip);
      trap_msg.gen_trap = generic_trap;
      trap_msg.spc_trap = specific_trap;
      if (generic_trap == SNMP_GENTRAP_ENTERPRISESPC)
      {
        /* enterprise-Specific trap */
        trap_msg.enterprise = eoid;

				
      }
      else
      {
        /* generic (MIB-II) trap */
        snmp_get_sysobjid_ptr(&trap_msg.enterprise);
      }
      snmp_get_sysuptime(&trap_msg.ts);

      /* pass 0, calculate length fields */
      tot_len = snmp_varbind_list_sum(&trap_msg.outvb);
      tot_len = snmp_trap_header_sum(&trap_msg, tot_len);

      p = malloc_tx_buf(1518UL);

      /* pass 1, encode packet ino the pbuf(s) */
      ofs = snmp_trap_header_enc(&trap_msg, p);
      snmp_varbind_list_enc(&trap_msg.outvb, p, ofs);
      
      /** connect to the TRAP destination */
	  socketId = lwps_socket(LWPS_L3TYPE_IP, LWPS_L4TYPE_UDP, snmpHandler);
	  if (socketId < 0)
	  {
	    OSAL_PRINTF("SNMP Connect Error! alloc socket failed!\n");
	    lwps_close(socketId);
	    return ERR_CONN;
	  }
	  if (LWPS_OK != lwps_sendto(socketId, p, tot_len, trap_msg.dip, SNMP_TRAP_PORT))
	  {
	    OSAL_PRINTF("SNMP Send Error! alloc socket failed!\n");
	    lwps_close(socketId);
	    return ERR_CONN;
	  }
	  /*if (LWPS_OK != lwps_sendto(socketId, p, tot_len, trap_msg.dip, SNMP_TRAP_PORT))
	  {
	    OSAL_PRINTF("SNMP Send Error! alloc socket failed!\n");
	    lwps_close(socketId);
	    return ERR_CONN;
	  }*/
      lwps_close(socketId);
		

    }
  }
  return ERR_OK;
}

/*
 * Function: snmp_coldstart_trap
 * -----------------------------
 * Triggers coldstart trap and enables trap flag.
 * Called during system initialization after boot.
 */
void
snmp_coldstart_trap(void)
{
  enable_trap_flag = 1;
	snmp_add_trap(SNMP_TRAP_TYPE_COLDSTART);
}

/**
 * Sums trap header field lengths from tail to head and
 * returns trap_header_lengths for second encoding pass.
 *
 * @param vb_len varbind-list length
 * @param thl points to returned header lengths
 * @return the required lenght for encoding the trap header
 */
/*
 * Function: snmp_trap_header_sum
 * ------------------------------
 * Calculates total length for SNMP trap header.
 * Sums enterprise OID, agent address, generic/specific traps, timestamp.
 * Stores lengths in trap header structure for encoding.
 */
static u16_t
snmp_trap_header_sum(struct snmp_msg_trap *m_trap, u16_t vb_len)
{
  u16_t tot_len;
  struct snmp_trap_header_lengths *thl;

  thl = &m_trap->thl;
  tot_len = vb_len;

  snmp_asn1_enc_u32t_cnt(m_trap->ts, &thl->tslen);
  snmp_asn1_enc_length_cnt(thl->tslen, &thl->tslenlen);
  tot_len += 1 + thl->tslen + thl->tslenlen;

  snmp_asn1_enc_s32t_cnt(m_trap->spc_trap, &thl->strplen);
  snmp_asn1_enc_length_cnt(thl->strplen, &thl->strplenlen);
  tot_len += 1 + thl->strplen + thl->strplenlen;

  snmp_asn1_enc_s32t_cnt(m_trap->gen_trap, &thl->gtrplen);
  snmp_asn1_enc_length_cnt(thl->gtrplen, &thl->gtrplenlen);
  tot_len += 1 + thl->gtrplen + thl->gtrplenlen;

  thl->aaddrlen = 4;
  snmp_asn1_enc_length_cnt(thl->aaddrlen, &thl->aaddrlenlen);
  tot_len += 1 + thl->aaddrlen + thl->aaddrlenlen;

  snmp_asn1_enc_oid_cnt(m_trap->enterprise->len, &m_trap->enterprise->id[0], &thl->eidlen);
  snmp_asn1_enc_length_cnt(thl->eidlen, &thl->eidlenlen);
  tot_len += 1 + thl->eidlen + thl->eidlenlen;

  thl->pdulen = tot_len;
  snmp_asn1_enc_length_cnt(thl->pdulen, &thl->pdulenlen);
  tot_len += 1 + thl->pdulenlen;

  //thl->comlen = sizeof(snmp_publiccommunity) - 1;
  thl->comlen = strlen(snmp_write_community);
  snmp_asn1_enc_length_cnt(thl->comlen, &thl->comlenlen);
  tot_len += 1 + thl->comlenlen + thl->comlen;

  snmp_asn1_enc_s32t_cnt(snmp_version, &thl->verlen);
  snmp_asn1_enc_length_cnt(thl->verlen, &thl->verlenlen);
  tot_len += 1 + thl->verlen + thl->verlenlen;

  thl->seqlen = tot_len;
  snmp_asn1_enc_length_cnt(thl->seqlen, &thl->seqlenlen);
  tot_len += 1 + thl->seqlenlen;

  return tot_len;
}

/**
 * Encodes trap header from head to tail.
 */
/*
 * Function: snmp_trap_header_enc
 * ------------------------------
 * Encodes SNMP trap header into buffer from head to tail.
 * Writes version, community, PDU type, enterprise OID, agent IP.
 * Writes generic/specific trap codes and timestamp.
 */
static u16_t
snmp_trap_header_enc(struct snmp_msg_trap *m_trap, u8_t *p)
{
  u16_t ofs;

  ofs = 0;
  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_CONSTR | SNMP_ASN1_SEQ));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_trap->thl.seqlen);
  ofs += m_trap->thl.seqlenlen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_trap->thl.verlen);
  ofs += m_trap->thl.verlenlen;
  snmp_asn1_enc_s32t(p, ofs, m_trap->thl.verlen, snmp_version);
  ofs += m_trap->thl.verlen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_trap->thl.comlen);
  ofs += m_trap->thl.comlenlen;
  snmp_asn1_enc_raw(p, ofs, m_trap->thl.comlen, (u8_t *)&snmp_write_community[0]);
  ofs += m_trap->thl.comlen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_CONTXT | SNMP_ASN1_CONSTR | SNMP_ASN1_PDU_TRAP));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_trap->thl.pdulen);
  ofs += m_trap->thl.pdulenlen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OBJ_ID));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_trap->thl.eidlen);
  ofs += m_trap->thl.eidlenlen;
  snmp_asn1_enc_oid(p, ofs, m_trap->enterprise->len, &m_trap->enterprise->id[0]);
  ofs += m_trap->thl.eidlen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_IPADDR));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_trap->thl.aaddrlen);
  ofs += m_trap->thl.aaddrlenlen;
  snmp_asn1_enc_raw(p, ofs, m_trap->thl.aaddrlen, &m_trap->sip_raw[0]);
  ofs += m_trap->thl.aaddrlen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_trap->thl.gtrplen);
  ofs += m_trap->thl.gtrplenlen;
  snmp_asn1_enc_u32t(p, ofs, m_trap->thl.gtrplen, m_trap->gen_trap);
  ofs += m_trap->thl.gtrplen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_trap->thl.strplen);
  ofs += m_trap->thl.strplenlen;
  snmp_asn1_enc_u32t(p, ofs, m_trap->thl.strplen, m_trap->spc_trap);
  ofs += m_trap->thl.strplen;

  snmp_asn1_enc_type(p, ofs, (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_TIMETICKS));
  ofs += 1;
  snmp_asn1_enc_length(p, ofs, m_trap->thl.tslen);
  ofs += m_trap->thl.tslenlen;
  snmp_asn1_enc_u32t(p, ofs, m_trap->thl.tslen, m_trap->ts);
  ofs += m_trap->thl.tslen;

  return ofs;
}

#endif /* end of LWPS_SNMP_TRAP_ENABLE */

/*
 * Function: snmp_linkChange_cb
 * ----------------------------
 * Callback for link state changes detected by switch.
 * Updates portPoe and portLan global variables for ports 3 and 4.
 * Queues link down/up traps and updates link change time.
 */
void
snmp_linkChange_cb(portmask_type pm)
{
  u8_t i;
  snmp_new_linkdown_pm = pm ^ (snmp_keep_pm | pm);
  snmp_new_linkup_pm = (snmp_keep_pm | pm) - snmp_keep_pm;

  /* Update portPoe for port 3 (index 2) */
  if (snmp_new_linkdown_pm & (1UL << 2)) {
    portPoe = 0; /* Port 3 is down */
  } else if (snmp_new_linkup_pm & (1UL << 2)) {
    portPoe = 1; /* Port 3 is up */
  }

  /* Update portLan for port 4 (index 3) */
  if (snmp_new_linkdown_pm & (1UL << 3)) {
    portLan = 0; /* Port 4 is down */
  } else if (snmp_new_linkup_pm & (1UL << 3)) {
    portLan = 1; /* Port 4 is up */
  }

  if (snmp_new_linkdown_pm)
  {
#if LWPS_SNMP_TRAP_ENABLE
    snmp_add_trap(SNMP_TRAP_TYPE_LINKDOWN);
#endif
    for (i = 0; i < INTERFACE_NUMBER; i++) {
      if (snmp_new_linkdown_pm & (1UL << i)) {
        snmp_get_sysuptime(&(linkChangeTime[i]));
      }
    }
  }

  if (snmp_new_linkup_pm)
  {
#if LWPS_SNMP_TRAP_ENABLE
    snmp_add_trap(SNMP_TRAP_TYPE_LINKUP);
		
#endif
    for (i = 0; i < INTERFACE_NUMBER; i++) {
      if (snmp_new_linkup_pm & (1UL << i)) {
        snmp_get_sysuptime(&(linkChangeTime[i]));
      }
    }
  }
  snmp_keep_pm = pm;
	
}

#endif /* LWIP_SNMP */

/*
 * Function: extract_a_value
 * -------------------------
 * Extracts AC status value from JSON string.
 * Searches for "A": pattern and returns integer value.
 * Returns -1 if key not found or value invalid.
 */
u8_t extract_a_value(const char *json_str) {
    u8_t value = 0;
    const char *key = "\"A\":";
    char *key_pos = strstr(json_str, key);
    if (key_pos == NULL) {
        return -1; // Key not found
    }

    key_pos += strlen(key);
    while (*key_pos == ' ' || *key_pos == '\t') {
        key_pos++;
    }

    if (*key_pos >= '0' && *key_pos <= '9') {
        char buffer[3] = {0}; // Buffer to store the number
        int i = 0;
        // Copy digits until a non-digit is encountered
        while (*key_pos >= '0' && *key_pos <= '9' && i < sizeof(buffer) - 1) {
            buffer[i++] = *key_pos++;
        }
        buffer[i] = '\0'; // Null-terminate the string
        value = atoi(buffer);

    } else {
        return -1; // Invalid value
    }

    return value;
}
#if LWPS_SNMP_TRAP_ENABLE

/**
 * Triggers a battery low trap with minimal memory usage, sending a string "Battery Low -25%".
 * Uses local variables for OID and varbind to avoid global memory allocation.
 */
/**
 * Triggers a battery low trap with minimal memory usage, sending the current battery percentage as an integer.
 * Uses local variables for OID and varbind to avoid global memory allocation.
 */
/*
 * Function: snmp_battery_low_trap
 * -------------------------------
 * Sends battery low enterprise-specific trap.
 * Extracts battery percentage from JSON and sends as integer.
 * Uses local varbind with OID .1.3.6.1.4.1.63663.1.12.
 */
void snmp_battery_low_trap()
{
    /* Local OID for battery status (1.3.6.1.4.1.63663.1.12) */
    s32_t batt_oid[] = {1, 3, 6, 1, 4, 1, 63663, 1, 12};
    u8_t batt_oid_len = sizeof(batt_oid) / sizeof(s32_t);
    
    /* Local varbind structure */
    struct snmp_varbind vb;
    struct snmp_varbind_root vb_root;
    
    /* Local value buffer for the battery percentage */
   s32_t value = extract_c_value(Get_Response); /* Get current battery percentage */
    
    /* Validate extracted value */
    if (value < 0 || value > 100) {
        // Invalid battery percentage, skip sending trap
        return;
    }
    
    /* Initialize varbind root */
    vb_root.head = NULL;
    vb_root.tail = NULL;
    vb_root.count = 0;
    
    /* Initialize varbind */
    memset(&vb, 0, sizeof(struct snmp_varbind));
    vb.next = NULL;
    vb.prev = NULL;
    
    /* Set up OID for the varbind */
    vb.ident_len = batt_oid_len;
    vb.ident = batt_oid; /* Use local OID array directly */
    
    /* Set up value (integer as SNMP INTEGER) */
    vb.value_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
    vb.value_len = sizeof(s32_t);
    vb.value = &value; /* Use local integer variable */
    
    /* Add varbind to the root */
    snmp_varbind_tail_add(&vb_root, &vb);
    
    /* Set up trap message */
    trap_msg.outvb = vb_root;
    
    /* Send enterprise-specific trap */
    snmp_send_trap(SNMP_GENTRAP_ENTERPRISESPC, NULL, SNMP_TRAP_TYPE_AC_STATUS - SNMP_TRAP_TYPE_ADD);
    
    
    /* Clean up */
    snmp_varbind_list_free(&trap_msg.outvb);
    RESET_VB_BUFS();
}

/*
 * Function: snmp_ac_status_trap
 * -----------------------------
 * Sends AC status enterprise-specific trap.
 * Extracts AC status from JSON and sends as integer.
 * Uses local varbind with OID .1.3.6.1.4.1.63663.1.13.
 */
void snmp_ac_status_trap(uint8 ac_status)
{
    /* Local OID for AC status (1.3.6.1.4.1.63663.1.13) */
    s32_t batt_oid[] = {1, 3, 6, 1, 4, 1, 63663, 1, 13};
    u8_t batt_oid_len = sizeof(batt_oid) / sizeof(s32_t);
    
    /* Local varbind structure */
    struct snmp_varbind vb;
    struct snmp_varbind_root vb_root;
    
    /* Local value buffer for the ac status */
   s32_t value = (s32_t *)ac_status; /* Get current ac status */
    
    /* Validate extracted value */
    if (value < 0 || value > 1) {
        // Invalid ac value, skip sending trap
        return;
    }
    
    /* Initialize varbind root */
    vb_root.head = NULL;
    vb_root.tail = NULL;
    vb_root.count = 0;
    
    /* Initialize varbind */
    memset(&vb, 0, sizeof(struct snmp_varbind));
    vb.next = NULL;
    vb.prev = NULL;
    
    /* Set up OID for the varbind */
    vb.ident_len = batt_oid_len;
    vb.ident = batt_oid; /* Use local OID array directly */
    
    /* Set up value (integer as SNMP INTEGER) */
    vb.value_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
    vb.value_len = sizeof(s32_t);
    vb.value = &value; /* Use local integer variable */
    
    /* Add varbind to the root */
    snmp_varbind_tail_add(&vb_root, &vb);
    
    /* Set up trap message */
    trap_msg.outvb = vb_root;
    
    /* Send enterprise-specific trap */
    snmp_send_trap(SNMP_GENTRAP_ENTERPRISESPC, NULL, SNMP_TRAP_TYPE_BATT_LOW - SNMP_TRAP_TYPE_ADD);
    
    
    /* Clean up */
    snmp_varbind_list_free(&trap_msg.outvb);
    RESET_VB_BUFS();
}

#endif /* LWPS_SNMP_TRAP_ENABLE */