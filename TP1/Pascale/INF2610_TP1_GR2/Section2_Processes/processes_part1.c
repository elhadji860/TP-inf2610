/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Processes - part1.c
 *
 * Ajoutez vos noms, prénoms et matricules
 * Pascale Lafond, 2204155
*/

#include "libprocesslab/libprocesslab.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void question1()
{
    // TODO
    int nombre_enfants = 0;
    registerProc(getpid(), getppid(), 0, 0);
    if(fork() == 0) { //level1.1
        if (fork() == 0) { //level2.1
            registerProc(getpid(), getppid(), 2, 1);
            nombre_enfants++;
            _exit(nombre_enfants);
        }
        wait(&nombre_enfants);
        registerProc(getpid(), getppid(), 1, 1);
        _exit(WEXITSTATUS(nombre_enfants)+1);
    }
    int enfants1; wait(&enfants1);
    if(fork() == 0) { //level1.2
        if(fork() == 0) { //level2.2
            registerProc(getpid(), getppid(), 2, 2);
            nombre_enfants++;
            _exit(nombre_enfants);
        }
        wait(&nombre_enfants);
        registerProc(getpid(), getppid(), 1, 2);
        _exit(WEXITSTATUS(nombre_enfants)+1);
    }
    int enfants2; wait(&enfants2);
    if(fork() == 0) { //level1.3
        if(fork() == 0) { //level2.3
            registerProc(getpid(), getppid(), 2, 3);
            nombre_enfants++;
            _exit(nombre_enfants);
        }
        if(fork() == 0) { //level2.4
            registerProc(getpid(), getppid(), 2, 4);
            nombre_enfants++;
            _exit(nombre_enfants);
        }
        if(fork() == 0) { //level2.5
            registerProc(getpid(), getppid(), 2, 5);
            nombre_enfants++;
            _exit(nombre_enfants);
        }
        if(fork() == 0) { //level2.6
            registerProc(getpid(), getppid(), 2, 6);
            nombre_enfants++;
            _exit(nombre_enfants);
        }
        int enf1, enf2, enf3, enf4; wait(&enf1); wait(&enf2); wait(&enf3); wait(&enf4);
        registerProc(getpid(), getppid(), 1, 3);
        nombre_enfants = WEXITSTATUS(enf1)+WEXITSTATUS(enf2)+WEXITSTATUS(enf3)+WEXITSTATUS(enf4);
        _exit(nombre_enfants+1);
    }
    int enfants3; wait(&enfants3);
    nombre_enfants = WEXITSTATUS(enfants1) + WEXITSTATUS(enfants2) + WEXITSTATUS(enfants3);
    printf("%d enfants\n", nombre_enfants);
    printProcRegistrations();
    execl("/usr/bin/ls", "ls", "-l", NULL);
}

