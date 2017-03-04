/*
 * mainStrassen.c
 *
 *  Created on: Dec 16, 2015
 *      Author: uriel
 */


#include "common.h"
int i=0,j;


int main(int argc,char** argv){

	char* infArg;
	infArg = strtok(argv[5], "=");
	infArg = strtok(NULL, "= ");

	int SIZE = atoi(infArg);
	printf(" size: %d\n",SIZE);

	int myRank,numRanks;

	task_t task[21];
	static char* srcPath="/home/uriel/Dev/mpiApps/extnsApps/StrassenV2/src/kernels.cl";
	task[0].ID=0;
	task[1].ID=14;
	task[2].ID=1;
	task[3].ID=2;
	task[4].ID=3;
	task[5].ID=4;
	task[6].ID=5;
	task[7].ID=6;
	task[8].ID=7;
	task[9].ID=8;
	task[10].ID=9;
	task[11].ID=10;
	task[12].ID=15;
	task[13].ID=11;
	task[14].ID=12;
	task[15].ID=16;
	task[16].ID=13;
	task[17].ID=17;
	task[18].ID=18;
	task[19].ID=19;
	task[20].ID=20;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);

	int numTasks=OMPI_CollectTaskInfo(ALL_DEVICES, MPI_COMM_WORLD);
	int HALF_SIZE=(int) SIZE/2;
	int subMtxSz=HALF_SIZE*HALF_SIZE;
	int subMtxMemSz=subMtxSz*sizeof(float);


	int DimsMAdd = 1;
	#define WPTS 16
	size_t globalDimsMAdd[] = {HALF_SIZE/WPTS};
	size_t localDimsMAdd[] = {8};

if(myRank==0){
	A11=malloc(subMtxMemSz);
	A12=malloc(subMtxMemSz);
	A21=malloc(subMtxMemSz);
	A22=malloc(subMtxMemSz);

	B11=malloc(subMtxMemSz);
	B12=malloc(subMtxMemSz);
	B21=malloc(subMtxMemSz);
	B22=malloc(subMtxMemSz);


	for(i=0;i<HALF_SIZE;i++){
		for(j=0;j<HALF_SIZE;j++){
			A11[HALF_SIZE*i+j]=1.0;
			A12[HALF_SIZE*i+j]=1.0;
			A21[HALF_SIZE*i+j]=1.0;
			A22[HALF_SIZE*i+j]=1.0;

			B11[HALF_SIZE*i+j]=2.0;
			B12[HALF_SIZE*i+j]=2.0;
			B21[HALF_SIZE*i+j]=2.0;
			B22[HALF_SIZE*i+j]=2.0;
		}
	}
}

//if(myRank==1){
	M=malloc(7*sizeof(float*));
	for(i=0;i<7;i++)
		M[i]=malloc(subMtxMemSz);
//}



//err|= OMPI_XclMallocTaskBuffer(task[17].ID, TRAY0, subMtxMemSz, MPI_COMM_WORLD );
//err|= OMPI_XclMallocTaskBuffer(task[17].ID, TRAY0, subMtxMemSz, MPI_COMM_WORLD );
//err|= OMPI_XclMallocTaskBuffer(task[17].ID, TRAY1, subMtxMemSz, MPI_COMM_WORLD );

//======================================================
//--------------------- TASK 0 -------------------------
if(myRank==0){
		gettimeofday(&tval_Init, NULL);
}
	err|= OMPI_XclWriteTaskBuffer(task[0].ID, TRAY0, subMtxMemSz, A11, MPI_COMM_WORLD);
	err|= OMPI_XclWriteTaskBuffer(task[0].ID, TRAY1, subMtxMemSz, A22, MPI_COMM_WORLD);

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[0].ID,srcPath,"mtxAdd");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[0].ID, DimsMAdd, globalDimsMAdd, localDimsMAdd, "%d %T %T", HALF_SIZE, TRAY0,TRAY1);

	err|=OMPI_XclWaitFor(1, &task[0].ID, MPI_COMM_WORLD);
	if(myRank==0){
		gettimeofday(&tval_End, NULL);
		timersub(&tval_End, &tval_Init, &tval_SumCost);
		secs = (double) tval_SumCost.tv_sec;
		mils = (double) (tval_SumCost.tv_usec) / 1000000;
		printf("Sum COST: %f\n", secs + mils);
	}


	err|=OMPI_XclFreeTaskBuffer(task[0].ID, TRAY1, MPI_COMM_WORLD);
	err|= OMPI_XclMallocTaskBuffer(task[2].ID, TRAY0, subMtxMemSz, MPI_COMM_WORLD );
	OMPI_XclSendRecv(task[0].ID, TRAY0, task[2].ID, TRAY0, subMtxSz, MPI_FLOAT, MPI_COMM_WORLD);
	err|=OMPI_XclFreeTaskBuffer(task[0].ID, TRAY0, MPI_COMM_WORLD);

//======================================================
//--------------------- TASK 1 -------------------------

	err|= OMPI_XclWriteTaskBuffer(task[1].ID, TRAY0, subMtxMemSz, B11, MPI_COMM_WORLD);
	err|= OMPI_XclWriteTaskBuffer(task[1].ID, TRAY1, subMtxMemSz, B22, MPI_COMM_WORLD);

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[1].ID,srcPath,"mtxAdd");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[1].ID, DimsMAdd, globalDimsMAdd, localDimsMAdd, "%d %T %T", HALF_SIZE, TRAY0,TRAY1);

	OMPI_XclWaitFor(1, &task[1].ID, MPI_COMM_WORLD);

	err|=OMPI_XclFreeTaskBuffer(task[1].ID, TRAY1, MPI_COMM_WORLD);
	err|= OMPI_XclMallocTaskBuffer(task[2].ID, TRAY1, subMtxMemSz, MPI_COMM_WORLD );
	if(myRank==0){
		gettimeofday(&tval_Init, NULL);
	}
	OMPI_XclSendRecv(task[1].ID, TRAY0, task[2].ID, TRAY1, subMtxSz, MPI_FLOAT, MPI_COMM_WORLD);
	if(myRank==0){
			gettimeofday(&tval_End, NULL);
			timersub(&tval_End, &tval_Init, &tval_LocalTransCost);
			secs = (double) tval_LocalTransCost.tv_sec;
			mils = (double) (tval_LocalTransCost.tv_usec) / 1000000;
			printf("Local Transfer COST: %f\n", secs + mils);
	}

	err|=OMPI_XclFreeTaskBuffer(task[1].ID, TRAY0, MPI_COMM_WORLD);



//======================================================
//--------------------- TASK 2 -------------------------

	int Dims = 2;
	#define TS 16
	#define WPT 8
	size_t globalDims3[] = { HALF_SIZE,HALF_SIZE/WPT };
	size_t localDims3[] = { TS,TS/WPT };

	gettimeofday(&tval_Init, NULL);

	err|= OMPI_XclMallocTaskBuffer(task[2].ID, TRAY2, subMtxMemSz, MPI_COMM_WORLD );

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[2].ID,srcPath,"GEMM");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[2].ID, Dims, globalDims3, localDims3, "%d %T %T %T", HALF_SIZE, TRAY0,TRAY1 ,TRAY2);
	OMPI_XclWaitFor(1, &task[2].ID, MPI_COMM_WORLD);


	gettimeofday(&tval_End, NULL);
	timersub(&tval_End, &tval_Init, &tval_MulCost);
	secs = (double) tval_MulCost.tv_sec;
	mils = (double) (tval_MulCost.tv_usec) / 1000000;
	printf("MULT COST: %f\n", secs + mils);
	double Gflops = (double) ((2 * pow(HALF_SIZE,3)))/ ((secs + mils) *pow(1024,3));

			printf("performance:) %f  Gflops", Gflops);


//if(myRank==1){
	//gettimeofday(&tval_Init, NULL);
//}
	err|= OMPI_XclMallocTaskBuffer(task[17].ID, TRAY0, subMtxMemSz, MPI_COMM_WORLD );
	OMPI_XclSendRecv(task[2].ID, TRAY2, task[17].ID, TRAY0, subMtxSz, MPI_FLOAT, MPI_COMM_WORLD);
/*
if(myRank==1){
		gettimeofday(&tval_End, NULL);
		timersub(&tval_End, &tval_Init, &tval_TransCost);
		secs = (double) tval_TransCost.tv_sec;
		mils = (double) (tval_TransCost.tv_usec) / 1000000;
		printf("Transfer COST: %f\n", secs + mils);
}*/

	err|=OMPI_XclFreeTaskBuffer(task[2].ID, TRAY0, MPI_COMM_WORLD);
	err|=OMPI_XclFreeTaskBuffer(task[2].ID, TRAY1, MPI_COMM_WORLD);
	err|=OMPI_XclFreeTaskBuffer(task[2].ID, TRAY2, MPI_COMM_WORLD);

//======================================================
//--------------------- TASK 3 -------------------------

	err|= OMPI_XclWriteTaskBuffer(task[3].ID, TRAY0, subMtxMemSz, A21, MPI_COMM_WORLD);
	err|= OMPI_XclWriteTaskBuffer(task[3].ID, TRAY1, subMtxMemSz, A22, MPI_COMM_WORLD);

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[3].ID,srcPath,"mtxAdd");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[3].ID, DimsMAdd, globalDimsMAdd, localDimsMAdd, "%d %T %T", HALF_SIZE, TRAY0,TRAY1);

	err|=OMPI_XclWaitFor(1, &task[3].ID, MPI_COMM_WORLD);

	err|=OMPI_XclFreeTaskBuffer(task[3].ID, TRAY1, MPI_COMM_WORLD);
	err|= OMPI_XclMallocTaskBuffer(task[4].ID, TRAY0, subMtxMemSz, MPI_COMM_WORLD );
	OMPI_XclSendRecv(task[3].ID, TRAY0, task[4].ID, TRAY0, subMtxSz, MPI_FLOAT, MPI_COMM_WORLD);
	err|=OMPI_XclFreeTaskBuffer(task[3].ID, TRAY0, MPI_COMM_WORLD);


//======================================================
//--------------------- TASK 4 -------------------------

	err|= OMPI_XclWriteTaskBuffer(task[4].ID, TRAY1, subMtxMemSz, B11, MPI_COMM_WORLD);
	err|= OMPI_XclMallocTaskBuffer(task[4].ID, TRAY2, subMtxMemSz, MPI_COMM_WORLD );

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[4].ID,srcPath,"GEMM");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[4].ID, Dims, globalDims3, localDims3, "%d %T %T %T", HALF_SIZE, TRAY0,TRAY1 ,TRAY2);
	OMPI_XclWaitFor(1, &task[4].ID, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[4].ID, TRAY2, task[17].ID, TRAY1, subMtxSz, MPI_FLOAT, MPI_COMM_WORLD);

	err|=OMPI_XclFreeTaskBuffer(task[4].ID, TRAY0, MPI_COMM_WORLD);
	err|=OMPI_XclFreeTaskBuffer(task[4].ID, TRAY1, MPI_COMM_WORLD);
	err|=OMPI_XclFreeTaskBuffer(task[4].ID, TRAY2, MPI_COMM_WORLD);


//======================================================
//--------------------- TASK 17 -------------------------

	//err|= OMPI_XclMallocTaskBuffer(task[17].ID, TRAY5, subMtxMemSz, MPI_COMM_WORLD );

	if(myRank==0){
		gettimeofday(&tval_Init, NULL);
	}
	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[17].ID,srcPath,"mtxAdd");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[17].ID, DimsMAdd, globalDimsMAdd, localDimsMAdd, "%d %T %T", HALF_SIZE, TRAY0,TRAY1);
	err|=OMPI_XclWaitFor(1, &task[17].ID, MPI_COMM_WORLD);

	if(myRank==0){
			gettimeofday(&tval_End, NULL);
			timersub(&tval_End, &tval_Init, &tval_FinalSumCost);
			secs = (double) tval_FinalSumCost.tv_sec;
			mils = (double) (tval_FinalSumCost.tv_usec) / 1000000;
			printf("Final Sum COST: %f\n", secs + mils);
	}


	if(myRank==0){
		OMPI_XclReadTaskBuffer(task[17].ID, TRAY0, subMtxMemSz, M[0], MPI_COMM_WORLD);
		printf("%f--\n",M[0][0]);
		printf("%f--\n",M[0][1]);
		printf("%f--\n",M[0][subMtxSz-3]);
		printf("%f--\n",M[0][subMtxSz-2]);
		printf("%f--\n",M[0][subMtxSz-1]);
	}


//Freeing memory

//if(myRank==1){
	for(i=0;i<7;i++)
		free(M[i]);
	free(M);
//}

if(myRank==0){
	free(A11);
	free(A12);
	free(A21);
	free(A22);

	free(B11);
	free(B12);
	free(B21);
	free(B22);
}
	MPI_Finalize();

}

