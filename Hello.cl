__kernel void square_fwd1(__global float* Value, __global float* RowIndex, __global float* Col, int j, __global float* C) 
 { 
	 C[j]=7;
 }