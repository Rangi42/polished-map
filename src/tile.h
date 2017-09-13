#ifndef TILE_H
#define TILE_H

#include <cstdlib>

#pragma warning(push, 0)
#include <FL/fl_types.h>
#pragma warning(pop)

#include "utils.h"

#define TILE_SIZE 8
#define NUM_CHANNELS 3
#define ZOOM_FACTOR 2

#define LINE_PX (TILE_SIZE * ZOOM_FACTOR)
#define LINE_BYTES (LINE_PX * NUM_CHANNELS)

class Tile {
public:
	enum Hue { WHITE, DARK, LIGHT, BLACK };
private:
	uint8_t _id;
	uchar _rgb[(TILE_SIZE * ZOOM_FACTOR) * (TILE_SIZE * ZOOM_FACTOR) * NUM_CHANNELS];
public:
	Tile(uint8_t id);
	uint8_t id(void) const { return _id; }
	const uchar *rgb(void) const { return _rgb; }
	uchar *pixel(int x, int y) { return _rgb + (y * LINE_BYTES + x * NUM_CHANNELS) * ZOOM_FACTOR; }
	const uchar *const_pixel(int x, int y) const { return _rgb + (y * LINE_BYTES + x * NUM_CHANNELS) * ZOOM_FACTOR; }
	void pixel(int x, int y, const uchar *rgb);
	void clear(void) { memset(_rgb, WHITE, sizeof(_rgb)); }
};

#endif
