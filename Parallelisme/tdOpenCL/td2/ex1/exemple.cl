__kernel
void calculpi(const float step, __global unsigned int * total, __global unsigned int * total_in){
   int x=get_global_id(0);
   int y=get_global_id(1);

   //nbre procs dans workgroup
   int taille_h = get_global_size(0);
   int taille_v = get_global_size(1);

   // pourcent de la longueur
   float cote_h = 2.0 / taille_h;
   float cote_v = 2.0 / taille_v;

   float debut_x = x*cote_h-1.0;
   float debut_y = y*cote_v-1.0;

   float fin_x = debut_x+cote_h;
   float fin_y = debut_y+cote_v;

   unsigned nb_total = 0;
   unsigned nb_dedans = 0;

   for(float i=debut_x;i<fin_x;i+=step){

     for(float j=debut_y;j<fin_y;j+=step){
        if(i*i+j*j<=1){
          //atomic_add(total,1);
          nb_total +=1;
          printf("[%f %f]",step,cote_h);

          if(i*i+j*j<=1){
            nb_dedans+=1;
          }
        }
     }
   }

   atomic_add(total,nb_total);
   atomic_add(total_in,nb_dedans);
}
