# Como Compilar
Para compilar o projeto basta executar o comando `make` neste diretório. O binário gerado será salvo neste diretório com o nome de `ex4`.

# Como Executar
Para executar o projeto basta executar o binário gerado após a compilação. O binário gerado será salvo neste diretório com o nome de `ex4`. Para executar o binário basta executar o comando `./ex4` neste diretório.

# Bibliotecas Utilizadas
- **sys/shm.h (Shared Memory)** - Para compartilhamento de memória entre processos.
- **sys/wait.h (Wait)** - Para esperar a finalização dos processos filhos.
- **sys/mman.h (Memory Map)** - Para mapear a memória compartilhada.
- **fcntl.h (File Control)** - Para abrir o arquivo de entrada.
- **unistd.h (Standard Symbolic Constants and Types)** - Para utilizar a função fork e criar um pipe.
- **cstring (String)** - Para manipulação de strings.
- **cerrno (Error)** - Para tratamento de erros.
- **cstdlib (Standard General Utilities Library)** - Para utilização da função exit e rand.

# Exemplo de Execução
```bash
$ make
g++    -c -o ex4.o ex4.cpp
g++ -Wall -Wextra -Werror -std=c++11 -o ex4 ex4.o
$ ./ex4
Informe o número de elementos do vetor: 20
Informe o número de processos: 4

Vetores:
Vetor 1: [87, 64, 14, 31, 3, 41, 74, 98, 57, 21, 46, 92, 0, 93, 76, 25, 41, 19, 99, 47]
Vetor 2: [99, 92, 44, 25, 41, 93, 83, 13, 14, 85, 19, 52, 33, 2, 77, 28, 55, 56, 16, 68]
Resultado da soma dos vetores:
Resultado: [186, 156, 58, 56, 44, 134, 157, 111, 71, 106, 65, 144, 33, 95, 153, 53, 96, 75, 115, 115]
```