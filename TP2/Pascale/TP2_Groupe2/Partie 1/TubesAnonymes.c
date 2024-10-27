// Ajouter les directives d'inclusion nécessaires
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


int main() {
    int tube1[2], tube2[2];

    pipe(tube1);
    pipe(tube2);

    // Premier rev
    if (fork() == 0) {
        int fichier = open("In.txt", O_RDONLY);
        dup2(fichier, STDIN_FILENO);
        close(fichier);

        dup2(tube1[1], STDOUT_FILENO);

        close(tube1[0]);
        close(tube1[1]);
        close(tube2[0]);
        close(tube2[1]);

        execlp("rev", "rev", NULL);
        exit(1); // En cas d'erreur
    }

    // Deuxième rev
    if (fork() == 0) {
        dup2(tube1[0], STDIN_FILENO);
        dup2(tube2[1], STDOUT_FILENO);

        close(tube1[0]);
        close(tube1[1]);
        close(tube2[0]);
        close(tube2[1]);

        execlp("rev", "rev", NULL);
        exit(1); // En cas d'erreur
    }

    // Diff
    if (fork() == 0) {
        dup2(tube2[0], STDIN_FILENO);

        close(tube1[0]);
        close(tube1[1]);
        close(tube2[0]);
        close(tube2[1]);

        execlp("diff", "diff", "-", "In.txt", "-s", NULL);
        exit(1); // En cas d'erreur
    }

    close(tube1[0]);
    close(tube1[1]);
    close(tube2[0]);
    close(tube2[1]);

    wait(NULL);
    wait(NULL);
    wait(NULL);
    
    return 0;
}
