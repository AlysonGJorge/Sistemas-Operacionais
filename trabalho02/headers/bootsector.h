#ifndef BOOTSECTOR_H
#define BOOTSECTOR_H

#include <stdint.h>

// Estrutura do Bootsector (reutilize a definição anterior)
#pragma pack(push, 1)
typedef struct {
    // Bytes 0x00-0x02: Jump code
    uint8_t jump_code[3];

    // Bytes 0x03-0x0A: Nome do fabricante
    char oem_name[8];

    // Bytes 0x0B-0x0C: Bytes por setor
    uint16_t bytes_per_sector;

    // Bytes 0x0D: Setores por cluster
    uint8_t sectors_per_cluster;

    // Bytes 0x0E-0x0F: Setores reservados
    uint16_t reserved_sectors;

    // Bytes 0x10: Número de FATs
    uint8_t num_fats;

    // Bytes 0x11-0x12: Entradas de diretório
    uint16_t root_entries_count;

    // Bytes 0x13-0x14: Total de setores (16 bits)
    uint16_t total_sectors_16;

    // Bytes 0x15: Tipo de mídia
    uint8_t media_descriptor;
    
    // Bytes 0x16-0x17: Fat size (16 bits) - Não utilizado no FAT32
    uint16_t fat_size_16;

    // Bytes 0x18-0x19: Setores por trilha
    uint16_t sectors_per_track;

    // Bytes 0x1A-0x1B: Número de cabeças
    uint16_t num_heads;

    // Bytes 0x1C-0x1F: Setores ocultos
    uint32_t hidden_sectors;

    // Bytes 0x20-0x23: Total de setores (32 bits)
    uint32_t total_sectors_32;

    // Bytes 0x24-0x27: Fat size (32 bits)
    uint32_t fat_size_32;

    // Bytes 0x28-0x29: Flags
    uint16_t flags;

    // Bytes 0x2A-0x2B: Versão do FAT
    uint16_t version;

    // Bytes 0x2C-0x2F: Cluster do diretório raiz
    uint32_t root_cluster;

    // Bytes 0x30-0x31: Setor de informações do FS
    uint16_t fs_info_sector;

    // Bytes 0x32-0x33: Setor de backup do boot
    uint16_t backup_boot_sector;

    // Bytes 0x34-0x3D: Reservado
    uint8_t reserved[12];

    // Bytes 0x3E: Drive number
    uint8_t drive_number;

    // Bytes 0x3F: Reserved
    uint8_t reserved_nt_flags;


    // Bytes 0x40: Assinatura extendida
    uint8_t signature;

    // Bytes 0x41-0x44: Volume ID
    uint32_t volume_id;

    // Bytes 0x45-0x51: Volume label
    char volume_label[11];

    // Bytes 0x52-0x59: Tipo de sistema de arquivos
    char fs_type[8];
    
    // Bytes 0x5A-0x1FD: Boot code
    uint8_t boot_code[410];

    // Bytes 0x1FE-0x1FF: Assinatura do setor de boot
    uint16_t boot_sector_signature;
} BootSector;
#pragma pack(pop)

int read_bootsector(const char *filename, BootSector *bs);
void print_bootsector_info(const BootSector *bs);

#endif // BOOTSECTOR_H