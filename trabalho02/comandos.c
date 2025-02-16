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
    uint8_t *buffer = malloc(cluster_size);
    if(!buffer){
        perror("Erro ao alocar buffer para leitura do cluster");
        return;
    }

    // printf("Cluster_path: %d\n", (int) cluster_path);
    // printf("Fat_offset: %d\n", (int) fat_offset);
    // printf("Data_offset: %d\n", (int) data_offset);

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
           //print_hex(entry_bytes, sizeof(DirectoryEntry));
           //printf("\n");
            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

            //printf ("Entrou aquiteste: %02X \n", (uint8_t) entry->name[0]);

            if ((uint8_t) entry->name[0] == 0x00) // Entrada vazia
                break;

            if ((uint8_t) entry->name[0] == 0xE5) // Entrada deletada
                continue;
            if ( entry->attr == 0x0F) { // Verifica se é uma entrada LFN
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

                if(entry->attr & ATTR_VOLUME_ID)
                    continue;

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
void generate_sfn(const char *long_name, char *sfn, uint32_t file_index) {
    char name_part[8] = {0}; // 8 caracteres + null terminador
    char ext_part[3] = {0};  // 3 caracteres + null terminador
    int name_len = 0, ext_len = 0;
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    srand(time(NULL));
    // Encontra a última ocorrência de '.' para separar nome e extensão
    const char *dot = strrchr(long_name, '.');
    if (dot) {
        name_len = dot - long_name; // Tamanho do nome antes do '.'
        ext_len = strlen(dot + 1);  // Tamanho da extensão
    } else {
        name_len = strlen(long_name);
    }

    // Copia e ajusta a parte do nome (máximo 8 caracteres)
    int j = 0;
    if(name_len > 8){
        for (int i = 0; i < 6; i++) {
            name_part[i] = (char) charset[rand() % (sizeof(charset) - 1)];
        }
        name_part[6] = '~';
        name_part[7] = '1';
    }else{
        for (int i = 0; i < name_len; i++) {
            if (isalnum(long_name[i])) {
                name_part[i] = toupper(long_name[i]);
            }
        }
        for (int i = name_len; i < 8; i++) {
            name_part[i] = ' ';
        }
    }
    

    // Copia e ajusta a parte da extensão (máximo 3 caracteres)
    j = 0;
    if (dot && ext_len > 0) {
        for (int i = 0; i < ext_len && j < 3; i++) {
            if (isalnum(dot[1 + i])) {
                ext_part[j++] = toupper(dot[1 + i]);
            }
        }
    }

    // Se houver conflito, adiciona "~N"
    strncpy(sfn,name_part,8);
    sfn[8]='\0';
    strcat(sfn,ext_part);
}

uint8_t calculate_sfn_checksum(const uint8_t *sfn) {
    uint8_t sum = 0;
    for (int i = 0; i < 11; i++) {
        sum = ((sum >> 1) | (sum << 7)) + sfn[i];
    }
    return sum;
}


uint32_t allocate_new_cluster(FILE *image, uint32_t current_cluster, uint32_t fat_offset, uint32_t num_clusters) {
    // Percorre a FAT para encontrar um cluster livre
    uint32_t new_cluster = 0;
    uint32_t end = EOF;
    for (uint32_t i = 2; i < num_clusters; i++) { // FAT começa no cluster 2
        uint32_t fat_entry;
        fseek(image, fat_offset + i * 4, SEEK_SET);
        fread(&fat_entry, sizeof(uint32_t), 1, image);
        fat_entry &= 0x0FFFFFFF; // Mascara para pegar apenas os 28 bits válidos

        if (fat_entry == 0x00000000) { // Encontrou um espaço livre
            new_cluster = i;
            break;
        }
    }

    if (new_cluster == 0) {
        printf("Erro: Não há espaço livre na FAT para alocar um novo cluster.\n");
        return 0;
    }

    // Atualiza a FAT: cluster atual aponta para o novo
    fseek(image, fat_offset + current_cluster * 4, SEEK_SET);
    fwrite(&new_cluster, sizeof(uint32_t), 1, image);

    // Marca o novo cluster como fim de cadeia (EOC)
    fseek(image, fat_offset + new_cluster * 4, SEEK_SET);
    fwrite(&end, sizeof(uint32_t), 1, image);

    return new_cluster;
}

void clear_cluster(FILE *image, uint32_t cluster, uint32_t data_offset, uint32_t cluster_size) {
    uint8_t *buffer = calloc(cluster_size, 1);  // Aloca e inicializa com zeros
    if (!buffer) {
        fprintf(stderr, "Erro ao alocar buffer para limpar cluster.\n");
        return;
    }

    // Calcula o endereço do cluster dentro da imagem
    uint32_t cluster_address = data_offset + (cluster - 2) * cluster_size;

    // Posiciona e escreve os zeros no cluster
    fseek(image, cluster_address, SEEK_SET);
    fwrite(buffer, cluster_size, 1, image);
    fflush(image);

    free(buffer);
}


void touch(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * nmArquivo, uint32_t tamArquivo, uint32_t clusterInicial, uint32_t num_cluster){
    uint32_t cluster = root_cluster;
    uint32_t cluster_size = bytes_per_sector * sectors_per_cluster;
    uint8_t buffer[SECTOR_SIZE];
    uint32_t lastCluster;
    int qtdEntrada1, qtdEntrada2;
    //printf("Este é o nome do arquivo no touch %s \n ", nmArquivo);
    
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
                    uint32_t found_cluster = (entry->start_high << 16) | entry->start_low;
                    if(!strcmp(long_name, nmArquivo)){
                        if((uint8_t) entry->name[0] != DELETED_ENTRY){
                            printf ("Arquivo encontrado com o mesmo nome : %s\n", nmArquivo);
                            return;
                        }
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
    uint32_t cluster_address_inicial1, cluster_address_inicial2;
    int qtdEspacosDisponiveis = 0;
     printf("Quantidade entradas: %d\n", qtdEntradas);

    while (cluster < 0x0FFFFFF8 && qtdEspacosDisponiveis != qtdEntradas) { // Clusters válidos no FAT32
        uint32_t cluster_address = data_offset + (cluster - 2) * cluster_size;
        fseek(image, cluster_address, SEEK_SET);
        printf("cluster entrada: %ld\n", (long int) cluster_address);
        fread(buffer, cluster_size, 1, image);
        for (int i = 0; i < cluster_size; i += sizeof(DirectoryEntry)) {
             uint8_t *entry_bytes = buffer + i;

            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

            if (entry->name[0] == 0x00 || entry->name[0] == 0xE5){ // Entrada vazia
                if(qtdEspacosDisponiveis == 0) cluster_address_inicial1 = cluster_address + i;
                qtdEspacosDisponiveis ++;
                if (qtdEspacosDisponiveis == qtdEntradas) break;
            }else{
                cluster_address_inicial1 = 0;
                qtdEspacosDisponiveis = 0;
            }
    
        }
        //cluster_address_inicial = 0;
        //qtdEspacosDisponiveis = 0;
        // Avança para o próximo cluster (usando a FAT)
        
        uint32_t fat_entry_address = fat_offset + cluster * 4;
        lastCluster = cluster;
        fseek(image, fat_entry_address, SEEK_SET);
        fread(&cluster, sizeof(uint32_t), 1, image);
        cluster &= 0x0FFFFFFF;
    }

    ///Caso não tenha espaço criar novo cluster e relacionar com o diretorio
    qtdEntrada1 = qtdEspacosDisponiveis;
     if (qtdEspacosDisponiveis != qtdEntradas) {
        uint32_t newCluster = allocate_new_cluster(image, lastCluster, fat_offset, num_cluster);
        if (!newCluster)
        {
            printf("Erro: Espaço insuficiente para criar o arquivo.\n");
            return;
        }
        clear_cluster (image,newCluster,data_offset, cluster_size );
        uint32_t cluster_address = data_offset + (newCluster - 2) * cluster_size;
        fseek(image, cluster_address, SEEK_SET);
        printf("cluster entrada: %ld\n", (long int) cluster_address);
        fread(buffer, cluster_size, 1, image);
        for (int i = 0; i < cluster_size; i += sizeof(DirectoryEntry)) {
             uint8_t *entry_bytes = buffer + i;

            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

            if (entry->name[0] == 0x00 || entry->name[0] == 0xE5){ // Entrada vazia
                if(qtdEspacosDisponiveis == 0) cluster_address_inicial2 = cluster_address + i;
                qtdEspacosDisponiveis ++;
                if (qtdEspacosDisponiveis == qtdEntradas) break;
            }else{
                cluster_address_inicial2 = 0;
                qtdEspacosDisponiveis = qtdEntrada1;
            }
        }
    }
    qtdEntrada2 = qtdEntradas - qtdEntrada1;

    /// gerar o SFN
    DirectoryEntry *sfn = (DirectoryEntry *)malloc(sizeof(DirectoryEntry));
    if (!sfn) {
        perror("Erro ao alocar memória");
        return;
    }
    memset(sfn, 0, sizeof(DirectoryEntry));
    char sfnNome[11];
    generate_sfn(nmArquivo, sfnNome, 0);
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    strncpy(sfn->name, sfnNome,11);
    sfn->attr = ATTR_ARCHIVE;
    sfn->date = ((tm_info->tm_year - 80) << 9) | ((tm_info->tm_mon + 1) << 5) | tm_info->tm_mday;
    sfn->time = (tm_info->tm_hour << 11) | (tm_info->tm_min << 5) | (tm_info->tm_sec / 2);
    sfn->date_mod = sfn->date;
    sfn->time_mod = sfn->time;
    sfn->size = (tamArquivo > 0) ? (uint16_t) tamArquivo: 0;
    if (clusterInicial)
    {
        sfn->start_high = (uint16_t)((clusterInicial >> 16) & 0xFFFF); // Parte alta
        sfn->start_low  = (uint16_t) (clusterInicial & 0xFFFF);
    }
    
    
    

    /// Gerar Chechsum
    uint8_t checksum = calculate_sfn_checksum(sfn->name);

    /// Gerar LFN
    LFNEntry lfn_buffer2[20];
    
    int len = strlen(nmArquivo);
    int index = 0;
    int posBuffer = 0;

    for (int i = qtdEntradas - 1; i > 0; i--) {
        LFNEntry *lfn = (LFNEntry *)malloc(sizeof(LFNEntry));
        memset(lfn, 0, sizeof(LFNEntry));            
        lfn->order = (i + 1) | (i == 1 ? 0x40 : 0x00); // Última entrada tem bit 0x40
        lfn->attr = ATTR_LFN;
        lfn->type = 0;
        lfn->checksum = checksum;
        lfn->first_cluster = 0;
        for (int j = 0; j < 5; j++) {
            if (index < len) {
                uint16_t utf16_char = nmArquivo[index++];
                ((uint16_t*)lfn->name1)[j] = utf16_char;
            } else {
                ((uint16_t*)lfn->name1)[j] = 0; // Espaços vazios em UTF-16
            }
        }
        for (int j = 0; j < 6; j++) {
            if (index < len) {
                uint16_t utf16_char = nmArquivo[index++];
                ((uint16_t*)lfn->name2)[j] = utf16_char;
            } else {
                ((uint16_t*)lfn->name2)[j] = 0; // Espaços vazios em UTF-16
            }
        }
        for (int j = 0; j < 2; j++) {
            if (index < len) {
                uint16_t utf16_char = nmArquivo[index++];
                ((uint16_t*)lfn->name3)[j] = utf16_char;
            } else {
                ((uint16_t*)lfn->name3)[j] = 0; // Espaços vazios em UTF-16
            }
        }
        lfn_buffer2[i-1] = *lfn;
         free(lfn);
    }
    //char teste_nome[256] = {0};
    //reconstruct_long_name(teste_nome, lfn_buffer2, (qtdEntradas-1));
    //printf("Esta aqui a saida: %s\n", teste_nome);
   


    

    uint8_t buffer2[cluster_size];
    memset(buffer2, 0x00, sizeof(buffer2));

    if (qtdEntrada2 == 0){
        for (int i = 0; i < qtdEntradas -1; i++){
            fseek(image, cluster_address_inicial1 + i * sizeof(LFNEntry), SEEK_SET);
            printf("cluster escrita: %ld\n", (long int) (cluster_address_inicial1 + i * sizeof(LFNEntry)));
            memcpy(buffer2, &lfn_buffer2[i], sizeof(LFNEntry));
            print_hex( buffer2, sizeof(LFNEntry));
            fwrite(buffer2, cluster_size, 1, image);
        }

        fseek(image, (cluster_address_inicial1 + (qtdEntradas -1) * sizeof(DirectoryEntry)), SEEK_SET);
        memcpy(buffer2, sfn, sizeof(LFNEntry));
        print_hex( buffer2, sizeof(DirectoryEntry));
        fwrite(buffer2, cluster_size, 1, image);
        fflush(image);
    }else{
        for (int i = 0; i < qtdEntrada1; i++){
            fseek(image, cluster_address_inicial1 + i * sizeof(LFNEntry), SEEK_SET);
            printf("cluster escrita: %ld\n", (long int) (cluster_address_inicial1 + i * sizeof(LFNEntry)));
            memcpy(buffer2, &lfn_buffer2[i], sizeof(LFNEntry));
            print_hex( buffer2, sizeof(LFNEntry));
            fwrite(buffer2, cluster_size, 1, image);
        }

        for (int i = 0; i < (qtdEntrada2 - 1); i++){
            fseek(image, cluster_address_inicial2 + i * sizeof(LFNEntry), SEEK_SET);
            printf("cluster escrita: %ld\n", (long int) (cluster_address_inicial2 + i * sizeof(LFNEntry)));
            memcpy(buffer2, &lfn_buffer2[i], sizeof(LFNEntry));
            print_hex( buffer2, sizeof(LFNEntry));
            fwrite(buffer2, cluster_size, 1, image);
        }

        fseek(image, (cluster_address_inicial2 + (qtdEntrada2 -1) * sizeof(DirectoryEntry)), SEEK_SET);
        memcpy(buffer2, sfn, sizeof(LFNEntry));
        print_hex( buffer2, sizeof(DirectoryEntry));
        fwrite(buffer2, cluster_size, 1, image);
        fflush(image);
    }
    
    

    printf("Arquivo Criado com Sucesso!\n");
    free(sfn);
}
//-----------------------------------------------------------------------------------------------------//
/*touch*/

//-----------------------------------------------------------------------------------------------------//
/*mkdir*/
void mkdir(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * nmArquivo, uint32_t * fat, uint32_t num_clusters){
    uint32_t cluster = root_cluster;
    uint32_t cluster_size = bytes_per_sector * sectors_per_cluster;
    uint8_t buffer[SECTOR_SIZE];
    uint32_t lastCluster;
    int qtdEntrada1, qtdEntrada2;

    
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
                        printf ("Arquivo/Diretório encontrado com o mesmo nome : %s\n", nmArquivo);
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
    uint32_t cluster_address_inicial1, cluster_address_inicial2;
    int qtdEspacosDisponiveis = 0;
     printf("Quantidade entradas: %d\n", qtdEntradas);

    while (cluster < 0x0FFFFFF8 && qtdEspacosDisponiveis != qtdEntradas) { // Clusters válidos no FAT32
        uint32_t cluster_address = data_offset + (cluster - 2) * cluster_size;
        fseek(image, cluster_address, SEEK_SET);
        printf("cluster entrada: %ld\n", (long int) cluster_address);
        fread(buffer, cluster_size, 1, image);
        for (int i = 0; i < cluster_size; i += sizeof(DirectoryEntry)) {
             uint8_t *entry_bytes = buffer + i;

            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

            if (entry->name[0] == 0x00 || entry->name[0] == 0xE5){ // Entrada vazia
                if(qtdEspacosDisponiveis == 0) cluster_address_inicial1 = cluster_address + i;
                qtdEspacosDisponiveis ++;
                if (qtdEspacosDisponiveis == qtdEntradas) break;
            }else{
                cluster_address_inicial1 = 0;
                qtdEspacosDisponiveis = 0;
            }
    
        }
        //cluster_address_inicial = 0;
        //qtdEspacosDisponiveis = 0;
        // Avança para o próximo cluster (usando a FAT)
        
        uint32_t fat_entry_address = fat_offset + cluster * 4;
        lastCluster = cluster;
        fseek(image, fat_entry_address, SEEK_SET);
        fread(&cluster, sizeof(uint32_t), 1, image);
        cluster &= 0x0FFFFFFF;
    }

    ///Caso não tenha espaço criar novo cluster e relacionar com o diretorio
    qtdEntrada1 = qtdEspacosDisponiveis;
     if (qtdEspacosDisponiveis != qtdEntradas) {
        uint32_t newCluster = allocate_new_cluster(image, lastCluster, fat_offset, num_clusters);
        if (!newCluster)
        {
            printf("Erro: Espaço insuficiente para criar o arquivo.\n");
            return;
        }
        clear_cluster (image,newCluster,data_offset, cluster_size );
        uint32_t cluster_address = data_offset + (newCluster - 2) * cluster_size;
        fseek(image, cluster_address, SEEK_SET);
        printf("cluster entrada: %ld\n", (long int) cluster_address);
        fread(buffer, cluster_size, 1, image);
        for (int i = 0; i < cluster_size; i += sizeof(DirectoryEntry)) {
             uint8_t *entry_bytes = buffer + i;

            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

            if (entry->name[0] == 0x00 || entry->name[0] == 0xE5){ // Entrada vazia
                if(qtdEspacosDisponiveis == 0) cluster_address_inicial2 = cluster_address + i;
                qtdEspacosDisponiveis ++;
                if (qtdEspacosDisponiveis == qtdEntradas) break;
            }else{
                cluster_address_inicial2 = 0;
                qtdEspacosDisponiveis = qtdEntrada1;
            }
        }
    }
    qtdEntrada2 = qtdEntradas - qtdEntrada1;

    /// gerar o SFN
    DirectoryEntry *sfn = (DirectoryEntry *)malloc(sizeof(DirectoryEntry));
    if (!sfn) {
        perror("Erro ao alocar memória");
        return;
    }
    memset(sfn, 0, sizeof(DirectoryEntry));
    char sfnNome[11];
    generate_sfn(nmArquivo, sfnNome, 0);
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    strncpy(sfn->name, sfnNome,11);
    sfn->attr = ATTR_DIRECTORY;
    sfn->date = ((tm_info->tm_year - 80) << 9) | ((tm_info->tm_mon + 1) << 5) | tm_info->tm_mday;
    sfn->time = (tm_info->tm_hour << 11) | (tm_info->tm_min << 5) | (tm_info->tm_sec / 2);
    sfn->date_mod = sfn->date;
    sfn->time_mod = sfn->time;
    sfn->size = 0;

    uint32_t localLivre = getEspacoLivreFat(fat,num_clusters);
    if(localLivre == 0xFFFFFFFF){
        printf("Erro: Não há espaço disponível na FAT.\n");
        return;
    } 
    fat[localLivre] = 0x0FFFFFF8;  // Marca como fim de arquivo/diretório
    // Atualiza a FAT no arquivo
    uint32_t fat_entry_address = fat_offset + (localLivre * 4);
    fseek(image, fat_entry_address, SEEK_SET);
    fwrite(&fat[localLivre], sizeof(uint32_t), 1, image);
    fflush(image);
    printf("Cluster encontrado livre %ld\n",(long int)localLivre);
    sfn->start_high = (uint16_t)((localLivre >> 16) & 0xFFFF); // Parte alta
    sfn->start_low  = (uint16_t) (localLivre & 0xFFFF);
    uint32_t starting_cluster = ((uint32_t)sfn->start_high << 16) | (uint32_t)sfn->start_low;
    printf("Cluster salvo %ld\n",(long int)starting_cluster);

    
    /// Gerar Chechsum
    uint8_t checksum = calculate_sfn_checksum(sfn->name);

    /// Gerar LFN
    LFNEntry lfn_buffer2[20];
    
    int len = strlen(nmArquivo);
    int index = 0;
    int posBuffer = 0;

    for (int i = qtdEntradas - 1; i > 0; i--) {
        LFNEntry *lfn = (LFNEntry *)malloc(sizeof(LFNEntry));
        memset(lfn, 0, sizeof(LFNEntry));            
        lfn->order = (i + 1) | (i == 1 ? 0x40 : 0x00); // Última entrada tem bit 0x40
        lfn->attr = ATTR_LFN;
        lfn->type = 0;
        lfn->checksum = checksum;
        lfn->first_cluster = 0;
        for (int j = 0; j < 5; j++) {
            if (index < len) {
                uint16_t utf16_char = nmArquivo[index++];
                ((uint16_t*)lfn->name1)[j] = utf16_char;
            } else {
                ((uint16_t*)lfn->name1)[j] = 0; // Espaços vazios em UTF-16
            }
        }
        for (int j = 0; j < 6; j++) {
            if (index < len) {
                uint16_t utf16_char = nmArquivo[index++];
                ((uint16_t*)lfn->name2)[j] = utf16_char;
            } else {
                ((uint16_t*)lfn->name2)[j] = 0; // Espaços vazios em UTF-16
            }
        }
        for (int j = 0; j < 2; j++) {
            if (index < len) {
                uint16_t utf16_char = nmArquivo[index++];
                ((uint16_t*)lfn->name3)[j] = utf16_char;
            } else {
                ((uint16_t*)lfn->name3)[j] = 0; // Espaços vazios em UTF-16
            }
        }
        lfn_buffer2[i-1] = *lfn;
         free(lfn);
    }
    char teste_nome[256] = {0};
    reconstruct_long_name(teste_nome, lfn_buffer2, (qtdEntradas-1));
    printf("Esta aqui a saida: %s\n", teste_nome);
   

    uint8_t buffer2[cluster_size];
    memset(buffer2, 0x00, sizeof(buffer2));

    if (qtdEntrada2 == 0){
        for (int i = 0; i < qtdEntradas -1; i++){
            fseek(image, cluster_address_inicial1 + i * sizeof(LFNEntry), SEEK_SET);
            printf("cluster escrita: %ld\n", (long int) (cluster_address_inicial1 + i * sizeof(LFNEntry)));
            memcpy(buffer2, &lfn_buffer2[i], sizeof(LFNEntry));
            print_hex( buffer2, sizeof(LFNEntry));
            fwrite(buffer2, cluster_size, 1, image);
        }

        fseek(image, (cluster_address_inicial1 + (qtdEntradas -1) * sizeof(DirectoryEntry)), SEEK_SET);
        memcpy(buffer2, sfn, sizeof(LFNEntry));
        print_hex( buffer2, sizeof(DirectoryEntry));
        fwrite(buffer2, cluster_size, 1, image);
        fflush(image);
    }else{
        for (int i = 0; i < qtdEntrada1; i++){
            fseek(image, cluster_address_inicial1 + i * sizeof(LFNEntry), SEEK_SET);
            printf("cluster escrita: %ld\n", (long int) (cluster_address_inicial1 + i * sizeof(LFNEntry)));
            memcpy(buffer2, &lfn_buffer2[i], sizeof(LFNEntry));
            print_hex( buffer2, sizeof(LFNEntry));
            fwrite(buffer2, cluster_size, 1, image);
        }

        for (int i = 0; i < (qtdEntrada2 - 1); i++){
            fseek(image, cluster_address_inicial2 + i * sizeof(LFNEntry), SEEK_SET);
            printf("cluster escrita: %ld\n", (long int) (cluster_address_inicial2 + i * sizeof(LFNEntry)));
            memcpy(buffer2, &lfn_buffer2[i], sizeof(LFNEntry));
            print_hex( buffer2, sizeof(LFNEntry));
            fwrite(buffer2, cluster_size, 1, image);
        }

        fseek(image, (cluster_address_inicial2 + (qtdEntrada2 -1) * sizeof(DirectoryEntry)), SEEK_SET);
        memcpy(buffer2, sfn, sizeof(LFNEntry));
        print_hex( buffer2, sizeof(DirectoryEntry));
        fwrite(buffer2, cluster_size, 1, image);
        fflush(image);
    }
    
    uint32_t last_cluster = root_cluster;
    uint32_t last_cluster_address = data_offset + (starting_cluster - 2) * cluster_size;
    DirectoryEntry *sfn_last_cluster = (DirectoryEntry *)malloc(sizeof(DirectoryEntry));
    if (!sfn_last_cluster) {
        perror("Erro ao alocar memória");
        return;
    }
    memset(sfn_last_cluster, 0, sizeof(DirectoryEntry));

    strcpy(sfn_last_cluster->name , "..");
    sfn_last_cluster->attr = ATTR_DIRECTORY;
    sfn_last_cluster->date = ((tm_info->tm_year - 80) << 9) | ((tm_info->tm_mon + 1) << 5) | tm_info->tm_mday;
    sfn_last_cluster->time = (tm_info->tm_hour << 11) | (tm_info->tm_min << 5) | (tm_info->tm_sec / 2);
    sfn_last_cluster->date_mod = sfn->date;
    sfn_last_cluster->time_mod = sfn->time;
    sfn_last_cluster->size = 0;

    sfn_last_cluster->start_high = (uint16_t)((last_cluster >> 16) & 0xFFFF); // Parte alta
    sfn_last_cluster->start_low  = (uint16_t) (last_cluster & 0xFFFF);
    //uint32_t starting_cluster = ((uint32_t)sfn->start_high << 16) | (uint32_t)sfn->start_low;
    //printf("Cluster salvo %ld\n",(long int)starting_cluster);

    fseek(image, last_cluster_address, SEEK_SET);
    memcpy(buffer2, sfn_last_cluster, sizeof(DirectoryEntry));
    print_hex( buffer2, sizeof(DirectoryEntry));
    fwrite(buffer2, cluster_size, 1, image);
    fflush(image);

    printf("Diretorio Criado com Sucesso!\n");
    free(sfn);
}
//-----------------------------------------------------------------------------------------------------//
/*mkdir*/

//-----------------------------------------------------------------------------------------------------//
/*rm*/
void free_clusters(FILE *image, uint32_t start_cluster, uint32_t fat_offset) {
    uint32_t current_cluster = start_cluster;
    uint32_t next_cluster;

    while (1) {
        if (current_cluster < 2 || current_cluster >= 0x0FFFFFF0) {
            break;
        }

        // Ler o próximo cluster na FAT
        fseek(image, fat_offset + current_cluster * 4, SEEK_SET);
        fread(&next_cluster, sizeof(uint32_t), 1, image);

        // Garantir que o valor lido está no formato correto (remover bits reservados)
        next_cluster &= 0x0FFFFFFF;

        // Marcar o cluster atual como livre (0x00000000)
        uint32_t free_entry = 0x00000000;
        fseek(image, fat_offset + current_cluster * 4, SEEK_SET);
        fwrite(&free_entry, sizeof(uint32_t), 1, image);
        fflush(image);

        current_cluster = next_cluster;

        // Se chegamos ao fim da cadeia, saímos
        if (current_cluster >= 0x0FFFFFF8) {
            break;
        }
    }
}


void rm(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * nmArquivo, uint32_t * fat, uint32_t num_clusters){
    uint32_t cluster = root_cluster;
    uint32_t cluster_size = bytes_per_sector * sectors_per_cluster;
    uint8_t *buffer = malloc(cluster_size);
    int inAchou = 0;
    uint32_t cluster_inicial; 

    
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
                    cluster_inicial = cluster_address + i;
                }
                lfn_buffer[lfn_index++] = *lfn_entry;
            } else {
                // Reconstrói o nome longo se existirem entradas LFN
                char long_name[256] = {0};
                if (lfn_index > 0) {
                    reconstruct_long_name(long_name, lfn_buffer, lfn_index);
                     // Limpa o índice
                    if(!strcmp(long_name, nmArquivo)){
                        if(entry->attr & ATTR_DIRECTORY){
                            printf ("Não é um arquivo \n");
                            return;
                        }else{
                            
                          
                            fseek(image, cluster_address + i, SEEK_SET);
                            uint8_t marker = 0xE5;
                            fwrite(&marker, sizeof(uint8_t), 1, image);
                            fflush(image);

                            for (int j = 0; j < lfn_index; j++) {
                                uint32_t lfn_offset = i - j * sizeof(DirectoryEntry);
                                fseek(image, cluster_address + lfn_offset, SEEK_SET);
                                uint8_t marker = 0xE5;
                                fwrite(&marker, sizeof(uint8_t), 1, image);
                            }

                            // fseek(image, cluster_address + i, SEEK_SET);
                            // fwrite(entry, sizeof(DirectoryEntry), 1, image);
                            // fflush(image);

                            uint32_t starting_cluster = (entry->start_high << 16) | entry->start_low;
                            printf ("cluster inicial %d \n", starting_cluster );
                            free_clusters(image, starting_cluster, fat_offset);
                            inAchou = 1;
                            break;
                        }
                    }
                    lfn_index = 0;
                }
            }
        }
        // Avança para o próximo cluster (usando a FAT)
        uint32_t fat_entry_address = fat_offset + cluster * 4;
        fseek(image, fat_entry_address, SEEK_SET);
        fread(&cluster, sizeof(uint32_t), 1, image);
        cluster &= 0x0FFFFFFF;
    }

    if(!inAchou){
        printf ("Arquivo não encontrado \n");
        return;
    }
}
//-----------------------------------------------------------------------------------------------------//
/*rm*/

//-----------------------------------------------------------------------------------------------------//
/*cp*/

uint32_t calcular_clusters_necessarios(uint32_t file_size, uint32_t cluster_size) {
    return (file_size + cluster_size - 1) / cluster_size; // Arredonda para cima
}

uint32_t alocar_clusters(FILE *image, uint32_t qtd_clusters, uint32_t fat_offset, uint32_t total_clusters) {
    uint32_t first_cluster = 0; // Primeiro cluster alocado
    uint32_t last_allocated = 0; // Último cluster alocado
    uint32_t count = 0; // Contador de clusters alocados

    for (uint32_t cluster = 2; cluster < total_clusters; cluster++) {
        uint32_t fat_entry;
        fseek(image, fat_offset + cluster * 4, SEEK_SET);
        fread(&fat_entry, sizeof(uint32_t), 1, image);
        fat_entry &= 0x0FFFFFFF; // Máscara para FAT32

        if (fat_entry == 0x00000000) { // Cluster livre
            if (first_cluster == 0) {
                first_cluster = cluster; // Marca o primeiro cluster alocado
            }
            if (last_allocated != 0) {
                // Atualiza o último cluster para apontar para o atual
                fseek(image, fat_offset + last_allocated * 4, SEEK_SET);
                fwrite(&cluster, sizeof(uint32_t), 1, image);
            }
            last_allocated = cluster; // Atualiza o último cluster alocado
            count++;

            if (count == qtd_clusters) {
                // Marca o último cluster como fim de cadeia (EOC)
                uint32_t eoc = 0x0FFFFFF8;
                fseek(image, fat_offset + cluster * 4, SEEK_SET);
                fwrite(&eoc, sizeof(uint32_t), 1, image);
                fflush(image);
                return first_cluster;
            }
        }
    }

    // Se não encontrou clusters suficientes, desfaz alocações
    if (first_cluster != 0) {
        uint32_t cluster = first_cluster;
        while (count--) {
            fseek(image, fat_offset + cluster * 4, SEEK_SET);
            uint32_t next_cluster;
            fread(&next_cluster, sizeof(uint32_t), 1, image);
            next_cluster &= 0x0FFFFFFF;

            uint32_t free_entry = 0x00000000;
            fseek(image, fat_offset + cluster * 4, SEEK_SET);
            fwrite(&free_entry, sizeof(uint32_t), 1, image);
            
            cluster = next_cluster;
        }
        fflush(image);
    }

    return 0; // Erro: espaço insuficiente
}

void copy_clusters(FILE *image, uint32_t src_cluster, uint32_t dest_cluster, uint32_t fat_offset, uint32_t data_offset, uint32_t cluster_size) {
    uint8_t *buffer = malloc(cluster_size);
    if (!buffer) {
        fprintf(stderr, "Erro ao alocar buffer de memória!\n");
        return;
    }

    uint32_t current_src = src_cluster;
    uint32_t current_dest = dest_cluster;

    while (current_src < END_OF_CLUSTER) {
        // Calcula endereço do cluster de origem e lê os dados
        uint32_t src_address = data_offset + (current_src - 2) * cluster_size;
        fseek(image, src_address, SEEK_SET);
        fread(buffer, cluster_size, 1, image);

        // Calcula endereço do cluster de destino e escreve os dados
        uint32_t dest_address = data_offset + (current_dest - 2) * cluster_size;
        fseek(image, dest_address, SEEK_SET);
        fwrite(buffer, cluster_size, 1, image);

        // Atualiza FAT para pegar o próximo cluster de origem
        uint32_t fat_src_address = fat_offset + current_src * 4;
        fseek(image, fat_src_address, SEEK_SET);
        fread(&current_src, sizeof(uint32_t), 1, image);
        current_src &= 0x0FFFFFFF; // Mascara para garantir 28 bits válidos

        // Atualiza FAT para pegar o próximo cluster de destino
        uint32_t fat_dest_address = fat_offset + current_dest * 4;
        fseek(image, fat_dest_address, SEEK_SET);
        fread(&current_dest, sizeof(uint32_t), 1, image);
        current_dest &= 0x0FFFFFFF;

        // Se o próximo cluster de destino não estiver alocado, deve-se alocar um novo
        if (current_dest >= END_OF_CLUSTER) {
            printf("Cópia concluída com sucesso!\n");
            free(buffer);
            return;
        }
    }

    free(buffer);
    printf("Cópia concluída com sucesso!\n");
}

int contabarras (char string[]) {
    int contabarras = 0;
    for (int i = 0; string[i] != '\0'; i++) {
        if(string[i] == '/'){
            contabarras++;
        }
    }
   return contabarras;
} 

void cp(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * sourcePath, char * targetPath, uint32_t * fat, uint32_t num_clusters, char *current_path){
    uint32_t cluster = root_cluster;
    printf("root_cluster: %d\n", root_cluster);
    printf("current_path: %s\n", current_path);

    char *actual_path = malloc(strlen(current_path) + 1);
    char true_path[strlen(current_path)];
    strcpy(actual_path, current_path);

    int fstSlash = 0;
    int truesize = 0;
    for(int i = strlen(actual_path) - 1; (actual_path[i] == '/' && fstSlash == 1) ; i--){
        if(actual_path[i] == '/'){
            fstSlash = 1;
            actual_path[i] = '\0';
        }
        truesize++;
    }



    printf("true_path: %s\n", true_path);

    uint32_t cluster_size = bytes_per_sector * sectors_per_cluster;
    uint8_t *buffer = malloc(cluster_size);
    int inAchou = 0;
    int inValido = 0;
    uint32_t cluster_inicial, tamArquivo, cluster_diretorio; 
    char *tokenize;
    char token[256];
    char nextToken[256];
    char *nmArquivo;
    uint32_t starting_cluster, src_cluster;
    int barcount = contabarras(sourcePath);
    int barcount2 = contabarras(targetPath);
    char **tokens_src = malloc((barcount + 1) * sizeof(char *));
    char **tokens_target = malloc((barcount2 + 1) * sizeof(char *));
    
    tokenize = strtok(sourcePath, "/");
    for(int i = 0; tokenize != NULL && i < barcount + 1; i++){
        tokens_src[i] = tokenize;
        tokenize = strtok(NULL, "/");
    }
    printf("tokens_src[-1]: %s\n", tokens_src[barcount]);

    tokenize = strtok(targetPath, "/");
    for(int i = 0; tokenize != NULL && i < barcount2 + 1; i++){
        tokens_target[i] = tokenize;
        tokenize = strtok(NULL, "/");
    }
    printf("tokens_target[-1]: %s\n", tokens_target[barcount2]);

    if(!(strncmp(sourcePath, "img", 3)) && !(strncmp(targetPath, "img", 3))){
        int advance_token = 1;
        printf("Entrou aqui\n");
        // strcpy(sourcePath, sourcePath + 3);
        // strcpy(targetPath, targetPath + 3);
        
        
        int idx_src = barcount;
        int idx_target = barcount2;

        strcpy(token, tokens_src[idx_src]);
        char nextTokenBuffer[256];
        if (idx_src +1 < barcount + 1) {
            strcpy(nextTokenBuffer, tokens_src[idx_src + 1]);
        } else {
            nextTokenBuffer[0] = '\0';
        }

        
        

        printf("token: %s --- Next token: %s \n", token, nextTokenBuffer);

        LFNEntry lfn_buffer[20];
        int lfn_index = 0;
        while (cluster < 0x0FFFFFF8) { // Clusters válidos no FAT32
            uint32_t cluster_address = data_offset + (cluster - 2) * cluster_size;
            fseek(image, cluster_address, SEEK_SET);
            fread(buffer, cluster_size, 1, image);

            for (int i = 0; i < cluster_size; i += sizeof(DirectoryEntry)) {
                uint8_t *entry_bytes = buffer + i;

                DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);
                printf("Entry attr: %d\n", entry->attr);
                if (entry->name[0] == 0x00) // Entrada vazia
                    break;

                if (entry->name[0] == 0xE5) // Entrada deletada
                    continue;
                if (entry->attr == 0x0F) { // Verifica se é uma entrada LFN
                    LFNEntry *lfn_entry = (LFNEntry *)(entry_bytes);
                    if ((lfn_entry->order & 0x40) != 0) { // Primeira entrada de uma sequência LFN
                        lfn_index = 0; // Reinicia o buffer
                        cluster_inicial = cluster_address + i;
                    }
                    lfn_buffer[lfn_index++] = *lfn_entry;
                } else {
                    // Reconstrói o nome longo se existirem entradas LFN
                    char long_name[256] = {0};
                    if (lfn_index > 0) {
                        printf("LFN index: %d\n", lfn_index);
                        reconstruct_long_name(long_name, lfn_buffer, lfn_index);
                        printf("LFN reconstruído: %s\n", long_name);
                        lfn_index = 0;
                        printf("token: %s --- long_name %s \n", token, long_name);

                        if (strcasecmp(token, long_name) == 0) {
                            printf("Token correspondente ao long_name: %s\n", long_name);
                        } else {
                            printf("Token e long_name não coincidem: token = %s, long_name = %s\n", token, long_name);
                        }

                        if (!strcmp(token, long_name))
                        {
                            if (idx_src +1 < (barcount + 1))
                            {
                                starting_cluster = (entry->start_high << 16) | entry->start_low;
                                cluster_address = data_offset + (starting_cluster - 2) * cluster_size;
                                fseek(image, cluster_address, SEEK_SET);
                                fread(buffer, cluster_size, 1, image);
                                i=0;
                                idx_src++;
                                strcpy(token, tokens_src[idx_src]);
                            }else if (entry->attr = ATTR_ARCHIVE)
                            {
                                tamArquivo = entry->size;
                                src_cluster = (entry->start_high << 16) | entry->start_low;
                                printf("Arquivo encontrado: tamanho %d cluster inicial %d \n\n",tamArquivo, src_cluster);
                                inAchou = 1;
                                break;
                            }    
                        }         
                    }
                }
                advance_token--;
            }
            // Avança para o próximo cluster (usando a FAT)
            if(!inAchou){
                uint32_t fat_entry_address = fat_offset + cluster * 4;
                fseek(image, fat_entry_address, SEEK_SET);
                fread(&cluster, sizeof(uint32_t), 1, image);
                cluster &= 0x0FFFFFFF;
            }else{
                break;
            }
        }
        if (!inAchou)
        {   
            printf("To no primeiro if\n");
            printf("Source path não encontrado\n");
            return;
        }
        

        cluster = root_cluster;
        strcpy(token, tokens_target[idx_target]);
        if (idx_target + 1 < barcount2 + 1) {
            strcpy(nextTokenBuffer, tokens_target[idx_target + 1]);
        } else {
            nextTokenBuffer[0] = '\0';
        }

        
        

        starting_cluster = root_cluster;
        printf("token: %s --- Next token: %s \n", token, nextTokenBuffer);
        while (cluster < 0x0FFFFFF8) { // Clusters válidos no FAT32
            uint32_t cluster_address = data_offset + (cluster - 2) * cluster_size;
            fseek(image, cluster_address, SEEK_SET);
            fread(buffer, cluster_size, 1, image);

            for (int i = 0; i < cluster_size; i += sizeof(DirectoryEntry)) {
                uint8_t *entry_bytes = buffer + i;

                DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

                if (nextToken == NULL){
                    printf("endereço válido\n");
                    printf ("Esse é o cluster: %d", starting_cluster);
                    cluster_diretorio = starting_cluster;
                    inValido = 1;
                    break;
                }

                if (entry->name[0] == 0x00) // Entrada vazia
                    break;

                if (entry->name[0] == 0xE5) // Entrada deletada
                    continue;
                if (entry->attr == 0x0F) { // Verifica se é uma entrada LFN
                    LFNEntry *lfn_entry = (LFNEntry *)(entry_bytes);
                    if ((lfn_entry->order & 0x40) != 0) { // Primeira entrada de uma sequência LFN
                        lfn_index = 0; // Reinicia o buffer
                        cluster_inicial = cluster_address + i;
                    }
                    lfn_buffer[lfn_index++] = *lfn_entry;
                } else {
                    // Reconstrói o nome longo se existirem entradas LFN
                    char long_name[256] = {0};
                    if (lfn_index > 0) {
                        reconstruct_long_name(long_name, lfn_buffer, lfn_index);
                        lfn_index = 0;
                        printf("token: %s --- long_name %s \n", token, long_name);
                        if (!strcmp(token, long_name))
                        {
                            if (idx_target + 1 < (barcount2 + 1))
                            {
                                starting_cluster = (entry->start_high << 16) | entry->start_low;
                                cluster_address = data_offset + (starting_cluster - 2) * cluster_size;
                                fseek(image, cluster_address, SEEK_SET);
                                fread(buffer, cluster_size, 1, image);
                                i = 0;
                                idx_target++;
                                strcpy(token, tokens_target[idx_target]);
                            }   
                        }         
                    }
                }
            }
            if(!inValido){
                // Avança para o próximo cluster (usando a FAT)
                uint32_t fat_entry_address = fat_offset + cluster * 4;
                fseek(image, fat_entry_address, SEEK_SET);
                fread(&cluster, sizeof(uint32_t), 1, image);
                cluster &= 0x0FFFFFFF;
            }
            else{
                break;
            }
        }

        if (!inValido)
        {
            printf("target path não encontrado\n");
            return;
        }

        uint32_t clusters_necessarios = calcular_clusters_necessarios(tamArquivo, cluster_size);
        uint32_t primeiro_cluster = alocar_clusters(image, clusters_necessarios, fat_offset, num_clusters);

        touch(image, cluster_diretorio, bytes_per_sector, sectors_per_cluster, fat_offset, data_offset, token, tamArquivo, primeiro_cluster, num_clusters);
        copy_clusters(image, src_cluster, primeiro_cluster, fat_offset, data_offset, cluster_size);
        //printf ("cluster diretorio: %d nome arquivo %s \n", cluster_diretorio, token);
        //printf ("cluster necessários: %d primeiro cluster %d \n", clusters_necessarios, primeiro_cluster);
        return;
    }else if (!(strncmp(sourcePath, "img", 3)) && !(strncmp(targetPath, "/", 1)))
    {
        strcpy(sourcePath, sourcePath + 3);
       
        int idx_src = barcount;
        int idx_target = barcount2;

        strcpy(token, tokens_src[idx_src]);
        char nextTokenBuffer[256];
        if (idx_src +1 < barcount + 1) {
            strcpy(nextTokenBuffer, tokens_src[idx_src + 1]);
        } else {
            nextTokenBuffer[0] = '\0';
        }

        
        

        printf("token: %s --- Next token: %s \n", token, nextToken);

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
                        cluster_inicial = cluster_address + i;
                    }
                    lfn_buffer[lfn_index++] = *lfn_entry;
                } else {
                    // Reconstrói o nome longo se existirem entradas LFN
                    char long_name[256] = {0};
                    if (lfn_index > 0) {
                        reconstruct_long_name(long_name, lfn_buffer, lfn_index);
                        lfn_index = 0;
                        printf("token: %s --- long_name %s \n", token, long_name);
                        if (!strcmp(token, long_name))
                        {
                            if (idx_src + 1 < (barcount + 1))
                            {
                                starting_cluster = (entry->start_high << 16) | entry->start_low;
                                cluster_address = data_offset + (starting_cluster - 2) * cluster_size;
                                fseek(image, cluster_address, SEEK_SET);
                                fread(buffer, cluster_size, 1, image);
                                i=0;
                                idx_src++;
                                strcpy(token, tokens_src[idx_src]);
                            }else if (entry->attr = ATTR_ARCHIVE)
                            {
                                tamArquivo = entry->size;
                                src_cluster = (entry->start_high << 16) | entry->start_low;
                                printf("Arquivo encontrado: tamanho %d cluster inicial %d \n\n",tamArquivo, src_cluster);
                                inAchou = 1;
                                break;
                            }    
                        }         
                    }
                }
            }
            // Avança para o próximo cluster (usando a FAT)
            if(!inAchou){
                uint32_t fat_entry_address = fat_offset + cluster * 4;
                fseek(image, fat_entry_address, SEEK_SET);
                fread(&cluster, sizeof(uint32_t), 1, image);
                cluster &= 0x0FFFFFFF;
            }else{
                break;
            }
        }
        if (!inAchou)
        {
            printf("Source path não encontrado\n");
            return;
        }



        FILE *out_file = fopen(targetPath, "wb");
        if (!out_file) {
            perror("Erro ao criar arquivo no host");
            return;
        }

        uint8_t *buffer = malloc(cluster_size);
        if (!buffer) {
            fprintf(stderr, "Erro ao alocar memória\n");
            fclose(out_file);
            return;
        }

        uint32_t current_cluster = src_cluster;
        while (current_cluster < END_OF_CLUSTER) {
            uint32_t src_address = data_offset + (current_cluster - 2) * cluster_size;
            fseek(image, src_address, SEEK_SET);
            fread(buffer, cluster_size, 1, image);
            fwrite(buffer, cluster_size, 1, out_file);

            // Avançar na FAT
            fseek(image, fat_offset + current_cluster * 4, SEEK_SET);
            fread(&current_cluster, sizeof(uint32_t), 1, image);
            current_cluster &= 0x0FFFFFFF;
        }

        printf("Arquivo copiado com sucesso para %s\n", targetPath);
        free(buffer);
        fclose(out_file);
    }else if ( !(strncmp(sourcePath, "/", 1)) && !(strncmp(targetPath, "img", 3)))
    {
        strcpy(targetPath, targetPath + 3);
        FILE *in_file = fopen(sourcePath, "rb");
        if (!in_file) {
            perror("Erro ao abrir arquivo do host");
            return;
        }

        cluster = root_cluster;
        int idx_src = barcount;
        int idx_target = barcount2;

        strcpy(token, tokens_target[idx_target]);
        char nextTokenBuffer[256];
        if (idx_target +1 < barcount + 1) {
            strcpy(nextTokenBuffer, tokens_src[idx_target + 1]);
        } else {
            nextTokenBuffer[0] = '\0';
        }

        
        

        starting_cluster = root_cluster;
        LFNEntry lfn_buffer[20];
        int lfn_index = 0;
        printf("token: %s --- Next token: %s \n", token, nextToken);
        while (cluster < 0x0FFFFFF8) { // Clusters válidos no FAT32
            uint32_t cluster_address = data_offset + (cluster - 2) * cluster_size;
            fseek(image, cluster_address, SEEK_SET);
            fread(buffer, cluster_size, 1, image);

            for (int i = 0; i < cluster_size; i += sizeof(DirectoryEntry)) {
                uint8_t *entry_bytes = buffer + i;

                DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);

                if (nextToken == NULL){
                    printf("endereço válido\n");
                    printf ("Esse é o cluster: %d", starting_cluster);
                    cluster_diretorio = starting_cluster;
                    inValido = 1;
                    break;
                }

                if (entry->name[0] == 0x00) // Entrada vazia
                    break;

                if (entry->name[0] == 0xE5) // Entrada deletada
                    continue;
                if (entry->attr == 0x0F) { // Verifica se é uma entrada LFN
                    LFNEntry *lfn_entry = (LFNEntry *)(entry_bytes);
                    if ((lfn_entry->order & 0x40) != 0) { // Primeira entrada de uma sequência LFN
                        lfn_index = 0; // Reinicia o buffer
                        cluster_inicial = cluster_address + i;
                    }
                    lfn_buffer[lfn_index++] = *lfn_entry;
                } else {
                    // Reconstrói o nome longo se existirem entradas LFN
                    char long_name[256] = {0};
                    if (lfn_index > 0) {
                        reconstruct_long_name(long_name, lfn_buffer, lfn_index);
                        lfn_index = 0;
                        printf("token: %s --- long_name %s \n", token, long_name);
                        if (!strcmp(token, long_name))
                        {
                            if (idx_target + 1 < (barcount2 + 1))
                            {
                                starting_cluster = (entry->start_high << 16) | entry->start_low;
                                cluster_address = data_offset + (starting_cluster - 2) * cluster_size;
                                fseek(image, cluster_address, SEEK_SET);
                                fread(buffer, cluster_size, 1, image);
                                i=0;
                                idx_target++;
                                strcpy(token, tokens_target[idx_target]);
                            }   
                        }         
                    }
                }
            }
            if(!inValido){
                // Avança para o próximo cluster (usando a FAT)
                uint32_t fat_entry_address = fat_offset + cluster * 4;
                fseek(image, fat_entry_address, SEEK_SET);
                fread(&cluster, sizeof(uint32_t), 1, image);
                cluster &= 0x0FFFFFFF;
            }
            else{
                break;
            }
        }

        if (!inValido)
        {
            printf("target path não encontrado\n");
            return;
        }


        fseek(in_file, 0, SEEK_END);
        uint32_t file_size = ftell(in_file);
        rewind(in_file);

        printf("tamnho arquivo : %d\n", file_size);
        uint32_t clusters_necessarios = calcular_clusters_necessarios(file_size, cluster_size);
        uint32_t primeiro_cluster = alocar_clusters(image, clusters_necessarios, fat_offset, num_clusters);

        touch(image, cluster_diretorio, bytes_per_sector, sectors_per_cluster, fat_offset, data_offset, token, file_size, primeiro_cluster, num_clusters);
        //copy_clusters(image, src_cluster, primeiro_cluster, fat_offset, data_offset, cluster_size);
        //printf ("cluster diretorio: %d nome arquivo %s \n", cluster_diretorio, token);
        //printf ("cluster necessários: %d primeiro cluster %d \n", clusters_necessarios, primeiro_cluster);

        uint32_t needed_clusters = (file_size + cluster_size - 1) / cluster_size;
        uint32_t dest_cluster = alocar_clusters(image, needed_clusters, fat_offset, num_clusters);
        if (dest_cluster == 0) {
            printf("Erro: Espaço insuficiente na imagem\n");
            fclose(in_file);
            return;
        }

        uint8_t *buffer = malloc(cluster_size);
        if (!buffer) {
            fprintf(stderr, "Erro ao alocar memória\n");
            fclose(in_file);
            return;
        }

        uint32_t current_cluster = primeiro_cluster;
        while (file_size > 0) {
            size_t bytes_to_write = (file_size > cluster_size) ? cluster_size : file_size;
            fread(buffer, 1, bytes_to_write, in_file);
            uint32_t dest_address = data_offset + (current_cluster - 2) * cluster_size;
            fseek(image, dest_address, SEEK_SET);
            fwrite(buffer, 1, bytes_to_write, image);

            file_size -= bytes_to_write;

            if (file_size > 0) { 
                uint32_t fat_dest_address = fat_offset + current_cluster * 4;
                fseek(image, fat_dest_address, SEEK_SET);
                fread(&current_cluster, sizeof(uint32_t), 1, image);
                current_cluster &= 0x0FFFFFFF;

                // Se o próximo cluster de destino não estiver alocado, deve-se alocar um novo
                if (current_cluster >= END_OF_CLUSTER) {
                    printf("Cópia concluída com sucesso!\n");
                    free(buffer);
                    return;
                }
            }
        }

        printf("Arquivo %s copiado para a imagem com sucesso!\n", sourcePath);
        free(buffer);
        fclose(in_file);

    }

}

void rmdir(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * nmArquivo, uint32_t * fat, uint32_t num_clusters) {
    uint32_t cluster = root_cluster;
    uint32_t cluster_size = bytes_per_sector * sectors_per_cluster;
    uint8_t *buffer = malloc(cluster_size);
    int inAchou = 0;
    uint32_t cluster_inicial;

    LFNEntry lfn_buffer[20];
    int lfn_index = 0;
    while (cluster < CLUSTER_END) {
        uint32_t cluster_address = data_offset + (cluster - 2) * cluster_size;
        fseek(image, cluster_address, SEEK_SET);
        fread(buffer, cluster_size, 1, image);
        
        for (int i = 0; i < cluster_size; i += sizeof(DirectoryEntry)) {
            uint8_t *entry_bytes = buffer + i;
    
            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);
    
            if (entry->name[0] == EMPTY_ENTRY) // Entrada vazia
                break;
            if (entry->name[0] == DELETED_ENTRY) // Entrada deletada
                continue;
            if (entry->attr == ATTR_LFN) {
                LFNEntry *lfn_entry = (LFNEntry *)(entry_bytes);
                if ((lfn_entry->order & 0x40) != 0) {
                    lfn_index = 0;
                    cluster_inicial = cluster_address + i;
                }
                lfn_buffer[lfn_index++] = *lfn_entry;            
            } else {
                char long_name[256] = {0};
                if (lfn_index > 0) {
                    reconstruct_long_name(long_name, lfn_buffer, lfn_index);
                    uint32_t dir_cluster = ((uint32_t)entry->start_high) |  ((uint32_t)entry->start_low << 16);
                    if(!strcmp(long_name, nmArquivo)) {
                       if(!(entry->attr & ATTR_DIRECTORY)) {
                            printf("Não é um diretório\n");
                            return;
                       } 

                       // checa se há arquivos dentro do diretório
                       if(!is_directory_empty(image, dir_cluster, fat_offset, data_offset, cluster_size)) {
                            printf("Erro: diretório não está vazio\n");
                            return;
                       }

                       entry->name[0] = (uint8_t) DELETED_ENTRY;
                       for (int j = 0; j < lfn_index; j++) {
                           lfn_buffer[j].order = (uint8_t) DELETED_ENTRY;
                       }
       
                       fseek(image, cluster_address + i, SEEK_SET);
                       fwrite(entry, sizeof(DirectoryEntry), 1, image);
                       fflush(image);
       
                       uint32_t starting_cluster = (entry->start_high << 16) | entry->start_low;
                       entry->start_low;
                       printf("Cluster inicial %d\n", starting_cluster);
                       free_clusters(image, starting_cluster, fat_offset);
                       inAchou = 1;
                       break;

                    }
                    lfn_index = 0;
                }
               
            }

        }
        // Avança para o próximo cluster (usando a FAT)
        uint32_t fat_entry_address = fat_offset + cluster * 4;
        fseek(image, fat_entry_address, SEEK_SET);
        fread(&cluster, sizeof(uint32_t), 1, image);
        cluster &= 0x0FFFFFFF;
    }

    if(!inAchou) {
        printf("Diretório não encontrado\n");
        return;
    }


}


void mv(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * sourcePath, char * targetPath, uint32_t * fat, uint32_t num_clusters, char *current_path) {


    char mod_source[256];
    strcpy(mod_source, sourcePath);
    
    cp(image, root_cluster, bytes_per_sector, sectors_per_cluster, fat_offset, data_offset, sourcePath, targetPath, fat, num_clusters, current_path);

    
    char *dummy = strtok(mod_source, "/");
    char *scPath = strtok(NULL, "/");
    // printf("sourcePath: %s\n", scPath);
    if (scPath == NULL)
    {
        printf("Erro: sourcePath inválido\n");
        return;
    }

    rm(image, root_cluster, bytes_per_sector, sectors_per_cluster, fat_offset, data_offset, scPath, fat, num_clusters);
}


char *format_path(char *name, char *current_path) {
    char fixed_path[256];
    char source_path[256];
    strcpy(source_path, "img");
    strcpy(fixed_path, current_path);
    strcat(source_path, strcat(fixed_path, name));
    printf("source_path: %s\n", source_path);
    char *result = malloc(strlen(source_path) + 1);
    if (!result) {
        perror("Erro ao alocar memória");
        exit(1);
    }
    strcpy(result, source_path);
    return result;
}



void rename_file(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * filename, char *new_filename, uint32_t *fat, uint32_t num_clusters, char* current_path) {
    // pega o nome do diretorio atual
    
    char *source_path = format_path(filename, current_path);
    char *target_path = format_path(new_filename, current_path);
   
    

    mv(image, root_cluster, bytes_per_sector, sectors_per_cluster, fat_offset, data_offset, source_path, target_path, fat, num_clusters, current_path);


}

//  mv img/hello.txt img/livros/hello.txt
//-----------------------------------------------------------------------------------------------------//
/*cp*/