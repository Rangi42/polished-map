#include <cstdio>
#include <string>
#include <sstream>
#include <png.h>
#include <zlib.h>

#pragma warning(push, 0)
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "utils.h"
#include "deep-tile.h"
#include "image.h"
#include "config.h"

Image::Result Image::write_rgb_image(const char *f, Fl_RGB_Image *image) {
	if (!image) { return Result::IMAGE_BAD_DATA; }
	size_t w = (size_t)image->w(), h = (size_t)image->h();
	uchar *buffer = (uchar *)image->data()[0];
	int d = image->d(), ld = image->ld();
	int pd = d > 1;
	return write_image(f, w, h, buffer, false, pd, d, ld);
}

Image::Result Image::write_tileset_image(const char *f, const Tileset &tileset, size_t off, size_t n) {
	if (!n) {
		for (n = MAX_NUM_TILES; tileset.const_tile((int)(n+off-1))->undefined(); n--);
	}
	size_t w = std::min(n, (size_t)TILES_PER_ROW) * TILE_SIZE;
	size_t h = ((n + TILES_PER_ROW - 1) / TILES_PER_ROW) * TILE_SIZE;
	uchar *buffer = tileset.print_rgb(w, h, off, n);
	return write_image(f, w, h, buffer, true);
}

Image::Result Image::write_roof_image(const char *f, const Tileset &tileset) {
	size_t w = ROOF_TILES_PER_ROW * TILE_SIZE;
	size_t h = ROOF_TILES_PER_COL * TILE_SIZE;
	uchar *buffer = tileset.print_roof_rgb(w, h);
	return write_image(f, w, h, buffer, true);
}

Image::Result Image::write_image(const char *f, size_t w, size_t h, uchar *buffer, bool is_2bpp, int pd, int d, int ld) {
	if (!ld) { ld = (int)w * d; }
	Result result = Result::IMAGE_OK;
	FILE *file = fl_fopen(f, "wb");
	if (!file) { result = Result::IMAGE_BAD_FILE; goto cleanup1; }
	{ // new scope avoids gcc "jump to label crosses initialization" error
		// Create the necessary PNG structures
		png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png) { result = Result::IMAGE_BAD_PNG; goto cleanup2; }
		png_infop info = png_create_info_struct(png);
		if (!info) { result = Result::IMAGE_BAD_PNG; goto cleanup2; }
		png_init_io(png, file);
		// Set compression options
		png_set_compression_level(png, Z_BEST_COMPRESSION);
		png_set_compression_mem_level(png, Z_BEST_COMPRESSION);
		png_set_compression_strategy(png, Z_DEFAULT_STRATEGY);
		png_set_compression_window_bits(png, 15);
		png_set_compression_method(png, Z_DEFLATED);
		png_set_compression_buffer_size(png, 8192);
		// Write the PNG IHDR chunk
		png_set_IHDR(png, info, (png_uint_32)w, (png_uint_32)h, is_2bpp ? 2 : 8,
			is_2bpp ? PNG_COLOR_TYPE_GRAY : PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		// Write the other PNG header chunks
		png_write_info(png, info);
		// Write the RGB pixels in row-major order from top to bottom
		size_t rs = is_2bpp ? w / 4 : w * NUM_CHANNELS;
		png_bytep png_row = new png_byte[rs];
		if (is_2bpp) {
			for (size_t i = 0; i < h; i++) {
				for (size_t j = 0; j < rs; j++) {
					uchar pp = 0;
					for (size_t k = 0; k < 4; k++) {
						size_t px = ld * i + d * (j * 4 + k);
						uchar v = buffer[px] / (0x100 / 4); // [0, 255] -> [0, 3]
						pp = (pp << 2) | v;
					}
					png_row[j] = pp;
				}
				png_write_row(png, png_row);
			}
		}
		else {
			for (size_t i = 0; i < h; i++) {
				for (size_t j = 0; j < w; j++) {
					size_t rd = NUM_CHANNELS * j;
					size_t px = ld * i + d * j;
					for (size_t k = 0; k < NUM_CHANNELS; k++) {
						png_row[rd+k] = buffer[px+pd*k];
					}
				}
				png_write_row(png, png_row);
			}
		}
		png_write_end(png, NULL);
		delete [] png_row;
		png_destroy_write_struct(&png, &info);
		png_free_data(png, info, PNG_FREE_ALL, -1);
	}
cleanup2:
	fclose(file);
cleanup1:
	delete [] buffer;
	return result;
}

const char *Image::error_message(Result result) {
	switch (result) {
	case Result::IMAGE_OK:
		return "OK.";
	case Result::IMAGE_BAD_DATA:
		return "Cannot read image data.";
	case Result::IMAGE_BAD_FILE:
		return "Cannot open file.";
	case Result::IMAGE_BAD_PNG:
		return "Cannot write PNG data.";
	default:
		return "Unspecified error.";
	}
}

bool Image::make_deimage(Fl_Widget *wgt) {
	if (!wgt || !wgt->image()) {
		return false;
	}
	Fl_Image *deimg = wgt->image()->copy();
	if (!deimg) {
		return false;
	}
	deimg->desaturate();
	deimg->color_average(FL_GRAY, 0.5f);
	if (wgt->deimage()) {
		delete wgt->deimage();
	}
	wgt->deimage(deimg);
	return true;
}
