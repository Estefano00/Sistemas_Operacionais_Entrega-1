#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>  // mmap, mprotect, munmap
#include <string.h>    // strcpy
#include <unistd.h>    // getpid

int main() {
    size_t size = 4096;

    // Mapeia memória anônima com permissão de leitura e escrita
    char *addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Escreve algo na região mapeada
    strcpy(addr, "Texto inicial na memória.\n");
    printf("Antes do mprotect: %s", addr);

    // Altera a proteção para somente leitura
    if (mprotect(addr, size, PROT_READ) == -1) {
        perror("mprotect");
        munmap(addr, size);
        return 1;
    }

    printf("Proteção alterada para leitura apenas.\n");
    printf("Tentando escrever após tornar a região read-only...\n");
    // Se descomentar a linha abaixo, possivelmente causará um segfault:
    // addr[0] = 'X';

    // Desfaz o mapeamento
    if (munmap(addr, size) == -1) {
        perror("munmap");
        return 1;
    }

    return 0;
}