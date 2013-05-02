#ifndef _IO_H_
#define _IO_H_

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int mpi_isend_hash_type(const char * type, int rank);
int mpi_recv_hash_type(char ** type);

int mpi_isend_hash(char * data, size_t size, int rank);
int mpi_recv_hash(char ** data);

int mpi_isend_start(int rank);
int mpi_isend_stop(int rank);

#endif
