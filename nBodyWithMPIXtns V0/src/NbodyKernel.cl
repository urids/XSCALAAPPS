typedef struct entityType_st{
	float P[4];
	float V[3];
	float nP[4];
	float nV[3];
}entityType;

#define UNROLL_FACTOR  8
__kernel void nbody_sim( __global entityType * entities, const int numBodies , const float deltaTime, const float epsSqr, int numTasks) {

    unsigned int gid = get_global_id(0);
  
if (gid < (numBodies/numTasks)){  
   int k;
   for(k=0;k<100;k++){ //wasting-time loop
   
    float4 myPos = {entities[gid].P[0],entities[gid].P[1],entities[gid].P[2],entities[gid].P[3]};
    float4 acc = (float4)0.0f;
	int i=0;
	
	
    for (; (i+UNROLL_FACTOR) < (numBodies/numTasks); ) {   
    #pragma unroll UNROLL_FACTOR
        for(int j = 0; j < UNROLL_FACTOR; j++,i++) {
            float4 p = {entities[i].P[0],entities[i].P[1],entities[i].P[2],entities[i].P[3]};
            float4 r;
            r.xyz = p.xyz - myPos.xyz;
            float distSqr = r.x * r.x  +  r.y * r.y  +  r.z * r.z;
            float invDist = 1.0f / sqrt(distSqr + epsSqr);
            float invDistCube = invDist * invDist * invDist;
            float s = p.w * invDistCube;
            // accumulate effect of all particles
            acc.xyz += s * r.xyz;           
        }
    }
    float3 oldVel = {entities[gid].V[0],entities[gid].V[1],entities[gid].V[2]};
    // updated position and velocity
    float4 newPos;
    newPos.xyz = myPos.xyz + oldVel.xyz * deltaTime + acc.xyz * 0.5f * deltaTime * deltaTime;
        
    float3 newVel;
    newVel.xyz = oldVel.xyz + acc.xyz * deltaTime;

    // write to global memory

    entities[gid].nP[0] = newPos.x;
    entities[gid].nP[1] = newPos.y;
    entities[gid].nP[2] = newPos.y;
    
	entities[gid].nV[0] = newVel.x;
	entities[gid].nV[1] = newVel.y;
	entities[gid].nV[2] = newVel.z;
	

	}
  }	
}