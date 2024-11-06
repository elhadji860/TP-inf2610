// Ajouter les directives d'inclusion nécessaires
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
    mkfifo("tube_rev1_rev2", 0600);
    mkfifo("tube_rev2_diff", 0600);

    if(fork()==0){
        int fd_inv = open("In.txt", O_RDONLY);
        int fd_tube_out = open("tube_rev1_rev2", O_WRONLY);

        dup2(fd_inv, STDIN_FILENO); close(fd_inv);
        dup2(fd_tube_out, STDOUT_FILENO); close(fd_tube_out);

        execlp("rev", "rev", NULL);

        _exit(1);
    }


     if(fork()== 0){

        int fd_tube_out = open("tube_rev2_diff", O_WRONLY);
        int fd_tube_in = open("tube_rev1_rev2", O_RDONLY);
        
        dup2(fd_tube_in, STDIN_FILENO); close(fd_tube_in);
        dup2(fd_tube_out, STDOUT_FILENO); close(fd_tube_out);

        execlp("rev", "rev", NULL);
        
        _exit(1);
     }

     if (fork() == 0){
        int fd_tube_in = open("tube_rev2_diff", O_RDONLY);

        dup2(fd_tube_in, 0); close(fd_tube_in);

        execlp("diff", "diff", "-", "In.txt", "-s",NULL);

        _exit(1);
     }
    
    while(wait(NULL) < 0);

    unlink("tube_rev1_rev2");
    unlink("tube_rev2_diff");
    
    return 0;
}
