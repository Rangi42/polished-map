#ifndef PALETTE_MAP_H
#define PALETTE_MAP_H

#include <cstring>

#include "utils.h"

#define MAX_TILESET_SIZE 256

class Palette_Map {
public:
	enum Palette { GRAY, RED, GREEN, WATER, YELLOW, BROWN, ROOF, TEXT };
private:
	Palette _palette[MAX_TILESET_SIZE];
	uint8_t _palette_size;
public:
	Palette_Map(void);
	bool read_from(const char *f);
	void clear(void) { memset(_palette, Palette::TEXT, MAX_TILESET_SIZE); _palette_size = 0; }
	uint8_t size(void) const { return _palette_size; }
};

#endif
