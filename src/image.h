#ifndef IMAGE_H
#define IMAGE_H

#include "map.h"
#include "metatileset.h"
#include "tileset.h"

class Image {
public:
	enum Result { IMAGE_OK, IMAGE_BAD_FILE, IMAGE_BAD_PNG };
	static Result write_map_image(const char *f, const Map &map, const Metatileset &mt);
	static Result write_tileset_image(const char *f, const Tileset &tileset);
	static Result write_roof_image(const char *f, const Tileset &tileset);
	static const char *error_message(Result result);
private:
	static Result write_image(const char *f, size_t w, size_t h, uchar *buffer);
};

#endif
