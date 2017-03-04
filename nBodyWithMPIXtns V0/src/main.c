#include "common.h"

int i, j; //exclusive for index variables

entityType *bodyArr;
int numBodies;
MPI_Datatype MPIentityType;

int main(int argc, char* argv[]) {
	int devKind;
	char* infArg;
	infArg = strtok(argv[7], "=");
	infArg = strtok(NULL, "= ");
	numBodies = atoi(infArg);
	infArg = strtok(NULL, "= ");
	devKind = atoi(infArg);

	int myRank, numRanks;
	int ePerRank, ePerTask;
	int err = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);

	struct timeval tval_before, tval_after, tval_result;

	//MPI_Datatype MPIentityType;
	int blockcount = 4;
	MPI_Datatype basictypes[4] = { MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT };
	int blocklen[4] = { 4, 3, 4, 3 };
	MPI_Aint disp[4];

	disp[0] = (MPI_Aint) offsetof(entityType,P);
	disp[1] = (MPI_Aint) offsetof(entityType,V);
	disp[2] = (MPI_Aint) offsetof(entityType,nP);
	disp[3] = (MPI_Aint) offsetof(entityType,nV);

	OMPI_commit_EntityType(blockcount, blocklen, disp, basictypes,&MPIentityType);
	createDataFile();

	if(myRank==0){
		gettimeofday(&tval_before, NULL);
	}

	int numTasks = OMPI_CollectTaskInfo(devKind, MPI_COMM_WORLD );
	ePerTask = (int) numBodies / numTasks;

	err = OMPI_XclScatter("dataFile.dat", &ePerRank, MPIentityType, NULL, 0,
			MPI_COMM_WORLD );
	//then we  create the host buffer with the info returned by
	bodyArr = (entityType*) malloc(ePerRank * sizeof(entityType)); //HOST BUFFER
	//and finally we perform data splitting.
	err |= OMPI_XclScatter("dataFile.dat", NULL, MPIentityType, bodyArr, 0,
			MPI_COMM_WORLD );

	err |=OMPI_XclSetProcedure(MPI_COMM_WORLD,
			"/home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/src/NBodyExt.cl",
			"computeForces");

	int step = 0;
	int taskIdx;

	void * bodyRet = (void*) malloc(ePerTask * sizeof(entityType));
	entityType* dataBuff = ((entityType*) bodyRet);

	int Dims = 1;
	size_t globalDims[] = { ePerTask };
	size_t localDims[] = { 32 };

	for (step = 0; step < numTasks; step++) {
		for (taskIdx = 0; taskIdx < numTasks; taskIdx++) {
			int srcTask = mymod((step+taskIdx-numTasks),numTasks);
			err |= OMPI_XclSendRecv(srcTask, 0, taskIdx, 1, ePerTask, MPIentityType, MPI_COMM_WORLD );
			err |= OMPI_XclExecTask(MPI_COMM_SELF, taskIdx, Dims, globalDims,
					localDims, "%T, %T, %d ,%f ,%f, %d ", 0, 1, numBodies,
					0.0005, 0.01, numTasks);
		}
		err |= OMPI_XclWaitAllTasks(MPI_COMM_WORLD );

		if (myRank == 0) {
			err |= OMPI_XclReadTaskBuffer(0, 0, ePerTask * sizeof(entityType),
					bodyRet, MPI_COMM_WORLD );
		}
	}

	if (myRank == 0) {
		double Gflops;
		gettimeofday(&tval_after, NULL );
		timersub(&tval_after, &tval_before, &tval_result);
		double secs = (double) tval_result.tv_sec;
		double mils = (double) (tval_result.tv_usec) / 1000000;
		Gflops = (double) (44 * pow(numBodies, 2)) / ((secs + mils) * pow(1024, 3));
		printf("time: %ld.%06ld\n", (long int) tval_result.tv_sec,
				(long int) tval_result.tv_usec);
		printf("performance: %f  Gflops", Gflops);

		FILE* resultsFp;
		resultsFp = fopen("resultsnBody.txt", "a");
		fprintf(resultsFp, "%f ",Gflops);
		fclose(resultsFp);

	}

	MPI_Type_free(&MPIentityType);
	MPI_Finalize();
	return err;

}

