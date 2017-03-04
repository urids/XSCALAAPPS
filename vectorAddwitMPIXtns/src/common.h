/*
 * common.h
 *
 *  Created on: Feb 25, 2015
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

#define TRAY0 0
#define TASK0 0

int i,j; //exclusive for index variables

int ePerTask;
int err;
int tskIdx;
int myrank;
struct timeval tval_before, tval_after, tval_result;

extern int VectorSize;

typedef struct entityType_st{
	float Opers[3];
}entityType;

MPI_Datatype MPIentityType;


#define initLapse do{ \
					if(myrank==0){ \
					gettimeofday(&tval_before, NULL); \
					}\
				  }\
				  while(0);\

#define finiLapse do{\
		if (myrank == 0) { \
			double Mflops; \
			gettimeofday(&tval_after, NULL ); \
			timersub(&tval_after, &tval_before, &tval_result); \
			double secs = (double) tval_result.tv_sec; \
			double mils = (double) (tval_result.tv_usec) / 1000000; \
			Mflops = (double) (2 * VectorSize) / ((secs + mils) * pow(1024, 2)); \
			printf("time: %ld.%08ld\n", (long int) tval_result.tv_sec, \
					(long int) tval_result.tv_usec); \
			printf("performance: %f  Mflops \n", Mflops); \
		}\
		}\
		while(0);\





#endif /* COMMON_H_ */
