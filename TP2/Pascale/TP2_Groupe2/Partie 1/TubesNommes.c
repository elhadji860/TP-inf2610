// Ajouter les directives d'inclusion nécessaires
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>


int main() {
    mkfifo("tube", 0666);

    // Premier rev
    if (fork() == 0) {
        int fichier = open("In.txt", O_RDONLY);
        dup2(fichier, STDIN_FILENO);
        close(fichier);

        int fd = open("tube", O_WRONLY);
        dup2(fd, STDOUT_FILENO);
        close(fd);

        execlp("rev", "rev", NULL);
        exit(1); // En cas d'erreur
    }

    // Deuxième rev
    if (fork() == 0) {
        int fifo_fd = open("tube", O_RDONLY);
        dup2(fifo_fd, STDIN_FILENO);
        close(fifo_fd);

        int tube_fd[2];
        pipe(tube_fd);
        dup2(tube_fd[1], STDOUT_FILENO);

        close(tube_fd[0]);
        close(tube_fd[1]);

        execlp("rev", "rev", NULL);
        exit(1); // En cas d'erreur
    }

    // Diff
    if (fork() == 0) {
        int tube_fd[2];
        pipe(tube_fd);
        dup2(tube_fd[0], STDIN_FILENO);

        close(tube_fd[0]);
        close(tube_fd[1]);

        execlp("diff", "diff", "-", "In.txt", "-s", NULL);
        exit(1); // En cas d'erreur
    }

    wait(NULL);
    wait(NULL);
    wait(NULL);

    unlink("tube");

    return 0;
}
