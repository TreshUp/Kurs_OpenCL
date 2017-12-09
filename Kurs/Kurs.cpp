#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <math.h>

#define N 5
//#define NZ 121
#define local_NZ 3
//#define VECTOR_SIZE 100000
#define MAX_SOURCE_SIZE (0x100000)

using namespace std;
struct crsMatrix
{
	// Массив номеров столбцов (размер NZ)
	int* Col;
	// Массив индексов строк (размер N + 1)
	int* RowIndex;
	// Массив значений (размер NZ)
	double* Value;
};

void InitializeMatrix(crsMatrix &mtx)
{
	mtx.Value = new double[local_NZ*N];
	mtx.Col = new int[local_NZ*N];
	mtx.RowIndex = new int[N + 1];
}
void FreeMemory(crsMatrix &mtx)
{
	delete[] mtx.Value;
	delete[] mtx.Col;
	delete[] mtx.RowIndex;
}
void Generate(crsMatrix &mtx)
{
	int* Num_Non_zero;
	Num_Non_zero = new int[N];
	for (int c = 0; c < N; c++)
	{
		Num_Non_zero[c] = local_NZ;
	}
	ofstream fout;
	fout.setf(ios::fixed);
	fout.precision(1);
	fout.open("MTX.txt", ios::out);
	int count = 0;
	mtx.RowIndex[0] = 0;
	//srand(time(NULL));
	int i,j = 0;
	for (i = 0; i < N; i++)
	{
		int k = 0;
		int	tmp = i;
		int flag = 1;
		while (k < Num_Non_zero[i])
		{
			if (flag == 1)
			{
				mtx.Value[j] = rand() % 5 + 1;
				fout << " " << mtx.Value[j];
				mtx.Col[j] = i;
				fout << "( " << mtx.Col[j] << " )";
				count++;
				j++;
				k++;
				flag = 0;
				Num_Non_zero[i]--;
				//tmp = i;
			}
			if (Num_Non_zero[i] != 0)
			{
				if ((N - 1 - tmp) <= local_NZ)
				{
					//
					if (i + local_NZ >= N) tmp = N - 1;
					else tmp = i + local_NZ;

				}
				else tmp = tmp + rand() % ((N - tmp) / 3) + 1;
				mtx.Value[j] = rand() % 5 + 1;
				fout << " " << mtx.Value[j];
				mtx.Col[j] = tmp;
				fout << "( " << mtx.Col[j] << " )";
				count++;
				Num_Non_zero[tmp]--;
				j++;
				k++;
			}
		}
		/*if (Num_Non_zero[i] == 0)
		{
			mtx.Value[j] = 0;
			fout << " " << mtx.Value[j];
			mtx.Col[j] = i;
			fout << "( " << mtx.Col[j] << " )";
		}*/
		fout << endl;
		mtx.RowIndex[i+1] = count;
	}
	fout << "end";
	fout.close();
}

void Len_Generate(crsMatrix &mtx)
{
	
	ofstream fout;
	fout.setf(ios::fixed);
	fout.precision(0);
	fout.open("MTX_New.txt", ios::out);
	int i, j = 0;
	mtx.RowIndex[0] = 0;
	for (i = 0; i < N-1; i++)
	{
		int  Num_Non_zero = 1 + local_NZ / 2;
		mtx.RowIndex[i + 1] = mtx.RowIndex[i] + Num_Non_zero;
		int	tmp = i;
		while (Num_Non_zero != 0)
		{
			mtx.Value[j] = rand() % 5 + 1;
			mtx.Col[j] = tmp;
			Num_Non_zero--;
			tmp += 1;
			j++;
		}
	}
	mtx.RowIndex[N] = mtx.RowIndex[N-1]  + local_NZ / 2;
	mtx.Value[j] = rand() % 5 + 1;
	mtx.Col[j] = N-1;
	int j1, j2,k;
	for (i = 0; i < N-1; i++)
	{
		j1 = mtx.RowIndex[i];
		j2 = mtx.RowIndex[i+1]-1;
		k = i;
		while (k != 0)
		{
			fout << "-"<<" ";
			k--;
		}
		for (int j = j1; j <= j2; j++)
		{
			fout << mtx.Value[j] << "( "<<mtx.Col[j]<<" ) ";
		}
		fout << endl;
	}
	j1 = mtx.RowIndex[N - 1];
	k = N-1;
	while (k != 0)
	{
		fout << "-" << " ";
		k--;
	}
	fout<< mtx.Value[j1] << "( " << mtx.Col[j] << " ) "<<endl;
	fout.close();
}
int main()
{
	crsMatrix mtx;
	//Выделение памяти
	InitializeMatrix(mtx);

	Generate(mtx);
	Len_Generate(mtx);
	for (int i = 0; i < N + 1; i++)
	{
		cout<<mtx.RowIndex[i]<<endl;
	}
#pragma region OpenCL
#pragma region READ
	//Это все для чтения кернела из файла Hello.cl
	FILE *fp;
	const char fileName[] = "../Kurs/Hello.cl";
	size_t source_size;
	char *source_str;
	int i;

	try {
		fp = fopen(fileName, "r");
		if (!fp) {
			fprintf(stderr, "Failed to load kernel.\n");
			exit(1);
		}
		source_str = (char *)malloc(MAX_SOURCE_SIZE);
		source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);//считывает массив размером MAX_SOURCE_SIZE, каждый размеров  1 // возвращает число успешно считанных элементов
		fclose(fp);
	}
	catch (int a) {
		printf("%f", a);
	}
#pragma endregion READ
	cout << "GET AND SET PLATFORM" << endl;
	// Get platform and device information
	cl_platform_id * platforms = NULL; //массив найденных платформ
	cl_uint num_platforms; //кол-во платформ

	//Set up the Platform
	cl_int clStatus = clGetPlatformIDs(0, NULL, &num_platforms); //первый параметр - число платформ на "add"; на выходе в num_platforms записано число платформ
	platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id)*num_platforms); //динамическое выделение памяти
	clStatus = clGetPlatformIDs(num_platforms, platforms, NULL); //получение списка доступных платформ; на выходе в массиве содер. все платформы

	//Get the devices list and choose the device you want to run on
	cl_device_id *device_list = NULL; //массив найденных устройств
	cl_uint num_devices; //кол-во устройств
	clStatus = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices); //первый параметр - какая платформа; второй - тип; третий - число устройств на добавление; на выходе в num_devices записано число устройств
	device_list = (cl_device_id *)malloc(sizeof(cl_device_id)*num_devices);//динамическое выделение памяти
	clStatus = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, num_devices, device_list, NULL); //получение списка доступных устройств; на выходе в массиве содер. все устройства

	cout << "CREATE CONTEXT" << endl;
	// Create one OpenCL context for each device in the platform
	cl_context context;
	context = clCreateContext(NULL, num_devices, device_list, NULL, NULL, &clStatus); //создание контекста
																						  // Create a command queue
	cl_command_queue command_queue = clCreateCommandQueue(context, device_list[0], 0, &clStatus); // создание очереди из команд для устройства;properties - список свойств очереди команд

	cout << "CREATE MEMORY" << endl;
	// Create memory buffers on the device for each vector
    //выделение памяти для каждого из векторов
	cl_mem Value_clmem = clCreateBuffer(context, CL_MEM_READ_WRITE, local_NZ*N * sizeof(double), NULL, &clStatus);
	cl_mem Col_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY, local_NZ*N * sizeof(int), NULL, &clStatus);
	cl_mem Row_Index_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY, (N + 1) * sizeof(int), NULL, &clStatus);

	cout << "COPY MATRIX TO DEVICE" << endl;
	// Copy the Buffer A and B to the device
	//копирование векторов А и В из памяти хоста в буфер; CL_TRUE - вектор А мб использован и после вызова функции clEnqueueWriteBuffer
	clStatus = clEnqueueWriteBuffer(command_queue, Value_clmem, CL_TRUE, 0, local_NZ*N * sizeof(double), mtx.Value, 0, NULL, NULL);
	clStatus = clEnqueueWriteBuffer(command_queue, Col_clmem, CL_TRUE, 0, local_NZ*N * sizeof(int), mtx.Col, 0, NULL, NULL);
	clStatus = clEnqueueWriteBuffer(command_queue, Row_Index_clmem, CL_TRUE, 0, (N + 1) * sizeof(int), mtx.RowIndex, 0, NULL, NULL);

	cout << "CREATE 2 PROGRAMMS" << endl;
	// Create a program from the kernel source
	//создание программы, используя контекст, код функции-вычисления
	cl_program program1 = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &clStatus);
	//cl_program program2 = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &clStatus);

	cout << "BUILD PROGRAM" << endl;
	// Build the program
	//компилирование и линковка программы
	//первый параметр - программа; второй -число устройств; третий - список устройств; пятый - If pfn_notify is NULL, clBuildProgram does not return until the build has completed.
	clStatus = clBuildProgram(program1, 1, device_list, NULL, NULL, NULL);
	//clStatus = clBuildProgram(program2, 1, device_list, NULL, NULL, NULL);

	cout << "CREATE 2 KERNELS" << endl;
	// Create the OpenCL kernel
	//второй параметр - название функции, объявленной в программе как __kernel; 
	cl_kernel kernel_fwd1 = clCreateKernel(program1, "square_fwd1", &clStatus);
	
	//cl_kernel kernel_fwd2 = clCreateKernel(program2, "square_fwd2", &clStatus);

	cout << "ARGUMENTS" << endl;
	// Set the arguments of the kernel
	clStatus = clSetKernelArg(kernel_fwd1, 0, sizeof(cl_mem), (void *)&Value_clmem);
	clStatus = clSetKernelArg(kernel_fwd1, 1, sizeof(cl_mem), (void *)&Col_clmem);
	clStatus = clSetKernelArg(kernel_fwd1, 2, sizeof(cl_mem), (void *)&Row_Index_clmem);


	//clStatus = clSetKernelArg(kernel_fwd2, 0, sizeof(cl_mem), (void *)&Value_clmem);
	//clStatus = clSetKernelArg(kernel_fwd2, 1, sizeof(cl_mem), (void *)&Col_clmem);
	//clStatus = clSetKernelArg(kernel_fwd2, 2, sizeof(cl_mem), (void *)&Row_Index_clmem);
	///clStatus = clSetKernelArg(kernel_fwd1, 3, sizeof(cl_mem), (void *)&);
	
	size_t m;
	int count = mtx.RowIndex[N];
	
	float *C = (float*)calloc(local_NZ*N,sizeof(float));
	/*for (int k = 0; k < local_NZ*N; k++)
	{
		C[k] = mtx.Value[k];
	}*/
	cl_mem C_clmem = clCreateBuffer(context, CL_MEM_READ_WRITE, local_NZ*N * sizeof(float), NULL, &clStatus);

	clStatus = clSetKernelArg(kernel_fwd1, 4, sizeof(cl_mem), (void *)&C_clmem);
	size_t global_size = 5; // Process the entire lists
	size_t local_size = 1; // Process one item at a time
	//clStatus = clEnqueueNDRangeKernel(command_queue, kernel_fwd1, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
	
	m = 192 * (count / 192);
	cout << "START" << endl;
	for (i = 0; i < 5; i++) {
		//square_fwd1(mtx.Value, mtx.Col, mtx.RowIndex, i);
		clStatus = clSetKernelArg(kernel_fwd1, 3, sizeof(int), (void *)&i);
		clStatus = clEnqueueNDRangeKernel(command_queue, kernel_fwd1, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
		cout << clStatus << endl;
		size_t len = 0;
		cl_int ret = CL_SUCCESS;
		ret = clGetProgramBuildInfo(program1, device_list[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
		char *buffer = (char*)calloc(len, sizeof(char));
		ret = clGetProgramBuildInfo(program1, device_list[0], CL_PROGRAM_BUILD_LOG, len, buffer, NULL);
		printf("\n\nBuildlog:   %s\n\n", buffer);
		////clStatus = clSetKernelArg(kernel_fwd1, 3, sizeof(int), (void *)&i);
		////clStatus = clEnqueueNDRangeKernel(command_queue, kernel_fwd1, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
		////cout << clStatus << endl;
		////size_t tmp = 192;
		//clStatus = clSetKernelArg(kernel_fwd2, 3, sizeof(int), (void *)&i);
		//clStatus = clEnqueueNDRangeKernel(command_queue, kernel_fwd2, 1, (0, 0, 0), &m, &tmp, 0, NULL, NULL);
		///square_fwd2(mtx.Value,mtx.Col, mtx.RowIndex, i);
		//square_fwd2.setGlobalWorkSize(m);
		//square_fwd2.setGlobalWorkOffset(0, 0, 0);
		//square_fwd2.setLocalWorkSize(192);
		//square_fwd2(buffA, n + 1, i);
		cout << "DONE 1_______________" << endl;

		/*if (count % 192) {
			size_t tmp_1 = count % 192;
			clStatus = clSetKernelArg(kernel_fwd2, 3, sizeof(int), (void *)&i);
			clStatus = clEnqueueNDRangeKernel(command_queue, kernel_fwd2, 1, (m, 0, 0), &tmp_1, &tmp_1, 0, NULL, NULL);
			//square_fwd2.setGlobalWorkSize(n % 192);
			//square_fwd2.setGlobalWorkOffset(m, 0, 0);
			//square_fwd2.setLocalWorkSize(n % 192);
			//square_fwd2(buffA, n + 1, i);
		}
		cout << "DONE 2_______________" << endl;*/
	}
	clStatus = clEnqueueReadBuffer(command_queue, Value_clmem, CL_TRUE, 0, local_NZ*N * sizeof(float), mtx.Value, 0, NULL, NULL);
	//clStatus = clEnqueueReadBuffer(command_queue, C_clmem, CL_TRUE, 0, 5 * sizeof(float), C, 0, NULL, NULL);
	cout << "FINAL VYVOD" << endl;
	for (i = 0; i<mtx.RowIndex[N]; i++)
	{
		cout <<i+1<<") "<< mtx.Value[i]<<endl << "-------" << endl;
		//cout << i + 1 << ") " << C[i] << endl << "-------"<<endl;
	}
	// Clean up and wait for all the comands to complete.
	//все команды добавлены в очередь
	clStatus = clFlush(command_queue);
	//выполнение всех оставшихся команд
	clStatus = clFinish(command_queue);

	clStatus = clReleaseKernel(kernel_fwd1);
	//clStatus = clReleaseKernel(kernel_fwd2);
	clStatus = clReleaseProgram(program1);
	//clStatus = clReleaseProgram(program2);
	clStatus = clReleaseMemObject(Value_clmem);
	clStatus = clReleaseMemObject(Col_clmem);
	clStatus = clReleaseMemObject(Row_Index_clmem);
	clStatus = clReleaseCommandQueue(command_queue);
	clStatus = clReleaseContext(context);
	free(mtx.Value);
	free(mtx.Col);
	free(mtx.RowIndex);
	free(platforms);
	free(device_list);
#pragma endregion
	
	//Очистка памяти
	system("pause");
	return 0;
}