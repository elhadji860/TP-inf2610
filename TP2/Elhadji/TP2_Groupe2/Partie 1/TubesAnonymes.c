// Ajouter les directives d'inclusion nécessaires
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include<sys/wait.h>

int main() {
    int fd_rev1[2];
    pipe(fd_rev1);

    if(fork()==0){
        close(fd_rev1[0]);
        int fd_inv = open("In.txt", O_RDONLY);

        dup2(fd_inv, STDIN_FILENO); close(fd_inv);
        dup2(fd_rev1[1], STDOUT_FILENO); close(fd_rev1[1]);

        execlp("rev", "rev", NULL);

        _exit(1);
    }

    int fd_rev2[2];
    pipe(fd_rev2);

     if(fork()== 0){
        close(fd_rev1[1]);
        close(fd_rev2[0]);
        
        dup2(fd_rev1[0],STDIN_FILENO); close(fd_rev1[0]);
        dup2(fd_rev2[1],STDOUT_FILENO); close(fd_rev2[1]);

        execlp("rev", "rev", NULL);

        _exit(1);
        
     }

    close(fd_rev1[1]);
    close(fd_rev1[0]);

     if (fork() == 0){
        close(fd_rev2[1]);

        dup2(fd_rev2[0], STDIN_FILENO); close(fd_rev2[0]);

        execlp("diff", "diff", "-", "In.txt", "-s",NULL);

        _exit(1);
     }
    

    close(fd_rev2[0]);
    close(fd_rev2[1]);
    while(wait(NULL) < 0);
    
    return 0;
}
