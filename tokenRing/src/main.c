
/*
 * Token Ring Example
 * */
#include "common.h"

int main(int argc, char** argv){

	XSCALA_Initialize(argc, argv);

	int numTasks;
	XSCALA_GetNumTasks(&numTasks, MPI_COMM_WORLD);
	printf("numTasks: %d\n",numTasks);

	for (taskId = 0; taskId < numTasks; taskId++) {
		err |= XSCALA_MallocTray(taskId, TRAY0, sizeof(int),MPI_COMM_WORLD);
		err |= XSCALA_SetProcedure(MPI_COMM_WORLD,taskId,"procedure.cl","Accum");
	}

	int token=0;
	err |= XSCALA_WriteTray(TASK0, TRAY0, sizeof(int),&token,MPI_COMM_WORLD);

	int Dims = 1;
	size_t globalDims[1] = {numThreads};
	size_t localDims[1] = {numThreads};

	for (taskId = 0, TAG=0; taskId < numTasks; taskId++, TAG++) {
		err |= XSCALA_ExecTask(MPI_COMM_WORLD, taskId, Dims, globalDims,localDims, "%T, %d", TRAY0,taskId);
		err |= XSCALA_SendRecv(taskId, TRAY0, taskId+1, TRAY0, sizeof(int), TAG);
	}

	err |= XSCALA_ReadTray(TASK5, TRAY0, sizeof(int), &token, MPI_COMM_WORLD);
	err |= XSCALA_WaitFor(1, (int[]){TASK5}, MPI_COMM_WORLD);
	printf("\n HOLA MUNDO: Final Value of the token = %d \n", token);

	XSCALA_Finalize();

	return 0;
}
