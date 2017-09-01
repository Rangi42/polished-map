#pragma warning(push, 0)
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_ask.H>
#pragma warning(pop)

#include "tileset.h"

// Palette_Map.Palette x Tile.Hue x RGB
static uchar indoor_colors[9][4][3] = { // indoor bg.pal
	// WHITE, DARK, LIGHT, BLACK
	{{0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}, {0xAB, 0xCD, 0xEF}}, // UNDEFINED
	{{0xF0, 0xE0, 0xD0}, {0x98, 0x98, 0x98}, {0x68, 0x68, 0x68}, {0x38, 0x38, 0x38}}, // GRAY
	{{0xF0, 0xE0, 0xD0}, {0xF8, 0x98, 0xC0}, {0xF0, 0x50, 0x30}, {0x38, 0x38, 0x38}}, // RED
	{{0x90, 0xC0, 0x48}, {0x78, 0xA0, 0x08}, {0x48, 0x68, 0x00}, {0x38, 0x38, 0x38}}, // GREEN
	{{0xF0, 0xE0, 0xD0}, {0x78, 0x80, 0xF8}, {0x48, 0x48, 0xF8}, {0x38, 0x38, 0x38}}, // WATER
	{{0xF0, 0xE0, 0xD0}, {0xF8, 0xF8, 0x38}, {0xF8, 0x80, 0x08}, {0x38, 0x38, 0x38}}, // YELLOW
	{{0xD0, 0xC0, 0x88}, {0xA8, 0x88, 0x38}, {0x80, 0x68, 0x18}, {0x38, 0x38, 0x38}}, // BROWN
	{{0xF0, 0xE0, 0xD0}, {0x88, 0x98, 0xF8}, {0x70, 0x80, 0xF8}, {0x38, 0x38, 0x38}}, // ROOF
	{{0xFF, 0xFF, 0xFF}, {0xF8, 0xF8, 0x80}, {0x70, 0x48, 0x00}, {0x00, 0x00, 0x00}}, // TEXT
};

Tileset::Tileset() : _palette_map(), _tiles(), _num_tiles(0), _result(GFX_NULL) {
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		_tiles[i] = new Tile((uint8_t)i);
	}
}

Tileset::~Tileset() {
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		delete _tiles[i];
	}
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

	const char *data = png.data()[0];
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			Tile *t = _tiles[y * w + x];
			Palette_Map::Palette p = _palette_map.palette((uint8_t)(y * w + x));
			for (int ty = 0; ty < TILE_SIZE; ty++) {
				for (int tx = 0; tx < TILE_SIZE; tx++) {
					int i = (y * TILE_SIZE + ty) * w + (x * TILE_SIZE + tx);
					Tile::Hue h = (Tile::Hue)((uchar)data[i] / 0x40); // [0, 255] -> [0, 3]
					uchar *rgb = indoor_colors[p][h];
					t->pixel(ty, tx, rgb);
				}
			}
		}
	}

	return (_result = GFX_OK);
}
