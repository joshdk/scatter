#ifndef _SHA256_H_
#define _SHA256_H_

#include <openssl/evp.h>




int info(size_t * size);
int hash(void * in_buf, size_t in_buf_size, void * out_buf, size_t * out_buf_size);

#endif
