#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // fork, execve
#include <sys/wait.h> // wait

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Processo filho
        printf("Filho (PID=%d), chamando execve para ls...\n", getpid());
        char *args[] = {"/bin/ls", "-l", NULL};
        // Substitui a imagem do processo pelo executável /bin/ls
        execve("/bin/ls", args, NULL);

        // Só chega aqui se execve falhar
        perror("execve");
        exit(1);
    } else {
        // Processo pai
        int status;
        wait(&status);  // aguarda o término do filho
        printf("Pai (PID=%d), filho terminou.\n", getpid());
        if (WIFEXITED(status)) {
            printf("Filho saiu com código %d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}