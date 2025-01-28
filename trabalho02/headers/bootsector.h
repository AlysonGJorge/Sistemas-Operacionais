#ifndef BOOTSECTOR_H
#define BOOTSECTOR_H

#include <stdint.h>

#pragma pack(push, 1)
// Estrutura do Bootsector (reutilize a definição anterior)
typedef struct {
    uint8_t jump_code[3];
    char oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t num_fats;
    uint16_t total_sectors_16;
    uint32_t total_sectors_32;
    uint32_t fat_size_32;
    uint32_t root_cluster;
    char volume_label[11];
    char fs_type[8];
    uint16_t boot_sector_signature;
} BootSector;
#pragma pack(pop)

int read_bootsector(const char *filename, BootSector *bs);
void print_bootsector_info(const BootSector *bs);

#endif // BOOTSECTOR_H