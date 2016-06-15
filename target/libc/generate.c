#include <sys/types.h>
#include <time.h>
#include <stdlib.h>

static int ctr = 0;

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
        max = rand() % 8192;

    r = calloc(max+1, sizeof(*r));

    if (r == NULL)
        return NULL;

    for (i = 0; i < max; i++)
        r[i] = format[rand() % sizeof(format)];

    r[max] = '\0';

    return 0;
}

char *
strftime_wrapper(struct tm *tm)
{
    size_t max;
    char *fmt;
    char *dst;

    max = 32 + rand() % 4096;
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
}
