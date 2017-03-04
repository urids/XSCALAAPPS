/*
 * dMutex.c
 *
 *  Created on: Jul 9, 2016
 *      Author: uriel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>
#include "MPImutex.h"
//#include "utest.h"

#define MPI_MUTEX_MSG_TAG_BASE 1023

struct MPI_Mutex {
	int numprocs, ID, home, tag;
	MPI_Comm comm;
	MPI_Win win;
	unsigned char *waitlist;
};

int MPI_Mutex_init(MPI_Mutex **mutex, int home)
{
	static int tag = MPI_MUTEX_MSG_TAG_BASE;
	int numprocs, ID;
	MPI_Mutex *mtx;

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &ID);

	mtx = (MPI_Mutex *)malloc(sizeof(MPI_Mutex));
	if (!mtx) {
		fprintf(stderr, "Warning: malloc failed on worker %2d\n", ID);
		return 1;
	}

	mtx->numprocs = numprocs;
	mtx->ID = ID;
	mtx->home = home;
	mtx->tag = tag++;
	mtx->comm = MPI_COMM_WORLD;

	if (ID == mtx->home) {
		// Allocate and expose waitlist
		MPI_Alloc_mem(mtx->numprocs, MPI_INFO_NULL, &mtx->waitlist);
		if (!mtx->waitlist) {
			fprintf(stderr, "Warning: MPI_Alloc_mem failed on worker %2d\n", ID);
			return 1;
		}
		memset(mtx->waitlist, 0, mtx->numprocs);
		MPI_Win_create(mtx->waitlist, mtx->numprocs, 1, MPI_INFO_NULL, mtx->comm, &mtx->win);
	} else {
		// Don't expose anything
		mtx->waitlist = NULL;
		MPI_Win_create(mtx->waitlist, 0, 1, MPI_INFO_NULL, mtx->comm, &mtx->win);
	}

	*mutex = mtx;

	MPI_Barrier(MPI_COMM_WORLD);

	return 0;
}

int MPI_Mutex_destroy(MPI_Mutex *mutex)
{
	assert(mutex != NULL);

	int ID;

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Comm_rank(MPI_COMM_WORLD, &ID);

	if (ID == mutex->home) {
		// Free waitlist
		MPI_Win_free(&mutex->win);
		MPI_Free_mem(mutex->waitlist);
	} else {
		MPI_Win_free(&mutex->win);
		assert(mutex->waitlist == NULL);
	}

	return 0;
}

int MPI_Mutex_lock(MPI_Mutex *mutex)
{
	assert(mutex != NULL);

	unsigned char waitlist[mutex->numprocs];
	unsigned char lock = 1;
	int i;

	// Try to acquire lock in one access epoch
	MPI_Win_lock(MPI_LOCK_EXCLUSIVE, mutex->home, 0, mutex->win);
	MPI_Put(&lock, 1, MPI_CHAR, mutex->home, mutex->ID /* &win[mutex->ID] */, 1, MPI_CHAR, mutex->win);
	MPI_Get(waitlist, mutex->numprocs, MPI_CHAR, mutex->home, 0, mutex->numprocs, MPI_CHAR, mutex->win);
	MPI_Win_unlock(mutex->home, mutex->win);

	assert(waitlist[mutex->ID] == 1);

	// Count the 1's
	for (i = 0; i < mutex->numprocs; i++) {
		if (waitlist[i] == 1 && i != mutex->ID) {
			// We have to wait for the lock
			// Dummy receive, no payload
			//printf("Worker %d waits for lock\n", mutex->ID);
			MPI_Recv(&lock, 0, MPI_CHAR, MPI_ANY_SOURCE, mutex->tag, mutex->comm, MPI_STATUS_IGNORE);
			break;
		}
	}

	//printf("Worker %d has the lock\n", mutex->ID);

	return 0;
}

int MPI_Mutex_trylock(MPI_Mutex *mutex)
{
	assert(mutex != NULL);

	unsigned char waitlist[mutex->numprocs];
	unsigned char lock = 1;
	int i;

	// Try to acquire lock in one access epoch
	MPI_Win_lock(MPI_LOCK_EXCLUSIVE, mutex->home, 0, mutex->win);
	MPI_Put(&lock, 1, MPI_CHAR, mutex->home, mutex->ID /* &win[mutex->ID] */, 1, MPI_CHAR, mutex->win);
	MPI_Get(waitlist, mutex->numprocs, MPI_CHAR, mutex->home, 0, mutex->numprocs, MPI_CHAR, mutex->win);
	MPI_Win_unlock(mutex->home, mutex->win);

	assert(waitlist[mutex->ID] == 1);

	// Count the 1's
	for (i = 0; i < mutex->numprocs; i++) {
		if (waitlist[i] == 1 && i != mutex->ID) {
			// Lock is already held, return immediately
			return 1;
		}
	}

	//printf("Worker %d has the lock\n", mutex->ID);

	return 0;
}

int MPI_Mutex_unlock(MPI_Mutex *mutex)
{
	assert(mutex != NULL);

	unsigned char waitlist[mutex->numprocs];
	unsigned char lock = 0;
	int i, next;

	MPI_Win_lock(MPI_LOCK_EXCLUSIVE, mutex->home, 0, mutex->win);
	MPI_Put(&lock, 1, MPI_CHAR, mutex->home, mutex->ID /* &win[mutex->ID] */, 1, MPI_CHAR, mutex->win);
	MPI_Get(waitlist, mutex->numprocs, MPI_CHAR, mutex->home, 0, mutex->numprocs, MPI_CHAR, mutex->win);
	MPI_Win_unlock(mutex->home, mutex->win);

	assert(waitlist[mutex->ID] == 0);

	// If there are other processes waiting for the lock, transfer ownership
        next = (mutex->ID + 1 + mutex->numprocs) % mutex->numprocs;
	for (i = 0; i < mutex->numprocs; i++, next = (next + 1) % mutex->numprocs) {
		if (waitlist[next] == 1) {
			// Dummy send, no payload
			//printf("Worker %d transfers lock ownership to worker %d\n", mutex->ID, i);
			MPI_Send(&lock, 0, MPI_CHAR, next, mutex->tag, mutex->comm);
			break;
		}
	}

	return 0;
}
/*
#define MASTER_ID 	0
#define WORKER(id) 	if (ID == (id))
#define WORKERS 	if (ID != MASTER_ID)
#define MASTER 		WORKER(MASTER_ID)

void UTEST()
{
	int numprocs, ID;
	int *counter, buf;
	MPI_Win win;
	MPI_Mutex *m;

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &ID);

	MPI_Mutex_init(&m, 0);
	check_equal(m->home, 0);
	check_equal(m->tag, MPI_MUTEX_MSG_TAG_BASE);
	MPI_Mutex_destroy(m);

	MPI_Mutex_init(&m, 1);
	check_equal(m->home, 1);
	check_equal(m->tag, MPI_MUTEX_MSG_TAG_BASE + 1);

	// A simple atomic counter

	MASTER {
		MPI_Alloc_mem(sizeof(int), MPI_INFO_NULL, &counter);
		*counter = 0;
		MPI_Win_create(counter, sizeof(int), 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
	} else {
		MPI_Win_create(counter, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Mutex_lock(m);

	MPI_Win_lock(MPI_LOCK_EXCLUSIVE, MASTER_ID, 0, win);
	MPI_Get(&buf, 1, MPI_INT, MASTER_ID, 0, 1, MPI_INT, win);
	MPI_Win_unlock(MASTER_ID, win);
	buf++;
	MPI_Win_lock(MPI_LOCK_EXCLUSIVE, MASTER_ID, 0, win);
	MPI_Put(&buf, 1, MPI_INT, MASTER_ID, 0, 1, MPI_INT, win);
	MPI_Win_unlock(MASTER_ID, win);

	MPI_Mutex_unlock(m);

	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Win_lock(MPI_LOCK_SHARED, MASTER_ID, 0, win);
	MPI_Get(&buf, 1, MPI_INT, MASTER_ID, 0, 1, MPI_INT, win);
	MPI_Win_unlock(MASTER_ID, win);
	check_equal(buf, numprocs);

	MPI_Win_free(&win);

	MASTER MPI_Free_mem(counter);

	MPI_Mutex_destroy(m);
}*/


