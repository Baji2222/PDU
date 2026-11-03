/**
 * @file
 * Management Information Base II (RFC1213) objects and functions.
 *
 * @note the object identifiers for this MIB-2 and private MIB tree
 * must be kept in sorted ascending order. This to ensure correct getnext operation.
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
#pragma STRING (far)
#include <stdio.h>
#include <string.h>
#include <local/config.h>
#include <snmp/opt.h>

#if LWIP_SNMP /* don't build if not configured for use in lwipopts.h */

#include <salport.h>
#include <salsys.h>
#include <salflash.h>

#if !defined(SAL_REPLACE)
#include <local/rtk_sal_mib.h>
#else
#include <snmp/mib_glue.h>
#endif

#include <lwps/lwps_api.h>
#include <snmp/snmp.h>
#include <snmp/snmp_asn1.h>
#include <snmp/snmp_structs.h>
#include <snmp/snmp_debug.h>
#include <snmp/mib/mib.h>
#include <lwps/lwps_api.h>
#include <common.h>

/**
 * IANA assigned enterprise ID for lwIP is 26381
 * @see http://www.iana.org/assignments/enterprise-numbers
 *
 * @note this enterprise ID is assigned to the lwIP project,
 * all object identifiers living under this ID are assigned
 * by the lwIP maintainers (contact Christiaan Simons)!
 * @note don't change this define, use snmp_set_sysobjid()
 *
 * If you need to create your own private MIB you'll need
 * to apply for your own enterprise ID with IANA:
 * http://www.iana.org/numbers.html
 */

#ifndef SNMP_SYSSERVICES
#define SNMP_SYSSERVICES ((1 << 6) | (1 << 3) | ((IP_FORWARD) << 2))
#endif

#ifndef SNMP_GET_SYSUPTIME
#define SNMP_GET_SYSUPTIME(up_time)
#endif

static void system_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
static void system_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
static u8_t system_set_test(struct obj_def *od, u16_t len, void *value) reentrant;
static void system_set_value(struct obj_def *od, u16_t len, void *value) reentrant;

static void interfaces_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
static void interfaces_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
static void ifentry_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
static void ifentry_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
#if !SNMP_SAFE_REQUESTS
static u8_t ifentry_set_test (struct obj_def *od, u16_t len, void *value)  reentrant;
static void ifentry_set_value (struct obj_def *od, u16_t len, void *value)  reentrant;
#endif /* SNMP_SAFE_REQUESTS */
//static void atentry_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
//static void atentry_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
//static void ip_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
//static void ip_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
//static u8_t ip_set_test(struct obj_def *od, u16_t len, void *value) reentrant;
//static void ip_addrentry_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
//static void ip_addrentry_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
//static void ip_rteentry_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
//static void ip_rteentry_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
//static void ip_ntomentry_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
//static void ip_ntomentry_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
//static void icmp_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
//static void icmp_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
#if LWIP_TCP
//static void tcp_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
//static void tcp_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
#ifdef THIS_SEEMS_UNUSED
//static void tcpconnentry_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
//static void tcpconnentry_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
#endif
#endif
//static void udp_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
//static void udp_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
//static void udpentry_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
//static void udpentry_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
//static void snmp_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant;
//static void snmp_get_value(struct obj_def *od, u16_t len, void *value) reentrant;
//static u8_t snmp_set_test(struct obj_def *od, u16_t len, void *value) reentrant;
//static void snmp_set_value(struct obj_def *od, u16_t len, void *value) reentrant;

#if 0 /* snmp objects */
/* snmp .1.3.6.1.2.1.11 */
far const mib_scalar_node snmp_scalar = {
  &snmp_get_object_def,
  &snmp_get_value,
  &snmp_set_test,
  &snmp_set_value,
  MIB_NODE_SC,
  0
};
far const s32_t snmp_ids[28] = {
  1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16,
  17, 18, 19, 20, 21, 22, 24, 25, 26, 27, 28, 29, 30
};
far struct mib_node* const snmp_nodes[28] = {
  (struct mib_node* const)&snmp_scalar, (struct mib_node* const)&snmp_scalar,
  (struct mib_node* const)&snmp_scalar, (struct mib_node* const)&snmp_scalar,
  (struct mib_node* const)&snmp_scalar, (struct mib_node* const)&snmp_scalar,
  (struct mib_node* const)&snmp_scalar, (struct mib_node* const)&snmp_scalar,
  (struct mib_node* const)&snmp_scalar, (struct mib_node* const)&snmp_scalar,
  (struct mib_node* const)&snmp_scalar, (struct mib_node* const)&snmp_scalar,
  (struct mib_node* const)&snmp_scalar, (struct mib_node* const)&snmp_scalar,
  (struct mib_node* const)&snmp_scalar, (struct mib_node* const)&snmp_scalar,
  (struct mib_node* const)&snmp_scalar, (struct mib_node* const)&snmp_scalar,
  (struct mib_node* const)&snmp_scalar, (struct mib_node* const)&snmp_scalar
};
far const struct mib_array_node snmp = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  28,
  snmp_ids,
  snmp_nodes
};
#endif /* end of snmp objects */

/* dot3 and EtherLike MIB not planned. (transmission .1.3.6.1.2.1.10) */
/* historical (some say hysterical). (cmot .1.3.6.1.2.1.9) */
/* lwIP has no EGP, thus may not implement it. (egp .1.3.6.1.2.1.8) */

#if 0 /* udp objects */
/* udp .1.3.6.1.2.1.7 */
/** index root node for udpTable */
struct mib_list_rootnode udp_root = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
far const s32_t udpentry_ids[2] = { 1, 2 };
far struct mib_node* const udpentry_nodes[2] = {
  (struct mib_node* const)&udp_root, (struct mib_node* const)&udp_root,
};
far const struct mib_array_node udpentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  2,
  udpentry_ids,
  udpentry_nodes
};

s32_t udptable_id = 1;
struct mib_node* udptable_node = (struct mib_node* const)&udpentry;
struct mib_ram_array_node udptable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  0,
  &udptable_id,
  &udptable_node
};

far const mib_scalar_node udp_scalar = {
  &udp_get_object_def,
  &udp_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_SC,
  0
};
far const s32_t udp_ids[5] = { 1, 2, 3, 4, 5 };
struct mib_node* const udp_nodes[5] = {
  (struct mib_node* const)&udp_scalar, (struct mib_node* const)&udp_scalar,
  (struct mib_node* const)&udp_scalar, (struct mib_node* const)&udp_scalar,
  (struct mib_node* const)&udptable
};
far const struct mib_array_node udp = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  5,
  udp_ids,
  udp_nodes
};
#endif /* end of udp objects */

#if 0 /* tcp objects */
/* tcp .1.3.6.1.2.1.6 */
#if LWIP_TCP
/* only if the TCP protocol is available may implement this group */
/** index root node for tcpConnTable */
struct mib_list_rootnode tcpconntree_root = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
far const s32_t tcpconnentry_ids[5] = { 1, 2, 3, 4, 5 };
far struct mib_node* const tcpconnentry_nodes[5] = {
  (struct mib_node* const)&tcpconntree_root, (struct mib_node* const)&tcpconntree_root,
  (struct mib_node* const)&tcpconntree_root, (struct mib_node* const)&tcpconntree_root,
  (struct mib_node* const)&tcpconntree_root
};
far const struct mib_array_node tcpconnentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  5,
  tcpconnentry_ids,
  tcpconnentry_nodes
};

s32_t tcpconntable_id = 1;
struct mib_node* tcpconntable_node = (struct mib_node* const)&tcpconnentry;
struct mib_ram_array_node tcpconntable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
/** @todo update maxlength when inserting / deleting from table
   0 when table is empty, 1 when more than one entry */
  0,
  &tcpconntable_id,
  &tcpconntable_node
};

far const mib_scalar_node tcp_scalar = {
  &tcp_get_object_def,
  &tcp_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_SC,
  0
};
far const s32_t tcp_ids[15] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
far struct mib_node* const tcp_nodes[15] = {
  (struct mib_node* const)&tcp_scalar, (struct mib_node* const)&tcp_scalar,
  (struct mib_node* const)&tcp_scalar, (struct mib_node* const)&tcp_scalar,
  (struct mib_node* const)&tcp_scalar, (struct mib_node* const)&tcp_scalar,
  (struct mib_node* const)&tcp_scalar, (struct mib_node* const)&tcp_scalar,
  (struct mib_node* const)&tcp_scalar, (struct mib_node* const)&tcp_scalar,
  (struct mib_node* const)&tcp_scalar, (struct mib_node* const)&tcp_scalar,
  (struct mib_node* const)&tcpconntable, (struct mib_node* const)&tcp_scalar,
  (struct mib_node* const)&tcp_scalar
};
far const struct mib_array_node tcp = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  15,
  tcp_ids,
  tcp_nodes
};
#endif /* end of LWIP_TCP*/
#endif /* end of tcp objects */


#if 0 /* at objects */
/** index root node for ipNetToMediaTable */
struct mib_list_rootnode ipntomtree_root = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
far const s32_t ipntomentry_ids[4] = { 1, 2, 3, 4 };
struct mib_node* const ipntomentry_nodes[4] = {
  (struct mib_node* const)&ipntomtree_root, (struct mib_node* const)&ipntomtree_root,
  (struct mib_node* const)&ipntomtree_root, (struct mib_node* const)&ipntomtree_root
};
far const struct mib_array_node ipntomentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  4,
  ipntomentry_ids,
  ipntomentry_nodes
};

s32_t ipntomtable_id = 1;
struct mib_node* ipntomtable_node = (struct mib_node* const)&ipntomentry;
struct mib_ram_array_node ipntomtable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  0,
  &ipntomtable_id,
  &ipntomtable_node
};

/** index root node for ipRouteTable */
struct mib_list_rootnode iprtetree_root = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
far const s32_t iprteentry_ids[13] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
far struct mib_node* const iprteentry_nodes[13] = {
  (struct mib_node* const)&iprtetree_root, (struct mib_node* const)&iprtetree_root,
  (struct mib_node* const)&iprtetree_root, (struct mib_node* const)&iprtetree_root,
  (struct mib_node* const)&iprtetree_root, (struct mib_node* const)&iprtetree_root,
  (struct mib_node* const)&iprtetree_root, (struct mib_node* const)&iprtetree_root,
  (struct mib_node* const)&iprtetree_root, (struct mib_node* const)&iprtetree_root,
  (struct mib_node* const)&iprtetree_root, (struct mib_node* const)&iprtetree_root,
  (struct mib_node* const)&iprtetree_root
};
far const struct mib_array_node iprteentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  13,
  iprteentry_ids,
  iprteentry_nodes
};

s32_t iprtetable_id = 1;
struct mib_node* iprtetable_node = (struct mib_node* const)&iprteentry;
struct mib_ram_array_node iprtetable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  0,
  &iprtetable_id,
  &iprtetable_node
};

/** index root node for ipAddrTable */
struct mib_list_rootnode ipaddrtree_root = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
far const s32_t ipaddrentry_ids[5] = { 1, 2, 3, 4, 5 };
far struct mib_node* const ipaddrentry_nodes[5] = {
  (struct mib_node* const)&ipaddrtree_root,
  (struct mib_node* const)&ipaddrtree_root,
  (struct mib_node* const)&ipaddrtree_root,
  (struct mib_node* const)&ipaddrtree_root,
  (struct mib_node* const)&ipaddrtree_root
};
far const struct mib_array_node ipaddrentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  5,
  ipaddrentry_ids,
  ipaddrentry_nodes
};

s32_t ipaddrtable_id = 1;
struct mib_node* ipaddrtable_node = (struct mib_node* const)&ipaddrentry;
struct mib_ram_array_node ipaddrtable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  0,
  &ipaddrtable_id,
  &ipaddrtable_node
};

/* ip .1.3.6.1.2.1.4 */
far const mib_scalar_node ip_scalar = {
  &ip_get_object_def,
  &ip_get_value,
  &ip_set_test,
  &noleafs_set_value,
  MIB_NODE_SC,
  0
};
far const s32_t ip_ids[23] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };
far struct mib_node* const ip_nodes[23] = {
  (struct mib_node* const)&ip_scalar, (struct mib_node* const)&ip_scalar,
  (struct mib_node* const)&ip_scalar, (struct mib_node* const)&ip_scalar,
  (struct mib_node* const)&ip_scalar, (struct mib_node* const)&ip_scalar,
  (struct mib_node* const)&ip_scalar, (struct mib_node* const)&ip_scalar,
  (struct mib_node* const)&ip_scalar, (struct mib_node* const)&ip_scalar,
  (struct mib_node* const)&ip_scalar, (struct mib_node* const)&ip_scalar,
  (struct mib_node* const)&ip_scalar, (struct mib_node* const)&ip_scalar,
  (struct mib_node* const)&ip_scalar, (struct mib_node* const)&ip_scalar,
  (struct mib_node* const)&ip_scalar, (struct mib_node* const)&ip_scalar,
  (struct mib_node* const)&ipaddrtable, (struct mib_node* const)&iprtetable,
  (struct mib_node* const)&ipntomtable, (struct mib_node* const)&ip_scalar
};
far const struct mib_array_node mib2_ip = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  23,
  ip_ids,
  ip_nodes
};
#endif /* end of ip objects */

#if 0 /* at objects */

/** index root node for atTable */
struct mib_list_rootnode arptree_root = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
far const s32_t atentry_ids[3] = { 1, 2, 3 };
far struct mib_node* const atentry_nodes[3] = {
  (struct mib_node* const)&arptree_root,
  (struct mib_node* const)&arptree_root,
  (struct mib_node* const)&arptree_root
};
far const struct mib_array_node atentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  3,
  atentry_ids,
  atentry_nodes
};

far const s32_t attable_id = 1;
far struct mib_node* const attable_node = (struct mib_node* const)&atentry;
far const struct mib_array_node attable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  1,
  &attable_id,
  &attable_node
};

/* at .1.3.6.1.2.1.3 */
s32_t at_id = 1;
struct mib_node* mib2_at_node = (struct mib_node* const)&attable;
struct mib_ram_array_node at = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  0,
  &at_id,
  &mib2_at_node
};
#endif /* end of at objects */

//#if LWPS_SNMP_INTERFACE_GROUP /* mib2-interface objects */
#if LWPS_SNMP_IFTABLE
/** index root node for ifTable */
struct mib_list_rootnode iflist_root = {
  &ifentry_get_object_def,
  &ifentry_get_value,
#if SNMP_SAFE_REQUESTS
  &noleafs_set_test,
  &noleafs_set_value,
#else /* SNMP_SAFE_REQUESTS */
  &ifentry_set_test,
  &ifentry_set_value,
#endif /* SNMP_SAFE_REQUESTS */
  MIB_NODE_LR,
  0,
  NULL,
  NULL,
  0
};
far const s32_t ifentry_ids[22] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22 };
far struct mib_node* const ifentry_nodes[22] = {
  (struct mib_node* const)&iflist_root, (struct mib_node* const)&iflist_root,
  (struct mib_node* const)&iflist_root, (struct mib_node* const)&iflist_root,
  (struct mib_node* const)&iflist_root, (struct mib_node* const)&iflist_root,
  (struct mib_node* const)&iflist_root, (struct mib_node* const)&iflist_root,
  (struct mib_node* const)&iflist_root, (struct mib_node* const)&iflist_root,
  (struct mib_node* const)&iflist_root, (struct mib_node* const)&iflist_root,
  (struct mib_node* const)&iflist_root, (struct mib_node* const)&iflist_root,
  (struct mib_node* const)&iflist_root, (struct mib_node* const)&iflist_root,
  (struct mib_node* const)&iflist_root, (struct mib_node* const)&iflist_root,
  (struct mib_node* const)&iflist_root, (struct mib_node* const)&iflist_root
};
far const struct mib_array_node ifentry = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  22,
  ifentry_ids,
  ifentry_nodes
};

s32_t iftable_id = 1;
struct mib_node* iftable_node = (struct mib_node* const)&ifentry;
struct mib_ram_array_node iftable = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_RA,
  0,
  &iftable_id,
  &iftable_node
};

/* interfaces .1.3.6.1.2.1.2 */
far const mib_scalar_node interfaces_scalar = {
  &interfaces_get_object_def,
  &interfaces_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_SC,
  0
};
far const s32_t interfaces_ids[2] = { 1, 2 };
far struct mib_node* const interfaces_nodes[2] = {
  (struct mib_node* const)&interfaces_scalar, (struct mib_node* const)&iftable
};
far const struct mib_array_node interfaces = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  2,
  interfaces_ids,
  interfaces_nodes
};
#endif /* end of mib2-interface objects */

/*             0 1 2 3 4 5 6 */
/* system .1.3.6.1.2.1.1 */
far const mib_scalar_node sys_tem_scalar = {
  &system_get_object_def,
  &system_get_value,
  &system_set_test,
  &system_set_value,
  MIB_NODE_SC,
  0
};
far const s32_t sys_tem_ids[7] = { 1, 2, 3, 4, 5, 6, 7 };
far struct mib_node* const sys_tem_nodes[7] = {
  (struct mib_node* const)&sys_tem_scalar, (struct mib_node* const)&sys_tem_scalar,
  (struct mib_node* const)&sys_tem_scalar, (struct mib_node* const)&sys_tem_scalar,
  (struct mib_node* const)&sys_tem_scalar, (struct mib_node* const)&sys_tem_scalar,
  (struct mib_node* const)&sys_tem_scalar
};
/* work around name issue with 'sys_tem', some compiler(s?) seem to reserve 'system' */
far const struct mib_array_node sys_tem = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  7,
  sys_tem_ids,
  sys_tem_nodes
};

/* mib-2 .1.3.6.1.2.1 */
far const s32_t mib2_ids[] =
{
  1,
#if LWPS_SNMP_IFTABLE
  2,
#endif

#if LWIP_TCP
  //6,
#endif
  //7,
#if LWPS_SNMP_ETHERNET_LIKE
  MIB_TRANSMISSION_ID,
#endif
  //11,
#if LWPS_SNMP_RMON
  MIB_RMON_ID,
#endif
  
#if LWPS_SNMP_BRIDGE_MIB
  MIB_BRIDGE_ID,
#endif

#if LWPS_SNMP_INTERFACE_GROUP
  MIB_INTERFACE_GROUP_ID,
#endif

#if LWPS_SNMP_OAM_MIB
  MIB_OAM_ID,
#endif
};
far struct mib_node* const mib2_nodes[] = {
  (struct mib_node* const)&sys_tem,
#if LWPS_SNMP_IFTABLE
  (struct mib_node* const)&interfaces,
#endif
  //(struct mib_node* const)&at,
  //(struct mib_node* const)&mib2_ip,
#if LWIP_TCP
  //(struct mib_node* const)&tcp,
#endif
  //(struct mib_node* const)&udp,
#if LWPS_SNMP_ETHERNET_LIKE
  MIB_TRANSMISSION_HANDLER,
#endif
  //(struct mib_node* const)&snmp,
#if LWPS_SNMP_RMON
  MIB_RMON_HANDLER,
#endif

#if LWPS_SNMP_BRIDGE_MIB
  MIB_BRIDGE_HANDLER,
#endif

#if LWPS_SNMP_INTERFACE_GROUP
  MIB_INTERFACE_GROUP_HANDLER,
#endif

#if LWPS_SNMP_OAM_MIB
  MIB_OAM_HANDLER,
#endif
};

far const struct mib_array_node mib2 = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  sizeof(mib2_ids)/sizeof(s32_t),
  mib2_ids,
  mib2_nodes
};

/* mgmt .1.3.6.1.2 */
far const s32_t mgmt_ids[1] = { 1 };
far struct mib_node* const mgmt_nodes[1] = { (struct mib_node* const)&mib2 };
far const struct mib_array_node mgmt = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  1,
  mgmt_ids,
  mgmt_nodes
};

/* internet .1.3.6.1 */
#if SNMP_PRIVATE_MIB
far const s32_t internet_ids[2] = { 2, 4 };
far struct mib_node* const internet_nodes[2] = { (struct mib_node* const)&mgmt, (struct mib_node* const)&private_mib };
struct mib_array_node internet = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  2,
  internet_ids,
  internet_nodes
};
#else
far const s32_t internet_ids[1] = { 2 };
far struct mib_node* const internet_nodes[1] = { (struct mib_node* const)&mgmt };
struct mib_array_node internet = {
  &noleafs_get_object_def,
  &noleafs_get_value,
  &noleafs_set_test,
  &noleafs_set_value,
  MIB_NODE_AR,
  1,
  internet_ids,
  internet_nodes
};
#endif

/** mib-2.system.sysObjectID  */
static struct snmp_obj_id sysobjid = {SNMP_SYSOBJID_LEN, SNMP_SYSOBJID};
/** enterprise ID for generic TRAPs, .iso.org.dod.internet.mgmt.mib-2.snmp */
static struct snmp_obj_id snmpgrp_id = {7,{1,3,6,1,2,1,11}};
/** mib-2.system.sysServices */
static const s32_t code sysservices = SNMP_SYSSERVICES;

/** mib-2.system.sysDescr */
//static u8_t sysdescr_default[SAL_SNMP_SYS_INFO_STR_LEN] = "Realtek-Switch";
//static u8_t sysdescr_len_default = sizeof(sysdescr_default);
//static u8_t* sysdescr_len_ptr = (u8_t*)&sysdescr_len_default;
static u8_t  sysdescription_buf_len = CONFIG_SYSTEM_NAME_LEN;
static u8_t  sysdescription_buf[CONFIG_SYSTEM_NAME_LEN];
static u8_t* sysdescr_ptr = (u8_t*)&sysdescription_buf[0];
/** mib-2.system.sysContact */
//static u8_t syscontact_default[SAL_SNMP_SYS_INFO_STR_LEN] = "";
//static u8_t syscontact_len_default = sizeof(syscontact_default);
//static u8_t* syscontact_len_ptr = (u8_t*)&syscontact_len_default;
static u8_t  syscontact_buf_len = CONFIG_SYSTEM_NAME_LEN;
static u8_t  syscontact_buf[CONFIG_SYSTEM_CONTACT_LEN] = "";
static u8_t* syscontact_ptr = (u8_t*)&syscontact_buf[0];
/** mib-2.system.sysName */
static u8_t sysname_default[] = SWITCH_NAME;
static u8_t sysname_len_default = sizeof(sysname_default)-1;
static u8_t* sysname_len_ptr = (u8_t*)&sysname_len_default;
static u8_t* sysname_ptr = (u8_t*)&sysname_default[0];
static u8_t  sysname_buf_len = 0;
static u8_t  sysname_buf[CONFIG_SYSTEM_NAME_LEN];
/** mib-2.system.sysLocation */
//static u8_t syslocation_len_default = 0;
//static u8_t syslocation_default[SAL_SNMP_SYS_INFO_STR_LEN] = "";
//static u8_t* syslocation_len_ptr = (u8_t*)&syslocation_len_default;
static u8_t  syslocation_buf_len = CONFIG_SYSTEM_NAME_LEN;
static u8_t  syslocation_buf[CONFIG_SYSTEM_LOCATION_LEN] = "";
static u8_t* syslocation_ptr = (u8_t*)&syslocation_buf[0];
/** mib-2.snmp.snmpEnableAuthenTraps */
static const u8_t code snmpenableauthentraps_default = 2; /* disabled */
static u8_t* snmpenableauthentraps_ptr = (u8_t*)&snmpenableauthentraps_default;

/** mib-2.interfaces.ifTable.ifEntry.ifSpecific (zeroDotZero) */
far const struct snmp_obj_id ifspecific = {2, {0, 0}};
/** mib-2.ip.ipRouteTable.ipRouteEntry.ipRouteInfo (zeroDotZero) */
static const struct snmp_obj_id code iprouteinfo = {2, {0, 0}};
far const u16_t ipaddrlen = CONFIG_INTERFACE_ADDR_LEN;


/* mib-2.system counter(s) */
//static u32_t sysuptime = 0;
//extern static u32_t up_time;

/* prototypes of the following functions are in lwip/src/include/lwip/snmp.h */


/*
 * Function: snmp_get_sysobjid_ptr
 * -------------------------------
 * Returns pointer to system object ID for SNMP MIB.
 * Provides OID of the device's system object.
 * Used by SNMP agent for sysObjectID response.
 */
void snmp_get_sysobjid_ptr(struct snmp_obj_id **oid)
{
  *oid = &sysobjid;
}

/*
 * Function: snmp_get_sysuptime
 * ----------------------------
 * Returns system uptime in hundredths of seconds.
 * Converts up_time (seconds) to centiseconds (*100).
 * Used for SNMP sysUpTime OID response.
 */
void snmp_get_sysuptime(u32_t *value)
{
  *value = (up_time*100);
}

/*
 * Function: noleafs_get_object_def
 * --------------------------------
 * Placeholder for leaf node object definition.
 * Sets instance to MIB_OBJECT_NONE.
 * Used for unused/unsupported MIB nodes.
 */
void
noleafs_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant
{
  if (ident_len){}
  if (ident){}
  od->instance = MIB_OBJECT_NONE;
}

/*
 * Function: noleafs_get_value
 * ---------------------------
 * Placeholder for leaf node value retrieval.
 * Empty function for unused MIB nodes.
 * Suppresses compiler warnings with unused parameters.
 */
void
noleafs_get_value(struct obj_def *od, u16_t len, void *value) reentrant
{
  if (od){}
  if (len){}
  if (value){}
}

/*
 * Function: noleafs_set_test
 * --------------------------
 * Placeholder for leaf node set test.
 * Always returns 0 (cannot set).
 * Prevents write operations on unsupported nodes.
 */
u8_t
noleafs_set_test(struct obj_def *od, u16_t len, void *value) reentrant
{
  if (od){}
  if (len){}
  if (value){}
  /* can't set */
  return 0;
}

/*
 * Function: noleafs_set_value
 * ---------------------------
 * Placeholder for leaf node value set.
 * Empty function for unused MIB nodes.
 * Suppresses compiler warnings with unused parameters.
 */
void
noleafs_set_value(struct obj_def *od, u16_t len, void *value) reentrant
{
  if (od){}
  if (len){}
  if (value){}
}

/*
 * Function: snmp_print_node_info
 * ------------------------------
 * Prints MIB array node information for debugging.
 * Iterates through node entries (currently empty).
 * Placeholder for SNMP node debug functionality.
 */
void snmp_print_node_info ( struct mib_array_node * node  ) large
{
    int i = 0;

    while(i < node->maxlength)
    {
        i++;
    }

    i=0;
    while(i < node->maxlength)
    {
       i++;
    }

}

/**
 * Returns systems object definitions.
 *
 * @param ident_len the address length (2)
 * @param ident points to objectname.0 (object id trailer)
 * @param od points to object definition.
 */
/*
 * Function: system_get_object_def
 * -------------------------------
 * Returns system MIB object definitions.
 * Handles sysDescr, sysObjectID, sysUpTime, sysContact, sysName, sysLocation, sysServices.
 * Sets access rights, ASN.1 type, and value length for each OID.
 */
static void
system_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od) reentrant
{
  u8_t id;

  /* return to object name, adding index depth (1) */
  ident_len += 1;
  ident -= 1;
  if (ident_len == 2)
  {
    od->id_inst_len = ident_len;
    od->id_inst_ptr = ident;

    id = ident[0];
    DBG_SNMP_PRINTF(("[%s][%d]get_object_def system.%"U16_F".0\n",\
      __FILE__, (uint16)__LINE__, (u16_t)id));
    switch (id)
    {
      case 1: /* sysDescr */
        od->instance = MIB_OBJECT_SCALAR;
        od->access = MIB_OBJECT_READ_ONLY;
        od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR);
        od->v_len = sysdescription_buf_len; //*sysdescr_len_ptr;
        break;
      case 2: /* sysObjectID */
        od->instance = MIB_OBJECT_SCALAR;
        od->access = MIB_OBJECT_READ_ONLY;
        od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OBJ_ID);
        od->v_len = sysobjid.len * sizeof(s32_t);
        break;
      case 3: /* sysUpTime */
        od->instance = MIB_OBJECT_SCALAR;
        od->access = MIB_OBJECT_READ_ONLY;
        od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_TIMETICKS);
        od->v_len = sizeof(u32_t);
        break;
      case 4: /* sysContact */
        od->instance = MIB_OBJECT_SCALAR;
        od->access = MIB_OBJECT_READ_WRITE;
        od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR);
        od->v_len = syscontact_buf_len;
        break;
      case 5: /* sysName */
        od->instance = MIB_OBJECT_SCALAR;
        od->access = MIB_OBJECT_READ_WRITE;
        od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR);
        od->v_len = *sysname_len_ptr;
        break;
      case 6: /* sysLocation */
        od->instance = MIB_OBJECT_SCALAR;
        od->access = MIB_OBJECT_READ_WRITE;
        od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR);
        od->v_len = syslocation_buf_len;
        break;
      case 7: /* sysServices */
        od->instance = MIB_OBJECT_SCALAR;
        od->access = MIB_OBJECT_READ_ONLY;
        od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
        od->v_len = sizeof(s32_t);
        break;
      default:
        DBG_SNMP_PRINTF(("system_get_object_def: no such object\n"));
        od->instance = MIB_OBJECT_NONE;
        break;
    };
  }
  else
  {
    DBG_SNMP_PRINTF(("system_get_object_def: no scalar\n"));
    od->instance = MIB_OBJECT_NONE;
  }
}

/**
 * Returns system object value.
 *
 * @param ident_len the address length (2)
 * @param ident points to objectname.0 (object id trailer)
 * @param len return value space (in bytes)
 * @param value points to (varbind) space to copy value into.
 */
/*
 * Function: system_get_value
 * --------------------------
 * Retrieves system MIB object values.
 * Returns sysDescr, sysObjectID, sysUpTime, sysContact, sysName, sysLocation, sysServices.
 * Copies values into SNMP response varbind buffer.
 */
static void
system_get_value(struct obj_def *od, u16_t len, void *value) reentrant
{
  u8_t id;
  
  DBG_SNMP_PRINTF(("[%s][%d][len=%d]\n", __FILE__, (uint16)__LINE__, \
      (u16_t)len));
  id = od->id_inst_ptr[0];
  switch (id)
  {
    case 1: /* sysDescr */
      ocstrncpy(value,sysdescription_buf, len);
      //ocstrncpy(value,sysdescr_ptr,len);
      break;
    case 2: /* sysObjectID */
      objectidncpy((s32_t*)value,(s32_t*)sysobjid.id,len / sizeof(s32_t));
      break;
    case 3: /* sysUpTime */
      {
        snmp_get_sysuptime(value);
      }
      break;
    case 4: /* sysContact */
      ocstrncpy(value,syscontact_buf,len);
      break;
    case 5: /* sysName */
      ocstrncpy(value,sysname_ptr,len);
      break;
    case 6: /* sysLocation */
      ocstrncpy(value,syslocation_buf,len);
      break;
    case 7: /* sysServices */
      {
        s32_t *sint_ptr = value;
        *sint_ptr = sysservices;
      }
      break;
  };
}

/*
 * Function: system_set_test
 * -------------------------
 * Tests write permission for system MIB objects.
 * Validates length for sysContact, sysName, sysLocation.
 * Returns 1 if write allowed, 0 if denied.
 */
static u8_t
system_set_test(struct obj_def *od, u16_t len, void *value) reentrant
{
  u8_t id, set_ok;

  if (value) {}
  set_ok = 0;
  id = od->id_inst_ptr[0];
  
  switch (id)
  {
    case 4: /* sysContact */
      if ( len <= CONFIG_SYSTEM_CONTACT_LEN )
      {
        set_ok = 1;
      }
      break;
    case 5: /* sysName */
      sysname_ptr = sysname_buf;
      sysname_len_ptr = (u8_t*)&sysname_buf_len;
      if ( len <= CONFIG_SYSTEM_NAME_LEN )
      {
        set_ok = 1;
      }
      break;
    case 6: /* sysLocation */
      if ( len <= CONFIG_SYSTEM_LOCATION_LEN )
      {
        set_ok = 1;
      }
      break;
  };
  return set_ok;
}

/*
 * Function: system_set_value
 * --------------------------
 * Writes values to system MIB objects.
 * Updates sysContact, sysName, sysLocation buffers.
 * Stores length for variable-length string fields.
 */
static void
system_set_value(struct obj_def *od, u16_t len, void *value) reentrant
{
  u8_t id;

  id = od->id_inst_ptr[0];
  switch (id)
  {
    case 4: /* sysContact */
      ocstrncpy(syscontact_buf,value,len);
      syscontact_buf_len = len;
      break;
    case 5: /* sysName */
      ocstrncpy(sysname_ptr,value,len);
      *sysname_len_ptr = len;
      break;
    case 6: /* sysLocation */
      ocstrncpy(syslocation_buf,value,len);
      syslocation_buf_len = len;
      break;
  };
}

/*
 * Function: snmp_inc_system_info
 * ------------------------------
 * Updates system information buffers from flash.
 * Reads sysDescription, sysContact, sysLocation from flash.
 * Updates buffer lengths for SNMP responses.
 */
void snmp_inc_system_info( void )
{
    memset(sysdescription_buf, 0, CONFIG_SYSTEM_NAME_LEN);
    sal_flash_sys_description_get(sysdescription_buf);
    sysdescription_buf_len = strlen(sysdescription_buf);
    memset(syscontact_buf, 0, CONFIG_SYSTEM_NAME_LEN);
    sal_flash_sys_contact_get(syscontact_buf);
    syscontact_buf_len = strlen(syscontact_buf);
    memset(syslocation_buf, 0, CONFIG_SYSTEM_NAME_LEN);
    sal_flash_sys_location_get(syslocation_buf);
    syslocation_buf_len = strlen(syslocation_buf);
}

//#if LWPS_SNMP_INTERFACE_GROUP
#if LWPS_SNMP_IFTABLE
/*
 * Function: interfaces_get_object_def
 * -----------------------------------
 * Defines interfaces scalar object (ifNumber).
 * Returns object definition for ifNumber OID.
 * Sets ASN.1 type to integer with read-only access.
 */
static void
interfaces_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od)  reentrant
{
 /* return to object name, adding index depth (1) */
    ident_len += 1;
    ident -= 1;
    if (ident_len == 2)
    {
        od->id_inst_len = ident_len;
        od->id_inst_ptr = ident;

        od->instance = MIB_OBJECT_SCALAR;
        od->access = MIB_OBJECT_READ_ONLY;
        od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
        od->v_len = sizeof(s32_t);
    }
    else
    {
        DBG_SNMP_PRINTF(("interfaces_get_object_def: no scalar\n"));
        od->instance = MIB_OBJECT_NONE;
    }
}

/*
 * Function: interfaces_get_value
 * ------------------------------
 * Returns number of interfaces (2 for ports 3 and 4).
 * Fixed value for ifNumber OID.
 * Only shows 2 interfaces in SNMP MIB.
 */
static void
interfaces_get_value(struct obj_def *od, u16_t len, void *value)  reentrant
{
    if (len)
    {
    }
    if (od->id_inst_ptr[0] == 1)
    {
        s32_t *sint_ptr = value;
        *sint_ptr = 2; // Only show 2 interfaces (ports 3 and 4)
    }
}

/*
 * Function: ifentry_get_object_def
 * --------------------------------
 * Defines ifTable entry objects for each interface.
 * Handles ifIndex, ifDescr, ifType, ifMtu, ifSpeed, ifPhysAddress, etc.
 * Sets ASN.1 types and access rights for each OID.
 */
static void
ifentry_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od)  reentrant
{
    u8_t xdata  id;
    u8_t xdata index;

    DBG_SNMP_PRINTF(("[%s][%d]\n", __FILE__, (uint16)__LINE__));

    index = od->id_inst_ptr[1];
    
    /* return to object name, adding index depth (1) */
    ident_len += 1;
    ident -= 1;
    if (ident_len == 2)
    {
        od->id_inst_len = ident_len;
        od->id_inst_ptr = ident;

        id = ident[0];
        DBG_SNMP_PRINTF(("ifentry_get_object_def %" U16_F "\n", (u16_t) id));
        switch (id)
        {
        case 1:                /* ifIndex */
        case 3:                /* ifType */
        case 4:                /* ifMtu */
        case 8:                /* ifOperStatus */
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
            od->v_len = sizeof(s32_t);
            break;
        case 2:                /* ifDescr */
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR);
            if(index > 9)
                od->v_len = 6;
            else
                od->v_len = 5;
            break;
        case 5:                /* ifSpeed */
        case 21:               /* ifOutQLen */
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_GAUGE);
            od->v_len = sizeof(u32_t);
            break;
        case 6:                /* ifPhysAddress */
            {
                od->instance = MIB_OBJECT_TAB;
                od->access = MIB_OBJECT_READ_ONLY;
                od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OC_STR);
                od->v_len = 6;
            }
            break;
        case 7:                /* ifAdminStatus */
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_WRITE;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
            od->v_len = sizeof(s32_t);
            break;
        case 9:                /* ifLastChange */
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_TIMETICKS);
            od->v_len = sizeof(u32_t);
            break;
        case 10:               /* ifInOctets */
        case 11:               /* ifInUcastPkts */
        case 12:               /* ifInNUcastPkts */
        case 13:               /* ifInDiscarts */
        case 14:               /* ifInErrors */
        case 15:               /* ifInUnkownProtos */
        case 16:               /* ifOutOctets */
        case 17:               /* ifOutUcastPkts */
        case 18:               /* ifOutNUcastPkts */
        case 19:               /* ifOutDiscarts */
        case 20:               /* ifOutErrors */
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_APPLIC | SNMP_ASN1_PRIMIT | SNMP_ASN1_COUNTER);
            od->v_len = sizeof(u32_t);
            break;
        case 22:               /* ifSpecific */
                /** @note returning zeroDotZero (0.0) no media specific MIB support */
            od->instance = MIB_OBJECT_TAB;
            od->access = MIB_OBJECT_READ_ONLY;
            od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_OBJ_ID);
//            od->v_len =  ifspecific.len  +  ifspecific.len * sizeof(s32_t);
             od->v_len = 0;
            break;
        default:
            DBG_SNMP_PRINTF(("ifentry_get_object_def: no such object\n"));
            od->instance = MIB_OBJECT_NONE;
            break;
        }
    }
    else
    {
        DBG_SNMP_PRINTF(("ifentry_get_object_def: no scalar\n"));
        od->instance = MIB_OBJECT_NONE;
    }
}



/*
 * Function: ifentry_get_value
 * ---------------------------
 * Retrieves ifTable entry values for each interface.
 * Maps interface index (1,2) to physical ports (3,4).
 * Returns interface statistics like speed, MAC, packet counts.
 */
static void
ifentry_get_value(struct obj_def *od, u16_t len, void *value)  reentrant
{
    u8_t xdata index;
    u8_t xdata id;
    u8_t xdata tmp[3];
    u8_t *u8_ptr = value;
    u8_t physical_port; // Map to actual physical port
    

    /*kill compile warning */
    if (len)
    {
    }

    index = od->id_inst_ptr[1];
    id = od->id_inst_ptr[0];
    
    // Map interface index to physical port: 1->3, 2->4
    if (index == 1) {
        physical_port = 3;
    } else if (index == 2) {
        physical_port = 4;
    } else {
        return; // Invalid index, return without processing
    }

    switch (id)
    {
    case 1:                    /* ifIndex */
        {
           s32_t *xdata  sint_ptr = value;
           *sint_ptr = od->id_inst_ptr[1];
        }
        break;
 
    case 2:                    /* ifDescr */
        {
           u8_ptr[0] = 'p';
        u8_ptr[1] = 'o';
        u8_ptr[2] = 'r';
        u8_ptr[3] = 't';

            // Show port names as 1,2 for UI, even though data comes from ports 3,4
            if( index >= 10)
        {
        tmp[0] = (index/10)+'0';
        tmp[1] = (index%10)+'0';
        }
        else
        {
        tmp[0] = (index)+'0';
        tmp[1] = '\0';
         }
        if( (len-4) < ((u16_t)0xFFF0U) )
        {
        ocstrncpy(value+4, tmp, len-4);
        }
        }
        break;
    case 3:                    /* ifType */
        {
            u32_t *xdata uint_ptr = value;
            *uint_ptr = 117;  /*find after time*/
        }
        break;
    case 4:                    /* ifMtu */
        {
            u32_t *xdata uint_ptr = value;
            *uint_ptr = P_BUFFER_SIZE;
        }
        break;
    case 5:                    /* ifSpeed */
        {
            u32_t *xdata  uint_ptr = value;
            sal_port_macStatus_t xdata macStatus;
            sal_enable_t xdata linkstatus;
            sal_port_link_get(physical_port-1, &linkstatus);
            if(linkstatus == SAL_DISABLED)
                *uint_ptr = 0;
            else
            {
             sal_port_macStatus_get(physical_port-1, &macStatus);
             if(macStatus.speed == 0)
                 *uint_ptr = 10000000;
             else if(macStatus.speed == 1)
                *uint_ptr = 100000000;
             else if(macStatus.speed == 2)
                *uint_ptr = 1000000000;
             else
                *uint_ptr = 0;
            }
        }
        break;
    case 6:                    /* ifPhysAddress */
        {
            u8_t *xdata  uint8_ptr = value;
            sal_mac_t sysmac ;
            sal_sys_mac_get(&sysmac);
            ocstrncpy(uint8_ptr, sysmac.octet,6);
        }
        break;
    case 7:                    /* ifAdminStatus */
        {
            u32_t *xdata uint_ptr = value;
            sal_enable_t xdata  adminEnable;
            /* Memory corruption
            sal_port_adminEnable_get(physical_port -1, &adminEnable);
           if(adminEnable == SAL_DISABLED)
                *uint_ptr = 2;
            else
            */
                *uint_ptr = 1;
        }
        break;
    case 8:                    /* ifOperStatus */
        {
            u32_t *xdata uint_ptr = value;
            sal_enable_t xdata linkstatus;
            sal_port_link_get(physical_port -1, &linkstatus);
            if(linkstatus == SAL_DISABLED)
                *uint_ptr = 2;
            else
                *uint_ptr = 1;
        }
        break;
    case 9:                    /* ifLastChange */
        {
            u32_t *xdata  uint_ptr = value;
            *uint_ptr = linkChangeTime[physical_port-1];
        }
        break;
    case 10:                   /* ifInOctets */
        {
            u32_t xdata value64[2];
            GET_IF_HC_IN_OCTETS(((u8_t)physical_port-1),((u8_t)(sizeof(u32_t)*2)), (u8_t *)value64);
            uncpy((u32_t *) value, (u32_t *) (value64 + 1), 1);
        }
        break;
    case 11:                   /* ifInUcastPkts */
        {
            u32_t * xdata uint_ptr = value;
            GET_IF_HC_IN_UCAST_PKTS(((u8_t)physical_port-1),((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 12:                   /* ifInNUcastPkts */
        {
            u32_t * xdata  uint_ptr = value;
            u32_t  xdata tmpcount ;
            GET_IF_IN_MULTICAST_PKTS(((u8_t)physical_port-1),((u8_t)sizeof(u32_t)), ((u8_t*)&tmpcount));
            *uint_ptr = tmpcount;
            GET_IF_IN_BROADCAST_PKTS(((u8_t)physical_port-1),((u8_t)sizeof(u32_t)), ((u8_t*)&tmpcount));
            *uint_ptr += tmpcount;
        }
        break;
    case 13:                   /* ifInDiscards */
        {
            u32_t *xdata  uint_ptr = value;
            GET_DOT1D_IN_DISCARDS_PKTS(physical_port-1,((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 14:                  /* ifInErrors */
        {
            u32_t *xdata  uint_ptr = value;
            u32_t  xdata tmpcount ;
#if MIB_ETHERNET_LIKE_ENABLE
            GET_DOT3_STATS_ALIGNMENT_ERRORS((u8_t)physical_port-1,((u8_t)sizeof(u32_t)), ((u8_t*)&tmpcount));
            *uint_ptr = tmpcount;
            GET_DOT3_STATS_FCS_ERRORS((u8_t)physical_port-1,((u8_t)sizeof(u32_t)), ((u8_t*)&tmpcount));
            *uint_ptr += tmpcount;
            GET_DOT3_STATS_FRAME_TOO_LONGS((u8_t)physical_port-1,((u8_t)sizeof(u32_t)), ((u8_t*)&tmpcount));
            *uint_ptr += tmpcount;
#endif
#if MIB_RMON_ENABLE
            GET_ETHER_STATS_FRAGMENTS((u8_t)physical_port-1,((u8_t)sizeof(u32_t)), ((u8_t*)&tmpcount));
             *uint_ptr += tmpcount;
            GET_ETHER_STATS_JABBERS((u8_t)physical_port-1,((u8_t)sizeof(u32_t)), ((u8_t*)&tmpcount));
             *uint_ptr += tmpcount;
#endif
        }
        break;
    case 15:                   /* ifInUnknownProtos */
        {
            u32_t *xdata  uint_ptr = value;
            *uint_ptr = 0;
        }
        break;
    case 16:                   /* ifOutOctets */
        {
            u32_t xdata value64[2];
            GET_IF_HC_OUT_OCTETS(((u8_t)physical_port-1),((u8_t)(sizeof(u32_t)*2)), ((u8_t*)value64));
            uncpy((u32_t *) value, (u32_t *) (value64 + 1), 1);
        }
        break;
    case 17:                   /* ifOutUcastPkts */
        {
            u32_t *xdata  uint_ptr = value;
            GET_IF_HC_OUT_UCAST_PKTS(((u8_t)physical_port-1),((u8_t)sizeof(u32_t)), ((u8_t*)uint_ptr));
        }
        break;
    case 18:                   /* ifOutNUcastPkts */
        {
            u32_t *xdata  uint_ptr = value;
            u32_t  xdata tmpcount ;
            GET_IF_HC_OUT_MULTICAST_PKTS(((u8_t)physical_port-1),((u8_t)sizeof(u32_t)), ((u8_t*)&tmpcount));
            *uint_ptr = tmpcount;
            GET_IF_HC_OUT_BROADCAST_PKTS(((u8_t)physical_port-1),((u8_t)sizeof(u32_t)), ((u8_t*)&tmpcount));
            *uint_ptr += tmpcount;
        }
        break;
    case 19:                   /* ifOutDiscards */
        {
            u32_t *xdata  uint_ptr = value;
            *uint_ptr = 0;
        }
        break;
    case 20:                   /* ifOutErrors */
        {
            u32_t *xdata  uint_ptr = value;
            *uint_ptr = 0;
        }
        break;
     case 21:                   /* ifOutQLen */
        {
            u32_t *xdata  uint_ptr = value;
            *uint_ptr = 0;
        }
        break;
    case 22:                   /* ifSpecific */
        {
            /*
            u8_t *xdata uint_ptr = value;
            u32_t *xdata uint = value + 1;
            
            *uint_ptr = 2;
            *(uint + 1) = 0;
            *(uint + 2) = 0;
            */
        }
        break;
    };
}


#if !SNMP_SAFE_REQUESTS
/*
 * Function: ifentry_set_test
 * --------------------------
 * Tests write permission for ifEntry objects.
 * Only allows ifAdminStatus (id=7) with valid values (1 or 2).
 * Returns 1 if write allowed, 0 if denied.
 */
static u8_t ifentry_set_test (struct obj_def *od, u16_t len, void *value)  reentrant
{
    u8_t id, set_ok;
    xdata u32_t *xdata int_ptr = value;

    if (len)
    {
    }
    if (value)
    {
    }
    set_ok = 0;
    id = od->id_inst_ptr[0];
    switch (id)
    {
      case 7:
        if ((*int_ptr == 1) || (*int_ptr == 2))
            set_ok = 1;
        break;
    }
    return set_ok;
}

/*
 * Function: ifentry_set_value
 * ---------------------------
 * Writes values to ifEntry objects.
 * Sets ifAdminStatus to enable/disable port.
 * Maps interface index (1,2) to physical ports (3,4).
 */
static void ifentry_set_value (struct obj_def *od, u16_t len, void *value)  reentrant
{
    u8_t id, index;
    u32_t *uint_ptr = value;
    sal_enable_t adminEnable;
    u8_t physical_port;
    
    if (len)
    {
    }
    id = od->id_inst_ptr[0];
    index = od->id_inst_ptr[1];

    // Map interface index to physical port: 1->3, 2->4
    if (index == 1) {
        physical_port = 3;
    } else if (index == 2) {
        physical_port = 4;
    } else {
        return; // Invalid index, return without processing
    }

    if(*uint_ptr == 1)
        adminEnable = SAL_ENABLED;
    else
        adminEnable = SAL_DISABLED;
    
    switch (id)
    {
    case 7:
        sal_port_adminEnable_set(physical_port -1, adminEnable);
        break;
    }
}
#endif

/*
 * Function: snmp_inc_syslist
 * --------------------------
 * Inserts nodes into system list (currently empty).
 * Placeholder for MIB list node insertion.
 * Not fully implemented.
 */
void snmp_inc_syslist( void )
{
    struct mib_list_node *xdata if_node = NULL;
    snmp_mib_node_insert(&iflist_root, iflist_root.count + 1, &if_node);


}

/*
 * Function: snmp_inc_iflist
 * -------------------------
 * Inserts interface nodes into ifTable list.
 * Adds 2 entries for ports 3 and 4.
 * Enables getnext traversal on the ifTable.
 */
void snmp_inc_iflist(void)
{
    struct mib_list_node *xdata if_node = NULL;
    // Add only 2 interfaces (for ports 3 and 4)
    snmp_mib_node_insert(&iflist_root, 1, &if_node);
    snmp_mib_node_insert(&iflist_root, 2, &if_node);
    /* enable getnext traversal on filled table */
    iftable.maxlength = 1;
}
#endif

#endif /* LWIP_SNMP */