#include <stdio.h>
#include <curl/curl.h>
#include "exec.h"

char *
get_host(URI_t *x) {
    char *r = NULL;
    CURLU *h = curl_url();

    if (curl_url_set(h, CURLUPART_URL, x, 0) != CURLUE_OK)
        goto cleanup;

    curl_url_get(h, CURLUPART_HOST, &r, 0);

cleanup:
    curl_url_cleanup(h);
    return r;
}
