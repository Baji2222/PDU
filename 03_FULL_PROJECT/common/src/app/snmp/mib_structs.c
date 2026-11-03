#pragma STRING (far)
#include <stdio.h>
#include <string.h>
/**
 * @file
 * MIB tree access/construction functions.
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

#if LWIP_SNMP /* don't build if not configured for use in lwipopts.h */
#include <lwps/lwps_api.h>
#include <snmp/snmp_structs.h>
#include <snmp/mem.h>
#include <snmp/snmp_debug.h>

/** .iso.org.dod.internet address prefix, @see snmp_iso_*() */
const s32_t code prefix[4] = {1, 3, 6, 1};

#define NODE_STACK_SIZE (LWIP_SNMP_OBJ_ID_LEN)
/** node stack entry (old news?) */
struct nse
{
  /** right child */
  struct mib_node* r_ptr;
  /** right child identifier */
  s32_t r_id;
  /** right child next level */
  u8_t r_nl;
};
static u8_t node_stack_cnt;
static struct nse node_stack[NODE_STACK_SIZE];

/**
 * Pushes nse struct onto stack.
 */
/*
 * Function: push_node
 * -------------------
 * Pushes a node stack entry onto the MIB traversal stack.
 * Used for depth-first tree traversal during getnext operations.
 * Asserts stack size to prevent overflow.
 */
static void
push_node(struct nse* node)
{
  LWIP_ASSERT("node_stack_cnt < NODE_STACK_SIZE",node_stack_cnt < NODE_STACK_SIZE);
  DBG_SNMP_PRINTF(("[%s][%d]push_node() node=%p id=%"S32_F"\n",\
  	__FILE__, (uint16)__LINE__, (void*)(node->r_ptr),node->r_id));
  if (node_stack_cnt < NODE_STACK_SIZE)
  {
    node_stack[node_stack_cnt] = *node;
    node_stack_cnt++;
  }
}

/**
 * Pops nse struct from stack.
 */
/*
 * Function: pop_node
 * ------------------
 * Pops a node stack entry from the MIB traversal stack.
 * Used to backtrack during tree traversal.
 * Updates stack count and returns the popped entry.
 */
static void
pop_node(struct nse* node)
{
  if (node_stack_cnt > 0)
  {
    node_stack_cnt--;
    *node = node_stack[node_stack_cnt];
  }
  DBG_SNMP_PRINTF(("[%s][%d]pop_node() node=%p id=%"S32_F"\n", \
  	__FILE__, (uint16)__LINE__,(void *)(node->r_ptr),node->r_id));
}

/**
 * Conversion from ifIndex to lwIP netif
 * @param ifindex is a s32_t object sub-identifier
 * @param netif points to returned netif struct pointer
 */
#if 0 /* Vic Remove Function - snmp_ifindextonetif */
void
snmp_ifindextonetif(s32_t ifindex, struct netif **netif)
{

  struct netif *nif = netif_list;
  u16_t i, ifidx;

  ifidx = ifindex - 1;
  i = 0;
  while ((nif != NULL) && (i < ifidx))
  {
    nif = nif->next;
    i++;
  }
  *netif = nif;
}
#endif /* end of Vic Remove Function - snmp_ifindextonetif  */


/**
 * Conversion from lwIP netif to ifIndex
 * @param netif points to a netif struct
 * @param ifidx points to s32_t object sub-identifier
 */

#if 0 /* Vic Remove Function - snmp_netiftoifindex */
void
snmp_netiftoifindex(struct netif *netif, s32_t *ifidx)
{

  struct netif *nif = netif_list;
  u16_t i;

  i = 0;
  while (nif != netif)
  {
    nif = nif->next;
    i++;
  }
  *ifidx = i+1;

}
#endif /* end of Vic Remove Function - snmp_netiftoifindex */

#if 0 /* uncalled function */
/**
 * Conversion from oid to lwIP ip_addr
 * @param ident points to s32_t ident[4] input
 * @param ip points to output struct
 */
void
snmp_oidtoip(s32_t *ident, ip_addr_t *ip)
{
  ip->addr[0] = ident[0];
  ip->addr[1] = ident[1];
  ip->addr[2] = ident[2];
  ip->addr[3] = ident[3];
}

/**
 * Conversion from lwIP ip_addr to oid
 * @param ip points to input struct
 * @param ident points to s32_t ident[4] output
 */
void
snmp_iptooid(ip_addr_t *ip, s32_t *ident)
{
#if 1 /* Vic Add */
  ident[0] = ip->addr[0];
  ident[1] = ip->addr[1];
  ident[2] = ip->addr[2];
  ident[3] = ip->addr[3];
#else /* else of Vic Add */
  u32_t ipa;

  ipa = ip->addr;
  ident[0] = (ipa >> 24) & 0xff;
  ident[1] = (ipa >> 16) & 0xff;
  ident[2] = (ipa >> 8) & 0xff;
  ident[3] = ipa & 0xff;
#endif /* end of else of Vic Add */
}
#endif

/*
 * Function: snmp_mib_ln_alloc
 * ---------------------------
 * Allocates a new MIB list node with given object ID.
 * Sets prev/next pointers to NULL and stores objid.
 * Returns pointer to allocated node or NULL on failure.
 */
struct mib_list_node *
snmp_mib_ln_alloc(s32_t id)
{
  struct mib_list_node *ln;

  ln = (struct mib_list_node *)mem_malloc(sizeof(struct mib_list_node));
  if (ln != NULL)
  {
    ln->prev = NULL;
    ln->next = NULL;
    ln->objid = id;
    ln->nptr = NULL;
  }
  return ln;
}

#if 0 /* uncalled function */
void
snmp_mib_ln_free(struct mib_list_node *ln)
{
  mem_free(ln);
}
#endif

/*
 * Function: snmp_mib_lrn_alloc
 * ----------------------------
 * Allocates a new MIB list root node structure.
 * Initializes function pointers to noleaf handlers.
 * Sets node type to MIB_NODE_LR and clears list pointers.
 */
struct mib_list_rootnode *
snmp_mib_lrn_alloc(void)
{
  struct mib_list_rootnode xdata *lrn;

  lrn = (struct mib_list_rootnode*)mem_malloc(sizeof(struct mib_list_rootnode));
  if (lrn != NULL)
  {
    lrn->get_object_def = noleafs_get_object_def;
    lrn->get_value = noleafs_get_value;
    lrn->set_test = noleafs_set_test;
    lrn->set_value = noleafs_set_value;
    lrn->node_type = MIB_NODE_LR;
    lrn->maxlength = 0;
    lrn->head = NULL;
    lrn->tail = NULL;
    lrn->count = 0;
  }
  return lrn;
}

#if 0 /* uncalled function */
void
snmp_mib_lrn_free(struct mib_list_rootnode *lrn)
{
  mem_free(lrn);
}
#endif

/**
 * Inserts node in idx list in a sorted
 * (ascending order) fashion and
 * allocates the node if needed.
 *
 * @param rn points to the root node
 * @param objid is the object sub identifier
 * @param insn points to a pointer to the inserted node
 *   used for constructing the tree.
 * @return -1 if failed, 1 if inserted, 2 if present.
 */
/*
 * Function: snmp_mib_node_insert
 * ------------------------------
 * Inserts a node into a MIB list in ascending order.
 * Returns -1 on allocation failure, 1 if inserted, 2 if already present.
 * Used for building SNMP MIB tree structures.
 */
s8_t
snmp_mib_node_insert(struct mib_list_rootnode *rn, s32_t objid, struct mib_list_node **insn) large
{
  struct mib_list_node *nn;
  s8_t insert;

  LWIP_ASSERT("rn != NULL",rn != NULL);

  /* -1 = malloc failure, 0 = not inserted, 1 = inserted, 2 = was present */
  insert = 0;
  if (rn->head == NULL)
  {
    /* empty list, add first node */
    DBG_SNMP_PRINTF(("[%s][%d]alloc empty list objid==%"S32_F"\n", __FILE__, (uint16)__LINE__,objid));
    nn = snmp_mib_ln_alloc(objid);
    if (nn != NULL)
    {
      rn->head = nn;
      rn->tail = nn;
      *insn = nn;
      insert = 1;
    }
    else
    {
      insert = -1;
    }
  }
  else
  {
    struct mib_list_node *n;
    /* at least one node is present */
    n = rn->head;
    while ((n != NULL) && (insert == 0))
    {
      if (n->objid == objid)
      {
        /* node is already there */
        DBG_SNMP_PRINTF(("[%s][%d]node already there objid==%"S32_F"\n", __FILE__, (uint16)__LINE__,objid));
        *insn = n;
        insert = 2;
      }
      else if (n->objid < objid)
      {
        if (n->next == NULL)
        {
          /* alloc and insert at the tail */
          DBG_SNMP_PRINTF(("[%s][%d]alloc ins tail objid==%"S32_F"\n", __FILE__, (uint16)__LINE__,objid));
          nn = snmp_mib_ln_alloc(objid);
          if (nn != NULL)
          {
            nn->next = NULL;
            nn->prev = n;
            n->next = nn;
            rn->tail = nn;
            *insn = nn;
            insert = 1;
          }
          else
          {
            /* insertion failure */
            insert = -1;
          }
        }
        else
        {
          /* there's more to explore: traverse list */
          DBG_SNMP_PRINTF(("[%s][%d]traverse list\n", __FILE__, (uint16)__LINE__));
          n = n->next;
        }
      }
      else
      {
        /* n->objid > objid */
        /* alloc and insert between n->prev and n */
        DBG_SNMP_PRINTF(("[%s][%d]alloc ins n->prev, objid==%"S32_F", n\n", __FILE__, (uint16)__LINE__,objid));
        nn = snmp_mib_ln_alloc(objid);
        if (nn != NULL)
        {
          if (n->prev == NULL)
          {
            /* insert at the head */
            nn->next = n;
            nn->prev = NULL;
            rn->head = nn;
            n->prev = nn;
          }
          else
          {
            /* insert in the middle */
            nn->next = n;
            nn->prev = n->prev;
            n->prev->next = nn;
            n->prev = nn;
          }
          *insn = nn;
          insert = 1;
        }
        else
        {
          /* insertion failure */
          insert = -1;
        }
      }
    }
  }
  if (insert == 1)
  {
    rn->count += 1;
  }
  LWIP_ASSERT("insert != 0",insert != 0);
  return insert;
}

#if 0 /* uncalled function */
/**
 * Finds node in idx list and returns deletion mark.
 *
 * @param rn points to the root node
 * @param objid  is the object sub identifier
 * @param fn returns pointer to found node
 * @return 0 if not found, 1 if deletable,
 *   2 can't delete (2 or more children), 3 not a list_node
 */
s8_t
snmp_mib_node_find(struct mib_list_rootnode *rn, s32_t objid, struct mib_list_node **fn)
{
  s8_t fc;
  struct mib_list_node *n;

  LWIP_ASSERT("rn != NULL",rn != NULL);
  n = rn->head;
  while ((n != NULL) && (n->objid != objid))
  {
    n = n->next;
  }
  if (n == NULL)
  {
    fc = 0;
  }
  else if (n->nptr == NULL)
  {
    /* leaf, can delete node */
    fc = 1;
  }
  else
  {
    struct mib_list_rootnode *r;

    if (n->nptr->node_type == MIB_NODE_LR)
    {
      r = (struct mib_list_rootnode *)n->nptr;
      if (r->count > 1)
      {
        /* can't delete node */
        fc = 2;
      }
      else
      {
        /* count <= 1, can delete node */
        fc = 1;
      }
    }
    else
    {
      /* other node type */
      fc = 3;
    }
  }
  *fn = n;
  return fc;
}


/**
 * Removes node from idx list
 * if it has a single child left.
 *
 * @param rn points to the root node
 * @param n points to the node to delete
 * @return the nptr to be freed by caller
 */
struct mib_list_rootnode *
snmp_mib_node_delete(struct mib_list_rootnode *rn, struct mib_list_node *n)
{
  struct mib_list_rootnode *next;

  LWIP_ASSERT("rn != NULL",rn != NULL);
  LWIP_ASSERT("n != NULL",n != NULL);

  /* caller must remove this sub-tree */
  next = (struct mib_list_rootnode*)(n->nptr);
  rn->count -= 1;

  if (n == rn->head)
  {
    rn->head = n->next;
    if (n->next != NULL)
    {
      /* not last node, new list begin */
      n->next->prev = NULL;
    }
  }
  else if (n == rn->tail)
  {
    rn->tail = n->prev;
    if (n->prev != NULL)
    {
      /* not last node, new list end */
      n->prev->next = NULL;
    }
  }
  else
  {
    /* node must be in the middle */
    n->prev->next = n->next;
    n->next->prev = n->prev;
  }
  DBG_SNMP_PRINTF(("free list objid==%"S32_F"\n",n->objid));
  snmp_mib_ln_free(n);
  if (rn->count == 0)
  {
    rn->head = NULL;
    rn->tail = NULL;
  }
  return next;
}
#endif 


/**
 * Searches tree for the supplied (scalar?) object identifier.
 *
 * @param node points to the root of the tree ('.internet')
 * @param ident_len the length of the supplied object identifier
 * @param ident points to the array of sub identifiers
 * @param np points to the found object instance (rerurn)
 * @return pointer to the requested parent (!) node if success, NULL otherwise
 */
/*
 * Function: snmp_search_tree
 * ---------------------------
 * Searches the MIB tree for a given object identifier.
 * Handles array, list, external, and scalar node types.
 * Returns parent node and fills name pointer on success.
 */
struct mib_node *
snmp_search_tree(struct mib_node *node, u8_t ident_len, s32_t *ident, struct snmp_name_ptr *np)
{
  u8_t node_type, ext_level;

  ext_level = 0;
  DBG_SNMP_PRINTF(("node==%p *ident==%"S32_F"\n",(void*)node,*ident));

  while (node != NULL)
  {
    node_type = node->node_type;
    if ((node_type == MIB_NODE_AR) || (node_type == MIB_NODE_RA))
    {
      struct mib_array_node *an;
      u16_t i;

      if (ident_len > 0)
      {
        /* array node (internal ROM or RAM, fixed length) */
        an = (struct mib_array_node *)node;
        i = 0;
        DBG_SNMP_PRINTF(("maxlen=%d\n", (uint16) an->maxlength));
        while ((i < an->maxlength) && (an->objid[i] != *ident))
        {
          DBG_SNMP_PRINTF(("objid=%"S32_F" *ident==%"S32_F"\n",an->objid[i],*ident));
          i++;
        }
        if (i < an->maxlength)
        {
          /* found it, if available proceed to child, otherwise inspect leaf */
          DBG_SNMP_PRINTF(("[%s][%d]an->objid[%"U16_F"]==%"S32_F" *ident==%"S32_F"\n", \
            __FILE__, (uint16)__LINE__, i,an->objid[i],*ident));
          if (an->nptr[i] == NULL)
          {
            /* a scalar leaf OR table,
               inspect remaining instance number / table index */
            np->ident_len = ident_len;
            np->ident = ident;
            return (struct mib_node*)an;
          }
          else
          {
            /* follow next child pointer */
            ident++;
            ident_len--;
            node = an->nptr[i];
          }
        }
        else
        {
          /* search failed, identifier mismatch (nosuchname) */
          DBG_SNMP_PRINTF(("an search failed *ident==%"S32_F"\n",*ident));
          return NULL;
        }
      }
      else
      {
        /* search failed, short object identifier (nosuchname) */
        DBG_SNMP_PRINTF(("an search failed, short object identifier\n"));
        return NULL;
      }
    }
    else if(node_type == MIB_NODE_LR)
    {
      struct mib_list_rootnode *lrn;
      struct mib_list_node *ln;

      if (ident_len > 0)
      {
        /* list root node (internal 'RAM', variable length) */
        lrn = (struct mib_list_rootnode *)node;
        ln = lrn->head;
        /* iterate over list, head to tail */
        while ((ln != NULL) && (ln->objid != *ident))
        {
          ln = ln->next;
        }
        if (ln != NULL)
        {
          /* found it, proceed to child */;
          DBG_SNMP_PRINTF(("[%s][%d]ln->objid==%"S32_F" *ident==%"S32_F"\n", \
		  	__FILE__, (uint16)__LINE__,ln->objid,*ident));
          if (ln->nptr == NULL)
          {
            np->ident_len = ident_len;
            np->ident = ident;
            return (struct mib_node*)lrn;
          }
          else
          {
            /* follow next child pointer */
            ident_len--;
            ident++;
            node = ln->nptr;
          }
        }
        else
        {
          /* search failed */
          DBG_SNMP_PRINTF(("ln search failed *ident==%"S32_F"\n",*ident));
          return NULL;
        }
      }
      else
      {
        /* search failed, short object identifier (nosuchname) */
        DBG_SNMP_PRINTF(("ln search failed, short object identifier\n"));
        return NULL;
      }
    }
    else if(node_type == MIB_NODE_EX)
    {
      struct mib_external_node *en;
      u16_t i, len;

      if (ident_len > 0)
      {
        /* external node (addressing and access via functions) */
        en = (struct mib_external_node *)node;

        i = 0;
        len = en->level_length(en->addr_inf,ext_level);
        while ((i < len) && (en->ident_cmp(en->addr_inf,ext_level,i,*ident) != 0))
        {
          i++;
        }
        if (i < len)
        {
          s32_t debug_id;

          en->get_objid(en->addr_inf,ext_level,i,&debug_id);
          DBG_SNMP_PRINTF(("en->objid==%"S32_F" *ident==%"S32_F"\n",debug_id,*ident));
          if ((ext_level + 1) == en->tree_levels)
          {
            np->ident_len = ident_len;
            np->ident = ident;
            return (struct mib_node*)en;
          }
          else
          {
            /* found it, proceed to child */
            ident_len--;
            ident++;
            ext_level++;
          }
        }
        else
        {
          /* search failed */
          DBG_SNMP_PRINTF(("en search failed *ident==%"S32_F"\n",*ident));
          return NULL;
        }
      }
      else
      {
        /* search failed, short object identifier (nosuchname) */
        DBG_SNMP_PRINTF(("en search failed, short object identifier\n"));
        return NULL;
      }
    }
    else if (node_type == MIB_NODE_SC)
    {
      mib_scalar_node *sn;

      sn = (mib_scalar_node *)node;
      if ((ident_len == 1) && (*ident == 0))
      {
        np->ident_len = ident_len;
        np->ident = ident;
        return (struct mib_node*)sn;
      }
      else
      {
        /* search failed, short object identifier (nosuchname) */
        DBG_SNMP_PRINTF(("search failed, invalid object identifier length\n"));
        return NULL;
      }
    }
    else
    {
      /* unknown node_type */
      DBG_SNMP_PRINTF(("search failed node_type %"U16_F" unkown\n",(u16_t)node_type));
      return NULL;
    }
  }
  /* done, found nothing */
  DBG_SNMP_PRINTF(("search failed node==%p\n",(void*)node));
  return NULL;
}

/**
 * Test table for presence of at least one table entry.
 */
/*
 * Function: empty_table
 * ---------------------
 * Tests if a MIB table node has any entries.
 * Returns 1 if table is empty, 0 if it has entries.
 * Supports array, list, and external node types.
 */
static u8_t
empty_table(struct mib_node *node)
{
  u8_t node_type;
  u8_t empty = 0;

  if (node != NULL)
  {
    node_type = node->node_type;
    if (node_type == MIB_NODE_LR)
    {
      struct mib_list_rootnode *lrn;
      lrn = (struct mib_list_rootnode *)node;
      if ((lrn->count == 0) || (lrn->head == NULL))
      {
        empty = 1;
      }
    }
    else if ((node_type == MIB_NODE_AR) || (node_type == MIB_NODE_RA))
    {
      struct mib_array_node *an;
      an = (struct mib_array_node *)node;
      if ((an->maxlength == 0) || (an->nptr == NULL))
      {
        empty = 1;
      }
    }
    else if (node_type == MIB_NODE_EX)
    {
      struct mib_external_node *en;
      en = (struct mib_external_node *)node;
      if (en->tree_levels == 0)
      {
        empty = 1;
      }
    }
  }
  return empty;
}

/**
 * Tree expansion.
 */
/*
 * Function: snmp_expand_tree
 * --------------------------
 * Expands a MIB tree from a given OID for getnext operations.
 * Traverses tree depth-first and returns next available OID.
 * Uses stack for backtracking through the tree structure.
 */
struct mib_node *
snmp_expand_tree(struct mib_node *node, u8_t ident_len, s32_t *ident, struct snmp_obj_id *oidret)
{
  u8_t node_type, ext_level, climb_tree;

  ext_level = 0;
  /* reset node stack */
  node_stack_cnt = 0;
  while (node != NULL)
  {
    climb_tree = 0;
    node_type = node->node_type;
    if ((node_type == MIB_NODE_AR) || (node_type == MIB_NODE_RA))
    {
      struct mib_array_node *an;
      u16_t i;

      /* array node (internal ROM or RAM, fixed length) */
      an = (struct mib_array_node *)node;
      if (ident_len > 0)
      {
        i = 0;
        while ((i < an->maxlength) && (an->objid[i] < *ident))
        {
          i++;
        }
        if (i < an->maxlength)
        {
          DBG_SNMP_PRINTF(("[%s][%d]an->objid[%"U16_F"]==%"S32_F" *ident==%"S32_F"\n", \
		  	__FILE__, (uint16)__LINE__,i,an->objid[i],*ident));
          /* add identifier to oidret */
          oidret->id[oidret->len] = an->objid[i];
          (oidret->len)++;

          if (an->nptr[i] == NULL)
          {
            DBG_SNMP_PRINTF(("[%s][%d]leaf node\n", __FILE__, (uint16)__LINE__));
            /* leaf node (e.g. in a fixed size table) */
            if (an->objid[i] > *ident)
            {
              return (struct mib_node*)an;
            }
            else if ((i + 1) < an->maxlength)
            {
              /* an->objid[i] == *ident */
              (oidret->len)--;
              oidret->id[oidret->len] = an->objid[i + 1];
              (oidret->len)++;
              return (struct mib_node*)an;
            }
            else
            {
              /* (i + 1) == an->maxlength */
              (oidret->len)--;
              climb_tree = 1;
            }
          }
          else
          {
            u8_t j;
            struct nse cur_node;

            DBG_SNMP_PRINTF(("[%s][%d]non-leaf node\n", __FILE__, (uint16)__LINE__));
            /* non-leaf, store right child ptr and id */
            j = i + 1;
            while ((j < an->maxlength) && (empty_table(an->nptr[j])))
            {
              j++;
            }
            if (j < an->maxlength)
            {
              cur_node.r_ptr = an->nptr[j];
              cur_node.r_id = an->objid[j];
              cur_node.r_nl = 0;
            }
            else
            {
              cur_node.r_ptr = NULL;
            }
            push_node(&cur_node);
            if (an->objid[i] == *ident)
            {
              ident_len--;
              ident++;
            }
            else
            {
              /* an->objid[i] < *ident */
              ident_len = 0;
            }
            /* follow next child pointer */
            node = an->nptr[i];
          }
        }
        else
        {
          /* i == an->maxlength */
          climb_tree = 1;
        }
      }
      else
      {
        u8_t j;
        /* ident_len == 0, complete with leftmost '.thing' */
        j = 0;
        while ((j < an->maxlength) && empty_table(an->nptr[j]))
        {
          j++;
        }
        if (j < an->maxlength)
        {
          DBG_SNMP_PRINTF(("[%s][%d]left an->objid[j]==%"S32_F"\n", \
		  	__FILE__, (uint16)__LINE__,an->objid[j]));
          oidret->id[oidret->len] = an->objid[j];
          (oidret->len)++;
          if (an->nptr[j] == NULL)
          {
            /* leaf node */
            return (struct mib_node*)an;
          }
          else
          {
            /* no leaf, continue */
            node = an->nptr[j];
          }
        }
        else
        {
          /* j == an->maxlength */
          climb_tree = 1;
        }
      }
    }
    else if(node_type == MIB_NODE_LR)
    {
      struct mib_list_rootnode *lrn;
      struct mib_list_node *ln;

      /* list root node (internal 'RAM', variable length) */
      lrn = (struct mib_list_rootnode *)node;
      if (ident_len > 0)
      {
        ln = lrn->head;
        /* iterate over list, head to tail */
        while ((ln != NULL) && (ln->objid < *ident))
        {
          ln = ln->next;
        }
        if (ln != NULL)
        {
          DBG_SNMP_PRINTF(("[%s][%d]ln->objid==%"S32_F" *ident==%"S32_F"\n", \
		  	__FILE__, (uint16)__LINE__,ln->objid,*ident));
          oidret->id[oidret->len] = ln->objid;
          (oidret->len)++;
          if (ln->nptr == NULL)
          {
            /* leaf node */
            if (ln->objid > *ident)
            {
              return (struct mib_node*)lrn;
            }
            else if (ln->next != NULL)
            {
              /* ln->objid == *ident */
              (oidret->len)--;
              oidret->id[oidret->len] = ln->next->objid;
              (oidret->len)++;
              return (struct mib_node*)lrn;
            }
            else
            {
              /* ln->next == NULL */
              (oidret->len)--;
              climb_tree = 1;
            }
          }
          else
          {
            struct mib_list_node *jn;
            struct nse cur_node;

            /* non-leaf, store right child ptr and id */
            jn = ln->next;
            while ((jn != NULL) && empty_table(jn->nptr))
            {
              jn = jn->next;
            }
            if (jn != NULL)
            {
              cur_node.r_ptr = jn->nptr;
              cur_node.r_id = jn->objid;
              cur_node.r_nl = 0;
            }
            else
            {
              cur_node.r_ptr = NULL;
            }
            push_node(&cur_node);
            if (ln->objid == *ident)
            {
              ident_len--;
              ident++;
            }
            else
            {
              /* ln->objid < *ident */
              ident_len = 0;
            }
            /* follow next child pointer */
            node = ln->nptr;
          }

        }
        else
        {
          /* ln == NULL */
          climb_tree = 1;
        }
      }
      else
      {
        struct mib_list_node *jn;
        /* ident_len == 0, complete with leftmost '.thing' */
        jn = lrn->head;
        while ((jn != NULL) && empty_table(jn->nptr))
        {
          jn = jn->next;
        }
        if (jn != NULL)
        {
          DBG_SNMP_PRINTF(("[%s][%d]left jn->objid==%"S32_F"\n", __FILE__, (uint16)__LINE__,jn->objid));
          oidret->id[oidret->len] = jn->objid;
          (oidret->len)++;
          if (jn->nptr == NULL)
          {
            /* leaf node */
            DBG_SNMP_PRINTF(("[%s][%d]jn->nptr == NULL\n", __FILE__, (uint16)__LINE__));
            return (struct mib_node*)lrn;
          }
          else
          {
            /* no leaf, continue */
            node = jn->nptr;
          }
        }
        else
        {
          /* jn == NULL */
          climb_tree = 1;
        }
      }
    }
    else if(node_type == MIB_NODE_EX)
    {
      struct mib_external_node *en;
      s32_t ex_id;

      /* external node (addressing and access via functions) */
      en = (struct mib_external_node *)node;
      if (ident_len > 0)
      {
        u16_t i, len;

        i = 0;
        len = en->level_length(en->addr_inf,ext_level);
        while ((i < len) && (en->ident_cmp(en->addr_inf,ext_level,i,*ident) < 0))
        {
          i++;
        }
        if (i < len)
        {
          /* add identifier to oidret */
          en->get_objid(en->addr_inf,ext_level,i,&ex_id);
          DBG_SNMP_PRINTF(("en->objid[%"U16_F"]==%"S32_F" *ident==%"S32_F"\n",i,ex_id,*ident));
          oidret->id[oidret->len] = ex_id;
          (oidret->len)++;

          if ((ext_level + 1) == en->tree_levels)
          {
            DBG_SNMP_PRINTF(("leaf node\n"));
            /* leaf node */
            if (ex_id > *ident)
            {
              return (struct mib_node*)en;
            }
            else if ((i + 1) < len)
            {
              /* ex_id == *ident */
              en->get_objid(en->addr_inf,ext_level,i + 1,&ex_id);
              (oidret->len)--;
              oidret->id[oidret->len] = ex_id;
              (oidret->len)++;
              return (struct mib_node*)en;
            }
            else
            {
              /* (i + 1) == len */
              (oidret->len)--;
              climb_tree = 1;
            }
          }
          else
          {
            u8_t j;
            struct nse cur_node;

            DBG_SNMP_PRINTF(("non-leaf node\n"));
            /* non-leaf, store right child ptr and id */
            j = i + 1;
            if (j < len)
            {
              /* right node is the current external node */
              cur_node.r_ptr = node;
              en->get_objid(en->addr_inf,ext_level,j,&cur_node.r_id);
              cur_node.r_nl = ext_level + 1;
            }
            else
            {
              cur_node.r_ptr = NULL;
            }
            push_node(&cur_node);
            if (en->ident_cmp(en->addr_inf,ext_level,i,*ident) == 0)
            {
              ident_len--;
              ident++;
            }
            else
            {
              /* external id < *ident */
              ident_len = 0;
            }
            /* proceed to child */
            ext_level++;
          }
        }
        else
        {
          /* i == len (en->level_len()) */
          climb_tree = 1;
        }
      }
      else
      {
        /* ident_len == 0, complete with leftmost '.thing' */
        en->get_objid(en->addr_inf,ext_level,0,&ex_id);
        DBG_SNMP_PRINTF(("left en->objid==%"S32_F"\n",ex_id));
        oidret->id[oidret->len] = ex_id;
        (oidret->len)++;
        if ((ext_level + 1) == en->tree_levels)
        {
          /* leaf node */
          DBG_SNMP_PRINTF(("(ext_level + 1) == en->tree_levels\n"));
          return (struct mib_node*)en;
        }
        else
        {
          /* no leaf, proceed to child */
          ext_level++;
        }
      }
    }
    else if(node_type == MIB_NODE_SC)
    {
      mib_scalar_node *sn;

      /* scalar node  */
      sn = (mib_scalar_node *)node;
      if (ident_len > 0)
      {
        /* at .0 */
        climb_tree = 1;
      }
      else
      {
        /* ident_len == 0, complete object identifier */
        oidret->id[oidret->len] = 0;
        (oidret->len)++;
        /* leaf node */
        DBG_SNMP_PRINTF(("[%s][%d]completed scalar leaf\n", __FILE__, (uint16)__LINE__));
        return (struct mib_node*)sn;
      }
    }
    else
    {
      /* unknown/unhandled node_type */
      DBG_SNMP_PRINTF(("[%s][%d]expand failed node_type %"U16_F" unkown\n", \
        __FILE__, (uint16)__LINE__,(u16_t)node_type));
      return NULL;
    }

    if (climb_tree)
    {
      struct nse child;

      /* find right child ptr */
      child.r_ptr = NULL;
      child.r_id = 0;
      child.r_nl = 0;
      while ((node_stack_cnt > 0) && (child.r_ptr == NULL))
      {
        pop_node(&child);
        /* trim returned oid */
        (oidret->len)--;
      }
      if (child.r_ptr != NULL)
      {
        /* incoming ident is useless beyond this point */
        ident_len = 0;
        oidret->id[oidret->len] = child.r_id;
        oidret->len++;
        node = child.r_ptr;
        ext_level = child.r_nl;
      }
      else
      {
        /* tree ends here ... */
        DBG_SNMP_PRINTF(("expand failed, tree ends here\n"));
        return NULL;
      }
    }
  }
  /* done, found nothing */
  DBG_SNMP_PRINTF(("expand failed node==%p\n",(void*)node));
  return NULL;
}

/**
 * Test object identifier for the iso.org.dod.internet prefix.
 *
 * @param ident_len the length of the supplied object identifier
 * @param ident points to the array of sub identifiers
 * @return 1 if it matches, 0 otherwise
 */
/*
 * Function: snmp_iso_prefix_tst
 * -----------------------------
 * Tests if an OID starts with the ISO/Internet prefix (1.3.6.1).
 * Returns 1 if prefix matches, 0 otherwise.
 * Used for validating SNMP object identifiers.
 */
u8_t
snmp_iso_prefix_tst(u8_t ident_len, s32_t *ident)
{
  if ((ident_len > 3) &&
      (ident[0] == 1) && (ident[1] == 3) &&
      (ident[2] == 6) && (ident[3] == 1))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
 * Expands object identifier to the iso.org.dod.internet
 * prefix for use in getnext operation.
 *
 * @param ident_len the length of the supplied object identifier
 * @param ident points to the array of sub identifiers
 * @param oidret points to returned expanded object identifier
 * @return 1 if it matches, 0 otherwise
 *
 * @note ident_len 0 is allowed, expanding to the first known object id!!
 */
/*
 * Function: snmp_iso_prefix_expand
 * --------------------------------
 * Expands an OID to include the ISO/Internet prefix (1.3.6.1).
 * Used for getnext operations to normalize OID format.
 * Returns 1 on success, 0 if prefix doesn't match.
 */
u8_t
snmp_iso_prefix_expand(u8_t ident_len, s32_t *ident, struct snmp_obj_id *oidret)
{
  const s32_t *prefix_ptr;
  s32_t *ret_ptr;
  u8_t i;


  DBG_SNMP_PRINTF(("[%s][%d]\n", __FILE__, (uint16)__LINE__));
  i = 0;
  prefix_ptr = &prefix[0];
  ret_ptr = &oidret->id[0];
  ident_len = ((ident_len < 4)?ident_len:4);
  while ((i < ident_len) && ((*ident) <= (*prefix_ptr)))
  {
    *ret_ptr++ = *prefix_ptr++;
    ident++;
    i++;
  }
  if (i == ident_len)
  {
    /* match, complete missing bits */
    while (i < 4)
    {
      *ret_ptr++ = *prefix_ptr++;
      i++;
    }
    oidret->len = i;
    return 1;
  }
  else
  {
    /* i != ident_len */
    return 0;
  }
}

#endif /* LWIP_SNMP */