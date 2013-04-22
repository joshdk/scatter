#ifndef _HASHGEN_H_
#define _HASHGEN_H_

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>




typedef struct{
	void * handle;
	int (* info)(size_t *);
	int (* hash)(void *, size_t, void *, size_t *);
} hash_ctx;

int hash_init(hash_ctx * ctx);
int hash_fini(hash_ctx * ctx);
int hash_load(hash_ctx * ctx, const char * path);

#endif
