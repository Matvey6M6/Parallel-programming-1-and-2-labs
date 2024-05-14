#include<direct.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <conio.h>
#include <random>
#include <chrono>
#include <string>
#include<windows.h>

#include <omp.h>

using namespace std;


void FillMat(int n, int*** Mat) {
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			(*Mat)[i][j] = rand() % (100 + 100 + 1) - 100;
		}
	}
}

int** Reader(const string filename) {
	fstream fin;
	fin.open(filename);

	int n;
	fin >> n;

	int** Mat = new int* [n];
	for (int i = 0; i < n; i++)
	{
		Mat[i] = new int[n];
	}

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++) {
			fin >> Mat[i][j];
		}
	}

	return Mat;
}

void Writer(int n, int** Mat, const string filename)
{
	fstream fout;
	fout.open(filename, ofstream::out | ofstream::trunc);
	fout << n << endl;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			fout << Mat[i][j] << ' ';
		}
		fout << endl;
	}
}

int** Multiplier(int n, int** Mat1, int** Mat2, int threads) 
{
	int** Mat_res = new int* [n];
	for (int i = 0; i < n; i++)
	{
		Mat_res[i] = new int[n];
	}
	omp_set_num_threads(threads);
#pragma omp parallel for 
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			Mat_res[i][j] = 0;
			for (int k = 0; k < n; k++)
			{
				Mat_res[i][j] += Mat1[i][k] * Mat2[k][j];
			}
		}
	}
	return Mat_res;
}



int main() {
	srand(time(0));

	#ifdef _OPENMP
		std::cout
			<< "OpenMP Version: "
			<< _OPENMP / 100 << " (" << _OPENMP % 100 << ")" << std::endl;
	#else
		std::cout << "Sequential Version" << std::endl;
	#endif
	std::cout
		<< "Processors: " << omp_get_num_procs()
		<< ", Max threads: " << omp_get_max_threads()
		<< std::endl;
	
	const int target_thread_num = 10;
	omp_set_num_threads(target_thread_num);
	
	#pragma omp parallel 
		cout << "OpenMP Test" << endl;

	CreateDirectory(L"res_matrix", NULL);
	CreateDirectory(L"1_matrix", NULL);
	CreateDirectory(L"2_matrix", NULL);

	int** Mat1 = NULL;
	int** Mat2 = NULL;
	int** Mat_res = NULL;

	int num_of_threads[4] = {2, 4, 6, 8};
	for (int threads = 0; threads < 4; threads ++)
	{
		cout << "\nNum of threads = " << num_of_threads[threads] << endl;
		fstream fout;
		fout.open(string("times_thread_") + to_string(num_of_threads[threads]) + string(".txt"), ofstream::out | ofstream::trunc);
		double times[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		for (int n = 400; n <= 2000; n += 400)
		{
			cout << n << endl;


			Mat1 = new int* [n];
			Mat2 = new int* [n];
			Mat_res = new int* [n];

			for (int i = 0; i < n; i++)
			{
				Mat_res[i] = new int[n];
				Mat1[i] = new int[n];
				Mat2[i] = new int[n];
			}
			for (int i = 1; i <= 10; i++)
			{
				cout << '\t' << i;
				FillMat(n, &Mat1);
				FillMat(n, &Mat2);

				auto start = chrono::steady_clock::now();
				Mat_res = Multiplier(n, Mat1, Mat2, num_of_threads[threads]);
				auto end = chrono::steady_clock::now();

				times[i - 1] = double(chrono::duration_cast<chrono::milliseconds>(end - start).count());
			}

			Writer(n, Mat_res, string("res_matrix/res_matrix") + to_string(n) + string(".txt"));
			Writer(n, Mat1, string("1_matrix/1_matrix") + to_string(n) + string(".txt"));
			Writer(n, Mat2, string("2_matrix/2_matrix") + to_string(n) + string(".txt"));
			fout << n << ';';
			for (int i = 0; i < 10; i++)
				fout << times[i] << ';';
			fout << endl;
			cout << endl;

			for (int i = 0; i < n; i++)
			{
				delete[] Mat_res[i];
				delete[] Mat1[i];
				delete[] Mat2[i];
			}

			delete[] Mat_res;
			delete[] Mat1;
			delete[] Mat2;

			Mat_res = NULL;
			Mat1 = NULL;
			Mat2 = NULL;
		}
		fout.close();
	}

	return 0;
}