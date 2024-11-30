#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main() {
    int fd[2];

    pipe(fd);

    if(!fork()){
        close(fd[1]);
        char* string[6];    
        read(fd[0], string, 6);
        exit(0);
    }

    if(!fork()){
        close(fd[1]);
        close(fd[0]);
        execlp("ls", "ls", "-l", NULL);
        exit(0);
    }

    char string[6] = "Salut!";
    write(fd[1], &string, strlen(string));
    while(wait(NULL) > 0);
    printf("Process completed!\n");
    close(fd[0]);
    close(fd[1]);
    return 0;
}

// 1. Il y a un interblocage:
// Condition 1 : Exclusion mutuelle
// Tous les enfants ferment le tube en écriture, donc le parent est l'unique écrivain
// et le premier enfant est l'unique lecteur.
// Blocs responsables : lignes 11-14 et 27.

// Condition 2 : Détention et attente
// Le parent detient l'écriture, mais attend la fin des processus fils
// et le premier fils detient la fermeture, mais attend l'écriture dans le tube.

// Condition 3 : Pas de réquisition
// Le parent ne peut pas forcer ses enfants à se terminer 
// et les fils ne peuvent pas forcer l'écriture dans le tube.

// Condition 4 : Attente circulaire
// Le parent attend la fin de ses fils avant d'écrire dans le tube
// et le premier fils attend une écriture dans le tube avant de se terminer.

// 2. Il s'agit du problème du lecteur-rédacteur

// 3. Faire en sorte que le parent écrit dans le tube avant de se mettre en attente 
// permet de mettre fin à la condition d'attente circulaire 
// et de Détention et attente