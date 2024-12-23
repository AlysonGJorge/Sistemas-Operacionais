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
- `list`: Biblioteca para manipulação de listas.
- `unistd.h`: Biblioteca padrão para manipulação de processos no POSIX.
- `algorithm`: Biblioteca para algoritmos padrão.

## Exemplo de uso

Após compilar e executar o programa, você verá a simulação do sistema de avaliação de Padawans gerenciado por Yoda. Cada Padawan passará por um teste e se aprovado, passará pela cerimônia de corte de tranças, enquanto espectadores assistem ao processo. Ao final, Yoda fará um discurso para encerrar a cerimônia. Abaixo, um exemplo de execução do programa e como seu fluxo funciona:

```sh
make
./main
Digite o numero de padawans: 5
Digite o numero de espectadores: 4
Padawan_0 foi criado
Padawan_1 foi criado
Padawan_2 foi criado
Padawan_3 foi criado
Padawan_4 foi criado
Espectador_0 foi criado
Espectador_1 foi criado
Espectador_2 foi criado
Espectador_3 foi criado
Yoda foi criado
[Yoda]: Por 4 segundos, a entrada liberarei
Yoda liberou a entrada para os próximos testes
Padawan_2 entrou no salao
Padawan_2 cumprimentou os mestres avaliadores
Padawan_1 entrou no salao
Padawan_1 cumprimentou os mestres avaliadores
Padawan_1 está aguardando teste
Espectador_0 entrou no salao
Padawan_4 entrou no salao
Padawan_4 cumprimentou os mestres avaliadores
Padawan_4 está aguardando teste
Padawan_2 está aguardando teste
Espectador_3 entrou no salao
Padawan_3 entrou no salao
Padawan_3 cumprimentou os mestres avaliadores
Padawan_3 está aguardando teste
Padawan_0 entrou no salao
Padawan_0 cumprimentou os mestres avaliadores
Padawan_0 está aguardando teste
Espectador_1 entrou no salao
Espectador_2 entrou no salao
[Yoda]: Em teste, todos estão. O teste começará em breve.
Espectador_2 está assistindo os testes
Espectador_0 está assistindo os testes
Padawan_2 está realizando o teste.
Espectador_1 está assistindo os testes
Espectador_3 está assistindo os testes
Padawan_1 está realizando o teste.
Padawan_4 está realizando o teste.
Padawan_3 está realizando o teste.
Padawan_0 está realizando o teste.
[Yoda]: Concluído o teste foi. Os Resultados anunciados em breve serão.
[Yoda]: Padawan_1 foi aprovado
[Yoda]: Padawan_4 foi aprovado
[Yoda]: Padawan_3 foi aprovado
[Yoda]: A cerimônia de corte de trança começará em breve
Padawan_2 decepcionadamente cumprimenta Yoda
Padawan_2 saiu do salao
Padawan_0 decepcionadamente cumprimenta Yoda
Padawan_0 saiu do salao
Padawan_3 está aguardando o corte de trança
Padawan_4 está aguardando o corte de trança
Padawan_1 está aguardando o corte de trança
[Yoda]: Padawan_1 sua trança cortada teve. 
[Yoda]: Padawan_4 sua trança cortada teve. 
[Yoda]: Padawan_3 sua trança cortada teve. 
Padawan_3 saiu do salao
Padawan_1 saiu do salao
Padawan_4 saiu do salao
Espectador_3 se entediou e resolve sair do salao
Espectador_1 se entediou e resolve sair do salao
Espectador_2 se entediou e resolve sair do salao
Espectador_0 se entediou e resolve sair do salao
[Yoda]: Por fim, os testes todos se encerram.
Yoda começou a discursar
[Yoda]:A avaliação, concluída está. Muitos desafios enfrentaram, mas a força dentro de cada um de vocês, clara foi.
 [Yoda]: Alguns de vocês, Jedi se tornam, outros, o caminho ainda terão que percorrer. 
O sucesso, mais que habilidade demonstra, a coragem, a perseverança e a sabedoria que cada um possui em seu coração.
[Yoda]: Lembre-se, o aprendizado nunca termina. A jornada, longa é. 
Em vocês, jovens Padawans, vejo o futuro da Ordem. Não importa o resultado, todos cresceram e aprenderam algo valioso.
[Yoda]: A Força, sempre os guiará. Mantenham-se firmes em seu propósito, e nunca duvidem de si mesmos, pois grandes feitos estão diante de vocês. Que a Força os acompanhe, hoje e sempre.
```