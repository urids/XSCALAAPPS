/*
 * common.h
 *
 *  Created on: Feb 25, 2015
 *      Author: uriel
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include "math.h"
#include "CL/cl.h"
#include <sys/time.h>

#define member_size(st_name, member) sizeof(((st_name *)0)->member)
#define member_type(st_name, member) typeof(((st_name *)0)->member)

#define mmod(n,m) ((n % m) + m) % m;

//#define numBodies (1<<19)//1048576 at 20
extern int numBodies;
enum descrip{x=0,y=1,z=2,w=3};
extern MPI_Datatype bodyType;

typedef struct entityType_st{
	float P[4];
	float V[3];
	float nP[4];
	float nV[3];
}entityType;

#define TRAY0 0
#define TRAY1 1
#define TRAY2 2
#define TRAY3 3

#define TASK0 0
#define TASK1 1
#define TASK2 2
#define TASK3 3

//task_t* myTasks=malloc(4*sizeof(task_t));
//XSCALA_CreateNewTasks(myTasks,4,0,0,0,MPI_COMM_WORLD);

#endif /* COMMON_H_ */
