#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include <string.h>
#include <fetch.h>
#include "exec.h"

char *
get_host(URI_t *x) {
    struct url *u = fetchParseURL(x);
    char *r = NULL;
    
    if (u == NULL || strlen(u->host) == 0)
        goto cleanup;

    r = strdup(u->host);

cleanup:
    free(u);
	return r;
}
