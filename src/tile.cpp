#include "utils.h"
#include "tile.h"

Tile::Tile(uint8_t id) : _id(id), _palette(Palette::UNDEFINED), _hues(), _rgb() {}

void Tile::pixel(int x, int y, Hue h, uchar r, uchar g, uchar b) {
	_hues[y * TILE_SIZE + x] = h;
	int i = (y * LINE_BYTES + x * NUM_CHANNELS) * ZOOM_FACTOR;
	// red
	_rgb[i] = r;
	_rgb[i + NUM_CHANNELS] = r;
	_rgb[i + LINE_BYTES] = r;
	_rgb[i + LINE_BYTES + NUM_CHANNELS] = r;
	i++;
	// green
	_rgb[i] = g;
	_rgb[i + NUM_CHANNELS] = g;
	_rgb[i + LINE_BYTES] = g;
	_rgb[i + LINE_BYTES + NUM_CHANNELS] = g;
	i++;
	// blue
	_rgb[i] = b;
	_rgb[i + NUM_CHANNELS] = b;
	_rgb[i + LINE_BYTES] = b;
	_rgb[i + LINE_BYTES + NUM_CHANNELS] = b;
}

void Tile::clear() {
	FILL(_hues, Hue::WHITE, TILE_SIZE * TILE_SIZE);
	FILL(_rgb, 0xff, LINE_PX * LINE_PX * NUM_CHANNELS);
}

void Tile::copy(const Tile *t) {
	_palette = t->_palette;
	memcpy(_hues, t->_hues, TILE_SIZE * TILE_SIZE * sizeof(Hue));
	memcpy(_rgb, t->_rgb, LINE_PX * LINE_PX * NUM_CHANNELS);
}

void Tile::update_lighting(Lighting l) {
	for (int ty = 0; ty < TILE_SIZE; ty++) {
		for (int tx = 0; tx < TILE_SIZE; tx++) {
			Hue h = hue(tx, ty);
			const uchar *rgb = Color::color(l, _palette, h);
			pixel(tx, ty, h, rgb[0], rgb[1], rgb[2]);
		}
	}
}
