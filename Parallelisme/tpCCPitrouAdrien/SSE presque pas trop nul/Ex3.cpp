
#include <immintrin.h>
#include <iostream>
#include <math.h>
#include <array>
#include <vector>
#include <time.h>
#include <chrono>

using namespace std;

/***
* Affiche un vecteur
**/
template<typename T,unsigned int n>
void printVector(T* array){
	cout << "[";
	for(int i=0;i<n;i++){
		cout << array[i] << ", ";
	}
	cout << "]" << endl;
}

void iota_sse(short* input,unsigned int n){
	input = (short*)_mm_malloc(sizeof(short) * n,16);
	__m128i temp;

	for(int i=0;i<n;i+=8){//8 short dans un registre de taille 16
		temp = _mm_load_si128(input);
		temp[0] = i;
		temp[1] = i+1;
		temp[2] = i=2;
		temp[3] = i=3;
		temp[4] = i+4;
		temp[5] = i+5;
		temp[6] = i=6;
		temp[7] = i=7;
		_mm_store_si128(input,temp);
	}
}

int main () {

	const unsigned int n = 12;
	srand(time(NULL));

	short* input __attribute__(( aligned(16)));
	iota_sse(input,n);

	printVector<short, n>(input);

	_mm_free(input);

	return 0;
}

//-------------------------------
// g++ -o executable Ex2.cpp
// ./executable
//-------------------------------
