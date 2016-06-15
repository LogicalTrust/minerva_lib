/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef _MINERVA_REFS_H_
#define _MINERVA_REFS_H_

#include <signal.h>

typedef sig_atomic_t minerva_refs_t;

#define MINERVA_REFS_INC(x) \
    do {(x)->refs++;} while(0)

#define MINERVA_REFS_DEC(x) \
    do {(x)->refs--;} while(0)

#define MINERVA_REFS(x) ((x)->refs)

#endif
