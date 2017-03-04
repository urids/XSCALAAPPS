


__kernel void mtxAdd(__global float* res_blk, __global float* blk1, __global float* blk2){
int thd_Row= get_global_id(1);
int thd_Col= get_global_id(0);
int sz=get_global_size(0);

res_blk[thd_Row*sz+thd_Col]=blk1[thd_Row*sz+thd_Col]+blk2[thd_Row*sz+thd_Col];
}


#define VECSZ 16
__kernel __attribute__((vec_type_hint(float16))) void mtxAdd2(__global float16* C, __global float16* A, __global float16* B){
	int Row= get_global_id(0);
	int sz=get_global_size(0);
	int redSz=(int)sz/VECSZ;
	for(int i=0;i<redSz;i++){
		C[Row*redSz+i]=A[Row*redSz+i]+B[Row*redSz+i];
	}	
}


__kernel void mtxSubs(__global float* res_blk, __global float* blk1, __global float* blk2){
int Row= get_global_id(1);
int Col= get_global_id(0);
int sz=get_global_size(0);

	res_blk[Row*sz+Col]=blk1[Row*sz+Col]-blk2[Row*sz+Col];
}

__kernel void mtxConquer(__global float* res_blk, __global float* blk1, __global float* blk2, __global float* blk3, __global float* blk4){
int Row= get_global_id(1);
int Col= get_global_id(0);
int sz=get_global_size(0);
	res_blk[Row*sz+Col]=blk1[Row*sz+Col]+blk2[Row*sz+Col]-blk3[Row*sz+Col]+blk4[Row*sz+Col];
}


// Compute C = A * B
#define TILE_WIDTH 16

__kernel void mtxMultShared(__global float * C, __global float * A,__global float * B,const int numARows,const int numAColumns) {

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
	
	if (Row < numCRows && Col < numCColumns){
	    C[numAColumns*Row+Col]=0.0;
	}
	
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

	if (Row < numARows && Col < numAColumns)
		C[numCColumns*Row + Col] += Accum;
} 




#define TS 16	
#define WPT 2
#define RTS (TS/WPT) 

// Increased the amount of work-per-thread by a factor WPT
__kernel void myGEMM3(const int M, const int N, const int K,
		const __global float* A,
		const __global float* B,
		__global float* C) {
// Thread identifiers
	const int row = get_local_id(0);// Local row ID (max: TS)
	const int col = get_local_id(1);// Local col ID (max: TS/WPT == RTS)
	const int globalRow = TS*get_group_id(0) + row;// Row ID of C (0..M)
	const int globalCol = TS*get_group_id(1) + col;// Col ID of C (0..N)

// Local memory to fit a tile of TS*TS elements of A and B
	__local float Asub[TS][TS];
	__local float Bsub[TS][TS];

// Initialise the accumulation registers
	float acc[WPT];
	for (int w=0; w<WPT; w++) {
		acc[w] = 0.0f;
	}
// Loop over all tiles
	const int numTiles = K/TS;
	for (int t=0; t<numTiles; t++) {

// Load one tile of A and B into local memory
		for (int w=0; w<WPT; w++) {
			const int tiledRow = TS*t + row;
			const int tiledCol = TS*t + col;
			Asub[col + w*RTS][row] = A[(tiledCol + w*RTS)*M + globalRow];
			Bsub[col + w*RTS][row] = B[(globalCol + w*RTS)*K + tiledRow];
		}
// Synchronise to make sure the tile is loaded
		barrier(CLK_LOCAL_MEM_FENCE);

// Perform the computation for a single tile
		for (int k=0; k<TS; k++) {
			for (int w=0; w<WPT; w++) {
				acc[w] += Asub[k][row] * Bsub[col + w*RTS][k];
			}
		}

// Synchronise before loading the next tile
		barrier(CLK_LOCAL_MEM_FENCE);
	}

// Store the final results in C
	for (int w=0; w<WPT; w++) {
		C[(globalCol + w*RTS)*M + globalRow] = acc[w];
	}
}




