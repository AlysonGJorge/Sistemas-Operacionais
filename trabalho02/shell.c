// Variável global para o diretório atual (inicia no cluster do root)
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "comandos.c"
/*void display_cluster_content(FILE *image, const BootSector *bs, uint32_t cluster_num) {
    DiskInfo info;
    gather_disk_info(image, bs, &info);

    if (cluster_num < 2 || cluster_num >= info.total_clusters + 2) {
        printf("Cluster inválido: %u. Os clusters válidos estão entre 2 e %u.\n",
               cluster_num, info.total_clusters + 1);
        return;
    }

    // Calcula o offset do cluster na Data Region
    uint32_t cluster_offset = info.data_offset + 
                              (cluster_num - 2) * bs->sectors_per_cluster * bs->bytes_per_sector;

    fseek(image, cluster_offset, SEEK_SET);

    // Lê o conteúdo do cluster
    uint8_t *buffer = malloc(info.cluster_size_bytes);
    if (!buffer) {
        perror("Erro ao alocar memória para o buffer");
        return;
    }

    if (fread(buffer, 1, info.cluster_size_bytes, image) != info.cluster_size_bytes) {
        perror("Erro ao ler o cluster");
        free(buffer);
        return;
    }

    printf("===== Conteúdo do Cluster %u =====\n", cluster_num);
    for (uint32_t i = 0; i < info.cluster_size_bytes; i++) {
        if (buffer[i] == '\0') {
            // Exibe espaços para bytes nulos
            putchar('.');
        } else {
            putchar(buffer[i]);
        }
    }
    putchar('\n');

    free(buffer);
}*/




void process_command(char *command, FILE *file, const BootSector *bs, uint32_t *fat, const char *image_path, uint32_t *current_cluster, char * current_path, char * last_path) {
    char *args[10];
    int arg_count = 0;

    char *token = strtok(command, " ");
    while (token != NULL && arg_count < 10) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }

    if (arg_count == 0) {
        return;
    }

    uint32_t fat_offset = bs->reserved_sectors * bs->bytes_per_sector;
    uint32_t data_offset = fat_offset + (bs->num_fats * bs->fat_size_32 * bs->bytes_per_sector);

    if (strcmp(args[0], "attr") == 0) {
        if (arg_count > 1) {
            attr(file, *current_cluster, bs->bytes_per_sector, bs->sectors_per_cluster, fat_offset, data_offset, args[1]);
        } else {
            printf("Uso: attr <file | dir>\n");
        }
    }else if (strcmp(args[0], "ls") == 0) {
        ls(file, *current_cluster, bs->bytes_per_sector, bs->sectors_per_cluster, fat_offset, data_offset );
    } else if (strcmp(args[0], "cd") == 0) {
        if (arg_count > 1) {
            cd(file, *current_cluster, bs->bytes_per_sector, bs->sectors_per_cluster, fat_offset, data_offset, args[1], current_cluster, current_path, last_path);
            printf("current_cluster : %d\n",  (int) *current_cluster);
            printf("current_cluster_path : %s\n", current_path);
        } else {
            printf("Uso: cd <diretorio>\n");
        }
    } else if (strcmp(args[0], "info") == 0) {
        info_teste(file, bs);
    } else if (strcmp(args[0], "cluster") == 0) {
        if (arg_count > 1) {
            uint32_t cluster_num = strtoul(args[1], NULL, 10);
            display_cluster_content(file, bs, cluster_num);
        } else {
            printf("Uso: cluster <num>\n");
        }
    } else if (strcmp(args[0], "pwd") == 0) {
        pwd(current_path);
    } else if (strcmp(args[0], "exit") == 0) {
        printf("Saindo...\n");
        exit(0);
    } else {
        printf("Comando desconhecido: %s\n", args[0]);
    }
}