
#include <immintrin.h>
#include <iostream>

int main () {
	float array0 [ 4 ] __attribute__(( aligned(16))) = {0.0f ,  1.0f ,  2.0f ,  3.0f};
	float array1 [ 4 ] __attribute__(( aligned(16)));

	__m128 r0 = _mm_load_ps(array0);
	_mm_store_ps(array1,r0);
	
	for(auto x:array1){
		std::cout << x << std::endl;
	}
	
	return 0;
}
