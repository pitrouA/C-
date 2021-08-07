#include <mpi.h>
#include <iostream>
using namespace std;

int main(int argc, char**argv){
  int nslaves, pid, nmasters,flag;
  MPI_Comm intercom;
  MPI_Comm intracom;

  MPI_Init (&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&nslaves);
  MPI_Comm_rank(MPI_COMM_WORLD,&pid);
  MPI_Comm_get_parent(&intercom); // obtention de l'intercommunicateur vers le/les parents

  MPI_Intercomm_merge(intercom,0,&intracom);

  MPI_Comm_remote_size(intercom,&nmasters); // permet de connaître le nombre de parents
  // code de l'esclave

  int pid_intra;
  MPI_Comm_rank(intracom,&pid_intra);


  cout << "je suis l'esclave " << pid << " parmi " << nslaves << " esclaves et avec " << nmasters << " parents " << "dans l'intra communicateur je suis " << pid_intra << endl;

  // Attention pour les esclaves on a esclave ./maitre 16 4 0 donc n est l'argument n°2
  int n = atoi(argv[2]);
  int root = atoi(argv[4]);

  MPI_Win TheWin;
  int a = 15;
  MPI_Win_create(&a, 1*sizeof(int), sizeof(int),MPI_INFO_NULL, intracom,&TheWin);

 MPI_Win_free(&TheWin);
  MPI_Finalize();
  return 0;
}
