#include <stdio.h>
#include <unistd.h>
#include "rump-syscalls.h"

int *
get_int()
{
    int *x = malloc(sizeof(int));

    if (x == NULL)
        return NULL;

    *x = rand();

    return x;
}

int
filter_write(int a, char *b, int c) {
    if (a <= 2)
        return 0;
    else
        return rump___sysimpl_write(a, b, c);
         
}

int
filter_read(int a, char *b, int c) {
    if (a <= 2)
        return 0;
    else
        return rump___sysimpl_read(a, b, c);
         
}

char *
empty(int x)
{
    char *r = malloc(x);
    if (r == NULL)
        return NULL;
    memset(r, 0, x);
    return r;
}

char *
randstr(int x)
{
    char *r = malloc(x);
    int i;

    if (r == NULL)
        return NULL;

    for (i = 0 ; i < x; i++)
        r[i] = rand() % 256;

    return r;
}

int
zero(void)
{
    return 0;
}

int
add_one(int x)
{
    return x+1;
}
