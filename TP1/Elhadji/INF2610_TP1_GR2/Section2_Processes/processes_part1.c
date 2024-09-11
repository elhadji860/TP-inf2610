/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Processes - part1.c
 *
 * Ajoutez vos noms, prénoms et matricules
*/

#include "libprocesslab/libprocesslab.h"


void question1(){

    registerProc(getpid(), getppid(), 0, 0);

    int nbProcessDescendant = 0;

       //PEs 1.2 et 1.1
    for(int n = 1; n < 3; ++n ){
        //PEs 2.1 et 2.2
        if (fork() == 0) { 
            registerProc(getpid(), getppid(), 1, n);

            int nbProcessEnfant = 0;

            if (fork() == 0){
                registerProc(getpid(), getppid(), 2, n);

                _exit(0);   
            }

            int statusLevel_2;
            while(wait(&statusLevel_2) > 0){
                ++nbProcessEnfant;
            };
            _exit(nbProcessEnfant);
        }
    }

    // PE 1.3
    if (fork() == 0){
        //PEs 2.3, 2.4, 2.5 et 2.6
        registerProc(getpid(), getppid(), 1, 3);

        int nbProcessEnfant = 0;

        for (int n = 3; n < 7; n++){

            if (fork() == 0){
                registerProc(getpid(), getppid(), 2, n);

                _exit(0);
            }
        
        } 
        int statusLevel_2;
        while(wait(&statusLevel_2) > 0){
            ++nbProcessEnfant;
        };
        _exit(nbProcessEnfant);
    }

    int statusLevel_1;
   while(wait(&statusLevel_1) > 0){
    nbProcessDescendant += WEXITSTATUS(statusLevel_1);
   };
    // TODO : donner le nombre de processus enfants crees par le process level0 et ses descendants
    printf("nombre de processus crees par le processus level0 et par ses descendants : %d \n", nbProcessDescendant);

    // TODO : informations sur les differents processus
    printProcRegistrations();

    //TODO : executer ls
    execlp("ls", "ls", "-l", NULL);
};

    

