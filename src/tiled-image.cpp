#pragma warning(push, 0)
#include <FL/Fl_PNG_Image.H>
#pragma warning(pop)

#include "utils.h"
#include "palette-map.h"
#include "tiled-image.h"

Tiled_Image::Tiled_Image(const char *f) : _tile_hues(NULL), _num_tiles(0), _result(IMG_NULL) {
	if (ends_with(f, ".png")) { read_png_graphics(f); }
	else if (ends_with(f, ".2bpp")) { read_2bpp_graphics(f); }
	else if (ends_with(f, ".2bpp.lz")) { read_lz_graphics(f); }
}

Tiled_Image::~Tiled_Image() {
	delete [] _tile_hues;
}

Tiled_Image::Result Tiled_Image::read_png_graphics(const char *f) {
	Fl_PNG_Image png(f);
	if (png.fail()) { return (_result = IMG_BAD_FILE); }

	int w = png.w(), h = png.h();
	if (w % TILE_SIZE || h % TILE_SIZE) { return (_result = IMG_BAD_DIMS); }

	w /= TILE_SIZE;
	h /= TILE_SIZE;
	_num_tiles = w * h;
	if (_num_tiles > MAX_NUM_TILES) { return (_result = IMG_TOO_LARGE); }

	png.desaturate();
	if (png.d() != 1 || png.count() != 1) { return (_result = IMG_NOT_GRAYSCALE); }

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

static void convert_2bytes_to_8hues(uchar b1, uchar b2, Tile::Hue *hues8) {
	// %ABCD_EFGH %abcd_efgh -> %Aa %Bb %Cc %Dd %Ee %Ff %GG %Hh
	hues8[0] = (Tile::Hue)((b1 >> 6 & 2) | (b2 >> 7 & 1));
	hues8[1] = (Tile::Hue)((b1 >> 5 & 2) | (b2 >> 6 & 1));
	hues8[2] = (Tile::Hue)((b1 >> 4 & 2) | (b2 >> 5 & 1));
	hues8[3] = (Tile::Hue)((b1 >> 3 & 2) | (b2 >> 4 & 1));
	hues8[4] = (Tile::Hue)((b1 >> 2 & 2) | (b2 >> 3 & 1));
	hues8[5] = (Tile::Hue)((b1 >> 1 & 2) | (b2 >> 2 & 1));
	hues8[6] = (Tile::Hue)((b1      & 2) | (b2 >> 1 & 1));
	hues8[7] = (Tile::Hue)((b1 << 1 & 2) | (b2      & 1));
}

Tiled_Image::Result Tiled_Image::read_2bpp_graphics(const char *f) {
	FILE *file = fopen(f, "rb");
	if (!file) { return (_result = IMG_BAD_FILE); }

	fseek(file, 0, SEEK_END);
	long n = ftell(file);
	rewind(file);
	if (n % BYTES_PER_2BPP_TILE) { fclose(file); return (_result = IMG_BAD_DIMS); }

	uchar *data = new uchar[n];
	size_t r = fread(data, 1, n, file);
	if (r != n) { fclose(file); delete [] data; return (_result = IMG_TOO_SHORT); }

	n /= BYTES_PER_2BPP_TILE;
	if (n > MAX_NUM_TILES) { fclose(file); delete [] data; return (_result = IMG_TOO_LARGE); }

	_num_tiles = n;
	delete [] _tile_hues;
	_tile_hues = new Tile::Hue[_num_tiles * TILE_SIZE * TILE_SIZE]();

	for (int i = 0; i < _num_tiles; i++) {
		for (int j = 0; j < TILE_SIZE; j++) {
			uchar b1 = data[i * BYTES_PER_2BPP_TILE + j * 2];
			uchar b2 = data[i * BYTES_PER_2BPP_TILE + j * 2 + 1];
			convert_2bytes_to_8hues(b1, b2, _tile_hues + (i * TILE_SIZE + j) * 8);
		}
	}

	fclose(file);
	delete [] data;
	return (_result = IMG_OK);
}

Tiled_Image::Result Tiled_Image::read_lz_graphics(const char *) {
	return (_result = IMG_NULL);
}
