#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <ctype.h>

// Estrutura para uma entrada de diretório
typedef struct __attribute__((packed)) {
    uint8_t name[8];
    uint8_t ext[3];
    uint8_t attr;
    uint8_t ntres;
    uint8_t crt_time_tenth;
    uint16_t crt_time;
    uint16_t crt_date;
    uint16_t lst_acc_date;
    uint16_t start_high;
    uint16_t wrt_time;
    uint16_t wrt_date;
    uint16_t start_low;
    uint32_t size;
} DirectoryEntry;

typedef struct __attribute__((packed)) {
    uint8_t order;
    uint16_t name1[5];
    uint8_t attr;
    uint8_t type;
    uint8_t checksum;
    uint16_t name2[6];
    uint16_t zero;
    uint16_t name3[2];
} LFNEntry;

void read_directory(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset);

void extract_lfn_characters(LFNEntry *lfn_entry, char *long_name, int *index);

void reconstruct_long_name(char *long_name, LFNEntry *lfn_entries, int total_entries);

void format_to_83_name(const char *name, uint8_t *dest);

uint32_t find_free_cluster(FILE *image, uint32_t fat_offset, uint32_t total_clusters);

int create_directory(FILE *image, uint32_t parent_cluster,char *directory_name, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, uint32_t total_clusters);

# endif // DIRECTORY_H