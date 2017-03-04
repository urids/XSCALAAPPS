/*
 * main.c
 *
 *  Created on: Nov 2, 2015
 *      Author: uriel
 */
#include "mpi.h"
#include "common.h"
int i,j;

#define BLOCK_SIZE size/2

int size;
int numTasks;
int main(int argc, char** argv){

	char* infArg;
	infArg = strtok(argv[6], "=");
	infArg = strtok(NULL, "= ");

	size = atoi(infArg);
	printf(" size: %d\n",size);

	int myRank,numRanks;

	task_t task[21];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);

	//createData();
	//refactorData();

	numTasks=OMPI_CollectTaskInfo(ALL_DEVICES, MPI_COMM_WORLD);
	//printf("num tasks: %d",numTasks );


	// start timing:
	struct timeval tval_intGlobal, tval_endGlobal, tval_result;
	if(myRank==0){
		gettimeofday(&tval_intGlobal, NULL);
	}

	int taskId;
	int trayId;
	int err=0;

	MPI_File File_A;
	MPI_File File_B;

	MPI_File_open(MPI_COMM_SELF,"dataFileA1.dat", MPI_MODE_RDONLY ,MPI_INFO_NULL, &File_A);
	MPI_File_open(MPI_COMM_SELF,"dataFileB1.dat", MPI_MODE_RDONLY ,MPI_INFO_NULL, &File_B);

	int sizeA=size * size * sizeof(float);
	int sizeB=size * size * sizeof(float);

	float* A = malloc(sizeA);
	float* B = malloc(sizeB);

	MPI_File_read(File_A,A,sizeA,MPI_FLOAT,MPI_STATUSES_IGNORE);
	MPI_File_read(File_B,B,sizeB,MPI_FLOAT,MPI_STATUSES_IGNORE);


	float *a00=(A+0*(BLOCK_SIZE*BLOCK_SIZE));
	float *a01=(A+1*(BLOCK_SIZE*BLOCK_SIZE));
	float *a10=(A+2*(BLOCK_SIZE*BLOCK_SIZE));
	float *a11=(A+3*(BLOCK_SIZE*BLOCK_SIZE));

	float *b00=(B+0*(BLOCK_SIZE*BLOCK_SIZE));
	float *b01=(B+1*(BLOCK_SIZE*BLOCK_SIZE));
	float *b10=(B+2*(BLOCK_SIZE*BLOCK_SIZE));
	float *b11=(B+3*(BLOCK_SIZE*BLOCK_SIZE));

	int Dims = 2;
	size_t HALF_SIZE=(size_t)(size)/2;
	size_t globalDims[] = { HALF_SIZE,HALF_SIZE };
	size_t localDims[] = { 16,16 };

#define TS 16
#define WPT 2

	//size_t globalDimsM[] = { HALF_SIZE,HALF_SIZE/WPT };
	//size_t localDimsM[] = { 16,16/WPT };

	size_t globalDimsM[] = { HALF_SIZE,HALF_SIZE/WPT };
	size_t localDimsM[] = { TS,TS/WPT };

	size_t globalDims6[] = { HALF_SIZE/WPT,HALF_SIZE/WPT };
	size_t localDims6[] = { TS/WPT,TS/WPT };

	int DimsA2 = 1;
	size_t globalDimsA2[] = { HALF_SIZE};
	size_t localDimsA2[] = { 16 };

//	size_t globalDims3[] = { HALF_SIZE,HALF_SIZE/128 };
//	size_t localDims3[] = { 16,1 };


	for(i=0;i<21;i++){
		task[i].ID=i;
		task[i].ProcPath="/home/uriel/Dev/mpiApps/extnsApps/StrassenWithMPIXtns/src/procedures.cl";
	}

/*		task[0].ID=0;
		task[1].ID=6;
		task[2].ID=9;
		task[3].ID=11;
		task[4].ID=14;
		task[5].ID=17;
		task[6].ID=19;
		task[7].ID=1;
		task[8].ID=2;
		task[9].ID=3;
		task[10].ID=4;
		task[11].ID=7;
		task[12].ID=10;
		task[13].ID=12;
		task[14].ID=15;
		task[15].ID=18;
		task[16].ID=20;
		task[17].ID=5;
		task[18].ID=8;
		task[19].ID=13;
		task[20].ID=16;
*/


	int SubMtxSz=BLOCK_SIZE*BLOCK_SIZE*sizeof(float);


	for(taskId=0;taskId<10;taskId++){
		for(trayId=0;trayId<3;trayId++){
			err |= OMPI_XclMallocTray(task[taskId].ID, trayId, SubMtxSz, MPI_COMM_WORLD);
		}
	}

	err|=OMPI_XclWriteTray(task[0].ID, TRAY1, SubMtxSz, a00, MPI_COMM_WORLD);
	err|=OMPI_XclWriteTray(task[0].ID, TRAY2, SubMtxSz, a11, MPI_COMM_WORLD);

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[0].ID,task[0].ProcPath,"mtxAdd2");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[0].ID, DimsA2, globalDimsA2, localDimsA2, "%T %T %T ",  TRAY0,  TRAY1,  TRAY2);

	err|=OMPI_XclWriteTray(task[1].ID, TRAY1, SubMtxSz, b00, MPI_COMM_WORLD);
	err|=OMPI_XclWriteTray(task[1].ID, TRAY2, SubMtxSz, b11, MPI_COMM_WORLD);

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[1].ID,task[1].ProcPath,"mtxAdd");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[1].ID, Dims, globalDims, localDims, "%T %T %T ",  TRAY0,  TRAY1,  TRAY2);

	err|=OMPI_XclWriteTray(task[2].ID, TRAY1, SubMtxSz, a10, MPI_COMM_WORLD);
	err|=OMPI_XclWriteTray(task[2].ID, TRAY2, SubMtxSz, a11, MPI_COMM_WORLD);
	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[2].ID,task[2].ProcPath,"mtxAdd");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[2].ID, Dims, globalDims, localDims, "%T %T %T ",  TRAY0,  TRAY1,  TRAY2);

	err|=OMPI_XclWriteTray(task[3].ID, TRAY1, SubMtxSz, b01, MPI_COMM_WORLD);
	err|=OMPI_XclWriteTray(task[3].ID, TRAY2, SubMtxSz, b11, MPI_COMM_WORLD);

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[3].ID,task[3].ProcPath,"mtxSubs");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[3].ID, Dims, globalDims, localDims, "%T %T %T ",  TRAY0,  TRAY1,  TRAY2);

	err|=OMPI_XclWriteTray(task[4].ID, TRAY1, SubMtxSz, b10, MPI_COMM_WORLD);
	err|=OMPI_XclWriteTray(task[4].ID, TRAY2, SubMtxSz, b00, MPI_COMM_WORLD);

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[4].ID,task[4].ProcPath,"mtxSubs");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[4].ID, Dims, globalDims, localDims, "%T %T %T ",  TRAY0,  TRAY1,  TRAY2);

	err|=OMPI_XclWriteTray(task[5].ID, TRAY1, SubMtxSz, a00, MPI_COMM_WORLD);
	err|=OMPI_XclWriteTray(task[5].ID, TRAY2, SubMtxSz, a01, MPI_COMM_WORLD);
	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[5].ID,task[5].ProcPath,"mtxAdd");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[5].ID, Dims, globalDims, localDims, "%T %T %T ",  TRAY0,  TRAY1,  TRAY2);

	err|=OMPI_XclWriteTray(task[6].ID, TRAY1, SubMtxSz, a10, MPI_COMM_WORLD);
	err|=OMPI_XclWriteTray(task[6].ID, TRAY2, SubMtxSz, a00, MPI_COMM_WORLD);
	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[6].ID,task[6].ProcPath,"mtxSubs");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[6].ID, Dims, globalDims, localDims, "%T %T %T ",  TRAY0,  TRAY1,  TRAY2);

	err|=OMPI_XclWriteTray(task[7].ID, TRAY1, SubMtxSz, b00, MPI_COMM_WORLD);
	err|=OMPI_XclWriteTray(task[7].ID, TRAY2, SubMtxSz, b01, MPI_COMM_WORLD);
	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[7].ID,task[7].ProcPath,"mtxAdd");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[7].ID, Dims, globalDims, localDims, "%T %T %T ",  TRAY0,  TRAY1,  TRAY2);

	err|=OMPI_XclWriteTray(task[8].ID, TRAY1, SubMtxSz, a01, MPI_COMM_WORLD);
	err|=OMPI_XclWriteTray(task[8].ID, TRAY2, SubMtxSz, a11, MPI_COMM_WORLD);
	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[8].ID,task[8].ProcPath,"mtxSubs");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[8].ID, Dims, globalDims, localDims, "%T %T %T ",  TRAY0,  TRAY1,  TRAY2);

	err|=OMPI_XclWriteTray(task[9].ID, TRAY1, SubMtxSz, b10, MPI_COMM_WORLD);
	err|=OMPI_XclWriteTray(task[9].ID, TRAY2, SubMtxSz, b11, MPI_COMM_WORLD);
	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[9].ID,task[9].ProcPath,"mtxAdd");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[9].ID, Dims, globalDims, localDims, "%T %T %T ",  TRAY0,  TRAY1,  TRAY2);

	static int taskDeps[10];
	for (i=0;i<10;i++){
		taskDeps[i]=task[i].ID;
	}

	OMPI_XclWaitFor(10, taskDeps, MPI_COMM_WORLD);

	for(i=0;i<10;i++){
		err|=OMPI_XclFreeTray(task[i].ID, TRAY1, MPI_COMM_WORLD);
		err|=OMPI_XclFreeTray(task[i].ID, TRAY2, MPI_COMM_WORLD);
	}


	//matrix multiplications step

	for(i=10;i<17;i++){
		for(trayId=0;trayId<3;trayId++){
			err |= OMPI_XclMallocTray(task[i].ID, trayId, SubMtxSz, MPI_COMM_WORLD );
		}
	}

	int SubMtxCnt=BLOCK_SIZE*BLOCK_SIZE;

	OMPI_XclSendRecv(task[0].ID, TRAY0, task[10].ID, TRAY1, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[1].ID, TRAY0, task[10].ID, TRAY2, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);

	OMPI_XclSendRecv(task[2].ID, TRAY0, task[11].ID, TRAY1, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);
	err|=OMPI_XclWriteTray(task[11].ID, TRAY2, SubMtxSz, b00, MPI_COMM_WORLD);

	err|=OMPI_XclWriteTray(task[12].ID, TRAY1, SubMtxSz, a00, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[3].ID, TRAY0, task[12].ID, TRAY2, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);

	err|=OMPI_XclWriteTray(task[13].ID, TRAY1, SubMtxSz, a11, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[4].ID, TRAY0, task[13].ID, TRAY2, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);



	OMPI_XclSendRecv(task[5].ID, TRAY0, task[14].ID, TRAY1, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);
	err|=OMPI_XclWriteTray(task[14].ID, TRAY2, SubMtxSz, b11, MPI_COMM_WORLD);

	OMPI_XclSendRecv(task[6].ID, TRAY0, task[15].ID, TRAY1, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[7].ID, TRAY0, task[15].ID, TRAY2, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);

	OMPI_XclSendRecv(task[8].ID, TRAY0, task[16].ID, TRAY1, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[9].ID, TRAY0, task[16].ID, TRAY2, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[13].ID,task[13].ProcPath,"myGEMM3");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[13].ID, Dims, globalDimsM, localDimsM, " %d %d %d %T %T %T", HALF_SIZE, HALF_SIZE,HALF_SIZE, TRAY1,  TRAY2 ,TRAY0);

	//OMPI_XclWaitFor(1, &task[13].ID, MPI_COMM_WORLD);


	for(i=10;i<17;i++){
		//err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,/*task[i].ID,task[i].ProcPath,"mtxMultShared");
		//err|=OMPI_XclExecTask(MPI_COMM_SELF, task[i].ID, Dims, globalDims, localDims, "%T %T %T %d %d",  TRAY0,  TRAY1 ,TRAY2, HALF_SIZE,HALF_SIZE);

		err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[i].ID,task[i].ProcPath,"myGEMM3");
		err|=OMPI_XclExecTask(MPI_COMM_SELF, task[i].ID, Dims, globalDimsM, localDimsM, " %d %d %d %T %T %T", HALF_SIZE, HALF_SIZE,HALF_SIZE, TRAY1,  TRAY2 ,TRAY0);

		//err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[i].ID,task[i].ProcPath,"myGEMM6");
		//err|=OMPI_XclExecTask(MPI_COMM_SELF, task[i].ID, Dims, globalDims6, localDims6, " %d %d %d %T %T %T", HALF_SIZE, HALF_SIZE,HALF_SIZE, TRAY1,  TRAY2 ,TRAY0);

		//err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[i].ID,task[i].ProcPath,"gemm_nn");
		//err|=OMPI_XclExecTask(MPI_COMM_SELF, task[i].ID, Dims, globalDims3, localDims3, "%T %d %T %d %T %d %d %f %f", TRAY1, stride, TRAY2, stride ,TRAY0, stride, HALF_SIZE, alpha,beta);
	}

	static int taskmulDps[7];
	for (i=10;i<17;i++){
		taskmulDps[i-10]=task[i].ID;
		}
	OMPI_XclWaitFor(7, taskmulDps, MPI_COMM_WORLD);

/*	if(myRank==2){
			float *res=malloc(SubMtxSz);
			OMPI_XclReadTray(task[13].ID, TRAY0, SubMtxSz, res, MPI_COMM_WORLD);

			printf("%f--\n",res[0]);
			printf("%f--\n",res[1]);
			printf("%f--\n",res[BLOCK_SIZE*BLOCK_SIZE-3]);
			printf("%f--\n",res[BLOCK_SIZE*BLOCK_SIZE-2]);
			printf("%f--\n",res[BLOCK_SIZE*BLOCK_SIZE-1]);
		}*/


	for(i=10;i<17;i++){
		err|=OMPI_XclFreeTray(task[i].ID, TRAY1, MPI_COMM_WORLD);
		err|=OMPI_XclFreeTray(task[i].ID, TRAY2, MPI_COMM_WORLD);
	}


	//Conquer Steps.

	for(trayId=0;trayId<5;trayId++){
		err |= OMPI_XclMallocTray(task[17].ID, trayId, SubMtxSz, MPI_COMM_WORLD );
		err |= OMPI_XclMallocTray(task[20].ID, trayId, SubMtxSz, MPI_COMM_WORLD );
	}

	for(trayId=0;trayId<3;trayId++){
		err |= OMPI_XclMallocTray(task[18].ID, trayId, SubMtxSz, MPI_COMM_WORLD );
		err |= OMPI_XclMallocTray(task[19].ID, trayId, SubMtxSz, MPI_COMM_WORLD );
	}


	OMPI_XclSendRecv(task[10].ID, TRAY0, task[17].ID, TRAY1, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[13].ID, TRAY0, task[17].ID, TRAY2, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[14].ID, TRAY0, task[17].ID, TRAY3, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[16].ID, TRAY0, task[17].ID, TRAY4, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);

	OMPI_XclSendRecv(task[12].ID, TRAY0, task[18].ID, TRAY1, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[14].ID, TRAY0, task[18].ID, TRAY2, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);

	OMPI_XclSendRecv(task[11].ID, TRAY0, task[19].ID, TRAY1, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[13].ID, TRAY0, task[19].ID, TRAY2, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);

	OMPI_XclSendRecv(task[10].ID, TRAY0, task[20].ID, TRAY1, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[11].ID, TRAY0, task[20].ID, TRAY2, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[12].ID, TRAY0, task[20].ID, TRAY3, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);
	OMPI_XclSendRecv(task[15].ID, TRAY0, task[20].ID, TRAY4, SubMtxCnt, MPI_FLOAT, MPI_COMM_WORLD);


	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[17].ID,task[17].ProcPath,"mtxConquer");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[17].ID, Dims, globalDims, localDims, "%T %T %T %T %T",  TRAY0, TRAY1 ,TRAY2, TRAY3, TRAY4);

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[18].ID,task[18].ProcPath,"mtxAdd");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[18].ID, Dims, globalDims, localDims, "%T %T %T",  TRAY0, TRAY1 ,TRAY2);

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[19].ID,task[19].ProcPath,"mtxAdd");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[19].ID, Dims, globalDims, localDims, "%T %T %T",  TRAY0, TRAY1 ,TRAY2);

	err|=OMPI_XclSetProcedure(MPI_COMM_WORLD,task[20].ID,task[20].ProcPath,"mtxConquer");
	err|=OMPI_XclExecTask(MPI_COMM_SELF, task[20].ID, Dims, globalDims, localDims, "%T %T %T %T %T",  TRAY0, TRAY1 ,TRAY3, TRAY2, TRAY4);

	static int conquerDps[4];//={18,19};
	for (i=17;i<21;i++){
		conquerDps[i-17]=task[i].ID;
		}
	OMPI_XclWaitFor(4, conquerDps, MPI_COMM_WORLD);

	// end timing:
	if (myRank == 0) {
		double Gflops;
		gettimeofday(&tval_endGlobal, NULL );
		timersub(&tval_endGlobal, &tval_intGlobal, &tval_result);
		double secs = (double) tval_result.tv_sec;
		double mils = (double) (tval_result.tv_usec) / 1000000;
		//printf("%f\n", secs + mils);
		Gflops = (double) (7*(2 * pow(HALF_SIZE, 3))+(14 * pow(HALF_SIZE, 2))) / ((secs + mils) * pow(1024, 3));
		printf("\n time: %ld.%06ld\n", (long int) tval_result.tv_sec,
				(long int) tval_result.tv_usec);
		printf("performance: %f  Gflops", Gflops);

		FILE* resultsFp;
		resultsFp = fopen("resultsStrassenMtxMult.txt", "a");
		fprintf(resultsFp, "%ld.%06ld ",(long int) tval_result.tv_sec,(long int) tval_result.tv_usec);
		fclose(resultsFp);
	}

	if(myRank==0){
		float *res=malloc(SubMtxSz);
		OMPI_XclReadTray(task[18].ID, TRAY0, SubMtxSz, res, MPI_COMM_WORLD);

		printf("%f--\n",res[0]);
		printf("%f--\n",res[1]);
		printf("%f--\n",res[BLOCK_SIZE*BLOCK_SIZE-3]);
		printf("%f--\n",res[BLOCK_SIZE*BLOCK_SIZE-2]);
		printf("%f--\n",res[BLOCK_SIZE*BLOCK_SIZE-1]);
	}
	MPI_Finalize();
	return 0;
}




