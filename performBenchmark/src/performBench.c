/*
 * performBench.c
 *
 *  Created on: Dec 9, 2015
 *      Author: uriel
 */

#include "../include/performBench.h"
int i,j;
static int err=0;
char* procPath="/home/uriel/Dev/mpiApps/extnsApps/performBenchmark/src/gemm.cl";
int taskmulDps[1]={0};

int main(int argc, char ** argv){
	int myRank,numRanks;
	double secs,mils;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
	int numTasks=OMPI_CollectTaskInfo(ALL_DEVICES, MPI_COMM_WORLD);

	unsigned int MtxByteSize= PSIZE*PSIZE*sizeof(float);
	unsigned int VecByteSize= PSIZE*sizeof(float);
	float* A=malloc(MtxByteSize);
	float* B=malloc(MtxByteSize);
	float* C=malloc(MtxByteSize);
	float* X=malloc(VecByteSize);
	float* Y=malloc(VecByteSize);

	for(i=0;i<PSIZE;i++){
		for(j=0;j<PSIZE;j++){
			A[PSIZE*i+j]=1.0;
			B[PSIZE*i+j]=1.0;
		}
		X[i]=2.0;
	}

	int Dims = 2;
#define TS 16
#define WPT 8
	size_t globalDims3[] = { PSIZE,PSIZE/WPT };
	size_t localDims3[] = { TS,TS/WPT };


	task_t task[6];
	task[0].ID=0;
	struct timeval tval_intGlobal, tval_endGlobal, tval_result;
if(myRank==0){
		gettimeofday(&tval_intGlobal, NULL);
	}
	err|=OMPI_XclWriteTaskBuffer(task[0].ID, TRAY0, MtxByteSize, A, MPI_COMM_WORLD);
	err|=OMPI_XclWriteTaskBuffer(task[0].ID, TRAY1, MtxByteSize, B, MPI_COMM_WORLD);
	err|=OMPI_XclMallocTaskBuffer(task[0].ID, TRAY2, MtxByteSize, MPI_COMM_WORLD );

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[0].ID,procPath,"myGEMM3");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[0].ID, Dims, globalDims3, localDims3, " %d %d %d %T %T %T", PSIZE, PSIZE,PSIZE, TRAY0,TRAY1 ,TRAY2);

	OMPI_XclWaitFor(1, taskmulDps, MPI_COMM_WORLD);

	if (myRank == 0) {
		double Gflops;
		gettimeofday(&tval_endGlobal, NULL );
		timersub(&tval_endGlobal, &tval_intGlobal, &tval_result);
		secs = (double) tval_result.tv_sec;
		mils = (double) (tval_result.tv_usec) / 1000000;
		printf("%f\n", secs + mils);
		Gflops = (double) (2 * pow(PSIZE, 3)) / ((secs + mils) * ONEGB);
		printf("GEMM3 performance: %f  Gflops \n", Gflops);

		FILE* resultsFp;
		resultsFp = fopen("perforBenchmark.txt", "a");
		fprintf(resultsFp, "%f ",Gflops);
		fclose(resultsFp);
	}



	OMPI_XclReadTaskBuffer(task[0].ID, TRAY2, MtxByteSize, C, MPI_COMM_WORLD);

	printf("%f--\n",C[0]);
	printf("%f--\n",C[1]);
	printf("%f--\n",C[PSIZE*PSIZE-3]);
	printf("%f--\n",C[PSIZE*PSIZE-2]);
	printf("%f--\n",C[PSIZE*PSIZE-1]);

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
	int DimsMAdd = 1;
	#define WPTS 16
		size_t globalDimsMAdd[] = {PSIZE/WPTS};
		size_t localDimsMAdd[] = {8};

	if(myRank==0){
		gettimeofday(&tval_intGlobal, NULL);
	}
	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[0].ID,procPath,"mtxAdd");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[0].ID, DimsMAdd, globalDimsMAdd, localDimsMAdd, "%d %T %T %T", PSIZE, TRAY0,TRAY1 ,TRAY2);

	OMPI_XclWaitFor(1, taskmulDps, MPI_COMM_WORLD);

	if (myRank == 0) {
		double Gflops;
		gettimeofday(&tval_endGlobal, NULL );
		timersub(&tval_endGlobal, &tval_intGlobal, &tval_result);
		secs = (double) tval_result.tv_sec;
		mils = (double) (tval_result.tv_usec) / 1000000;
		Gflops = (double) (pow(PSIZE, 2)) / ((secs + mils) * ONEGB);
		printf("MTXADD performance: %f  Gflops \n", Gflops);

		FILE* resultsFp;
		resultsFp = fopen("perforBenchmark.txt", "a");
		fprintf(resultsFp, "%f ",Gflops);
		fclose(resultsFp);
	}


	OMPI_XclReadTaskBuffer(task[0].ID, TRAY2, MtxByteSize, C, MPI_COMM_WORLD);

	printf("%f--\n",C[0]);
	printf("%f--\n",C[1]);
	printf("%f--\n",C[PSIZE*PSIZE-3]);
	printf("%f--\n",C[PSIZE*PSIZE-2]);
	printf("%f--\n",C[PSIZE*PSIZE-1]);


//---------------------------------------------------------------------------------------------------------
//TODO: check the issue: __local modifier in kernel to allocate memory in the kernel do not works. =(
//---------------------------------------------------------------------------------------------------------

	err|=OMPI_XclWriteTaskBuffer(task[0].ID, TRAY1, VecByteSize, X, MPI_COMM_WORLD);
	err|=OMPI_XclMallocTaskBuffer(task[0].ID, TRAY2, VecByteSize,  MPI_COMM_WORLD );

#define WPTx 16
#define tPTx 16
#define tPTy 1

#define TSx (WPTx*tPTx)
#define TSy 16

#define NUMTILESx ((PSIZE-1)/TSx)+1)
#define NUMTILESy (((PSIZE-1)/TSy)+1)

	int GEMVDims = 2;
	size_t GEMVglobalDims[] = {PSIZE/TSy,tPTx};
	size_t GEMVlocalDims[] = {tPTy,tPTx};
	if(myRank==0){
			gettimeofday(&tval_intGlobal, NULL);
		}

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[0].ID,procPath,"GEMV");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[0].ID, GEMVDims, GEMVglobalDims, GEMVlocalDims, "%T %T %T %d", TRAY0,TRAY1 ,TRAY2,PSIZE);


	OMPI_XclWaitFor(1, taskmulDps, MPI_COMM_WORLD);

		if (myRank == 0) {
			double Gflops;
			gettimeofday(&tval_endGlobal, NULL );
			timersub(&tval_endGlobal, &tval_intGlobal, &tval_result);
			secs = (double) tval_result.tv_sec;
			mils = (double) (tval_result.tv_usec) / 1000000;
			Gflops = (double) (2*pow(PSIZE, 2)) / ((secs + mils) * ONEGB);
			printf("SGEMV performance: %f  Gflops \n", Gflops);

			FILE* resultsFp;
			resultsFp = fopen("perforBenchmark.txt", "a");
			fprintf(resultsFp, "%f ",Gflops);
			fclose(resultsFp);
		}

	OMPI_XclReadTaskBuffer(task[0].ID, TRAY2, VecByteSize, Y, MPI_COMM_WORLD);

	printf("%f--\n",Y[0]);
	printf("%f--\n",Y[1]);
	printf("%f--\n",Y[PSIZE-3]);
	printf("%f--\n",Y[PSIZE-2]);
	printf("%f--\n",Y[PSIZE-1]);

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------


#define VECSZ 16
#define SA_WPt 16
#define SA_tPT 16
#define SA_TILESZ 	(VECSZ*SA_WPt*SA_tPT)
#define SA_NumTiles ((PSIZE/SA_TILESZ -1 )+1)


	int DimsSAXPY = 1;
		size_t globalDimsSAXPY[] = {SA_NumTiles*SA_tPT};
		size_t localDimsSAXPY[] = {SA_tPT};

	if(myRank==0){
		gettimeofday(&tval_intGlobal, NULL);
	}
	float alpha=2.0;
	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[0].ID,procPath,"SAXPY");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[0].ID, DimsSAXPY, globalDimsSAXPY, localDimsSAXPY, "%f %T %T", alpha, TRAY1,TRAY2);

	OMPI_XclWaitFor(1, taskmulDps, MPI_COMM_WORLD);

	if (myRank == 0) {
		double Gflops;
		gettimeofday(&tval_endGlobal, NULL );
		timersub(&tval_endGlobal, &tval_intGlobal, &tval_result);
		secs = (double) tval_result.tv_sec;
		mils = (double) (tval_result.tv_usec) / 1000000;
		Gflops = (double) (2*PSIZE) / ((secs + mils) * ONEGB);
		printf("SAXPY performance: %f  Gflops \n", Gflops);
	}


	OMPI_XclReadTaskBuffer(task[0].ID, TRAY2, VecByteSize, Y, MPI_COMM_WORLD);


	printf("%f--\n",Y[0]);
	printf("%f--\n",Y[1]);
	printf("%f--\n",Y[PSIZE-3]);
	printf("%f--\n",Y[PSIZE-2]);
	printf("%f--\n",Y[PSIZE-1]);


	MPI_Finalize();
	return 0;
}


