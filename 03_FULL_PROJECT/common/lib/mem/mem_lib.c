/*
* Copyright c                  Realtek Semiconductor Corporation, 2008  
* All rights reserved.
* 
*/


#include <local/config.h>
#include <mem/mem_lib.h>
#include <common.h>

#include <stdio.h>

extern uint8     xdata p_tx_buffer[];

#if defined(STANDARD_MEM_LIB)
#define MEMLIB_MEMCPY  memcpy
#define MEMLIB_MEMSET  memset
#define MEMLIB_MEMCMP  memcmp
#define MEMLIB_STRCMP  strcmp
#else
#define MEMLIB_MEMCPY  nostarnard_memcpy
#define MEMLIB_MEMSET  nostarnard_memset
#define MEMLIB_MEMCMP  nostarnard_memcmp
#define MEMLIB_STRCMP  nostarnard_strcmp
#if 0
// Memory Copy: Copies n bytes from source to destination using byte-by-byte loop
// Provides custom implementation when standard memory library is not available
void* nostarnard_memcpy(void *dest, const void *src, u16_t n)
{
    u16_t i;
    u8_t  *d, *s;
    d = (u8_t*)dest;
    s = (u8_t*)src;

    for(i=0;i<n;i++)
    {
        d[i] = s[i];
    }
    
    return dest;
}
#endif
#if !defined(LWS_LOADER)
// Memory Compare: Compares n bytes of two memory regions and returns difference
// Returns 1 if s1>s2, -1 if s1<s2, 0 if equal (custom implementation)
int nostarnard_memcmp(const void *s1, const void *s2, u16_t n)
{
    u16_t i;
    u8_t  *d, *s;
    d = (u8_t*)s1;
    s = (u8_t*)s2;

    for(i=0;i<n;i++)
    {
        if( d[i]>s[i] )
        {
            return 1;
        }
        else if( d[i]<s[i] )
        {
            return -1;
        }
    }
    
    return 0;
}
#endif
// Memory Set: Fills n bytes of memory with a constant byte value using loop
// Returns pointer to memory area after filling with specified byte
int nostarnard_memset(void *s, u8_t c, u16_t n)
{
    u16_t i;
    u8_t  *d;
    d = (u8_t*)s;

    for(i=0;i<n;i++)
    {
        d[i] = c;
    }
    
    return s;
}

// String Compare: Compares two null-terminated strings character by character
// Returns difference at first non-matching character, 0 if strings are equal
int nostarnard_strcmp(const char *str1, const char *str2)
{
    while(1)
	{
	    if(*str1 == 0 && *str2 == 0)
		    return 0;
        else if(*str1 != *str2)
		    return *str1 - *str2;
		else
		{
		    str1++;
			str2++;
		}
	}
}

#endif /* end of defined(STANDARD_MEM_LIB) */

u8_t xdata ram_heap[MEM_SIZE];

// Memory Initialization: Initializes heap memory and sets up memory pool for dynamic allocation
// Allocates and frees test block to include malloc segment in binary
void mem_init(void)
{
    uint8 *ptr;

    MEMLIB_MEMSET(ram_heap,0x0,MEM_SIZE);
    init_mempool((void xdata *) &ram_heap[0], MEM_SIZE);

    /* trigger malloc call to add malloc segment into binary */
    ptr = malloc (32);
    free(ptr);
}
#if !defined(LWS_LOADER)
// Allocate TX Buffer: Returns pointer to transmit buffer if requested length is within limits
// Returns NULL if requested length exceeds maximum buffer size
u8_t* malloc_tx_buf(u32_t len){
	
	if( len>P_BUFFER_SIZE ){
		return NULL;
	}else{
		return p_tx_buffer;
	}
}
#endif