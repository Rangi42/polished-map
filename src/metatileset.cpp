#include <cstdio>
#include <fstream>
#include <sstream>

#pragma warning(push, 0)
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "config.h"
#include "metatileset.h"

// 8x8 translucent zigzag pattern for tile priority
static uchar small_priority_png_buffer[119] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x02, 0x03, 0x00, 0x00, 0x00, 0xb9, 0x61, 0x56,
	0x18, 0x00, 0x00, 0x00, 0x0c, 0x50, 0x4c, 0x54, 0x45, 0x68, 0xd8, 0xd8, 0x56, 0xba, 0xba, 0x32,
	0x7e, 0x7e, 0x20, 0x60, 0x60, 0xb4, 0x24, 0x6a, 0xe6, 0x00, 0x00, 0x00, 0x04, 0x74, 0x52, 0x4e,
	0x53, 0x60, 0x60, 0x60, 0x60, 0xe8, 0x2d, 0x50, 0x46, 0x00, 0x00, 0x00, 0x16, 0x49, 0x44, 0x41,
	0x54, 0x78, 0x5e, 0x63, 0x90, 0x90, 0x60, 0xa8, 0xab, 0x63, 0x78, 0xfe, 0x9c, 0xa1, 0xb1, 0x11,
	0x99, 0x0d, 0x00, 0x55, 0xe0, 0x07, 0xf9, 0x01, 0x25, 0x72, 0xd2, 0x00, 0x00, 0x00, 0x00, 0x49,
	0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static Fl_PNG_Image small_priority_png(NULL, small_priority_png_buffer, 119);

// 16x16 translucent zigzag pattern for tile priority
static uchar large_priority_png_buffer[143] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x02, 0x03, 0x00, 0x00, 0x00, 0x62, 0x9d, 0x17,
	0xf2, 0x00, 0x00, 0x00, 0x0c, 0x50, 0x4c, 0x54, 0x45, 0x20, 0x60, 0x60, 0x32, 0x7e, 0x7e, 0x56,
	0xba, 0xba, 0x68, 0xd8, 0xd8, 0xdc, 0xe3, 0x64, 0x6e, 0x00, 0x00, 0x00, 0x04, 0x74, 0x52, 0x4e,
	0x53, 0x60, 0x60, 0x60, 0x60, 0xe8, 0x2d, 0x50, 0x46, 0x00, 0x00, 0x00, 0x2e, 0x49, 0x44, 0x41,
	0x54, 0x78, 0x5e, 0x63, 0xf8, 0x57, 0xff, 0xaf, 0x9e, 0xe1, 0x87, 0xfc, 0x0f, 0x79, 0x86, 0x07,
	0xec, 0x0f, 0xd8, 0x19, 0x1a, 0x18, 0x1b, 0x18, 0x19, 0x80, 0xb8, 0x81, 0x01, 0xc8, 0x7b, 0xc0,
	0x20, 0x0f, 0x94, 0x61, 0x00, 0xaa, 0xf8, 0x47, 0xac, 0x3a, 0x00, 0xfb, 0x8c, 0x1f, 0xe1, 0x3f,
	0x4c, 0xa1, 0xea, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static Fl_PNG_Image large_priority_png(NULL, large_priority_png_buffer, 143);

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
	size_t low = MIN(n, _num_metatiles), high = MAX(n, _num_metatiles);
	for (size_t i = low; i < high; i++) {
		_metatiles[i]->clear();
	}
	_num_metatiles = n;
	_modified = true;
}

void Metatileset::draw_metatile(int x, int y, uint8_t id, bool zoom, bool show_priority) const {
	int s = TILE_SIZE * (zoom ? ZOOM_FACTOR : 1);
	if (id >= size()) {
		fl_color(EMPTY_RGB);
		fl_rectf(x, y, METATILE_SIZE * s, METATILE_SIZE * s);
		return;
	}
	int k = zoom ? 1 : ZOOM_FACTOR;
	int d = NUM_CHANNELS * k;
	int ld = LINE_BYTES * k;
	Metatile *mt = _metatiles[id];
	for (int ty = 0; ty < METATILE_SIZE; ty++) {
		int ay = y + ty * s;
		for (int tx = 0; tx < METATILE_SIZE; tx++) {
			int ax = x + tx * s;
			const Attributable *a = mt->attributes(tx, ty);
			const Tile *t = _tileset.const_tile_or_roof(a->index());
			const uchar *buffer = t->rgb(a->palette());
			buffer += a->y_flip()
				? a->x_flip() ? (LINE_PX * LINE_PX - k) * NUM_CHANNELS : LINE_BYTES * (LINE_PX - 1)
				: a->x_flip() ? (LINE_PX - 1) * k * NUM_CHANNELS : 0;
			int td = a->x_flip() ? -d : d;
			int tld = a->y_flip() ? -ld : ld;
			fl_draw_image(buffer, ax, ay, s, s, td, tld);
			if (show_priority && a->priority()) {
				(zoom ? large_priority_png : small_priority_png).draw(ax, ay, s, s);
			}
		}
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
					const Attributable *a = m->attributes(tx, ty);
					const Tile *t = _tileset.const_tile_or_roof(a->index());
					size_t o = ((y * METATILE_SIZE + ty) * bw + x * METATILE_SIZE + tx) * TILE_SIZE * NUM_CHANNELS;
					for (int py = 0; py < TILE_SIZE; py++) {
						int my = a->y_flip() ? TILE_SIZE - py - 1 : py;
						for (int px = 0; px < TILE_SIZE; px++) {
							int mx = a->x_flip() ? TILE_SIZE - px - 1 : px;
							const uchar *rgb = t->const_colored_pixel(a->palette(), mx, my);
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
				uchar v = data[y * METATILE_SIZE + x];
				mt->tile_byte(x, y, v);
			}
		}
	}

	fclose(file);
	return (_result = Result::META_OK);
}

bool Metatileset::write_metatiles(const char *f) const {
	FILE *file = fl_fopen(f, "wb");
	if (!file) { return false; }
	for (size_t i = 0; i < _num_metatiles; i++) {
		Metatile *mt = _metatiles[i];
		for (int y = 0; y < METATILE_SIZE; y++) {
			for (int x = 0; x < METATILE_SIZE; x++) {
				uchar t = mt->tile_byte(x, y);
				fputc(t, file);
			}
		}
	}
	fclose(file);
	return true;
}

Metatileset::Result Metatileset::read_attributes(const char *f) {
	if (!_tileset.num_tiles()) { return (_result = Result::META_NO_GFX); } // no graphics

	FILE *file = fl_fopen(f, "rb");
	if (file == NULL) { return (_result = Result::META_BAD_FILE); } // cannot load file

	uchar data[METATILE_SIZE * METATILE_SIZE] = {};
	size_t i = 0;
	while (!feof(file)) {
		size_t c = fread(data, 1, METATILE_SIZE * METATILE_SIZE, file);
		if (!c) { break; } // end of file
		if (c < METATILE_SIZE * METATILE_SIZE) { fclose(file); return (_result = Result::META_TOO_SHORT); }
		if (i >= _num_metatiles) { fclose(file); return (_result = Result::META_TOO_LONG); }
		Metatile *mt = _metatiles[i++];
		for (int y = 0; y < METATILE_SIZE; y++) {
			for (int x = 0; x < METATILE_SIZE; x++) {
				uchar a = data[y * METATILE_SIZE + x];
				mt->attribute_byte(x, y, a);
				if (mt->tile_index(x, y) > 0x100) {
					Config::allow_512_tiles(true);
				}
			}
		}
	}

	fclose(file);
	return (_result = Result::META_OK);
}

bool Metatileset::write_attributes(const char *f) const {
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
	if (!_tileset.num_tiles()) { return (_result = Result::META_NO_GFX); } // no graphics

	std::ifstream ifs(f);
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
