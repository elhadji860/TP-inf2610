#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define ENTRY_CODE 7
#define N_THREADS 5

sem_t sem_critical;

int try_lock(int key){
    if(key == ENTRY_CODE){
        printf("Door unlocked\n");
        return 1;
    }
    sleep(1);
    return 0;
}

void* door(void* args){
    while(1){
        sem_wait(&sem_critical);
        int key = rand() % 30;
        if(try_lock(key))
            pthread_exit(NULL);
        sem_post(&sem_critical);
    }
}
int main() {

    sem_init(&sem_critical, 0, 1); 
    pthread_t threads[N_THREADS];

    for(int i = 0; i < N_THREADS; i++){
        pthread_create(&threads[i], NULL, door, NULL);
    }

    for(int i = 0; i < N_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}

// 1. Il n'y a pas un interblocage:
// Condition 1 : Exclusion mutuelle
// Chaque thread attend d’obtenir sem_critical, 
// et sem_critical ne peut être détenu que par un seul thread.
// Blocs responsables : lignes 24-28.

// Condition 2 : Détention et attente
// Cette condition n'est pas remplie.
// Comme il n'y a qu'un seul sémaphore, 
// un thread possèdant sem_critical n'est pas en attente 
// d'une autre ressource.

// Condition 3 : Pas de réquisition
// Le sémaphore ne peut être libéré que par 
// le thread qui l'utilise.

// Condition 4 : Attente circulaire
// Tous les thread sauf celui en possession de sem_critical
// se placent en attente de ce dernier.

// 2. Il ne correspond à aucun exemple vu en classe.

// 3. Pas d'interblocage dans ce cas.