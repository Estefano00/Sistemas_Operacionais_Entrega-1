#include <stdio.h>
#include <unistd.h>  // sbrk

int main() {
    // Obtém a posição atual do program break
    void *initial_break = sbrk(0);
    printf("Program break inicial: %p\n", initial_break);

    // Aumenta o program break em 1024 bytes
    if (sbrk(1024) == (void *)-1) {
        perror("sbrk");
        return 1;
    }
    void *new_break = sbrk(0);
    printf("Program break após alocar 1KB: %p\n", new_break);

    // Diminui novamente
    if (sbrk(-1024) == (void *)-1) {
        perror("sbrk");
        return 1;
    }
    void *final_break = sbrk(0);
    printf("Program break final após liberar 1KB: %p\n", final_break);

    return 0;
} 