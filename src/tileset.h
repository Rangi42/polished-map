#ifndef TILESET_H
#define TILESET_H

#include "tile.h"
#include "palette-map.h"

class Tileset {
private:
	Palette_Map _palette_map;
	Tile _tiles[MAX_TILESET_SIZE];
public:
	Tileset();
	bool read_palette_map(const char *f);
	bool read_2bpp_graphics(const char *f);
	bool read_png_graphics(const char *f);
};

#endif
