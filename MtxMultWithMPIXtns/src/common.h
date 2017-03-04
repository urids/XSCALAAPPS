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
int myRank;

#define TRAY0 0
#define TRAY1 1
#define TRAY2 2
#define TRAY3 3
#define TRAY4 4

#define TASK0 0
#define TASK1 1
#define TASK2 2
#define TASK3 3

//#define numTasks 9
//#define size 4096
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


#define initLap do{ \
					if(myRank==0){ \
					gettimeofday(&tval_before, NULL); \
					}\
				  }\
				  while(0);\

#define finiLap do{ \
if (myRank == 0) { \
	double Gflops; \
	gettimeofday(&tval_after, NULL );\
	timersub(&tval_after, &tval_before, &tval_result);\
	double secs = (double) tval_result.tv_sec; \
	double mils = (double) (tval_result.tv_usec) / 1000000; \
	printf("%f\n", secs + mils);\
	Gflops = (double) (2 * pow(size, 3)) / ((secs + mils) * pow(1024, 3)); \
	printf("time: %ld.%06ld\n", (long int) tval_result.tv_sec, \
			(long int) tval_result.tv_usec); \
	printf("performance: %f  Gflops", Gflops); \
	FILE* resultsFp; \
	resultsFp = fopen("resultsMtxMult.txt", "a"); \
	fprintf(resultsFp, "%f ",Gflops); \
	fclose(resultsFp);\
}\
}\
while(0);\

#endif /* COMMON_H_ */


//Aditionally we can perform the scalar-Mtx multiplication and the Mtx-Mtx addition.
/*
err  = XSCALA_CreateKernel(MPI_COMM_WORLD,"/home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/src/matrixMult.cl","mtxAdd");
int Dims2 = 1;
size_t globalDims2[] = { 4 };
size_t localDims2[] = { 4 };

for (taskIdx = 0; taskIdx < numTasks; taskIdx++) {
			err |= XSCALA_ExecKernel(MPI_COMM_SELF, taskIdx, Dims2, globalDims2, localDims2,
					"%T %T %d %d %d ", 2, 6, rPBlock_C, cPBlock_C, 3);
}
err |= XSCALA_WaitAllTasks(MPI_COMM_WORLD );
 */

/*	void * dataRet =  malloc(ePerTask * szEntityA);
float* dataBuff = ((float*) dataRet);

if(myRank==0){
err |= XSCALA_ReadTray(TASK0, TRAY0, ePerTask * szEntityA, dataRet, MPI_COMM_WORLD);
err |= XSCALA_WaitFor(1, (int[]){TASK0}, MPI_COMM_WORLD);
printf(" --%f ", dataBuff[0]);
}*/

/*--	void * res=malloc(sizeof(float));
XSCALA_ReadTray(TASK0, TRAY2, sizeof(float), res, MPI_COMM_WORLD);
err |= XSCALA_WaitFor(1, (int[]){TASK0}, MPI_COMM_WORLD);
printf(" \n res: --%f ", *(float*)res);
--*/


