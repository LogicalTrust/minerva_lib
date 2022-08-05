#include <openssl/bn.h>
#include <random.h>

#define GEN_RAND_MAX (12)

BIGNUM *
bn_new(void)
{
    BIGNUM *x;
    x = BN_new();
    if (!x)
        return NULL;
    //BN_rand(x, 64, 0, 0);
    BN_one(x);
    return x;
}

/*
 * bn_gen_rand - generates random BN, it uses internal minerva rand
 * implementation which ensures it is determnistic
 */

BIGNUM *
bn_gen_rand(void)
{
    int i, len;
    uint8_t x[GEN_RAND_MAX];

    len = (rand() % (GEN_RAND_MAX-1)) + 1;

    for (i = 0; i < len; i++) {
        x[i] = rand();
    }

    return BN_bin2bn(x, len, NULL);
}

BN_ULONG
bn_ulong_new(void)
{
    BN_ULONG r;
    uint8_t *x = (uint8_t *)&r;
    int i;

    for (i = 0; i < sizeof(r); i++)
        *(x++) = rand();

    return r;
}

void __sanitizer_cov_trace_pc(void) {
//    printf("HERE: %p\n", __builtin_return_address(0));
}
