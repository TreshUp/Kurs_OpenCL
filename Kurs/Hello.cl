__kernel void square_fwd1(__global double* Value, __global int* Col, __global int* RowIndex, int j,__global float* C) 
 { 
	 //Value[RowIndex[j]]=sqrt(fabs(Value[RowIndex[j]]))+2;
	 //Value[RowIndex[j]]=15;
	 Value[RowIndex[j]]=sqrt(fabs(Value[RowIndex[j]]));
	 //C[j]=1*C[j];
	 //C[j]=sqrt(fabs(C[j]));
 }
 /*__kernel void square_fwd2(__global float* Value, __global int* Col, __global int* RowIndex, int j) 
 { 
	 int i = get_global_id(0); 
	//TO DO
     if(j > i) { 
         //float s = A[j+i*n];
		 float s = Value[RowIndex[i]+j - RowIndex[i]];
 
         for(int k = 0; k < i; k++) { 
             //s -= A[i+k*n]*A[j+k*n]; 
			 s -= Value[RowIndex[i]+k - RowIndex[i]]*Value[RowIndex[j]+k - RowIndex[j]];
         } 
 
 
         //s /= A[i+i*n];
		s/=Value[RowIndex[i]];
         //A[j+i*n] = s; 
		 Value[RowIndex[i]+j - RowIndex[i]] = s;
 
 
         //A[j+j*n] -= s*s;
		 Value[RowIndex[j]] -= s*s;
     } 
 }*/