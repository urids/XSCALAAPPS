/*
 * dtbLList.c
 *
 *  Created on: Jul 9, 2016
 *      Author: uriel
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <assert.h>

//#include "mpitest.h"

#define NUM_ELEMS 5
#define NPROBE    100
#define ELEM_PER_ROW 16
#define HOME_RANK 0

/* Linked list pointer */
typedef struct {
	int      rank;
	MPI_Aint disp;
} meta_ptr_t;

/* Linked list element */
enum Mode_en{SENDER_MOD=0,RECEIVER_MOD=1};

typedef struct {
	int Tag;
	meta_ptr_t next;
} container_t;

static const meta_ptr_t nil = { -1, (MPI_Aint) MPI_BOTTOM };
static const int verbose = 1;

/* List of locally allocated elements. */
static container_t **l_containers = NULL;
static int l_containers_size  = 0;
static int l_containers_count = 0;

int           procid, nproc, i;
MPI_Win       llist_win;
meta_ptr_t    head_ptr, tail_ptr;




int find_if(int tagVal){
	/* Traverse the list and verify that all processes inserted exactly the correct
       number of elements. */

		int  have_root = 0;
		tail_ptr = head_ptr;

		/* Walk the list and tally up the number of elements inserted by each rank */
		while (tail_ptr.disp != nil.disp) {
			container_t elem;

			MPI_Win_lock(MPI_LOCK_EXCLUSIVE, tail_ptr.rank, 0, llist_win);

			MPI_Get(&elem, sizeof(container_t), MPI_BYTE,
					tail_ptr.rank, tail_ptr.disp, sizeof(container_t), MPI_BYTE, llist_win);

			MPI_Win_unlock(tail_ptr.rank, llist_win);

			tail_ptr = elem.next;

			/* This is not the root */
			if (have_root) {
				assert(elem.Tag >= 0 );

				if(elem.Tag==tagVal){
					return 1;
				}

			}

			/* This is the root */
			else {
				assert(elem.Tag == -1);
				have_root = 1;
			}
		}
	return 0;
}

/* Allocate a new shared linked list element */
MPI_Aint alloc_container(int tagVal,  MPI_Win win) {
	MPI_Aint disp;
	container_t *container_ptr;

	/* Allocate the new element */
	MPI_Alloc_mem(sizeof(container_t), MPI_INFO_NULL, &container_ptr);
	container_ptr->Tag = tagVal;
	container_ptr->next  = nil;

	/* then register it within the window */
	MPI_Win_attach(win, container_ptr, sizeof(container_t));

	/* Add the element to the l_containers list so we can free it later. */
	if (l_containers_size == l_containers_count) {
		l_containers_size += 100;
		l_containers = realloc(l_containers, l_containers_size*sizeof(void*));
	}
	l_containers[l_containers_count] = container_ptr;
	l_containers_count++;

	MPI_Get_address(container_ptr, &disp);
	return disp;
}



int new_dstrContLst(int hostRank){
	MPI_Win_create_dynamic(MPI_INFO_NULL, MPI_COMM_WORLD, &llist_win);

		/* Process 0 creates the head node */
		if (procid == hostRank)
			head_ptr.disp = alloc_container(-1, llist_win);

		/* Broadcast the head pointer to everyone */
		head_ptr.rank = hostRank;
		MPI_Bcast(&head_ptr.disp, 1, MPI_AINT, hostRank, MPI_COMM_WORLD);
		tail_ptr = head_ptr;

	return 0;
}


int dstrContLst_push(int tagVal){

		meta_ptr_t new_elem_ptr;
		int success;

		/* Create a new list element and register it with the window */
		new_elem_ptr.rank = procid;
		new_elem_ptr.disp = alloc_container(tagVal, llist_win);

		/* Append the new node to the list.  This might take multiple attempts if
	           others have already appended and our tail pointer is stale. */
		do {
			meta_ptr_t xplr_elem_ptr = nil;

			MPI_Win_lock(MPI_LOCK_EXCLUSIVE, tail_ptr.rank, 0, llist_win);

			MPI_Compare_and_swap((void*) &new_elem_ptr.rank, (void*) &nil.rank,
					(void*) &xplr_elem_ptr.rank, MPI_INT, tail_ptr.rank,
					(MPI_Aint) &(((container_t*)tail_ptr.disp)->next.rank), llist_win);

			MPI_Win_unlock(tail_ptr.rank, llist_win);
			success = (xplr_elem_ptr.rank == nil.rank);

			if (success) {
				int i, flag;

				MPI_Win_lock(MPI_LOCK_EXCLUSIVE, tail_ptr.rank, 0, llist_win);

				MPI_Put(&new_elem_ptr.disp, 1, MPI_AINT, tail_ptr.rank,
						(MPI_Aint) &(((container_t*)tail_ptr.disp)->next.disp), 1,
						MPI_AINT, llist_win);

				MPI_Win_unlock(tail_ptr.rank, llist_win);
				tail_ptr = new_elem_ptr;

				/* For implementations that use pt-to-pt messaging, force progress for other threads'
	                   RMA operations. */
	                   for (i = 0; i < NPROBE; i++)
	                	   MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);

			} else {
				/* Tail pointer is stale, fetch the displacement.  May take multiple tries
	                   if it is being updated. */
				do {
					MPI_Win_lock(MPI_LOCK_EXCLUSIVE, tail_ptr.rank, 0, llist_win);

					MPI_Get( &xplr_elem_ptr.disp, 1, MPI_AINT, tail_ptr.rank,
							(MPI_Aint) &(((container_t*)tail_ptr.disp)->next.disp),
							1, MPI_AINT, llist_win);

					MPI_Win_unlock(tail_ptr.rank, llist_win);
				} while (xplr_elem_ptr.disp == nil.disp);
				tail_ptr = xplr_elem_ptr;
			}
		} while (!success);
	return 0;

}


int free_dstrContLst(){
	MPI_Win_free(&llist_win);

	/* Free all the elements in the list */
	for ( ; l_containers_count > 0; l_containers_count--)
		MPI_Free_mem(l_containers[l_containers_count-1]);

	return 0;
}

int main(int argc, char **argv) {


	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &procid);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	new_dstrContLst(HOME_RANK);

	/* All processes concurrently append NUM_ELEMS elements to the list */
	for (i = 0; i < NUM_ELEMS; i++) {

		dstrContLst_push(i);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (procid == 0) {
		int found =  find_if(3) == 1;
		printf("%s", found ? "found \n" : " not_found \n");
	}

	free_dstrContLst();

	dmutex();

	MPI_Finalize();
	return 0;
}


