#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "comandos.h"

void process_command(char *command, FILE *file, const BootSector *bs, uint32_t *fat, const char *image_path, uint32_t *current_cluster, char * current_path, char * last_path);