/*
 * MPIutex.h
 *
 *  Created on: Jul 9, 2016
 *      Author: uriel
 */

#ifndef MPIMUTEX_H_
#define MPIMUTEX_H_

#include <mpi.h>

typedef struct MPI_Mutex MPI_Mutex;

struct MPI_Mutex;

// Collective call
int MPI_Mutex_init(MPI_Mutex **mutex, int home);

// Collective call
int MPI_Mutex_destroy(MPI_Mutex *mutex);

int MPI_Mutex_lock(MPI_Mutex *mutex);

int MPI_Mutex_trylock(MPI_Mutex *mutex);

int MPI_Mutex_unlock(MPI_Mutex *mutex);


#endif /* MPIMUTEX_H_ */
