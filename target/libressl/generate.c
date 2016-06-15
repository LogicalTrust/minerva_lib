#include <openssl/bn.h>

BIGNUM *
bn_new(void)
{
    BIGNUM *x;
    x = BN_new();
    if (!x)
        return NULL;
    BN_one(x);
    return x;
}
