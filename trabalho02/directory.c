#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // Para memcpy
#include <wchar.h>  // Para wchar_t e funções relacionadas


#define SECTOR_SIZE 512 // Tamanho do setor
//#define DIR_ENTRY_SIZE 32 // Tamanho da entrada de diretório
#define ATTR_DIRECTORY 0x10 // Atributo para diretório

// Estrutura para uma entrada de diretório
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


void extract_lfn_characters(LFNEntry *lfn_entry, char *long_name, int *index) {
    for (int i = 0; i < 5; i++) {
        if (lfn_entry->name1[i] == 0x0000) // Fim do nome
            return;
        long_name[(*index)++] = (lfn_entry->name1[i] <= 0xFF) ? (char)lfn_entry->name1[i] : '?';
    }
    for (int i = 0; i < 6; i++) {
        if (lfn_entry->name2[i] == 0x0000) // Fim do nome
            return;
        long_name[(*index)++] = (lfn_entry->name2[i] <= 0xFF) ? (char)lfn_entry->name2[i] : '?';
    }
    for (int i = 0; i < 2; i++) {
        if (lfn_entry->name3[i] == 0x0000) // Fim do nome
            return;
        long_name[(*index)++] = (lfn_entry->name3[i] <= 0xFF) ? (char)lfn_entry->name3[i] : '?';
    }
}



void reconstruct_long_name(char *long_name, LFNEntry *lfn_entries, int total_entries) {
    int index = 0;
    long_name[0] = '\0'; // Inicializa a string

    for (int i = total_entries - 1; i >= 0; i--) {
        extract_lfn_characters(&lfn_entries[i], long_name, &index);
    }
    long_name[index] = '\0'; // Finaliza a string
}




void read_directory(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset) {
    uint32_t cluster = root_cluster;
    uint32_t cluster_size = bytes_per_sector * sectors_per_cluster;
    uint8_t buffer[SECTOR_SIZE];

    
    LFNEntry lfn_buffer[20];
    int lfn_index = 0;
    while (cluster < 0x0FFFFFF8) { // Clusters válidos no FAT32
        uint32_t cluster_address = data_offset + (cluster - 2) * cluster_size;
        fseek(image, cluster_address, SEEK_SET);
        fread(buffer, cluster_size, 1, image);

        for (int i = 0; i < cluster_size; i += sizeof(DirectoryEntry)) {
            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

            if (entry->name[0] == 0x00) // Entrada vazia
                break;

            if (entry->name[0] == 0xE5) // Entrada deletada
                continue;
            if (entry->attr == 0x0F) { // Verifica se é uma entrada LFN
                LFNEntry *lfn_entry = (LFNEntry *)entry;
                if ((lfn_entry->order & 0x40) != 0) { // Primeira entrada de uma sequência LFN
                    lfn_index = 0; // Reinicia o buffer
                }
                lfn_buffer[lfn_index++] = *lfn_entry;
            } else {
                // Reconstrói o nome longo se existirem entradas LFN
                char long_name[256] = {0};
                if (lfn_index > 0) {
                    reconstruct_long_name(long_name, lfn_buffer, lfn_index);
                    lfn_index = 0; // Limpa o índice
                }

                uint32_t starting_cluster = (entry->start_high << 16) | entry->start_low;
                if (entry->attr & ATTR_DIRECTORY) {
                    printf("[DIR] %s (%.11s) - Cluster: %u\n", 
                        (strlen(long_name) > 0) ? long_name : entry->name, 
                        entry->name, 
                        starting_cluster);
                } else {
                    printf("[FILE] %s (%.11s) - %u bytes - Cluster: %u\n", 
                        (strlen(long_name) > 0) ? long_name : entry->name, 
                        entry->name, 
                        entry->size, 
                        starting_cluster);
                }
            }

        }

        // Avança para o próximo cluster (usando a FAT)
        uint32_t fat_entry_address = fat_offset + cluster * 4;
        fseek(image, fat_entry_address, SEEK_SET);
        fread(&cluster, sizeof(uint32_t), 1, image);
        cluster &= 0x0FFFFFFF;
    }
}


