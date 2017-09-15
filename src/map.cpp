#include <cstdio>

#include "map.h"

Map::Map() : _width(0), _height(0), _blocks(NULL), _result(MAP_NULL), _modified(false) {}

Map::~Map() {
	clear();
}

void Map::size(uint8_t w, uint8_t h) {
	clear();
	_width = w;
	_height = h;
	_blocks = new Block *[size()]();
}

void Map::block(uint8_t x, uint8_t y, Block *b) {
	_blocks[(size_t)y * _width + (size_t)x] = b;
}

void Map::clear() {
	delete [] _blocks;
	_blocks = NULL;
	_width = _height = 0;
	_result = MAP_NULL;
	_modified = false;
}

Map::Result Map::read_blocks(const char *f) {
	bool too_long = false;

	FILE *file = fl_fopen(f, "rb");
	if (file == NULL) { return (_result = MAP_BAD_FILE); } // cannot load file

	uint8_t *data = new uint8_t[size() + 1];
	size_t c = fread(data, 1, size() + 1, file);
	fclose(file);
	if (c < size()) { return (_result = MAP_TOO_SHORT); } // too-short blk
	if (c == size() + 1) { too_long = true; }

	for (uint8_t y = 0; y < (size_t)_height; y++) {
		for (uint8_t x = 0; x < (size_t)_width; x++) {
			size_t i = (size_t)y * _width + (size_t)x;
			uint8_t id = data[i];
			_blocks[i] = new Block(0, 0, 0, y, x, id);
		}
	}

	return (_result = too_long ? MAP_TOO_LONG : MAP_OK);
}

const char *Map::error_message(Result result) {
	switch (result) {
	case MAP_OK:
		return "OK.";
	case MAP_BAD_FILE:
		return "Cannot open file.";
	case MAP_TOO_SHORT:
		return "File ends too early.";
	case MAP_TOO_LONG:
		return "The .blk file is larger than the specified size.";
	case MAP_NULL:
		return "No *.blk file chosen.";
	default:
		return "Unspecified error.";
	}
}
