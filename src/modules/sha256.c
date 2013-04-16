#ifndef _SHA256_C_
#define _SHA256_C_

#include "sha256.h"




static EVP_MD_CTX context;
static const EVP_MD *digest;


static void __attribute__ ((constructor)) __init(void){
	if((digest = EVP_sha256()) == NULL) {
		return;
	}

	EVP_MD_CTX_init(&context);
}


static void __attribute__ ((destructor)) __fini(void){
	EVP_MD_CTX_cleanup(&context);
}


int info(size_t * size){
	*size = 32;
	return 0;
}


int hash(void * in_buf, size_t in_buf_size, void * out_buf, size_t * out_buf_size){
	if(EVP_DigestInit_ex(&context, digest, NULL) == 0){
		return 1;
	}

	if(EVP_DigestUpdate(&context, in_buf, in_buf_size) == 0){
		return 1;
	}

	unsigned int size;

	if(EVP_DigestFinal_ex(&context, out_buf, &size) == 0){
		return 1;
	}

	*out_buf_size = size;

	return 0;
}

#endif
