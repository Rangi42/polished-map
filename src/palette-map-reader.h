#ifndef PALETTE_MAP_READER_H
#define PALETTE_MAP_READER_H

#include "utils.h"

#define MAX_PALETTE_SIZE 256

class Palette_Map_Reader {
public:
	enum Palette { GRAY, RED, GREEN, WATER, YELLOW, BROWN, ROOF, TEXT };
private:
	bool _good;
	Palette _palette[MAX_PALETTE_SIZE];
	uint8_t _palette_size;
public:
	Palette_Map_Reader(const char *f);
	uint8_t size(void) const { return _palette_size; }
};

#endif
