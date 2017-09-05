#ifndef COLORS_H
#define COLORS_H

#include <cstring>

#include "utils.h"

#define MAX_NUM_TILES 256

class Tile_Colors {
public:
	enum Color { GRAY, RED, GREEN, WATER, YELLOW, BROWN, ROOF, TEXT, UNDEFINED };
	enum Result { COLORS_OK, BAD_COLORS_FILE, BAD_COLOR_NAME, COLORS_NULL };
private:
	Color _colors[MAX_NUM_TILES];
	size_t _colors_size;
	Result _result;
public:
	Tile_Colors(void);
	Color color(uint8_t i) { return _colors[i]; }
	size_t size(void) const { return _colors_size; }
	Result result(void) const { return _result; }
	void clear(void);
	Result read_from(const char *f);
	static const char *error_message(Result result);
};

#endif
