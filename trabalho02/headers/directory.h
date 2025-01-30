#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <ctype.h>

// Estrutura para uma entrada de diretório
#pragma pack(push, 1)
typedef struct {
    char name[11];         // Nome do arquivo/diretório (8.3 format)
    uint8_t attr;          // Atributos
    uint8_t reserved[10];  // Reservado
    uint16_t time;         // Hora de criação
    uint16_t date;         // Data de criação
    uint16_t start_high;   // Cluster inicial (parte alta)
    uint16_t time_mod;     // Hora de modificação
    uint16_t date_mod;     // Data de modificação
    uint16_t start_low;    // Cluster inicial (parte baixa)
    uint32_t size;         // Tamanho do arquivo (0 para diretórios)
} DirectoryEntry;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint8_t order;          // Ordem da entrada LFN
    uint16_t name1[5];      // Primeiros 5 caracteres do nome
    uint8_t attr;           // Atributo (0x0F para LFN)
    uint8_t type;           // Tipo (sempre 0)
    uint8_t checksum;       // Checksum associado à entrada principal
    uint16_t name2[6];      // Próximos 6 caracteres do nome
    uint16_t first_cluster; // Sempre 0 para entradas LFN
    uint16_t name3[2];      // Últimos 2 caracteres do nome
} LFNEntry;
#pragma pack(pop)

void read_directory(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset);

void extract_lfn_characters(LFNEntry *lfn_entry, char *long_name, int *index);

void reconstruct_long_name(char *long_name, LFNEntry *lfn_entries, int total_entries);

void format_to_83_name(const char *name, uint8_t *dest);

uint32_t find_free_cluster(FILE *image, uint32_t fat_offset, uint32_t total_clusters);

int create_directory(FILE *image, uint32_t parent_cluster,char *directory_name, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, uint32_t total_clusters);

# endif // DIRECTORY_H