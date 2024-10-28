# Exercícios de programação do laboratório 2 de Sistemas Operacionais
O intuito deste README é instruir o usuário no processo de compilação e execução dos programas feitos em C (`ex1`, `ex2`, `ex3` e `ex4`), além de apresentar as biblioeca utilizadas.

## Bibliotecas utilizadas
- `unistd.h`: usado para obter as funções como `fork()`, `getpid`, `getppid`e `sysconf`;
- `sys/types.h`: usado para obter o tipo `pid_t`;
- `sys/wait.h`: usado para obter a função `wait`;
- `math.h`: usado para obter acesso à funções matemáticas;
- `time.h`: usado para obter acesso à funções de tempo;
- `stdlib.h`: usado para obter acesso à funções de alocação de memória;
- `stdbool.h`: usado para obter acesso à tipo `bool`;
- `string.h`: usado para obter acesso à funções de manipulação de strings;

## Compilação
Para compilar os programas, voce pode executar os seguintes comandos:
```bash
make
```
que irá compilar todos os programas de uma vez, ou
```bash
make compile1
```
para compilar o programa `ex1`, `make compile2` para compilar o programa `ex2`, `make compile3` para compilar o programa `ex3` e `make compile4` para compilar o programa `ex4`.

Caso você não esteja conseguindo usar o make, pode ser que ele não esteja instalado em sua máquina, então certifique-se de instalar o make.

## Execução
Cada programa tem um propósito diferente, então a execução de cada um deles pode diferir. Abaixo, segue a instrução de execução de cada um dos programas.

### ex1
Para executar o programa `ex1`, você deve executar o seguinte comando:
```bash
./ex1 <n>
```
Onde `<n>` é um número inteiro que representa a quantidade de processos filhos que serão criados. O programa irá criar `<n>` processos filhos, e cada um deles irá imprimir seu PID e o PID de seu pai. É possível ver a árvore de processos abrindo um terminal à parte e executando o comando `pstree <pid> -p`, onde `<pid>` é o PID do processo pai, apresentado na execução do programa como:
```bash
Processo raiz (pid: `<pid>`).
```

### ex2
Para executar o programa `ex2`, você deve executar o seguinte comando:
```bash
./ex2 <comando> <arg1> <arg2> ... <argn>
```
Onde `<comando>` é o comando que será executado pelo processo filho, e `<arg1> <arg2> ... <argn>` são os argumentos que serão passados para o comando. O programa irá criar um processo filho que irá executar o comando passado como argumento.

### ex3
Para executar o programa `ex3`, você deve executar o seguinte comando:
```bash
./ex3
```
O programa irá pedir ao usuário que digite um número inteiro que será utilizado para dar tamanho a um vetor que será criado com valores aleatórios, pedir ao usuário o número de processos filhos que serão criados para fazer a busca no vetor, e pedir ao usuário o valor que será buscado no vetor. O programa irá criar um vetor de tamanho `<n>` com valores aleatórios, e `<n>` processos filhos que irão buscar o valor passado pelo usuário no vetor.

### ex4
Para executar o programa `ex4`, você deve executar o seguinte comando:
```bash
./ex4
```
Ele simplesmente vai ser um programa que vai executar similar a um shell, onde o usuário pode digitar comandos e ele será executado. Para sair do programa, basta digitar `exit`.