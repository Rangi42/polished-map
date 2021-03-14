#ifndef IMAGE_H
#define IMAGE_H

#pragma warning(push, 0)
#include <FL/Fl_RGB_Image.H>
#pragma warning(pop)

#include "map.h"
#include "metatileset.h"
#include "tileset.h"

class Image {
public:
	enum class Result { IMAGE_OK, IMAGE_BAD_DATA, IMAGE_BAD_FILE, IMAGE_BAD_PNG, IMAGE_EMPTY };
	static Result write_rgb_image(const char *f, Fl_RGB_Image *image);
	static Result write_tileset_image(const char *f, const Tileset &tileset, size_t off = 0x000, size_t n = 0);
	static Result write_roof_image(const char *f, const Tileset &tileset);
	static const char *error_message(Result result);
	static bool make_deimage(Fl_Widget *wgt);
private:
	static Result write_image(const char *f, size_t w, size_t h, uchar *buffer, bool is_2bpp = false,
		int pd = 1, int d = 3, int ld = 0);
};

#endif
