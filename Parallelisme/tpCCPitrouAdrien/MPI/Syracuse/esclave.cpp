#include <mpi.h>
#include <iostream>
#include "syracuse.h"
using namespace std;

int main(int argc, char **argv) {
    int nslaves, pid, nmasters;
    MPI_Comm intercom;
      MPI_Comm intracom;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nslaves);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_get_parent(&intercom);
    MPI_Comm_remote_size(intercom, &nmasters);


    int n = atoi(argv[2]); // la taille de la suite passée en paramètre



      MPI_Intercomm_merge(intercom,0,&intracom);

      MPI_Comm_remote_size(intercom,&nmasters); // permet de connaître le nombre de parents
      // code de l'esclave

      int pid_intra;
      MPI_Comm_rank(intracom,&pid_intra);


      cout << "je suis l'esclave " << pid << " parmi " << nslaves << " esclaves et avec " << nmasters << " parents " << "dans l'intra communicateur je suis " << pid_intra << endl;

    // A compléter
    MPI_Win TheWin;
    int* tabTravail = new int[n+2];
    MPI_Win_create(tabTravail, n*sizeof(int), sizeof(int),MPI_INFO_NULL, intracom,&TheWin);

    MPI_Win_lock(MPI_LOCK_EXCLUSIVE,0,0,TheWin);
    MPI_Get(tabTravail, n, MPI_INT, 0, 0, 1, MPI_INT, TheWin); //ne récupère rien ????
    // -----------------------
    // je ne peux pas continuer car je n'arrive pas à récupérer ces $@%? de valeurs sur l'esclave
    // ( alors qu' en faisant exactement pareil j'avail réussi dans un précédent tp )
    // -------------------------------------
    cout << "reçu : " << tabTravail[0] << endl;
    //int test = test_syracuse(??, ??) implémentée dans syracuse.cpp
    //MPI_Put(&a, 1, MPI_INT, 0, 0, 1, MPI_INT, TheWin); //
    MPI_Win_unlock(0,TheWin);

    MPI_Comm_free(&intercom);
    MPI_Comm_free(&intracom);
  delete[] tabTravail;
    MPI_Finalize();
    return 0;
}
