#pragma warning(push, 0)
#include <FL/Fl_PNG_Image.H>
#pragma warning(pop)

#include "tileset.h"

// Palette_Map.Palette x Tile.Hue x RGB
static uchar indoor_colors[9][4][3] = { // indoor bg.pal
	// WHITE, DARK, LIGHT, BLACK
	{{0xF0, 0xE0, 0xD0}, {0x68, 0x68, 0x68}, {0x98, 0x98, 0x98}, {0x38, 0x38, 0x38}}, // GRAY
	{{0xF0, 0xE0, 0xD0}, {0xF0, 0x50, 0x30}, {0xF8, 0x98, 0xC0}, {0x38, 0x38, 0x38}}, // RED
	{{0x90, 0xC0, 0x48}, {0x48, 0x68, 0x00}, {0x78, 0xA0, 0x08}, {0x38, 0x38, 0x38}}, // GREEN
	{{0xF0, 0xE0, 0xD0}, {0x48, 0x48, 0xF8}, {0x78, 0x80, 0xF8}, {0x38, 0x38, 0x38}}, // WATER
	{{0xF0, 0xE0, 0xD0}, {0xF8, 0x80, 0x08}, {0xF8, 0xF8, 0x38}, {0x38, 0x38, 0x38}}, // YELLOW
	{{0xD0, 0xC0, 0x88}, {0x80, 0x68, 0x18}, {0xA8, 0x88, 0x38}, {0x38, 0x38, 0x38}}, // BROWN
	{{0xF0, 0xE0, 0xD0}, {0x70, 0x80, 0xF8}, {0x88, 0x98, 0xF8}, {0x38, 0x38, 0x38}}, // ROOF
	{{0xFF, 0xFF, 0xFF}, {0x70, 0x48, 0x00}, {0xF8, 0xF8, 0x80}, {0x00, 0x00, 0x00}}, // TEXT
	{{0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}}, // UNDEFINED
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

Tileset::Result Tileset::read_2bpp_graphics(const char *) {
	return (_result = GFX_BAD_FILE);
}

Tileset::Result Tileset::read_png_graphics(const char *f) {
	if (!_palette_map.size()) { return (_result = GFX_NO_PALETTE); } // no colors

	Fl_PNG_Image png(f);
	if (png.fail()) { return (_result = GFX_BAD_FILE); } // cannot load PNG

	int w = png.w(), h = png.h();
	if (w % TILE_SIZE || h % TILE_SIZE) { return (_result = GFX_BAD_DIMS); } // dimensions do not fit the tile grid

	w /= TILE_SIZE;
	h /= TILE_SIZE;
	_num_tiles = w * h;
	if (_num_tiles > MAX_NUM_TILES) { return (_result = GFX_TOO_LARGE); } // too many tiles

	png.desaturate();
	if (png.d() != 1 || png.count() != 1) { return (_result = GFX_NOT_GRAYSCALE); } // not grayscale

	Tile::Hue png_hues[4] = {Tile::BLACK, Tile::DARK, Tile::LIGHT, Tile::WHITE};

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int i = y * w + x;
			Tile *t = _tiles[i];
			Palette_Map::Palette p = _palette_map.palette((uint8_t)i);
			for (int ty = 0; ty < TILE_SIZE; ty++) {
				for (int tx = 0; tx < TILE_SIZE; tx++) {
					long ti = (y * TILE_SIZE + ty) * (w * TILE_SIZE) + (x * TILE_SIZE + tx);
					// BUG: this array access gets the wrong value
					Tile::Hue h = png_hues[png.array[ti] / (0x100 / 4)]; // [0, 255] -> [0, 3]
					uchar *rgb = indoor_colors[p][h];
					t->pixel(ty, tx, rgb);
				}
			}
		}
	}

	return (_result = GFX_OK);
}
