#include <cstdio>
#include <fstream>
#include <sstream>

#pragma warning(push, 0)
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "metatileset.h"
#include "parse-asm.h"

Metatileset::Metatileset() : _tileset(), _metatiles(), _num_metatiles(0), _result(Result::META_NULL), _modified(false),
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
	_result = Result::META_NULL;
	_modified = false;
	_bin_collisions = false;
}

void Metatileset::size(size_t n) {
	size_t low = std::min(n, _num_metatiles), high = std::max(n, _num_metatiles);
	for (size_t i = low; i < high; i++) {
		_metatiles[i]->clear();
	}
	_num_metatiles = n;
	_modified = true;
}

bool Metatileset::uses_tile(uint8_t id) const {
	for (size_t i = 0; i < _num_metatiles; i++) {
		if (_metatiles[i]->uses_tile(id)) {
			return true;
		}
	}
	return false;
}

void Metatileset::trim_tileset() {
	for (uint8_t i = MAX_NUM_TILES - 1; i > 0; i--) {
		Tile *t = _tileset.tile(i);
		if (t->palette() != Palette::UNDEFINED && (!t->is_blank() || uses_tile(i))) {
			break;
		}
		t->palette(Palette::UNDEFINED);
		_tileset.palette_map().palette(i, Palette::UNDEFINED);
	}
}

void Metatileset::draw_metatile(int x, int y, uint8_t id, bool zoom, bool show_priority) const {
	if (id < size()) {
		Metatile *mt = _metatiles[id];
		int s = TILE_SIZE * (zoom ? ZOOM_FACTOR : 1);
		for (int ty = 0; ty < METATILE_SIZE; ty++) {
			int ay = y + ty * s;
			for (int tx = 0; tx < METATILE_SIZE; tx++) {
				int ax = x + tx * s;
				uint8_t tid = mt->tile_id(tx, ty);
				const Tile *t = _tileset.const_tile_or_roof(tid);
				t->draw_with_priority(ax, ay, zoom ? TILE_PX_SIZE : TILE_SIZE, show_priority);
			}
		}
	}
	else {
		int s = TILE_SIZE * METATILE_SIZE * (zoom ? ZOOM_FACTOR : 1);
		fl_color(EMPTY_RGB);
		fl_rectf(x, y, s, s);
	}
}

uchar *Metatileset::print_rgb(const Map &map) const {
	int w = map.width(), h = map.height();
	int bw = w * METATILE_PX_SIZE, bh = h * METATILE_PX_SIZE;
	uchar *buffer = new uchar[bw * bh * NUM_CHANNELS]();
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			Block *b = map.block((uint8_t)x, (uint8_t)y);
			const Metatile *m = _metatiles[b->id()];
			for (int ty = 0; ty < METATILE_SIZE; ty++) {
				for (int tx = 0; tx < METATILE_SIZE; tx++) {
					uint8_t tid = m->tile_id(tx, ty);
					const Tile *t = _tileset.const_tile_or_roof(tid);
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
	if (!_tileset.num_tiles()) { return (_result = Result::META_NO_GFX); } // no graphics

	if (ends_with(f, ".CEL")) { return read_asm_metatiles(f); }

	FILE *file = fl_fopen(f, "rb");
	if (file == NULL) { return (_result = Result::META_BAD_FILE); } // cannot load file

	uchar data[METATILE_SIZE * METATILE_SIZE] = {};
	while (!feof(file)) {
		size_t c = fread(data, 1, METATILE_SIZE * METATILE_SIZE, file);
		if (!c) { break; } // end of file
		if (c < METATILE_SIZE * METATILE_SIZE) { fclose(file); return (_result = Result::META_TOO_SHORT); }
		if (_num_metatiles == MAX_NUM_METATILES) { fclose(file); return (_result = Result::META_TOO_LONG); }
		Metatile *mt = _metatiles[_num_metatiles++];
		for (int y = 0; y < METATILE_SIZE; y++) {
			for (int x = 0; x < METATILE_SIZE; x++) {
				mt->tile_id(x, y, data[y * METATILE_SIZE + x]);
			}
		}
	}

	fclose(file);
	return (_result = Result::META_OK);
}

Metatileset::Result Metatileset::read_asm_metatiles(const char *f) {
	Parsed_Asm data(f);
	if (data.result() != Parsed_Asm::Result::ASM_OK) {
		return (_result = Result::META_BAD_FILE); // cannot parse file
	}

	size_t c = data.size();
	size_t n = c / (METATILE_SIZE * METATILE_SIZE);
	_num_metatiles = std::min(n, (size_t)MAX_NUM_METATILES);

	for (size_t i = 0; i < _num_metatiles; i++) {
		Metatile *mt = _metatiles[i];
		int off = (int)i * METATILE_SIZE * METATILE_SIZE;
		for (int y = 0; y < METATILE_SIZE; y++) {
			for (int x = 0; x < METATILE_SIZE; x++) {
				mt->tile_id(x, y, data.get(off  + y * METATILE_SIZE + x));
			}
		}
	}

	if (n > MAX_NUM_METATILES) { return (_result = Result::META_TOO_LONG); }
	if (c % (METATILE_SIZE * METATILE_SIZE)) { return (_result = Result::META_TOO_SHORT); }
	return (_result = Result::META_OK);
}

bool Metatileset::write_metatiles(const char *f) const {
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

Metatileset::Result Metatileset::read_asm_collisions(const char *f) {
	if (!_tileset.num_tiles()) { return (_result = Result::META_NO_GFX); } // no graphics

	std::ifstream ifs;
	open_ifstream(ifs, f);
	if (!ifs.is_open()) { return (_result = Result::META_BAD_FILE); } // cannot load file

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
	return (_result = Result::META_OK);
}

Metatileset::Result Metatileset::read_bin_collisions(const char *f) {
	if (!_tileset.num_tiles()) { return (_result = Result::META_NO_GFX); } // no graphics

	FILE *file = fl_fopen(f, "rb");
	if (file == NULL) { return (_result = Result::META_BAD_FILE); } // cannot load file

	size_t i = 0;
	uchar data[NUM_QUADRANTS] = {};
	while (!feof(file)) {
		size_t c = fread(data, 1, NUM_QUADRANTS, file);
		if (!c) { break; } // end of file
		if (c < NUM_QUADRANTS) { fclose(file); return (_result = Result::META_TOO_SHORT); }
		for (int j = 0; j < NUM_QUADRANTS; j++) {
			_metatiles[i]->bin_collision((Quadrant)j, data[j]);
		}
		if (++i == _num_metatiles) { break; }
	}

	fclose(file);
	_bin_collisions = true;
	return (_result = Result::META_OK);
}

bool Metatileset::write_asm_collisions(const char *f) const {
	FILE *file = fl_fopen(f, "wb");
	if (!file) { return false; }
	for (size_t i = 0; i < _num_metatiles; i++) {
		Metatile *mt = _metatiles[i];
		fprintf(file, "\ttilecoll %s, %s, %s, %s ; %02x\n",
			mt->collision(Quadrant::TOP_LEFT).c_str(), mt->collision(Quadrant::TOP_RIGHT).c_str(),
			mt->collision(Quadrant::BOTTOM_LEFT).c_str(), mt->collision(Quadrant::BOTTOM_RIGHT).c_str(), (uint32_t)i);
	}
	fclose(file);
	return true;
}

bool Metatileset::write_bin_collisions(const char *f) const {
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
	case Result::META_OK:
		return "OK.";
	case Result::META_NO_GFX:
		return "No corresponding graphics file chosen.";
	case Result::META_BAD_FILE:
		return "Cannot open file.";
	case Result::META_TOO_SHORT:
		return "The last block is incomplete.";
	case Result::META_TOO_LONG:
		return "More than 256 blocks defined.";
	case Result::META_NULL:
		return "No blockset file chosen.";
	default:
		return "Unspecified error.";
	}
}
