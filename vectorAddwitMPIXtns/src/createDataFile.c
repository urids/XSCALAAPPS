

#include "common.h"

float myrand(float randMax, float randMin) {
	float result;
	result = (float) rand() / (float) RAND_MAX;
	return ((1.0f - result) * randMin + result * randMax);
}


void createAddDataFile(){
	unsigned int i;
	MPI_File AdddataFile;
	const char* fileName = "AdddataFile.dat";

	entityType *opersData=malloc(VectorSize*sizeof(entityType));
	if (opersData == NULL ) {
		printf("cannot allocate memory.");
		exit(-1);
	}


	MPI_Datatype MPIentityType;
	MPI_Datatype basictypes[1] = { MPI_FLOAT};
	int blockCount=1;
	int blocklen[1] = { 3 };
	MPI_Aint disp[1];
	disp[0] = (MPI_Aint) &opersData[0].Opers - (MPI_Aint) &opersData[0];

	MPI_Type_create_struct(blockCount, blocklen, disp, basictypes, &MPIentityType);
	MPI_Type_commit(&MPIentityType);


	MPI_File_open(MPI_COMM_SELF,fileName, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&AdddataFile);
	for (i = 0; i < VectorSize; ++i){
		opersData[i].Opers[0]=i;//myrand(3, 5);
		opersData[i].Opers[1]=i+1;//myrand(3, 5);
		opersData[i].Opers[2]=0;
	}


	MPI_File_write(AdddataFile, opersData, VectorSize, MPIentityType, MPI_STATUS_IGNORE );
	MPI_File_close(&AdddataFile);
	free(opersData);

}



