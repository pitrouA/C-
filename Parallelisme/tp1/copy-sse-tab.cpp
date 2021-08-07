
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
void fillTab(array<T, n>* array0){
	//array0 = (float*)_mm_malloc(sizeof(float) * n,16);

	for(int i=0;i<n;i++){
		(*array0)[i] = T(rand()%55 + 1);
	}

	//return array0;
}

/***
* Fait la somme des valeurs du tableau avec sse
**/
template<int n>
float sumFloat(array<float, n> array){

	float ret[4];

	__m128 sum = _mm_load_ps(array.data());
	__m128 extracted;

	for(int i=4;i<n;i+=4){
		extracted = _mm_load_ps(array.data()+i);
		sum = _mm_add_ps(sum, extracted);
	}

	_mm_store_ps(ret,sum);

	return ret[0]+ret[1]+ret[2]+ret[3];
}

/***
* Fait la somme des valeurs du tableau avec sse
**/
template<int n>
double sumDouble(array<double, n> array){

	double ret[2];

	__m128d sum = _mm_load_pd(array.data());
	__m128d extracted;

	for(int i=2;i<n;i+=2){
		extracted = _mm_load_pd(array.data()+i);
		sum = _mm_add_pd(sum, extracted);
	}

	_mm_store_pd(ret,sum);

	return ret[0]+ret[1];
}

/***
* Fait la somme des valeurs du tableau avec sse
**/
template<int n>
float sumFloatRegular(array<float, n> array){

	float sum = 0.0;

	for(int i=0;i<n;i++){
		sum += array[i];
	}

	return sum;
}

/***
* Fait la somme des valeurs du tableau avec sse
**/
/*template<typename T, int n>
T sum(array<T, n> array){

	double h;
	T ret[4];

	if(typeid(T) == typeid(h)){
		__m128d sum = _mm_load_pd(array.data());
		__m128d extracted;

		for(int i=4;i<n;i+=4){
			extracted = _mm_load_pd(array.data()+i);
			sum = _mm_add_pd(sum, extracted);
		}

		_mm_store_pd(ret,sum);
	}else{
		__m128 sum = _mm_load_ps(array.data());
		__m128 extracted;

		for(int i=4;i<n;i+=4){
			extracted = _mm_load_ps(array.data()+i);
			sum = _mm_add_ps(sum, extracted);
		}

		_mm_store_ps(ret,sum);
	}

	return ret[0]+ret[1]+ret[2]+ret[3];
}*/

/***
* Affiche un array
**/
template<typename T,int n>
void printTab(array<T, n> array){
	cout << "[";
	for(T x:array){
		cout << x << ", ";
	}
	cout << "]" << endl;
}

/***
* Trouve le max du tableau
**/
template<typename T,int n>
int max(array<T, n> array){

	__m128 max = _mm_load_ps(array.data());
	__m128 extracted;

	for(int i=4;i<n;i+=4){
		extracted = _mm_load_ps(array.data()+i);
		max = _mm_max_ps(max, extracted);
	}

	T ret[4];
	_mm_store_ps(ret,max);

	if(ret[0] > ret[1]){
		if(ret[0] > ret[2]){
			if(ret[0] > ret[3]){
				return ret[0];
			}else{
				return ret[3];
			}
		}else{
			if(ret[2] > ret[3]){
				return ret[2];
			}else{
				return ret[3];
			}
		}
	}else{
		if(ret[1] > ret[2]){
			if(ret[1] > ret[3]){
				return ret[1];
			}else{
				return ret[3];
			}
		}else{
			if(ret[2] > ret[3]){
				return ret[2];
			}else{
				return ret[3];
			}
		}
	}
}

void calcPd(){

}

/***
* Calcule le produit scalaire de 2 vecteurs
**/
template<int n>
float prodScalFloat(array<float, n> vect0,array<float, n> vect1){

	array<float, n> vectMul;

	for(int i=0;i<n;i++){
		__m128 r0 = _mm_load_ps(vect0.data() + i);
		__m128 r1 = _mm_load_ps(vect1.data() + i);
		__m128 rmul = _mm_mul_ps(r0, r1);
		_mm_store_ps(vectMul.data() + i,rmul);
	}

	return sumFloat<n>(vectMul);
}

/***
* Calcule le produit scalaire de 2 vecteurs
**/
template<int n>
float prodScalDouble(array<double, n> vect0,array<double, n> vect1){

	array<double, n> vectMul;

	for(int i=0;i<n;i+=2){
		__m128d r0 = _mm_load_pd(vect0.data() + i);
		__m128d r1 = _mm_load_pd(vect1.data() + i);
		__m128d rmul = _mm_mul_pd(r0, r1);
		_mm_store_pd(vectMul.data() + i,rmul);
	}

	//printTab<double, n>(vectMul);

	return sumDouble<n>(vectMul);
}

/***
* Calcule le produit scalaire de 2 vecteurs
**/
/*template<typename T,int n>
T prodScal(array<T, n> vect0,array<T, n> vect1){

	double h;

	array<T, n> vectMul;

	for(int i=0;i<n;i++){
		if(typeid(T) == typeid(h)){
			__m128d r0 = _mm_load_pd(vect0.data() + i);
			__m128d r1 = _mm_load_pd(vect1.data() + i);
			__m128d rmul = _mm_mul_pd(r0, r1);
			_mm_store_pd(vectMul.data() + i,rmul);
		}else{
			__m128 r0 = _mm_load_ps(vect0.data() + i);
			__m128 r1 = _mm_load_ps(vect1.data() + i);
			__m128 rmul = _mm_mul_ps(r0, r1);
			_mm_store_ps(vectMul.data() + i,rmul);
		}
	}

	return sum<double, n>(vectMul);
}*/

int main () {

	const int n=12;

	srand(time(NULL));

	array<float, n> array0 __attribute__(( aligned(16)));
	array<float, n> array1 __attribute__(( aligned(16)));

	fillTab<float, n>(&array0);

	for(int i=0;i<n;i+=4){
		__m128 r0 = _mm_load_ps(array0.data()+i);
		_mm_store_ps(array1.data()+i,r0);
	}

	printTab<float, n>(array1);

	cout << "Somme tableau : " << sumFloat<n>(array1) << endl;
	cout << "Max tableau : " << max<float, n>(array1) << endl;

	//----------------------------- test durees -----------------------------

	auto start = std::chrono::system_clock::now();
	sumFloatRegular<n>(array1);
	auto stop = std::chrono::system_clock::now();
	cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms" << endl;


	start = std::chrono::system_clock::now();
	sumFloat<n>(array1);
	stop = std::chrono::system_clock::now();
	cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms" << endl;



	//------------------------- doubles -------------------------

	array<double, n> vect0 __attribute__(( aligned(16)));
	array<double, n> vect1 __attribute__(( aligned(16)));

	fillTab<double,n>(&vect0);
	fillTab<double,n>(&vect1);

	printTab<double, n>(vect0);
	printTab<double, n>(vect1);

	cout << "Prod Scal : " << prodScalDouble<n>(vect0,vect1) << endl;

	//------------------------ int -------------------------------

	

	return 0;
}

//-------------------------------
// g++ -o hhh copy-sse-tab.cpp
//-------------------------------
