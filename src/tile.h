#ifndef TILE_H
#define TILE_H

#include <cstdlib>
#include <cstring>

#pragma warning(push, 0)
#include <FL/fl_types.h>
#pragma warning(pop)

#include "utils.h"
#include "colors.h"

#define TILE_SIZE 8
#define TILE_AREA (TILE_SIZE * TILE_SIZE)
#define ZOOM_FACTOR 2

#define TILE_PX_SIZE (TILE_SIZE * ZOOM_FACTOR)

#define LINE_PX (TILE_SIZE * ZOOM_FACTOR)
#define LINE_BYTES (LINE_PX * NUM_CHANNELS)

#define CHIP_ZOOM_FACTOR 3
#define CHIP_PX_SIZE (TILE_SIZE * CHIP_ZOOM_FACTOR)
#define CHIP_LINE_BYTES (CHIP_PX_SIZE * NUM_CHANNELS)

class Tile {
protected:
	uint8_t _id;
	Palette _palette;
	Hue _hues[TILE_AREA];
	uchar _rgb[LINE_PX * LINE_PX * NUM_CHANNELS];
public:
	Tile(uint8_t id);
	inline uint8_t id(void) const { return _id; }
	inline void id(uint8_t id) { _id = id; }
	inline Palette palette(void) const { return _palette; }
	inline void palette(Palette p) { _palette = p; }
	inline bool priority(void) const { return _palette >= Palette::PRIORITY_GRAY; }
	inline const uchar *rgb(void) const { return _rgb; }
	inline Hue hue(int x, int y) const { return _hues[y * TILE_SIZE + x]; }
	inline uchar *pixel(int x, int y) { return _rgb + (y * LINE_BYTES + x * NUM_CHANNELS) * ZOOM_FACTOR; }
	inline const uchar *const_pixel(int x, int y) const { return _rgb + (y * LINE_BYTES + x * NUM_CHANNELS) * ZOOM_FACTOR; }
	bool is_blank(void) const;
	void pixel(int x, int y, Hue h, uchar r, uchar g, uchar b);
	void clear(void);
	void copy(const Tile *t);
	void update_palettes(Palettes l);
	void draw_with_priority(int x, int y, int s, bool show_priority) const;
	void draw_for_clipboard(int x, int y);
};

#endif
