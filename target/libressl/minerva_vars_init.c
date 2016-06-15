#include <stdio.h>
#include <stdlib.h>
#include <openssl/bn.h>
#include <openssl/bio.h>

#include <xmalloc.h>
#include <target.h>
#include <minerva_var.h>

unsigned short USE_ONCE = 0;


BN_ULONG *minerva_BN_ULONG_new(void)
{
    BIGNUM a;
    BN_ULONG v, *ret;

    BN_init(&a);
    do {
        BN_rand(&a, 512, 0, 0);
        v = a.d[0];
    } while (!v);

    ret = &v;

    return (ret);
}

BIO *minerva_BIO_new(void)
{
    BIO *ret = NULL;

    ret = BIO_new(BIO_s_file());

    if (ret == NULL)
        fprintf(stderr, "minerva_BIO_new: BIO_new() call failed.\n");

    if(!BIO_set_fp(ret, stdout, BIO_NOCLOSE))
        fprintf(stderr, "minerva_BIO_new: BIO_set_fp() call failed.\n");

    return (ret);
}

FILE *minerva_FILE_new(void)
{
    FILE *ret = NULL;

    /*  XXXsn: this must be definietly solved
     *  in a different way, e.g. check if the
     *  LIST has an element already assigned.
     */
    if (USE_ONCE == 0)
        USE_ONCE = 1;
    else
        return (NULL);

    ret = fopen("/dev/urandom", "r");

    if (ret == NULL)
        fprintf(stderr, "minerva_FILE_new: fopen() call failed.\n");

    return (ret);
}
