#ifndef TILE_H
#define TILE_H

#pragma warning(push, 0)
#include <FL/fl_types.h>
#pragma warning(pop)

#include "utils.h"

#define TILE_SIZE 8

class Tile {
public:
	enum Hue { WHITE, DARK, LIGHT, BLACK };
private:
	uint8_t _id;
	uchar _rgb[TILE_SIZE * TILE_SIZE * 3];
public:
	Tile(uint8_t id);
	uint8_t id(void) const { return _id; }
	void pixel(int y, int x, uchar *rgb);
};

#endif
