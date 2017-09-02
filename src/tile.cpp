#include "utils.h"
#include "tile.h"

Tile::Tile(uint8_t id) : _id(id), _rgb(), _rgb2() {}

void Tile::pixel(int y, int x, const uchar *rgb) {
	int i = (y * TILE_SIZE + x) * 3;
	int i2 = (y * 2 * TILE_SIZE * 2 + x * 2) * 3;
	// red
	uchar r = *rgb++;
	_rgb[i] = r;
	_rgb2[i2] = r;
	_rgb2[i2+3] = r;
	_rgb2[i2+TILE_SIZE*2*3] = r;
	_rgb2[i2+TILE_SIZE*2*3+3] = r;
	i++;
	i2++;
	// green
	uchar g = *rgb++;
	_rgb[i] = g;
	_rgb2[i2] = g;
	_rgb2[i2+3] = g;
	_rgb2[i2+TILE_SIZE*2*3] = g;
	_rgb2[i2+TILE_SIZE*2*3+3] = g;
	i++;
	i2++;
	// blue
	uchar b = *rgb;
	_rgb[i] = b;
	_rgb2[i2] = b;
	_rgb2[i2+3] = b;
	_rgb2[i2+TILE_SIZE*2*3] = b;
	_rgb2[i2+TILE_SIZE*2*3+3] = b;
}
