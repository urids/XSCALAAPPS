#include "common.h"

void createData(){

	int i;
	MPI_File dataFile_A1,dataFile_B1;
	MPI_File dataFile_A2,dataFile_B2;
	//MPI_File dataFile_A3,dataFile_B3;

/*	MPI_File dataFile_T0,dataFile_T1;
	MPI_File dataFile_T2,dataFile_T3;*/

	const char* fileA1_Name="A1.dat";
	const char* fileA2_Name="A2.dat";
	//const char* fileA3_Name="A3.dat";
	const char* fileB1_Name="B1.dat";
	const char* fileB2_Name="B2.dat";
	//const char* fileB3_Name="B3.dat";
	/*const char* fileT0_Name="T0.dat";
	const char* fileT1_Name="T1.dat";
	const char* fileT2_Name="T2.dat";
	const char* fileT3_Name="T3.dat";*/

	MPI_File_open(MPI_COMM_SELF,fileA1_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile_A1);
	MPI_File_open(MPI_COMM_SELF,fileA2_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile_A2);
	//MPI_File_open(MPI_COMM_SELF,fileA3_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile_A3);

	MPI_File_open(MPI_COMM_SELF,fileB1_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile_B1);
	MPI_File_open(MPI_COMM_SELF,fileB2_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile_B2);
	//MPI_File_open(MPI_COMM_SELF,fileB3_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile_B3);

	/*MPI_File_open(MPI_COMM_SELF,fileT0_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile_T0);
	MPI_File_open(MPI_COMM_SELF,fileT1_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile_T1);
	MPI_File_open(MPI_COMM_SELF,fileT2_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile_T2);
	MPI_File_open(MPI_COMM_SELF,fileT3_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile_T3);
*/




#ifdef DOUBLE_P
	double* A1=malloc(rowsA1*colsA1*sizeof(double));
	double* A2=malloc(rowsA2*colsA2*sizeof(double));
	double* B1=malloc(rowsB1*colsB1*sizeof(double));
	double* B2=malloc(rowsB2*colsB2*sizeof(double));
#else
	float* A1=malloc(rowsA1*colsA1*sizeof(float));
	float* A2=malloc(rowsA2*colsA2*sizeof(float));
	float* B1=malloc(rowsB1*colsB1*sizeof(float));
	float* B2=malloc(rowsB2*colsB2*sizeof(float));
#endif



	for (i = 0; i < rowsA1; i++) {
		for (j = 0; j < colsA1; j++) {
			A1[colsA1 * i + j] =  colsA1 * i + j;
		}
	}

	for (i = 0; i < rowsA2; i++) {
		for (j = 0; j < colsA2; j++) {
			A2[colsA1 * i + j] = 1;
		}
	}

	for (i = 0; i < rowsB1; i++) {
		for (j = 0; j < colsB1; j++) {
			B1[colsA1 * i + j] = colsB1 * i + j;
		}
	}

	for (i = 0; i < rowsB2; i++) {
		for (j = 0; j < colsB2; j++) {
			B2[colsA1 * i + j] = 1;
		}
	}

#ifdef DOUBLE_P
	MPI_File_write(dataFile_A1, A1, rowsA1*colsA1, MPI_DOUBLE, MPI_STATUS_IGNORE );
	MPI_File_write(dataFile_A2, A2, rowsA2*colsA2, MPI_DOUBLE, MPI_STATUS_IGNORE );
	MPI_File_write(dataFile_B1, B1, rowsB1*colsB1, MPI_DOUBLE, MPI_STATUS_IGNORE );
	MPI_File_write(dataFile_B2, B2, rowsB2*colsB2, MPI_DOUBLE, MPI_STATUS_IGNORE );
#else
	MPI_File_write(dataFile_A1, A1, rowsA1*colsA1, MPI_FLOAT, MPI_STATUS_IGNORE );
	MPI_File_write(dataFile_A2, A2, rowsA2*colsA2, MPI_FLOAT, MPI_STATUS_IGNORE );
	MPI_File_write(dataFile_B1, B1, rowsB1*colsB1, MPI_FLOAT, MPI_STATUS_IGNORE );
	MPI_File_write(dataFile_B2, B2, rowsB2*colsB2, MPI_FLOAT, MPI_STATUS_IGNORE );
#endif

	MPI_File_close(&dataFile_A1);
	MPI_File_close(&dataFile_A2);
	MPI_File_close(&dataFile_B1);
	MPI_File_close(&dataFile_B2);

}
