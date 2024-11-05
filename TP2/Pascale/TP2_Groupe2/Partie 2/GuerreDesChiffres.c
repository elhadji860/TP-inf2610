#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>

// variables globales
int *tampon;
int taille_tampon;
int in, out = 0;
int total_produit, total_consomme = 0;
int somme_produit, somme_consomme = 0;
int flag_de_fin = 0;

// mutex et sémaphores
pthread_mutex_t mutex;
// Sem libre pour les producteurs et sem plein pour les consommateurs
sem_t vide, plein;

void* producteur(void* pid) {
    int id = *((int *)pid);
    int somme_locale = 0;
    int *resultat = malloc(sizeof(int));
    srand(time(NULL) + id);

    while (1) {
        int x = (rand() % 9) + 1;

        sem_wait(&vide);
        pthread_mutex_lock(&mutex);
        tampon[in] = x;
        in = (in + 1) % taille_tampon;
        total_produit++;
        somme_produit += x;
        somme_locale += x;
        pthread_mutex_unlock(&mutex);
        sem_post(&plein);

        if (flag_de_fin) break;
    }
    *resultat = somme_locale;
    pthread_exit(resultat);
}

void* consommateur(void* cid) {
    int id = *((int *)cid);
    int somme_locale = 0;
    int *resultat = malloc(sizeof(int));

    while (1) {
        if (tampon[out] == 0) break;

        sem_wait(&plein);
        pthread_mutex_lock(&mutex);
        int x = tampon[out];
        out = (out + 1) % taille_tampon;
        total_consomme++;
        somme_consomme += x;
        somme_locale += x;
        pthread_mutex_unlock(&mutex);
        sem_post(&vide);

        
    }
    *resultat = somme_locale;
    pthread_exit(resultat);
}

// Gestionnaire de SIGALRM
void handler(int sig) {
    flag_de_fin = 1;
}

int main(int argc, char* argv[]) {
    // Les paramètres du programme sont, dans l'ordre :
    // le nombre de producteurs, le nombre de consommateurs
    // et la taille du tampon.
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <nb_producteurs> <nb_consommateurs> <taille_tampon>\n", argv[0]);
        exit(1);
    }

    int nb_producteurs = atoi(argv[1]);
    int nb_consommateurs = atoi(argv[2]);
    taille_tampon = atoi(argv[3]);

    tampon = (int *)malloc(taille_tampon * sizeof(int));

    pthread_mutex_init(&mutex, NULL);
    sem_init(&vide, 0, taille_tampon); // places libres dans tampon
    sem_init(&plein, 0, 0); // Éléments dispo dans tampon


    pthread_t producteurs[nb_producteurs];
    pthread_t consommateurs[nb_consommateurs];
    int ids_producteurs[nb_producteurs];
    int ids_consommateurs[nb_consommateurs];

    for (int i=0; i<nb_producteurs; i++) {
        ids_producteurs[i] = i;
        pthread_create(&producteurs[i], NULL, producteur, &ids_producteurs[i]);

    }

    for (int i=0; i<nb_consommateurs; i++) {
        ids_consommateurs[i] = i;
        pthread_create(&consommateurs[i], NULL, consommateur, &ids_consommateurs[i]);
        
    }

    signal(SIGALRM, handler);
    alarm(1);

    int somme_totale_produite = 0;
    for (int i=0; i<nb_producteurs; i++) {
        int *resultat;
        pthread_join(producteurs[i], (void **)&resultat);
        somme_totale_produite+= *resultat;
    }

    // Mettre les 0 dans tampon 
    for (int i=0; i<nb_consommateurs; i++) {
        sem_wait(&vide);
        pthread_mutex_lock(&mutex);
        tampon[in] = 0;
        in = (in + 1) % taille_tampon;
        pthread_mutex_unlock(&mutex);
        sem_post(&plein);
    }

    int somme_totale_consomme = 0;
    for (int i=0; i<nb_consommateurs; i++) {
        int *resultat;
        pthread_join(consommateurs[i], (void **)&resultat);
        somme_totale_consomme += *resultat;
    }

    printf("Somme produite: %d, Somme consommée: %d\n", somme_totale_produite, somme_totale_consomme);
    printf("Chiffres produits: %d, Chiffres consommés: %d\n", total_produit, total_consomme);

    free(tampon);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&vide);
    sem_destroy(&plein);

    return 0;
}
