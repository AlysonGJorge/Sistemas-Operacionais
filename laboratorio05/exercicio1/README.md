# Como Compilar
Para compilar os Exercícios basta executar o comando `make` neste diretório. Os binários gerados serão salvos neste diretório com os nomes de `ex1Escrita` e `ex1Leitura`.

# Como Executar
Para executar os Exercícios basta executar o programa `ex1Leitura` primeiro para gerar o FIFO caso ele já não exista e logo em seguida o programa `ex1Escrita`, em seguida o usuário pode digitar no programa

# Bibliotecas Utilizadas
- **stdio.h (Standard I/O)** - para funções de entrada e saída
- **stdlib.h (Standard Library)** - para funções de utilidade geral
- **sys/stat.h (File Status)** - Para funções relacionadas a status de arquivos, como mkfifo.
- **errno.h (Error NUmbers)** - Para tratamento de erros.
- **unistd.h (UNIX standard)** - Para funções de sistema.
- **string.h (String Operations)** - Para manipulação de strings
- **fcntl.h (File Control)** - Para funções de controle de arquivos.

# Exemplo de Execução
```bash
$ make
gcc ex1Leitura.c -o ex1Leitura && gcc ex1Escrita.c -o ex1Escrita
$ ./ex1Leitura
ex1Leitura:
Leitura iniciada, aguardando mensagens...

em outro terminal:
$ ./ex1Escrita
envie uma mensagem!
$ estou escrevendo

ex1Leitura:
Mensagem recebida:
estou escrevendo
Inicio da contagem de caracteres:
Número de caracteres: 16
Número de vogais: 7
Número de consoantes: 8
Número de espaços: 1
```