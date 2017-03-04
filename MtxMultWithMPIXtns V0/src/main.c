#include "common.h"

int err;
int size;
int main(int argc, char* argv[])
{

	char* sz;
		int devKind;
		char* infArg;
		infArg = strtok(argv[6], "=");
		infArg = strtok(NULL, "= ");
		size = atoi(infArg);
		//printf(" \n %d",size);
		infArg = strtok(NULL, "= ");
		devKind=atoi(infArg);
	int myRank, numRanks;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);

    numBcks=OMPI_CollectTaskInfo(devKind, MPI_COMM_WORLD);

	createData();
	refactorData();

//task based matrix multiplication.
	struct timeval tval_before, tval_after, tval_result;
	if(myRank==0){
	gettimeofday(&tval_before, NULL);
	}

		MPI_Datatype Aentity;
		MPI_Type_vector(1, blockSize_A ,blockSize_A, MPI_FLOAT, &Aentity);
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

		 OMPI_XclScatter("dataFileA1.dat", &eAPerRank, Aentity, NULL,0,MPI_COMM_WORLD);
		 float * hA = (float *) malloc(eAPerRank * szEntityA);
		 OMPI_XclScatter("dataFileA1.dat", &eAPerRank, Aentity, hA, 0 , MPI_COMM_WORLD);

		 OMPI_XclScatter("dataFileB1.dat", &eBPerRank, Bentity, NULL, 1,MPI_COMM_WORLD);
		 float * hB = (float *) malloc(eBPerRank * szEntityB);
		 OMPI_XclScatter("dataFileB1.dat", &eBPerRank, Bentity, hB, 1,MPI_COMM_WORLD);

		 OMPI_XclScatter("dataFile_C.dat", &eCPerRank, Centity, NULL, 2,MPI_COMM_WORLD);
		 float * hC = (float *) malloc(eCPerRank * szEntityC);
		 OMPI_XclScatter("dataFile_C.dat", &eCPerRank, Centity, hC, 2,MPI_COMM_WORLD);

	/*	TODO: this slicing causes a bug !!!
	 *  OMPI_XclScatter("dataFile_D.dat", &eDPerRank, Dentity, NULL, 6,MPI_COMM_WORLD);
		 float * hD = (float *) malloc(eDPerRank * szEntityD);
		 OMPI_XclScatter("dataFile_D.dat", &eDPerRank, Dentity, hD, 6, MPI_COMM_WORLD);*/



		 err  = OMPI_XclSetProcedure(MPI_COMM_WORLD,"/home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/src/matrixMult.cl","mtxMultShared");

		 int step;
		 int taskIdx;

		 int Dims = 2;

		 size_t globalDims[] = { size/sqrt(numBcks),size/sqrt(numBcks) };
		 size_t localDims[] = { 16,16 };


		 for (step = 0; step < sqrt(numBcks); step++) {
			 //the task is requested only on the appropriate rank by our RT. =)
			 for (taskIdx = 0; taskIdx < numBcks; taskIdx++) {

			err |= OMPI_XclExecTask(MPI_COMM_SELF, taskIdx, Dims, globalDims, localDims,
					"%T %T %T %d %d ", 3*(step%2)+0, 3*(step%2)+1, 2, rPBlock_A, cPBlock_A);




			int srcATray =3*(step%2)+0;
			int destATray=3*((step+1)%2)+0;

			int srcBTray =3*(step%2)+1;
			int destBTray=3*((step+1)%2)+1;

			err |= OMPI_XclSendRecv(myRight[taskIdx], srcATray ,taskIdx , destATray, 1,
			 Aentity, MPI_COMM_WORLD );

			err |= OMPI_XclSendRecv(myDown[taskIdx], srcBTray ,taskIdx , destBTray, 1,
						 Bentity, MPI_COMM_WORLD );

		}
		err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD );
	}

	if (myRank == 0) {
		double Gflops;
		gettimeofday(&tval_after, NULL );
		timersub(&tval_after, &tval_before, &tval_result);
		double secs = (double) tval_result.tv_sec;
		double mils = (double) (tval_result.tv_usec) / 1000000;
		printf("%f\n", secs + mils);
		Gflops = (double) (2 * pow(size, 3)) / ((secs + mils) * pow(1024, 3));
		printf("time: %ld.%06ld\n", (long int) tval_result.tv_sec,
				(long int) tval_result.tv_usec);
		printf("performance: %f  Gflops", Gflops);

		FILE* resultsFp;
		resultsFp = fopen("resultsMtxMult.txt", "a");
		fprintf(resultsFp, "%f ",Gflops);
		fclose(resultsFp);

	 }

//finally we can perform the scalar-Mtx multiplication and the Mtx-Mtx addition.
/*
	err  = OMPI_XclCreateKernel(MPI_COMM_WORLD,"/home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/src/matrixMult.cl","mtxAdd");
	int Dims2 = 1;
	size_t globalDims2[] = { 4 };
	size_t localDims2[] = { 4 };

	for (taskIdx = 0; taskIdx < numTasks; taskIdx++) {
				err |= OMPI_XclExecKernel(MPI_COMM_SELF, taskIdx, Dims2, globalDims2, localDims2,
						"%T %T %d %d %d ", 2, 6, rPBlock_C, cPBlock_C, 3);
	}
	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD );
*/

	void * dataRet =  malloc(ePerTask * szEntityC);
	printf("\n");
	float* dataBuff = ((float*) dataRet);

	if(myRank==0){
	err |= OMPI_XclReadTaskBuffer(0, 2, ePerTask * szEntityC, dataRet, MPI_COMM_WORLD );
	printf(" %f ", dataBuff[0]);
	}


/*
	for (i = 0; i < rPBlock_C; i++) {
		for (j = 0; j < cPBlock_C; j++) {
			printf(" %f ", dataBuff[cPBlock_C * i + j]);
		}
		printf("\n");
	}
*/

	free(C);

	MPI_Finalize();
	return 0;
}




