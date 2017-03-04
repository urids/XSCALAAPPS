/*
 * common.h
 *
 *  Created on: May 15, 2015
 *      Author: uriel
 */

/*	void *v;
	int flag;
	int vval;
	MPI_Comm_get_attr( MPI_COMM_WORLD, MPI_WTIME_IS_GLOBAL, &v, &flag );
	if (flag) {
		/* Wtime need not be set */
	/*	vval = *(int*)v;
		printf("vval: %d \n",vval);
		if (vval < 0 || vval > 1) {
			fprintf( stderr, "Invalid value for WTIME_IS_GLOBAL (got %d)\n", vval );fflush(stderr);
		}
	}*/

#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include "math.h"
#include <sys/time.h>

int i,j,k;//index exclusive variables.
/*
#define TASK0 0
#define TASK1 1
#define TASK2 2
#define TASK3 3
#define TASK4 4
#define TASK5 5
#define TASK6 6
*/
#define TRAY0 0
#define TRAY1 1
#define TRAY2 2
#define TRAY3 3
#define TRAY4 4
#define TRAY5 5
#define TRAY6 6

//#define numTasks 9
//#define size 4096
extern int size;

#define rowsA size
#define colsA size

#define rowsB size
#define colsB size

#define rowsC rowsA
#define colsC colsB


MPI_Datatype entityType;



void refactorData();
extern int numTasks;


extern int rowsAs; /* number of rows in _partition_ */
extern int colsAs; /* number of cols in _partition_ */

extern int rPBlock_A; /* number of rows in a _block_ of A */
extern int cPBlock_A; /* number of cols in a _block_ of A */

extern int rPBlock_B; /* number of rows in a _block_ of A */
extern int cPBlock_B; /* number of cols in a _block_ of A */

extern int rPBlock_C; /* number of rows in a _block_ of A */
extern int cPBlock_C; /* number of cols in a _block_ of A */

extern int *myRight;
extern int *myDown;

extern int blockId;
extern int blockSize_A;
extern int blockSize_B;
extern int blockSize_C;

extern float* A;
extern float* B;
extern float* C;

#define TSM 128 // The tile-size in dimension M
#define TSN 128 // The tile-size in dimension N
#define TSK 16 // The tile-size in dimension K
#define WPTM 8 // The work-per-thread in dimension M
#define WPTN 8 // The work-per-thread in dimension N
#define RTSM (TSM/WPTM) // The reduced tile-size in dimension M
#define RTSN (TSN/WPTN) // The reduced tile-size in dimension N
#define LPTA ((TSK*TSM)/(RTSM*RTSN)) // Loads-per-thread for A
#define LPTB ((TSK*TSN)/(RTSM*RTSN)) // Loads-per-thread for B

#endif /* COMMON_H_ */



