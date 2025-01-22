// Variável global para o diretório atual (inicia no cluster do root)
#include <stdio.h>
#include <stdint.h>
#include <string.h>


uint32_t current_cluster = 2; // Cluster 2 é geralmente o root.

void process_command(char *command, const char *image_path) {
    char *args[10]; // Máximo de 10 argumentos
    int arg_count = 0;

    // Divide a entrada em palavras
    char *token = strtok(command, " ");
    while (token != NULL && arg_count < 10) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }

    if (arg_count == 0) {
        return; // Nenhum comando digitado
    }

    // Identifica e executa o comando
    if (strcmp(args[0], "ls") == 0) {
        // Listar diretório atual
        //list_directory();
    } else if (strcmp(args[0], "cd") == 0) {
        if (arg_count > 1) {
            // Muda para o diretório especificado
            //change_directory(args[1]);
        } else {
            printf("Uso: cd <diretorio>\n");
        }
    } else if (strcmp(args[0], "exit") == 0) {
        printf("Saindo...\n");
        exit(0);
    } else {
        printf("Comando desconhecido: %s\n", args[0]);
    }
}