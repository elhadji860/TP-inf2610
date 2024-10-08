/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Processes - part2.c
 *
 * Ajoutez vos noms, prénoms et matricules
*/

#include "./libprocesslab/libprocesslab.h"

#define m 1000000
// question2 calcule la somme des m premiers nombres naturels 1+2+...+m

// nb est le nombre de threads qui vont contribuer au calcul
#define nb 4

//tableau somme est utilisé pour le calcul des sommes patielles par les threads
long somme[nb];


// fonction exécutée par chaque thread créé
void* contribution(void*p)
{
    // TODO

  int no = *(int*) p;
  long int limInferieure = (no * m/nb) + 1;
  long int limSuperieure = (no + 1) * m / nb;

  long int sommeFinal;
  
  sommeFinal = 0; 

  for(int i = limInferieure; i <= limSuperieure; ++i){
    sommeFinal += i;
  }
  somme[no] = sommeFinal;

  //printf("%ld \n", somme[no]);

  pthread_exit(0);
}


void question2( )
{
  pthread_t tabTid[nb];
  int tabArgs[nb];
  for (int i = 0; i < nb; ++i){
      tabArgs[i] = i;
      pthread_create(&tabTid[i], NULL, contribution, (void *)&tabArgs[i]);

  }
  
  for (int i = 0; i < nb; ++i){
    pthread_join(tabTid[i], NULL);
  }

  long long sommeThread = 0;
  for (int i = 0; i < nb; ++i){
      sommeThread += (long long) somme[i];
  }

  long long somme= (long long)m * ((long long) (m+1))/2;

  printf("%lld \n%lld \n", somme, sommeThread);

}

