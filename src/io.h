#ifndef _IO_H_
#define _IO_H_

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>




int mpi_isend_hash_type(const char * type, int rank);
int mpi_recv_hash_type(char ** type);

int mpi_isend_hashes(char ** data, size_t nmem, size_t size, int rank);
int mpi_recv_hashes(char *** data, size_t size, size_t * nmem);

int mpi_isend_start(int rank);
int mpi_isend_stop(int rank);

#endif
