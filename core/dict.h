/* $Id: dict.h,v 1.13 2011/04/03 15:09:33 mkocielski Exp $ */

/*
 * Copyright (c) 2010, 2011 Mateusz Kocielski
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _DICT_H_
#define _DICT_H_
#include <stddef.h>
#include <minerva_dict.h>

typedef enum {
    DICT_OK = 0,
    DICT_NOMEM,
    DICT_KEYNOTFOUND,
    DICT_KEYEXISTS,
    DICT_KEYINVALID,
    DICT_VALBAD
} dict_result_t;

/* dictionary type */
/* in minerva_dict.h */

/* interface */
dict_t *dict_create(void);
dict_result_t dict_insert(dict_t *, const char *,
    void *);
void *dict_get(const dict_t *, const char *);
dict_result_t dict_remove(dict_t *, const char *);
void dict_destroy(dict_t *);

#endif /* ! _DICT_H_ */
