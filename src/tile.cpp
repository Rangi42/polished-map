#pragma warning(push, 0)
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "utils.h"
#include "tile.h"

Tile::Tile(uint8_t id) : _id(id), _undefined(true), _hues(), _rgb() {}

static void fill_tile_pixel(uchar *tile, int x, int y, const uchar *rgb) {
	int i = TILE_PIXEL_OFFSET(x, y);
	// red
	tile[i] = rgb[0];
	tile[i + NUM_CHANNELS] = rgb[0];
	tile[i + LINE_BYTES] = rgb[0];
	tile[i + LINE_BYTES + NUM_CHANNELS] = rgb[0];
	i++;
	// green
	tile[i] = rgb[1];
	tile[i + NUM_CHANNELS] = rgb[1];
	tile[i + LINE_BYTES] = rgb[1];
	tile[i + LINE_BYTES + NUM_CHANNELS] = rgb[1];
	i++;
	// blue
	tile[i] = rgb[2];
	tile[i + NUM_CHANNELS] = rgb[2];
	tile[i + LINE_BYTES] = rgb[2];
	tile[i + LINE_BYTES + NUM_CHANNELS] = rgb[2];
}

void Tile::pixel(Palette p, int x, int y, Hue h, uchar r, uchar g, uchar b) {
	_hues[y * TILE_SIZE + x] = h;
	uchar rgb[NUM_CHANNELS] = {r, g, b};
	fill_tile_pixel(_rgb[p], x, y, rgb);
}

void Tile::monochrome_pixel(int x, int y, Hue h) {
	const uchar *rgb = Color::monochrome_color(h);
	fill_tile_pixel(_monochrome_rgb, x, y, rgb);
}

void Tile::undefined_pixel(int x, int y, Hue h) {
	const uchar *rgb = Color::undefined_color(h);
	fill_tile_pixel(_undefined_rgb, x, y, rgb);
}

void Tile::clear() {
	_undefined = true;
	FILL(_hues, Hue::WHITE, TILE_AREA);
	for (int i = 0; i < NUM_PALETTES; i++) {
		FILL(_rgb[i], 0xff, TILE_BYTES);
	}
	FILL(_monochrome_rgb, 0xff, TILE_BYTES);
	FILL(_undefined_rgb, 0xff, TILE_BYTES);
}

void Tile::copy(const Tile *t) {
	_undefined = t->_undefined;
	memcpy(_hues, t->_hues, TILE_AREA * sizeof(Hue));
	for (int i = 0; i < NUM_PALETTES; i++) {
		memcpy(_rgb[i], t->_rgb[i], TILE_BYTES);
	}
	memcpy(_monochrome_rgb, t->_monochrome_rgb, TILE_BYTES);
	memcpy(_undefined_rgb, t->_undefined_rgb, TILE_BYTES);
}

void Tile::draw_attributable(const Attributable *a, int x, int y, bool zoom) const {
	const uchar *buffer = rgb(a->palette());
	int s = TILE_SIZE * (zoom ? ZOOM_FACTOR : 1);
	int k = zoom ? 1 : ZOOM_FACTOR;
	int d = NUM_CHANNELS * k;
	int ld = LINE_BYTES * k;
	buffer += a->y_flip()
		? a->x_flip() ? (LINE_PX * LINE_PX - k) * NUM_CHANNELS : LINE_BYTES * (LINE_PX - 1)
		: a->x_flip() ? (LINE_PX - 1) * k * NUM_CHANNELS : 0;
	int td = a->x_flip() ? -d : d;
	int tld = a->y_flip() ? -ld : ld;
	fl_draw_image(buffer, x, y, s, s, td, tld);
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
			monochrome_pixel(tx, ty, h);
			undefined_pixel(tx, ty, h);
		}
	}
}
