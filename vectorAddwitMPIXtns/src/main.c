
#include "common.h"

int VectorSize;

int main(int argc, char* argv[])
{

	VectorSize = atoi(argv[1]);
	//createAddDataFile();

	XSCALA_Initialize(argc,argv);

	int numDevices;
	XSCALA_GetNumDevices(&numDevices, ALL_DEVICES, MPI_COMM_WORLD);
	printf("numDevices: %d \n",numDevices);

	int numTasks;
	XSCALA_GetNumTasks(&numTasks, MPI_COMM_WORLD);
	printf("numTasks: %d \n",numTasks);


	initLapse
	MPI_Datatype eType;
	int blockcount=1;
	MPI_Datatype basictypes[1] = { MPI_FLOAT };
	int blocklen[1] = { 3 };
	MPI_Aint disp[1];\
	disp[0]=(MPI_Aint) offsetof(entityType,Opers);




	XSCALA_CommitEntity(blockcount, blocklen,  disp,  basictypes, &eType);

	ePerTask=(int)VectorSize/numTasks;

	XSCALA_Scatter("AdddataFile.dat",  eType, TRAY0, MPI_COMM_WORLD);


	int alpha=2;

	for (tskIdx = 0; tskIdx < numTasks; tskIdx++){
		err|= XSCALA_SetProcedure(MPI_COMM_WORLD,tskIdx,"vecAdd.cl","vecAdd");
	}

	int Dims=1;
	size_t globalDims[]={ePerTask};
	size_t localDims[]={64};

	for(tskIdx=0;tskIdx<numTasks;tskIdx++){
		err|=XSCALA_ExecTask(MPI_COMM_SELF,tskIdx,Dims,globalDims,localDims,"%T %d %d", 0,alpha,VectorSize);
	}
	XSCALA_WaitAllTasks(MPI_COMM_WORLD);


	finiLapse

	//print results
	void * dataRet = (entityType*)malloc(10*sizeof(entityType));
	err|= XSCALA_ReadTray(TASK0, TRAY0, 10*sizeof(entityType),dataRet, MPI_COMM_WORLD);
	XSCALA_WaitAllTasks(MPI_COMM_WORLD);

		for (i = 1; i < 5; i++) {
			printf("sum: %d --> %d * %f + %f = %f \n", i,  alpha,
					((entityType*)dataRet)[i].Opers[0],
					((entityType*)dataRet)[i].Opers[1],
					((entityType*)dataRet)[i].Opers[2]);
		}

	XSCALA_Finalize();
	return 0;
}
