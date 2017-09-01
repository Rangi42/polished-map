#include "utils.h"
#include "tile.h"

Tile::Tile(uint8_t id) : _id(id), _rgb() {}

void Tile::pixel(int y, int x, uchar *rgb) {
	int i = (y * TILE_SIZE + x) * 3;
	_rgb[i++] = *(rgb++);
	_rgb[i++] = *(rgb++);
	_rgb[i] = *rgb;
}
