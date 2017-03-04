
    #define TSM 64 // The tile-size in dimension M
    #define TSN 64 // The tile-size in dimension N
    #define TSK 64 // The tile-size in dimension K
    #define WPTM 8 // The work-per-thread in dimension M
    #define WPTN 8 // The work-per-thread in dimension N
    #define RTSM (TSM/WPTM) // The reduced tile-size in dimension M
    #define RTSN (TSN/WPTN) // The reduced tile-size in dimension N
    #define LPTA ((TSK*TSM)/(RTSM*RTSN)) // Loads-per-thread for A
    #define LPTB ((TSK*TSN)/(RTSM*RTSN)) // Loads-per-thread for B
    
 
 // Use 2D register blocking (further increase in work per thread)
__kernel void myGEMM6V0(const int M, const int N, const int K,
		const __global float* A,
		const __global float* B,
		__global float* C) {
		
	// Thread identifiers
	const int tidm = get_local_id(0); // Local row ID (max: TSM/WPTM)
	const int tidn = get_local_id(1); // Local col ID (max: TSN/WPTN)
	const int offsetM = TSM*get_group_id(0); // Work-group offset
	const int offsetN = TSN*get_group_id(1); // Work-group offset

	// Local memory to fit a tile of A and B
	__local float Asub[TSK][TSM];
	__local float Bsub[TSN][TSK+2];

	// Allocate register space
	float Areg;
	float Breg[WPTN];
	float acc[WPTM][WPTN];

	// Initialise the accumulation registers
	for (int wm=0; wm<WPTM; wm++) {
		for (int wn=0; wn<WPTN; wn++) {
			acc[wm][wn] = 0.0f;
		}
	}
	// Loop over all tiles
	int numTiles = K/TSK;
	for (int t=0; t<numTiles; t++) {

		// Load one tile of A and B into local memory
		for (int la=0; la<LPTA; la++) {
			int tid = tidn*RTSM + tidm;
			int id = la*RTSN*RTSM + tid;
			int row = id % TSM;
			int col = id / TSM;
			int tiledIndex = TSK*t + col;
			Asub[col][row] = A[tiledIndex*M + offsetM + row];
			Bsub[row][col] = B[tiledIndex*N + offsetN + row];
		}
		// Synchronise to make sure the tile is loaded
		barrier(CLK_LOCAL_MEM_FENCE);

		// Loop over the values of a single tile
		for (int k=0; k<TSK; k++) {

			// Cache the values of Bsub in registers
			for (int wn=0; wn<WPTN; wn++) {
				int col = tidn + wn*RTSN;
				Breg[wn] = Bsub[col][k];
			}

			// Perform the computation
			for (int wm=0; wm<WPTM; wm++) {
				int row = tidm + wm*RTSM;
				Areg = Asub[k][row];
				for (int wn=0; wn<WPTN; wn++) {
					acc[wm][wn] += Areg * Breg[wn];
				}
			}
		}

		// Synchronise before loading the next tile
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	// Store the final results in C
	for (int wm=0; wm<WPTM; wm++) {
		int globalRow = offsetM + tidm + wm*RTSM;
		for (int wn=0; wn<WPTN; wn++) {
			int globalCol = offsetN + tidn + wn*RTSN;
			C[globalCol*M + globalRow] = acc[wm][wn];
		}
	}
}    


#define TSN 32
#define TSM 32
#define TSK 32
#define WPTN 16
#define WPTM 16
#define RTSM (TSM/WPTM) // The reduced tile-size in dimension M (== number of threads)
#define RTSN (TSN/WPTN) // The reduced tile-size in dimension N (== number of threads)
#define LPTA ((TSK*WPTM*WPTN)/(TSN)) // The amount of loads-per-thread for A
#define LPTB ((TSK*WPTM*WPTN)/(TSM)) // The amount of loads-per-thread for B
#define MIN(a,b) ((a) > (b)) ? (b) : (a)
#define MAX(a,b) ((a) > (b)) ? (a) : (b)
#define CEIL_DIV(x,y) (((x) + (y) - 1) / (y))
#define MOD2(x,y) ((x) % (y))
#define DIV2(x,y) ((x) / (y))

__kernel void myGEMM6(const int M, const int N, const int K,
		const __global float* A,
		const __global float* B,
		__global float* C) {
	// Thread identifiers
	const int tidm = get_local_id(0); // Local row ID (max: TSM/WPTM == RTSM)
	const int tidn = get_local_id(1); // Local col ID (max: TSN/WPTN == RTSN)
	const int offsetM = TSM*get_group_id(0); // Work-group offset
	const int offsetN = TSN*get_group_id(1); // Work-group offset
	// Local memory to fit a tile of A and B
	__local float Asub[TSK][TSM];
	__local float Bsub[TSN][TSK+2];
	// Allocate register space
	float Areg;
	float Breg[WPTN];
	float acc[WPTM][WPTN];
	// Initialise the accumulation registers
#pragma unroll
	for (int wm=0; wm<WPTM; wm++) {
#pragma unroll
		for (int wn=0; wn<WPTN; wn++) {
			acc[wm][wn] = 0.0f;
		}
	}
	// Loop over all tiles
	const int numTiles = K/TSK;
	int t=0;
	do {
		// Load one tile of A and B into local memory
#pragma unroll
		for (int la=0; la<LPTA; la++) {
			int tid = tidn*RTSM + tidm;
			int id = la*RTSN*RTSM + tid;
			int row = MOD2(id,TSM);
			int col = DIV2(id,TSM);
			int tiledIndex = TSK*t + col;
			Asub[col][row] = A[tiledIndex*M + offsetM + row];
			Bsub[row][col] = B[tiledIndex*N + offsetN + row];
		}
		// Synchronise to make sure the tile is loaded
		barrier(CLK_LOCAL_MEM_FENCE);
		// Loop over the values of a single tile
		for (int k=0; k<TSK; k++) {
			// Cache the values of Bsub in registers
#pragma unroll
			for (int wn=0; wn<WPTN; wn++) {
				int col = tidn + wn*RTSN;
				Breg[wn] = Bsub[col][k];
			}
			// Perform the computation
#pragma unroll
			for (int wm=0; wm<WPTM; wm++) {
				int row = tidm + wm*RTSM;
				Areg = Asub[k][row];
#pragma unroll
				for (int wn=0; wn<WPTN; wn++) {
					acc[wm][wn] += Areg * Breg[wn];
				}
			}
		}
		// Synchronise before loading the next tile
		barrier(CLK_LOCAL_MEM_FENCE);
		// Next tile
		t++;
	} while (t<numTiles);
	// Store the final results in C
#pragma unroll
	for (int wm=0; wm<WPTM; wm++) {
		int globalRow = offsetM + tidm + wm*RTSM;
#pragma unroll
		for (int wn=0; wn<WPTN; wn++) {
			int globalCol = offsetN + tidn + wn*RTSN;
			C[globalCol*M + globalRow] = acc[wm][wn];
		}
	}
}



#define TILE_SIZE_M 1
#define TILE_SIZE_N 128
#define TILE_SIZE_K 8
#define TILE_GROUP_M 16
#define TILE_GROUP_N 1


__attribute__((reqd_work_group_size(TILE_GROUP_M, TILE_GROUP_N, 1)))
kernel void gemm_nn (
    global const float * restrict A,
    int lda,    // column stride in elements for matrix A
    global const float * restrict B,
    int ldb,    // column stride in elements for matrix B
    global float * restrict C,
    int ldc,    // column stride in elements for matrix C
    int k,
    float alpha,
    float beta
)
{
    // Indices for matrices A and B are calculated differently
    // because they have the same format (both column-major) and
    // matrix multiplication involves "natural transpose" for
    // one of the matrix.

    int Aind = get_group_id(0)*TILE_GROUP_M*TILE_SIZE_M + get_local_id(0);
    int Bind = get_group_id(1)*TILE_GROUP_N*TILE_SIZE_N + get_local_id(1);
    int Cind = Aind + Bind*ldc;

    Bind *= ldb;    // matrix B is in column-major form

    float c[TILE_SIZE_M*TILE_SIZE_N] = {(float)0};

    // Main accumulation loop
    for(int l_block = 0; l_block < k; l_block += TILE_SIZE_K)
    {
        for(int i = 0; i < TILE_SIZE_M; ++i){
            for(int j = 0; j < TILE_SIZE_N; ++j){
                for(int l = 0; l < TILE_SIZE_K; ++l){
                    c[i*TILE_SIZE_N + j] +=
                        A[Aind + l*lda + i*TILE_GROUP_M] *
                        B[Bind + l + j*ldb*TILE_GROUP_N];
                } 
            }           
        }
                        
        Aind += lda*TILE_SIZE_K;
        Bind += TILE_SIZE_K;
    }

    // Store accumulated results from c to C with alpha and beta multiplication
    for(int i = 0; i < TILE_SIZE_M; ++i)
        for(int j = 0; j < TILE_SIZE_N; ++j)
        {
            int Ccur = Cind + i*TILE_GROUP_M + j*TILE_GROUP_N*ldc;
            C[Ccur] = alpha*c[i*TILE_SIZE_N + j];// + beta*C[Ccur];
        }
}