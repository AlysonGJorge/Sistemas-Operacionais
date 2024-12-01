#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

FILE *file; // Ponteiro global para o arquivo
int should_continue = 1; // Flag para controle do loop principal

void handle_signal(int signal) {
    if (file != NULL) {
        printf("\nSignal %d recebido. Salvando informações e fechando arquivo...\n", signal);
        fflush(file); // Garante que os dados pendentes sejam escritos no arquivo
        fclose(file);
        printf("Arquivo fechado com sucesso.\n");
    }
    should_continue = 0; // Sinaliza para encerrar o programa
}

void setup_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;

    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
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

    printf("Arquivo '%s' aberto para escrita e leitura.\n", filename);
    printf("Digite texto para salvar no arquivo. Digite 'sair' para encerrar o programa normalmente.\n");

    while (should_continue) {
        printf("> ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break; // Trata interrupção ou erro
        }

        if (strncmp(buffer, "sair", 4) == 0) {
            printf("Encerrando o programa normalmente...\n");
            break;
        }

        fprintf(file, "%s", buffer);
        fflush(file); // Garante que os dados sejam salvos imediatamente
    }

    if (file != NULL) {
        fclose(file);
        printf("Arquivo fechado.\n");
    }

    printf("Programa encerrado.\n");
    return EXIT_SUCCESS;
}