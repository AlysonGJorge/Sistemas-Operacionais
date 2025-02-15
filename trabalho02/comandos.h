#pragma once
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "bootsector.h"
#include "directory.h"
#include "fat.h"

void gather_disk_info(FILE *image, const BootSector *bs, DiskInfo *info);
void display_disk_info(const DiskInfo *info);
void info_teste( FILE *file, const BootSector *bs);
void display_cluster_content(FILE *image, const BootSector *bs, uint32_t cluster_num);
void pwd(char* current_path);
void attr(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * nmArquivo);
void ls(FILE *image, uint32_t cluster_path, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset);
void cd(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * nmArquivo, uint32_t * current_cluster, char * current_path, char * last_path);
void touch(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * nmArquivo, uint32_t tamArquivo, uint32_t clusterInicial, uint32_t num_cluster);
void rm(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * nmArquivo, uint32_t * fat, uint32_t num_clusters);
void cp(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * sourcePath, char * targetPath, uint32_t * fat, uint32_t num_clusters);
void mkdir(FILE *image, uint32_t root_cluster, uint32_t bytes_per_sector, uint32_t sectors_per_cluster, uint32_t fat_offset, uint32_t data_offset, char * nmArquivo, uint32_t * fat, uint32_t num_clusters);