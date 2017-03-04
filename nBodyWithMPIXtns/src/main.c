#include "common.h"

int numBodies;
MPI_Datatype bodyType;
entityType *bodyArr;
int ePerRank, ePerTask;
int err;

int main(int argc, char* argv[]) {

	int i, j; //exclusive for index variables
	int devKind=0;
	numBodies=atoi(argv[1]);
	int myRank, numRanks;

	XSCALA_Initialize(argc,argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);


	int numTasks;
	XSCALA_GetNumTasks(&numTasks, MPI_COMM_WORLD);
	printf("numTasks: %d\n",numTasks);

	struct timeval tval_before, tval_after, tval_result;
	int blockcount = 4;
	MPI_Datatype basictypes[4] = { MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT };
	int blocklen[4] = { 4, 3, 4, 3 };
	MPI_Aint disp[4];

	disp[0] = (MPI_Aint) offsetof(entityType,P);
	disp[1] = (MPI_Aint) offsetof(entityType,V);
	disp[2] = (MPI_Aint) offsetof(entityType,nP);
	disp[3] = (MPI_Aint) offsetof(entityType,nV);

	XSCALA_CommitEntity(blockcount, blocklen, disp, basictypes,&bodyType);
	createDataFile();

	if(myRank==0){
		gettimeofday(&tval_before, NULL);
	}

	int bodyTypeSz;
	MPI_Type_size(bodyType, &bodyTypeSz);

	ePerTask = (int) numBodies / numTasks;

	int taskIdx,TRAY;
	for(taskIdx=0;taskIdx<numTasks;taskIdx++){
		for(TRAY=0;TRAY<2;TRAY++){
			err |= XSCALA_MallocTray(taskIdx, TRAY, bodyTypeSz*ePerTask, MPI_COMM_WORLD);
		}
	}

	XSCALA_Scatter("dataFile.dat", bodyType, TRAY0, MPI_COMM_WORLD);

	for(taskIdx=0;taskIdx<numTasks;taskIdx++){
		err = XSCALA_SetProcedure(MPI_COMM_WORLD,taskIdx,
				"/home/uriel/Dev/mpiApps/extnsApps/nBodyWithMPIXtns/src/NBodyExt.cl",
				"computeForces");
	}


	void * bodyRet = (void*) malloc(ePerTask * sizeof(entityType));
		entityType* dataBuff = ((entityType*) bodyRet);

		int Dims = 1;
		size_t globalDims[] = { ePerTask };
		size_t localDims[] = { 16 };



		int TAG=1;
		int step = 0;
		int dstTask, srcTask;



				for (step = 0; step < numTasks; step++) {
					for (dstTask = 0; dstTask < numTasks; dstTask++,TAG++) {
						srcTask = mmod((step+dstTask-numTasks),numTasks);
						err |= XSCALA_SendRecv(srcTask, 0, dstTask, 1, bodyTypeSz*ePerTask ,TAG );
						err |= XSCALA_WaitAllTasks(MPI_COMM_WORLD);
						err |= XSCALA_ExecTask(MPI_COMM_WORLD, dstTask, Dims, globalDims,
								localDims, "%T, %T, %d ,%f ,%f, %d ", TRAY0, TRAY1, numBodies,
								0.0005, 0.01, numTasks);
					}
					err |= XSCALA_WaitAllTasks(MPI_COMM_WORLD);
				}


					err |= XSCALA_ReadTray(TASK0, TRAY0, ePerTask * sizeof(entityType),
							bodyRet, MPI_COMM_WORLD);

				err = XSCALA_WaitFor(1, (int[]){TASK0}, MPI_COMM_WORLD);

					printf("%f \n",dataBuff->P[0]);
					printf("%f \n",dataBuff->P[1]);
					printf("%f \n",dataBuff->P[2]);
					printf("%f \n",dataBuff->P[3]);




					double Gflops;
					gettimeofday(&tval_after, NULL );
					timersub(&tval_after, &tval_before, &tval_result);
					double secs = (double) tval_result.tv_sec;
					double mils = (double) (tval_result.tv_usec) / 1000000;
					Gflops = (double) (44 * pow(numBodies, 2)) / ((secs + mils) * pow(1024, 3));
					printf("time:: %ld.%06ld\n", (long int) tval_result.tv_sec,
							(long int) tval_result.tv_usec);
					printf("performance: %f  Gflops", Gflops);

					FILE* resultsFp;
					resultsFp = fopen("resultsnBody.txt", "a");
					fprintf(resultsFp, "%f ",Gflops);
					fclose(resultsFp);







	XSCALA_Finalize();
	return err;

}

