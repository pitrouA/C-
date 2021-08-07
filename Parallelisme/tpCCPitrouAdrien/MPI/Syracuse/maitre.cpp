#include <mpi.h>
#include <iostream>
#include "syracuse.h"
using namespace std;

int main(int argc, char **argv) {
    int nmasters, pid;
    MPI_Comm intercom;
      MPI_Comm intracom;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nmasters);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    int n = atoi(argv[1]); // la taille de la suite à vérifier
    int nslaves = atoi(argv[2]); // le nombre d'esclaves à lancer
    int mode = atoi(argv[3]); // le mode de la génération
    if (nmasters != 1) {
        cout << "Attention un seul master !";
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    /* Génération de la suite sur le maître */
    int *tab;
    tab = new int[n];

    switch (mode) {
        case 0:
            cout << "test génération aléatoire " << endl;
            non_syracuse(n, tab);
            break;
        case 1:
            cout << "test syracuse partielle" << endl;
            demi_syracuse(n, tab);
            break;
        default:
            cout << "test syracuse" << endl;
            syracuse(n, tab);
    }


    cout << "tab : ";
    for (int i=0; i<n; i++)
        cout << tab[i] << " ";
    cout << endl;

    MPI_Comm_spawn("esclave", argv, nslaves, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercom, MPI_ERRCODES_IGNORE);


    MPI_Intercomm_merge(intercom,0,&intracom);
      int pid_intra;
    MPI_Comm_rank(intracom,&pid_intra);
    cout << "je suis le maître " << pid << " parmi " << nmasters << " maîtres et dans l'intra communicateur je suis " << pid_intra << endl;


    MPI_Win TheWin; // Déclaration de la fenêtre
    MPI_Win_create(tab, n*sizeof(int), sizeof(int),MPI_INFO_NULL, intracom,&TheWin);

    //envoi de l'information aux esclaves
    //mais en fait, ça pose des problemes de synchronisation -> se rabattre sur le get qui normalement fonctionne mieux
    /*for (int i = 0; i < nslaves; i++){
        if (i != pid){
            MPI_Win_lock(MPI_LOCK_EXCLUSIVE,i,0,TheWin);
            MPI_Put(tab, n, MPI_INT, i, pid, 1, MPI_INT, TheWin);
            MPI_Win_unlock(i,TheWin);
        }
    }*/

    MPI_Win_lock(MPI_LOCK_EXCLUSIVE,0,0,TheWin);
    MPI_Win_unlock(0,TheWin);

    free(tab);
    MPI_Comm_free(&intercom);
    MPI_Comm_free(&intracom);
    MPI_Finalize();
    return 0;
}

// make
// mpirun -np 1 ./maitre 16 4 0
