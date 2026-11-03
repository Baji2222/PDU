#ifndef __COMMON_TYPES_H_
#define __COMMON_TYPES_H_

typedef unsigned long uint32;
typedef unsigned int uint16;
typedef unsigned char uint8;

typedef struct uint64_s{
    uint32 value[2];
}uint64;


typedef long int32;
typedef int int16;
typedef char int8;

#define REG8(x)    (*((volatile uint8 xdata *)(x)))
#define REG16(x)   (*((volatile uint16 xdata *)(x)))
#define REG32(x)   (*((volatile uint32 xdata *)(x)))


/* for LWPS */
typedef unsigned long Tuint32;
typedef unsigned int Tuint16;
typedef unsigned char Tuint8;

typedef long Tint32;
typedef int Tint16;
typedef char Tint8;
typedef char Tchar;
typedef char Tbool;
typedef char Tbyte;

/* for IGMP */
typedef uint32  portmask_type;


/*for SNMP*/
typedef unsigned long u32_t;
typedef unsigned int u16_t;
typedef unsigned char u8_t;

typedef long s32_t;
typedef int s16_t;
typedef char s8_t;

#define U16_F "u"
#define U32_F "lu"
#define S16_F "d"
#define S32_F "ld"
#define X8_F "bX"
#define X16_F "X"
#define X32_F "lX"

/* network address */
#define IP_ADDR_LEN        (4)
typedef struct ip_addr_s {
    uint8 addr[IP_ADDR_LEN];
} ip_addr_t;

#ifndef NULL
#define NULL  ((void *) 0)
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef SUCCESS
#define SUCCESS 	0
#endif
#ifndef FAILED
#define FAILED -1
#endif

#ifndef ENABLE
#define ENABLE 1
#endif

#ifndef DISABLE
#define DISABLE 0
#endif

#ifndef bool
#define bool uint8
#endif

#endif /* end of __COMMON_TYPES_H_ */

