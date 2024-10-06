/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Processes - part2.c
 *
 * Ajoutez vos noms, prénoms et matricules
 * Pascale Lafond 2204155, 
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
    long resultat;
    for (long i=((long)p * m/nb)+1; i<(((long)p+1)*m/nb)+1; i++) {
      resultat += i;
    }
    somme[(long)p] = resultat;
  return NULL;
}


void question2()
{
    // TODO
    pthread_t *tid = malloc(nb * sizeof(pthread_t));
    for (int i=0; i<nb; i++) {
      pthread_create(&tid[i], NULL, contribution, NULL);
    }
    for (int i=0; i<nb; i++) {
      pthread_join(tid[i], NULL);
    }
    long somme_threads;
    for (int i=0; i<nb; i++) {
      somme_threads += somme[i];
    }
    long long somme_calculée = (long long)m*((long long)m+1)/2;
    printf("Somme avec les threads: %ld, somme calculée: %lld", somme_threads, somme_calculée);
    
}

