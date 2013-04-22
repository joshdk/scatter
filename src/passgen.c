#ifndef _PASSGEN_C_
#define _PASSGEN_C_

#include "passgen.h"




/*{{{ Create a pass object */
int pass_init(pass_ctx * ctx, size_t base){
	ctx->index = NULL;
	ctx->index_size = 0;
	ctx->index_length = 0;
	ctx->base = base;
	return 0;
}
/*}}}*/


/*{{{ Destroy a pass object */
int pass_fini(pass_ctx * ctx){
	free(ctx->index);
	ctx->index = NULL;

	// free(ctx->pass);
	// ctx->pass = NULL;

	// free(ctx->chars);
	// ctx->chars = NULL;

	// free(ctx->last);
	// ctx->last = NULL;
	return 0;
}
/*}}}*/


/*{{{ Initialize a pass object from a string */
int pass_load_str(pass_ctx * ctx, const char * start, size_t start_size, const char * chars, size_t chars_size){
	ctx->index_size = 256;
	ctx->index_length = start_size;
	ctx->index = calloc(ctx->index_size, sizeof(size_t));

	for(size_t i=ctx->index_length; i-->0;){
		int index = -1;
		for(size_t j=0; j<start_size; j++){
			if(start[start_size - i - 1] == chars[j]){
				index = j;
				break;
			}
		}

		if(index == -1){
			printf("invalid character: [%c]\n", start[start_size - i - 1]);
			return 1;
		}

		ctx->index[i] = index;
		printf("index[%zu]: %zu\n", i, ctx->index[i]);
	}

	return 0;
}
/*}}}*/


/*{{{ Initialize a pass object from an int */
int pass_load_int(pass_ctx * ctx, size_t step){
	ctx->index_size = 256;
	ctx->index_length = 0;
	ctx->index = calloc(ctx->index_size, sizeof(size_t));

	for(size_t i=0; step > 0; i++){
		// printf("BEFORE | step: %zu\n", step);
		if(i == ctx->index_length){ // we need more space
			ctx->index[i] = 0;
			ctx->index_length += 1;
			step -= 1;
		}
		size_t digit = step % ctx->base;
		ctx->index[i] += digit;
		step -= digit;
		step /= ctx->base;
		size_t carry = ctx->index[i] / ctx->base;
		ctx->index[i] %= ctx->base;
		// step = (step - digit) / base + carry;
		step += carry;
		// printf("AFTER | step: %zu\n", step);
	}

	return 0;
}
/*}}}*/


/*{{{ Increment a pass onject by the specified step */
int pass_step(pass_ctx * ctx, pass_ctx * step){

	int carry = 0;

	for(size_t i=0; i < step->index_length || carry != 0; i++){
		int mod = 1;

		if(ctx->index_length == ctx->index_size){
			ctx->index_size += 1;
			printf("resizing to [%zu]\n", ctx->index_size);
			size_t * tmp = NULL;
			if((tmp = realloc(ctx->index, ctx->index_size * sizeof(size_t))) == NULL){
				printf("Failed to realloc...\n");
				exit(1);
			}
			ctx->index = tmp;
			printf("resized to [%zu]\n", ctx->index_size);
		}

		if(i == ctx->index_length){
			ctx->index_length += 1;
			ctx->index[i] = 0;
			mod -= 1;
		}


		if(i >= step->index_length){

			if(i == step->index_size){
				step->index_size += 1;
				printf("resizing step to [%zu]\n", step->index_size);
				size_t * tmp = NULL;
				if((tmp = realloc(step->index, step->index_size * sizeof(size_t))) == NULL){
					printf("Failed to realloc...\n");
					exit(1);
				}
				step->index = tmp;
				printf("resized step to [%zu]\n", step->index_size);
			}

			assert(i < step->index_size);

			step->index[i] = 0;
			mod -= 1;
		}

		assert(i < step->index_size);

		ctx->index[i] = 
		ctx->index[i] +
		step->index[i] + carry + mod;

		for(carry=0; ctx->index[i] >= ctx->base; carry++){
			ctx->index[i] -= ctx->base;
		}
	}

	return 0;
}
/*}}}*/


/*{{{ Copy a pass object's data into a c string */
int pass_blit(pass_ctx * ctx, const char * chars, char * pass, size_t * pass_length){
	size_t last = ctx->index_length - 1;
	for(size_t i=0; i<ctx->index_length; i++){
		pass[last - i] = chars[ctx->index[i]];
	}
	*pass_length = ctx->index_length;
	return 0;
}
/*}}}*/


/*{{{ Print the state of a pass object to stdout */
void pass_print(pass_ctx * ctx){
	printf("index(%zu) | ", ctx->index_length);
	for(size_t i=0; i<ctx->index_length; i++){
		printf("[%zu]", ctx->index[i]);
	}
	printf("\n");
}
/*}}}*/

#endif
