#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void process_command(char *command, const char *image_path, const BootSector *bs, FILE *file, uint32_t *fat);



#endif // SHELL_H