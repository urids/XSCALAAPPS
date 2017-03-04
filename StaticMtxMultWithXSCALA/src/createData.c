#include "common.h"

float myrand(float randMax, float randMin) {
	float result;
	result = (float) rand() / (float) RAND_MAX;
	return ((1.0f - result) * randMin + result * randMax);
}

void createData(){

	int i;
	MPI_File dataFile_A,dataFile_B,dataFile_C,dataFile_D;
	const char* fileA_Name="dataFile_A.dat";
	const char* fileB_Name="dataFile_B.dat";
	const char* fileC_Name="dataFile_C.dat";
	MPI_File_open(MPI_COMM_SELF,fileA_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile_A);
	MPI_File_open(MPI_COMM_SELF,fileB_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile_B);
	MPI_File_open(MPI_COMM_SELF,fileC_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile_C);

	float* A=malloc(rowsA*colsA*sizeof(float));
	float* B=malloc(rowsB*colsB*sizeof(float));
	float* C=malloc(rowsC*colsC*sizeof(float));

	for(i=0;i<rowsA;i++){
		for(j=0;j<colsA;j++){
			A[colsA*i+j]=colsA*i+j;
			//A[colsA*i+j]=1;
		}
	}

	for(i=0;i<rowsB;i++){
			for(j=0;j<colsB;j++){
				B[colsB*i+j]=1;
			}
		}

	for(i=0;i<rowsC;i++){
			for(j=0;j<colsC;j++){
				C[colsC*i+j]=0;
			}
		}



	MPI_File_write(dataFile_A, A, rowsA*colsA, MPI_FLOAT, MPI_STATUS_IGNORE );
	MPI_File_write(dataFile_B, B, rowsB*colsB, MPI_FLOAT, MPI_STATUS_IGNORE );
	MPI_File_write(dataFile_C, C, rowsC*colsC, MPI_FLOAT, MPI_STATUS_IGNORE );

	MPI_File_close(&dataFile_A);
	MPI_File_close(&dataFile_B);
	MPI_File_close(&dataFile_C);


}
