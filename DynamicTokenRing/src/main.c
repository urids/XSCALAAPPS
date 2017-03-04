/*
 * main.c
 *
 *  Created on: Sep 17, 2016
 *      Author: uriel
 */

#include "dynTokenR.h"

int main(int argc, char** argv){

	int numTasks;
	int token=7;

	XSCALA_Initialize(argc,argv);


	int Dims = 1;
	size_t globalDims[1] = {numThreads};
	size_t localDims[1] = {numThreads};

	task_t *myNewTask=malloc(4*sizeof(task_t));
	int err = XSCALA_CreateNewTasks(myNewTask,4,RANK0,NULL,NULL,MPI_COMM_WORLD);


	//XSCALA_GetNumTasks(&numTasks, MPI_COMM_WORLD);
	//printf("numTasks: %d\n",numTasks);

	err |= XSCALA_MallocTray(myNewTask[0].ID, TRAY_0, sizeof(int),MPI_COMM_WORLD);
	err |= XSCALA_MallocTray(myNewTask[1].ID, TRAY_0, sizeof(int),MPI_COMM_WORLD);


	err |= XSCALA_WriteTray(myNewTask[0].ID, TRAY_0, sizeof(int), &token, MPI_COMM_WORLD);

	err |= XSCALA_SetProcedure(MPI_COMM_WORLD,myNewTask[0].ID,"procedure.cl","Accum");
	err |= XSCALA_SetProcedure(MPI_COMM_WORLD,myNewTask[1].ID,"procedure.cl","Accum");

	XSCALA_ExecTask(MPI_COMM_WORLD, myNewTask[0].ID, Dims, globalDims, localDims, "%T, %d", TRAY_0, myNewTask[0].ID);


	int res=0;
	err |= XSCALA_ReadTray(myNewTask[0].ID, TRAY_0, sizeof(int), &res, MPI_COMM_WORLD);
	err = XSCALA_WaitFor(1, &(int){myNewTask[0].ID}, MPI_COMM_WORLD);
	printf("HELLO WORLD (Dynamic Scheduling Version) \n"
			"Final Token Value : %d\n",res);



	XSCALA_Finalize();

	return 0;

}

