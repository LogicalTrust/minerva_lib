/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef _XMALLOC_H_
#define _XMALLOC_H_

#include <string.h>
#include <stdlib.h>

void xfree(void *);

void *xcalloc(size_t, size_t);

void *xmalloc(size_t);

void *xrealloc(void *, size_t, size_t);

char *xstrdup(const char *);

#endif /* ! _XMALLOC_H_ */
