#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#define N_THREADS 10

sem_t mutex;

int flag = 0;
int key;

void* is_this_a_deadlock(void * args) {
    sem_wait(&mutex);
    int a = 0, b = 0, c = 0, d = 0;
    int i, j, k, m, n;

    a += 1;
    b += 2;
    c += 3;
    d += 4;
    a -= 1;
    b -= 2;
    c -= 3;
    d -= 4;

    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++) {
            for (k = 0; k < 5; k++) {
                printf(".");
            }
        }
    }
    printf("\n");

    for (i = 0; i < 10; i++) {
        printf("Loop 1, Iteration: %d\n", i);
    }
    key = rand() % 100;
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 5; j++) {
            for (k = 0; k < 3; k++) {
                printf("+");
            }
        }
    }
    printf("\n");

    m = a + b;
    n = c - d;
    a = m + n;
    b = a - n;
    c = b + m;
    d = a + c;

    printf("Hello World 1\n");
    printf("Hello World 2\n");
    printf("Hello World 3\n");

    for (i = 0; i < 10; i++) {
        printf("Another loop: %d\n", i);
        a += i;
        b -= i;
        c += i * 2;
        d -= i * 2;
    }

    if (a > 0) {
        if (key == 17) while(1);
        printf("A is positive\n");
    } else {
        printf("A is not positive\n");
    }

    if (b < 0) {
        printf("B is negative\n");
    } else {
        printf("B is not negative\n");
    }

    printf("This function is long but doesn't do much.\n");
    sem_post(&mutex);
}

int main() {  
    pthread_t threads[N_THREADS];
    sem_init(&mutex, 0, 1); 
    srand(time(NULL));
    
    for(int i = 0; i < N_THREADS; i++){
        pthread_create(&threads[i], NULL, is_this_a_deadlock, NULL);
    }

    for(int i = 0; i < N_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}

// 1. Il n'y a pas un interblocage:
// Condition 1 : Exclusion mutuelle
// Avec le mutex, l'accès à la fonction is_this_a_deadlock est restreinte
// à un seul processus à la fois
// Blocs responsables : lignes 16-84.

// Condition 2 : Détention et attente
// Cette condition n'est pas remplie, 
// car le processus qui possède le mutex n'est pas en attente d'une 
// autre ressource.

// Condition 3 : Pas de réquisition
// Le mutex est libérer seulement par le 
// processus qui le possède.

// Condition 4 : Attente circulaire
// Chaque processus attend la libération du mutex pour
// s'exécuter, celui-ci étant détenu par un des processus

// 2. Il ne correspond à aucun exemple vu en classe.

// 3. Aucun interblocage ici.