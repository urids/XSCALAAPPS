/*
 * common.h
 *
 *  Created on: Dec 16, 2015
 *      Author: uriel
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "mpi.h"


int i,j,k;//index exclusive variables.
int err;

#define param_t float

#define TRAY0 0
#define TRAY1 1
#define TRAY2 2
#define TRAY3 3
#define TRAY4 4
#define TRAY5 5
#define TRAY6 6


float *A11,*A12,*A21,*A22;
float *B11,*B12,*B21,*B22;
float ** M;

struct timeval tval_Init, tval_End;
struct timeval tval_SumCost, tval_MulCost,
	   tval_TransCost,tval_LocalTransCost,
	   tval_FinalSumCost;
double secs,mils;

#endif /* COMMON_H_ */
