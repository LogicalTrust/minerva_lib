/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdlib.h>
#include <minerva_generic.h>

/* generic check functions */

int
generic_not_null(const void *ptr)
{
    void *p = (void *)ptr;
    return (p != NULL) ? 1 : 0;
}

int
generic_not_zero(const void *ptr)
{
    int *p = (int *)ptr;

    return (p != 0) ? 1 : 0;
}

int
generic_zero(const void *ptr)
{
    int *p = (int *)ptr;

    return p == 0;
}

int
generic_void(const void *ptr)
{
    return 0;
}

int
generic_success(const void *ptr)
{
    return 1;
}

/* MUTATORS */

int
mutate_int_xor(int a, int b)
{
    return a^b;
}

int
mutate_int_or(int a, int b)
{
    return a|b;
}

int
mutate_int_and(int a, int b)
{
    return a&b;
}

/* God, have mercy on me */

#define MUTATE_TYPE_BITFLIP(type) \
    type \
    mutate_##type##_bitflip(type x) \
    { \
        type shift = rand() % (sizeof(type)*8); \
        return x ^ (1<<shift); \
    }

MUTATE_TYPE_BITFLIP(int)
MUTATE_TYPE_BITFLIP(char)
MUTATE_TYPE_BITFLIP(long)

/* CASTS */
/* God, have mercy on him */
#define CAST_TYPE(from_type,to_type) \
    to_type from_type##_to_##to_type(from_type x) \
    { \
        return (to_type) x; \
    }
CAST_TYPE(int,char)
CAST_TYPE(char,int)
CAST_TYPE(long, int)
CAST_TYPE(int, long)
CAST_TYPE(float,double)
CAST_TYPE(double,float)
CAST_TYPE(int,float)
CAST_TYPE(float,int)