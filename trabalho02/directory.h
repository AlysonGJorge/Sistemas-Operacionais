#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // Para memcpy
#include <wchar.h>  // Para wchar_t e funções relacionadas
#include <ctype.h>

//#include <utilidades.h>

#define SECTOR_SIZE 512 // Tamanho do setor
//#define DIR_ENTRY_SIZE 32 // Tamanho da entrada de diretório
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20
#define ATTR_LFN 0x0F
#define END_OF_CLUSTER 0x0FFFFFF8
#define EMPTY_ENTRY 0x00
#define DELETED_ENTRY 0xE5


typedef struct {
    uint32_t total_sectors;
    uint32_t fat_size_sectors;
    uint32_t cluster_size_bytes;
    uint32_t root_cluster;
    uint32_t fat_offset;
    uint32_t data_offset;
    uint32_t total_clusters;
    uint32_t free_clusters;
    uint32_t reserved_clusters;
    uint32_t used_clusters;
} DiskInfo;

// Estrutura para uma entrada de diretório
#pragma pack(push, 1)
typedef struct {
    char name[11];         // Nome do arquivo/diretório (8.3 format)
    uint8_t attr;          // Atributos
    uint8_t reserved;      // Reservado
    uint8_t createTimeTenth;
    uint16_t time;         // Hora de criação
    uint16_t date;         // Data de criação
    uint16_t accessDate;
    uint16_t start_high;   // Cluster inicial (parte alta)
    uint16_t time_mod;     // Hora de modificação
    uint16_t date_mod;     // Data de modificação
    uint16_t start_low;    // Cluster inicial (parte baixa)
    uint32_t size;         // Tamanho do arquivo (0 para diretórios)
} DirectoryEntry;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint8_t order;
    uint16_t name1[5];
    uint8_t attr;
    uint8_t type;
    uint8_t checksum;
    uint16_t name2[6];
    uint16_t first_cluster;
    uint16_t name3[2];
} LFNEntry;
#pragma pack(pop)

void print_lfn_entry_hex(const LFNEntry *entry);
void extract_lfn_characters(LFNEntry *lfn_entry, char *long_name, int *index);
int create_directory_entry(FILE *image, uint32_t dir_cluster, uint32_t data_offset,
    uint32_t cluster_size, uint32_t fat_ofsset, const char *filename,
    uint8_t attr, uint32_t size, uint32_t *start_cluster);
int find_entry_in_directory(FILE *image, uint32_t dir_cluster, uint32_t data_offset,
    uint32_t cluster_size, const char *name, uint8_t req_attr,
    uint32_t *start_cluster, uint32_t *size);
void reconstruct_long_name(char *long_name, LFNEntry *lfn_entries, int total_entries);
void print_hex(const uint8_t *data, size_t size);
void read_directory(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset);
int is_directory_empty(FILE *image, uint32_t start_cluster, uint32_t fat_offset, uint32_t data_offset, uint32_t cluster_size);
void display_attributes(uint8_t attributes);