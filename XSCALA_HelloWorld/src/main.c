#include "mpi.h"
#include "stdlib.h"
#include "stdio.h"



int main(int argc, char** argv) {

	MPI_Init(&argc, &argv);

	int numTasks = OMPI_CollectTaskInfo(ALL_DEVICES, MPI_COMM_WORLD);
	int taskId;

	for (taskId = 0; taskId < numTasks; taskId++) {
		printf("Hello I'm Task %d \n", taskId);
	}

	MPI_Finalize();
	return 0;
}
