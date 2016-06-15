/*
 * Copyright (c) 2010, 2011, 2012 Mateusz Kocielski
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

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/queue.h>
#include <minerva_dict.h>
#include "dict.h"
#include "xmalloc.h"

/* local headers */

/** dictionary */
LIST_HEAD(dict_t, dict_node_t);

/** dictionary linked list */
typedef struct dict_node_t {
    LIST_ENTRY(dict_node_t) nodes;
    char *key; /**< key */
    void *value; /**< value */
} dict_node_t;

static bool valid_key(const char *);
static void list_node_destroy(dict_node_t *);
static dict_node_t *get_node_by_key(const dict_t *,
  const char *);

/**
 * @brief checks if the key is legal
 * @param key node key
 * @return true if key is legal, false otherwise
 */

static bool
valid_key(const char *key)
{
    size_t i;

    /* key is empty string */
    if (key == NULL)
        return false;

    for (i = 0; key[i] != '\0'; i++) {
        if (key[i] != '_' && isalnum((unsigned char)key[i]) == 0)
            return false;
    }

    return true;
}

/**
 * @brief destroys and deallocates list node
 * @param node list node
 */

static void
list_node_destroy(dict_node_t *node)
{
    free(node->key);
    minerva_dict_var_destroy(node->value);
    LIST_REMOVE(node, nodes);
    free(node);
}

/**
 * @brief gets node from the dictionary using key
 * @param dict dictionary
 * @param key node key
 * @return pointer to node if key is in the dictionary, NULL otherwise
 */

static dict_node_t *
get_node_by_key(const dict_t *dict, const char *key)
{
    dict_node_t *node;

        LIST_FOREACH(node, dict, nodes) {
        if (strcmp(node->key, key) == 0)
            return node;
    }

    return NULL;
}

/**
 * @brief destroys and deallocates the dictionary
 * @param dict dictionary
 */

void
dict_destroy(dict_t *dict)
{
    while(!LIST_EMPTY(dict))
        list_node_destroy(LIST_FIRST(dict));

    free(dict);
}

/**
 * @brief removes node from the dictionary using key
 * @param dict dictionary
 * @param key node key
 * @return DICT_OK on success, DICT_KEYNOTFOUND if node was not found (key
 * does not exist in the dictionary.
 */

dict_result_t
dict_remove(dict_t *dict, const char *key)
{
    dict_node_t *node;

    LIST_FOREACH(node, dict, nodes) {
        if (strcmp(node->key, key) == 0) {
            list_node_destroy(node);
            return DICT_OK;
        }
    }

    return DICT_KEYNOTFOUND;
}

/**
 * @brief gets node value from the dictionary using key
 * @param dict dictionary
 * @param key node key
 * @return pointer to the value if key was found in the dictionary, NULL
 * otherwise.
 */

void *
dict_get(const dict_t *dict, const char *key)
{
    dict_node_t *node = get_node_by_key(dict, key);

    return (node != NULL) ? node->value : NULL;
}

/**
 * @brief creates and allocates dictionary
 * @return pointer to new dictionary, NULL is returned on allocation failure
 */

dict_t *
dict_create(void)
{
    dict_t *head;

    head = xcalloc(1, sizeof(dict_t));

    LIST_INIT(head);

    return head;
}

/**
 * @brief inserts node into the dictionary
 * @param dict dictionary
 * @param key node key
 * @param val node value
 * @return
 * DICT_OK - on success,
 * DICT_KEYINVALID - if node key is illegal,
 * DICT_VALBAD - if node value is illegal,
 * DICT_KEYEXISTS - if node with the same key already exists in the
 * dictionary,
 */

dict_result_t
dict_insert(dict_t *dict, const char *key, void *val)
{
    char *d_key = NULL;
    dict_node_t *node;

    if (valid_key(key) == false) 
        return DICT_KEYINVALID;

    if (val == NULL)
        return DICT_VALBAD;

    /* XXXSHM: leak you, leak me */
    /* check if key exists in dictionary */
    node = get_node_by_key(dict, key);
    if (node != NULL)
        dict_remove(dict, key);
    d_key = xstrdup(key);
    node = xcalloc(1, sizeof(*node));

    LIST_INSERT_HEAD(dict, node, nodes);
    node->key = d_key;
    node->value = val;

    return DICT_OK;
}
