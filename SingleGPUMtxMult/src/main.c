#include "common.h"

int err;
int size;

int main(int argc, char* argv[])
{
	char* sz;
		//printf("%s",argv[5]);
		sz=strtok(argv[5],"=");
		sz=strtok(NULL,"=");
		size=atoi(sz);

	int myRank, numRanks;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);

    numTasks=OMPI_collectDevicesInfo(GPU_DEVICES, MPI_COMM_WORLD);

    //printf("myRank: %d\n",myRank);
	createData();
	refactorData();

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


//SINGLE GPU EXECUTION:

		struct timeval tval_before, tval_after, tval_result;
		if (myRank == 0) {
			gettimeofday(&tval_before, NULL );
		}

		MPI_File dataFile_Ax;
		MPI_File dataFile_Bx;
		MPI_File dataFile_Cx;
		void * hA=malloc(szEntityA);
		void * hB=malloc(szEntityB);
		void * hC=malloc(szEntityC);
		MPI_Status status;

		MPI_File_open(MPI_COMM_WORLD, "dataFileA1.dat", MPI_MODE_RDONLY, MPI_INFO_NULL,
							&dataFile_Ax);
		MPI_File_open(MPI_COMM_WORLD, "dataFileB1.dat", MPI_MODE_RDONLY, MPI_INFO_NULL,
					&dataFile_Bx);
		MPI_File_open(MPI_COMM_WORLD, "dataFile_C.dat", MPI_MODE_RDONLY, MPI_INFO_NULL,
							&dataFile_Cx);

		MPI_File_read(dataFile_Ax, hA, 1, Aentity, &status);
		MPI_File_read(dataFile_Bx, hB, 1, Bentity, &status);
		MPI_File_read(dataFile_Cx, hC, 1, Centity, &status);
		int i,j,k;


		err  = OMPI_XclCreateKernel(MPI_COMM_WORLD,"/home/uriel/Dev/mpiApps/extnsApps/MtxMultWithMPIXtns/src/matrixMult.cl","mtxMultShared");

		int Dims = 2;
		size_t globalDims[] = { size / 4, size / 4 };
		size_t localDims[] = { 16, 16 };

		void *tmpA=hA;
		void *tmpB=hB;
		void *tmpC=hC;
		void *dataRet = malloc(rPBlock_A*cPBlock_A*sizeof(float));


	int N=rowsAs;

	for(i=0;i<N;i++){
		for (j=0; j < N; j++) {
			//tmpC += (N*i+j)*(rPBlock_A*cPBlock_A*sizeof(float));
			OMPI_XclWriteTaskBuffer(0, 2,  szEntityC, hC, MPI_COMM_WORLD );

			for(k=0;k<N;k++){

				//tmpA += (N*i+k)*(rPBlock_A*cPBlock_A*sizeof(float));
				//tmpB += (N*k+j)*(rPBlock_A*cPBlock_A*sizeof(float));
				OMPI_XclWriteTaskBuffer(0, 0, szEntityA, hA, MPI_COMM_WORLD );
				OMPI_XclWriteTaskBuffer(0, 1, szEntityA, hB, MPI_COMM_WORLD );

				err |= OMPI_XclExecKernel(MPI_COMM_SELF, 0, Dims, globalDims,
						localDims, "%T %T %T %d %d ", 0, 1, 2, rPBlock_A,
						cPBlock_A);
				err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD );

				//tmpA=hA;
				//tmpB=hB;

			}
			err |= OMPI_XclReadTaskBuffer(0, 2,  szEntityA, dataRet,MPI_COMM_WORLD );
			printf(" %f \n", ((float*)dataRet)[0]);
			tmpC=hC;


		}

	}


	if(myRank==0){
				double Gflops;
				gettimeofday(&tval_after, NULL);
				timersub(&tval_after, &tval_before, &tval_result);
				double secs=(double)tval_result.tv_sec;
				double mils=(double)(tval_result.tv_usec)/1000000;
				printf("%f\n",secs+mils);
				Gflops=(double)(2*pow(size, 3))/((secs+mils)*pow(1024,3));
				printf("time: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
				printf("performance: %f  Gflops",Gflops);


				FILE* resultsFp;
				resultsFp=fopen("1GPU.txt","a");
				fprintf(resultsFp,"%d %ld.%06ld	%f \n",size, (long int)tval_result.tv_sec, (long int)tval_result.tv_usec,Gflops);
				fclose(resultsFp);

	}



/*	void * dataRet =  malloc(ePerTask * szEntityC);
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




