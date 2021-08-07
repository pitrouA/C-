#include <iostream>
#include <mpi.h>
#include <string>
using namespace std;
int main(int argc, char**argv) {

    int nprocs;
    int pid;

    MPI_Init (&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);

    MPI_Win TheWin; // Déclaration de la fenêtre

    int* tab = new int[nprocs];
    for (int i=0; i<nprocs; i++)
        tab[i]=pid;

    string chaine = "";
    for (int i=0; i<nprocs; i++)
        chaine = chaine + to_string(tab[i]) + " ";

    cout << "je suis " << pid << " tab avant:" << chaine << endl;

    MPI_Win_create(tab, nprocs*sizeof(int), sizeof(int),MPI_INFO_NULL, MPI_COMM_WORLD,&TheWin);

    MPI_Win_fence(0,TheWin);
    /*for (int i = 0; i < nprocs; i++)
        if (i != pid)
            MPI_Get(tab+i, nprocs, MPI_INT, i, i, 1, MPI_INT, TheWin);*/

    for (int i = 0; i < nprocs; i++)
        if (i != pid)
            MPI_Put(tab+pid, nprocs, MPI_INT, i, pid, 1, MPI_INT, TheWin);
    MPI_Win_fence(0,TheWin);

    chaine = "";
    for (int i=0; i<nprocs; i++)
        chaine = chaine + to_string(tab[i]) + " ";

    cout << "je suis " << pid << " tab après: " << chaine << endl;

    MPI_Win_free(&TheWin);
    MPI_Finalize();

    return 0;
}

//--------
// make
// mpirun -np 4 ./main
//-----------
