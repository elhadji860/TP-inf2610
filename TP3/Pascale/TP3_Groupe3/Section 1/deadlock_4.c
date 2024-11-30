#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_THREADS 5
#define MAX_ROUNDS 3

pthread_mutex_t locks[NUM_THREADS];

void* cons(void* arg) {
    int id = *(int*)arg;
    int rounds = 0;

    while (rounds < MAX_ROUNDS) {
        sleep(1);
        int left = id;
        int right = (id + 1) % NUM_THREADS;
        
         while (1) {
            if (pthread_mutex_trylock(&locks[left]) == 0) {
                if (pthread_mutex_trylock(&locks[right]) == 0) {
                    break;
                } else {
                    pthread_mutex_unlock(&locks[left]);
                }
            }
            sleep(1);
        }

        sleep(1);
        rounds++;
        pthread_mutex_unlock(&locks[left]);
        pthread_mutex_unlock(&locks[right]);
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_mutex_init(&locks[i], NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, cons, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

// 1. Il y a un interblocage:
// Condition 1 : Exclusion mutuelle
// Chaque thread attend d’obtenir un verrou, 
// et un verrou ne peut être détenu que par un seul thread à la fois.
// Blocs responsables : lignes 11-26.

// Condition 2 : Détention et attente
// Lorsqu'un thread est en possession d'un premier verrou, 
// il va attendre pour un deuxième verrou (lignes 19-21)

// Condition 3 : Pas de réquisition
// Les locks ne peuvent être libérés que par
// les threads qui les détiennent

// Condition 4 : Attente circulaire
// Si tous les threads sont en attente de leur deuxième verrou,
// Aucun thread ne pourra continuer car le verrou sera detenu 
// par un autre thread

// 2. Cela correspond au problème des philosophes.

// 3. Pour éviter cette situation d’interblocage, une
// solution est d’appliquer la politique de « tout
// ou rien » : un thread déteint 0 ou 2 
// locks mais jamais un seul lock.