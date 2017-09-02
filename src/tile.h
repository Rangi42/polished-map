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
	uchar _rgb2[(TILE_SIZE * 2) * (TILE_SIZE * 2) * 3];
public:
	Tile(uint8_t id);
	uint8_t id(void) const { return _id; }
	const uchar *rgb(void) const { return _rgb; }
	const uchar *rgb2(void) const { return _rgb2; }
	void pixel(int y, int x, uchar *rgb);
	void clear(void) { _rgb[0] = _rgb[1] = _rgb[2] = WHITE; }
};

#endif
