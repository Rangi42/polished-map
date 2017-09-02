#ifndef METATILESET_H
#define METATILESET_H

#include "palette-map.h"
#include "tileset.h"
#include "metatile.h"

#define MAX_NUM_METATILES 256

class Metatileset {
public:
	enum Result { META_OK, META_NO_GFX, META_BAD_FILE, META_TOO_SHORT, META_NULL };
private:
	Tileset _tileset;
	Metatile *_metatiles[MAX_NUM_METATILES];
	size_t _num_metatiles;
	Result _result;
public:
	Metatileset();
	~Metatileset();
	size_t num_metatiles(void) const { return _num_metatiles; }
	Result result(void) const { return _result; }
	void clear(void);
	void draw_metatile(int x, int y, uint8_t id, bool z);
	Palette_Map::Result read_palette_map(const char *f);
	Tileset::Result read_2bpp_graphics(const char *f);
	Tileset::Result read_png_graphics(const char *f);
	Result read_metatiles(const char *f);
};

#endif
