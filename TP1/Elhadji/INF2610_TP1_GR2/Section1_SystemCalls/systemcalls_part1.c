/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * SystemCalls - part1.c
 *
 * Ajoutez vos noms, prénoms et matricules
*/
#include <stdlib.h>
#include <unistd.h>             
#include <stdio.h>             
#include <fcntl.h>
#include <string.h>

const int erreur_ouverture = -1;
const int stdout_fd = 1;
const int stdin_fd = 0;
const int longueur_entree = 1;
const int read_eof = 0;



int main () {
    // TODO : ouvrir le fichier nommé systemcalls_output2.txt en mode écriture et l’option O_TRUNC;
    int fd_systemcalls = open("./systemcalls_output2.txt", O_WRONLY | O_TRUNC);
    if (fd_systemcalls == erreur_ouverture){
       exit(EXIT_FAILURE);
    }

    // TODO : afficher à l’écran le texte “Saisissez votre texte suivi de CTRL-D :\n". La combinaison des touches CTRL et D (ˆD) indique une fin de fichier
    char message[] = "Saisissez votre texte suivi de CTRL-D :\n";
    int val_write = write (stdout_fd, message, strlen(message));

    // TODO : lire caractère par caractère des données à partir du clavier jusqu’à la rencontre d’une fin de fichier(ˆD)
    // sauvegarder dans le fichier systemcalls_output2.txt chaque caractère lu à partir du clavier
    int val_read = 1;
    while (val_read != read_eof){
        char entree;
        val_read = read(stdin_fd, &entree, longueur_entree);
        
        int val_write_fl = write(fd_systemcalls, &entree, longueur_entree);
    }

    // fermer le fichier systemcalls_output2.txt.
    int val_close = close(fd_systemcalls);

    return 0;
}
