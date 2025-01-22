
#define SECTOR_SIZE 512 // Tamanho do setor
#define DIR_ENTRY_SIZE 32 // Tamanho da entrada de diretório
#define ATTR_DIRECTORY 0x10 // Atributo para diretório

// Estrutura para uma entrada de diretório
typedef struct {
    char name[11];         // Nome do arquivo/diretório (8.3 format)
    uint8_t attr;          // Atributos
    uint8_t reserved[10];  // Reservado
    uint16_t time;         // Hora de criação
    uint16_t date;         // Data de criação
    uint16_t start_high;   // Cluster inicial (parte alta)
    uint16_t time_mod;     // Hora de modificação
    uint16_t date_mod;     // Data de modificação
    uint16_t start_low;    // Cluster inicial (parte baixa)
    uint32_t size;         // Tamanho do arquivo (0 para diretórios)
} DirectoryEntry;

void read_directory(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset) {
    uint32_t cluster = root_cluster;
    uint32_t cluster_size = bytes_per_sector * sectors_per_cluster;
    uint8_t buffer[SECTOR_SIZE];

    while (cluster < 0x0FFFFFF8) { // Clusters válidos no FAT32
        // Calcular o endereço do cluster na área de dados
        uint32_t cluster_address = data_offset + (cluster - 2) * cluster_size;

        // Ler o cluster
        fseek(image, cluster_address, SEEK_SET);
        fread(buffer, SECTOR_SIZE, 1, image);

        // Percorrer as entradas de diretório
        for (int i = 0; i < SECTOR_SIZE; i += DIR_ENTRY_SIZE) {
            DirectoryEntry *entry = (DirectoryEntry *)(buffer + i);
            // Ignorar entradas vazias ou apagadas
            if (entry->name[0] == 0x00 || entry->name[0] == 0xE5)
                continue;

            // Verificar se é arquivo ou diretório
            if (entry->attr & ATTR_DIRECTORY) {
                printf("[DIR] %.11s\n", entry->name);
            } else {
                printf("[FILE] %.11s - %d bytes\n", entry->name, entry->size);
            }
            
        }

        // Avançar para o próximo cluster (usando a FAT)
        uint32_t fat_entry_address = fat_offset + cluster * 4;
        fseek(image, fat_entry_address, SEEK_SET);
        fread(&cluster, sizeof(uint32_t), 1, image);
        cluster &= 0x0FFFFFFF; // Máscara para 28 bits
    }
}
