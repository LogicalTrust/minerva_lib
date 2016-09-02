#include <sys/types.h>
#include <time.h>
#include <wctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int ctr = 0;

void* char_ptr_to_void_ptr(char* a)
{
    return (void*) a;
}
time_t *
generate_time_t(void)
{
    return calloc(1, sizeof(time_t));
    //return (ctr++)%2 == 0 ? calloc(1, sizeof(time_t)) : NULL;
}

char format[] = "%qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM_-0987654321^#";

static char *
format_string(size_t max)
{
    char *r;
    size_t i;

    if (max == 0)
        max = rand() % 0x2000;

    r = calloc(max+1, sizeof(*r));

    if (r == NULL)
        return NULL;

    for (i = 0; i < max; i++)
        r[i] = format[rand() % sizeof(format)];

    r[max] = '\0';

    return 0;
}
wint_t generate_wint_t()
{
    return rand()%0x100;
}
/*
char *
strftime_wrapper(struct tm *tm)
{
    size_t max;
    char *fmt;
    char *dst;

    max = 32 + rand() % 0x1000;
    dst = calloc(max, sizeof(*dst));
    if (dst == NULL)
        goto out;
    fmt = format_string(0);
    if (fmt == NULL)
        goto out;
    if (strftime(dst, max, fmt, tm) == 0) {
        free(dst);
        dst = NULL;
    }
out:
    free(fmt);
    return dst;
}*/