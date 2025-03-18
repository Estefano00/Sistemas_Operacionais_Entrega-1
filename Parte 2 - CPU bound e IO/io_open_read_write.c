#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>   // open
#include <unistd.h>  // read, write, close
#include <string.h>  // strlen

int main() {
    const char *filename = "example.txt";

    // Abre (ou cria) um arquivo para leitura/escrita
    int fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Escreve algo no arquivo
    const char *text = "Hello, file!\n";
    ssize_t written = write(fd, text, strlen(text));
    if (written == -1) {
        perror("write");
        close(fd);
        return 1;
    }

    // Volta o offset para o início do arquivo (SEEK_SET = 0)
    off_t offset = lseek(fd, 0, SEEK_SET);
    if (offset == (off_t)-1) {
        perror("lseek");
        close(fd);
        return 1;
    }

    // Lê o conteúdo do arquivo
    char buffer[128];
    ssize_t read_bytes = read(fd, buffer, sizeof(buffer) - 1);
    if (read_bytes == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    buffer[read_bytes] = '\0';

    printf("Conteúdo lido do arquivo: %s\n", buffer);

    // Fecha o arquivo
    if (close(fd) == -1) {
        perror("close");
        return 1;
    }

    return 0;
}