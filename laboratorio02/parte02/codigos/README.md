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

## Exemplos de Execução

### ex1

Usuário inicializa o programa com o valor 3 de argumento, a execução do terminal travará pois o programa está esperando inputs do usuário para continuar, isto fazendo com que o programa fique aguardando o processo filho, sem que este encerre, possibilitando o uso do pstree em um terminal à parte.

```bash
./ex1 3
```

#### Exemplo de saída
usuário abre outro terminal de digita o comando pstree + pid do pai que está no terminal do programa executado e visualiza os processos filhos.
exemplo do comando pstree:
```bash
> pstree -c -p 4368 

ex1(4368)─┬─ex1(4369)─┬─ex1(4371)
          │           └─ex1(4373)
          └─ex1(4370)─┬─ex1(4372)
                      └─ex1(4374)
```
### ex2
Usuário inicializa o programa com o comando ls
```bash
./ex2 ls
```
#### Exemplo de saída
```bash
I am your father...
ex1  ex1.c  ex2  ex2.c  ex3  ex3.c  ex4  ex4.c  Makefile  README.md
The Force is strong with this one.
```

### ex3
Usuário inicializa o programa e insere 10 para o tamanho do vetor, 3 para o número de processos filhos e 4 para o número a ser procurado

```bash
./ex3 
informe o tamanho do vetor
10
me informe quantos processos filhos você quer para realizar a busca
3
me diga qual o valor que você quer que eu busque
3
```
#### Exemplo de saída
```bash
Eu, o 0º com o PID 3441 fui de 0 até 2 e não encontrei o número 4 :(
Eu, o 1º com o PID 3442 fui de 3 até 5 e não encontrei o número 4 :(
Eu, o 2º com o PID 3443 fui de 6 até 9 e não encontrei o número 4 :(
o vetor continha esses números:
66,50,59,33,89,88,27,15,6,91
O número 4 não foi encontrado no vetor
```
### ex4
Usuário incializa o programa e digita o comando ls no terminal
```bash
./ex4
```
#### Exemplo de saída
```bash
Digite um comando> ls
ex1  ex1.c  ex2  ex2.c  ex3  ex3.c  ex4  ex4.c  Makefile
Processo 2837 executado com sucesso.
```