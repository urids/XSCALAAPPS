/*
 * common.h
 *
 *  Created on: May 15, 2015
 *      Author: uriel
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include "math.h"
#include "CL/cl.h"
#include <sys/time.h>

int i,j,k;//index exclusive variables.


//#define numTasks 9
extern int size;

#define rowsA size
#define colsA size

#define rowsB size
#define colsB size

#define rowsC rowsA
#define colsC colsB

#define rowsD rowsC
#define colsD colsC


/*typedef struct entityType_st{
	float* A_block;
}entityType;
*/

MPI_Datatype entityType;



void refactorData();
extern int numTasks;

int numTasks;

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

#endif /* COMMON_H_ */



