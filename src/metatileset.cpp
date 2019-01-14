#include <cstdio>
#include <fstream>
#include <sstream>

#pragma warning(push, 0)
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "metatileset.h"

Metatileset::Metatileset() : _tileset(), _metatiles(), _num_metatiles(0), _result(META_NULL), _modified(false),
	_bin_collisions(false) {
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
	_bin_collisions = false;
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
				const Attributable *a = mt->attributes(tx, ty);
				const Tile *t = _tileset.const_tile_or_roof(a->id());
				const uchar *rgb = t->rgb(a->palette());
				rgb += a->y_flip()
					? a->x_flip()
						? NUM_CHANNELS * (LINE_PX * LINE_PX - (z ? 1 : ZOOM_FACTOR))
						: LINE_BYTES * (LINE_PX - 1)
					: a->x_flip()
						? (LINE_PX - 1) * NUM_CHANNELS * (z ? 1 : ZOOM_FACTOR)
						: 0;
				int td = a->x_flip() ? -d : d;
				int tld = a->y_flip() ? -ld : ld;
				fl_draw_image(rgb, x + tx * s, y + ty * s, s, s, td, tld);
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
					const Attributable *a = m->attributes(tx, ty);
					const Tile *t = _tileset.const_tile_or_roof(a->id());
					size_t o = ((y * METATILE_SIZE + ty) * bw + x * METATILE_SIZE + tx) * TILE_SIZE * NUM_CHANNELS;
					for (int py = 0; py < TILE_SIZE; py++) {
						int my = a->y_flip() ? TILE_SIZE - py - 1 : py;
						for (int px = 0; px < TILE_SIZE; px++) {
							int mx = a->x_flip() ? TILE_SIZE - px - 1 : px;
							const uchar *rgb = t->const_pixel(a->palette(), mx, my);
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

	uchar data[METATILE_SIZE * METATILE_SIZE] = {};
	while (!feof(file)) {
		size_t c = fread(data, 1, METATILE_SIZE * METATILE_SIZE, file);
		if (!c) { break; } // end of file
		if (c < METATILE_SIZE * METATILE_SIZE) { fclose(file); return (_result = META_TOO_SHORT); }
		if (_num_metatiles == MAX_NUM_METATILES) { fclose(file); return (_result = META_TOO_LONG); }
		Metatile *mt = _metatiles[_num_metatiles++];
		for (int y = 0; y < METATILE_SIZE; y++) {
			for (int x = 0; x < METATILE_SIZE; x++) {
				mt->tile_id(x, y, data[y * METATILE_SIZE + x]);
			}
		}
	}

	fclose(file);
	return (_result = META_OK);
}

bool Metatileset::write_metatiles(const char *f) {
	FILE *file = fl_fopen(f, "wb");
	if (!file) { return false; }
	for (size_t i = 0; i < _num_metatiles; i++) {
		Metatile *mt = _metatiles[i];
		for (int y = 0; y < METATILE_SIZE; y++) {
			for (int x = 0; x < METATILE_SIZE; x++) {
				uint8_t id = mt->tile_id(x, y);
				fputc(id, file);
			}
		}
	}
	fclose(file);
	return true;
}

Metatileset::Result Metatileset::read_attributes(const char *f) {
	if (!_tileset.num_tiles()) { return (_result = META_NO_GFX); } // no graphics

	FILE *file = fl_fopen(f, "rb");
	if (file == NULL) { return (_result = META_BAD_FILE); } // cannot load file

	uchar data[METATILE_SIZE * METATILE_SIZE] = {};
	size_t i = 0;
	while (!feof(file)) {
		size_t c = fread(data, 1, METATILE_SIZE * METATILE_SIZE, file);
		if (!c) { break; } // end of file
		if (c < METATILE_SIZE * METATILE_SIZE) { fclose(file); return (_result = META_TOO_SHORT); }
		if (i >= _num_metatiles) { fclose(file); return (_result = META_TOO_LONG); }
		Metatile *mt = _metatiles[i++];
		for (int y = 0; y < METATILE_SIZE; y++) {
			for (int x = 0; x < METATILE_SIZE; x++) {
				uchar a = data[y * METATILE_SIZE + x];
				mt->attribute_byte(x, y, a);
			}
		}
	}

	fclose(file);
	return (_result = META_OK);
}

bool Metatileset::write_attributes(const char *f) {
	FILE *file = fl_fopen(f, "wb");
	if (!file) { return false; }
	for (size_t i = 0; i < _num_metatiles; i++) {
		Metatile *mt = _metatiles[i];
		for (int y = 0; y < METATILE_SIZE; y++) {
			for (int x = 0; x < METATILE_SIZE; x++) {
				uchar a = mt->attribute_byte(x, y);
				fputc(a, file);
			}
		}
	}
	fclose(file);
	return true;
}

Metatileset::Result Metatileset::read_asm_collisions(const char *f) {
	if (!_tileset.num_tiles()) { return (_result = META_NO_GFX); } // no graphics

	std::ifstream ifs(f);
	if (!ifs.is_open()) { return (_result = META_BAD_FILE); } // cannot load file

	size_t i = 0;
	while (ifs.good()) {
		std::string line;
		std::getline(ifs, line);
		trim(line);
		if (!starts_with(line, "tilecoll")) { continue; }
		line.erase(0, strlen("tilecoll") + 1); // include next whitespace character
		std::istringstream lss(line);
		std::string c1, c2, c3, c4;
		std::getline(lss, c1, ','); trim(c1);
		std::getline(lss, c2, ','); trim(c2);
		std::getline(lss, c3, ','); trim(c3);
		std::getline(lss, c4, ';'); trim(c4);
		_metatiles[i]->collision(Quadrant::TOP_LEFT, c1);
		_metatiles[i]->collision(Quadrant::TOP_RIGHT, c2);
		_metatiles[i]->collision(Quadrant::BOTTOM_LEFT, c3);
		_metatiles[i]->collision(Quadrant::BOTTOM_RIGHT, c4);
		if (++i == _num_metatiles) { break; }
	}

	_bin_collisions = false;
	return (_result = META_OK);
}

Metatileset::Result Metatileset::read_bin_collisions(const char *f) {
	if (!_tileset.num_tiles()) { return (_result = META_NO_GFX); } // no graphics

	FILE *file = fl_fopen(f, "rb");
	if (file == NULL) { return (_result = META_BAD_FILE); } // cannot load file

	size_t i = 0;
	uchar data[NUM_QUADRANTS] = {};
	while (!feof(file)) {
		size_t c = fread(data, 1, NUM_QUADRANTS, file);
		if (!c) { break; } // end of file
		if (c < NUM_QUADRANTS) { fclose(file); return (_result = META_TOO_SHORT); }
		for (int j = 0; j < NUM_QUADRANTS; j++) {
			_metatiles[i]->bin_collision((Quadrant)j, data[j]);
		}
		if (++i == _num_metatiles) { break; }
	}

	fclose(file);
	_bin_collisions = true;
	return (_result = META_OK);
}

bool Metatileset::write_asm_collisions(const char *f) {
	FILE *file = fl_fopen(f, "wb");
	if (!file) { return false; }
	for (size_t i = 0; i < _num_metatiles; i++) {
		Metatile *mt = _metatiles[i];
		fprintf(file, "\ttilecoll %s, %s, %s, %s ; %02x\n",
			mt->collision(Quadrant::TOP_LEFT).c_str(), mt->collision(Quadrant::TOP_RIGHT).c_str(),
			mt->collision(Quadrant::BOTTOM_LEFT).c_str(), mt->collision(Quadrant::BOTTOM_RIGHT).c_str(), i);
	}
	fclose(file);
	return true;
}

bool Metatileset::write_bin_collisions(const char *f) {
	FILE *file = fl_fopen(f, "wb");
	if (!file) { return false; }
	for (size_t i = 0; i < _num_metatiles; i++) {
		Metatile *mt = _metatiles[i];
		fwrite(mt->bin_collisions(), 1, NUM_QUADRANTS, file);
	}
	fclose(file);
	return true;
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
	case META_TOO_LONG:
		return "More than 256 blocks defined.";
	case META_NULL:
		return "No blockset file chosen.";
	default:
		return "Unspecified error.";
	}
}
