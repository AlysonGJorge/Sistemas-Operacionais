// Variável global para o diretório atual (inicia no cluster do root)
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "./headers/bootsector.h"
#include "./headers/shell.h"
#include "./headers/directory.h"

uint32_t current_cluster = 2; // Cluster 2 é geralmente o root.

void process_command(char *command, const char *image_path, const BootSector bs, FILE *file) {
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
        uint32_t fat_offset = bs.reserved_sectors * bs.bytes_per_sector;
        uint32_t data_offset = fat_offset + (bs.num_fats * bs.fat_size_32 * bs.bytes_per_sector);
        read_directory(file, current_cluster, bs.bytes_per_sector, bs.sectors_per_cluster, fat_offset, data_offset);
    } else if (strcmp(args[0], "cd") == 0) {
        if (arg_count > 1) {
            // Muda para o diretório especificado
            //change_directory(args[1]);
        } else {
            printf("Uso: cd <diretorio>\n");
        }
    } else if (strcmp(args[0], "mkdir") == 0) {
        if (arg_count <= 1) {
            printf("Uso: mkdir <nome>\n");
            return;
        }
        // Cria um novo diretório
        // TODO: Implementar verificação para caracteres inválidos.
        printf("root_cluster: %d\n", bs.root_cluster);
        uint32_t fat_offset = bs.reserved_sectors * bs.bytes_per_sector;
        uint32_t data_offset = fat_offset + (bs.num_fats * bs.fat_size_32 * bs.bytes_per_sector);
        uint32_t total_clusters = (bs.total_sectors_32 - (data_offset / bs.bytes_per_sector)) / bs.sectors_per_cluster;
        int result = create_directory(file, current_cluster, args[1], bs.bytes_per_sector, 
            bs.sectors_per_cluster, 
            fat_offset, data_offset, total_clusters
        );

        if (result != 0) {
            printf("Erro %d ao criar diretório.\n", result);
        }

        
    }  else if (strcmp(args[0], "exit") == 0) {
        printf("Saindo...\n");
        exit(0);
    } else {
        printf("Comando desconhecido: %s\n", args[0]);
    }
}