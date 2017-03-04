

#include "common.h"

float myrand(float randMax, float randMin) {
	float result;
	result = (float) rand() / (float) RAND_MAX;
	return ((1.0f - result) * randMin + result * randMax);
}

void createDataFile(){


	entityType *bodyData=malloc(numBodies*sizeof(entityType));
/*
	MPI_Datatype basictypes[4] = { MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT };
	int blocklen[4] = { 4, 3, 4, 3};
	MPI_Aint disp[4];
	disp[0] = (MPI_Aint) &bodyData[0].P - (MPI_Aint) &bodyData[0];
	disp[1] = (MPI_Aint) &bodyData[0].V - (MPI_Aint) &bodyData[0];
	disp[2] = (MPI_Aint) &bodyData[0].nP - (MPI_Aint) &bodyData[0];
	disp[3] = (MPI_Aint) &bodyData[0].nV - (MPI_Aint) &bodyData[0];
	MPI_Type_create_struct(4, blocklen, disp, basictypes, &MPIentityType);
	MPI_Type_commit(&MPIentityType);
*/

	int i;
	MPI_File dataFile;
	const char* fileName="dataFile.dat";
	MPI_File_open(MPI_COMM_SELF,fileName, MPI_MODE_WRONLY | MPI_MODE_CREATE,MPI_INFO_NULL,&dataFile);

	for (i = 0; i < numBodies; ++i){
		bodyData[i].P[x]=myrand(1.0, 3.0);
		bodyData[i].P[y]=myrand(1.0, 3.0);
		bodyData[i].P[z]=myrand(1.0, 3.0);
		bodyData[i].P[w]=myrand(3.0, 5.0);
		bodyData[i].V[x]=myrand(1.0, 20.0);
		bodyData[i].V[y]=myrand(1.0, 20.0);
		bodyData[i].V[z]=myrand(1.0, 20.0);

		bodyData[i].nP[x] = 0.0;
		bodyData[i].nP[y] = 0.0;
		bodyData[i].nP[z] = 0.0;
		bodyData[i].nP[w] = bodyData[i].P[w];
		bodyData[i].nV[x] = 0.0;
		bodyData[i].nV[y] = 0.0;
		bodyData[i].nV[z] = 0.0;
	}

		MPI_File_write(dataFile, bodyData, numBodies, MPIentityType, MPI_STATUS_IGNORE );

	MPI_File_close(&dataFile);

	free(bodyData);

}
