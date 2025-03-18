#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>  // mmap, munmap
#include <string.h>    // strcpy
#include <unistd.h>    // close, getpid

int main() {
    size_t size = 4096;  // Tamanho da região a mapear

    // Mapeia memória anônima com permissão de leitura e escrita
    char *map = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Escreve algo na região mapeada
    strcpy(map, "Hello from mmap!\n");

    // Exibe o conteúdo
    printf("Conteúdo mapeado: %s", map);

    // Desfaz o mapeamento
    if (munmap(map, size) == -1) {
        perror("munmap");
        return 1;
    }

    return 0;
}