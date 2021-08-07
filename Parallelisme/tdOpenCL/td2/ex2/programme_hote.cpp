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
// taille des données (soit le vecteur ou le coté d'une matrice carrée)
const int taille=2048*4096;
// taille des données en octets Attention au type des données)
size_t nboctets=sizeof(float)*taille;

const float M[] = {1.0,2.3,0.3,2.3,0.8,2.5,0.4,0.3,0.1};

// fonction permettant de récupérer le temps écoulé entre debut et fin
double temps(time_point<system_clock> debut, time_point<system_clock> fin){
  duration<double> tps=fin-debut;
  return tps.count();
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

void affiche_vec(unsigned int * vec, int taille){
  cout<<"[";
  for (int i =0;i<taille; i++){
    cout<<vec[i];
    if (i < taille - 1){
      cout<<", ";
    }
  }
  cout<<"]"<<endl;
}


void test_CPU(){
  cout<<"Résultat CPU"<<endl;
  time_point<system_clock> debut=system_clock::now();

  unsigned int F[3];

  //code
  for(int i=0;i<3;i++){
    for(int j=0;j<9;j++){
      if(M[j] >= i && M[j] < i+1){
        F[i] ++;
      }
    }
  }

  cout << "Nbre occ : ";
  affiche_vec(F, 9);

  time_point<system_clock> fin=system_clock::now();
  cout<<"temps execution "<<temps(debut,fin)<<endl;
  cout<<"Résultat CPU"<<endl;
  //affiche_vec(C,taille);
}


void test_GPU(cl::Program programme, cl::CommandQueue queue, cl::Context contexte){
  cout<<"Résultat GPU"<<endl;
  time_point<system_clock> debut=system_clock::now();
  // Création des buffers de données dans le contexte
  cl::Buffer bufferTotal = cl::Buffer(contexte, CL_MEM_READ_WRITE, sizeof(int));
  cl::Buffer bufferIn = cl::Buffer(contexte, CL_MEM_READ_WRITE, sizeof(int));

  long int total = 0;
  long int in = 0;

  // Chargement des données en mémoire video
  queue.enqueueWriteBuffer(bufferTotal , CL_TRUE, 0, sizeof(int) , &total);
  queue.enqueueWriteBuffer(bufferIn , CL_TRUE, 0, sizeof(int) , &in);
  // creation du kernel (fonction à exécuter)
  cl::Kernel kernel(programme,"frequence");
  // Attribution des paramètres de ce kernel
  kernel.setArg(0,6);
  kernel.setArg(1,bufferTotal);
  kernel.setArg(2,bufferIn);

  // création de la topologie des processeurs
  cl::NDRange global(taille, taille); // nombre total d'éléments de calcul -processing elements
  cl::NDRange local(4,4); // dimension des unités de calcul -compute units- c'à-dire le nombre d'éléments de calcul par unités de calcul

  // lancement du programme en GPU
  queue.enqueueNDRangeKernel(kernel,cl::NullRange,global,local);

  // recupération du résultat
  queue.enqueueReadBuffer(bufferTotal,CL_TRUE,0,nboctets,&total);
  time_point<system_clock> fin=system_clock::now();

  cout<<"temps execution "<<temps(debut,fin)<<endl;
}


int main(){
  // pour mesurer le temps
  time_point<system_clock> debut,debut2,fin;
  // initialisation de générateur aléatoire
  srand (time(NULL));


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

    // affichage des données initialisées
    cout<<" Données initialisées"<<endl;
    //affiche_vec(A,taille);
    //affiche_vec(B,taille);

    test_CPU();
    //test_GPU(programme,queue,contexte);

  } catch (cl::Error err) { // Affichage des erreurs en cas de pb OpenCL
    cout << "Exception\n";
    cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << endl;
    return EXIT_FAILURE;
  }
}

//g++ programme_hote.cpp -O3 -lOpenCL -o programme_hote
