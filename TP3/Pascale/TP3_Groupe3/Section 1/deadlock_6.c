#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define N_THREADS 5

int flag = 0;

void* function_that_does_something(void* arg) {
    while (1) {
        if (flag)
            break;
    }
    pthread_exit(NULL);
}

void changeFlag(){
    if (rand() % 10 == 10)
        flag = 1;
    printf("Signal called\n");
}

int main() {
    pthread_t threads[N_THREADS];
 
    signal(SIGALRM, &changeFlag);      

    for(int i = 0; i < N_THREADS; i++){
        pthread_create(&threads[i], NULL, function_that_does_something, NULL);
    }

    alarm(1);

    for(int i = 0; i < N_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}

// 1. Il n'y a pas un interblocage:
// Condition 1 : Exclusion mutuelle
// Cette condition n'est pas rempli,
// Tous les threads partagent le même flag.
// Bloc responsable : ligne 10.

// Condition 2 : Détention et attente
// Cette condition n'est pas remplie.
// Lorsqu'un thread obtient un flag valide,
// il met fin à son exécution et n'attend pas d'autre ressource
// d'une autre ressource.

// Condition 3 : Pas de réquisition
// Cette condition n'est pas remplie.
// La ressource flag n'est contrôlée que par 
// le processus principal et est utilisé seulement par
// les threads.

// Condition 4 : Attente circulaire
// Cette condition n'est pas remplie.
// Comme l'attente dépend de l'aléatoire (rand() % 10 == 10), 
// il n'y a pas un véritable cycle d'attente entre les threads.

// 2. Il ne correspond à aucun exemple vu en classe.

// 3. Pas d'interblocage dans ce cas.