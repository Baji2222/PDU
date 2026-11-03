
#ifndef __SAL_UTIL_H__
#define __SAL_UTIL_H__

#include <common_types.h>
#include <local/web_config.h>
#include <salportmask.h>
#include <stdlib.h>

void
qsort(
    void *base,
    uint32 num,
    uint32 width,
    int32 (*comp)(const void *, const void *));

int32
bsearch(
    void *arr,
    uint32 arr_len,
    uint32 size,
    void *x,
    void *y,
    int32 (*cmp)(const void *, const void*, const void*));

int str_to_mac(unsigned char *mac, char *str, bool padding);
void mac_to_str(char *str, unsigned char *mac);
int  str_to_ip(uint32 *ip, char *str);
void ip_to_str(unsigned char *str, unsigned char* ip);
void lmask_to_comma(char *comma, sal_lPortmask_t *pstLPortMask);
sal_port_t u32_mask_to_num(uint32 pm);
   
#endif/*__SAL_UTIL_H__*/

