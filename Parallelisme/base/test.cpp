
#include <stdio.h>
#include <array>
#include <iostream>
#include <string>
#include <time.h>

/*template<int N, int M>
using Array2D = array<array<int, M>, N>;

void fillMatrix(std::array<std::array<int,512>,512> mat, int val){
	
	
	for(int i=0;i<512;i++){
		for(int j=0;j<512;j++){
			mat[i][j] = val;
		}
	}
}

std::string toStrMat(std::array<std::array<int,512>,512> mat){
	std::string str ("");
	
	for(int i=0;i<512;i++){
		for(int j=0;j<512;j++){
			str = str + std::to_string(mat[i][j]) + ",";
		}
	}
	
	return str;
}

template<int N, int M>
void printArray2D(const Array2D<N, M> &arr) {
    for (const auto &row : arr) {
        for (const auto &elem : row) {
            cout << std::setw(3) << elem;
        }
        cout << endl;
        cout << endl;
    }
}

int main ( int argc , char * argv[] ) {
	
	//int mat[512][512];
	
	const int N = 5;
    const int M = 5;
    Array2D<N, M> mat;
	
	//std::array<std::array<int,512>,512> mat;

	fillMatrix(mat, 5);
	
	printf("Nombre  d’arguments  passes  au  programme  :  %d\n", argc);
	//std::cout << toStrMat(mat) << std::endl;
	printArray2D(mat)
}*/

using namespace std;

int main () {
   clock_t start_t, end_t;
   double total_t;
   int i;

   start_t = clock();
   printf("Starting of the program, start_t = %ld\n", start_t);
    
   printf("Going to scan a big loop, start_t = %ld\n", start_t);
   for(i=0; i< 2000000000; i++) {
   }
   end_t = clock();
   printf("End of the big loop, end_t = %ld\n", end_t);
   
   total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
   printf("Total time taken by CPU: %f\n", total_t  );
   printf("Exiting of the program...\n");

   return(0);
}















