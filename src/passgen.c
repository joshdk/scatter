#ifndef _PASSGEN_C_
#define _PASSGEN_C_

#include "passgen.h"




int pass_init(const char * start, size_t start_size, const char * end, size_t end_size, const char * chars, size_t chars_size, pass_ctx * ctx){
	ctx->chars_size = chars_size;
	ctx->chars = calloc(ctx->chars_size, sizeof(char));
	memcpy(ctx->chars, chars, ctx->chars_size);

	ctx->pass_size = end_size + 1;
	ctx->pass_length = start_size;
	ctx->pass = calloc(ctx->pass_size, sizeof(char));
	memcpy(ctx->pass, start, ctx->pass_length);

	ctx->index_size = end_size + 1;
	ctx->index_length = start_size;
	ctx->index = calloc(ctx->index_size, sizeof(size_t));
	for(size_t i=0; i<ctx->pass_length; i++){
		int index = -1;
		for(size_t j=0; j<ctx->chars_size; j++){
			printf("trying: %c == %c\n", ctx->pass[i], ctx->chars[j]);
			if(ctx->pass[i] == ctx->chars[j]){
				index = j;
				break;
			}
		}

		if(index == -1){
			printf("invalid character: [%c]\n", ctx->pass[i]);
			return 1;
			// fail
		}

		ctx->index[i] = index;
		printf("index[%d]: %d\n", i, ctx->index[i]);
	}

	ctx->last_size = end_size;
	ctx->last = calloc(ctx->last_size, sizeof(char));
	memcpy(ctx->last, end, ctx->last_size);

	return 0;
}


int pass_next(pass_ctx * ctx){

	for(int i=ctx->index_length; i-->0;){
		ctx->index[i] += 1;
		if(ctx->index[i] != ctx->chars_size){
			ctx->pass[i] = ctx->chars[ctx->index[i]];
			return 0;
		}
		ctx->index[i] = 0;
		ctx->pass[i] = ctx->chars[0];
	}

	ctx->index[ctx->index_length] = 0;
	ctx->index_length += 1;
	ctx->pass[ctx->pass_length] = ctx->chars[0];
	ctx->pass_length += 1;

	return 0;
}


int pass_done(pass_ctx * ctx){
	if(ctx->index_length == ctx->last_size){
		if(memcmp(ctx->pass, ctx->last, ctx->last_size) == 0){
			return 1;
		}
	}

	return 0;
}


int pass_fini(pass_ctx * ctx){
	free(ctx->index);
	ctx->index = NULL;

	free(ctx->pass);
	ctx->pass = NULL;

	free(ctx->chars);
	ctx->chars = NULL;

	free(ctx->last);
	ctx->last = NULL;
	return 0;
}


#endif
