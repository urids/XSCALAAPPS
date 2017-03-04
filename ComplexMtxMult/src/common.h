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
#include <sys/time.h>



//#define DOUBLE_P 1


int i,j,k;//index exclusive variables.


extern int size;

#define rowsA1 size
#define colsA1 size

#define rowsB1 size
#define colsB1 size

#define rowsA2 size
#define colsA2 size

#define rowsB2 size
#define colsB2 size


/*typedef struct entityType_st{
	float* A_block;
}entityType;
*/

MPI_Datatype entityType;
extern int numBcks;
int numBcks;

#endif /* COMMON_H_ */



