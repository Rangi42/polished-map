#ifndef PALETTE_MAP_H
#define PALETTE_MAP_H

#include <cstring>

#include "utils.h"
#include "colors.h"

#define MAX_NUM_TILES 256
#define PALETTES_PER_LINE 8

class Palette_Map {
public:
	enum class Result { PALETTE_OK, BAD_PALETTE_FILE, BAD_PALETTE_NAME, PALETTE_TOO_LONG, PALETTE_NULL };
private:
	Palette _palette[MAX_NUM_TILES];
	size_t _palette_size;
	Result _result;
	int64_t _mod_time;
public:
	Palette_Map(void);
	inline Palette palette(uint8_t i) const { return i < _palette_size ? _palette[i] : Palette::UNDEFINED; }
	inline void palette(uint8_t i, Palette p) { _palette[i] = p; }
	inline size_t size(void) const { return _palette_size; }
	inline Result result(void) const { return _result; }
	inline bool other_modified(const char *f) const { return file_modified(f) > _mod_time; }
	void clear(void);
	Result read_from(const char *f);
	static const char *error_message(Result result);
	bool write_palette_map(const char *f);
};

#endif
