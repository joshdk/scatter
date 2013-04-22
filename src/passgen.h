#ifndef _PASSGEN_H_
#define _PASSGEN_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>




typedef struct{
	size_t * index;
	size_t   index_size;
	size_t   index_length;
	size_t   base;
} pass_ctx;


int pass_init(pass_ctx * ctx, size_t base);
int pass_fini(pass_ctx * ctx);
int pass_load_str(pass_ctx * ctx, const char * start, size_t start_size, const char * chars, size_t chars_size);
int pass_load_int(pass_ctx * ctx, size_t step);
int pass_step(pass_ctx * ctx, pass_ctx * step);
int pass_blit(pass_ctx * ctx, const char * chars, char * pass, size_t * pass_length);

void pass_print(pass_ctx * ctx);

#endif
