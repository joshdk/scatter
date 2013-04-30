#ifndef _PARSE_C_
#define _PARSE_C_

#include "parse.h"




int afreadline(char ** strp, FILE * stream){
	size_t buffer_size = 64;
	size_t buffer_length = 0;
	char * buffer = malloc(sizeof(*buffer) * buffer_size);

	while(1){
		char ch = fgetc(stream);

		if(feof(stream) != 0){
			if(buffer_length == 0){
				free(buffer);
				return -1;
			}
			break;
		}

		if(ch == '\n' || ch == '\r' || ch == '\0'){
			if(buffer_length == 0){
				continue;
			}
			break;
		}

		while(buffer_length >= buffer_size - 1){
			buffer_size *= 2;
			char * tmp = NULL;
			if((tmp = realloc(buffer, buffer_size)) == NULL){
				return -1;
			}
			buffer = tmp;
		}

		buffer[buffer_length] = ch;
		buffer_length += 1;
	}

	buffer[buffer_length] = '\0';
	*strp = buffer;
	return buffer_length;
}


int asplit(char *** arrp, const char * str, size_t size, const char * delim){
	size_t chunks_size = 1;
	size_t chunks_length = 0;
	char ** chunks = malloc(sizeof(*chunks) * chunks_size);

	size_t chunk_size = 0;
	size_t chunk_length = 0;
	char * chunk = NULL;

	size_t str_length = size;
	size_t delim_length = strlen(delim);

	for(size_t i=0; i<str_length; i++){
		if(chunk == NULL){
			chunk_size = 1;
			chunk_length = 0;
			chunk = malloc(sizeof(*chunk) * chunk_size);
		}

		if(chunk_length == chunk_size - 1){
			chunk_size *= 2;
			char * tmp = NULL;
			if((tmp = realloc(chunk, sizeof(*chunk) * chunk_size)) == NULL){
				return -1;
			}
			chunk = tmp;
		}

		if(chunks_length == chunks_size - 1){
			chunks_size *= 2;
			char ** tmp = NULL;
			if((tmp = realloc(chunks, sizeof(*chunks) * chunks_size)) == NULL){
				return -1;
			}
			chunks = tmp;
		}

		// if the current character a delimeter?
		int found = 0;
		for(size_t j=0; j<delim_length; j++){
			if(str[i] == delim[j]){
				found = 1;
				break;
			}
		}

		// if it was...
		if(found == 1){
			if(chunk_length == 0){
				continue;
			}
			chunks[chunks_length] = chunk;
			chunks_length += 1;
			chunks[chunks_length] = NULL;
			chunk = NULL;
		// if it wasn't...
		}else{
			chunk[chunk_length] = str[i];
			chunk_length += 1;
			chunk[chunk_length] = '\0';
			// fprintf(stderr, "chunk: [%s]\n", chunk);
		}
	}

	// chunks[chunks_length] = NULL;
	*arrp = chunks;
	return chunks_length;
}

#endif
