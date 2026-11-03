#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "common_types.h"

#define MAX_BANK_NUMBER (16)
#define KEIL_BANK_SIZE  (64*1024UL)
#define BANK_SIZE       (48*1024UL)
#define IROM_SIZE	    (16*1024UL)
#define SEC_SIZE        (32768)
#define IROM_size       (16*1024)
#define BANK_size       (48*1024)

#define LOADER_IROM_SIZE  (4*1024UL)
#define RUNTIME_IROM_SIZE (IROM_SIZE-LOADER_IROM_SIZE)
#define LOADER_BANK_NUM   (2UL)
#define LOADER_BANK_SIZE  (LOADER_BANK_NUM*BANK_SIZE)

#define RUNTIME_HEADER_BANK    (0x03)
#define RUNTIME_HEADER_ADDRESS (0x035000)
#define RUNTIME_MAGIC_NUMBER   (0x12345678UL)

#define WARM_START_MAGIC_NUMBER (0x10101010UL)

/* 20 bytes, must 4 byte alignment */
typedef struct runTimeImageHeader_s{
	u32_t magic_number;
	u32_t length;
	u32_t header_chksum;
	u32_t payload_chksum;	
	u32_t reserved;
}/*__attribute__((packed))*/ runTimeImageHeader_t;


#endif  /* end of __IMAGE_HEADER_H__ */



