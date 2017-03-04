
#define VECSZ 16
#define WPTS 16

#define TS 16	
#define WPT 8
#define RTS (TS/WPT) 


__kernel void SAXPY(const float alpha, __global float16* X, __global float16* Y){

	//int gId=get_global_id(0);
	int lId=get_local_id(0);
	int lsize=get_local_size(0);
	
	int g_offset=get_group_id(0)*lsize*WPTS;
	int l_offset=lId*WPTS;
	
	for(int i=0;i<WPTS;i++){
	int offset=g_offset+l_offset;	
		Y[offset+i]=(float16)alpha*X[offset+i]+Y[offset+i];		
	}	
}

__kernel __attribute__((vec_type_hint(float16))) void mtxAdd(const int psize, __global float16* A, __global float16* B,__global float16* C){
	int GT_Row= get_global_id(0);	
	int redSz=(int)psize/VECSZ;
	int offset=GT_Row*redSz*WPTS;
	
	for(int i=0;i<WPTS;i++){
	    for(int j=0;j<redSz;j++){
		    C[offset+(i*redSz)+j]=A[offset+(i*redSz)+j]+B[offset+(i*redSz)+j];
	    }	
	}
}


#define scalar_t float
#define WPtx 16
#define tPTx 16
#define tPTy 1

#define TSx (WPtx*tPTx)
#define TSy 16
#define PSIZE (1<<14)

#define NUMTILESx (((PSIZE-1)/TSx)+1)
//#define NUMTILESy (((PSIZE-1)/TSy)+1)
#define STEPOFFSET (WPtx*tPTx)

__kernel void GEMV(__global const scalar_t * A,__global const scalar_t * X,
		__global scalar_t * Y, int psize)
{

	__local scalar_t work[TSx]; 
	
	

	int thIdx=get_local_id(1);	
	
	__local scalar_t tmp_sum[TSy];
	for (int w=0; w<TSy; w++) {
		tmp_sum[w] = 0.0f;
	}
	
	__local scalar_t work_sum[tPTx];
	for (int w=0; w<tPTx; w++) {
		work_sum[w] = 0.0f;
	}
	 	

	for(int tile=0;tile<NUMTILESx;tile++){
		// Load a slice of X in work, using all local threads
		for (int k=0;k<WPtx;k++)
		{
			work[thIdx*WPtx+k]=X[(tile*STEPOFFSET)+(thIdx*WPtx)+k];
		}
		barrier(CLK_LOCAL_MEM_FENCE); // sync group
		// Compute partial dot product
		for (int i=0;i<TSy;i++)
		{
			int yOffset=(get_group_id(0)*TSy+i)*psize;
			int xOffset=tile*STEPOFFSET;
			for(int j=0;j<WPtx;j++){
			work_sum[thIdx]+=A[yOffset+xOffset+j]*work[(thIdx*WPtx)+j];					
			}
			
			
			for(int offset=1;offset<tPTx;offset<<=1){
				int mask=(offset<<1)-1;
				if((thIdx & mask)==0){		
					tmp_sum[i]+=work_sum[thIdx]+work_sum[thIdx+offset];
				}
			barrier(CLK_LOCAL_MEM_FENCE);
			}
			
		}	
	}
		
	barrier(CLK_LOCAL_MEM_FENCE); // sync group
	
	// Finally Store in Y 
	for (int i=0;i<TSy;i++)
		{
		int yOffset=get_group_id(0)*TSy+i;
		Y[yOffset]=work_sum[i];
		}

}








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


