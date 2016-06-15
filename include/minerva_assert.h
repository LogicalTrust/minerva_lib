/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef _MINERVA_ASSERT_H_
#define _MINERVA_ASSERT_H_
#ifdef MINERVA_DEBUG

#include <assert.h>

#define minerva_assert(X) assert(X);

#endif /* ! MINERVA_DEBUG */
#endif /* ! _MINERVA_ASSERT_H_ */
