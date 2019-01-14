#ifndef TILE_H
#define TILE_H

#include <cstdlib>
#include <cstring>

#pragma warning(push, 0)
#include <FL/fl_types.h>
#pragma warning(pop)

#include "utils.h"
#include "colors.h"
#include "attributable.h"

#define TILE_SIZE 8
#define ZOOM_FACTOR 2

#define LINE_PX (TILE_SIZE * ZOOM_FACTOR)
#define LINE_BYTES (LINE_PX * NUM_CHANNELS)

class Tile {
protected:
	uint8_t _id;
	Hue _hues[TILE_SIZE * TILE_SIZE];
	uchar _rgb[NUM_PALETTES][LINE_PX * LINE_PX * NUM_CHANNELS];
	uchar _monochrome_rgb[LINE_PX * LINE_PX * NUM_CHANNELS];
public:
	Tile(uint8_t id);
	inline uint8_t id(void) const { return _id; }
	inline void id(uint8_t id) { _id = id; }
	inline const uchar *rgb(Palette p) const { return _rgb[p & 0xf]; }
	inline const uchar *monochrome_rgb(void) const { return _monochrome_rgb; }
	inline Hue hue(int x, int y) const { return _hues[y * TILE_SIZE + x]; }
	inline uchar *pixel(Palette p, int x, int y) {
		return &_rgb[p & 0xf][(y * LINE_BYTES + x * NUM_CHANNELS) * ZOOM_FACTOR];
	}
	inline const uchar *const_pixel(Palette p, int x, int y) const {
		return &_rgb[p & 0xf][(y * LINE_BYTES + x * NUM_CHANNELS) * ZOOM_FACTOR];
	}
	inline uchar *monochrome_pixel(int x, int y) {
		return _monochrome_rgb + (y * LINE_BYTES + x * NUM_CHANNELS) * ZOOM_FACTOR;
	}
	inline const uchar *const_monochrome_pixel(int x, int y) const {
		return _monochrome_rgb + (y * LINE_BYTES + x * NUM_CHANNELS) * ZOOM_FACTOR;
	}
	void pixel(Palette p, int x, int y, Hue h, uchar r, uchar g, uchar b);
	void monochrome_pixel(int x, int y, Hue h);
	void clear(void);
	void copy(const Tile *t);
	void draw_attributable(const Attributable *a, int x, int y, bool zoom) const;
	void update_lighting(Lighting l);
};

#endif
