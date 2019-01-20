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
#define TILE_AREA (TILE_SIZE * TILE_SIZE)
#define ZOOM_FACTOR 2

#define LINE_PX (TILE_SIZE * ZOOM_FACTOR)
#define LINE_BYTES (LINE_PX * NUM_CHANNELS)
#define TILE_BYTES (LINE_BYTES * LINE_PX)

#define TILE_PIXEL_OFFSET(x, y) (((y) * LINE_BYTES + (x) * NUM_CHANNELS) * ZOOM_FACTOR)

class Tile {
protected:
	uint8_t _id;
	bool _undefined;
	Hue _hues[TILE_AREA];
	uchar _rgb[NUM_PALETTES][TILE_BYTES];
	uchar _monochrome_rgb[TILE_BYTES];
	uchar _undefined_rgb[TILE_BYTES];
public:
	Tile(uint8_t id);
	inline uint8_t id(void) const { return _id; }
	inline void id(uint8_t id) { _id = id; }
	inline bool undefined(void) const { return _undefined; }
	inline void undefined(bool u) { _undefined = u; }
	inline const uchar *rgb(Palette p) const { return _undefined ? _undefined_rgb : _rgb[p]; }
	inline Hue hue(int x, int y) const { return _hues[y * TILE_SIZE + x]; }
	inline uchar *colored_pixel(Palette p, int x, int y) { return &_rgb[p][TILE_PIXEL_OFFSET(x, y)]; }
	inline const uchar *const_colored_pixel(Palette p, int x, int y) const { return &_rgb[p][TILE_PIXEL_OFFSET(x, y)]; }
	inline uchar *monochrome_pixel(int x, int y) { return _monochrome_rgb + TILE_PIXEL_OFFSET(x, y); }
	inline const uchar *const_monochrome_pixel(int x, int y) const { return _monochrome_rgb + TILE_PIXEL_OFFSET(x, y); }
	inline uchar *undefined_pixel(int x, int y) { return _undefined_rgb + TILE_PIXEL_OFFSET(x, y); }
	inline const uchar *const_undefined_pixel(int x, int y) const { return _undefined_rgb + TILE_PIXEL_OFFSET(x, y); }
	void clear(void);
	void copy(const Tile *t);
	void render_pixel(int x, int y, Lighting l, Hue h);
	void update_lighting(Lighting l);
};

#endif
