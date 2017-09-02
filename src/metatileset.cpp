#include <cstdio>

#pragma warning(push, 0)
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "metatileset.h"

Metatileset::Metatileset() : _tileset(), _metatiles(), _num_metatiles(0), _result(META_NULL) {
	for (size_t i = 0; i < MAX_NUM_METATILES; i++) {
		_metatiles[i] = new Metatile((uint8_t)i);
	}
}

Metatileset::~Metatileset() {
	clear();
	for (size_t i = 0; i < MAX_NUM_METATILES; i++) {
		delete _metatiles[i];
	}
}

void Metatileset::clear() {
	_tileset.clear();
	for (size_t i = 0; i < MAX_NUM_METATILES; i++) {
		_metatiles[i]->clear();
	}
	_num_metatiles = 0;
	_result = META_NULL;
}

void Metatileset::draw_metatile(int x, int y, uint8_t id, bool z) {
	Metatile *mt = _metatiles[id];
	for (int ty = 0; ty < METATILE_SIZE; ty++) {
		for (int tx = 0; tx < METATILE_SIZE; tx++) {
			uint8_t tid = mt->tile_id(ty, tx);
			const Tile *t = _tileset.tile(tid);
			if (z) {
				const uchar *rgb2 = t->rgb2();
				fl_draw_image(rgb2, x + tx * TILE_SIZE * 2, y + ty * TILE_SIZE * 2, TILE_SIZE * 2, TILE_SIZE * 2);
			}
			else {
				const uchar *rgb = t->rgb();
				fl_draw_image(rgb, x + tx * TILE_SIZE, y + ty * TILE_SIZE, TILE_SIZE, TILE_SIZE);
			}
		}
	}
}

Palette_Map::Result Metatileset::read_palette_map(const char *f) {
	return _tileset.read_palette_map(f);
}

Tileset::Result Metatileset::read_2bpp_graphics(const char *f) {
	return _tileset.read_2bpp_graphics(f);
}

Tileset::Result Metatileset::read_png_graphics(const char *f) {
	return _tileset.read_png_graphics(f);
}

Metatileset::Result Metatileset::read_metatiles(const char *f) {
	if (!_tileset.num_tiles()) { return (_result = META_NO_GFX); } // no graphics

	FILE *file = fopen(f, "rb");
	if (file == NULL) { return (_result = META_BAD_FILE); } // cannot load file

	uchar data[METATILE_SIZE * METATILE_SIZE];
	while (!feof(file)) {
		size_t c = fread(data, 1, METATILE_SIZE * METATILE_SIZE, file);
		if (!c) { break; } // end of file
		if (c < METATILE_SIZE * METATILE_SIZE) { return (_result = META_TOO_SHORT); } // too-short metatile
		Metatile *mt = _metatiles[_num_metatiles++];
		for (int y = 0; y < METATILE_SIZE; y++) {
			for (int x = 0; x < METATILE_SIZE; x++) {
				mt->tile_id(y, x, data[y * METATILE_SIZE + x]);
			}
		}
	}

	return (_result = META_OK);
}
