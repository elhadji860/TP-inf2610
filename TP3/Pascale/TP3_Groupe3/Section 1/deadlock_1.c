#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define BUFFER_SIZE 5
#define N_THREADS 1
#define N_THREADS_2 10

int buffer[BUFFER_SIZE];
int ip;
int ic;

sem_t sem_initial;
sem_t sem_busy;
sem_t sem_critical;

int flag = 0;

void* producer(void* arg) {
    while (1) {
        sem_wait(&sem_initial); 
        sem_wait(&sem_critical); 
        buffer[ip] = rand() % 9 + 1;
        ip = (ip + 1) % BUFFER_SIZE;
        sem_post(&sem_critical);
        sem_post(&sem_busy);
        if (flag)
            break;
    }
    pthread_exit(NULL);
}

void* consumer(void* arg) {
    while (1) {
        sem_wait(&sem_busy); 
        sem_wait(&sem_critical); 
        int x = buffer[ic];
        ic = (ic + 1) % BUFFER_SIZE;
        sem_post(&sem_critical);
        sem_post(&sem_initial); 
        if(x == 0){
            break;
        }
    }
    pthread_exit(NULL);
}

void changeFlag(){
    flag = 1;
    printf("Flag changed\n");
}

int main() {
    pthread_t prod_thread[N_THREADS], cons_thread[N_THREADS_2];

    sem_init(&sem_initial, 0, BUFFER_SIZE);
    sem_init(&sem_busy, 0, 0);           
    sem_init(&sem_critical, 0, 1); 
    signal(SIGALRM, &changeFlag);      

    for(int i = 0; i < N_THREADS; i++){
        pthread_create(&prod_thread[i], NULL, producer, NULL);
    }

    for(int i = 0; i < N_THREADS_2; i++){
        pthread_create(&cons_thread[i], NULL, consumer, NULL);
    }

    alarm(1);

    for(int i = 0; i < N_THREADS; i++){
        pthread_join(prod_thread[i], NULL);
    }

    for(int i = 0; i < N_THREADS_2; i++){
        buffer[ip] = 0;
        ip = (ip + 1) % BUFFER_SIZE;
    }

    for(int i = 0; i < N_THREADS_2; i++){
        pthread_join(cons_thread[i], NULL);
    }

    sem_destroy(&sem_initial);
    sem_destroy(&sem_busy);
    sem_destroy(&sem_critical);

    return 0;
}

// 1. Il n'y a pas un interblocage:
// Condition 1 : Exclusion mutuelle
// sem_critical garantit que seul un thread à la fois modifie les variables buffer, ip et ic.
// Blocs responsables : lignes 24-29 et 38-43.

// Condition 2 : Détention et attente
// Les threads détiennent un sémaphore tout en attendant un autre.
// Producer : sem_initial, puis sem_critical.
// Consumer : sem_busy, puis sem_critical.

// Condition 3 : Pas de réquisition
// La condition n'est pas satisfaite.
// Les sémaphores sem_initial et sem_busy ne sont pas
// libérés par les threads qui les utilisent.
// Les consumers utilisent sem_busy et libèrent sem_initial et
// l'inverse pour les producers.
// sem_critical est quant à lui libéré par les consumers et les producers.

// Condition 4 : Attente circulaire
// La condition n'est pas satisfaite.
// le producer n'attend pas sem_busy et le consumer n'attend pas sem_initial.

// 2. Il s'agit du problème du producteur-consommateur

// 3. Pas de situation d'interblocage