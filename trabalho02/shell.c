#include "shell.h"


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
    uint32_t num_clusters = (bs->total_sectors_32 - bs->reserved_sectors) / bs->sectors_per_cluster;

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
    } else if (strcmp(args[0], "touch") == 0) {
        if (arg_count > 1) {
            touch(file, *current_cluster, bs->bytes_per_sector, bs->sectors_per_cluster, fat_offset, data_offset, args[1],0,0, num_clusters);
        } else {
            printf("Uso: touch NomeArquivo\n");
        }
    } else if (strcmp(args[0], "mkdir") == 0) {
        if (arg_count > 1) {
            mkdir(file, *current_cluster, bs->bytes_per_sector, bs->sectors_per_cluster, fat_offset, data_offset, args[1], fat, num_clusters);
        } else {
            printf("Uso: mkdir NomeArquivo\n");
        }
    } else if (strcmp(args[0], "rm") == 0) {
        if (arg_count > 1) {
            rm(file, *current_cluster, bs->bytes_per_sector, bs->sectors_per_cluster, fat_offset, data_offset, args[1], fat, num_clusters);
        } else {
            printf("Uso: rm NomeArquivo\n");
        }
    }  else if (strcmp(args[0], "cp") == 0) {
        if (arg_count > 2) {
            cp(file, *current_cluster, bs->bytes_per_sector, bs->sectors_per_cluster, fat_offset, data_offset, args[1], args[2], fat, num_clusters);
        } else {
            printf("Uso: cp sourcePath targetPath\n");
        }
    } else  if (strcmp(args[0], "rmdir") == 0) {
        if (arg_count < 2)
            printf("Uso: rmdir <diretorio>\n");

        rmdir(file, *current_cluster, bs->bytes_per_sector, bs->sectors_per_cluster, fat_offset, data_offset, args[1], fat, num_clusters);
    } else if (strcmp(args[0], "mv") == 0) {
        if (arg_count < 3)
            printf("Uso: mv <sourcePath> <targetPath>\n");

        mv(file, *current_cluster, bs->bytes_per_sector, bs->sectors_per_cluster, fat_offset, data_offset, args[1], args[2], fat, num_clusters);
    } else if (strcmp(args[0], "rename") == 0) {
        if (arg_count < 3)
            printf("Uso: rename <filename> <new_filename>\n");

        rename_file(file, *current_cluster, bs->bytes_per_sector, bs->sectors_per_cluster, fat_offset, data_offset, args[1], args[2], fat, num_clusters, current_path);
    }
    
    else if (strcmp(args[0], "exit") == 0) {
        printf("Saindo...\n");
        exit(0);
    } else {
        printf("Comando desconhecido: %s\n", args[0]);
    }
}