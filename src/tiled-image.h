#ifndef TILED_IMAGE_H
#define TILED_IMAGE_H

#include <vector>

#pragma warning(push, 0)
#include <FL/fl_ask.H>
#pragma warning(pop)

#include "deep-tile.h"

#define BYTES_PER_2BPP_TILE (TILE_AREA / 4)

class Tiled_Image {
public:
	enum class Result { IMG_OK, IMG_BAD_FILE, IMG_BAD_EXT, IMG_BAD_DIMS, IMG_TOO_SHORT,
		IMG_TOO_LARGE, IMG_NOT_GRAYSCALE, IMG_BAD_CMD, IMG_NULL };
private:
	std::vector<Hue> _tile_hues;
	size_t _num_tiles;
	Result _result;
public:
	Tiled_Image(const char *f);
	~Tiled_Image();
	inline Hue tile_hue(size_t i, size_t x, size_t y) const {
		return i < _num_tiles ? _tile_hues[(i * TILE_SIZE + y) * TILE_SIZE + x] : Hue::WHITE;
	}
	inline size_t num_tiles(void) const { return _num_tiles; }
	inline Result result(void) const { return _result; }
private:
	Result read_png_graphics(const char *f);
	Result read_2bpp_graphics(const char *f);
	Result read_lz_graphics(const char *f);
	Result parse_2bpp_data(const std::vector<uchar> &data);
};

#endif
