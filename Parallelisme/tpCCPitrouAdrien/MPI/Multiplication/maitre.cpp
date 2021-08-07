#include <mpi.h>
#include <iostream>
#include "fonctions.h"
using namespace std;

int main(int argc, char **argv) {
    int nmasters, pid;
    MPI_Comm intercom;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nmasters);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    // le code des maîtres
    if (nmasters != 1) {
        cout << "Attention un seul master !";
        MPI_Abort(MPI_COMM_WORLD, 0);
    }
    int n = atoi(argv[1]); // taille de la matrice carrée
    int m = atoi(argv[2]); // nombre de vecteurs en entrée
    int nslaves = atoi(argv[3]); // nombre d'esclaves à lancer

    /* Génération de la matrice */
    int *matrice = new int[n * n];

    srand(time(NULL));
    for (int i = 0; i < n * n; i++)
        matrice[i] = rand() % 20;

    /* cout << "je suis maître " << pid << " matrice = ";
      for (int i=0; i<n; i++) {
          for (int j = 0; j < n; j++)
              cout << matrice[i * n + j] << " ";
          cout << endl;
      }
      cout << endl;
 */
    /* Génération des vecteurs partiellement nuls*/
    int *vecteurs = new int[n * m];
    for (int i = 0; i < m; i++) {
        int nb_zero = rand() % n;
        generation_vecteur(n, vecteurs + i * n, nb_zero);
    }

    /*   cout << "je suis maître " << pid << " vecteurs = ";
        for (int i=0; i<m; i++) {
            for (int j = 0; j < n; j++)
                cout << vecteurs[i * n + j] << " ";
            cout << endl;
        }
        cout << endl;
   */
    MPI_Comm_spawn("esclave", argv, nslaves, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercom, MPI_ERRCODES_IGNORE);

    int *resultats = new int[n * m];


/*    cout << "je suis maître " << pid << " résultats = ";
     for (int i=0; i<m; i++) {
         for (int j = 0; j < n; j++)
             cout << resultats[i * n + j] << " ";
         cout << endl;
     }
     cout << endl;
*/
    free(matrice);
    free(vecteurs);
    free(resultats);
    MPI_Comm_free(&intercom);
    MPI_Finalize();
    return 0;
}
