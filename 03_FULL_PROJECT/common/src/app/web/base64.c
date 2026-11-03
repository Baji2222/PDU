/*
 *  Third Party module: Web Encryption Algorithm
 */
#include <common_types.h>
#include <local/config.h>
#include "base64.h"

#if WEB_ENABLE

#define MAX_INPUT_LENGTH        (16 + 1 + 8 + 1)
#define MAX_OUTPUT_LENGTH       (MAX_INPUT_LENGTH * 2)

/*
 * Translation Table as described in RFC1113
 */
uint8 const far cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static char _base64_output[MAX_OUTPUT_LENGTH];

/*
 * encodeblock()
 *
 * encode 3 8-bit binary bytes as 4 '6-bit' characters
 */
// Encode Block: Converts 3 binary bytes into 4 Base64 characters
// Splits 24 bits into four 6-bit groups and maps to Base64 alphabet
static void encodeblock( unsigned char in[3], unsigned char out[4], int len )
{
    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

// Base64 Encode: Encodes input data as Base64 string
// Processes input bytes in chunks of 3 and returns encoded Base64 output
unsigned char* base64_encode( unsigned char *input, int length )
{
    unsigned char i;

    if (length > MAX_INPUT_LENGTH)
        return NULL;

    for (i=0; (i*3)<length; i++)
    {
        encodeblock(input + (i*3), &_base64_output[i*4], ((length - (i*3)) >= 3)? 3 : (length - (i*3)));
    }
    _base64_output[i*4] = '\0';

    return _base64_output;
}

#endif