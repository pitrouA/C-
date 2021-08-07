#include <mpi.h>
#include <iostream>
#include "fonctions.h"
using namespace std;

int main(int argc, char **argv) {
    int nslaves, pid, nmasters, flag;
    MPI_Comm intercom;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nslaves);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_get_parent(&intercom);
    MPI_Comm_remote_size(intercom, &nmasters);

    int n = atoi(argv[2]); // l'esclave connaît la taille de la matrice nxn et des vecteurs
    int m = atoi(argv[3]); // l'esclave connaît le nombre de vecteurs en entrée

    int *matrice = new int[n * n];
    int *v1 = new int[n];
    int *v2 = new int[n];

    free(matrice);
    free(v1);
    free(v2);
    MPI_Comm_free(&intercom);
    MPI_Finalize();
    return 0;
}
