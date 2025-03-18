# UNIVERSIDADE DE SÃO PAULO

## Primeira Entrega de Sistemas Operacionais

Estefano Nascimento 7970044

Pyerry Klyzlow Xavier 15484839

Ligia Keiko Carvalho 13242363

# Sumário

# 1. Introdução

Este documento apresenta a primeira entrega do projeto prático da disciplina de Sistemas Operacionais, tomando como referência a bibliografia do Tanenbaum e abordando conceitos fundamentais de chamadas de sistema (system calls), gerenciamento de memória, criação e controle de processos, além da distinção entre processos CPU-bound e I/O-bound. O objetivo principal é demonstrar, por meio de programas escritos em C, como o sistema operacional interage com aplicativos de usuário ao gerenciar recursos de hardware e software.

Nesta entrega, foram selecionadas nove chamadas de sistema, distribuídas em três categorias (gerenciamento de memória, processos e E/S-arquivos), as quais foram implementadas e analisadas. Além disso, dois programas adicionais foram desenvolvidos para demonstrar comportamentos típicos de processos CPU-bound e I/O-bound. As medições e observações aqui descritas foram realizadas no dia 16/03/2025, utilizando a ferramenta strace para inspecionar e contabilizar as chamadas de sistema, e a ferramenta /usr/bin/time para coletar dados sobre uso de CPU, tempo total de execução, trocas de contexto e outras estatísticas relevantes.

Os testes ocorreram no servidor andromeda.lasdpc.icmc.usp.br, acessado por meio de SSH, onde se encontra um ambiente Ubuntu 24.04.1 LTS instalado.

Ressalta-se que, embora o código possa funcionar em outras plataformas, é fortemente indicado que seja testado em sistemas Linux, pois as chamadas de sistema, ferramentas (strace, /usr/bin/time) e comportamento de processo podem variar em outros ambientes.

Nos tópicos seguintes, apresenta-se a estrutura de cada programa desenvolvido, a descrição de como executar cada um e as principais observações obtidas a partir das ferramentas de análise de chamadas de sistema e uso de recursos.

## 1.1 Breve Explicação e Instruções de Execução

Para compilar qualquer um dos códigos em C apresentados, basta utilizar o gcc ou outro compilador compatível. Por exemplo, se você tem um arquivo chamado nome_do_arquivo.c:

gcc nome_do_arquivo.c -o nome_do_arquivo

Após a compilação, o executável (por exemplo, nome_do_arquivo) pode ser executado normalmente:

./nome_do_arquivo

Os arquivos da Parte 1 (System Calls) encontram-se no diretório:

/Parte 1 - System Calls/Códigos

E os resultados gerados pelo strace estão em:

/Parte 1 - System Calls/strace

Já os códigos da Parte 2 (CPU-bound e I/O-bound) estão em:

/Parte 2 - CPU bound e IO/Códigos

E os resultados do strace ficam em:

/Parte 2 - CPU bound e IO/Strace

(no repositório do GitHub que hospeda o projeto).

## 1.2 Ferramentas Utilizadas

strace: Monitora e exibe todas as chamadas de sistema realizadas por um programa. Com a opção -T, ainda é possível verificar o tempo gasto em cada chamada. Exemplos de uso:

strace -T -o saida.txt ./nome_do_arquivo

* Nesse caso, o arquivo saida.txt conterá as informações sobre as system calls.

/usr/bin/time -v: Exibe estatísticas detalhadas de tempo e recursos usados durante a execução de um programa, incluindo tempo total, tempo em modo usuário, tempo em modo kernel, trocas de contexto voluntárias/involuntárias, entre outras métricas. Exemplo:

/usr/bin/time -v ./nome_do_arquivo

Essas ferramentas são fundamentais para analisar o comportamento das aplicações, tanto na parte de chamadas de sistema (Parte 1) quanto na avaliação de processos CPU-bound e I/O-bound (Parte 2).**

# 2. Parte 1 - System Calls

## 2.1.Justificativa

Na Parte 1 - System Calls, optou-se por contemplar de forma balanceada as principais áreas que um sistema operacional gerencia: memória, processos e entrada/saída de arquivos. Para isso, foram selecionados cinco scripts em C, que exemplificam três chamadas de sistema de gerenciamento de memória e mostram como funcionam as interações com o sistema para alocação, liberação e proteção de regiões de memória; um script que aborda a criação e controle de processos (incorporando chamadas como fork, exec e wait); e, por fim, um script para operações de E/S com arquivos, destacando chamadas como open, read, write e close. Abaixo, listamos os arquivos e seus propósitos gerais:

### memoria_mmap.c

Demonstra o uso de mapeamento de memória anônimo, permitindo que o processo reserve áreas de memória controladas diretamente pelo kernel (por meio de mmap) e as libere depois (com munmap).

### memoria_brk.c

Ilustra o ajuste dinâmico do “program break” do processo, usando sbrk (que, internamente, faz chamadas a brk).

Evidencia como é possível manipular a área de dados alocada para um processo além do heap padrão fornecido pela biblioteca de runtime em C.

### memoria_mprotect.c

Mostra como alterar as permissões de uma região de memória já mapeada (por exemplo, tornando-a somente leitura ou read/write) através da chamada mprotect.

Permite explorar o conceito de proteção de páginas de memória, fundamental para segurança e estabilidade do sistema.

### processo_fork_exec_wait.c

Aborda as chamadas clássicas para criação e gerenciamento de processos: fork (duplicação do processo atual), exec (substituição de imagem do processo para execução de um novo programa) e wait (sincronização do processo pai com o término do processo filho).

A escolha se deve à relevância desses conceitos para compreender a escalabilidade e concorrência em sistemas operacionais, bem como a forma com que o SO cria novos processos e carrega programas no espaço de usuário.

### io_open_read_write.c

Representa as operações mais básicas de entrada/saída em arquivos: abrir (open), ler (read), escrever (write) e fechar (close).

Demonstra como o kernel controla o acesso a dispositivos de armazenamento, tratando cada arquivo como um descritor e intermediando o fluxo de dados entre o espaço de usuário e o hardware.

### Resumo

A seleção dessas primitivas foi feita para cobrir aspectos essenciais dos sistemas operacionais, pois cada categoria (memória, processos e E/S) envolve mecânicas distintas de interação com o kernel. O nível de complexidade de cada script varia, mas todos requerem um entendimento prévio dos seguintes pontos:

Programação em C: por fornecer acesso relativamente direto às chamadas de sistema, é a linguagem mais apropriada para ilustrar esses conceitos;

Modelo de memória de um processo: para lidar com chamadas como mmap, mprotect e sbrk, é essencial compreender como o SO organiza as regiões de código, dados, heap e stack;

Conceitos de processos: entender a diferença entre processo pai e processo filho, bem como o que acontece quando um processo chama exec para carregar um novo binário;

Entrada/saída em baixo nível: compreender como o sistema operacional gerencia descritores de arquivo, permissões e buffering de dados para operações de leitura e escrita.

Tais conhecimentos são fundamentais para qualquer estudo aprofundado de Sistemas Operacionais, dando base para entender como o SO viabiliza a comunicação entre o hardware e os aplicativos de usuário. Portanto, as chamadas aqui escolhidas cobrem um escopo mínimo indispensável para se ter uma visão geral e prática das principais funcionalidades que o kernel oferece ao espaço de usuário.

## 2.2 Código comentado

### 2.2.1 memoria_mmap.c

1. Bibliotecas

* <stdio.h>: fornece funções de entrada e saída em nível de usuário, como printf.
* <stdlib.h>: inclui funções utilitárias de uso geral (e.g., perror, exit).
* <sys/mman.h>: contém as declarações para as funções de mapeamento de memória (mmap e munmap), que são chamadas de sistema do Linux.
* <string.h>: oferece funções para manipulação de strings, como strcpy.
* <unistd.h>: inclui protótipos de chamadas de sistema Unix/Linux, como close, getpid, entre outras.

Do ponto de vista de Sistemas Operacionais, os cabeçalhos <sys/mman.h> e <unistd.h> são fundamentais para interagir diretamente com serviços de gerenciamento de memória e funções oferecidas pelo kernel.

---

int main() {

    size_t size = 4096;  // Tamanho da região a mapear

2. Função principal e definição de tamanho
   * A função main dá início ao programa.
   * A variável size define a quantidade de memória (em bytes) que desejamos mapear. Aqui foi escolhido 4096 bytes (4 KB), o tamanho de uma página de memória em muitos sistemas, embora isso possa variar.

Esse ponto conecta-se aos conceitos de Sistemas Operacionais relacionados a páginas de memória: 4 KB é frequentemente o tamanho de página padrão em arquiteturas x86, por exemplo.

---

   // Mapeia memória anônima com permissão de leitura e escrita

    char *map = mmap(NULL, size, PROT_READ | PROT_WRITE,

    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

3. Chamando a função mmap
   * mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0):
     * NULL: O kernel escolhe onde mapear a memória.
     * size: quantidade de bytes (4 KB).
     * PROT_READ | PROT_WRITE: define que a região terá permissões de leitura e escrita.
     * MAP_PRIVATE: mudanças na memória não são compartilhadas com outros processos (cópia em escrita).
     * MAP_ANONYMOUS: indica que não há arquivo associado ao mapeamento, criando uma área de memória “virgem”.
     * Os últimos parâmetros (arquivo = -1, offset = 0) são usados quando MAP_ANONYMOUS está presente, pois não há arquivo para vincular.

O mmap é uma chamada de sistema essencial em Sistemas Operacionais para alocação de memória fora das funções tradicionais de biblioteca (malloc). Ele permite controlar com mais precisão as permissões e a origem dos dados (arquivo ou memória anônima).

---

   if (map == MAP_FAILED) {

    perror("mmap");

    return 1;

    }

4. Verificação de erro
   * Se map retornar MAP_FAILED, significa que o kernel não conseguiu satisfazer o pedido de alocação.
   * Neste caso, imprime-se uma mensagem de erro (perror) e o programa é encerrado com código de erro (1).

Em um Sistema Operacional, pedidos de recursos (como memória) podem falhar por falta de espaço ou restrições do processo. É crucial tratar essa condição.

---

// Escreve algo na região mapeada

    strcpy(map, "Hello from mmap!\n");

5. Manipulação de memória mapeada
   * Como definimos PROT_READ | PROT_WRITE, podemos escrever diretamente na área apontada por map.
   * strcpy copia a string "Hello from mmap!\n" para a memória recém-alocada.

Aqui, exploramos um conceito de gestão de memória: embora não usemos malloc, temos acesso a um espaço de endereçamento pronto para uso. O kernel configura as páginas para permitir escrita segura pelo processo.

---

   // Exibe o conteúdo

    printf("Conteúdo mapeado: %s", map);

6. Leitura e impressão dos dados
   * Demonstra que o conteúdo escrito foi armazenado e pode ser lido normalmente.
   * printf é uma função em nível de usuário que exibirá a string contida no endereço apontado por map.

Nesse ponto, vemos a integração entre espaço de usuário e memória do processo: qualquer função C tradicional pode acessar aquele endereço, pois ele está mapeado e permitido pelo kernel.

---

   // Desfaz o mapeamento

    if (munmap(map, size) == -1) {

    perror("munmap");

    return 1;

    }

7. Chamada munmap
   * Libera a área de memória mapeada, informando ao kernel que o processo não precisa mais dessas páginas específicas.
   * Se falhar, imprime-se mensagem de erro.

Em termos de Sistemas Operacionais, esse passo retorna os recursos de memória ao gerenciador do SO. É análogo a free (usado após malloc), mas aqui é feito explicitamente com a chamada de sistema correspondente.

---

   return 0;

}

8. Finalização
   * Indica que o programa terminou com sucesso.
   * Ao sair, o sistema operacional também liberaria qualquer recurso associado ao processo, mas chamar munmap manualmente é considerado boa prática e demonstra controle explícito sobre a memória.

---

#### 2.2.1.1 Relação com Sistemas Operacionais

* Mapeamento de Memória (mmap/munmap): É um dos mecanismos centrais de gestão de memória em SOs Unix-like, permitindo que processos façam requisições diretamente ao kernel para reservar (ou liberar) áreas com permissões específicas.
* Permissões de Acesso (PROT_READ/PROT_WRITE): Remetem ao modelo de proteção de páginas, fundamental para evitar acesso indevido (violação de memória).
* Espaço de Endereçamento do Processo: O kernel cria e gerencia regiões separadas (página de código, página de dados, stack, heap), e mmap permite estender e manipular essas regiões.
* Retorno e Tratamento de Erro: Representa como o SO informa ao programa quando uma ação não pode ser realizada.

#### 2.2.1.2 Execução

O código levou menos de 1 segundo para ser executado

![](https://lh7-rt.googleusercontent.com/docsz/AD_4nXcl7B9qWQWHmqJhheUoHQx0mLMYQv85-x7HRUacqcYY-dMUb7kcGnuVBnZthyEqo2AUsdIAnlHRoZCXZ_nZCZGC924W0O3HH10A5_q5Mg_Cc06C1_qMu-wpZJe7_mQ3eLv27O0DMg?key=_Tx7RGyrDEWzTgFvdywY82LZ)

### 2.2.2 memoria_brk.c

Este programa demonstra como a chamada de sistema sbrk permite ajustar o “program break” de um processo, que é o limite superior (ou final) da área de dados/heap do processo. Em outras palavras, o program break é o endereço lógico que marca até onde a aplicação pode usar memória além das variáveis estáticas e antes de chegar à stack (em arquiteturas tradicionais). A função sbrk oferece acesso de baixo nível para expandir ou reduzir a região de heap.

1. Inclusão de bibliotecas

   * <stdio.h>: para operações de E/S em nível de usuário (como printf).
   * <unistd.h>: contém o protótipo da função sbrk, dentre outras chamadas de sistema do Unix/Linux.
2. sbrk(0)

* Quando se passa 0 como argumento para sbrk, ela não altera o break, apenas retorna o endereço atual do limite da área de dados do processo.
* Assim, initial_break guarda o valor do program break no momento em que o programa inicia.

1. Exibição do program break inicial

* Serve para fins de comparação, mostrando ao usuário a posição (endereço de memória) onde começa a área de “expansão” da heap do processo.

---

   // Aumenta o program break em 1024 bytes

    if (sbrk(1024) == (void *)-1) {

    perror("sbrk");

    return 1;

    }

    void *new_break = sbrk(0);

    printf("Program break após alocar 1KB: %p\n", new_break);

4. Expansão do heap

   * A chamada sbrk(1024) solicita ao kernel que aumente em 1024 bytes o espaço disponível para o processo na heap.
   * Caso sbrk retorne (void *) -1, significa que o pedido não pôde ser atendido (por exemplo, falta de memória ou outra restrição).
5. Obter e exibir o novo program break

* Chama-se sbrk(0) novamente para ver onde o break ficou depois da expansão.
* Se houver sucesso, esse endereço deve ser 1024 bytes acima do inicial.

---

   // Diminui novamente

    if (sbrk(-1024) == (void *)-1) {

    perror("sbrk");

    return 1;

    }

    void *final_break = sbrk(0);

    printf("Program break final após liberar 1KB: %p\n", final_break);

6. Redução do heap

   * sbrk(-1024) pede ao kernel que reduza o program break em 1024 bytes, voltando à posição anterior.
   * Novamente, verifica-se o retorno para tratar eventuais erros.
7. Exibição final

* final_break mostra que o break retornou ao valor inicial (ou muito próximo, dependendo de como a libc gerencia internamente essa região).

---

   return 0;

}

8. Finalização do programa
   * O programa termina, liberando todos os recursos de memória para o sistema operacional.
   * Embora sbrk tenha sido usado para manipular diretamente a região de dados, ao encerrar, todo o espaço alocado é devolvido ao SO.

---

#### 2.2.2.1 Relação com Sistemas Operacionais

* Gerenciamento de Memória:
  O uso de sbrk permite entender como o heap pode crescer e diminuir “manualmente”. Geralmente, as funções de biblioteca em C (malloc, free) chamam internamente sbrk (ou mmap) para requisitar memória. Apesar de sbrk ser considerado de uso mais “baixo nível” e menos seguro hoje em dia, é um ótimo exemplo para estudo de como o kernel gerencia a região de dados de um processo e como as chamadas de sistema expandem ou retraem esse espaço.
* Alocação de Recursos:
  Quando o program break é aumentado, o sistema operacional ajusta as tabelas de páginas para que o processo tenha acesso a mais memória contígua na sua área de dados. Diminuir o break faz o inverso, liberando aquelas páginas de volta para o sistema.
* Arquiteturas de Memória:
  Em muitos sistemas, o espaço de endereçamento de um processo é dividido em segmentos: text segment (código), data segment (variáveis estáticas e heap), stack segment (pilha), etc. O sbrk atua sobre o final da área de dados para expandir/reduzir a região destinada ao heap

#### 2.2.2.2 Execução

A execução demorou menos que 1 segundo

![img](https://lh7-rt.googleusercontent.com/docsz/AD_4nXeq91SnqWwy-H6-8hPTqqXFey3Kh5Gj7SI5z3imSH3tVFqscXwwe9mUDUsJBjSHbr9rPqreApUH0kBsBqGDOSZmVxoC5DsPI2viU2fnmJF-zWwTey46RNBRk_svSyyWCVuoUsSV?key=_Tx7RGyrDEWzTgFvdywY82LZ)

### **2.2.3 memoria_mprotect.c**

Este código em C demonstra como mapear memória anônima usando mmap, escrever nessa região e, em seguida, alterar as permissões de acesso na mesma área por meio de mprotect. Por fim, ele mostra como desfazer o mapeamento com munmap. A seguir, cada parte do script é analisada, relacionando seus aspectos técnicos a conceitos de Sistemas Operacionais.

---

#include <stdio.h>

#include <stdlib.h>

#include <sys/mman.h>  // mmap, mprotect, munmap

#include <string.h>    // strcpy

#include <unistd.h>    // getpid

1. Inclusão de bibliotecas
   * <stdio.h> e <stdlib.h>: para funções de entrada e saída (como printf) e utilitários gerais (como exit ou perror).
   * <sys/mman.h>: contém declarações das funções mmap, mprotect e munmap, que são chamadas de sistema para manipulação de memória em sistemas Unix/Linux.
   * <string.h>: necessário para a função strcpy, que copia strings.
   * <unistd.h>: fornece diversas funções do sistema Unix (como getpid), mas não é amplamente usado aqui, além do possível uso de close, etc.

Em Sistemas Operacionais, essas bibliotecas e chamadas de sistema permitem que o processo solicite recursos de memória ao kernel e controle as permissões de leitura, escrita e execução.

---

int main() {

    size_t size = 4096;

2. Tamanho da região de memória
   * A variável size define a quantidade de memória (em bytes) que se pretende mapear. Em muitos sistemas, 4096 bytes equivalem ao tamanho de uma página de memória.

A ideia de páginas é central no gerenciamento de memória, pois o sistema operacional trabalha em unidades de página ao atribuir áreas ao espaço de endereçamento de cada processo.

---

   // Mapeia memória anônima com permissão de leitura e escrita

    char *addr = mmap(NULL, size, PROT_READ | PROT_WRITE,

    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (addr == MAP_FAILED) {

    perror("mmap");

    return 1;

    }

3. Chamada mmap

   * mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0):
     * NULL: o kernel escolherá o endereço onde essa região será mapeada.
     * size: quantidade de bytes solicitados (4 KB aqui).
     * PROT_READ | PROT_WRITE: a memória pode ser lida e escrita.
     * MAP_PRIVATE: alterações nessa área não serão compartilhadas com outros processos; cada processo tem sua cópia privada caso existisse um arquivo associado.
     * MAP_ANONYMOUS: não há arquivo real associado, criando-se um mapeamento “virgem” de memória.
     * (arquivo = -1, offset = 0): parâmetros obrigatórios, mas sem uso prático quando MAP_ANONYMOUS está definido.
4. Verificação de erro

* Caso retorne MAP_FAILED, algo impediu a alocação (por exemplo, falta de memória, permissões inadequadas).
* perror("mmap") exibe a causa do erro, e o programa finaliza com código 1.

---

   // Escreve algo na região mapeada

    strcpy(addr, "Texto inicial na memória.\n");

    printf("Antes do mprotect: %s", addr);

5. Utilizando a memória mapeada
   * A string "Texto inicial na memória.\n" é copiada para o endereço apontado por addr. Graças a PROT_READ | PROT_WRITE, essa ação é permitida.
   * A seguir, imprime-se o conteúdo armazenado para confirmar que a escrita funcionou.

Esse passo confirma que a área está acessível para leitura e escrita, bem como o fluxo típico de espaço de usuário (o programa) interagindo com sua área de memória.

---

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

6. Chamada mprotect

   * mprotect(addr, size, PROT_READ) altera as permissões da região de memória para somente leitura.
   * Se falhar, imprime erro e libera a memória (munmap), pois não faz sentido manter o mapeamento inconsistente.
7. Efeito da alteração de permissões

* Após essa chamada, a memória deixa de poder ser escrita. Tentativas de escrita podem gerar uma falha de segmentação (segfault).
* O comentário que sugere addr[0] = 'X'; demonstra exatamente como essa operação de escrita agora é ilegal e normalmente resultaria num erro de memória.

Em termos de Sistemas Operacionais, páginas de memória podem ser marcadas como executáveis, somente leitura ou leitura-escrita. Isso é essencial para segurança (por exemplo, prevenindo execução de código em regiões de dados) e para organizar o acesso a dados compartilhados.

---

   // Desfaz o mapeamento

    if (munmap(addr, size) == -1) {

    perror("munmap");

    return 1;

    }

    return 0;

}

8. Chamada munmap

   * Com munmap(addr, size), o processo informa ao kernel que não precisa mais daquela região de memória.
   * Trata-se da contrapartida de mmap: libera as páginas e remove o mapeamento do espaço de endereçamento do processo.
9. Finalização

* O programa retorna 0, indicando término bem-sucedido.
* Caso não chamasse munmap, o SO ainda recuperaria a memória ao encerrar o processo, mas a chamada explícita é boa prática, principalmente em programas mais complexos de longa duração.

---

#### 2.2.3.1 Relação com Sistemas Operacionais

1. mmap e munmap: Chamadas centrais em Unix/Linux para gerenciar regiões de memória no espaço de usuário.
2. mprotect: Permite modificar dinamicamente permissões (read, write, execute) em uma área de memória mapeada, ilustrando o modelo de proteção de páginas do SO.
3. Permissões: A capacidade de alternar entre leitura e escrita e somente leitura demonstra como o kernel usa tabelas de página para controlar acesso a cada região de memória de um processo, um pilar fundamental da segurança e estabilidade do sistema.
4. Segfault: Tentar escrever em uma área protegida com PROT_READ resulta em falha de segmentação, reforçando o conceito de isolamento e integridade de memória que o SO mantém.

#### 2.2.3.2 Execução

Execução demorou menos de 1 segundo

![](https://lh7-rt.googleusercontent.com/docsz/AD_4nXdKi5zz8LtSQr1KBRveDkwqtreMEeySo3gjekIhgOULG-wkgTRaHcPNrlbsGrtcZYP0w53gbJCf9PPmYilNmdIm45HrGMS95AT8xmFNFW2QSjpzpmitov9VHeHnJdt5xJJ4MQqB1w?key=_Tx7RGyrDEWzTgFvdywY82LZ)

### 2.2.4 processo_fork_exec_wait.c

Este código ilustra como um processo Unix/Linux pode criar um processo filho por meio da chamada fork, substituir o conteúdo da memória desse processo filho usando execve e, por fim, aguardar sua finalização com wait. Cada etapa demonstra conceitos fundamentais de Sistemas Operacionais relacionados a processos e sincronização.

---

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>   // fork, execve

#include <sys/wait.h> // wait

1. Inclusão de bibliotecas
   * <stdio.h> e <stdlib.h>: fornecem funções como printf, perror, exit, etc.
   * <unistd.h>: contém protótipos de funções como fork e execve. Estas são chamadas de sistema (system calls) importantes para criação e execução de novos processos.
   * <sys/wait.h>: declara a função wait (e macros auxiliares), usada para sincronizar a finalização de um processo filho.

No contexto de Sistemas Operacionais, fork, execve e wait são mecanismos essenciais para criar, substituir a imagem de um processo e sincronizar execução entre processos.

---

int main() {

    pid_t pid = fork();

2. Chamada fork
   * fork()duplica o processo em execução, criando dois fluxos: o processo pai (que recebe o PID do filho) e o processo filho (que recebe 0 como valor de retorno).
   * O tipopid_t é usado para armazenar o valor de retorno de fork.

---

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

Bloco Condicional

3. Tratamento de erro (pid < 0)

   * Se o fork falhar, pid será negativo. Nesse caso, uma mensagem de erro é exibida e o programa encerra com código de erro (1).
4. Processo Filho (pid == 0)

* Quando o retorno de fork é 0, significa que estamos no contexto do processo filho.
* Usa-se printf("Filho (PID=%d)...", getpid()); para exibir o PID (identificador de processo) do filho.
* Construção do vetor args: char *args[] = {"/bin/ls", "-l", NULL}; especifica o executável a ser chamado (/bin/ls) e o parâmetro -l. O vetor termina em NULL para indicar fim da lista.
* Chamada execve: substitui a “imagem” do processo filho (o código, dados, seções de memória) pelo programa /bin/ls. Após execve, o fluxo de execução do filho será o que está no binário do comando ls.
* Se execve falhar: a execução continua no próximo comando, imprimindo perror("execve") e chamando exit(1).

Essa sequência (fork seguido de exec) é um padrão clássico para criar um novo processo que executa um programa diferente. Em termos de Sistemas Operacionais, a memória do filho é totalmente substituída pelo binário chamado, mas mantém o mesmo PID.

5. Processo Pai (quando pid > 0)
   * O processo pai continua sua execução após o fork.
   * wait(&status) faz o pai aguardar até que o filho termine, bloqueando até a conclusão do comando ls.
   * Depois que o filho finaliza, wait retorna, e o pai pode exibir Pai (PID=%d), filho terminou..
   * As macros WIFEXITED(status) e WEXITSTATUS(status) permitem checar se o filho terminou normalmente e obter seu código de saída.

No contexto de Sistemas Operacionais, wait representa a sincronização entre processos e permite que o pai recupere informações sobre como o filho terminou.

6. Retorno final
   * return 0; encerra o programa pai com sucesso depois que o filho já foi finalizado.

---

#### 2.2.4.1 Relação com Sistemas Operacionais

1. Processos: Em Unix/Linux, cada instância de programa em execução é um processo. Aqui, fork cria um processo filho que compartilha quase todo o estado inicial com o pai, mas passa a ter fluxo de execução independente.
2. Execução de Programas: execve é a base para executar novos programas. Ele carrega um binário do disco e substitui o conteúdo do processo chamador, mantendo apenas o PID, recursos abertos (se especificado) e variáveis de ambiente (caso sejam passadas).
3. Sincronização: wait força o processo pai a esperar que o filho termine, garantindo que o pai possa obter o código de saída e liberar recursos do SO associados ao filho (tabela de processos, etc.).
4. Comunicação com o Kernel: Tanto fork quanto execve quanto wait são chamadas de sistema, ativando o código do kernel para gerenciar processos, atualizar tabelas internas e garantir o correto compartilhamento de CPU e memória.

#### 2.2.4.2 Execução

A execução demorou menos de 1 segundo

![](https://lh7-rt.googleusercontent.com/docsz/AD_4nXdqIsNVx-0rxjooiIXtTaAMnWgbXNk4TcEn3IRvagskNvybCwoUOsULWb1LtyYLPzC6bzDjoXLxBo4wBbLjo8me2-zB70OkDNAPdRz0wj_8RkO48it6OK_ccs7mN-4HLPiFY4-b?key=_Tx7RGyrDEWzTgFvdywY82LZ)

### 2.2.5 io_open_read_write.c

Este programa em C demonstra operações básicas de entrada/saída (E/S) em arquivos usando as chamadas de sistema (system calls) do Unix/Linux: open, write, read, lseek e close. Tais chamadas são oferecidas pela API de baixo nível do sistema, em contraste com funções de alto nível como fopen, fprintf e fclose. A seguir, abordamos cada parte do código relacionando seu propósito aos conceitos de Sistemas Operacionais.

---

#include <stdio.h>

#include <stdlib.h>

#include <fcntl.h>   // open

#include <unistd.h>  // read, write, close

#include <string.h>  // strlen

1. Inclusão de Bibliotecas
   * <stdio.h> e <stdlib.h>: para impressão em tela (printf, perror) e utilidades gerais (exit, etc.).
   * <fcntl.h>: declara a função open e constantes usadas no controle de arquivos (e.g., flags como O_RDWR, O_CREAT).
   * <unistd.h>: contém protótipos para read, write, close, lseek, além de outras chamadas de sistema.
   * <string.h>: usado para a função strlen, necessária na escrita de strings.

No contexto de Sistemas Operacionais, essas chamadas de sistema correspondem a interações diretas com o subsistema de arquivos do kernel, controlando como o processo acessa e manipula dados armazenados em disco ou outros dispositivos.

---

int main() {

    const char *filename = "example.txt";

2. Nome do Arquivo
   * Define-se example.txt como arquivo para teste, seja para criação ou abertura existente.

---

   // Abre (ou cria) um arquivo para leitura/escrita

    int fd = open(filename, O_RDWR | O_CREAT, 0644);

    if (fd == -1) {

    perror("open");

    return 1;

    }

3. Chamada de sistema open
   * open(filename, O_RDWR | O_CREAT, 0644): tenta abrir ou criar (O_CREAT) o arquivo example.txt, fornecendo permissões de leitura e escrita (O_RDWR).
   * A permissão 0644 define as permissões de acesso do arquivo no estilo Unix (dono lê/escreve, demais usuários apenas leem). Se o arquivo já existir, essas permissões não necessariamente sobrescrevem as atuais, mas são usadas em caso de criação.
   * Se a chamada falhar, retorna -1; o programa então imprime erro e finaliza.

Em Sistemas Operacionais, cada chamada open retorna um descritor de arquivo (file descriptor), nesse caso armazenado em fd. Esse descritor é um inteiro que representa o canal de comunicação com o arquivo no espaço de kernel.

---

   // Escreve algo no arquivo

    const char *text = "Hello, file!\n";

    ssize_t written = write(fd, text, strlen(text));

    if (written == -1) {

    perror("write");

    close(fd);

    return 1;

    }

4. Chamada de sistema write
   * Envia dados para o arquivo associado ao descritor fd.
   * text contém "Hello, file!\n", e strlen(text) retorna o tamanho em bytes.
   * O valor de retorno de write indica quantos bytes foram efetivamente escritos; se for -1, houve erro.

Para o SO, a função write copia os dados da memória do processo para um buffer no kernel e, em seguida, o kernel providencia (por meio de drivers) a gravação no dispositivo físico.

---

   // Volta o offset para o início do arquivo (SEEK_SET = 0)

    off_t offset = lseek(fd, 0, SEEK_SET);

    if (offset == (off_t)-1) {

    perror("lseek");

    close(fd);

    return 1;

    }

5. Reposicionamento de Offset (lseek)
   * lseek(fd, 0, SEEK_SET) move o “cursor” interno do arquivo para a posição zero (início do arquivo).
   * Permite controlar onde as próximas leituras/escritas ocorrerão.
   * Se lseek falhar, retorna -1.

Dentro do kernel, cada descritor de arquivo mantém um offset que aponta para a posição atual de leitura/escrita. Com lseek, o processo atualiza esse offset sem precisar reabrir o arquivo.

---

   // Lê o conteúdo do arquivo

    char buffer[128];

    ssize_t read_bytes = read(fd, buffer, sizeof(buffer) - 1);

    if (read_bytes == -1) {

    perror("read");

    close(fd);

    return 1;

    }

    buffer[read_bytes] = '\0';

6. Chamada de sistema read
   * read(fd, buffer, sizeof(buffer) - 1) lê no máximo sizeof(buffer) - 1 bytes do arquivo e os armazena em buffer.
   * Retorna a quantidade de bytes lidos ou -1 em caso de erro.
   * Após a leitura, coloca-se um '\0' no final para tratar o conteúdo como string C.

O kernel transfere dados do arquivo para o espaço de usuário do processo, atualizando o offset interno. Isso demonstra a separação entre buffers do kernel e buffers da aplicação.

---

   printf("Conteúdo lido do arquivo: %s\n", buffer);

7. Impressão do conteúdo
   * Verifica-se se a escrita anterior foi bem-sucedida e se o arquivo contém a mensagem esperada.

---

   // Fecha o arquivo

    if (close(fd) == -1) {

    perror("close");

    return 1;

    }

    return 0;

}

8. Chamada de sistema close

   * Libera o descritor de arquivo, sinalizando ao kernel que o processo não precisa mais do arquivo aberto.
   * Importante para bom gerenciamento de recursos (limite de descritores e buffers) e para garantir que todos os dados sejam corretamente gravados no dispositivo.
9. Retorno final

* O programa encerra com return 0, indicando sucesso.

---

#### 2.2.5.1 Relação com Sistemas Operacionais

1. File Descriptors: Cada arquivo aberto por um processo recebe um inteiro que o identifica no kernel. Essa abstração unifica o acesso a diferentes dispositivos (discos, pipes, sockets, etc.) no Unix/Linux.
2. Chamada de Sistema: Funções como open, write, lseek, read e close são implementadas no kernel. Aqui, a linguagem C é apenas uma camada de interface. Cada chamada transiciona do modo usuário para o modo kernel para efetivar a operação.
3. Buffering e Sincronização: O kernel gerencia buffers que armazenam dados vindos ou destinados ao disco, melhorando o desempenho do sistema e controlando simultaneamente a escrita/leitura por múltiplos processos.
4. Permissões e Proteção: A flag 0644 e a presença de O_CREAT mostram como o SO controla a criação de arquivos e o acesso, evitando escritas e leituras não autorizadas.

#### 2.2.5.2 Execução

A execução demorou menos de 1 segundo

![](https://lh7-rt.googleusercontent.com/docsz/AD_4nXfqaLqTLqJux-2fHQfJMxy-vusMJgQ541XehxA0Kt8poOm3in8KagXtjeU-bxp-6xJ2tNY3wSKQh-xlDhIaU0KsmRNG2lMKL5LtxVpXMxjU6WFCH1VQacbK3gTVw6gk_ytb62Ct?key=_Tx7RGyrDEWzTgFvdywY82LZ)

Criando o example.txt

![](https://lh7-rt.googleusercontent.com/docsz/AD_4nXfLSJMHzp59kJVr4MVmZ3y_eJyzj74n8hBoJ3M8yNOJO1Yf8zJDgEUFtpy37RuqrVReoZnDqIY5Y3bQA_7YLeQP9GgPIhsGxWwHgtAJDH-84xIi4eXtkUPtMswgVjtW0G4N-SbKkw?key=_Tx7RGyrDEWzTgFvdywY82LZ)

# 3. Parte 2 - CPU bound e I/O bound

## 3.1 Justificativa

Na segunda parte do projeto, o objetivo foi demonstrar como diferentes programas podem comportar-se de forma distinta em termos de uso de CPU e de I/O (entrada/saída). Para tanto, selecionamos dois scripts:

1. cpu_bound.c

   * Projetado para executar cálculos intensivos (geralmente sem muita operação de leitura/escrita), servindo como exemplo de processo CPU-bound.
   * Em resumo, é um programa que faz laços extensos ou cálculos matemáticos complexos, gastando praticamente todo o seu tempo de execução “esmagando” a CPU, sem ficar aguardando I/O de disco ou rede.
2. io_bound.c

* Focado em realizar muitas operações de entrada/saída em disco ou outra forma de I/O, tornando-o um processo I/O-bound.
* Nesse caso, boa parte do tempo de execução é gasta aguardando a conclusão das operações de escrita ou leitura, em vez de efetivamente executar instruções na CPU.

## 3.2 Características

* Diferenciação clara de comportamento:
  Esses dois tipos de programas ilustram de forma didática o que significa um processo ser limitado pelo tempo de processamento (CPU-bound) versus ser limitado pela velocidade de acesso a algum dispositivo de E/S (I/O-bound).
* Observação de métricas distintas:
  Executá-los com /usr/bin/time -v permite coletar informações como:

  * Tempo total (elapsed time)
  * Tempo de CPU em modo usuário e tempo em modo kernel
  * Quantidade de trocas de contexto voluntárias e involuntárias
    Essas métricas refletem diretamente o comportamento de cada tipo de processo. Um CPU-bound tende a ter um alto uso de CPU (com tempo total próximo ao tempo de usuário), enquanto o I/O-bound frequentemente apresenta períodos de ociosidade da CPU, evidenciados por mais trocas de contexto e menor percentual de uso da CPU em relação ao tempo total.
* Complexidade moderada:
  O conceito em si (CPU-bound vs. I/O-bound) é simples de entender, mas criar exemplos didáticos exige:
* Conhecimento de programação em C e compreensão de laços, cálculos, funções de E/S de arquivo, buffers etc.
* Entendimento básico de Sistemas Operacionais, especialmente sobre como processos se alternam entre execução e espera quando há operações de disco ou rede.
* Conhecimentos necessários:
* Processos e escalonamento: o aluno deve saber que o escalonador do SO dá fatias de CPU para cada processo e que, se um processo passa tempo aguardando E/S, a CPU pode ser usada por outros processos.
* Ferramentas de análise:
* strace: para verificar as chamadas de sistema e tempo nelas gasto.
* /usr/bin/time -v: para analisar o consumo de recursos (tempo total, tempo de CPU, trocas de contexto).
* Aplicabilidade didática:
  Esses dois scripts cobrem pontos fundamentais de como o kernel aloca tempo de CPU e gerencia bloqueios de I/O, sendo um exemplo clássico para explicar por que certos programas demoram mais pelo gargalo de CPU (cálculo pesado) ou de I/O (acesso a disco, rede).

Em síntese, a escolha do cpu_bound.c e io_bound.c se deve à necessidade de mostrar de forma concreta dois perfis de consumo de recursos — pesado em CPU e pesado em E/S — ajudando a evidenciar como o sistema operacional trabalha para atender diferentes demandas de processos.


## 3.3 Código comentado

### 3.3.1 cpu_bound.c

Este programa em C exemplifica um processo CPU-bound ao realizar um cálculo intensivo de números primos até um determinado limite. A principal ideia é que ele não dependa de operações de E/S significativas, mas sim de repetitivos loops de processamento que utilizam bastante o poder de cálculo da CPU. A seguir, cada trecho do código é analisado:

---

#include <stdio.h>

#include <stdlib.h>

1. Inclusão de bibliotecas
   * <stdio.h>: fornece funções de entrada e saída em nível de usuário, como printf.
   * <stdlib.h>: inclui funções auxiliares gerais (por exemplo, exit, malloc, etc.) – ainda que algumas não sejam usadas aqui, é comum que <stdlib.h> seja incluído por padrão para conveniência.

---

int main() {

    int limit = 500000; // Ajuste conforme desejar

    int count = 0;

2. Variáveis iniciais
   * limit define até qual número o programa fará a verificação de primos. O valor 500.000 pode ser aumentado ou reduzido para ajustar a duração do teste.
   * count será usado para somar quantos números primos são encontrados.

Este valor de limit configura quão “pesado” será o processamento. Em Sistemas Operacionais, aumentar esse número faz o programa demandar mais tempo de CPU, ilustrando claramente o comportamento de um processo CPU-bound.

---

   for (int n = 2; n < limit; n++) {

    int prime = 1;

    for (int i = 2; i * i <= n; i++) {

    if (n % i == 0) {

    prime = 0;

    break;

    }

    }

    if (prime) {

    count++;

    }

    }

3. Loop principal – Verificação de números primos
   * Loop externo: percorre todos os números n de 2 até limit - 1.
   * Variável prime: assume inicialmente valor 1, indicando que n pode ser primo.
   * Loop interno: verifica divisibilidade de n por todos os i de 2 até sqrt(n) (o trecho i * i <= n é uma forma de evitar checar até n-1).
     * Se n % i == 0, significa que n possui um divisor, logo não é primo (prime = 0) e interrompe-se o loop (break).
   * Contagem: se, ao final do loop interno, prime ainda for 1, incrementa-se count, pois n é um número primo.

Esse duplo loop representa uma carga de trabalho predominantemente computacional: muitos cálculos repetitivos são executados, e o programa não realiza entradas/saídas de disco ou rede relevantes. Em termos de Sistemas Operacionais, essa característica o torna ideal para demonstrar alta utilização de CPU, sem bloqueios de E/S.

---

   printf("Total de números primos encontrados abaixo de %d: %d\n", limit, count);

    return 0;

}

4. Exibição do resultado
   * Após o término dos loops, imprime-se o total de números primos.
   * return 0 indica finalização bem-sucedida do programa.

Exibir o resultado é praticamente a única operação de E/S significativa aqui, sendo mínima comparada ao custo computacional dos laços que avaliam primalidade.

---

#### 3.3.1.1 Relação com Sistemas Operacionais

1. Processos CPU-bound

   * Um processo CPU-bound é aquele cujo tempo de execução é limitado pelo poder de processamento da CPU, em vez de depender de recursos de E/S.
   * Neste programa, não há chamadas de leitura ou escrita em arquivo, nem espera por dados de rede, de modo que o gargalo é a própria capacidade de cálculo do processador.
2. Métricas de desempenho

* Ferramentas como /usr/bin/time -v podem revelar o alto uso de CPU (tempo em modo de usuário), indicando poucas trocas de contexto e pouca espera por operações de E/S.
* Em testes em um SO Linux, o %CPU tende a ficar próximo de 100% durante a execução, uma característica clara de um programa CPU-bound.

1. Escalonamento e Prioridades

* Se existirem outros processos competindo pela CPU, o escalonador do SO decide quantos quantums de CPU cada processo recebe.
* Como este programa não faz I/O significativo, ele não cede voluntariamente a CPU; portanto, seu tempo de execução depende basicamente de quantos quantums lhe forem atribuídos e do desempenho do processador.


#### 3.3.1.2 Execução /usr/bin/time -v

![](https://lh7-rt.googleusercontent.com/docsz/AD_4nXf4PQElSra886sNYKHEUY44XjsTxtLzYJFq7yozuBvQJdyNaATzdaGBSYSv376PykUhauMgGD7jKBonN64-1wOjhUk_14vIfLMW9thlzthjx-IyVjSHCMtKnz9lnnzFQ9ZIkoWLUQ?key=_Tx7RGyrDEWzTgFvdywY82LZ)


### 3.3.2 io_bound.c

Este programa exemplifica um processo I/O-bound, pois passa boa parte do seu tempo executando operações de escrita em disco ao invés de cálculos intensivos em CPU. Em outras palavras, o gargalo principal aqui está na velocidade das operações de entrada/saída (I/O), o que leva o processo a esperar com frequência que as escritas sejam finalizadas. A seguir, analisamos cada parte do código:

---

#include <stdio.h>

#include <stdlib.h>

1. Inclusão de bibliotecas
   * <stdio.h>: fornece funções de E/S de alto nível, como fopen, fprintf, fclose e fflush.
   * <stdlib.h>: inclui funções de utilidades gerais (por exemplo, perror, exit) — útil para tratamento de erros e saída do programa.

---

int main() {

    FILE *fp = fopen("testio.txt", "w");

    if (!fp) {

    perror("fopen");

    return 1;

    }

2. Abertura de arquivo (fopen)
   * Tenta criar ou sobrescrever um arquivo chamado testio.txt com permissões de escrita ("w").
   * Caso fopen falhe (por falta de permissão ou espaço em disco, por exemplo), exibe mensagem de erro via perror("fopen") e encerra retornando código 1.

Internamente, fopen chama funções de sistema (como open) e controla buffers em espaço de usuário para E/S. Em Sistemas Operacionais, cada arquivo aberto gera um descritor de arquivo que o kernel gerencia, mas essa camada de biblioteca C facilita operações de alto nível.

---

   // Ajuste o número de linhas conforme desejar.

    // Quanto maior, mais tempo de escrita, mais I/O-bound fica a aplicação.

    int lines = 300000;

3. Configuração do volume de escrita
   * A variável lines determina o número total de linhas a escrever no arquivo.
   * Quanto maior o valor, mais operações de escrita são realizadas, aumentando o tempo total do programa e enfatizando o caráter I/O-bound (mais I/O, mais espera).

A escolha de 300000 (trezentas mil) escritas faz o programa gerar muitas chamadas de E/S. Em um ambiente de produção, valores ainda maiores tornam esse comportamento mais perceptível.

---

   for (int i = 0; i < lines; i++) {

    // Escreve no arquivo

    fprintf(fp, "Linha número %d\n", i);

    // Força flush a cada escrita, forçando E/S de disco frequente

    fflush(fp);

    }

4. Loop de escrita
   * Loop de i = 0 até lines - 1: para cada iteração, imprime a string "Linha número X\n" no arquivo via fprintf.
   * fflush(fp) imediatamente envia o conteúdo do buffer em memória para o disco. Sem fflush, a biblioteca C poderia agrupar várias escritas para otimizar desempenho, retardando a gravação efetiva.
   * Com fflush, cada linha escrita bloqueia o processo até que o buffer seja esvaziado, aumentando substancialmente o tempo de E/S e, portanto, realçando o fator I/O-bound.

Em Sistemas Operacionais, o flushing frequente torna o processo mais sujeito a bloqueios de E/S. A CPU passa a maior parte do tempo aguardando a escrita no dispositivo de armazenamento. Isso explica por que, ao medir com /usr/bin/time -v, o uso de CPU pode ser relativamente baixo em comparação ao tempo total transcorrido.

---

   fclose(fp);

    return 0;

}

5. Fechamento do arquivo e finalização
   * fclose(fp) grava qualquer dado ainda no buffer, libera o descritor de arquivo e indica ao kernel que a aplicação não precisa mais do recurso.
   * return 0 sinaliza a conclusão bem-sucedida do programa.

---

#### 3.3.2.1 Relação com Sistemas Operacionais

1. I/O-bound

   * O programa realiza muitas operações de escrita, cada uma seguida de fflush, de forma que a CPU fica frequentemente em espera (bloqueada) pela conclusão dessas escritas.
   * Em um sistema multitarefa, o processo seria removido do processador sempre que chamasse fflush (ou chegasse a outra chamada de sistema que envolva I/O), permitindo que a CPU atenda outros processos. Isso se reflete em voluntary context switches (trocas de contexto voluntárias) quando medido com /usr/bin/time -v.
2. Ferramentas de Medição

* Ao rodar /usr/bin/time -v ./io_bound, geralmente se observa um tempo total (elapsed time) alto em relação ao tempo de CPU (user + sys), pois a maior parte da execução é gasta na fila de operações de disco.
* Com strace -T, se pode ver chamadas de write ou fsync sendo feitas repetidamente, cada uma gerando um tempo adicional de bloqueio.

1. Buffering e Sincronização

* A biblioteca C realiza buffering para reduzir a frequência de acessos ao disco, mas o fflush faz o flush manual a cada linha, ilustrando como o comportamento de buffering influencia o tempo de execução.
* Em termos de SO, isso mostra como a estratégia de E/S afeta o desempenho e o consumo de recursos.


#### 3.3.2.2 Execução /usr/bin/time -v


![](https://lh7-rt.googleusercontent.com/docsz/AD_4nXfljaKcMUJur0bsg0oDndFe6wLtO2ym7_78TivIRguShdxI20d_bRA9lS1jiFuQCzmmRfZV4MKXXYnOV6XVl6yBh4gRANQcWnVSZqaVzJ37FVjgj6c-1adSIfWcO-5_XVO2HTn3eg?key=_Tx7RGyrDEWzTgFvdywY82LZ)
