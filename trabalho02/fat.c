#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "./headers/fat.h"
#include "./headers/bootsector.h"

// Função para ler a FAT
uint32_t *read_fat(FILE *file, const BootSector *bs) {
    uint32_t fat_size_bytes = bs->fat_size_32 * bs->bytes_per_sector;
    uint32_t *fat = malloc(fat_size_bytes);
    if (!fat) {
        perror("Erro ao alocar memória para a FAT");
        return NULL;
    }

    // Mover o ponteiro do arquivo para o início da FAT
    uint32_t fat_start = bs->reserved_sectors * bs->bytes_per_sector;
    if (fseek(file, fat_start, SEEK_SET) != 0) {
        perror("Erro ao posicionar ponteiro para a FAT");
        free(fat);
        return NULL;
    }

    // Ler a FAT inteira
    if (fread(fat, 1, fat_size_bytes, file) != fat_size_bytes) {
        perror("Erro ao ler a FAT");
        free(fat);
        return NULL;
    }

    return fat;
}

void print_fat(uint32_t *fat, uint32_t num_entries, uint32_t start, uint32_t count) {
    printf("Entradas da FAT (de %u a %u):\n", start, start + count - 1);
    for (uint32_t i = start; i < start + count && i < num_entries; i++) {
        printf("Cluster %u: 0x%08X\n", i, fat[i]);
    }
}

