#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "./headers/bootsector.h"



// Função para ler o Bootsector
int read_bootsector(const char *filename, BootSector *bs) {
    FILE *file = fopen(filename, "r+b");  // Abrir arquivo em modo de leitura binária
    if (!file) {
        perror("Erro ao abrir a imagem");
        return -1;
    }

    // Ler o bootsector do início do arquivo
    if (fread(bs, sizeof(BootSector), 1, file) != 1) {
        perror("Erro ao ler o bootsector");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

void print_bootsector_info(const BootSector *bs) {
    printf("Jump Code: %02X %02X %02X\n", bs->jump_code[0], bs->jump_code[1], bs->jump_code[2]);
    printf("OEM Name: %.8s\n", bs->oem_name);
    printf("Bytes per Sector: %u\n", bs->bytes_per_sector);
    printf("Sectors per Cluster: %u\n", bs->sectors_per_cluster);
    printf("Reserved Sectors: %u\n", bs->reserved_sectors);
    printf("Number of FATs: %u\n", bs->num_fats);
    printf("Total Sectors (16-bit): %u\n", bs->total_sectors_16);
    printf("Total Sectors (32-bit): %u\n", bs->total_sectors_32);
    printf("FAT Size (32-bit): %u\n", bs->fat_size_32);
    printf("Root Cluster: %u\n", bs->root_cluster);
    printf("Volume Label: %.11s\n", bs->volume_label);
    printf("File System Type: %.8s\n", bs->fs_type);
    printf("Boot Sector Signature: 0x%04X\n", bs->boot_sector_signature);
}
