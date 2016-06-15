/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void
xfree(void *ptr)
{
    free(ptr);
}

void *
xcalloc(size_t nmemb, size_t size)
{
    void *ret;
    size_t memory;
    
    /* we don't want to allocate 0 memory */
    if (size == 0) {
        fprintf(stderr, "Failure on allocating 0 memory.\n");
        exit(EXIT_FAILURE);
    }

    memory = nmemb*size;
        
    /* integer overflow */
    if (memory/size != nmemb) {
        fprintf(stderr, "Integer overflow on xcalloc.\n");
        exit(EXIT_FAILURE);
    }

    ret = calloc(nmemb, size);
    
    if (ret == NULL) {
        fprintf(stderr, "Failure on calloc.\n");
        exit(EXIT_FAILURE);
    }
    else
        return ret;
}

void *
xmalloc(size_t size)
{
    void *ret;

    if (size == 0) {
        fprintf(stderr, "Failure on allocating 0 memory.\n");
        exit(EXIT_FAILURE);
    }
    
    ret = malloc(size);

    if (ret == NULL) {
        fprintf(stderr, "Failure on malloc.\n");
        exit(EXIT_FAILURE);
    }
    else {
        return ret;
    }
}

/* XXX: nuke that shit? */
void *
_xrealloc(void * ptr, size_t nmemb, size_t size)
{
    void *ret;
    size_t memory;
    
    /* we don't want to allocate 0 memory */
    if (nmemb == 0 || size == 0) {
        fprintf(stderr, "Failure on allocating 0 memory.\n");
        exit(EXIT_FAILURE);
    }
        
    memory = nmemb*size;
        
    /* integer overflow */
    if (memory/size != nmemb) {
        fprintf(stderr, "Integer overflow on realloc.\n");
        exit(EXIT_FAILURE);
    }

    ret = realloc(ptr, memory);
    
    if (ret == NULL) {
        fprintf(stderr, "Failure on realloc.\n");
        exit(EXIT_FAILURE);
    }
    else {
        return ret;
    }
}

char *
xstrdup(const char *str)
{
    char * ret;
    ret = strdup(str);
    if (ret == NULL) {
        fprintf(stderr, "Failure on strdup.\n");
        exit(EXIT_FAILURE);
    } else {
        return ret;
    }
}
