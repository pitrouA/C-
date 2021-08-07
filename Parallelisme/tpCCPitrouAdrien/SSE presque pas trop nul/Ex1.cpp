
#include <immintrin.h>
#include <iostream>
#include <math.h>
#include <array>
#include <vector>
#include <time.h>
#include <chrono>

using namespace std;

/***
* Remplit le tableau avec des valeurs aleatoires
**/
template<typename T, int n>
void fillVector(T** array){
	(*array) = (T*)_mm_malloc(sizeof(T) * n,16);

	for(int i=0;i<n;i++){
		(*array)[i] = T(rand()%10);
	}
}

/***
* Affiche un vecteur
**/
template<typename T,int n>
void printVector(T* array){
	cout << "[";
	for(int i=0;i<n;i++){
		cout << array[i] << ", ";
	}
	cout << "]" << endl;
}

/***
* Calcule le produit de 2 vecteurs
**/
double* dmult(double* vect0, double* vect1, int n){

	double* vectMul __attribute__(( aligned(16))) = (double*)_mm_malloc(sizeof(double) * n,16);

	for(int i=0;i<n;i+=2){
		__m128d r0 = _mm_load_pd(vect0 + i);
		__m128d r1 = _mm_load_pd(vect1 + i);
		__m128d rmul = _mm_mul_pd(r0, r1);
		_mm_store_pd(vectMul + i,rmul);
	}

	return vectMul;
}

int main () {

	const int n = 12;
	srand(time(NULL));

	double* v0 __attribute__(( aligned(16)));
	double* v1 __attribute__(( aligned(16)));

	fillVector<double,n>(&v0);
	fillVector<double,n>(&v1);

	printVector<double, n>(v0);
	printVector<double, n>(v1);

	double* res = dmult(v0,v1,n);

	cout << "Resultat multiplication : " << endl;
	printVector<double, n>(res);

	_mm_free(v0);
	_mm_free(v1);
	_mm_free(res);

	return 0;
}

//-------------------------------
// g++ -o executable Ex1.cpp
// ./executable
//-------------------------------
