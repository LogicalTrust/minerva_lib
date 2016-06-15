/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef _MINERVA_GENERIC_H_
#define _MINERVA_GENERIC_H_

#include <minerva_func.h>

/** checks if result is not null */
minerva_check_t generic_not_null;

/** checks if result is not zero */
minerva_check_t generic_not_zero;

/** checks if result is zero */
minerva_check_t generic_zero;

/** void function return check - always failure */
minerva_check_t generic_void;

/** always success */
minerva_check_t generic_success;

/** mutators */

int mutate_int_xor(int, int);
int mutate_int_or(int, int);
int mutate_int_and(int, int );

/* God, have mercy on me */
#define MUTATE_TYPE_BITFLIP_PROTO(type) type mutate_##type##_bitflip(type)
MUTATE_TYPE_BITFLIP_PROTO(int);
MUTATE_TYPE_BITFLIP_PROTO(char);

#endif /* ! _MINERVA_GENERIC_H_ */
