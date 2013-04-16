#ifndef _PASSGEN_H_
#define _PASSGEN_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>




typedef struct{
	size_t * index;
	size_t   index_size;
	size_t   index_length;
	char   * pass;
	size_t   pass_size;
	size_t   pass_length;
	char   * chars;
	size_t   chars_size;
	char   * last;
	size_t   last_size;
} pass_ctx;


int pass_init(const char * start, size_t start_size, const char * end, size_t end_size, const char * chars, size_t chars_size, pass_ctx * ctx);
int pass_next(pass_ctx * ctx);
int pass_done(pass_ctx * ctx);
int pass_fini(pass_ctx * ctx);

#endif
