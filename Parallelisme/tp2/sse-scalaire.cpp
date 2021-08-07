
#include <immintrin.h>
#include <iostream>
#include <math.h>
#include <array>
#include <vector>
#include <time.h>
#include <chrono>
#include <algorithm>

using namespace std;

/***
* Affiche un array
**/
template<size_t n>
void printMatrix(float array[n*n]){
	std::string stuff(n*3, '-');
	cout << stuff << endl;
	for(int i=0;i<n;i++){
		cout << "[";
		for(int j=0;j<n;j++){
			cout << array[i*n+j] << ", ";
		}
		cout << "]" << endl;
	}
	cout << stuff << endl;
}

/***
* Transpose
**/
template<size_t dim>
float* transpose(float A[dim*dim]){
	float* B __attribute__(( aligned(16))) = new float[dim * dim];

	for(size_t i=0;i<dim;++i){
		for(size_t j=0;j<dim;++j){
			B[j*dim + i] = A[i*dim + j];
		}
	}

	return B;
}

/***
* Fait le prod scal de 2 matrices
**/
template<size_t dim>
float* scalarProduct(float A[dim*dim], float B[dim*dim]){

	float* C = new float[dim * dim];

	for(size_t i=0;i<dim;++i){
		for(size_t j=0;j<dim;++j){
			C[i*dim+j] = 0;
		}
	}

	for(size_t i=0;i<dim;++i){
		for(size_t j=0;j<dim;++j){
			for(size_t k=0;k<dim;++k){
				C[i*dim+j] += A[i*dim+k] * B[k*dim+j];
			}
		}
	}

	return C;
}

/***
* Fait le prod scal de 2 matrices
**/
template<size_t dim>
float* scalarProductSSE(float A[dim*dim], float B[dim*dim]){

	float* C __attribute__(( aligned(16)))  = new float[dim * dim];
	float* tB = transpose<dim>(B);
	float* tA = transpose<dim>(A);

	for(size_t i=0;i<dim;++i){
		for(size_t j=0;j<dim;++j){
			C[i*dim+j] = 0;
		}
	}

	//printMatrix<dim>(A);
	printMatrix<dim>(tB);

	for(size_t i=0;i<dim;++i){
		for(size_t j=0;j<dim;++j){ //lb, lta
			for(size_t k=0;k<dim;++k){
				C[i*dim+j] += A[i*dim+k] * tB[j*dim+k];
			}
			/*for(size_t k=0;k<dim;k+=4){

				__m128 A_sse = _mm_load_ps(A+(i*dim+k));
				cout << "i=" << i << " j=" << j << " k=" << k << endl;
				__m128 B_sse = _mm_load_ps(tB+(j*dim+k));
				cout << "i=" << i << " j=" << j << " k=" << k << endl;
				__m128 C_sse = _mm_load_ps(C+(i*dim+j));
				cout << "i=" << i << " j=" << j << " k=" << k << endl;

				__m128 mul = _mm_mul_ps(A_sse, B_sse);
				C_sse = _mm_add_ps(C_sse, mul);

				cout << "s1" << endl;
				_mm_store_ps(C+(i*dim+j),C_sse);
				cout << "s2" << endl;
			}*/
		}
	}

	delete [] tA;
	delete [] tB;

	return C;
}

int main () {

	const size_t dim=8;
	const int n = 2;

	srand(time(NULL));

	float* A __attribute__(( aligned(16)))  = new float[dim * dim];
	float* B __attribute__(( aligned(16)))  = new float[dim * dim];

	generate_n( A, dim*dim, [](){return rand ()%n;});
	generate_n( B, dim*dim, [](){return rand ()%n;});

	//array<float, n> array0 __attribute__(( aligned(16)));
	//array<float, n> array1 __attribute__(( aligned(16)));

	//printTab<float, n>(array0);

	printMatrix<dim>(A);
	printMatrix<dim>(B);

	//----------------------------- test durees -----------------------------

	auto start = std::chrono::system_clock::now();
	float* D = scalarProductSSE<dim>(A, B);
	auto stop = std::chrono::system_clock::now();
	cout << "-- Mat scal SSE --" << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms" << endl;

	printMatrix<dim>(D);

	start = std::chrono::system_clock::now();
	float* C = scalarProduct<dim>(A, B);
	stop = std::chrono::system_clock::now();
	cout << "-- Mat scal --" << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms" << endl;

	printMatrix<dim>(C);

	//float* transpA = transpose<dim>(A);
	//printMatrix<dim>(transpA);
	//delete[] transpA;


	//---------------------------- delete --------------------------------

	delete [] A;
	delete [] B;
	delete [] C;
	delete [] D;


	return 0;
}

//-------------------------------
// g++ -O3 -o hhh sse-scalaire.cpp
//-------------------------------
