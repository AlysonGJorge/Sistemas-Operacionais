# Exercícios de programação do laboratório 3 de Sistemas Operacionais
O intuito deste README é instruir o usuário no processo de compilação e execução dos programas feitos em C (`ex1` e `ex2`), além de apresentar as biblioeca utilizadas.

## Bibliotecas utilizadas
- `pthread.h`: usado para obter acesso às funções para criação e gerencia de threads
- `math.h`: usado para obter acesso à funções matemáticas;
- `time.h`: usado para obter acesso à funções de tempo;
- `stdlib.h`: usado para obter acesso à funções de alocação de memória;
- `stdbool.h`: usado para obter acesso à tipo `bool`;
- `string.h`: usado para obter acesso à funções de manipulação de strings;
- `matriz.h`: usado para obter os códigos disponibilizados pelo professor;

## Compilação
Para compilar os programas, voce pode executar os seguintes comandos:
```bash
make
```
que irá compilar todos os programas de uma vez, ou
```bash
make compileEx1
```
para compilar o programa `ex1`, `make compileEx2` para compilar o programa `ex2`.

Caso você não esteja conseguindo usar o make, pode ser que ele não esteja instalado em sua máquina, então certifique-se de instalar o make.

## Execução
Cada programa tem um propósito diferente, então a execução de cada um deles pode diferir. Abaixo, segue a instrução de execução de cada um dos programas.

### ex1
Para executar o programa `ex1`, você deve executar o seguinte comando:
```bash
./ex1
```

### ex2
Para executar o programa `ex2`, você deve executar o seguinte comando:
```bash
./ex2 <arquivo saida> <matrizEntrada> ou  ./ex2 <arquivo saida>
```
Onde `<arquivo saida>` é o argumento que determinará qual será o nome do arquivo de resposta e `<matrizEntrada>` é o argumento responsável por determinar uma matriz já existente a ser lida, este argumento deve ser um arquivo já existente

AVISO IMPORTANTE: Colocar à extensão do arquivo de entrada, por exemplo: `data_matriz_100_200.in`

## Exemplos de Execução

### ex1
Usuário incializa o programa utilizando o comando:
```bash
./ex1
```

#### Exemplo de saída
usuário escolhe o tamanho do vetor, o número de threads que está limitado ao tamanho do vetor e um valor de a ser buscado:

```bash
informe o tamanho do vetor
10
8 7 93 69 36 79 79 82 47 3 
Me diga quantas threads deseja criar, lembrando que o máximo é o tamanho do vetor! que no caso é: 10
3
me diga qual o valor que você quer que eu busque
8
eu sou a thread 0 e estou procurando o valor 8 nas posições 0 até 2
eu, a thread 0 encontrei o valor 8 na posição 0
eu sou a thread 2 e estou procurando o valor 8 nas posições 6 até 9
eu sou a thread 1 e estou procurando o valor 8 nas posições 3 até 5

O número 8 foi encontrado no vetor
```
### ex2
Usuário escolhe o número de linhas e o número de colunas caso incialize o programa apenas com o arquivo de saída

```bash
informe o número de linhas da matriz
10
informe o número de colunas da matriz
10
 84  87  78  16  94  36  87  93  50  22 
 63  28  91  60  64  27  41  27  73  37 
 12  69  68  30  83  31  63  24  68  36 
 30   3  23  59  70  68  94  57  12  43 
 30  74  22  20  85  38  99  25  16  71 
 14  27  92  81  57  74  63  71  97  82 
  6  26  85  28  37   6  47  30  14  58 
 25  96  83  46  15  68  35  65  44  51 
 88   9  77  79  89  85   4  52  55 100 
 33  61  77  69  40  13  27  87  95  40 
Me diga quantas threads deseja criar, lembrando que o máximo é o menor número entre colunaXLinha! que no caso é: 10
10
Resultado gerado com sucessos!Demorou 0.001517 segundos
```

ou

Usuário incializa o programa com arquivo de saida e entrada:

```bash
 84  87  78  16  94  36  87  93  50  22 
 63  28  91  60  64  27  41  27  73  37 
 12  69  68  30  83  31  63  24  68  36 
 30   3  23  59  70  68  94  57  12  43 
 30  74  22  20  85  38  99  25  16  71 
 14  27  92  81  57  74  63  71  97  82 
  6  26  85  28  37   6  47  30  14  58 
 25  96  83  46  15  68  35  65  44  51 
 88   9  77  79  89  85   4  52  55 100 
 33  61  77  69  40  13  27  87  95  40 
Me diga quantas threads deseja criar, lembrando que o máximo é o menor número entre colunaXLinha! que no caso é: 10
10
Resultado gerado com sucessos!Demorou 0.000974 segundos
```