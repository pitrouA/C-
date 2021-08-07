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
#include <math.h>

using namespace std;
using namespace std::chrono;

// variables globales
// taille des données (soit le vecteur ou le coté d'une matrice carrée)
const int taille=2000;
// taille des données en octets Attention au type des données)
size_t nboctets=sizeof(float)*(taille*taille);
// pointeurs vers le stockage des données en mémoire centrale
float *A;
float *B;
float *C;


// fonction permettant de récupérer le temps écoulé entre debut et fin
double temps(time_point<system_clock> debut, time_point<system_clock> fin){
  duration<double> tps=fin-debut;
  return tps.count();
}

// initialisation d'un vecteur à une valeur aléatoire entre min et max
void init_vec(int *vec,int taille, int min, int max){
  if (min==max)
    for (int i=0;i<taille;vec[i++]=min);
  else{
    int interval=max-min+1;
    for (int i=0;i<taille;vec[i++]=min+rand()%interval);
  }
}

// initialisation d'un vecteur à une valeur aléatoire entre min et max
void init_vec(float *vec,int taille, float min, float max){
  if (min==max)
    for (int i=0;i<taille;vec[i++]=min);
  else{
    int interval=max-min+1;
    for (int i=0;i<taille;i++){
      float val = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
      vec[i]=floor(val);
    }
  }
}

void affiche_vec(int * vec, int taille, int nb_col=-1){
  if (nb_col==-1) nb_col=taille;
  cout << "[";
  for (int i =0;i<taille; i++){
    if (i%nb_col==0) cout<<endl;
    cout<<vec[i]<<",";
  }
  cout<<"]"<<endl;
}

void affiche_vec(float * vec, int taille, int nb_col=-1){
  if (nb_col==-1) nb_col=taille;
  cout << "[";
  for (int i =0;i<taille; i++){
    if (i%nb_col==0) cout<<endl;
    cout<<fixed<<vec[i]<<",";
  }
  cout<<"]"<<endl;
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


void test_CPU(){
  time_point<system_clock> debut=system_clock::now();
  //#pragma omp parallel for
  for (int i=0;i<taille;i++){
    for (int j=0;j<taille;j++){
      float acc=0;
      for(int k=0;k<taille;k++){
       acc += A[i*taille+j]*B[k*taille+j];
      }
      C[i*taille+j]=acc;
    }
  }
  time_point<system_clock> fin=system_clock::now();
  cout<<"temps execution "<<temps(debut,fin)<<endl;
  cout<<"Résultat CPU"<<endl;
  //affiche_vec(C,taille);
}
void test_GPU(cl::Program programme, cl::CommandQueue queue, cl::Context contexte){
  time_point<system_clock> debut=system_clock::now();
  // Création des buffers de données dans le contexte
  cl::Buffer bufferA = cl::Buffer(contexte, CL_MEM_READ_ONLY, nboctets);//taille * taille * sizeof(float)
  cl::Buffer bufferB = cl::Buffer(contexte, CL_MEM_READ_ONLY, nboctets);
  cl::Buffer bufferC = cl::Buffer(contexte, CL_MEM_WRITE_ONLY, nboctets);

  // Chargement des données en mémoire video
  queue.enqueueWriteBuffer(bufferA , CL_TRUE, 0, nboctets , A);
  queue.enqueueWriteBuffer(bufferB , CL_TRUE, 0, nboctets , B);
  // creation du kernel (fonction à exécuter)
  cl::Kernel kernel(programme,"multmat");
  // Attribution des paramètres de ce kernel
  kernel.setArg(0,taille);
  kernel.setArg(1,bufferA);
  kernel.setArg(2,bufferB);
  kernel.setArg(3,bufferC);

  // création de la topologie des processeurs
  cl::NDRange global(taille,taille); // nombre total d'éléments de calcul -processing elements
  cl::NDRange local(1,16); // dimension des unités de calcul -compute units- c'à-dire le nombre d'éléments de calcul par unités de calcul

  // lancement du programme en GPU
  queue.enqueueNDRangeKernel(kernel,cl::NullRange,global,local);

  // recupération du résultat
  queue.enqueueReadBuffer(bufferC,CL_TRUE,0,nboctets,C);
  time_point<system_clock> fin=system_clock::now();

  cout<<"temps execution "<<temps(debut,fin)<<endl;
  cout<<"Résultat GPU"<<endl;
  //affiche_vec(C,taille*taille);
}
void test_GPU_2(cl::Program programme, cl::CommandQueue queue, cl::Context contexte){
  time_point<system_clock> debut=system_clock::now();
  // Création des buffers de données dans le contexte
  cl::Buffer bufferA = cl::Buffer(contexte, CL_MEM_READ_ONLY, nboctets);//taille * taille * sizeof(float)
  cl::Buffer bufferB = cl::Buffer(contexte, CL_MEM_READ_ONLY, nboctets);
  cl::Buffer bufferC = cl::Buffer(contexte, CL_MEM_WRITE_ONLY, nboctets);

  // Chargement des données en mémoire video
  queue.enqueueWriteBuffer(bufferA , CL_TRUE, 0, nboctets , A);
  queue.enqueueWriteBuffer(bufferB , CL_TRUE, 0, nboctets , B);
  // creation du kernel (fonction à exécuter)
  cl::Kernel kernel(programme,"multmat");
  // Attribution des paramètres de ce kernel
  kernel.setArg(0,taille);
  kernel.setArg(1,bufferA);
  kernel.setArg(2,bufferB);
  kernel.setArg(3,bufferC);

  // création de la topologie des processeurs
  cl::NDRange global(taille); // nombre total d'éléments de calcul -processing elements
  cl::NDRange local(16); // dimension des unités de calcul -compute units- c'à-dire le nombre d'éléments de calcul par unités de calcul

  // lancement du programme en GPU
  queue.enqueueNDRangeKernel(kernel,cl::NullRange,global,local);

  // recupération du résultat
  queue.enqueueReadBuffer(bufferC,CL_TRUE,0,nboctets,C);
  time_point<system_clock> fin=system_clock::now();

  cout<<"temps execution "<<temps(debut,fin)<<endl;
  cout<<"Résultat GPU"<<endl;
  //affiche_vec(C,taille*taille);
}


int main(){
  // pour mesurer le temps
  time_point<system_clock> debut,debut2,fin;
  // initialisation de générateur aléatoire
  srand (time(NULL));


  // création des zone de stockage de données en mémoire centrale
  A= new float[taille*taille];
  B= new float[taille*taille];
  C=new float[taille*taille];



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
    init_vec(A,taille*taille,1,1);
    init_vec(B,taille*taille,1,1);
    // affichage des données initialisées
    cout<<" Données initialisées"<<endl;
    //affiche_vec(A,taille*taille);
    //affiche_vec(B,taille*taille);

    //test_CPU();
    test_GPU(programme,queue,contexte);
    test_GPU_2(programme,queue,contexte);

  } catch (cl::Error err) { // Affichage des erreurs en cas de pb OpenCL
    cout << "Exception\n";
    cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << endl;
    return EXIT_FAILURE;
  }
}

//g++ programme_hote.cpp -O3 -lOpenCL -o programme_hote
