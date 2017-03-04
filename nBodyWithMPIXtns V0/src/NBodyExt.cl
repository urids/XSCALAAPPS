typedef struct entityType_st {
	float P[4];
	float V[3];
	float nP[4];
	float nV[3];
} entityType;

#define UNROLL_FACTOR  8
__kernel void computeForces( __global entityType * entities0, __global entityType * entities1 , const int numBodies , const float deltaTime, const float epsSqr, int numTasks) {

	unsigned int gid = get_global_id(0);
	int ePerTask=numBodies/numTasks;
	int k;

	float4 myPos = {entities0[gid].P[0],entities0[gid].P[1],entities0[gid].P[2],entities0[gid].P[3]};
	float4 acc = (float4)1.0f;
	int i=0;
							//(numBodies/numTasks)
	for (; (i+UNROLL_FACTOR) <(numBodies/numTasks); ) {
#pragma unroll UNROLL_FACTOR
		for(int j = 0; j < UNROLL_FACTOR; j++,i++) {
			float4 p = {entities1[i].P[0],entities1[i].P[1],entities1[i].P[2],entities1[i].P[3]};
			float4 r;
			r.xyz = p.xyz - myPos.xyz;  //[3 FLOP]
			float distSqr = r.x * r.x + r.y * r.y + r.z * r.z;//[5 FLOP]
			float invDist = 1.0f / sqrt(distSqr + epsSqr);//  [3 FLOP (1 sum, 1 sqrt, 1 inv)]
			float invDistCube = invDist * invDist * invDist;//[2 FLOP]
			float s = p.w * invDistCube;//[1 FLOP]
			// accumulate effect of all particles
			acc.xyz += s * r.xyz;//[6 FLOP]         
		}
	}

	float3 oldVel = {entities0[gid].V[0],entities0[gid].V[1],entities0[gid].V[2]};
	// updated position and velocity
	float4 newPos;
	newPos.xyz = myPos.xyz + oldVel.xyz * deltaTime + acc.xyz * 0.5f * deltaTime * deltaTime;//[6*3 FLOP]

	float3 newVel;
	newVel.xyz = oldVel.xyz + acc.xyz * deltaTime; //[2*3 FLOP]

	// write to global memory

	entities0[gid].nP[0] = newPos.x;
	entities0[gid].nP[1] = newPos.y;
	entities0[gid].nP[2] = newPos.z;

	entities0[gid].nV[0] = newVel.x;
	entities0[gid].nV[1] = newVel.y;
	entities0[gid].nV[2] = newVel.z;

}



