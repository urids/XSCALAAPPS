/*
 * dynTokenR.h
 *
 *  Created on: Sep 17, 2016
 *      Author: uriel
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"

#ifndef DYNTOKENR_H_
#define DYNTOKENR_H_


#define RANK0 0

#define TRAY_0 0
#define TASK0 0

#define TRAY_1 1
#define TASK1 1

#define TRAY_2 2
#define TASK2 2

#define TRAY_3 3
#define TASK3 3

#define TRAY_4 4
#define TASK4 4


#define TAG0 0
#define TAG1 1
#define TAG2 2
#define TAG3 3
#define TAG4 4
#define TAG5 5


#define mymod(n,m) ((n % m) + m) % m;
#define numThreads 10

//int i,j; //indexExclusive variables
//int err;


#endif /* DYNTOKENR_H_ */

/*
	int taskId,res;
	for (taskId = 0; taskId < numTasks; taskId++) {
		err |= OMPI_XclMallocTray(taskId, TRAY_0, sizeof(int),MPI_COMM_WORLD);
		err |= OMPI_XclSetProcedure(MPI_COMM_WORLD,taskId,"/home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing/src/proc.cl","Accum");
	}


	int te=0;
	err |= OMPI_XclWriteTray(TASK0, TRAY_0, sizeof(int),&te,MPI_COMM_WORLD);
	err |= OMPI_XclWriteTray(TASK2, TRAY_0, sizeof(int),&te,MPI_COMM_WORLD);

	err |= OMPI_XclExecTask(MPI_COMM_WORLD, TASK0, Dims, globalDims, localDims, "%T, %d", TRAY_0, TASK0);

	err |= OMPI_XclSendRecv(TASK0, TRAY_0, TASK1, TRAY_0, sizeof(int), 9);

	err |= OMPI_XclExecTask(MPI_COMM_WORLD, TASK1, Dims, globalDims, localDims, "%T, %d", TRAY_0, TASK1);

	err = OMPI_XclExecTask(MPI_COMM_WORLD, TASK2, Dims, globalDims, localDims, "%T, %d", TRAY_0, TASK2);

	err |= OMPI_XclSendRecv(TASK2, TRAY_0, TASK3, TRAY_0, sizeof(int), 7);

	err |= OMPI_XclExecTask(MPI_COMM_WORLD, TASK3, Dims, globalDims,localDims, "%T, %d", TRAY_0,TASK3);

	task_t *moreTasks=malloc(1*sizeof(task_t));
	err = XclCreateNewTasks(moreTasks,1,RANK0,NULL,NULL,MPI_COMM_WORLD);
	printf("\n moreTasks[0].ID: %d \n",moreTasks[0].ID);


	err |= OMPI_XclMallocTray(moreTasks[0].ID, TRAY_1, sizeof(int),MPI_COMM_WORLD);
	err |= OMPI_XclMallocTray(moreTasks[0].ID, TRAY_2, sizeof(int),MPI_COMM_WORLD);
	err |= OMPI_XclSetProcedure(MPI_COMM_WORLD,moreTasks[0].ID,"/home/uriel/Dev/mpiApps/extnsApps/DynamicTokenRing/src/proc.cl","Sum");


	err |= OMPI_XclSendRecv(TASK1, TRAY_0, moreTasks[0].ID, TRAY_0, sizeof(int), TAG2);
	err |= OMPI_XclSendRecv(TASK3, TRAY_0, moreTasks[0].ID, TRAY_1, sizeof(int), TAG3);
	err = OMPI_XclWaitFor(1, &(int){moreTasks[0].ID}, MPI_COMM_WORLD);
	err |= OMPI_XclExecTask(MPI_COMM_WORLD, moreTasks[0].ID, Dims, globalDims,localDims, "%T, %T, %T", TRAY_2,TRAY_0,TRAY_1);

	//if(myRank==0){
	err |= OMPI_XclReadTray(moreTasks[0].ID, TRAY_2, sizeof(int), &res, MPI_COMM_WORLD);
	//}
	err = OMPI_XclWaitFor(1, (int[]){moreTasks[0].ID}, MPI_COMM_WORLD);
	//if(myRank==0){
	printf("\n res: %d \n", res);
	//}

*/

