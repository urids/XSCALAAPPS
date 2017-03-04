
#include "../include/commBench.h"

int main(int argc, char** argv){



	MPI_Init(&argc, &argv);

	//printf("ok test \n");

/*	char* buffer=malloc(sizeof(char)*MAX_SIZE);

	for(i=0;i<MAX_SIZE;i++){
		buffer[i]='X';
	}*/
	//int PUs=OMPI_collectDevicesInfo(CPU_DEVICES, MPI_COMM_WORLD);
	//printf("Num PUs: %d\n",PUs);

	int numTasks=OMPI_collectTaskInfo(ALL_DEVICES, MPI_COMM_WORLD);
	int taskId;

	for(taskId=0;taskId<numTasks;taskId++){
		printf("Hello I'm Task %d \n",taskId);
	}


	/*
	L_Mtx=malloc(sizeof(float)*PUs*PUs);
	BW_Mtx=malloc(sizeof(float)*PUs*PUs);

	for(i=0;i<PUs;i++){
		err|=OMPI_XclWriteTaskBuffer(i, 0, sizeof(char)*MAX_SIZE, buffer, MPI_COMM_WORLD); //first task inits token to 1
		err|= OMPI_XclMallocTaskBuffer(i, 1, sizeof(char)*MAX_SIZE,MPI_COMM_WORLD);//remaining tasks allocate space
	}


	//latency test.
	for(src=0;src<PUs;src++){
		for(dst=0;dst<=src;dst++){
			accumTime=0;
			for(i=0;i<LATENCY_REPS;i++){
				deltaT = 0;
				T1 = MPI_Wtime(); // start time
				err |= OMPI_XclSendRecv(src, 0, dst, 1, 1, MPI_CHAR,MPI_COMM_WORLD ); //SEND
				err |= OMPI_XclSendRecv(dst, 1, src, 0, 1, MPI_CHAR,MPI_COMM_WORLD ); //SEND-BACK
				T2 = MPI_Wtime(); // end time
				deltaT = T2-T1;
				accumTime += deltaT;
			}
			L_Mtx[PUs*src+dst]=L_Mtx[src+PUs*dst]=(accumTime/LATENCY_REPS); //Symmetric mtx.
		}
	}



	//Bandwidth test.

	for(src=0;src<PUs;src++){
		for(dst=0;dst<=src;dst++){
			msgSz=MIN_SIZE;
			numBWTrials=0;
			sumAvgs=0;
			while(msgSz<=MAX_SIZE){
				deltaT = 0;
				T1 = MPI_Wtime(); // start time
				err |= OMPI_XclSendRecv(src, 0, dst, 1, sizeof(char)*msgSz, MPI_CHAR,MPI_COMM_WORLD ); //SEND
				err |= OMPI_XclSendRecv(dst, 1, src, 0, sizeof(char)*msgSz, MPI_CHAR,MPI_COMM_WORLD ); //SEND-BACK
				T2 = MPI_Wtime(); // end time
				deltaT = T2-T1;
				Avg[numBWTrials]=2*msgSz/deltaT;
				numBWTrials++;
				msgSz<<=1;
			}

			for(i=0;i<numBWTrials;i++){
				sumAvgs+=Avg[i];
			}
			BW_Mtx[PUs*src+dst]=BW_Mtx[src+PUs*dst]=(sumAvgs/numBWTrials); //the matrix is symmetric.
		}
	}


	//Average computations
	float LAccum=0;
	float BWAccum=0;
	for(i=0;i<PUs;i++){
		for(j=0;j<PUs;j++){
			LAccum+=L_Mtx[PUs*i+j];
			BWAccum+=BW_Mtx[PUs*i+j];
		}
	}
	L=LAccum/(PUs*(PUs+1)/2);
	BW=BWAccum/(PUs*(PUs+1)/2);

	printf("Average Latency: %8.8f microSeconds\n",L*1000000);
	printf("Average Bandwidth: %8.8f GB/s\n",BW/(float)(ONEGB));


	for(i=0;i<PUs;i++){
		printf("| ");
		for(j=0;j<PUs;j++){
			printf(" %8.8f |",L_Mtx[PUs*i+j]*1000000);
		}
		printf("\n-------------------------------\n");
	}

	printf("\n\n");

	for(i=0;i<PUs;i++){
		printf("| ");
		for(j=0;j<PUs;j++){
			printf("  %8.6f |",BW_Mtx[PUs*i+j]/(float)(ONEGB));
		}
		printf("\n-------------------------------\n");
	}

	for(i=0;i<PUs;i++){
		OMPI_XclFreeTaskBuffer(i, 0, MPI_COMM_WORLD);
		OMPI_XclFreeTaskBuffer(i, 1, MPI_COMM_WORLD);
	}

	free(buffer);
*/
	MPI_Finalize();

	return 0;
}
