# Como Compilar
Para compilar o projeto basta executar o comando `make` neste diretório. O binário gerado será salvo neste diretório com o nome de `ex3`.

# Como Executar
Para executar o projeto basta executar o binário gerado após a compilação. O binário gerado será salvo neste diretório com o nome de `ex3`. Para executar o binário basta executar o comando `./ex3` neste diretório.Você pode alterar o arquivo: "vetores.txt", a 1ª linha é o tamanho dos vetores, as duas linhas seguintes são os vetores, no final ele escreve o vetor resultante na última linha do arquivo.

# Bibliotecas Utilizadas
- <stdio.h>: Fornece funções básicas de entrada e saída, como printf e scanf.
- <stdlib.h>: Fornece funções para alocação de memória, controle de processos e conversões
- <fcntl.h>: Contém definições para manipulação de arquivos, como modos de abertura e permissões.
- <sys/shm.h>: Fornece funções e definições para criar e gerenciar memória compartilhada em sistemas UNIX.
- <sys/stat.h>: Contém definições para manipular informações de arquivos, como permissões e tipos.
- <sys/mman.h>: Fornece funções para mapear arquivos ou dispositivos na memória, como mmap e munmap.
- <unistd.h>: Define funções do sistema POSIX, como fork, exec e controle de arquivos.
- <string.h>: Oferece funções para manipulação de strings e memória, como strcpy e memcpy.

# Exemplo de Execução
```bash
$ make
gcc ex3.c -o ex3 -lrt
$ ./ex3
Número de processos:  2
Processo 0:
1 + 7 = 8 
2 + 8 = 10 
3 + 9 = 12 
Processo 1:
4 + 10 = 14 
5 + 11 = 16 
6 + 12 = 18
```