# Como Compilar
Para compilar o Exercício basta executar o comando `make` neste diretório. O binário gerado será salvo neste diretório com o nome de `ex2`.

# Como Executar
Para executar o Exercício basta executar o programa `ex2`, em seguida o usuário pode digitar no programa para salvá-lo em um arquivo de texto chamado dados.txt

# Bibliotecas Utilizadas
- **stdio.h (Standard I/O)** - Para funções de entrada e saída.
- **stdlib.h (Standard Library)** - Para funções de utilidade geral.
- **signal.h (Signal Handling)** - Para manipulação de sinais.
- **string.h (String Operations)** - Para manipulação de strings.
- **unistd.h (UNIX Standard)** - Para funções de sistema.

# Exemplo de Execução
```bash
$ make
gcc ex2.c -o ex2 -Wall -Wextra -Werror -pedantic -std=c99
$./ex2
eu, o programa com o pid <pid> irei escrever no arquivo 'dados.txt'
Digite o texto para salvar no arquivo. Digite 'Sair' para encerrar o programa normalmente.
$> a
$> a
$> a
CNTRL + C
Signal 2 recebido. Salvando informações e fechando arquivo...
Arquivo Fechado com sucesso.
Programa encerrado.
```