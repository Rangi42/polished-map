#include "config.h"
#include "tiled-image.h"
#include "tileset.h"

Tileset::Tileset() : _name(), _palette_map(), _tiles(), _num_tiles(0), _result(GFX_NULL) {
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		_tiles[i] = new Tile((uint8_t)i);
	}
}

Tileset::~Tileset() {
	clear();
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		delete _tiles[i];
	}
}

void Tileset::clear() {
	_name.clear();
	_palette_map.clear();
	_num_tiles = 0;
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		_tiles[i]->clear();
	}
	_result = GFX_NULL;
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
	bool skip = Config::skip_tiles_60_to_7f();
	for (int i = 0; i < MAX_NUM_TILES; i++) {
		int j = skip ? (i >= 0x60 ? (i >= 0xE0 ? i - 0x80 : i + 0x20) : i) : i;
		Tile *t = _tiles[j];
		Palette p = _palette_map.palette((uint8_t)i);
		t->palette(p);
		for (int ty = 0; ty < TILE_SIZE; ty++) {
			for (int tx = 0; tx < TILE_SIZE; tx++) {
				Hue h = ti.tile_hue(i, tx, ty);
				const uchar *rgb = Color::color(l, p, h);
				t->pixel(tx, ty, h, rgb);
			}
		}
	}

	return (_result = GFX_OK);
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
