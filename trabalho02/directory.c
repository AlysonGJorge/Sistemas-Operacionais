#include "./headers/directory.h"
#include "./headers/coversion.h"

#define SECTOR_SIZE 512 // Tamanho do setor
#define DIR_ENTRY_SIZE 32 // Tamanho da entrada de diretório
#define ATTR_DIRECTORY 0x10 // Atributo para diretório
#define EOC 0x0FFFFFF8


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

void format_to_83_name(const char *name, uint8_t *dest){
    memset(dest, ' ', 11);
    const char *dot = strchr(name, '.');
    int base_len = dot ? (dot - name) : strlen(name);
    int ext_len = dot ? strlen(dot + 1) : 0;

    for (int i = 0; i < base_len; i++) {
        dest[i] = toupper(name[i]);
    }

    if (dot) {
        for (int i = 0;  i < ext_len; i++) {
            dest[8 + i] = toupper(dot[i + 1]);
        }
    }
}

uint32_t find_free_cluster(FILE *image, uint32_t fat_offset, uint32_t total_clusters){
    uint32_t fat_entry;
    for (uint32_t i = 2; i < total_clusters; i++) {
        fseek(image, fat_offset + i * 4, SEEK_SET);
        fread(&fat_entry, 4, 1, image);
        fat_entry = le32toh(fat_entry) & 0x0FFFFFFF;
        if (fat_entry == 0x00000000) return i;
    }
    return 0;
}


int create_directory(FILE *image, uint32_t parent_cluster,char* directory_name, uint32_t bytes_per_sector, 
    uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, uint32_t total_clusters) {

    uint32_t cluster_size = bytes_per_sector * sectors_per_cluster;
    uint8_t *buffer = malloc(cluster_size);
    DirectoryEntry *entry = NULL;
    int found = 0;
    uint32_t cluster_address;
    uint32_t current_cluster = parent_cluster;

    while (current_cluster < EOC && !found) {
        cluster_address = data_offset + (current_cluster - 2) * cluster_size;
        printf("Cluster address: %u\n", cluster_address);

        if (fseek(image, cluster_address, SEEK_SET) != 0) {
            perror("Erro ao posicionar ponteiro para o cluster");
            free(buffer);
            return -1;
        }

        size_t bytes_read = fread(buffer, 1, cluster_size, image);

        if (bytes_read != cluster_size) {
            perror("Erro ao ler o diretório");
            printf("Bytes lidos: %lu\n", bytes_read);
            printf("Cluster size: %u\n", cluster_size);	
            free(buffer);
            return -1;
        }

        for (int i = 0; i < cluster_size; i += sizeof(DirectoryEntry)) {
            entry = (DirectoryEntry *) (buffer + i);
            printf("Entry #%ld: %02X (decimal: %d)\n", i / sizeof(DirectoryEntry), (uint8_t)entry->name[0], (uint8_t)entry->name[0]);

            if ((uint8_t)entry->name[0] == 0x00 || (uint8_t)entry->name[0] == 0xE5) {
                printf("Achei uma entrada vazia ou apagada\n");
                found = 1;
                break;
            }
        }

        if (!found) {
            uint32_t next_cluster;
            fseek(image, fat_offset + current_cluster * 4, SEEK_SET);
            fread(&next_cluster, 4, 1, image);
            current_cluster = le32toh(next_cluster) & 0x0FFFFFFF;
        }
    }

    if (!found) {
        printf("Num achei\n");
        free(buffer);
        return -1;
    }

    uint32_t new_cluster = find_free_cluster(image, fat_offset, total_clusters);
    if (new_cluster == 0) {
        free(buffer);
        return -2;
    }

    uint32_t fat_entry = htole32(0x0FFFFFF8);
    fseek(image, fat_offset + new_cluster * 4, SEEK_SET);
    fwrite(&fat_entry, 4, 1, image);

    uint8_t *new_dir_buffer = calloc(cluster_size, 1);
    DirectoryEntry *dot = (DirectoryEntry *) new_dir_buffer;
    DirectoryEntry *ddot = (DirectoryEntry *) (new_dir_buffer + 32);


    format_to_83_name(".", dot->name);
    dot->attr = 0x10;
    dot->start_high = htole16((new_cluster >> 16) &0xFFFF);
    dot->start_low = htole16((new_cluster & 0xFFFF));
    dot->size = 0;


    format_to_83_name("..", ddot->name);
    ddot->attr = 0x10; // Diretório
    ddot->start_high = htole16((parent_cluster >> 16) & 0xFFFF);
    ddot->start_low = htole16(parent_cluster & 0xFFFF);
    ddot->size = 0;

    uint32_t new_cluster_addr = data_offset + (new_cluster - 2) * cluster_size;
    fseek(image, new_cluster_addr, SEEK_SET);
    fwrite(new_dir_buffer, cluster_size, 1, image);

    format_to_83_name(directory_name, entry->name);
    entry->attr = 0x10;
    entry->start_high = htole16((new_cluster >> 16) & 0xFFFF);
    entry->start_low = htole16(new_cluster & 0xFFFF);
    entry->size = 0;

    fseek(image, cluster_address + ((uint8_t *)entry - buffer), SEEK_SET);
    fwrite(entry, sizeof(DirectoryEntry), 1, image);

    free(buffer);
    free(new_dir_buffer);
    return 0;
} 
