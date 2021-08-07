__kernel
void frequence(const int taille, __global float *M, __global unsigned int *F, __local int *loc_freq){
   int x=get_global_id(0);
   int id_loc = get_local_id(0);
   int pos = get_local_size(0);

   //init freq locale
   for(int i=id_loc;i<valMax;i+=pos){
     loc_freq[i] = 0;
   }

   //barriere
   barrier(CLK_LOCAL_MEM_FENCE);

   int val = floor(M[x]);
   atomic_add(&loc_frec[val],1);

   //barriere
   barrier(CLK_LOCAL_MEM_FENCE);
   //maj freq globales
   for(int i=id_loc;i<valMax;i+=pos){
     atomic_add(F, loc_freq[i]);
   }
}
