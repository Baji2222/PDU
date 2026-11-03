#include <snmp/mib_glue.h>
#include <salerror.h>
#include <salmib.h>
#include <local/sal_config.h>
#include <stdio.h>
#define CNTR_TO_ARR(counter, arr) do { \
        *(arr + 0) = (counter >> 24) & 0xFF; \
        *(arr + 1) = (counter >> 16) & 0xFF; \
        *(arr + 2) = (counter >> 8)  & 0xFF; \
		*(arr + 3) =  counter        & 0xFF; \
    } while(0)

/*
counter is 32 bit:
    *ptr : MSB
    *(ptr + 3): LSB
counter is 64 bit:
    *ptr : MSB
    *(ptr + 7): LSB
*/

/*
 * Function: mib_glue_portCounter_get
 * ----------------------------------
 * Retrieves MIB counter values for a specified port and places into byte array.
 * Handles both 32-bit and 64-bit counters (octets uses 64-bit, others use 32-bit).
 * Converts counter to big-endian byte order for SNMP response.
 * Returns SUCCESS on success, FAILED on error.
 */
int32 mib_glue_portCounter_get(sal_stat_port_type_t mibIndex, u8_t interface, u8_t size, u8_t *ptr)
{
    uint8  i;
    uint32 cntrH, cntrL;

    if(interface > SAL_MAX_PHY_PORT)
        return FAILED;

    for(i = 0; i < size;i++)
    ptr[i] = 0;

    if(SAL_ERR_SUCCESS == sal_mib_port_get(interface, mibIndex, &cntrH, &cntrL))
    {
        /* if counter is 64 bit */
         if(mibIndex == SAL_IF_IN_OCTETS_INDEX || 
            mibIndex == SAL_IF_OUT_OCTETS_INDEX ||
            mibIndex == SAL_ETHER_STATS_OCTETS_INDEX) 
           {
              CNTR_TO_ARR(cntrH, ptr);
              CNTR_TO_ARR(cntrL, ptr + 4);
           }
           else
            {
                CNTR_TO_ARR(cntrL, ptr);
                CNTR_TO_ARR(cntrL, ptr + 4);    
            }
		/* write cntrL to higher 32 bit and lower 32 bit */
        return SUCCESS;
    }
    else
        return FAILED;
}