#include "common.h"

int err;
int size=1024;
struct timeval tval_before, tval_after, tval_result;
int numTasks;

int main(int argc, char* argv[])
{

	XSCALA_Initialize(argc,argv);

	XSCALA_GetNumTasks(&numTasks, MPI_COMM_WORLD);
	printf("numTasks=> %d\n",numTasks);

	createData();
	refactorData();


	initLap
	//Task-based matrix multiplication.
	MPI_Datatype Aentity;
	MPI_Type_vector(1, blockSize_A, blockSize_A, MPI_FLOAT, &Aentity);
	MPI_Type_commit(&Aentity);

	MPI_Datatype Bentity;
	MPI_Type_vector(1, blockSize_B, blockSize_B, MPI_FLOAT, &Bentity);
	MPI_Type_commit(&Bentity);

	MPI_Datatype Centity;
	MPI_Type_vector(1, blockSize_C, blockSize_C, MPI_FLOAT, &Centity);
	MPI_Type_commit(&Centity);

	MPI_Datatype Dentity;
	MPI_Type_vector(1, blockSize_C, blockSize_C, MPI_FLOAT, &Dentity);
	MPI_Type_commit(&Dentity);


	int szEntityA;
	int szEntityB;
	int szEntityC;
	int szEntityD;
	MPI_Type_size(Aentity, &szEntityA);
	MPI_Type_size(Bentity, &szEntityB);
	MPI_Type_size(Centity, &szEntityC);
	MPI_Type_size(Dentity, &szEntityD);


	int eAPerRank;
	int eBPerRank;
	int eCPerRank;
	int eDPerRank;

	int ePerTask=1;

	int TRAY, taskIdx;
	for(taskIdx=0;taskIdx<numTasks;taskIdx++){
		for(TRAY=0;TRAY < 5;TRAY++){
			err |= XSCALA_MallocTray(taskIdx, TRAY, szEntityA, MPI_COMM_WORLD);
		}
	}

	XSCALA_Scatter("dataFileA1.dat", Aentity, TRAY0, MPI_COMM_WORLD);
	XSCALA_Scatter("dataFileB1.dat", Bentity, TRAY1, MPI_COMM_WORLD);
	XSCALA_Scatter("dataFile_C.dat", Centity, TRAY2, MPI_COMM_WORLD);

	for(taskIdx=0;taskIdx<numTasks;taskIdx++){
		err = XSCALA_SetProcedure(MPI_COMM_WORLD,taskIdx,"matrixMult.cl","mtxMultShared");
	}


	int step;
	int Dims = 2;
	size_t globalDims[] = { size/sqrt(numTasks),size/sqrt(numTasks)};
	size_t localDims[] = {16, 16};

	int AentitySize;
	MPI_Type_size(Aentity, &AentitySize);
	int BentitySize;
	MPI_Type_size(Bentity, &BentitySize);


	for (step = 0 ; step < sqrt(numTasks); step++) {
		for (taskIdx = 0 ; taskIdx < numTasks; taskIdx++) {
			err |= XSCALA_WaitAllTasks(MPI_COMM_WORLD);
			err |= XSCALA_ExecTask(MPI_COMM_WORLD, taskIdx, Dims, globalDims, localDims,
				"%T %T %T %d %d ", 3*(step%2)+0, 3*(step%2)+1, 2, rPBlock_A, cPBlock_A);

			int srcATray =3*(step%2)+0;
			int destATray=3*((step+1)%2)+0;

			int srcBTray =3*(step%2)+1;
			int destBTray=3*((step+1)%2)+1;
			err |= XSCALA_SendRecv(myRight[taskIdx], srcATray ,taskIdx , destATray, AentitySize,(2*step)+1);
			err |= XSCALA_SendRecv(myDown[taskIdx], srcBTray ,taskIdx , destBTray, BentitySize, (2*step)+2);
		}

	}

	finiLap

		void * res=malloc(sizeof(float));

			XSCALA_ReadTray(TASK0, TRAY2, sizeof(float), res, MPI_COMM_WORLD);
		err |= XSCALA_WaitFor(1, (int[]){TASK0}, MPI_COMM_WORLD);

		printf(" \n C[1][1]= %f ", *(float*)res);


	XSCALA_Finalize();
	return 0;
}




