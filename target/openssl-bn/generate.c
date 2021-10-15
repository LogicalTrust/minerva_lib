#include <openssl/bn.h>

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

void __sanitizer_cov_trace_pc(void) {
//    printf("HERE: %p\n", __builtin_return_address(0));
}
