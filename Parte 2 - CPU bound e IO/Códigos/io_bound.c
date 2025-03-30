#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fp = fopen("testio.txt", "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    // Ajuste o número de linhas conforme desejar.
    // Quanto maior, mais tempo de escrita, mais I/O-bound fica a aplicação.
    int lines = 300000;  

    for (int i = 0; i < lines; i++) {
        // Escreve no arquivo
        fprintf(fp, "Linha número %d\n", i);
        // Força flush a cada escrita, forçando E/S de disco frequente
        fflush(fp);
    }

    fclose(fp);
    return 0;
}