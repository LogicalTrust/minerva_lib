#ifndef _MINERVA_VARS_INIT_H_
#define _MINERVA_VARS_INIT_H_

#include <openssl/bn.h>
#include <openssl/bio.h>

BN_ULONG minerva_BN_ULONG_new(void);
BIO *minerva_BIO_new(void);
FILE *minerva_FILE_new(void);

#endif /* ! _MINERVA_VARS_INIT_H_ */
