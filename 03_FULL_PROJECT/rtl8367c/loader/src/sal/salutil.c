#include <salutil.h>

#include <lib/lib_net.h>
#include <common_types.h>

#define UINT8_MAX         255
#define IP_STR_LEN_MAX    (16)

#if defined(UPGRADE_TFTP)
/* This is simplified inet_aton. This function convert string
   "x.x.x.x" to a uint8 array with 4 element.
   This function return error if input out of range, or "." less than 3
 */
int16 str_to_ip(uint32 *pIp, char *ipStr)
{
    uint8  i, arrPtr =0;
    uint32 tmp = 0;

    *pIp = 0;

    for(i = 0; i < IP_STR_LEN_MAX; i++)
    {
        if ( ipStr[i] >= '0' && ipStr[i] <= '9' )
        {
            tmp = tmp * 10 + ipStr[i] - '0';
            
            if (tmp > UINT8_MAX)
                return FAILED;
        }
        else if (ipStr[i] == '.' || ipStr[i] == 0)
        {
            *pIp = ((*pIp) << 8) | tmp;
            tmp = 0;

            if ( ipStr[i] == 0 )
                break;
        }
        else
        {
            return FAILED;
        }
    }

    return SUCCESS;
}
#endif
