#include <iostream>
#include <mpi.h>
#include <string>
using namespace std;
int main(int argc, char**argv) {

    int nprocs;
    int pid;
    int n = atoi(argv[1]);
    int root = atoi(argv[2]);

    MPI_Init (&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);

    MPI_Win TheWin; // Déclaration de la fenêtre

    srand(time(NULL));

    int n_local = n/nprocs;

    int* tab = new int[n];
    for (int i=0; i<n; i++){
      if(pid != root){
        tab[i] = 0;
      }else{
        tab[i]=int(rand()%n);
      }
    }
    int* tab_local = new int[n_local];

    string chaine = "";

    if(pid == root){
      chaine = "";
      for (int i=0; i<n; i++)
          chaine = chaine + to_string(tab[i]) + " ";

      cout << "avant tout: " << chaine << endl;
    }

    MPI_Scatter(tab, n_local, MPI_INT, tab_local, n_local, MPI_INT, root, MPI_COMM_WORLD);

    chaine = "";
    for (int i=0; i<n_local; i++)
        chaine = chaine + to_string(tab_local[i]) + " ";

    cout << "je suis " << pid << " tab local: " << chaine << endl;

    MPI_Win_create(tab, nprocs*sizeof(int), sizeof(int),MPI_INFO_NULL, MPI_COMM_WORLD,&TheWin);

    /*MPI_Win_fence(0,TheWin);
    for (int i = 0; i < nprocs; i++)
        if (i != pid)
            MPI_Get(tab+i, nprocs, MPI_INT, i, i, 1, MPI_INT, TheWin);

    for (int i = 0; i < nprocs; i++)
        if (i != pid)
            MPI_Put(tab+pid, nprocs, MPI_INT, i, pid, 1, MPI_INT, TheWin);*/

    MPI_Win_fence(0,TheWin);
    MPI_Accumulate(tab_local, n_local, MPI_INT, root, root*(n/nprocs), n_local, MPI_INT, MPI_SUM, TheWin);
    MPI_Win_fence(0,TheWin);


    if(pid == root){
      chaine = "";
      for (int i=0; i<n/nprocs; i++)
          chaine = chaine + to_string(tab[root*(n/nprocs)+i]-tab_local[i]) + " ";

      cout << "result: " << chaine << endl;
    }

    MPI_Win_free(&TheWin);
    MPI_Finalize();

    delete[] tab;
    delete[] tab_local;

    return 0;
}

//--------
// make
// mpirun -np 4 ./main 16 1
//-----------
