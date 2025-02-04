#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "fat.c"
//#include "bootsector.c"
#include "directory.c"
#include "shell.c"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <imagem_fat32>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    BootSector bs;

    FILE *file = fopen(filename, "r+b");
    if (!file) {
        perror("Erro ao abrir a imagem");
        return 1;
    }

    if (read_bootsector(filename, &bs) != 0) {
        fprintf(stderr, "Falha ao ler o bootsector da imagem %s\n", filename);
        fclose(file);
        return 1;
    }

    //printf("Informações do Bootsector:\n");
    //print_bootsector_info(&bs);

    // Ler a FAT
    uint32_t *fat = read_fat(file, &bs);
    if (!fat) {
        fclose(file);
        return 1;
    }

    // Exibir algumas entradas da FAT
    uint32_t num_clusters = (bs.total_sectors_32 - bs.reserved_sectors) / bs.sectors_per_cluster;
    print_fat(fat, num_clusters, 0, 20);
   //printf("Setor booot %u\n", bs.root_cluster);

     // Listar o diretório raiz
    printf("Listando o diretório raiz:\n");
    uint32_t fat_offset = bs.reserved_sectors * bs.bytes_per_sector;
    uint32_t data_offset = fat_offset + (bs.num_fats * bs.fat_size_32 * bs.bytes_per_sector);
    printf("FAT Offset: %u, Data Offset: %u\n", fat_offset, data_offset);
    printf("Bytes por Setor: %u, Setores por Cluster: %u\n", bs.bytes_per_sector, bs.sectors_per_cluster);

    read_directory(file, bs.root_cluster, bs.bytes_per_sector, bs.sectors_per_cluster, fat_offset, data_offset);

    char command[256];
     printf("\n\n");
     uint32_t current_cluster = 2; // Cluster 2 é geralmente o root.
     char current_path[256]  = {'/'}; // Cluster 2 é geralmente o root.
     char last_path[256]  = {' '}; // Cluster 2 é geralmente o root.
    while (1) {
        printf("fatshell:[img%s]$ ", current_path);
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; // EOF ou erro
        }

        // Remove o '\n' do final
        command[strcspn(command, "\n")] = '\0';

        // Processa o comando
        process_command(command, file, &bs, fat, "", &current_cluster, current_path, last_path);
    }


    free(fat);
    fclose(file);
    return 0;
}

