#ifndef METATILESET_H
#define METATILESET_H

#include "palette-map.h"
#include "tileset.h"
#include "metatile.h"

#define MAX_NUM_METATILES 256

class Metatileset {
public:
	enum Result { META_OK, META_NO_GFX, META_BAD_FILE, META_TOO_SHORT, META_NULL = -1 };
private:
	Tileset _tileset;
	Metatile *_metatiles[MAX_NUM_METATILES];
	size_t _num_metatiles;
	Result _result;
public:
	Metatileset();
	~Metatileset();
	Palette_Map::Result read_palette_map(const char *f);
	Tileset::Result read_2bpp_graphics(const char *f);
	Tileset::Result read_png_graphics(const char *f);
	Result read_metatiles(const char *f);
};

#endif
