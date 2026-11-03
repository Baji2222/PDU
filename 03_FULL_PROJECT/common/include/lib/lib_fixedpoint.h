#ifndef __LIB_FIXEDPOINT_H__
#define __LIB_FIXEDPOINT_H__

#include <local/config.h>
#include <common.h>

/* Configurations */
#define FIXEDP_INT_TYPE   u32_t
#define FIXEDP_FRAC_TYPE  u32_t


#define FIXEDP_INT_MAX    ( 2147483647L)
#define FIXEDP_INT_MIN    (-2147483648L)
#define FIXEDP_FRAC_MAX   ( 4294967295UL)
#define FIXEDP_FRAC_MIN   ( 0UL)

typedef enum FIXEDP_e
{
    FIXEDP_OK = 0,
    FIXEDP_FAIL,
    FIXEDP_INT_OVER,
    FIXEDP_NEGATIVE,
    FIXEDP_END
}FIXEDP_t;

typedef struct fixed_point_s{
    FIXEDP_INT_TYPE integer;
    FIXEDP_FRAC_TYPE fraction;
    /* Do not use functor in 8051 */
}fixed_point_t;

extern FIXEDP_t add_fixedp(fixed_point_t *d, fixed_point_t *s);

/*
FIXEDP_INT_TYPE get_fixedp_int(){}
FIXEDP_INT_TYPE set_fixedp_int(){}
*/

#endif /* end of __LIB_FIXEDPOINT_H__ */

