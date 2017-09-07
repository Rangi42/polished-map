#pragma warning(push, 0)
#include <FL/filename.H>
#include <FL/Fl_PNG_Image.H>
#pragma warning(pop)

#include "palette-map.h"
#include "tiled-image.h"

Tiled_Image::Tiled_Image(const char *f) : _tile_hues(NULL), _num_tiles(0), _result(IMG_NULL) {
	if (fl_filename_match(f, "*.[Pp][Nn][Gg]")) { read_png_graphics(f); }
	else if (fl_filename_match(f, "*.2[Bb][Pp][Pp]")) { read_2bpp_graphics(f); }
	else if (fl_filename_match(f, "*.2[Bb][Pp][Pp].[Ll][Zz]")) { read_lz_graphics(f); }
}

Tiled_Image::~Tiled_Image() {
	delete [] _tile_hues;
}

Tiled_Image::Result Tiled_Image::read_png_graphics(const char *f) {
	Fl_PNG_Image png(f);
	if (png.fail()) { return (_result = IMG_BAD_FILE); } // cannot load PNG

	int w = png.w(), h = png.h();
	if (w % TILE_SIZE || h % TILE_SIZE) { return (_result = IMG_BAD_DIMS); } // dimensions do not fit the tile grid

	w /= TILE_SIZE;
	h /= TILE_SIZE;
	_num_tiles = w * h;
	if (_num_tiles > MAX_NUM_TILES) { return (_result = IMG_TOO_LARGE); } // too many tiles

	png.desaturate();
	if (png.d() != 1 || png.count() != 1) { return (_result = IMG_NOT_GRAYSCALE); } // not grayscale

	delete [] _tile_hues;
	_tile_hues = new Tile::Hue[_num_tiles * TILE_SIZE * TILE_SIZE]();

	Tile::Hue png_hues[4] = {Tile::BLACK, Tile::DARK, Tile::LIGHT, Tile::WHITE};
	size_t hi = 0;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			for (int ty = 0; ty < TILE_SIZE; ty++) {
				for (int tx = 0; tx < TILE_SIZE; tx++) {
					long ti = (y * TILE_SIZE + ty) * (w * TILE_SIZE) + (x * TILE_SIZE + tx);
					Tile::Hue h = png_hues[png.array[ti] / (0x100 / 4)]; // [0, 255] -> [0, 3]
					_tile_hues[hi++] = h;
				}
			}
		}
	}

	return (_result = IMG_OK);
}

Tiled_Image::Result Tiled_Image::read_2bpp_graphics(const char *) {
	return (_result = IMG_NULL);
}

Tiled_Image::Result Tiled_Image::read_lz_graphics(const char *) {
	return (_result = IMG_NULL);
}
