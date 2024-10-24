#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>

#define tamanhoMaximoEntrada 1024
#define maximoArgumentos 100

int main() {
    printf("Bem-vindo ao melhor Shell do mundo!!!!!\n");
    printf("Se você digitar 9 o programa fecha\n");

    char buffer[tamanhoMaximoEntrada];
    char* argumentos[maximoArgumentos];
    bool deFundo;

    while (1) {
        deFundo = false;
        printf("Digite um comando> ");
        fflush(stdout); // limpa buffer de lixo de memória

        if (fgets(buffer, tamanhoMaximoEntrada, stdin) == NULL) {
            printf("Erro! algo aconteceu na captura do comando");
            break; // Se falhar ao ler 
        }

        if (strcmp(buffer, "9\n") == 0) {
            break; // Saída se o usuário digitar "9"
        }

        if (strcmp(buffer, "\n") == 0) {
            continue; // Ignorar linhas em branco
        }

        // Remove o \n do final do buffer
        buffer[strcspn(buffer, "\n")] = 0;

        // divide a string em vários tokens
        char *token = strtok(buffer, " ");

        
        int numeroArgumentos = 0;

        while (token != NULL) {
            if (numeroArgumentos < maximoArgumentos - 1) {
                argumentos[numeroArgumentos] = token; // Adiciona token e incrementa
                numeroArgumentos++;
            }
            token = strtok(NULL, " "); // vai pro próximo token e retorna o endereço dele
        }
        argumentos[numeroArgumentos] = NULL; // Termina a lista com NULL para o execvp funcionar

        // Verifica se o último argumento é "&"
        if (numeroArgumentos > 0 && strcmp(argumentos[numeroArgumentos - 1], "&") == 0) {
            deFundo = true; // faz com que o programa seja executado em segundo plano
            argumentos[numeroArgumentos - 1] = NULL; // Remove o "&"
        }

        pid_t processo = fork();
        if (processo == 0) {
            // Processo filho
            int CommandoValido = execvp(argumentos[0], argumentos);
            if (CommandoValido == -1) {
                perror("Comando inválido");
                exit(1); // Saída com erro
            }
        } else if (processo > 0) {
            // Processo pai
            if (!deFundo) {
                waitpid(processo, NULL, 0); // Espera o filho terminar
                printf("Processo %d executado com sucesso.\n", processo);
            } else {
                printf("Processo %d executando seu comando em segundo plano.\n", processo);
            }
        } else {
            perror("Erro ao criar processo");
        }
    }
    return 0;
}
