#pragma STRING (far)

#include <local/config.h>

#include <salportmask.h>
#include <salutil.h>
#include <string.h>
#include <ctype.h>	 
#include <stdio.h>  

#if SAL_ENABLE

#define CUTOFF 8

static void
shortsort(
    int8 *lo,
    int8 *hi,
    uint32 width,
    int32 (*comp)(const void *, const void *));

static void swap(int8 *p, int8 *q, uint32 width);
#if 0
static void
shortsort(
    int8 *lo,
    int8 *hi,
    uint32 width,
    int32 (*comp)(const void *, const void *))
{
    int8 *p, *max;

    while (hi > lo)
    {
        max = lo;

        for (p = lo+width; p <= hi; p += width)
        if (comp(p, max) > 0)
            max = p;

        swap(max, hi, width);
        hi -= width;
    }
}

static void
swap(int8 *a, int8 *b, uint32 width)
{
    int8 tmp;

    if (a != b)
    {
        while (width--)
        {
            tmp = *a;
            *a++ = *b;
            *b++ = tmp;
        }
    }
}

void
qsort(
    void *base,
    uint32 num,
    uint32 width,
    int32 (*comp)(const void *, const void *))
{
    int8 *lo, *hi;
    int8 *mid;
    int8 *loptr, *hiptr;
    uint32 size;
    int8 *lostk[30], *histk[30];
    int32 stkptr;

    if (num < 2 || width == 0)
        return;

    stkptr = 0;

    lo = base;
    hi = (int8 *) base + width * (num - 1);

    recurse:
    size = (hi - lo) / width + 1;

    if (size <= CUTOFF)
    {
        shortsort(lo, hi, width, comp);
    }
    else
    {
        mid = lo + (size / 2) * width;
        swap(mid, lo, width);

        loptr = lo;
        hiptr = hi + width;

        for (;;)
        {
            do { loptr += width; }
                while (loptr <= hi && comp(loptr, lo) <= 0);

            do { hiptr -= width; }
                while (hiptr > lo && comp(hiptr, lo) >= 0);

            if (hiptr < loptr)
                break;

            swap(loptr, hiptr, width);
        }

        swap(lo, hiptr, width);

        if (hiptr - 1 - lo >= hi - loptr)
        {
            if (lo + width < hiptr)
            {
                lostk[stkptr] = lo;
                histk[stkptr] = hiptr - width;
                ++stkptr;
            }

            if (loptr < hi)
            {
                lo = loptr;
                goto recurse;
            }
        }
        else
        {
            if (loptr < hi)
            {
                lostk[stkptr] = loptr;
                histk[stkptr] = hi;
                ++stkptr;
            }

            if (lo + width < hiptr)
            {
                hi = hiptr - width;
                goto recurse;
            }
        }
    }

    --stkptr;

    if (stkptr >= 0)
    {
        lo = lostk[stkptr];
        hi = histk[stkptr];
        goto recurse;
    }
    else
        return;
}

int32
bsearch(
    void *arr,
    uint32 arr_len,
    uint32 size,
    void *x,
    void *y,
    int32 (*cmp)(const void *, const void*, const void*))
{
    int32 lo, high, mid, ret;

    if (arr_len <= 0)
    {
        return -1;
    }

    lo = 0;
    high = arr_len - 1;


    while (lo <= high)
    {
        mid = (lo + high) / 2;

        ret = cmp((arr + (size * mid)), x, y);

        if (ret > 0)
        {
            high = mid - 1;
        }
        else
        if (ret < 0)
        {
            lo = mid + 1;
        }
        else
        {
            return mid;
        }
    }
    return -1;
}
#endif
/*
 * getnext -- get the next token
 *
 * Parameters:
 *   src: pointer to the start of the source string
 *   separater: the symbol used to separate the token
 *   dest: destination of the next token to be placed
 *
 * Returns:
 *   length of token (-1 when failed)
 */
int
getnext (
	char *	src,
	int	separator,
	char *	dest
	)
{
	char *	c;
	int	len = 0;

	if ( (src == NULL) || (dest == NULL) ) {
		return -1;
	}

	c = strchr(src, separator);
	if (c == NULL) {
		strcpy(dest, src);
		return -1;
	}
	len = c - src;
	strncpy(dest, src, len);
	dest[len] = '\0';
	return len + 1;
}

/* Convert MAC address from string to unsigned char array */
unsigned long simple_strtoul(const char *cp,char **endp, unsigned int base)
{
	unsigned long result = 0,value;

	if (*cp == '0') {
		cp++;
		if ((*cp == 'x') && isxdigit(cp[1])) {
			base = 16;
			cp++;
		}
		if (!base) {
			base = 8;
		}
	}
	if (!base) {
		base = 10;
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
	    ? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}

int
str_to_mac (
	unsigned char *	mac,
	char *		str,
	bool padding //for MAC validation
	)
{
	int		len;
	char *		ptr = str;
	char		buf[128], tmp[128];
	int		i;
	char *sec = str; //added for security
	for (i = 0; i < 5; ++i)
	{
		if ((len = getnext(ptr, ':', buf)) == -1 &&
            (len = getnext(ptr, '-', buf)) == -1)
        {
			return -1; /* parse error */
		}

        if (len != 3) return -1;

        strcpy(tmp, "");
        strcpy(tmp, buf);

        if (NULL != strtok(tmp, "0123456789ABCDEFabcdef")) return -1;

		mac[i] = simple_strtoul(buf, NULL, 16);
		ptr += len;
	}

	if(padding)
	{
		if (strlen(ptr) != 4) return -1;
	}
	else
	{
		if (strlen(ptr) != 2) return -1;
	}
	
	strcpy(tmp, "");
    strncpy(tmp, ptr, 2);

    if (NULL != strtok(tmp, "0123456789ABCDEFabcdef")) return -1;

	mac[5] = simple_strtoul(ptr, NULL, 16);

	return 0;
}

/* Convert MAC address from unsigned char array to string */
void
mac_to_str (
	char *		str,
	unsigned char *	mac
	)
{
        unsigned char	c[6];

	c[0] = *(mac);
	c[1] = *(mac+1);
	c[2] = *(mac+2);
	c[3] = *(mac+3);
	c[4] = *(mac+4);
	c[5] = *(mac+5);
        sprintf(str, "%02bX:%02bX:%02bX:%02bX:%02bX:%02bX",
		c[0], c[1], c[2], c[3], c[4], c[5]
	);
}

void
lmask_to_comma (
	char *		comma,
	sal_lPortmask_t *	pstLPortMask
	)
{
	int		first, begin,end;
	int		i;
	char		buf[16];

	if (comma == NULL) {
		return;
	}

	first = 1;
	comma[0] = '\0';


	first = 1;
	begin = -1;
	end = -1;

#if defined(LWS_8G_2F)
    if(pstLPortMask->bits[0] & 1UL << EXT_PORT0){
        pstLPortMask->bits[0] &= ~(1UL << EXT_PORT0);
        pstLPortMask->bits[0] |= 1UL << 8;
    }
    
    if(pstLPortMask->bits[0] & 1UL << EXT_PORT1){
        pstLPortMask->bits[0] &= ~(1UL << EXT_PORT1);
        pstLPortMask->bits[0] |= 1UL << 9;
    }        
#elif defined(LWS_5G_1F)
    if(pstLPortMask->bits[0] & 1UL << EXT_PORT0){
        pstLPortMask->bits[0] &= ~(1UL << EXT_PORT0);
        pstLPortMask->bits[0] |= 1UL << 6;
    }
#else
#endif

#if defined(LWS_5G_1F)
    for (i = 0; i <= SAL_MAX_PHY_PORT; ++i) {
#else
	for (i = 0; i < SAL_MAX_PHY_PORT; ++i) {
#endif	
//		if (i>=SAL_PORT_SKIP_START && i<=PORT_SKIP_END)
//            continue;
		if (SAL_ERR_SUCCESS == sal_lPortMask_chkPort(pstLPortMask, i)) {

			if (first == 1) {
				//printf("%6d    | ", ucMstId); //Instance ID
				first = 0;
			}
#if defined(LWS_5G_1F)
            if(i == 6)
            {
    			if (begin == -1) {
    				begin = end = 5;
    			}
    			else {
    				end = 5;
    			}
    			}
			else
			{
    			if (begin == -1) {
    				begin = end = i;
    			}
    			else {
    				end = i;
    			}			
			}
#else
            if (begin == -1) {
                begin = end = i;
            }
            else {
                end = i;
            }
#endif
		}
		else
		{
			if (first == 0 && (begin != -1) ) {
				first = -1;
			}
			else if (first == -1 && (begin != -1) ) {
				sprintf(buf, ",");
				strcat(comma, buf);
			}

			if ( (begin != -1) && begin == end) {
				sprintf(buf, "%d", begin+1);
				strcat(comma, buf);
			}
			else if ( begin !=-1 ) {
				sprintf(buf, "%d-%d", begin+1, end+1);
				strcat(comma, buf);
			}

			begin = -1;
			end = -1;
		}
	}

	if ( (begin != -1) || (end != -1) ) {
		if (first == -1) {
			sprintf(buf, ",");
			strcat(comma, buf);
		}
		if(begin == end) {
			sprintf(buf, "%d", begin + 1);
			strcat(comma, buf);
		}
		else {
			sprintf(buf, "%d-%d", begin+1, end+1);
			strcat(comma, buf);
		}
	}

	for (i = SAL_MAX_PHY_N_CPU_PORT; i < SAL_MAX_PHY_N_CPU_PORT + SAL_MAX_TRK_PORT; ++i) {

		if (SAL_ERR_SUCCESS == sal_lPortMask_chkPort(pstLPortMask, i)) {

			if (first == 1) {
				first = 0;
				sprintf(buf, "Trunk%d", i - SAL_MAX_PHY_PORT - 1 + 1);

			}
			else {
				sprintf(buf, ",Trunk%d", i - SAL_MAX_PHY_PORT - 1 + 1);
			}
			strcat(comma, buf);
		}

	}
}

int str_to_ip(uint32 *ip, char *str)
{
	int		len;
	char *		ptr = str;
	char		buf[128];
	int		c[4];	/* do not use unsigned char because we need it
				   to do the range checking */
	int		i;

	for (i = 0; i < 3; ++i) {
		if ((len = getnext(ptr, '.', buf)) == -1) {
			return 1; /* parse error */
		}

		c[i] = atoi(buf);
		//c[i] = (int)simple_strtoul(buf, NULL, 10);
		if ((c[i] < 0) || (c[i] > 255)) {
			return 1; /* parse error */
		}

		ptr += len;
	}
	c[3] = atoi(ptr);
	//c[3] = (int)simple_strtoul(ptr, NULL, 10);
	if ((c[3] < 0) || (c[3] > 255)) {
		return 1; /* parse error */
	}

	*ip = ((uint32)c[0]<<24) + ((uint32)c[1]<<16) + ((uint32)c[2]<<8) + c[3];

	return 0;
}

void ip_to_str (
	unsigned char *	str,
	unsigned char* ip
	)
{
	sprintf(str, "%d.%d.%d.%d", (uint16)ip[0], (uint16)ip[1], (uint16)ip[2], (uint16)ip[3]);
}

sal_port_t u32_mask_to_num(uint32 pm)
{
    uint32 i;
    uint8  hit = FALSE;
    sal_port_t  pn;
    

    if( 0==pm )
    {
        return (-1);
    }
    
    for (i=0; i<SAL_PORT_ID_CPU; i++)
    {
        if( FALSE==hit )
        {
            if( pm&0x01 )
            {
                hit = TRUE;
                pn = i;
            }
        }
        else if( (TRUE==hit) && (pm&0x01) )
        {
            return (-1);
        }
        pm = pm>>1;
    }

    return pn;
}

#endif

