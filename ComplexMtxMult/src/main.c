#include "common.h"


/*
#ifdef DOUBLE_P
#else
#endif
*/

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
		infArg = strtok(NULL, "= ");
		devKind=atoi(infArg);
	int myRank, numRanks;



	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);

   numBcks=OMPI_collectDevicesInfo(devKind, MPI_COMM_WORLD);
   printf("numTasks: %d , numHosts: %d \n",numBcks, numRanks);
	createData();

	MPI_File File_A1,File_A2;
	MPI_File File_B1,File_B2;

	/*	MPI_File_open(MPI_COMM_SELF,"Sar5000.dat", MPI_MODE_RDONLY ,MPI_INFO_NULL, &File_A1);
	MPI_File_open(MPI_COMM_SELF,"Sai5000.dat", MPI_MODE_RDONLY ,MPI_INFO_NULL, &File_A2);
	MPI_File_open(MPI_COMM_SELF,"Sbr5000.dat", MPI_MODE_RDONLY ,MPI_INFO_NULL, &File_B1);
	MPI_File_open(MPI_COMM_SELF,"Sbi5000.dat", MPI_MODE_RDONLY ,MPI_INFO_NULL, &File_B2);
	*/

	MPI_File_open(MPI_COMM_SELF,"A1.dat", MPI_MODE_RDONLY ,MPI_INFO_NULL, &File_A1);
	MPI_File_open(MPI_COMM_SELF,"A2.dat", MPI_MODE_RDONLY ,MPI_INFO_NULL, &File_A2);
	MPI_File_open(MPI_COMM_SELF,"B1.dat", MPI_MODE_RDONLY ,MPI_INFO_NULL, &File_B1);
	MPI_File_open(MPI_COMM_SELF,"B2.dat", MPI_MODE_RDONLY ,MPI_INFO_NULL, &File_B2);

#ifdef DOUBLE_P
	int sizeA1=rowsA1 * colsA1 * sizeof(double);
	int sizeA2=rowsA2 * colsA2 * sizeof(double);
	int sizeB1=rowsB1 * colsB1 * sizeof(double);
	int sizeB2=rowsB2 * colsB2 * sizeof(double);

	double* A1 = malloc(sizeA1);
	double* A2 = malloc(sizeA2);
	double* B1 = malloc(sizeB1);
	double* B2 = malloc(sizeB2);

#else
	int sizeA1=rowsA1 * colsA1 * sizeof(float);
	int sizeA2=rowsA2 * colsA2 * sizeof(float);
	int sizeB1=rowsB1 * colsB1 * sizeof(float);
	int sizeB2=rowsB2 * colsB2 * sizeof(float);

	float* A1 = malloc(sizeA1);
	float* A2 = malloc(sizeA2);
	float* B1 = malloc(sizeB1);
	float* B2 = malloc(sizeB2);

#endif



#ifdef DOUBLE_P
	void * dataRet =  malloc(sizeA1);
	double* dataBuff = ((double*) dataRet);
#else
	void * dataRet =  malloc(sizeA1);
	float* dataBuff = ((float*) dataRet);
#endif


/*	float** A=(float**)malloc(2*sizeof(float*));
	float** B=(float**)malloc(2*sizeof(float*));
	A[0]=A1;
	A[1]=A2;
	B[0]=B1;
	B[1]=B2;*/

// start timing:
	struct timeval tval_before, tval_after, tval_result;
	if(myRank==0){
	gettimeofday(&tval_before, NULL);
	}
//task based  complex matrix multiplication.

	//load Ai & Bi In Host memory.
#ifdef DOUBLE_P
	MPI_File_read(File_A1,A1,rowsA1*colsA1,MPI_DOUBLE,MPI_STATUSES_IGNORE);
	MPI_File_read(File_A2,A2,rowsA2*colsA2,MPI_DOUBLE,MPI_STATUSES_IGNORE);
	MPI_File_read(File_B1,B1,rowsB1*colsB1,MPI_DOUBLE,MPI_STATUSES_IGNORE);
	MPI_File_read(File_B2,B2,rowsB2*colsB2,MPI_DOUBLE,MPI_STATUSES_IGNORE);

#else
	MPI_File_read(File_A1,A1,rowsA1*colsA1,MPI_FLOAT,MPI_STATUSES_IGNORE);
	MPI_File_read(File_A2,A2,rowsA2*colsA2,MPI_FLOAT,MPI_STATUSES_IGNORE);
	MPI_File_read(File_B1,B1,rowsB1*colsB1,MPI_FLOAT,MPI_STATUSES_IGNORE);
	MPI_File_read(File_B2,B2,rowsB2*colsB2,MPI_FLOAT,MPI_STATUSES_IGNORE);

#endif

	//load Ai & Bi In Device memory.
	err=OMPI_XclWriteTaskBuffer(0,0,sizeA1,A1,MPI_COMM_WORLD);
	err|=OMPI_XclWriteTaskBuffer(0,1,sizeA2,A2,MPI_COMM_WORLD);
	err|=OMPI_XclMallocTaskBuffer(0,2, sizeA1, MPI_COMM_WORLD);

	err=OMPI_XclWriteTaskBuffer(1,0,sizeB1,B1,MPI_COMM_WORLD);
	err|=OMPI_XclWriteTaskBuffer(1,1,sizeB2,B2,MPI_COMM_WORLD);
	err|=OMPI_XclMallocTaskBuffer(1,2, sizeB1, MPI_COMM_WORLD);

	err=OMPI_XclWriteTaskBuffer(2,0,sizeB1,B1,MPI_COMM_WORLD);
	err|=OMPI_XclWriteTaskBuffer(2,1,sizeA2,A2,MPI_COMM_WORLD);
	err|=OMPI_XclMallocTaskBuffer(2,2, sizeA1, MPI_COMM_WORLD);

	err=OMPI_XclWriteTaskBuffer(3,0,sizeA1,A1,MPI_COMM_WORLD);
	err|=OMPI_XclWriteTaskBuffer(3,1,sizeB2,B2,MPI_COMM_WORLD);
	err|=OMPI_XclMallocTaskBuffer(3,2, sizeB1, MPI_COMM_WORLD);

#ifdef DOUBLE_P
	MPI_Datatype mtx;
	MPI_Type_vector(1, size*size, size*size, MPI_DOUBLE, &mtx);
	MPI_Type_commit(&mtx);
#else
	MPI_Datatype mtx;
	MPI_Type_vector(1, size*size, size*size, MPI_FLOAT, &mtx);
	MPI_Type_commit(&mtx);
#endif


	int Dims = 2;
	int appSize=((size-1)/16+1)*16; //The size of global threads must be multiple of 16.

	size_t globalDims[] = { appSize, appSize};
	size_t localDims[] = { 16, 16 };

	//matrix multiplication tasks procedure
	#ifdef DOUBLE_P
		err  = OMPI_XclCreateKernel(MPI_COMM_WORLD,"/home/uriel/Dev/mpiApps/extnsApps/ComplexMtxMult/src/matrixMult.cl","mtxMultSharedDoub");
	#else
		err  = OMPI_XclCreateKernel(MPI_COMM_WORLD,"/home/uriel/Dev/mpiApps/extnsApps/ComplexMtxMult/src/matrixMult.cl","mtxMultShared");
	#endif


	int taskIdx;
	for(taskIdx=0;taskIdx<4;taskIdx++){
		err |= OMPI_XclExecKernel(MPI_COMM_SELF, taskIdx, Dims, globalDims, localDims,
						"%T %T %T %d %d ", 0, 1, 2, size, size);
	}
	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD);


	//print multiplication step rsults.
/*	err |= OMPI_XclReadTaskBuffer(1, 2, sizeA1, dataBuff, MPI_COMM_WORLD);
	printf("--C1[0]= %lf \n", dataBuff[0]);
	printf("--C1[SIZE*(SIZE-1)+SIZE]= %lf \n", dataBuff[size*(size-1)+(size-1)]);
*/
	//free trays in tasks 0-3.
	for(taskIdx=0;taskIdx<4;taskIdx++){
		err |= OMPI_XclFreeTaskBuffer(taskIdx, 0, MPI_COMM_WORLD );
		//err |= OMPI_XclFreeTaskBuffer(taskIdx, 1, MPI_COMM_WORLD );
	}

	//matrix addition tasks
	#ifdef DOUBLE_P
		err|= OMPI_XclCreateKernel(MPI_COMM_WORLD,"/home/uriel/Dev/mpiApps/extnsApps/ComplexMtxMult/src/matrixMult.cl","mtxAddDoub");
	#else
		err|= OMPI_XclCreateKernel(MPI_COMM_WORLD,"/home/uriel/Dev/mpiApps/extnsApps/ComplexMtxMult/src/matrixMult.cl","mtxAdd");
	#endif


	err|=OMPI_XclSendRecv(1,2,0,1,1,mtx,MPI_COMM_WORLD);
	err|=OMPI_XclSendRecv(3,2,2,1,1,mtx,MPI_COMM_WORLD);

	//print copied data
/*	err |= OMPI_XclReadTaskBuffer(0, 1, sizeA1, dataBuff, MPI_COMM_WORLD);
		printf("C1[0]= %lf \n", dataBuff[0]);
		printf("C1[SIZE*(SIZE-1)+SIZE]= %lf \n", dataBuff[size*(size-1)+(size-1)]);
*/

	for(taskIdx=0;taskIdx<3;taskIdx+=2){
		err |= OMPI_XclExecKernel(MPI_COMM_SELF, taskIdx, Dims, globalDims, localDims,
							"%T %T %d %d ", 1, 2, size, size);
	}
	err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD );


// end timing:
	if (myRank == 0) {
		double Gflops;
		gettimeofday(&tval_after, NULL );
		timersub(&tval_after, &tval_before, &tval_result);
		double secs = (double) tval_result.tv_sec;
		double mils = (double) (tval_result.tv_usec) / 1000000;
		printf("%f\n", secs + mils);
		Gflops = (double) (4*(2 * pow(size, 3))+(2 * pow(size, 2))) / ((secs + mils) * pow(1024, 3));
		printf("time: %ld.%06ld\n", (long int) tval_result.tv_sec,
				(long int) tval_result.tv_usec);
		printf("performance: %f  Gflops", Gflops);

		FILE* resultsFp;
		resultsFp = fopen("resultsCplx.txt", "a");
		fprintf(resultsFp, "%f ",Gflops);
		fclose(resultsFp);

	 }

	printf("\n");

	if(myRank==0){
	    err |= OMPI_XclReadTaskBuffer(0, 1, sizeA1, dataRet, MPI_COMM_WORLD );
	    printf(" %f ", dataBuff[0]);
	}


	free(A1);
	free(A2);
	free(B1);
	free(B2);

	MPI_Finalize();
	return 0;
}




