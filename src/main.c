#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "passgen.h"
#include "hashgen.h"




void print_hex(void * data, size_t data_size){
	for(size_t i=0; i<data_size; i++){
		printf("%02x", ((unsigned char *)data)[i]);
	}
	printf("\n");
}


int main(int argc, char **argv){

	if(argc < 2){
		fprintf(stderr, "scatter: error: Insufficient parameters.\n");
		exit(1);
	}

	char * module = NULL;
	module = argv[1];

	size_t iterations = 100;

	if(argc >= 3){
		iterations = atoi(argv[2]);
	}

	hash_ctx hctx;
	hash_init(&hctx);

	if(hash_load(&hctx, module) != 0){
		fprintf(stderr, "scatter: error: Failed to load module `%s'\n", module);
		exit(1);
	}

	size_t pass_size = 256;
	size_t pass_length = 0;
	char * pass = calloc(pass_size, sizeof(char));

	size_t hash_size;
	hctx.info(&hash_size);
	char * hash = calloc(hash_size, sizeof(char));

	pass_ctx pctx;
	pass_init(&pctx, 3);
	pass_load_int(&pctx, 0);

	pass_ctx pstp;
	pass_init(&pstp, 3);
	pass_load_int(&pstp, 2);

	for(size_t n=0; n<iterations; n++){
		pass_blit(&pctx, "abc", pass, &pass_length);
		// printf("pass: %s\n", pass);
		hctx.hash(pass, pass_length, hash, &hash_size);
		// print_hex(hash, hash_size);
		pass_step(&pctx, &pstp);
	}

	pass_fini(&pctx);
	pass_fini(&pstp);

	free(pass);
	free(hash);

	hash_fini(&hctx);

	return 0;
}
