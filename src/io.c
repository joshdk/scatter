#ifndef _IO_C_
#define _IO_C_

#include "io.h"




int mpi_isend_hash_type(const char * type, int rank){
	MPI_Request request;

	MPI_Isend(type, strlen(type), MPI_CHAR, rank, 0, MPI_COMM_WORLD, &request);

	return 0;
}


int mpi_recv_hash_type(char ** type){
	MPI_Status status;
	int buffer_size = 0;

	MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_CHAR, &buffer_size);

	char * buffer = NULL;
	buffer = calloc(buffer_size + 1, sizeof(char));

	MPI_Recv(buffer, buffer_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);

	*type = buffer;
	return 0;
}


int mpi_isend_hashes(char ** data, size_t size, size_t nmem, int rank){
	size_t buffer_size = nmem * size;
	char * buffer = calloc(nmem * size, sizeof(char));

	for(size_t i=0; i<nmem; i++){
		memcpy(buffer + i*size, data[i], size);
	}

	MPI_Request request;

	MPI_Isend(buffer, buffer_size, MPI_CHAR, rank, 1, MPI_COMM_WORLD, &request);

	return 0;
}


int mpi_recv_hashes(char *** data, size_t size, size_t * nmem){
	MPI_Status status;
	int buffer_size = 0;

	MPI_Probe(0, 1, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_CHAR, &buffer_size);
	assert(buffer_size % size == 0);

	char * buffer = NULL;
	buffer = calloc(buffer_size, sizeof(char));

	MPI_Recv(buffer, buffer_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);

	size_t hashes_size = buffer_size / size;
	size_t hashes_length = 0;
	char ** hashes = malloc(sizeof(*hashes) * hashes_size);

	for(size_t i=0; i<hashes_size; i++){
		hashes[i] = malloc(sizeof(char) * size);
		memcpy(hashes[i], buffer + i*size, size);
	}

	free(buffer);

	*data = hashes;
	*nmem = hashes_size;
	return 0;
}


int mpi_isend_charset(char * data, size_t size, int rank){
	size_t buffer_size = size;
	char * buffer = calloc(buffer_size, sizeof(char));

	memcpy(buffer, data, buffer_size);

	MPI_Request request;

	MPI_Isend(buffer, buffer_size, MPI_CHAR, rank, 2, MPI_COMM_WORLD, &request);

	return 0;
}


int mpi_recv_charset(char ** data, size_t * size){
	MPI_Status status;
	int buffer_size = 0;

	MPI_Probe(0, 2, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_CHAR, &buffer_size);

	char * buffer = NULL;
	buffer = calloc(buffer_size + 1, sizeof(char));

	MPI_Recv(buffer, buffer_size, MPI_CHAR, 0, 2, MPI_COMM_WORLD, &status);

	*size = buffer_size;
	*data = buffer;
	return 0;
}

#endif
