# Como Compilar
Para compilar os Exercícios basta executar o comando `make` neste diretório. Os binários gerados serão salvos neste diretório com os nomes de `ex1` e `ex2`.

# Como Executar
Para executar os Exercícios basta executar o programa `ex1` utilizando o comando `./ex1` e para executar o `ex2` utilize o comando `./ex2`

# Bibliotecas Utilizadas
- **stdio.h (Standard I/O)** - para funções de entrada e saída
- **stdlib.h (Standard Library)** - para funções de utilidade geral
- **pthread.h (POSIX threads)** - para funções de manipulação de threads
- **unistd.h (UNIX standard)** - Para funções de sistema.
- **"matriz.h"** - biblioteca cedida pelo professor para criação, preenchimento e leitura de matrizes

# Exemplo de Execução ex1
```bash
$ make
gcc ex1.c matriz.c -o ex1 && gcc ex2.c -o ex2
$ ./ex1
me diga o número de colunas da matriz!
$ 3
me diga o número de linhas da matriz!
$ 3
me diga o número de threads que deseja criar!
$ 10
 84  87  78 
 16  94  36 
 87  93  50 
Eu, a thread 4, estou realizando a soma da linha 0
Eu, a thread 4, estou realizando a soma da linha 1
Eu, a thread 4, estou realizando a soma da linha 2
Soma total: 625
OBSERVAÇÃO: recomendado utilizar um número gigantesco de colunas e linhas na matriz para poder ver as threads trabalhandos, pouco volume de dados faz uma thread x trabalhar sozinha
```

# Exemplo de Execução ex2
```bash
$ make
gcc ex1.c matriz.c -o ex1 && gcc ex2.c -o ex2
$ ./ex2
Digite o número de threads ping:
$ 10
Digite o número de threads pong:  
$ 10
Eu, a thread ping nr 0 vos digo: ping
Eu, a thread pong nr 1 vos digo: pong
Eu, a thread ping nr 3 vos digo: ping
Eu, a thread pong nr 0 vos digo: pong
Eu, a thread ping nr 0 vos digo: ping
Eu, a thread pong nr 7 vos digo: pong
Eu, a thread ping nr 2 vos digo: ping
...
...
...
OBSERVAÇÃO: No enunciado do exercício não fica explicito que as threads devem terminar com determinado número de execuções, então fica em loop eterno
```