#include "utils.h"
#include "tile.h"

Tile::Tile(uint8_t id) : _id(id), _hues(), _rgb() {}

void Tile::pixel(Palette p, int x, int y, Hue h, uchar r, uchar g, uchar b) {
	_hues[y * TILE_SIZE + x] = h;
	int c = (int)p & 0xf;
	int i = (y * LINE_BYTES + x * NUM_CHANNELS) * ZOOM_FACTOR;
	// red
	_rgb[c][i] = r;
	_rgb[c][i + NUM_CHANNELS] = r;
	_rgb[c][i + LINE_BYTES] = r;
	_rgb[c][i + LINE_BYTES + NUM_CHANNELS] = r;
	i++;
	// green
	_rgb[c][i] = g;
	_rgb[c][i + NUM_CHANNELS] = g;
	_rgb[c][i + LINE_BYTES] = g;
	_rgb[c][i + LINE_BYTES + NUM_CHANNELS] = g;
	i++;
	// blue
	_rgb[c][i] = b;
	_rgb[c][i + NUM_CHANNELS] = b;
	_rgb[c][i + LINE_BYTES] = b;
	_rgb[c][i + LINE_BYTES + NUM_CHANNELS] = b;
}

void Tile::clear() {
	FILL(_hues, Hue::WHITE, TILE_SIZE * TILE_SIZE);
	for (int i = 0; i < NUM_PALETTES; i++) {
		FILL(_rgb[i], 0xff, LINE_PX * LINE_PX * NUM_CHANNELS);
	}
}

void Tile::copy(const Tile *t) {
	memcpy(_hues, t->_hues, TILE_SIZE * TILE_SIZE * sizeof(Hue));
	for (int i = 0; i < NUM_PALETTES; i++) {
		memcpy(_rgb[i], t->_rgb[i], LINE_PX * LINE_PX * NUM_CHANNELS);
	}
}

void Tile::update_lighting(Lighting l) {
	for (int ty = 0; ty < TILE_SIZE; ty++) {
		for (int tx = 0; tx < TILE_SIZE; tx++) {
			Hue h = hue(tx, ty);
			for (int pi = 0; pi < NUM_PALETTES; pi++) {
				Palette p = (Palette)pi;
				const uchar *rgb = Color::color(l, p, h);
				pixel(p, tx, ty, h, rgb[0], rgb[1], rgb[2]);
			}
		}
	}
}
