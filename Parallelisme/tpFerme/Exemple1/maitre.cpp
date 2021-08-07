#include <mpi.h>
#include <iostream>
using namespace std;

int* genererSuite(int numProc, int tailleTab){
  int* ret = new int[numProc];

  int ratioMin = tailleTab / numProc;

  for(int i=0;i<ratioMin;i++){
    ret[i] = ratioMin;
  }
  for(int i=0;i<tailleTab - (ratioMin*numProc);i++){
    ret[i] ++;
  }

  return ret;
}


int main(int argc, char**argv){
  int nmasters, pid;
  MPI_Comm intercom;
  MPI_Init (&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&nmasters);
  MPI_Comm_rank(MPI_COMM_WORLD,&pid);

  // le code des maîtres

  int n = atoi(argv[1]);
  int nslaves = atoi(argv[2]);
  int root = atoi(argv[3]);

  cout << "je suis le maître " << pid << " parmi " << nmasters << " maîtres" << endl;
  int* tab = new int[n];
  int* tabMax = new int[nslaves];

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


  if (pid==root) {
      int* suite = genererSuite(nslaves, n);
      int cumul = 0;
      for (int i = 0; i < nslaves; i++){
        //cout << "Suite de " << i << " = " << suite[i] << endl;
          MPI_Ssend(tab+cumul, suite[i], MPI_INT, i, 10, intercom);
          cumul += suite[i];
      }
      for (int i = 0; i < nslaves; i++){
        //cout << "Suite de " << i << " = " << suite[i] << endl;
          //MPI_Recv(tab, suite[i], MPI_INT, i, 10, intercom);
          MPI_Status status;
          MPI_Recv(&tabMax[i],1,MPI_INT,i,11,intercom,&status);
      }
      delete[] suite;

      int max = tabMax[0];
      for(int i=1;i<nslaves;i++){
        //cout << tab[i] << " ";
        if(max < tabMax[i]){
          max = tabMax[i];
        }
      }
      cout << "Max = " << max << endl;
  }

  delete[] tabMax;
  delete[] tab;

  MPI_Comm_free(&intercom);
  MPI_Finalize();
  return 0;
}

//// mpirun -np 1 ./maitre 16 4 0
