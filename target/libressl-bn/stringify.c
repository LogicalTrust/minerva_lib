#include <stdio.h>
#include <stdlib.h>
#include <openssl/bn.h>

char *stringify_bn(void *x) {
    return BN_bn2dec(x);
}
