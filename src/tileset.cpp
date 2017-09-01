#pragma warning(push, 0)
#include <FL/Fl_Image.H>
#include <FL/Fl_PNG_Image.H>
#pragma warning(pop)

#include "tileset.h"

Tileset::Tileset() : _palette_map() {}

bool Tileset::read_palette_map(const char *f) {
	return _palette_map.read_from(f);
}

bool Tileset::read_2bpp_graphics(const char *f) {
	return false;
}

bool Tileset::read_png_graphics(const char *f) {
	Fl_PNG_Image png(f);
	//if (png.fail()) { return false; }
	return false;
}
