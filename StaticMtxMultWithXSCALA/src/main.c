#include "common.h"
#define BLOCK_SIZE size/2
#define VALIDATE_RESULTS

int err;
int size;

int main(int argc, char* argv[])
{

	size=1024;
	int devKind=0;

	int myRank, numRanks;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);

	int numTasks=8;
	task_t* myTasks=malloc(numTasks*sizeof(task_t));
	XSCALA_InitTasks(argc,argv,&myTasks, 8);

	createData();
	refactorData();

	//Start TIMING.
	struct timeval tval_before, tval_after, tval_result;
	if(myRank==0){
		gettimeofday(&tval_before, NULL);
	}

	char* kernelPath="/home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/src/matrixMult.cl";

		MPI_Datatype Aentity;
		MPI_Type_vector(1, blockSize_A, blockSize_A, MPI_FLOAT, &Aentity);
		MPI_Type_commit(&Aentity);

		MPI_Datatype Bentity;
		MPI_Type_vector(1, blockSize_B, blockSize_B, MPI_FLOAT, &Bentity);
		MPI_Type_commit(&Bentity);

		MPI_Datatype Centity;
		MPI_Type_vector(1, blockSize_C, blockSize_C, MPI_FLOAT, &Centity);
		MPI_Type_commit(&Centity);



		int szEntityA;
		int szEntityB;
		int szEntityC;

		MPI_Type_size(Aentity, &szEntityA);
		MPI_Type_size(Bentity, &szEntityB);
		MPI_Type_size(Centity, &szEntityC);


		int TRAY, TASKID;
		for(TASKID=0;TASKID<numTasks;TASKID++){
			for(TRAY=0;TRAY<5;TRAY++){
				err |= OMPI_XclMallocTray(TASKID, TRAY, szEntityA, MPI_COMM_WORLD);
			}
		}

		MPI_File File_A;
		MPI_File File_B;

		MPI_File_open(MPI_COMM_SELF,"dataFileA1.dat", MPI_MODE_RDONLY ,MPI_INFO_NULL, &File_A);
		MPI_File_open(MPI_COMM_SELF,"dataFileB1.dat", MPI_MODE_RDONLY ,MPI_INFO_NULL, &File_B);

		int sizeA=size * size * sizeof(float);
		int sizeB=size * size * sizeof(float);

		float* A = malloc(sizeA);
		float* B = malloc(sizeB);
		float* C = calloc(size * size, sizeof(float));

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


		int SubMtxSz=BLOCK_SIZE*BLOCK_SIZE*sizeof(float);

		err|=OMPI_XclWriteTray(TASK0, TRAY2, SubMtxSz, C, MPI_COMM_WORLD);
		err|=OMPI_XclWriteTray(TASK4, TRAY2, SubMtxSz, C, MPI_COMM_WORLD);


		err|=OMPI_XclWriteTray(TASK0, TRAY0, SubMtxSz, a00, MPI_COMM_WORLD);
		err|=OMPI_XclWriteTray(TASK0, TRAY1, SubMtxSz, b00, MPI_COMM_WORLD);

		err|=OMPI_XclWriteTray(TASK1, TRAY0, SubMtxSz, a01, MPI_COMM_WORLD);
		err|=OMPI_XclWriteTray(TASK1, TRAY1, SubMtxSz, b01, MPI_COMM_WORLD);

		err|=OMPI_XclWriteTray(TASK2, TRAY0, SubMtxSz, a10, MPI_COMM_WORLD);
		err|=OMPI_XclWriteTray(TASK2, TRAY1, SubMtxSz, b10, MPI_COMM_WORLD);

		err|=OMPI_XclWriteTray(TASK3, TRAY0, SubMtxSz, a11, MPI_COMM_WORLD);
		err|=OMPI_XclWriteTray(TASK3, TRAY1, SubMtxSz, b11, MPI_COMM_WORLD);



	for(TASKID=0;TASKID<numTasks;TASKID++){
		err = OMPI_XclSetProcedure(MPI_COMM_WORLD,TASKID,kernelPath,"mtxMultShared");
	}


	int step;
	int Dims = 2;
	size_t globalDims[] = {size/2,size/2};
	size_t localDims[] = {16, 16};

	int AentitySize;
	MPI_Type_size(Aentity, &AentitySize);
	int BentitySize;
	MPI_Type_size(Bentity, &BentitySize);


	for (TASKID = 0 ; TASKID < 4; TASKID++) {
		err |= OMPI_XclExecTask(MPI_COMM_WORLD, TASKID, Dims, globalDims, localDims,
				"%T %T %T %d %d ", 0, 1, 2, rPBlock_A, cPBlock_A);
	}

	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);


	for (TASKID = 0 ; TASKID < 4; TASKID++) {
		err |= OMPI_XclSendRecv(TASKID, 2 ,TASKID+4 , 2, AentitySize,TASKID+1);
		err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);
	}


	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);

	err |= OMPI_XclSendRecv(0, 0 ,5 , 0, AentitySize,4);
	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);
	err |= OMPI_XclSendRecv(0, 1 ,6 , 1, AentitySize,5);
	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);

	err |= OMPI_XclSendRecv(3, 0 ,5 , 0, AentitySize,6);
	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);
	err |= OMPI_XclSendRecv(3, 1 ,6 , 1, AentitySize,7);
	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);


	err |= OMPI_XclSendRecv(1, 0 ,4 , 0, AentitySize,8);
	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);
	err |= OMPI_XclSendRecv(2, 1 ,4 , 1, AentitySize,9);
	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);

	err |= OMPI_XclSendRecv(1, 1 ,7 , 1, AentitySize,10);
	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);
	err |= OMPI_XclSendRecv(2, 1 ,7 , 1, AentitySize,11);
	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);

	for (TASKID = 4 ; TASKID < 8; TASKID++) {
		err |= OMPI_XclExecTask(MPI_COMM_WORLD, TASKID, Dims, globalDims, localDims,
				"%T %T %T %d %d ", 0, 1, 2, rPBlock_A, cPBlock_A);
	}
	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);

/*	for (step = 0 ; step < sqrt(numTasks); step++) {
		for (TASKID = 0 ; TASKID < numTasks; TASKID++) {

			err |= OMPI_XclExecTask(MPI_COMM_WORLD, TASKID, Dims, globalDims, localDims,
					"%T %T %T %d %d ", 3*(step%2)+0, 3*(step%2)+1, 2, rPBlock_A, cPBlock_A);

			int srcATray =3*(step%2)+0;
			int destATray=3*((step+1)%2)+0;

			int srcBTray =3*(step%2)+1;
			int destBTray=3*((step+1)%2)+1;

			err |= OMPI_XclSendRecv(myRight[TASKID], srcATray ,TASKID , destATray, AentitySize,(2*step)+1);
			err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);
			err |= OMPI_XclSendRecv(myDown[TASKID], srcBTray ,TASKID , destBTray, BentitySize, (2*step)+2);

		}
		err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);
	}
*/

#include "PerformanceReults.h"
	//validate the results
#ifdef VALIDATE_RESULTS
		void * res=malloc(sizeof(float));
		if(myRank==0)
			OMPI_XclReadTray(TASK4, TRAY2, sizeof(float), res, MPI_COMM_WORLD);
		err |= OMPI_XclWaitFor(1, (int[]){TASK4}, MPI_COMM_WORLD);
		if(myRank==0)
		printf(" \n res =  %f ", *(float*)res);
#endif

	MPI_Finalize();
	return 0;
}




