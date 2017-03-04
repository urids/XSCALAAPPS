
#include "common.h"
int numTasks;

int rowsAs; /* number of rows in _partition_ */
int colsAs; /* number of cols in _partition_ */

int rPBlock_A; /* number of rows in a _block_ of A */
int cPBlock_A; /* number of cols in a _block_ of A */

int rPBlock_B; /* number of rows in a _block_ of A */
int cPBlock_B; /* number of cols in a _block_ of A */

int rPBlock_C; /* number of rows in a _block_ of A */
int cPBlock_C; /* number of cols in a _block_ of A */

int *myRight;
int *myDown;

int blockId;
int blockSize_A;
int blockSize_B;
int blockSize_C;

float* A;
float* B;
float* C;




void refactorData(){

//****//
	 rowsAs = 8; /* number of rows in _partition_ */
	 colsAs = rowsAs; /* number of cols in _partition_ */
//****//





	 rPBlock_A = ceil(rowsA / rowsAs); /* number of rows in a _block_ of A */
	 cPBlock_A = ceil(colsA / colsAs); /* number of cols in a _block_ of A */

	 rPBlock_B = ceil(rowsB / rowsAs); /* number of rows in a _block_ of A */
	 cPBlock_B = ceil(colsB / colsAs); /* number of cols in a _block_ of A */

	 rPBlock_C = ceil(rowsC / rowsAs); /* number of rows in a _block_ of A */
	 cPBlock_C = ceil(colsC / colsAs); /* number of cols in a _block_ of A */



	 A = malloc(rowsA * colsA * sizeof(float));
	 B = malloc(rowsB * colsB * sizeof(float));
	 C = malloc(rowsC * colsC * sizeof(float));



	MPI_File dataFile_A,dataFile_B,dataFile_C;
	MPI_Offset filesize_A;
	MPI_Status status;

	/* READ THE MATRICES A,B, and C.*/
	MPI_File_open(MPI_COMM_WORLD, "dataFile_A.dat", MPI_MODE_RDONLY, MPI_INFO_NULL,
			&dataFile_A);
	//MPI_File_get_size(dataFile_A, &filesize_A);
	MPI_File_read(dataFile_A, A, rowsA * colsA, MPI_FLOAT, &status);


	MPI_File_open(MPI_COMM_WORLD, "dataFile_B.dat", MPI_MODE_RDONLY, MPI_INFO_NULL,
			&dataFile_B);
	MPI_File_read(dataFile_B, B, rowsB * colsB, MPI_FLOAT, &status);

	MPI_File_open(MPI_COMM_WORLD, "dataFile_C.dat", MPI_MODE_RDONLY, MPI_INFO_NULL,
			&dataFile_C);
	MPI_File_read(dataFile_C, C, rowsC * colsC, MPI_FLOAT, &status);



	MPI_Datatype typeVectmp_A,typeVec_A;
	MPI_Type_vector(rPBlock_A,cPBlock_A,colsA,MPI_FLOAT,&typeVectmp_A);
	MPI_Type_create_resized(typeVectmp_A,0,sizeof(char),&typeVec_A);
	MPI_Type_commit(&typeVec_A);

	MPI_Datatype typeVectmp_B, typeVec_B;
	MPI_Type_vector(rPBlock_B, cPBlock_B, colsB, MPI_FLOAT, &typeVectmp_B);
	MPI_Type_create_resized(typeVectmp_B, 0, sizeof(char), &typeVec_B);
	MPI_Type_commit(&typeVec_B);

	MPI_Datatype typeVectmp_C, typeVec_C;
	MPI_Type_vector(rPBlock_C, cPBlock_C, colsC, MPI_FLOAT, &typeVectmp_C);
	MPI_Type_create_resized(typeVectmp_C, 0, sizeof(char), &typeVec_C);
	MPI_Type_commit(&typeVec_C);


	//here we compute the displacements to build the blocks.
	int disps_A[rowsAs*colsAs];
	int disps_B[rowsAs*colsAs];
	int disps_C[rowsAs*colsAs];
	for(i=0;i<rowsAs;i++){
		for(j=0;j<colsAs;j++){
			disps_A[colsAs*i+j]=i*colsA*rPBlock_A+j*cPBlock_A;
			disps_B[colsAs*i+j]=i*colsB*rPBlock_B+j*cPBlock_B;
			disps_C[colsAs*i+j]=i*colsC*rPBlock_C+j*cPBlock_C;
		}
	}


	//pttA is the same matrix stored in blocks.
	float *pttA = malloc(rowsA * colsA * sizeof(float));
	for (i = 0; i < rowsAs; i++) {
		for (j = 0; j < colsAs; j++) {
			MPI_Pack(A + disps_A[colsAs * i + j], 1, typeVec_A, pttA,
					rowsA * colsA * sizeof(float), disps_A, MPI_COMM_WORLD );
		}
	}
	free(A);

	//pttB is the same matrix than B stored in blocks.
	float *pttB = malloc(rowsB * colsB * sizeof(float));
	for (i = 0; i < rowsAs; i++) {
		for (j = 0; j < colsAs; j++) {
			MPI_Pack(B + disps_B[colsAs * i + j], 1, typeVec_B, pttB,
					rowsB * colsB * sizeof(float), disps_B, MPI_COMM_WORLD );
		}
	}
	free(B);

	//pttC is the same matrix than C stored in blocks.
	float *pttC = malloc(rowsC * colsC * sizeof(float));
	for (i = 0; i < rowsAs; i++) {
		for (j = 0; j < colsAs; j++) {
			MPI_Pack(C + disps_C[colsAs * i + j], 1, typeVec_C, pttC,
					rowsC * colsC * sizeof(float), disps_C, MPI_COMM_WORLD );
		}
	}



	//dataFileA1 is the permutation of matrix A stored in the precise order to do scatter.
	int myRow;
	int myCol;
	int *PA = malloc(sizeof(int)*numTasks);
	int *PB = malloc(sizeof(int)*numTasks);

	 myRight = malloc(sizeof(int)*numTasks);
	 myDown = malloc(sizeof(int)*numTasks);

	for(i=0;i<numTasks;i++){
		myRow = floor(i/colsAs);
		myCol = i-(myRow*colsAs);
		PA[i]=((i + myRow) % colsAs)+(myRow*colsAs);
		PB[i]=(i +(myCol*colsAs))%numTasks;

		myRight[i]=((i+1)%colsAs)+(myRow*colsAs);
		myDown[i]=(i+colsAs)%numTasks;
	}


	 blockSize_A=rPBlock_A*cPBlock_A;
	 blockSize_B=rPBlock_B*cPBlock_B;
	 blockSize_C=rPBlock_C*cPBlock_C;

	MPI_File dataFileA1;
	const char* fileA1_Name = "dataFileA1.dat";
	MPI_File_open(MPI_COMM_SELF, fileA1_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,
				MPI_INFO_NULL, &dataFileA1);

	for(i=0;i<rowsAs;i++){
			for(j=0;j<colsAs;j++){
				blockId=PA[colsAs*i+j];
				MPI_File_write(dataFileA1, pttA+(blockSize_A*blockId), rPBlock_A*cPBlock_A, MPI_FLOAT, MPI_STATUS_IGNORE);
			}
	}

	MPI_File dataFileB1;
	const char* fileB1_Name = "dataFileB1.dat";
	MPI_File_open(MPI_COMM_SELF, fileB1_Name, MPI_MODE_WRONLY | MPI_MODE_CREATE,
			MPI_INFO_NULL, &dataFileB1);

	for (i = 0; i < rowsAs; i++) {
		for (j = 0; j < colsAs; j++) {
			blockId = PB[colsAs * i + j];
			MPI_File_write(dataFileB1, pttB + (blockSize_B * blockId),
					rPBlock_B * cPBlock_B, MPI_FLOAT, MPI_STATUS_IGNORE );
		}
	}

}
