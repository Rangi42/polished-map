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
	int s = TILE_SIZE * (z ? ZOOM_FACTOR : 1);
	int d = NUM_CHANNELS * (z ? 1 : ZOOM_FACTOR);
	int ld = LINE_BYTES * (z ? 1 : ZOOM_FACTOR);
	for (int ty = 0; ty < METATILE_SIZE; ty++) {
		for (int tx = 0; tx < METATILE_SIZE; tx++) {
			uint8_t tid = mt->tile_id(ty, tx);
			const Tile *t = _tileset.tile(tid);
			const uchar *rgb = t->rgb();
			fl_draw_image(rgb, x + tx * s, y + ty * s, s, s, d, ld);
		}
	}
}

Palette_Map::Result Metatileset::read_palette_map(const char *f) {
	return _tileset.read_palette_map(f);
}

Tileset::Result Metatileset::read_graphics(const char *f, Tileset::Lighting l) {
	return _tileset.read_graphics(f, l);
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

const char *Metatileset::error_message(Result result) {
	switch (result) {
	case META_OK:
		return "OK.";
	case META_NO_GFX:
		return "No corresponding graphics file chosen.";
	case META_BAD_FILE:
		return "Cannot open file.";
	case META_TOO_SHORT:
		return "File ends too early.";
	case META_NULL:
		return "No *_metatiles.bin file chosen.";
	default:
		return "Unspecified error.";
	}
}
