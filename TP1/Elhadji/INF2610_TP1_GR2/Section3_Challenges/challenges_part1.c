/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Challenges - part1.c
 *
 * Ajoutez vos noms, prénoms et matricules
*/
#include "challenges_part1.h"

const int sizeOfFileNames = 250;
const int sizeOfBuffer = 450;
const int erreur_ouverture = -1;


int processsTreeGenertor(char* path){
    DIR *directory;
    struct dirent *entry;

    int nbSubDirectories = 0;
    int nbTxtFile = 0;
    int length1 = strlen(path);
    char namesOfFIles[sizeOfFileNames];
    strcpy(namesOfFIles, " "); 



    directory = opendir(path);
    if (directory == NULL){
        nbSubDirectories = 0;
        nbTxtFile = 0;
        printf("erreur ouverture  + %s\n", path);
        exit(0);
    }


    while ((entry = readdir(directory)) != NULL){
        
        struct stat st;
        int length2 = strlen(entry->d_name) + 1;
        char nextPath[length1 + length2 + 10];
        strcpy(nextPath, path);
        strcat(nextPath, "/");
        strcat(nextPath, entry->d_name);

        
        stat(nextPath, &st);

        if(S_ISDIR(st.st_mode) && strcmp(entry->d_name,"..") && strcmp(entry->d_name,".")){
            ++nbSubDirectories;
            if(fork() == 0){
                exit(processsTreeGenertor(nextPath));
            }
        }
        else if (S_ISREG(st.st_mode)){
            strcat(namesOfFIles, "\n\t");
            strcat(namesOfFIles, entry->d_name);
            ++nbTxtFile;
        }
        int status;
           
        while(wait(&status) > 0){
            nbTxtFile += WEXITSTATUS(status); 
        }
    }
    
    
/*  – L’emplacement du répertoire;
    – Le numéro d’identification du répertoire (Indice: on se souvient qu’à chaque répertoire ren-
      contré, un nouveau processus est créé);
    – Le numéro d’identification du répertoire supérieur immédiat;
    – La liste des fichiers texte se trouvant dans le répertoire.
*/

    int fd = open("./challenges_output.txt", O_WRONLY | O_APPEND);
    if (fd == erreur_ouverture){
       exit(EXIT_FAILURE);
    }

    char buffer[sizeOfBuffer];
    int len = snprintf(buffer, sizeof(buffer), "emplacement du répertoire : %s\n", path);
    write (fd, buffer, len);
    
    char buffer1[sizeOfBuffer];
    int len1 = snprintf(buffer1, sizeof(buffer1), "numéro d’identification du répertoire: %d\n", getpid());
    write (fd, buffer1, len1);
    
    char buffer2[sizeOfBuffer];
    int len2 = snprintf(buffer2, sizeof(buffer2),"numéro d’identification du répertoire supérieur immédiat: %d\n", getppid());
    write (fd, buffer2, len2);
    
    char buffer3[sizeOfBuffer];
    int len3 = snprintf(buffer3, sizeof(buffer3), "Fichiers texte: %s\n\n", namesOfFIles);
    write (fd, buffer3, len3);

    close(fd);
    closedir(directory);
    return(nbTxtFile);
}

int main(int argc, char*argv[])
{
    FILE* file = fopen("./challenges_output.txt", "w");
    printf("nombre total de fichier texte : %d \n",processsTreeGenertor("./root"));
    fclose(file);
    return 0;
}



