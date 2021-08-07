#include <mpi.h>
#include <iostream>
#include <string>
using namespace std;

string toStringTab(int* tab, int size){
  string retour = "";

  for(int i=0;i<size;i++){
    if(tab[i] != -1){
      retour = retour + to_string(tab[i]) + ",";
    }
  }

  return retour;
}


int main(int argc, char**argv){
  int nmasters, pid;
  MPI_Comm intercom;
  MPI_Comm intracom;
  MPI_Init (&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&nmasters);
  MPI_Comm_rank(MPI_COMM_WORLD,&pid);

  // le code des maîtres

  int n = atoi(argv[1]);
  int nslaves = atoi(argv[2]);
  int root = atoi(argv[3]);

  cout << "je suis le maître " << pid << " parmi " << nmasters << " maîtres" << endl;
  int* tab = new int[n];

  srand(time(NULL));
  for (int i=0; i<n; i++)
      tab[i] = pid+i;

  MPI_Comm_spawn("esclave", // exécutable
                     argv, // arguments à passer sur la ligne de commande
                     nslaves, // nombre de processus esclave
                     MPI_INFO_NULL, //permet de préciser où et comment lancer les processus
                     root, // rang du processus maître qui effectue réellement le spawn
                     MPI_COMM_WORLD, // Monde dans lequel est effectué le spwan
                     &intercom, // l'intercommunicateur permettant de communiquer avec les esclaves
                     MPI_ERRCODES_IGNORE // tableau contenant les erreurs
      );

  MPI_Intercomm_merge(intercom,0,&intracom);
    int pid_intra;
  MPI_Comm_rank(intracom,&pid_intra);
  cout << "je suis le maître " << pid << " parmi " << nmasters << " maîtres et dans l'intra communicateur je suis " << pid_intra << endl;


//-------------------------------------------------------------------------------

  MPI_Win TheWin;
  MPI_Win TheWin2;
    int a = 1;
    bool b = true;
  bool* b2 = new bool[2];
  b2[0] = true;
  b2[1] = true;

  MPI_Win_create(&a, 1*sizeof(int), sizeof(int),MPI_INFO_NULL, intracom,&TheWin);
  MPI_Win_create(b2, 2*sizeof(bool), sizeof(bool),MPI_INFO_NULL, intracom,&TheWin2);

  /*MPI_Win_lock(MPI_LOCK_EXCLUSIVE,1,0,TheWin);
  MPI_Get(&a, 1, MPI_INT, 1, 0, 1, MPI_INT, TheWin);
  MPI_Win_unlock(1,TheWin);*/

  /*MPI_Win_lock(MPI_LOCK_EXCLUSIVE,1,0,TheWin2);
  MPI_Get(b2, 2, MPI_C_BOOL, 1, 0, 1, MPI_C_BOOL, TheWin2);
  MPI_Win_unlock(1,TheWin2);*/

  /*MPI_Win_lock(MPI_LOCK_EXCLUSIVE,3,0,TheWin2);
  MPI_Get(b2+1, 2, MPI_C_BOOL, 3, 0, 1, MPI_C_BOOL, TheWin2);
  MPI_Win_unlock(3,TheWin2);*/
  /*while(a == 1){
    cout << "je suis maitre et a=" << a << " et b=" << b2[0] << " " << b2[1] << endl;
  }*/

  cout << "je suis maitre et a=" << a << " et b=" << b2[0] << " " << b2[1] << endl;

    MPI_Win_free(&TheWin);
    MPI_Win_free(&TheWin2);

MPI_Comm_free(&intracom);
  MPI_Comm_free(&intercom);
  MPI_Finalize();
  return 0;
}

// mpirun -np 1 ./maitre 16 4 0
