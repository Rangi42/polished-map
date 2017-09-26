#ifndef PALETTE_MAP_H
#define PALETTE_MAP_H

#include <cstring>

#include "utils.h"

#define MAX_NUM_TILES 256

class Palette_Map {
public:
	enum Palette { GRAY, RED, GREEN, WATER, YELLOW, BROWN, ROOF, TEXT, UNDEFINED };
	enum Result { PALETTE_OK, BAD_PALETTE_FILE, BAD_PALETTE_NAME, PALETTE_NULL };
private:
	Palette _palette[MAX_NUM_TILES];
	size_t _palette_size;
	Result _result;
public:
	Palette_Map(void);
	inline Palette palette(uint8_t i) const { return i < _palette_size ? _palette[i] : Palette::UNDEFINED; }
	inline size_t size(void) const { return _palette_size; }
	inline Result result(void) const { return _result; }
	void clear(void);
	Result read_from(const char *f);
	static const char *error_message(Result result);
};

#endif
