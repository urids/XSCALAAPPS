
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable

__kernel void Accum(__global int* token, const int TaskId){
atom_inc(token);
}

__kernel void Sum(__global int* C, __global int* A,__global int* B){
int thID=get_global_id(0);

	if (thID==0){
		*C=*A+*B;
	}

}