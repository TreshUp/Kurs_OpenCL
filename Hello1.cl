/*__kernel void VectorAdd(float alpha, __global float* a,__global float* b,__global float* c)
{
	//get index into global data array
	int index=get_global_id(0); //Returns the unique global work-item ID value for dimension;
	//add the vector elements
	c[index]=alpha*a[index]+b[index];
}*/

__kernel void square_fwd1(global float* Value, global float* RowIndex, global float* Col, int j) 
 { 
     //A[i+i*n] = sqrt(fabs(A[i+i*n]));
	 Value[RowIndex[j]]=sqrt(fabs(Value[RowIndex[j]]));
	 ////Value[RowIndex[j+j]]=sqrt(fabs(Value[RowIndex[j+j]]));
	 ////int position=RowIndex[j];
	 ////while (position<=(RowIndex[j+1]-1))
	 ////{
		////if Col[position]==j break;
	 ////}
	 ////Value[position]=sqrt(fabs(Value[position]));
 } 
 
 
__kernel void square_fwd2(global float* Value, global float* RowIndex, global float* Col, int j) 
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
 } 
 
 
/*__kernel void square_y1(global float *U, global float *x, int n, int i) 
 { 
     x[i] = U[n-1+i*n] / U[i+i*n]; 
 } 
 
 
__kernel void square_y2(global float *U, global float *x, int n, int i) 
 { 
     local float y; 
     y = x[i]; 
 
 
     int k = get_global_id(0); 
 

     if(k > i) { 
         U[n-1+k*n] -= U[k+i*n] * y; 
     } 
 } 
 
 
__kernel void square_x1(global float *U, global float *x, int n, int i) 
 { 
     x[i] /= U[i+i*n]; 
 } 
 
 
__kernel void square_x2(global float *U, global float *x, int n, int i) 
 { 
     local float y; 
     y = x[i]; 
 
 
     int k = get_global_id(0); 
 
 
     x[k] -= y * U[i+k*n]; 
 } 
*/