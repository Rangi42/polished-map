#include "config.h"
#include "tiled-image.h"
#include "tileset.h"

#define RGB5C(x) (uchar)((x) * 8) // (uchar)((x) * 33 / 4) for BGB instead of VBA
#define RGB5(r, g, b) {RGB5C(r), RGB5C(g), RGB5C(b)}
#define RGBX(rgb) {(((rgb) & 0xFF0000) >> 16), (((rgb) & 0xFF00) >> 8), ((rgb) & 0xFF)}

// Tileset.Lighting x Palette_Map.Palette x Tile.Hue x RGB
static const uchar tileset_colors[5][10][4][NUM_CHANNELS] = {
	{ // DAY
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(27,31,27), RGB5(13,13,13), RGB5(21,21,21), RGB5( 7, 7, 7)}, // GRAY
		{RGB5(27,31,27), RGB5(30,10, 6), RGB5(31,19,24), RGB5( 7, 7, 7)}, // RED
		{RGB5(22,31,10), RGB5( 5,14, 0), RGB5(12,25, 1), RGB5( 7, 7, 7)}, // GREEN
		{RGB5(27,31,27), RGB5(13,12,31), RGB5(18,19,31), RGB5( 7, 7, 7)}, // WATER
		{RGB5(27,31,27), RGB5(31,16, 1), RGB5(31,31, 7), RGB5( 7, 7, 7)}, // YELLOW
		{RGB5(27,31,27), RGB5(20,15, 3), RGB5(24,18, 7), RGB5( 7, 7, 7)}, // BROWN
		{RGB5(27,31,27), RGB5( 5,17,31), RGB5(15,31,31), RGB5( 7, 7, 7)}, // ROOF
		{RGB5(31, 0,31), RGB5(16, 0,16), RGB5(31, 0,31), RGB5( 0, 0, 0)}, // TEXT
		{RGB5(28,31,26), RGB5( 6,13,10), RGB5(17,24,14), RGB5( 1, 3, 4)}, // MONOCHROME
		{RGBX(0xABCDEF), RGBX(0x56789A), RGBX(0xABCDEF), RGBX(0x123456)}, // UNDEFINED
	},
	{ // NITE
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(15,14,24), RGB5( 7, 7,12), RGB5(11,11,19), RGB5( 0, 0, 0)}, // GRAY
		{RGB5(15,14,24), RGB5(13, 0, 8), RGB5(14, 7,17), RGB5( 0, 0, 0)}, // RED
		{RGB5(15,14,24), RGB5( 0,11,13), RGB5( 8,13,19), RGB5( 0, 0, 0)}, // GREEN
		{RGB5(15,14,24), RGB5( 4, 3,18), RGB5(10, 9,20), RGB5( 0, 0, 0)}, // WATER
		{RGB5(30,30,11), RGB5(16,14,10), RGB5(16,14,18), RGB5( 0, 0, 0)}, // YELLOW
		{RGB5(15,14,24), RGB5( 8, 4, 5), RGB5(12, 9,15), RGB5( 0, 0, 0)}, // BROWN
		{RGB5(15,14,24), RGB5(11, 9,20), RGB5(13,12,23), RGB5( 0, 0, 0)}, // ROOF
		{RGB5(31, 0,31), RGB5(16, 0,16), RGB5(31, 0,31), RGB5( 0, 0, 0)}, // TEXT
		{RGB5(17,24,14), RGB5( 6,13,10), RGB5( 6,13,10), RGB5( 1, 3, 4)}, // MONOCHROME
		{RGBX(0xABCDEF), RGBX(0x56789A), RGBX(0xABCDEF), RGBX(0x123456)}, // UNDEFINED
	},
	{ // INDOOR
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(30,28,26), RGB5(13,13,13), RGB5(19,19,19), RGB5( 7, 7, 7)}, // GRAY
		{RGB5(30,28,26), RGB5(30,10, 6), RGB5(31,19,24), RGB5( 7, 7, 7)}, // RED
		{RGB5(18,24, 9), RGB5( 9,13, 0), RGB5(15,20, 1), RGB5( 7, 7, 7)}, // GREEN
		{RGB5(30,28,26), RGB5( 9, 9,31), RGB5(15,16,31), RGB5( 7, 7, 7)}, // WATER
		{RGB5(30,28,26), RGB5(31,16, 1), RGB5(31,31, 7), RGB5( 7, 7, 7)}, // YELLOW
		{RGB5(26,24,17), RGB5(16,13, 3), RGB5(21,17, 7), RGB5( 7, 7, 7)}, // BROWN
		{RGB5(30,28,26), RGB5(14,16,31), RGB5(17,19,31), RGB5( 7, 7, 7)}, // ROOF
		{RGB5(31, 0,31), RGB5(16, 0,16), RGB5(31, 0,31), RGB5( 0, 0, 0)}, // TEXT
		{RGB5(31,29,31), RGB5(20,26,31), RGB5(21,28,11), RGB5( 3, 2, 2)}, // MONOCHROME
		{RGBX(0xABCDEF), RGBX(0x56789A), RGBX(0xABCDEF), RGBX(0x123456)}, // UNDEFINED
	},
	{ // ICE_PATH
		{RGB5(15,14,24), RGB5( 7, 7,12), RGB5(11,11,19), RGB5( 0, 0, 0)}, // GRAY
		{RGB5(15,14,24), RGB5(13, 0, 8), RGB5(14, 7,17), RGB5( 0, 0, 0)}, // RED
		{RGB5(22,29,31), RGB5(31,31,31), RGB5(10,27,31), RGB5( 5, 0, 9)}, // GREEN
		{RGB5(15,14,24), RGB5( 3, 3,10), RGB5( 5, 5,17), RGB5( 0, 0, 0)}, // WATER
		{RGB5(30,30,11), RGB5(16,14,10), RGB5(16,14,18), RGB5( 0, 0, 0)}, // YELLOW
		{RGB5(15,14,24), RGB5( 8, 4, 5), RGB5(12, 9,15), RGB5( 0, 0, 0)}, // BROWN
		{RGB5(25,31,31), RGB5(16,11,31), RGB5( 9,28,31), RGB5( 5, 0, 9)}, // ROOF
		{RGB5(31, 0,31), RGB5(16, 0,16), RGB5(31, 0,31), RGB5( 0, 0, 0)}, // TEXT
		{RGB5(21,14, 9), RGB5( 3, 2, 2), RGB5(18,24,22), RGB5( 3, 2, 2)}, // MONOCHROME
		{RGBX(0xABCDEF), RGBX(0x56789A), RGBX(0xABCDEF), RGBX(0x123456)}, // UNDEFINED
	},
	{ // ARTIFICIAL
		{RGBX(0xE0E0E0), RGBX(0x606060), RGBX(0xB0B0B0), RGBX(0x303030)}, // GRAY
		{RGBX(0xFFC0C0), RGBX(0x802020), RGBX(0xFF4040), RGBX(0x301010)}, // RED
		{RGBX(0xC0FFC0), RGBX(0x008000), RGBX(0x00FF00), RGBX(0x003000)}, // GREEN
		{RGBX(0xC0D8FF), RGBX(0x0040C0), RGBX(0x0080FF), RGBX(0x002040)}, // WATER
		{RGBX(0xFFFFC0), RGBX(0x808000), RGBX(0xFFFF00), RGBX(0x303000)}, // YELLOW
		{RGBX(0xF8D8C8), RGBX(0x704010), RGBX(0xD08020), RGBX(0x281808)}, // BROWN
		{RGBX(0xC0FFFF), RGBX(0x008080), RGBX(0x00FFFF), RGBX(0x003030)}, // ROOF
		{RGBX(0xFFC0FF), RGBX(0x800080), RGBX(0xFF00FF), RGBX(0x300030)}, // TEXT
		{RGBX(0xFFFFFF), RGBX(0x555555), RGBX(0xAAAAAA), RGBX(0x000000)}, // MONOCHROME
		{RGBX(0xABCDEF), RGBX(0x56789A), RGBX(0xABCDEF), RGBX(0x123456)}, // UNDEFINED
	},
};

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
		Palette_Map::Palette p = _palette_map.palette((uint8_t)i);
		for (int ty = 0; ty < TILE_SIZE; ty++) {
			for (int tx = 0; tx < TILE_SIZE; tx++) {
				Tile::Hue h = ti.tile_hue(i, tx, ty);
				const uchar *rgb = tileset_colors[l][p][h];
				t->pixel(tx, ty, rgb);
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
