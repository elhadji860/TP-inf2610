#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>
 #include <signal.h>

int *tampon;
int tailleTampon;
int nombrechiffresGeneres;
int nombreChiffresConsommes;
sem_t libre;
sem_t occupe;
sem_t tourProducteurs; // pour eviter les famines entre producteur
sem_t tourConsommateurs; // pour eviter les famines entre consommateur
sem_t mutex; // pour assurer l'acces exclusif aux donnees du tampon
int ic = 0;
int ip = 0;
bool flag_de_fin = false;


void* producteur(void* pid) {
    int sommeGen  = 0;
    srand(time(NULL));

    if (*(int *) pid < tailleTampon) { 
        while(1){
            sem_wait(&tourProducteurs);
            sem_wait(&libre);
            sem_post(&tourProducteurs);

            sem_wait(&mutex);
        
            int nombreAleatoire = (rand ()%9) + 1;
            ++nombrechiffresGeneres;


            tampon[ip] = nombreAleatoire;
            sommeGen += nombreAleatoire;
            ip = (ip + 1) % tailleTampon;
        
            sem_post(&mutex);


            sem_post(&occupe);

            if(flag_de_fin)
                break;
        }
    }
    
    else{
        while(1){
            sem_wait(&libre);
            sem_post(&occupe);
        }
    }
    
    int *exitValue = malloc(sizeof(int));
    *exitValue = sommeGen;
    pthread_exit (exitValue);
}

void* consommateur(void* cid) {
    int sommeLu = 0;
    while(1){
        sem_wait(&tourConsommateurs);
        sem_wait(&occupe);
        sem_post(&tourConsommateurs);

        sem_wait(&mutex);

        int nombreLu = tampon[ic];
        sommeLu += nombreLu;
        ic = (ic + 1) % tailleTampon;
        ++nombreChiffresConsommes;
        
        sem_post(&mutex);

        sem_post(&libre);

        if(nombreLu == 0){
            sem_wait(&mutex);
            --nombreChiffresConsommes;
            sem_post(&mutex);
            break;
        }

    }


    int *exitValue = malloc(sizeof(int));
    *exitValue = sommeLu;
    pthread_exit (exitValue);
}

void handle_alarm(int sig) {
    flag_de_fin = true;
}
// ...

int main(int argc, char* argv[]) {
    // Les paramètres du programme sont, dans l'ordre :
    // le nombre de producteurs, le nombre de consommateurs
    // et la taille du tampon.
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <nb_producteurs> <nb_consommateurs> <taille_tampon>\n", argv[0]);
        exit(1);
    }

    int nombreProducteurs = atoi(argv[1]);
    int nombreConsommateurs = atoi(argv[2]);
    tailleTampon = atoi(argv[3]);

    if (signal(SIGALRM, handle_alarm) == SIG_ERR) {
        perror("Error registering signal handler");
        exit(1);
    }


    sem_init(&libre, 1, tailleTampon);
    sem_init(&occupe, 1, 0);
    sem_init(&tourConsommateurs, 1, 1);
    sem_init(&tourProducteurs, 1, 1);
    sem_init(&mutex, 1, 1);

    pthread_t producteursTids[nombreProducteurs];
    pthread_t consommateursTids[nombreConsommateurs];
    tampon = malloc(sizeof(int) * tailleTampon);

    for(int i = 0; i < tailleTampon; ++i){
        tampon[i] = 0;
    }

    int argumentThreadsProd[nombreProducteurs];
    int argumentThreadsCons[nombreConsommateurs];

    for(int i = 0; i < nombreProducteurs; ++i){
        argumentThreadsProd[i] = i;
        pthread_create(&producteursTids[i], NULL, producteur, (void*)&argumentThreadsProd[i]);   
    }
    for(int i = 0; i < nombreConsommateurs; ++i){
        argumentThreadsCons[i] = i;
        pthread_create(&consommateursTids[i], NULL, consommateur, (void *)&argumentThreadsCons[i]);   
    }

    int sommeTotaleGeneree = 0;
    int sommeTotaleLue = 0;
    
    alarm(1);
    
    
    for (int i = 0 ; i < nombreProducteurs; ++i){
        int* pstatus;
        pthread_join(producteursTids[i],(void**) &pstatus);
        sommeTotaleGeneree += *pstatus;
        free(pstatus);
    }

    

    sem_wait(&mutex);
    int i = 0;
    while (i < nombreConsommateurs)
    {
        tampon[ip] = 0;
        ip = (ip + 1) % tailleTampon;
        ++i;
    }
    sem_post(&mutex);

    pthread_t tidFinishThread;
    pthread_create(&tidFinishThread, NULL, producteur, (void *) &tailleTampon);

    for (int i = 0 ; i < nombreConsommateurs; ++i){
        int* pstatus;
        pthread_join(consommateursTids[i],(void**) &pstatus);
        sommeTotaleLue += *pstatus;
        free(pstatus);
    }

   pthread_cancel(tidFinishThread);

   printf("nombre de chiffres consommes: %d \nnombre de chiffres generes: %d \nsomme des chiffres generes: %d \nsomme des chiffres consommes: %d\n", 
   nombreChiffresConsommes, nombrechiffresGeneres, sommeTotaleGeneree, sommeTotaleLue);
   
    free(tampon);
    return 0;
}
