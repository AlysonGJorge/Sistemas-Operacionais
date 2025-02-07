#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bootsector.h"

uint32_t *read_fat(FILE *file, const BootSector *bs);
void print_fat(uint32_t *fat, uint32_t num_entries, uint32_t start, uint32_t count);
uint32_t getEspacoLivreFat(uint32_t *fat, uint32_t num_entries);