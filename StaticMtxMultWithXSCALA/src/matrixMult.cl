
__kernel void mtxMult(__global float* A, __global float* B, __global float*C , const int m, const int n) {
	int thIdx= get_global_id(0);

	int myRow = (int)(thIdx/n);
	int myCol = thIdx-(myRow*n);

	int j;
	for(j=0;j<n;j++) {
		C[thIdx]+=A[myRow*n+j]*B[myCol+j*n];
	}

}


// Compute C = A * B
#define TILE_WIDTH 16

__kernel void mtxMultShared(__global float * A,__global float * B,__global float * C,const int numARows,const int numAColumns) {


int numBRows=numARows; 
int numBColumns=numARows; 
int numCRows=numARows;
int numCColumns=numARows;


	__local float ds_M[TILE_WIDTH][TILE_WIDTH];
	__local	float ds_N[TILE_WIDTH][TILE_WIDTH];
	int bx = get_group_id(0);
	int by = get_group_id(1);
	
	int tx = get_local_id(0);
	int ty = get_local_id(1);
	
	int	Row = get_global_id(1);
	int Col = get_global_id(0);
	
	float Accum = 0;

	for (int m = 0; m < (numAColumns - 1) / TILE_WIDTH + 1; ++m) {
		if (Row < numARows && m * TILE_WIDTH + tx < numAColumns)
			ds_M[ty][tx] = A[Row * numAColumns + m * TILE_WIDTH + tx];
		else
			ds_M[ty][tx] = 0;
		if (Col < numBColumns && m * TILE_WIDTH + ty < numBRows)
			ds_N[ty][tx] = B[(m * TILE_WIDTH + ty) * numBColumns + Col];
		else
			ds_N[ty][tx] = 0;

		 barrier(CLK_LOCAL_MEM_FENCE);
		for (int k = 0; k < TILE_WIDTH; ++k)
			Accum += ds_M[ty][k] * ds_N[k][tx];
		barrier(CLK_LOCAL_MEM_FENCE);
	}
	if (Row < numCRows && Col < numCColumns)
		C[Row * numCColumns + Col] += Accum;
} 



__kernel void mtxAdd(__global float* C, __global float* D, const int m, const int n,const int alpha){
int thIdx= get_global_id(0);

	C[thIdx]=alpha*C[thIdx]+D[thIdx];

}