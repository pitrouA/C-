#include <iostream>
#include <mpi.h>
#include <string>
using namespace std;


string to_str_tab(int* tab, int k){
  string chaine = "[\n";
  for (int i=0; i<k; i++){
    chaine = chaine + "[";
    for (int j=0; j<k; j++){
      chaine = chaine + to_string(tab[i*k+j]) + " ";
    }
    chaine = chaine + "]\n";
  }

  return chaine + "]";
}


/*void reset_tab(int *tab, int* tab_local, int root, int k){
  for(int i=0;i<k;i++){
    (*tab)[root*k+i] = tab_local[i];
  }
}*/


int main(int argc, char**argv) {

    int nprocs;
    int pid;
    const int k = atoi(argv[1]);
    int root = atoi(argv[2]);
    //bool error = false;

    MPI_Init (&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);

    MPI_Win TheWin; // Déclaration de la fenêtre
    //MPI_Win TheWinError; // Déclaration de la fenêtre

    int tab[] = {
            5,3,4,6,8,8,9,1,2,
            6,7,2,1,9,5,3,4,8,
            1,9,8,3,4,2,5,6,7,
            8,5,9,7,6,1,4,2,3,
            4,2,6,8,5,3,7,9,1,
            7,1,3,9,2,4,8,5,6,
            9,6,1,5,3,7,2,8,4,
            2,8,7,4,1,9,6,3,5,
            3,4,5,2,8,6,1,7,9,
          };
    int* tab_local = new int[k];
    //int* tab_error = new int[k];
    //tab_error[0] = 0;//false
    //bool* error = new bool[1];
    //error[0] = false;
    bool error = false;

    if(pid == root){
      cout << "avant tout: \n" << to_str_tab(tab,k) << endl;
    }

    MPI_Scatter(tab, k, MPI_INT, tab_local, k, MPI_INT, root, MPI_COMM_WORLD);

    string chaine = "";
    for (int i=0; i<k; i++)
        chaine = chaine + to_string(tab_local[i]) + " ";

    cout << "je suis " << pid << " tab local: " << chaine << endl;

    MPI_Win_create(tab, nprocs*sizeof(int), sizeof(int),MPI_INFO_NULL, MPI_COMM_WORLD,&TheWin);
  //  MPI_Win_create(tab_error, nprocs*sizeof(int), sizeof(int),MPI_INFO_NULL, MPI_COMM_WORLD,&TheWinError);
    //MPI_Win_create(error, nprocs*sizeof(bool), sizeof(bool),MPI_INFO_NULL, MPI_COMM_WORLD,&TheWinError);


    /*MPI_Win_fence(0,TheWin);
    for (int i = 0; i < nprocs; i++)
        if (i != pid)
            MPI_Get(tab+i, nprocs, MPI_INT, i, i, 1, MPI_INT, TheWin);

    for (int i = 0; i < nprocs; i++)
        if (i != pid)
            MPI_Put(tab+pid, nprocs, MPI_INT, i, pid, 1, MPI_INT, TheWin);*/
    int sum = 0;
    for(int i=0;i<k;i++){
      sum += tab_local[i];
    }
    if(sum != 45){
      //erreur
      error = true;
      //error[0] = true;
      //MPI_Win_fence(0,TheWinError);
      //tab_error[0] = 1;
      //for (int i = 0; i < nprocs; i++){
        //if (i != pid){
          //MPI_Put(&error, 1, MPI_C_BOOL, i, true, 1, MPI_C_BOOL, TheWinError);
          //MPI_Put(error, 1, MPI_C_BOOL, i, true, 1, MPI_C_BOOL, TheWinError);

            //MPI_Put(tab_error+pid, nprocs, MPI_INT, i, pid, 1, MPI_INT, TheWinError);
            //MPI_Send( &error, 1, MPI_C_BOOL, root,1,MPI_COMM_WORLD );
        //}
      //}
      //MPI_Win_fence(0,TheWinError);
    }

    int* tab_sav;

    if(pid == root){
      tab_sav = new int[k];
      for(int i=0;i<k;i++){
        tab_sav[i] = tab_local[i];
      }
    }

    MPI_Win_fence(0,TheWin);
    if(pid != root){

      MPI_Accumulate(tab_local, k, MPI_INT, root, root*k, k, MPI_INT, MPI_SUM, TheWin);
      for(int i=0;i<k;i++){
        if(tab[i+root*k] != 45){
          //erreur
          //error[0] = true;
        }
      }
    }
    MPI_Win_fence(0,TheWin);

    /*MPI_Win_fence(0,TheWin);
    if(pid != root + 2){
      MPI_Accumulate(tab_local, k, MPI_INT, root+2, (root+2)*k, k, MPI_INT, MPI_SUM, TheWin);
    }
    MPI_Win_fence(0,TheWin);*/

    if(pid == root){
      //reset
      for(int i=0;i<k;i++){
        tab[root*k+i] = tab_sav[i];
      }
      delete[] tab_sav;
      cout << "après tout: \n" << to_str_tab(tab,k) << endl;
      cout << "erreur : " << error << endl;
    }
    /*if(pid == root){
      chaine = "";
      for (int i=0; i<n/nprocs; i++)
          chaine = chaine + to_string(tab[root*(n/nprocs)+i]-tab_local[i]) + " ";

      cout << "result: " << chaine << endl;
    }*/

    MPI_Win_free(&TheWin);
  //  MPI_Win_free(&TheWinError);
    MPI_Finalize();

    //delete[] tab;
    delete[] tab_local;

    return 0;
}

//--------
// make
// mpirun -np 9 ./main 9 1
//-----------
