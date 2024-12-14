# README

## Como compilar

Para compilar o programa, utilize o seguinte comando no terminal:

```sh
make
```

## Como executar

Para executar o programa, utilize o seguinte comando no terminal:

```sh
./main
```

## Bibliotecas usadas

- `pthread.h`: Biblioteca padrão para manipulação de threads no POSIX.
- `queue`: Biblioteca para manipulação de filas.
- `semaphore.h`: Biblioteca para manipulação de semáforos.
- `unistd.h`: Biblioteca padrão para manipulação de processos no POSIX.

## Exemplo de uso

Após compilar e executar o programa, você verá a simulação do problema do jantar dos filósofos. Cada filósofo alternará entre pensar e comer, e o programa garantirá que não haverá deadlock.

```sh
make
./main
O filoso 0 está pensando.
O filoso 1 está pensando.
O filoso 3 está pensando.
O filoso 2 está pensando.
O filoso 4 está pensando.
O filoso 0 está com fome.
O filoso 1 está com fome.
O filoso 2 está com fome.
O filoso 2 está comendo.
O filoso 0 está comendo.
O filoso 3 está com fome.
O filoso 4 está com fome.
O filoso 2 terminou de comer. 
O filoso 2 está pensando.
O filoso 0 terminou de comer. 
O filoso 3 está comendo.
O filoso 1 está comendo.
O filoso 0 está pensando.
O filoso 3 terminou de comer. 
O filoso 3 está pensando.
O filoso 1 terminou de comer. 
O filoso 1 está pensando.
O filoso 0 está com fome.
O filoso 2 está com fome.
O filoso 4 está comendo.
O filoso 2 está comendo.
O filoso 3 está com fome.
O filoso 4 terminou de comer. 
O filoso 4 está pensando.
O filoso 2 terminou de comer. 
O filoso 2 está pensando.
O filoso 0 está comendo.
O filoso 1 está com fome.
O filoso 3 está comendo.
O filoso 4 está com fome.
O filoso 2 está com fome.
O filoso 0 terminou de comer. 
O filoso 0 está pensando.
O filoso 3 terminou de comer. 
O filoso 3 está pensando.
O filoso 4 está comendo.
O filoso 1 está comendo.
O filoso 3 está com fome.
O filoso 4 terminou de comer. 
O filoso 4 está pensando.
O filoso 1 terminou de comer. 
O filoso 1 está pensando.
O filoso 3 está comendo.
O filoso 0 está com fome.
O filoso 0 está comendo.
O filoso 1 está com fome.
O filoso 4 está com fome.
O filoso 3 terminou de comer. 
O filoso 3 está pensando.
O filoso 0 terminou de comer. 
O filoso 0 está pensando.
O filoso 4 está comendo.
O filoso 2 está comendo.
O filoso 3 está com fome.
O filoso 0 está com fome.
O filoso 4 terminou de comer. 
O filoso 4 está pensando.
O filoso 0 está comendo.
O filoso 2 terminou de comer. 
O filoso 2 está pensando.
O filoso 3 está comendo.
O filoso 4 está com fome.
O filoso 0 terminou de comer. 
O filoso 0 está pensando.
O filoso 1 está comendo.
O filoso 2 está com fome.
O filoso 3 terminou de comer. 
O filoso 3 está pensando.
O filoso 4 está comendo.
O filoso 0 está com fome.
O filoso 1 terminou de comer. 
O filoso 1 está pensando.
O filoso 2 está comendo.
O filoso 3 está com fome.
O filoso 4 terminou de comer. 
O filoso 4 está pensando.
O filoso 0 está comendo.
O filoso 2 terminou de comer. 
O filoso 2 está pensando.
O filoso 3 está comendo.
O filoso 1 está com fome.
O filoso 4 está com fome.
O filoso 0 terminou de comer. 
O filoso 0 está pensando.
O filoso 1 está comendo.
```