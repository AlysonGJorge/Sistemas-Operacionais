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


#include <stdio.h>
#include <stdint.h>

void print_lfn_entry_hex(const LFNEntry *entry) {
    printf("LFN Entry:\n");
    printf("Order: 0x%02X\n", entry->order);

    printf("Name1: ");
    for (int i = 0; i < 5; i++) {
        printf("0x%04X ", entry->name1[i]);
    }
    printf("\n");

    printf("Attribute: 0x%02X\n", entry->attr);
    printf("Type: 0x%02X\n", entry->type);
    printf("Checksum: 0x%02X\n", entry->checksum);

    printf("Name2: ");
    for (int i = 0; i < 6; i++) {
        printf("0x%04X ", entry->name2[i]);
    }
    printf("\n");

    printf("First Cluster: 0x%04X\n", entry->first_cluster);

    printf("Name3: ");
    for (int i = 0; i < 2; i++) {
        printf("0x%04X ", entry->name3[i]);
    }
    printf("\n");
}



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
        //print_lfn_entry_hex(&lfn_entries[i]);
        extract_lfn_characters(&lfn_entries[i], long_name, &index);
    }
    long_name[index] = '\0'; // Finaliza a string
}

void print_hex(const uint8_t *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", data[i]);
        if ((i + 1) % 16 == 0) { // Quebra de linha a cada 16 bytes
            printf("\n");
        }
    }
    if (size % 16 != 0) { // Quebra de linha no final, se necessário
        printf("\n");
    }
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
             uint8_t *entry_bytes = buffer + i;

            // Exibe a entrada em formato hexadecimal
           // printf("Entrada de 32 bytes em hexadecimal:\n");
           // print_hex(entry_bytes, sizeof(DirectoryEntry));
            //printf("\n");
            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

            if (entry->name[0] == 0x00) // Entrada vazia
                break;

            if (entry->name[0] == 0xE5) // Entrada deletada
                continue;
            if (entry->attr == 0x0F) { // Verifica se é uma entrada LFN
                LFNEntry *lfn_entry = (LFNEntry *)(entry_bytes);
               //printf("Name1[0]-> %04X -- SIZE: %ld\n",lfn_entry->name1[0], sizeof(LFNEntry));
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


