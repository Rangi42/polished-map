#include <cstdio>

#pragma warning(push, 0)
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "metatileset.h"

Metatileset::Metatileset() : _tileset(), _metatiles(), _num_metatiles(0), _result(META_NULL), _modified(false) {
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
	_modified = false;
}

void Metatileset::size(size_t n) {
	size_t low = MIN(n, _num_metatiles), high = MAX(n, _num_metatiles);
	for (size_t i = low; i < high; i++) {
		_metatiles[i]->clear();
	}
	_num_metatiles = n;
	_modified = true;
}

void Metatileset::draw_metatile(int x, int y, uint8_t id, bool z) const {
	if (id < size()) {
		Metatile *mt = _metatiles[id];
		int s = TILE_SIZE * (z ? ZOOM_FACTOR : 1);
		int d = NUM_CHANNELS * (z ? 1 : ZOOM_FACTOR);
		int ld = LINE_BYTES * (z ? 1 : ZOOM_FACTOR);
		for (int ty = 0; ty < METATILE_SIZE; ty++) {
			for (int tx = 0; tx < METATILE_SIZE; tx++) {
				uint8_t tid = mt->tile_id(tx, ty);
				const Tile *t = _tileset.tile(tid);
				const uchar *rgb = t->rgb();
				fl_draw_image(rgb, x + tx * s, y + ty * s, s, s, d, ld);
			}
		}
	}
	else {
		int s = TILE_SIZE * METATILE_SIZE * (z ? ZOOM_FACTOR : 1);
		fl_color(EMPTY_RGB);
		fl_rectf(x, y, s, s);
	}
}

uchar *Metatileset::print_rgb(const Map &map) const {
	int w = map.width(), h = map.height();
	int bw = w * METATILE_SIZE * TILE_SIZE, bh = h * METATILE_SIZE * TILE_SIZE;
	uchar *buffer = new uchar[bw * bh * NUM_CHANNELS]();
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			Block *b = map.block((uint8_t)x, (uint8_t)y);
			const Metatile *m = _metatiles[b->id()];
			for (int ty = 0; ty < METATILE_SIZE; ty++) {
				for (int tx = 0; tx < METATILE_SIZE; tx++) {
					uint8_t tid = m->tile_id(tx, ty);
					const Tile *t = _tileset.tile(tid);
					size_t o = ((y * METATILE_SIZE + ty) * bw + x * METATILE_SIZE + tx) * TILE_SIZE * NUM_CHANNELS;
					for (int py = 0; py < TILE_SIZE; py++) {
						for (int px = 0; px < TILE_SIZE; px++) {
							const uchar *rgb = t->const_pixel(px, py);
							size_t j = o + (py * bw + px) * NUM_CHANNELS;
							buffer[j++] = rgb[0];
							buffer[j++] = rgb[1];
							buffer[j] = rgb[2];
						}
					}
				}
			}
		}
	}
	return buffer;
}

Metatileset::Result Metatileset::read_metatiles(const char *f) {
	if (!_tileset.num_tiles()) { return (_result = META_NO_GFX); } // no graphics

	FILE *file = fl_fopen(f, "rb");
	if (file == NULL) { return (_result = META_BAD_FILE); } // cannot load file

	uchar data[METATILE_SIZE * METATILE_SIZE];
	while (!feof(file)) {
		size_t c = fread(data, 1, METATILE_SIZE * METATILE_SIZE, file);
		if (!c) { break; } // end of file
		if (c < METATILE_SIZE * METATILE_SIZE) { return (_result = META_TOO_SHORT); } // too-short metatile
		Metatile *mt = _metatiles[_num_metatiles++];
		for (int y = 0; y < METATILE_SIZE; y++) {
			for (int x = 0; x < METATILE_SIZE; x++) {
				mt->tile_id(x, y, data[y * METATILE_SIZE + x]);
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
		return "The last block is incomplete.";
	case META_NULL:
		return "No blockset file chosen.";
	default:
		return "Unspecified error.";
	}
}
