#include "comandos.h"

//-----------------------------------------------------------------------------------------------------//
/*Info*/

void gather_disk_info(FILE *image, const BootSector *bs, DiskInfo *info) {
    info->total_sectors = bs->total_sectors_32 ? bs->total_sectors_32 : bs->total_sectors_16;
    info->fat_size_sectors = bs->fat_size_32;
    info->cluster_size_bytes = bs->bytes_per_sector * bs->sectors_per_cluster;
    info->root_cluster = bs->root_cluster;
    info->fat_offset = bs->reserved_sectors * bs->bytes_per_sector;
    info->data_offset = info->fat_offset + bs->num_fats * info->fat_size_sectors * bs->bytes_per_sector;
    info->total_clusters = (info->total_sectors - (info->data_offset / bs->bytes_per_sector)) / bs->sectors_per_cluster;

    // Inicializa contadores de clusters
    info->free_clusters = 0;
    info->used_clusters = 0;
    info->reserved_clusters = 0;

    // Lê a FAT para determinar clusters livres, usados e reservados
    uint32_t cluster_status;
    fseek(image, info->fat_offset, SEEK_SET);

    for (uint32_t i = 2; i < info->total_clusters + 2; i++) { // Clusters válidos começam no índice 2
        fread(&cluster_status, sizeof(uint32_t), 1, image);
        cluster_status &= 0x0FFFFFFF;

        if (cluster_status == 0x00000000) {
            info->free_clusters++;
        } else if (cluster_status >= 0x0FFFFFF8) {
            info->reserved_clusters++;
        } else {
            info->used_clusters++;
        }
    }
}

void display_disk_info(const DiskInfo *info) {
    printf("===== Informações do Disco =====\n");
    printf("Total de Setores: %u\n", info->total_sectors);
    printf("Tamanho da FAT (em setores): %u\n", info->fat_size_sectors);
    printf("Tamanho do Cluster (em bytes): %u\n", info->cluster_size_bytes);
    printf("Cluster Inicial do Root Directory: %u\n", info->root_cluster);
    printf("Offset da FAT: 0x%X\n", info->fat_offset);
    printf("Offset da Data Region: 0x%X\n", info->data_offset);
    printf("Total de Clusters: %u\n", info->total_clusters);
    printf("Clusters Livres: %u\n", info->free_clusters);
    printf("Clusters Ocupados: %u\n", info->used_clusters);
    printf("Clusters Reservados: %u\n", info->reserved_clusters);
}

void info_teste( FILE *file, const BootSector *bs){
    DiskInfo info;
    gather_disk_info(file, bs, &info);
    display_disk_info(&info);
}
//-----------------------------------------------------------------------------------------------------//
/*Info*/

//-----------------------------------------------------------------------------------------------------//
/*cluster*/
void display_cluster_content(FILE *image, const BootSector *bs, uint32_t cluster_num) {
    DiskInfo info;
    gather_disk_info(image, bs, &info);

    if (cluster_num < 2 || cluster_num >= info.total_clusters + 2) {
        printf("Cluster inválido: %u. Os clusters válidos estão entre 2 e %u.\n",
               cluster_num, info.total_clusters + 1);
        return;
    }

    // Calcula o offset do cluster na Data Region
    uint32_t cluster_offset = info.data_offset + 
                              (cluster_num - 2) * bs->sectors_per_cluster * bs->bytes_per_sector;

    fseek(image, cluster_offset, SEEK_SET);

    // Lê o conteúdo do cluster
    uint8_t *buffer = malloc(info.cluster_size_bytes);
    if (!buffer) {
        perror("Erro ao alocar memória para o buffer");
        return;
    }

    if (fread(buffer, 1, info.cluster_size_bytes, image) != info.cluster_size_bytes) {
        perror("Erro ao ler o cluster");
        free(buffer);
        return;
    }

    printf("===== Conteúdo do Cluster %u =====\n", cluster_num);
    for (uint32_t i = 0; i < info.cluster_size_bytes; i++) {
        if (buffer[i] == '\0') {
            // Exibe espaços para bytes nulos
            putchar('.');
        } else {
            putchar(buffer[i]);
        }
    }
    putchar('\n');

    free(buffer);
}
//-----------------------------------------------------------------------------------------------------//
/*cluster*/

//-----------------------------------------------------------------------------------------------------//
/*pwd*/
void pwd(char* current_path){
    printf("%s\n",current_path );
}
//-----------------------------------------------------------------------------------------------------//
/*pwd*/

//-----------------------------------------------------------------------------------------------------//
/*attr*/
// Função auxiliar para converter data FAT32 em formato legível
void decode_fat_date(uint16_t date, int *year, int *month, int *day) {
    *year = ((date >> 9) & 0x7F) + 1980;  // Bits 9-15 (7 bits) → Ano desde 1980
    *month = (date >> 5) & 0x0F;          // Bits 5-8 (4 bits) → Mês (1-12)
    *day = date & 0x1F;                   // Bits 0-4 (5 bits) → Dia (1-31)
}

// Função auxiliar para converter hora FAT32 em formato legível
void decode_fat_time(uint16_t time, int *hour, int *minute, int *second) {
    *hour = (time >> 11) & 0x1F;           // Bits 11-15 (5 bits) → Hora (0-23)
    *minute = (time >> 5) & 0x3F;          // Bits 5-10 (6 bits) → Minuto (0-59)
    *second = (time & 0x1F) * 2;           // Bits 0-4 (5 bits) → Segundo (0-58, multiplica por 2)
}

// Função para imprimir os atributos do arquivo/diretório
void print_attributes(DirectoryEntry *entry, char * longName) {
    int year, month, day, hour, minute, second;
    
    // Nome do arquivo/diretório
    printf("Nome: %s\n", longName);
    printf("Nome Short: %.11s\n", entry->name);
    
    // Exibe atributos específicos
    printf("Atributos: ");
    if (entry->attr & 0x01) printf("Somente Leitura ");
    if (entry->attr & 0x02) printf("Oculto ");
    if (entry->attr & 0x04) printf("Sistema ");
    if (entry->attr & 0x08) printf("Volume ");
    if (entry->attr & 0x10) printf("Diretório ");
    if (entry->attr & 0x20) printf("Arquivo ");

    printf("\n");

    // Data e hora de criação
    decode_fat_date(entry->date, &year, &month, &day);
    decode_fat_time(entry->time, &hour, &minute, &second);
    printf("Criado em: %02d/%02d/%04d %02d:%02d:%02d\n", day, month, year, hour, minute, second);

    // Data de última modificação
    decode_fat_date(entry->date_mod, &year, &month, &day);
    decode_fat_time(entry->time_mod, &hour, &minute, &second);
    printf("Última Modificação: %02d/%02d/%04d %02d:%02d:%02d\n", day, month, year, hour, minute, second);

    // Data de último acesso
    decode_fat_date(entry->accessDate, &year, &month, &day);
    printf("Último Acesso: %02d/%02d/%04d\n", day, month, year);

    // Tamanho do arquivo
    printf("Tamanho do Arquivo: %u bytes\n", entry->size);

    // Cluster inicial (High + Low)
    uint32_t cluster = (entry->start_high << 16) | entry->start_low;
    printf("Cluster Inicial: %u\n", cluster);
}




void attr(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * nmArquivo){
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

            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

            if (entry->name[0] == 0x00) // Entrada vazia
                break;

            if (entry->name[0] == 0xE5) // Entrada deletada
                continue;
            if (entry->attr == 0x0F) { // Verifica se é uma entrada LFN
                LFNEntry *lfn_entry = (LFNEntry *)(entry_bytes);
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
                    if(!strcmp(long_name, nmArquivo)){
                        print_attributes(entry, long_name);
                        return;
                    }
                }
            }
        }
        // Avança para o próximo cluster (usando a FAT)
        uint32_t fat_entry_address = fat_offset + cluster * 4;
        fseek(image, fat_entry_address, SEEK_SET);
        fread(&cluster, sizeof(uint32_t), 1, image);
        cluster &= 0x0FFFFFFF;
    }
    printf ("Arquivo %s não Encontrado\n", nmArquivo);
}
//-----------------------------------------------------------------------------------------------------//
/*attr*/

//-----------------------------------------------------------------------------------------------------//
/*ls*/
void ls(FILE *image, uint32_t cluster_path, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset) {
    uint32_t cluster = cluster_path;
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
           //printf("Entrada de 32 bytes em hexadecimal:\n");
           //print_hex(entry_bytes, sizeof(DirectoryEntry));
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
//-----------------------------------------------------------------------------------------------------//
/*ls*/

//-----------------------------------------------------------------------------------------------------//
/*cd*/


void cd(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * nmArquivo, uint32_t * current_cluster, char * current_path, char * last_path){
    uint32_t cluster = root_cluster;
    uint32_t cluster_size = bytes_per_sector * sectors_per_cluster;
    uint8_t buffer[SECTOR_SIZE];
    int tam_string;

    LFNEntry lfn_buffer[20];
    int lfn_index = 0;
    while (cluster < 0x0FFFFFF8) { // Clusters válidos no FAT32
        uint32_t cluster_address = data_offset + (cluster - 2) * cluster_size;
        fseek(image, cluster_address, SEEK_SET);
        fread(buffer, cluster_size, 1, image);

        for (int i = 0; i < cluster_size; i += sizeof(DirectoryEntry)) {
             uint8_t *entry_bytes = buffer + i;

            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

            if (entry->name[0] == 0x00) // Entrada vazia
                break;

            if (entry->name[0] == 0xE5) // Entrada deletada
                continue;
            if (entry->attr == 0x0F) { // Verifica se é uma entrada LFN
                LFNEntry *lfn_entry = (LFNEntry *)(entry_bytes);
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
                    if(!strcmp(long_name, nmArquivo)){
                        if(entry->attr & ATTR_DIRECTORY){
                            *current_cluster = (entry->start_high << 16) | entry->start_low;
                            strcpy(last_path, current_path);
                            strcat(current_path, long_name);
                            strcat(current_path, "/");
                            //print_attributes(entry, long_name);
                        }else
                            printf ("Não é um diretório \n");
                        return;
                    }
                }else{
                    strcpy(entry->name, strtok(entry->name," "));
                    if(!strcmp(entry->name, nmArquivo)){
                        if(entry->attr & ATTR_DIRECTORY){
                            if(!strcmp(entry->name, "..")){
                                *current_cluster = (entry->start_high << 16) | entry->start_low;
                                if((int) *current_cluster == 0) *current_cluster = 2;
                                //strcat(current_path, "/");
                                strcpy(current_path, last_path);
                                printf("Entrou aqui 1: %s  \n", last_path );
                                tam_string = strlen(last_path);
                                last_path[tam_string-1] = '\0';
                                printf("Entrou aqui 2: %s  \n", last_path );
                                for (int i = (tam_string-2); i >= 0; i--)
                                {
                                    printf("Entrou aqui 3: %c  \n", last_path[i] );
                                    if(last_path[i] == '/'){
                                        last_path[i+1] = '\0';
                                        printf("Entrou aqui 4: %s  \n", last_path );
                                        return;
                                    } 
                                }
                            }else{
                                *current_cluster = (entry->start_high << 16) | entry->start_low;
                                strcat(current_path, long_name);
                                strcat(current_path, "/");
                                 return;      
                            }
                            //print_attributes(entry, long_name);
                        }else
                            printf ("Não é um diretório \n");
                        return;
                    }
                }
            }
        }
        // Avança para o próximo cluster (usando a FAT)
        uint32_t fat_entry_address = fat_offset + cluster * 4;
        fseek(image, fat_entry_address, SEEK_SET);
        fread(&cluster, sizeof(uint32_t), 1, image);
        cluster &= 0x0FFFFFFF;
    }
    printf ("Arquivo %s não Encontrado\n", nmArquivo);
}
//-----------------------------------------------------------------------------------------------------//
/*cd*/

//-----------------------------------------------------------------------------------------------------//
/*touch*/


void touch(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * nmArquivo){
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

            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

            if (entry->name[0] == 0x00) // Entrada vazia
                break;

            if (entry->name[0] == 0xE5) // Entrada deletada
                continue;
            if (entry->attr == 0x0F) { // Verifica se é uma entrada LFN
                LFNEntry *lfn_entry = (LFNEntry *)(entry_bytes);
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
                    if(!strcmp(long_name, nmArquivo)){
                        printf ("Arquivo encontrado com o mesmo nome : %s\n", nmArquivo);
                        return;
                    }
                }
            }
        }
        // Avança para o próximo cluster (usando a FAT)
        uint32_t fat_entry_address = fat_offset + cluster * 4;
        fseek(image, fat_entry_address, SEEK_SET);
        fread(&cluster, sizeof(uint32_t), 1, image);
        cluster &= 0x0FFFFFFF;
    }

    cluster = root_cluster;
    cluster_size = bytes_per_sector * sectors_per_cluster;
    buffer[SECTOR_SIZE];


    int tamNome = strlen(nmArquivo);
    int qtdEntradas = tamNome/13;
    if (tamNome % 13) qtdEntradas ++;
    qtdEntradas ++;
    uint32_t cluster_address_inicial;
    int qtdEspacosDisponiveis = 0;
     printf("Quantidade entradas: %d\n", qtdEntradas);

    while (cluster < 0x0FFFFFF8 && qtdEspacosDisponiveis != qtdEntradas) { // Clusters válidos no FAT32
        uint32_t cluster_address = data_offset + (cluster - 2) * cluster_size;
        fseek(image, cluster_address, SEEK_SET);
        printf("cluster entrada: %ld\n", (long int) cluster_address);
        fread(buffer, cluster_size, 1, image);
        printf("passou aqui");
        for (int i = 0; i < cluster_size; i += sizeof(DirectoryEntry)) {
             uint8_t *entry_bytes = buffer + i;

            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

            if (entry->name[0] == 0x00 || entry->name[0] == 0xE5){ // Entrada vazia
                qtdEspacosDisponiveis ++;
                if (qtdEspacosDisponiveis == qtdEntradas)  break;
                cluster_address_inicial = cluster_address + i;
            }else{
                cluster_address_inicial = 0;
                qtdEspacosDisponiveis = 0;
            }
    
        }
        //cluster_address_inicial = 0;
        //qtdEspacosDisponiveis = 0;
        // Avança para o próximo cluster (usando a FAT)
        
        uint32_t fat_entry_address = fat_offset + cluster * 4;
        fseek(image, fat_entry_address, SEEK_SET);
        fread(&cluster, sizeof(uint32_t), 1, image);
        cluster &= 0x0FFFFFFF;
    }

    ///Caso não tenha espaço criar novo cluster e relacionar com o diretorio

     if (qtdEspacosDisponiveis != qtdEntradas) {
        printf("Erro: Espaço insuficiente para criar o arquivo.\n");
        return;
    }

    /// gerar o SFN
    DirectoryEntry sfn;
    

    /// Gerar Chechsum

    /// Gerar LFN

    /// Salvar


}



//-----------------------------------------------------------------------------------------------------//
/*touch*/