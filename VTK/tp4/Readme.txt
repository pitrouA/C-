Pour utiliser mpi:

sudo apt-get  install  libopenmpi-dev  openmpi-bin


il faut le fichier /share/etud/sn_resamp512 à cette position.


Puis une fois toutes le manipulations cmake faites vous pouvez lancer le code avec cette commande:

make prun1   

Elle lance un  processus mpi. Elle génère une seule image. 

"make prun2" lance 2 processus

"make prun4" lance 4 processus 

Autre lancement possible du programme:

mpirun -np 4 ./vtktp

