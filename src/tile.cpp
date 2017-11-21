#include "utils.h"
#include "tile.h"

Tile::Tile(uint8_t id) : _id(id), _hues(), _rgb() {}

void Tile::pixel(int x, int y, Hue h, const uchar *rgb) {
	_hues[y * TILE_SIZE + x] = h;
	int i = (y * LINE_BYTES + x * NUM_CHANNELS) * ZOOM_FACTOR;
	// red
	uchar r = *rgb++;
	_rgb[i] = r;
	_rgb[i + NUM_CHANNELS] = r;
	_rgb[i + LINE_BYTES] = r;
	_rgb[i + LINE_BYTES + NUM_CHANNELS] = r;
	i++;
	// green
	uchar g = *rgb++;
	_rgb[i] = g;
	_rgb[i + NUM_CHANNELS] = g;
	_rgb[i + LINE_BYTES] = g;
	_rgb[i + LINE_BYTES + NUM_CHANNELS] = g;
	i++;
	// blue
	uchar b = *rgb;
	_rgb[i] = b;
	_rgb[i + NUM_CHANNELS] = b;
	_rgb[i + LINE_BYTES] = b;
	_rgb[i + LINE_BYTES + NUM_CHANNELS] = b;
}
