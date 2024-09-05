/*
 * Ecole Polytechnique Montreal - GIGLL
 * Automne 2024
 * SystemCalls - part2.c
 *
 * Ajoutez vos noms, prénoms et matricules
*/

#include <stdlib.h>
#include <unistd.h>             
#include <stdio.h>             
#include <fcntl.h>
#include <string.h>


const int stdout_fd = 1;


void conclure_part21(){
    printf("\n");
}


void part21 ()
{
    //TODO : La deuxième ligne "77dbcb01f571f1c32g196c3a7d27f62e (printed using printf)" doit être affichée
    //en utilisant la fonction printf pour chaque caractère du message
    char message_printf[] = "77dbcb01f571f1c32g196c3a7d27f62e (printed using printf)";
    for (int i = 0; i < strlen(message_printf); ++i){
        printf("%c", message_printf[i]);
    }
    
    // TODO : La première ligne du message "77dbcb01f571f1c32g196c3a7d27f62e (printed using write)\n" doit
    //être affichée en utilisant un seul appel système write.
    char message_write[] = "77dbcb01f571f1c32g196c3a7d27f62e (printed using write)\n";
    int val_write = write(stdout_fd, message_write, strlen(message_write));

    conclure_part21();
}

void part22 ()
{
    // TODO : omplétez la fonction part22 du fichier systemcalls_part2.c pour qu’elle débute par un
    //appel à la fonction setvbuf, de la librairie stdio.h, avant de réaliser un traitement identique à celui de la
    //fonction part21.
    setvbuf(stdout ,NULL, _IONBF, 0);

    part21();
    
}

int main (int argc, char* argv[])
{
    
    if (argc!=2)
    {   printf("Le programme a un seul paramètre : 1 ou 2\n");
        return 1;
    }
    switch (atoi(argv[1])) {
         case 1:        part21();
                        break;
         case 2:        part22();
                        break;
        default:        printf(" Le paramètre du programme est invalide\n");
                        return 1;
    }
    return 0;
}
