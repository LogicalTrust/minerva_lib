/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <minerva_concat.h>

/* 
 * XXX: char is not the best type because we need to track length of the data 
 */

void
minerva_concat(char **r, char *s)
{
    char *prev = *r;

    /* skip in case of null */
    if (s == NULL || *r == NULL) {
        *r = NULL;
        goto out;
    }

    /* XXX: memory leak */
    asprintf(r, "%s%s", *r, s);

out:
    /* XXX: free */
    return;
}
