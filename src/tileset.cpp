#include "tiled-image.h"
#include "tileset.h"

// Tileset.Lighting x Palette_Map.Palette x Tile.Hue x RGB
static const uchar tileset_colors[3][9][4][3] = {
	{ // DAY
		// WHITE, DARK, LIGHT, BLACK
		{{8*27, 8*31, 8*27}, {8*13, 8*13, 8*13}, {8*21, 8*21, 8*21}, {8* 7, 8* 7, 8* 7}}, // GRAY
		{{8*27, 8*31, 8*27}, {8*30, 8*10, 8* 6}, {8*31, 8*19, 8*24}, {8* 7, 8* 7, 8* 7}}, // RED
		{{8*22, 8*31, 8*10}, {8* 5, 8*14, 8* 0}, {8*12, 8*25, 8* 1}, {8* 7, 8* 7, 8* 7}}, // GREEN
		{{8*27, 8*31, 8*27}, {8*13, 8*12, 8*31}, {8*18, 8*19, 8*31}, {8* 7, 8* 7, 8* 7}}, // WATER
		{{8*27, 8*31, 8*27}, {8*31, 8*16, 8* 1}, {8*31, 8*31, 8* 7}, {8* 7, 8* 7, 8* 7}}, // YELLOW
		{{8*27, 8*31, 8*27}, {8*20, 8*15, 8* 3}, {8*24, 8*18, 8* 7}, {8* 7, 8* 7, 8* 7}}, // BROWN
		{{8*27, 8*31, 8*27}, {8* 5, 8*17, 8*31}, {8*15, 8*31, 8*31}, {8* 7, 8* 7, 8* 7}}, // ROOF
		{{8*31, 8*31, 8*16}, {8*14, 8* 9, 8* 0}, {8*31, 8*31, 8*16}, {8* 0, 8* 0, 8* 0}}, // TEXT
		{{0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}}, // UNDEFINED
	},
	{ // NITE
		// WHITE, DARK, LIGHT, BLACK
		{{8*15, 8*14, 8*24}, {8* 7, 8* 7, 8*12}, {8*11, 8*11, 8*19}, {8* 0, 8* 0, 8* 0}}, // GRAY
		{{8*15, 8*14, 8*24}, {8*13, 8* 0, 8* 8}, {8*14, 8* 7, 8*17}, {8* 0, 8* 0, 8* 0}}, // RED
		{{8*15, 8*14, 8*24}, {8* 0, 8*11, 8*13}, {8* 8, 8*13, 8*19}, {8* 0, 8* 0, 8* 0}}, // GREEN
		{{8*15, 8*14, 8*24}, {8* 4, 8* 3, 8*18}, {8*10, 8* 9, 8*20}, {8* 0, 8* 0, 8* 0}}, // WATER
		{{8*30, 8*30, 8*11}, {8*16, 8*14, 8*10}, {8*16, 8*14, 8*18}, {8* 0, 8* 0, 8* 0}}, // YELLOW
		{{8*15, 8*14, 8*24}, {8* 8, 8* 4, 8* 5}, {8*12, 8* 9, 8*15}, {8* 0, 8* 0, 8* 0}}, // BROWN
		{{8*15, 8*14, 8*24}, {8*11, 8* 9, 8*20}, {8*13, 8*12, 8*23}, {8* 0, 8* 0, 8* 0}}, // ROOF
		{{8*31, 8*31, 8*16}, {8*14, 8* 9, 8* 0}, {8*31, 8*31, 8*16}, {8* 0, 8* 0, 8* 0}}, // TEXT
		{{0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}}, // UNDEFINED
	},
	{ // INDOOR
		// WHITE, DARK, LIGHT, BLACK
		{{8*30, 8*28, 8*26}, {8*13, 8*13, 8*13}, {8*19, 8*19, 8*19}, {8* 7, 8* 7, 8* 7}}, // GRAY
		{{8*30, 8*28, 8*26}, {8*30, 8*10, 8* 6}, {8*31, 8*19, 8*24}, {8* 7, 8* 7, 8* 7}}, // RED
		{{8*18, 8*24, 8* 9}, {8* 9, 8*13, 8* 0}, {8*15, 8*20, 8* 1}, {8* 7, 8* 7, 8* 7}}, // GREEN
		{{8*30, 8*28, 8*26}, {8* 9, 8* 9, 8*31}, {8*15, 8*16, 8*31}, {8* 7, 8* 7, 8* 7}}, // WATER
		{{8*30, 8*28, 8*26}, {8*31, 8*16, 8* 1}, {8*31, 8*31, 8* 7}, {8* 7, 8* 7, 8* 7}}, // YELLOW
		{{8*26, 8*24, 8*17}, {8*16, 8*13, 8* 3}, {8*21, 8*17, 8* 7}, {8* 7, 8* 7, 8* 7}}, // BROWN
		{{8*30, 8*28, 8*26}, {8*14, 8*16, 8*31}, {8*17, 8*19, 8*31}, {8* 7, 8* 7, 8* 7}}, // ROOF
		{{8*31, 8*31, 8*16}, {8*14, 8* 9, 8* 0}, {8*31, 8*31, 8*16}, {8* 0, 8* 0, 8* 0}}, // TEXT
		{{0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}}, // UNDEFINED
	},
};

Tileset::Tileset() : _palette_map(), _tiles(), _num_tiles(0), _result(GFX_NULL) {
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
	_palette_map.clear();
	_num_tiles = 0;
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		_tiles[i]->clear();
	}
	_result = GFX_NULL;
}

Palette_Map::Result Tileset::read_palette_map(const char *f) {
	return _palette_map.read_from(f);
}

Tileset::Result Tileset::read_graphics(const char *f, Lighting l, bool skip_60_7f) {
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
	default: return (_result = GFX_BAD_FILE);
	}

	_num_tiles = ti.num_tiles();
	for (int i = 0; i < _num_tiles; i++) {
		int j = skip_60_7f && i >= 0x60 ? i + 0x20 : i;
		Tile *t = _tiles[j];
		Palette_Map::Palette p = _palette_map.palette((uint8_t)i);
		for (int ty = 0; ty < TILE_SIZE; ty++) {
			for (int tx = 0; tx < TILE_SIZE; tx++) {
				Tile::Hue h = ti.tile_hue(i, tx, ty);
				const uchar *rgb = tileset_colors[l][p][h];
				t->pixel(ty, tx, rgb);
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
		return "No corresponding *_palette_map.asm file chosen.";
	case GFX_BAD_FILE:
		return "Cannot parse file format.";
	case GFX_BAD_EXT:
		return "Unkown file extension.";
	case GFX_BAD_DIMS:
		return "Image dimensions do not fit the tile grid.";
	case GFX_TOO_SHORT:
		return "Too few bytes.";
	case GFX_TOO_LARGE:
		return "Too many pixels.";
	case GFX_NOT_GRAYSCALE:
		return "Image cannot be made grayscale.";
	case GFX_NULL:
		return "No graphics file chosen.";
	default:
		return "Unspecified error.";
	}
}
