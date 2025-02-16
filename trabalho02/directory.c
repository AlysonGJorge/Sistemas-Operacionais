#include "directory.h" // Para wchar_t e funções relacionadas

int my_strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        int c1 = tolower((unsigned char)*s1);
        int c2 = tolower((unsigned char)*s2);
        if (c1 != c2)
            return c1 - c2;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

int create_directory_entry(FILE *image, uint32_t dir_cluster, uint32_t data_offset,
                            uint32_t cluster_size, uint32_t fat_ofsset, const char *filename,
                            uint8_t attr, uint32_t size, uint32_t *start_cluster) { 

    uint8_t *buffer = malloc(cluster_size);
    if (!buffer) {
        return 0;
    }

    uint32_t cluster_address = data_offset = (dir_cluster - 2) * cluster_size;
    fseek(image, cluster_address, SEEK_SET);
    fwrite(buffer, cluster_size, 1, image);

    int entry_created = 0;
    for (uint32_t offset = 0; offset < cluster_size; offset += sizeof(DirectoryEntry)) {
        DirectoryEntry *entry = (DirectoryEntry *)(buffer + offset);
        if ((uint8_t)entry->name[0] == EMPTY_ENTRY || (uint8_t)entry->name[0] == DELETED_ENTRY) {
            memset(entry, 0, sizeof(DirectoryEntry));
            
            char formatted[12];
            memset(formatted, ' ', 11);
            formatted[11] = '\0';
            int len = strlen(filename);
            int dot = -1;
            for (int i = 0; i < len; i++) {
                if (filename[i] == '.') {
                    dot = i;
                    break;
                }
            }
            if (dot == -1) {
                for (int i = 0; i  < len && i < 8; i++) {
                    formatted[i] = toupper(filename[i]);
                }
            } else {
                for (int i = 0; i < len && i < 8; i++) {
                    formatted[i] = toupper(filename[i]);
                }
                
                for (int i = 0; i < len - dot - 1 && i < 3; i++) {
                    formatted[8 + i] = toupper(filename[dot + 1 + i]);
                }

            }

            memcpy(entry->name, formatted, 11);
            entry->attr = attr;
            entry->size = size;
            entry->start_high = (uint16_t)(*start_cluster >> 16);
            entry->start_low = (uint16_t)(*start_cluster & 0xFFFF);

            fseek(image, cluster_address, SEEK_SET);
            fwrite(buffer, cluster_size, 1, image);
            entry_created = 1;
            break;

        }
    }
    free(buffer);
    return entry_created;
}

int find_entry_in_directory(FILE *image, uint32_t dir_cluster, uint32_t data_offset,
                            uint32_t cluster_size, const char *name, uint8_t req_attr,
                            uint32_t *start_cluster, uint32_t *size) {
    uint8_t *buffer = malloc(cluster_size);
    if (!buffer) {
        return 0;
    }

    uint32_t current_cluster = dir_cluster;
    int found = 0;
    uint32_t cluster_address  = data_offset + (current_cluster - 2) * cluster_size;
    fseek(image, cluster_address, SEEK_SET);
    fread(buffer, cluster_size, 1, image);

    LFNEntry lfn_entries[20];
    int lfn_count = 0;

    for (uint32_t offset = 0; offset < cluster_size; offset += sizeof(DirectoryEntry)) {
        DirectoryEntry *entry = (DirectoryEntry *)(buffer + offset);

        if((uint8_t)entry->name[0] == EMPTY_ENTRY) {
            break;
        }

        if ((uint8_t)entry->name[0] == DELETED_ENTRY) {
            lfn_count = 0;
            continue;
        }

        if (entry->attr == ATTR_LFN) {
            LFNEntry *lfn = (LFNEntry *)entry;
            if (lfn_count < 20) {
                lfn_entries[lfn_count++] = *lfn;
                continue;
            }
        }

        char entry_name[256] = {0};
        if (lfn_count > 0) {
            reconstruct_long_name(entry_name, lfn_entries, lfn_count);
            lfn_count = 0;
        } else {
            format_filename(entry->name, entry_name);
        }

        if (my_strcasecmp(entry_name, name) == 0) {
            if (req_attr == 0 || entry->attr == req_attr) {
                *start_cluster = (entry->start_high << 16) | entry->start_low;
                *size = entry->size;
                found = 1;
                break;
            }
        }
    }
    free(buffer);
    return found;
}


int is_directory_empty(FILE *image, uint32_t start_cluster, uint32_t fat_offset, uint32_t data_offset, uint32_t cluster_size) {
    uint8_t *buffer = malloc(cluster_size);
    if (!buffer) {
        perror("Erro ao alocar memória");
        return -1;
    }

    uint32_t cluster = start_cluster;
    while (cluster < END_OF_CLUSTER) {
        uint32_t cluster_address = data_offset + (cluster - 2) * cluster_size;
        fseek(image, cluster_address, SEEK_SET);
        fread(buffer, cluster_size, 1, image);

        for (int i = 0; i < cluster_size; i += sizeof(DirectoryEntry)) {
            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

            // Se encontramos uma entrada vazia, podemos assumir que não há mais itens válidos
            if ((uint8_t)entry->name[0] == EMPTY_ENTRY) {
                break;
            }
            // Ignorar entradas deletadas
            if ((uint8_t)entry->name[0] == DELETED_ENTRY) {
                continue;
            }
            // Caso seja uma entrada LFN, a pular
            if (entry->attr == ATTR_LFN) {
                continue;
            }
            // Ignorar as entradas especiais "." e ".."
            if ((strncmp(entry->name, ".          ", 11) == 0) ||
                (strncmp(entry->name, "..         ", 11) == 0)) {
                continue;
            }
            // Se houver qualquer outra entrada, o diretório não está vazio
            free(buffer);
            return 0;
        }

        // Avançar para o próximo cluster
  
        uint32_t next_cluster;
        fseek(image, fat_offset + cluster * 4, SEEK_SET);
        fread(&next_cluster, sizeof(uint32_t), 1, image);
        cluster = next_cluster & 0x0FFFFFFF;
        if (cluster >= END_OF_CLUSTER) {
            break;
        }
    }
    
    free(buffer);
    return 1; // Diretório está vazio
}


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

void format_filename(const char *raw, char *formatted) {
    char name[9] = {0}, ext[4] = {0};
    memcpy(name, raw, 8);
    memcpy(ext, raw + 8, 3);

    for (int i = 7; i >= 0; i--) {
        if (name[i] == ' ') {
            name[i] = '\0';
        } else {
            break;
        }
    }

    for (int i = 2; i >= 0; i--) {
        if (name[i] == ' ') {
            name[i] = '\0';
        } else {
            break;
        }
    }

    if (strlen(ext) > 0) {
        sprintf(formatted, "%s.%s", name, ext);
    } else {
        sprintf(formatted, "%s", name);
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
           printf("Entrada de 32 bytes em hexadecimal:\n");
           print_hex(entry_bytes, sizeof(DirectoryEntry));
            //printf("\n");
            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

            if (entry->name[0] == 0x00) // Entrada vazia
                break;

            if ((uint8_t)entry->name[0] == 0xE5) // Entrada deletada
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

// Função para exibir atributos
void display_attributes(uint8_t attributes) {
    printf("Atributos: ");
    if (attributes & 0x01) printf("Somente Leitura ");
    if (attributes & 0x02) printf("Oculto ");
    if (attributes & 0x04) printf("Sistema ");
    if (attributes & 0x08) printf("Volume Label ");
    if (attributes & 0x10) printf("Diretório ");
    if (attributes & 0x20) printf("Arquivo Arquivável ");
    printf("\n");
}



