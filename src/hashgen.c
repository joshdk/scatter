#ifndef _HASHGEN_C_
#define _HASHGEN_C_

#include "hashgen.h"




/*{{{ Create hash context */
int hash_init(hash_ctx * ctx){
	ctx->handle = NULL;
	ctx->info = NULL;
	ctx->hash = NULL;
	return 0;
}
/*}}}*/


/*{{{ Destroy hash context */
int hash_fini(hash_ctx * ctx){
	ctx->info = NULL;
	ctx->hash = NULL;

	if(ctx->handle == NULL){
		return 0;
	}

	if(dlclose(ctx->handle) != 0){
		return 1;
	}

	return 0;
}
/*}}}*/


/*{{{ Load hashing functions from shared library */
int hash_load(hash_ctx * ctx, const char * path){
	if((ctx->handle = dlopen(path, RTLD_NOW | RTLD_GLOBAL)) == NULL){
		fprintf(stderr, "error: Unable to open library `%s'.\n", path);
		return 1;
	}

	if((ctx->info = dlsym(ctx->handle, "info")) == NULL){
		fprintf(stderr, "error: Unable to load symbol`%s'.\n", "info");
		return 1;
	}

	if((ctx->hash = dlsym(ctx->handle, "hash")) == NULL){
		fprintf(stderr, "error: Unable to load symbol`%s'.\n", "hash");
		return 1;
	}

	return 0;
}
/*}}}*/

#endif
