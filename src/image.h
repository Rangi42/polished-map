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
	enum Result { IMAGE_OK, IMAGE_BAD_DATA, IMAGE_BAD_FILE, IMAGE_BAD_PNG };
	static Result write_rgb_image(const char *f, Fl_RGB_Image *image);
	static Result write_tileset_image(const char *f, const Tileset &tileset);
	static Result write_roof_image(const char *f, const Tileset &tileset);
	static const char *error_message(Result result);
private:
	static Result write_image(const char *f, size_t w, size_t h, uchar *buffer, bool grayscale = false,
		int pd = 1, int d = 3, int ld = 0);
};

#endif
