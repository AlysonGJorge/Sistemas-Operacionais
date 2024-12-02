#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

/*
    Cabeçalho do código ex1.c
    Criado por: Alyson Gonçalves Jorge, Hudson Thayllor Perrut Cassim, Natanael Tagliaferro Galafassi
    Data: 29/11/2024
    Finalidade: Algoritmo lê e escreve um arquivo, fazendo um graceful stop caso receba SIGINT ou SIGTERM;
*/

FILE *file = NULL; // Inicializa para NULL
int should_continue = 1; // Continua o laço

/**
 * Função handle_signal 
 * Trata os sinais SIGINT e SIGTERM
 * parâmetros: signal - sinal recebido
*/

void handle_signal(int signal) {
    if (file != NULL) {
        printf("\nSignal %d recebido. Salvando informações e fechando arquivo...\n", signal);
        fflush(file); // Assegura que os dados pendentes sejam escritos no arquivo
        fclose(file);
        file = NULL; // Coloca o ponteiro para null após fechar o arquivo
        printf("Arquivo fechado com sucesso.\n");
    }
        should_continue = 0; // Acaba o laço
    }

/**
 * Função setup_signal_handlers 
 * Configura os sinais SIGINT e SIGTERM
 * parâmetros: nenhum
*/

void setup_signal_handlers() {
    if (signal(SIGINT, handle_signal) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");
    if (signal(SIGTERM, handle_signal) == SIG_ERR)
        printf("\ncan't catch SIGTERM\n");
}

int main() {
    char buffer[256];
    const char *filename = "dados.txt";

    setup_signal_handlers();

    file = fopen(filename, "a+");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return EXIT_FAILURE;
    }
    printf("eu, o programa com o pid %d irei escrever no arquivo %s\n", getpid(), filename);
    printf("Arquivo '%s' aberto para escrita e leitura.\n", filename);
    printf("Digite texto para salvar no arquivo. Digite 'sair' para encerrar o programa normalmente.\n");

    while (should_continue) {
        printf("> ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break; // Encerra o laço se não houver mais entrada
        }

        if (strncmp(buffer, "sair", 4) == 0) {
            printf("Encerrando o programa normalmente...\n");
            break;
        }

        fprintf(file, "%s", buffer);
        fflush(file); // Assegura que os dados pendentes sejam escritos no arquivo
    }

    if (file != NULL) {
        fclose(file);
        file = NULL; // Coloca o ponteiro para null após fechar o arquivo
        printf("Arquivo fechado.\n");
    }

    printf("Programa encerrado.\n");
    return EXIT_SUCCESS;
}