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

#endif
