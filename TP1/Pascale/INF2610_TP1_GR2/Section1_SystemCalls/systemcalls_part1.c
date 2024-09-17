/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * SystemCalls - part1.c
 *
 * Ajoutez vos noms, prénoms et matricules
 * Pascale Lafond 2204155
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main () {
    // TODO
    int descripteur_fichier = open("systemcalls_output2.txt", O_WRONLY | O_TRUNC);
    if (descripteur_fichier == -1) {
        printf("Appel système open a échoué.");
        exit(EXIT_FAILURE);
    }

    write(STDOUT_FILENO, "Saisissez votre texte suivi de CTRL-D:\n", 40);

    char buffer;
    while (read(STDIN_FILENO, &buffer, 1) > 0) {
        if (write(descripteur_fichier, &buffer, 1) < 0) {
            printf("Appel système write a échoué.\n");
            close(descripteur_fichier);
            exit(EXIT_FAILURE);
        } 
    }

    if (close(descripteur_fichier) < 0) {
        printf("Appel système close a échoué.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
