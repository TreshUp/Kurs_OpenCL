#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

#define N 10
//#define NZ 121
#define local_NZ 2
//#define VECTOR_SIZE 100000
//#define MAX_SOURCE_SIZE (0x100000)

using namespace std;
struct crsMatrix
{
	// Массив номеров столбцов (размер NZ)
	double* Col;
	// Массив индексов строк (размер N + 1)
	double* RowIndex;
	// Массив значений (размер NZ)
	double* Value;
};

void InitializeMatrix(crsMatrix &mtx)
{
	mtx.Value = new double[local_NZ*N];
	mtx.Col = new double[local_NZ*N];
	mtx.RowIndex = new double[N + 1];
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
				//if (((N - 1 - tmp) / 3 == 0) && ((N-1)!=tmp)) { cout << tmp << endl; tmp = tmp + rand() % (tmp % 3) + 1; } //tmp = tmp + 1; }
				//else
				//{
				/*int check = tmp + rand() % 3 + 1;
				while (1)
				{
					if (check>N-1) check= tmp + rand() % 3 + 1;
					else
					{
						tmp = check;//((N - tmp) / 3) + 1;
						break;
					}
				}*/
				//}

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
		if (Num_Non_zero[i] == 0)
		{
			mtx.Value[j] = 0;
			fout << " " << mtx.Value[j];
			mtx.Col[j] = i;
			fout << "( " << mtx.Col[j] << " )";
		}
		fout << endl;
		mtx.RowIndex[i+1] = count;
	}

	
	
	//for (i = 1; i < N + 1; i++)
	//{
		//mtx.RowIndex[i] = mtx.RowIndex[i - 1]  + 2- Num_Non_zero[i - 1];
	//}
	fout.close();
}

int main()
{
	crsMatrix mtx;
	//Выделение памяти
	InitializeMatrix(mtx);

	Generate(mtx);
	//mtx.RowIndex[0] = 0;
	for (int i = 0; i < N + 1; i++)
	{
		cout<<mtx.RowIndex[i]<<endl;
	}
	/*cout << "_______________"<<endl;
	//ofstream fout;
	//fout.setf(ios::fixed);
	//fout.precision(4);
	//fout.open("MTX1.txt", ios::out);
	//for (int i = 0; i < local_NZ*N-1; i++)
	//{
	int i = 0;
	while (1)
	{
		int stroc = mtx.RowIndex[i + 1] - mtx.RowIndex[i];
		while (stroc != 0)
		{
			cout << mtx.Value[i];
			int k = mtx.Col[i];
			while (k < mtx.Col[i + 1] - 1)
			{
				k++;
				cout << " 0 ";
			}
			stroc--;
			i++;
		}
		break;
	}
		cout <<endl << mtx.Value[i + 1]<< " ";
	//}
		//cout.close();*/
	//Очистка памяти
	FreeMemory(mtx);
	system("pause");
	return 0;
}