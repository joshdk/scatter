#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <ctype.h>
#include <mpi.h>
#include "passgen.h"
#include "hashgen.h"
#include "parse.h"
#include "io.h"




void print_hex(void * data, size_t data_size){
	for(size_t i=0; i<data_size; i++){
		printf("%02x", ((unsigned char *)data)[i]);
	}
	printf("\n");
}


/*{{{ Convert raw buffer to hex string */
int buf_to_hex(char ** strp, const char * data, size_t data_length){
	size_t buffer_size = data_length * 2 + 1;
	size_t buffer_length = 0;
	char * buffer = malloc(sizeof(*buffer) * buffer_size);

	const char charset[] = "0123456789ABCDEF";

	for(size_t i=0; i<data_length; i++){
		buffer[buffer_length + 0] = charset[(data[i] & 0xF0) >> 4];
		buffer[buffer_length + 1] = charset[(data[i] & 0x0F) >> 0];
		buffer_length += 2;
	}

	buffer[buffer_length] = '\0';
	*strp = buffer;
	return buffer_length;
}
/*}}}*/


/*{{{ Convert hex string to raw buffer */
int hex_to_buf(char ** bufp, const char * data){
	size_t data_length = strlen(data);
	if(data_length % 2 == 1){
		return -1;
	}

	size_t buffer_size = data_length / 2 + 1;
	size_t buffer_length = 0;
	char * buffer = malloc(sizeof(*buffer) * buffer_size);

	for(size_t i=0; i<data_length; i+=2){
		if(isxdigit(data[i + 0]) == 0){
			return -1;
		}
		if(isxdigit(data[i + 1]) == 0){
			return -1;
		}

		printf("[%c][%c]\n", data[i], data[i+1]);

		int hi = isdigit(data[i+0]) ? data[i+0] - '0' : data[i+0] - (isupper(data[i+0]) ? 'A' : 'a') + 10;
		int lo = isdigit(data[i+1]) ? data[i+1] - '0' : data[i+1] - (isupper(data[i+1]) ? 'A' : 'a') + 10;
		printf("[%d][%d]\n", hi, lo);
		buffer[buffer_length] = (hi << 4) | (lo << 0);
		buffer_length += 1;
	}
	buffer[buffer_length] = '\0';
	*bufp = buffer;
	return buffer_length;
}
/*}}}*/


int mpi_master(size_t ranks, size_t rank, void * data){
	FILE ** files = data;
	FILE * ifile = files[0];
	FILE * ofile = files[1];

	// otain the type of hash we're dealing with
	char * type = NULL;
	if(afreadline(&type, ifile) < 0){
		fprintf(stderr, "scatter: error: Unable to read hash type from file.\n");
		return 1;
	}

	// Send hash type to all ranks
	for(size_t target=1; target<ranks; target++){
		mpi_isend_hash_type(type, target);
	}

	// Where should we look fo hash modules?
	char * dirname = NULL;
	if((dirname = getenv("HASH_MODULE_PATH")) == NULL){
		dirname = ".";
	}

	// Construct hash module path
	char * module = NULL;
	size_t module_length = asprintf(&module, "%s/%s.so", dirname, type);

	// Load our hash module
	hash_ctx hctx;
	hash_init(&hctx);
	if(hash_load(&hctx, module) != 0){
		fprintf(stderr, "scatter: error: Failed to load module `%s'\n", module);
		return 1;
	}

	// We don't need the module name anymore
	free(module);

	// What is the size of this specific hash?
	size_t hash_size;
	hctx.info(&hash_size);

	// We don't need the hash module any more
	hash_fini(&hctx);

	// This will contain our list of valid hashes
	size_t hashes_size = 1;
	size_t hashes_length = 0;
	char ** hashes = malloc(sizeof(*hashes) * hashes_size);

	char * line = NULL;
	while(afreadline(&line, ifile) >= 0){

		// resize hashes array
		while(hashes_length >= hashes_size - 1){
			char ** tmp = NULL;
			hashes_size *= 2;
			if((tmp = realloc(hashes, sizeof(*hashes) * hashes_size)) == NULL){
				fprintf(stderr, "scatter: error: Unable to reallocate hash table.\n");
				return 1;
			}
			hashes = tmp;
		}

		// is this a valid ___ hash?
		if(hash_size * 2 == strlen(line)){
			hashes[hashes_length] = line;
			hashes_length += 1;
			hashes[hashes_length] = NULL;
		}else{
			free(line);
		}

	}

	// Output some debug information
	printf("type: [%s]\n", type);
	free(type);

	for(size_t i=0; i<hashes_length; i++){
		printf("hash: [%s]\n", hashes[i]);
		free(hashes[i]);
	}
	free(hashes);

	return 0;
}


int mpi_slave(size_t ranks, size_t rank, void * data){
	// Recieve hash type
	char * type = NULL;
	mpi_recv_hash_type(&type);
	printf("rank: %zu type: [%s]\n", rank, type);

	char module[] = "./build/modules/md5.so";
	size_t iterations = 1000;
	printf("rank: %zu | module: %s iters: %zu\n", rank, module, iterations);

	hash_ctx hctx;
	hash_init(&hctx);

	if(hash_load(&hctx, module) != 0){
		fprintf(stderr, "scatter: error: Failed to load module `%s'\n", module);
		return 1;
	}

	size_t pass_size = 256;
	size_t pass_length = 0;
	char * pass = calloc(pass_size, sizeof(char));

	size_t hash_size;
	hctx.info(&hash_size);
	char * hash = calloc(hash_size, sizeof(char));

	pass_ctx pctx;
	pass_init(&pctx, 3);
	pass_load_int(&pctx, rank-1);

	pass_ctx pstp;
	pass_init(&pstp, 3);
	pass_load_int(&pstp, ranks-1);

	for(size_t n=0; n<iterations; n++){
		pass_blit(&pctx, "abc", pass, &pass_length);
		// printf("pass: %s\n", pass);
		hctx.hash(pass, pass_length, hash, &hash_size);
		// print_hex(hash, hash_size);
		pass_step(&pctx, &pstp);
	}

	printf("rank: %zu | pass: %s\n", rank, pass);

	pass_fini(&pctx);
	pass_fini(&pstp);

	free(pass);
	free(hash);

	hash_fini(&hctx);

	return 0;
}


/*{{{ MPI entry point */
int mpi_main(size_t ranks, size_t rank, size_t argc, char **argv){

	if(ranks < 2){
		fprintf(stderr, "scatter: error: There needs to be at least two ranks.\n");
		return 1;
	}

	if(argc < 3){
		fprintf(stderr, "scatter: error: Insufficient parameters.\n");
		return 1;
	}

	if(rank == 0){
		FILE * ifile = NULL;
		if((ifile = fopen(argv[1], "r")) == NULL){
			fprintf(stderr, "scatter: error: Unable to open `%s' for reading.\n", argv[1]);
			return 1;
		}

		FILE * ofile = NULL;
		if((ofile = fopen(argv[2], "a")) == NULL){
			fprintf(stderr, "scatter: error: Unable to open `%s' for writing.\n", argv[2]);
			return 1;
		}

		FILE ** files = malloc(sizeof(FILE *) * 2);
		files[0] = ifile;
		files[1] = ofile;

		mpi_master(ranks, rank, files);

		free(files);
		fclose(ifile);
		fclose(ofile);
	}

	if(rank != 0){
		mpi_slave(ranks, rank, NULL);
	}

	return 0;
}
/*}}}*/


/*{{{ Application entry point */
int main(int argc, char **argv){
	char name[] = "scatter";

	// Attempt to initialize MPI environment
	if(MPI_Init(&argc, &argv) != MPI_SUCCESS){
		fprintf(stderr, "%s: error: Unable to initialize MPI.\n", name);
		return 1;
	}

	int ranks;
	// Attempt to determine how many MPI ranks there are
	if(MPI_Comm_size(MPI_COMM_WORLD, &ranks) != MPI_SUCCESS){
		fprintf(stderr, "%s: error: Unable to determine MPI comm size.\n", name);
		return 1;
	}

	int rank;
	// Attempt to determine which MPI rank we are
	if(MPI_Comm_rank(MPI_COMM_WORLD, &rank) != MPI_SUCCESS){
		fprintf(stderr, "%s: error: Unable to determine MPI comm rank.\n", name);
		return 1;
	}

	int ret;
	// Call the MPI main, and pass in some extra information
	ret = mpi_main(ranks, rank, argc, argv);

	// Attempt to tear down MPI environment
	if(MPI_Finalize() != MPI_SUCCESS){
		fprintf(stderr, "%s: error: Unable to finalize MPI.\n", name);
		return 1;
	}

	// Return whatever the MPI main returned
	return ret;
}
/*}}}*/
