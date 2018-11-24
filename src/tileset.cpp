#include "config.h"
#include "tileset.h"
#include "image.h"

Tileset::Tileset() : _name(), _lighting(), _palette_map(), _tiles(), _num_tiles(0), _num_roof_tiles(0),
	_result(GFX_NULL), _modified(false), _modified_roof(false) {
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		_tiles[i] = new Tile((uint8_t)i);
		_roof_tiles[i] = new Tile((uint8_t)i);
	}
}

Tileset::~Tileset() {
	clear();
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		delete _tiles[i];
		delete _roof_tiles[i];
	}
}

void Tileset::clear() {
	_name.clear();
	_palette_map.clear();
	_num_tiles = 0;
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		_tiles[i]->clear();
	}
	clear_roof_graphics();
	_result = GFX_NULL;
	_modified = false;
	_modified_roof = false;
}

void Tileset::clear_roof_graphics() {
	_num_roof_tiles = 0;
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		_roof_tiles[i]->clear();
	}
}

void Tileset::update_lighting(Lighting l) {
	_lighting = l;
	bool allow_256_tiles = Config::allow_256_tiles();
	for (int i = 0; i < MAX_NUM_TILES; i++) {
		int j = (!allow_256_tiles && i >= 0x60) ? (i >= 0xE0 ? i - 0x80 : i + 0x20) : i;
		_tiles[j]->update_lighting(l);
		_roof_tiles[j]->update_lighting(l);
	}
}

uchar *Tileset::print_rgb(size_t w, size_t h, size_t n) const {
	uchar *buffer = new uchar[w * h * NUM_CHANNELS]();
	FILL(buffer, 0xff, w * h * NUM_CHANNELS);
	bool allow_256_tiles = Config::allow_256_tiles();
	for (size_t i = 0; i < n; i++) {
		if (!allow_256_tiles && i == 0x60) { i += 0x1f; continue; }
		const Tile *t = _tiles[i];
		int ty = (i / TILES_PER_ROW) * TILE_SIZE, tx = (i % TILES_PER_ROW) * TILE_SIZE;
		if (!allow_256_tiles && i >= 0x80) { ty -= 2 * TILE_SIZE; }
		print_tile_rgb(t, tx, ty, TILES_PER_ROW, buffer);
	}
	return buffer;
}

uchar *Tileset::print_roof_rgb(size_t w, size_t h) const {
	uchar *buffer = new uchar[w * h * NUM_CHANNELS]();
	FILL(buffer, 0xff, w * h * NUM_CHANNELS);
	for (size_t i = 0; i < NUM_ROOF_TILES; i++) {
		const Tile *t = _roof_tiles[i + FIRST_ROOF_TILE_ID];
		int ty = (i / ROOF_TILES_PER_ROW) * TILE_SIZE, tx = (i % ROOF_TILES_PER_ROW) * TILE_SIZE;
		print_tile_rgb(t, tx, ty, ROOF_TILES_PER_ROW, buffer);
	}
	return buffer;
}

void Tileset::read_tile(Tile *t, const Tiled_Image &ti, uint8_t i, size_t j) {
	Palette p = _palette_map.palette(i);
	t->palette(p);
	for (int ty = 0; ty < TILE_SIZE; ty++) {
		for (int tx = 0; tx < TILE_SIZE; tx++) {
			Hue h = ti.tile_hue(j, tx, ty);
			const uchar *rgb = Color::color(_lighting, p, h);
			t->pixel(tx, ty, h, rgb[0], rgb[1], rgb[2]);
		}
	}
}

void Tileset::print_tile_rgb(const Tile *t, int tx, int ty, int n, uchar *buffer) const {
	for (int py = 0; py < TILE_SIZE; py++) {
		for (int px = 0; px < TILE_SIZE; px++) {
			Hue h = t->hue(px, py);
			uchar c;
			switch (h) {
			case Hue::BLACK: c = 0x00; break;
			case Hue::DARK:  c = 0x55; break;
			case Hue::LIGHT: c = 0xAA; break;
			case Hue::WHITE: default: c = 0xFF;
			}
			size_t j = ((ty + py) * n * TILE_SIZE + tx + px) * NUM_CHANNELS;
			buffer[j++] = c;
			buffer[j++] = c;
			buffer[j] = c;
		}
	}
}

Tileset::Result Tileset::read_graphics(const char *f, Lighting l) {
	if (!_palette_map.size()) { return (_result = GFX_NO_PALETTE); } // no colors

	Tiled_Image ti(f);
	switch (ti.result()) {
	case Tiled_Image::IMG_OK: break;
	case Tiled_Image::IMG_NULL: return (_result = GFX_BAD_FILE);
	case Tiled_Image::IMG_BAD_FILE: return (_result = GFX_BAD_FILE);
	case Tiled_Image::IMG_BAD_EXT: return (_result = GFX_BAD_EXT);
	case Tiled_Image::IMG_BAD_DIMS: return (_result = GFX_BAD_DIMS);
	case Tiled_Image::IMG_TOO_SHORT: return (_result = GFX_TOO_SHORT);
	case Tiled_Image::IMG_TOO_LARGE: return (_result = GFX_TOO_LARGE);
	case Tiled_Image::IMG_NOT_GRAYSCALE: return (_result = GFX_NOT_GRAYSCALE);
	case Tiled_Image::IMG_BAD_CMD: return (_result = GFX_BAD_CMD);
	default: return (_result = GFX_BAD_FILE);
	}

	_num_tiles = ti.num_tiles();

	_lighting = l;
	bool allow_256_tiles = Config::allow_256_tiles();
	for (int i = 0; i < MAX_NUM_TILES; i++) {
		int j = (!allow_256_tiles && i >= 0x60) ? (i >= 0xE0 ? i - 0x80 : i + 0x20) : i;
		Tile *t = _tiles[j];
		read_tile(t, ti, (uint8_t)i, (size_t)i);
	}

	_modified = false;
	_modified_roof = false;

	return (_result = GFX_OK);
}

Tileset::Result Tileset::read_roof_graphics(const char *f) {
	if (!_palette_map.size()) { return GFX_NO_PALETTE; } // no colors

	Tiled_Image ti(f);
	switch (ti.result()) {
	case Tiled_Image::IMG_OK: break;
	case Tiled_Image::IMG_NULL: return GFX_BAD_FILE;
	case Tiled_Image::IMG_BAD_FILE: return GFX_BAD_FILE;
	case Tiled_Image::IMG_BAD_EXT: return GFX_BAD_EXT;
	case Tiled_Image::IMG_BAD_DIMS: return GFX_BAD_DIMS;
	case Tiled_Image::IMG_TOO_SHORT: return GFX_TOO_SHORT;
	case Tiled_Image::IMG_TOO_LARGE: return GFX_TOO_LARGE;
	case Tiled_Image::IMG_NOT_GRAYSCALE: return GFX_NOT_GRAYSCALE;
	case Tiled_Image::IMG_BAD_CMD: return GFX_BAD_CMD;
	default: return GFX_BAD_FILE;
	}

	if (ti.num_tiles() < NUM_ROOF_TILES) {
		return GFX_TOO_SHORT;
	}
	if (ti.num_tiles() > NUM_ROOF_TILES || FIRST_ROOF_TILE_ID + ti.num_tiles() > MAX_NUM_TILES) {
		return GFX_TOO_LARGE;
	}
	_num_roof_tiles = ti.num_tiles();

	bool allow_256_tiles = Config::allow_256_tiles();
	for (size_t i = 0; i < _num_roof_tiles; i++) {
		int k = (int)i + FIRST_ROOF_TILE_ID;
		int j = (!allow_256_tiles && k >= 0x60) ? (k >= 0xE0 ? k - 0x80 : k + 0x20) : k;
		Tile *t = _roof_tiles[j];
		read_tile(t, ti, (uint8_t)k, i);
	}

	return GFX_OK;
}

const char *Tileset::error_message(Result result) {
	switch (result) {
	case GFX_OK:
		return "OK.";
	case GFX_NO_PALETTE:
		return "No corresponding palette file chosen.";
	case GFX_BAD_FILE:
		return "Cannot parse file format.";
	case GFX_BAD_EXT:
		return "Unknown file extension.";
	case GFX_BAD_DIMS:
		return "Image dimensions do not fit the tile grid.";
	case GFX_TOO_SHORT:
		return "Too few bytes.";
	case GFX_TOO_LARGE:
		return "Too many pixels.";
	case GFX_NOT_GRAYSCALE:
		return "Image cannot be made grayscale.";
	case GFX_BAD_CMD:
		return "Invalid LZ command.";
	case GFX_NULL:
		return "No graphics file chosen.";
	default:
		return "Unspecified error.";
	}
}

bool Tileset::write_graphics(const char *f) {
	Image::Result result = Image::write_tileset_image(f, *this);
	return !result;
}

bool Tileset::write_roof_graphics(const char *f) {
	Image::Result result = Image::write_roof_image(f, *this);
	return !result;
}
