#define __CL_ENABLE_EXCEPTIONS

#include "CL/cl.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
// pour la génération aléatoire
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace std::chrono;

// variables globales
// pointeurs vers le stockage des données en mémoire centrale
//float *A;
//float *B;
//float *C;


// fonction permettant de récupérer le temps écoulé entre debut et fin
double temps(time_point<system_clock> debut, time_point<system_clock> fin){
  duration<double> tps=fin-debut;
  return tps.count();
}

template<int n>
void affiche_mat(int mat[n][n]){
  cout << "[ " << endl;
  for (int i =0;i<n; i++){
    cout << " {";
    for (int j =0;j<n; j++){
      cout<<mat[i][j];
      if(j<n-1){
        cout << ",";
      }
    }
    cout << "}" << endl;
  }
  cout<< "]" << endl;
}

void affiche_device(cl::Device device){
  cout << "\t\tDevice Name: " << device.getInfo<CL_DEVICE_NAME>() << endl;
  cout << "\t\tDevice Type: " << device.getInfo<CL_DEVICE_TYPE>();
  cout << " (GPU: " << CL_DEVICE_TYPE_GPU << ", CPU: " << CL_DEVICE_TYPE_CPU << ")" << endl;
  cout << "\t\tDevice Vendor: " << device.getInfo<CL_DEVICE_VENDOR>() << endl;
  cout << "\t\tDevice Max Compute Units: " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << endl;
  cout << "\t\tDevice Global Memory: " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << endl;
  cout << "\t\tDevice Max Clock Frequency: " << device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << endl;
  cout << "\t\tDevice Max Allocateable Memory: " << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << endl;
  cout << "\t\tDevice Local Memory: " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << endl;
  cout << "\t\tDevice Available: " << device.getInfo<CL_DEVICE_AVAILABLE>() << endl;
  cout << "\t\tMax Work-group Total Size: " << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << endl;
  vector<size_t> d= device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
  cout << "\t\tMax Work-group Dims: (";
  for (vector<size_t>::iterator st = d.begin(); st != d.end(); st++)
    cout << *st << " ";
  cout << "\x08)" << endl;
}


cl::Program creationProgramme(string nomFicSource, cl::Context contexte){
  // lecture du programme source
    ifstream sourceFile(nomFicSource);
    string sourceCode(istreambuf_iterator <char>(sourceFile),(istreambuf_iterator < char >()));
    // la premier argument indique le nombre de programmes sources utilisés, le deuxième est une paire (texte, taille du programme)
    cl::Program::Sources source(1,make_pair(sourceCode.c_str(),sourceCode.length()+1));
    // creation du programme dans le contexte
    return cl::Program(contexte,source);
}

//--------------------------------------------







//--------------------------------------------

template<int n>
int* getLine(int i,int mat1[n][n]){
  int* mat = new int[n];

  for(int j=0;j<n;j++){
    mat[j] = mat1[i][j];
  }

  return mat;
}

template<int n>
int* getColumn(int i,int mat1[n][n]){
  int* mat = new int[n];

  for(int j=0;j<n;j++){
    mat[j] = mat1[j][i];
  }

  return mat;
}

template<int n,size_t nboctets>
void test_1(cl::Program programme, cl::CommandQueue queue, cl::Context contexte, int mat1[n][n],int mat2[n][n],int mat3[n][n]){
  time_point<system_clock> debut=system_clock::now();
  // Création des buffers de données dans le contexte
  cl::Buffer bufferA = cl::Buffer(contexte, CL_MEM_READ_ONLY, nboctets);
  cl::Buffer bufferB = cl::Buffer(contexte, CL_MEM_READ_ONLY, nboctets);
  cl::Buffer bufferC = cl::Buffer(contexte, CL_MEM_WRITE_ONLY, nboctets);

  // Chargement des données en mémoire video
  for(int i=0;i<n;i++){
    queue.enqueueWriteBuffer(bufferA , CL_TRUE, 0, nboctets , getLine<n>(i,mat1));
    queue.enqueueWriteBuffer(bufferB , CL_TRUE, 0, nboctets , getColumn<n>(i,mat2));
  }
  // creation du kernel (fonction à exécuter)
  cl::Kernel kernel(programme,"nom_du_kernel");
  // Attribution des paramètres de ce kernel
  kernel.setArg(0,n);
  kernel.setArg(1,bufferA);
  kernel.setArg(2,bufferB);
  kernel.setArg(3,bufferC);

  // création de la topologie des processeurs
  cl::NDRange global(n); // nombre total d'éléments de calcul -processing elements
  cl::NDRange local(16); // dimension des unités de calcul -compute units- c'à-dire le nombre d'éléments de calcul par unités de calcul

  // lancement du programme en GPU
  queue.enqueueNDRangeKernel(kernel,cl::NullRange,global,local);

  // recupération du résultat
  queue.enqueueReadBuffer(bufferC,CL_TRUE,0,nboctets,mat3);
  time_point<system_clock> fin=system_clock::now();

  cout<<"temps execution "<<temps(debut,fin)<<endl;
  cout<<"Résultat GPU"<<endl;
}

template<int n,size_t nboctets>
void test_2(cl::Program programme, cl::CommandQueue queue, cl::Context contexte,int mat1[n][n],int mat2[n][n],int mat3[n][n]){

}


int main(){


  const int n = 3;
  // taille des données en octets Attention au type des données)
  const size_t nb_octets=sizeof(float)*n;


  // pour mesurer le temps
  time_point<system_clock> debut,debut2,fin;
  // initialisation de générateur aléatoire
  srand (time(NULL));


  // création des zone de stockage de données en mémoire centrale
  //A= new float[taille];
  //B= new float[taille];
  //C=new float[taille];

  int a[n][n] = { {0,0,0}, {1,2,0}, {2,4,0} };
  int b[n][n] = { {0,0,0}, {1,2,0}, {2,4,0} };
  int c[n][n];
  affiche_mat<n>(a);

  try{ // debut de la zone d'utilisation de l'API pour OpenCL
    // les plateformes
    vector <cl::Platform> plateformes;
    cl::Platform::get(&plateformes); // recherche des plateformes normalement 1 sur un PC

    //les devices
    vector <cl::Device> devices;
    plateformes[0].getDevices(CL_DEVICE_TYPE_ALL,&devices); // recherche des devices (normalement 1)

    // affichage des informations des devices
    for (int i=0;i<devices.size();i++){
      cout << "\tDevice " << i << ": " << endl;
      affiche_device(devices[i]);
    }

    // création d'un contexte pour les devices
    cl::Context contexte(devices);

    // création du programme dans le contexte (voir code fonction)
    cl::Program programme=creationProgramme("exemple.cl",contexte);
    // compilation du programme
   try{
      programme.build(devices);
    } catch (...) {
      // Récupération des messages d'erreur au cas où...
      cl_int buildErr = CL_SUCCESS;
      auto buildInfo = programme.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0],&buildErr);
      	cerr << buildInfo << endl << endl;
	exit(0);
    }

    // création de la file de commandes (ordres de l'hote pour le GPU)
    cl::CommandQueue queue= cl::CommandQueue(contexte,devices[0]);

    // initialisation des données sur l'hote
    //init_vec(A,taille,-10,10);
    //init_vec(B,taille,-10,10);
    // affichage des données initialisées
    cout<<" Données initialisées"<<endl;
    //affiche_vec(A,taille);
    //affiche_vec(B,taille);

    test_1<n,nb_octets>(programme,queue,contexte,a,b,c);
    test_2<n,nb_octets>(programme,queue,contexte,a,b,c);

    affiche_mat<n>(a);

  } catch (cl::Error err) { // Affichage des erreurs en cas de pb OpenCL
    cout << "Exception\n";
    cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << endl;
    return EXIT_FAILURE;
  }
}

//g++ programme_hote.cpp -O3 -lOpenCL -o programme_hote
