#include "../headers/coversion.h"

uint16_t convert_to_big_endian_16(uint16_t value) {
	return ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
}