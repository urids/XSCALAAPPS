/*
 * commBench.h
 *
 *  Created on: Sep 11, 2015
 *      Author: uriel
 */

#ifndef COMMBENCH_H_
#define COMMBENCH_H_

#include "mpi.h"
#include "stdlib.h"
#include "stdio.h"

#define MIN_SIZE (1<<24) //16 MB
#define MAX_SIZE (1<<27) //128 MB
#define ONEGB (1<<30)
#define MAX_BW_REPS 30
#define	LATENCY_REPS 1000

int msgSz;
int i,j;
int src,dst;
int err;

float L;
float* L_Mtx;

float BW;
float* BW_Mtx;
int numBWTrials;
float sumAvgs;
float Avg[MAX_BW_REPS];

double T1, T2,              /* start/end times per rep */
accumTime,                   /* sum of all reps times */
deltaT;                 /* time for one rep */

#endif /* COMMBENCH_H_ */
