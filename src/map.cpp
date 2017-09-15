#include <cstdio>

#include "map.h"

Map::Map() : _width(0), _height(0), _blocks(NULL), _result(MAP_NULL) {}

Map::~Map() {
	clear();
}

void Map::size(uint8_t w, uint8_t h) {
	clear();
	_width = w;
	_height = h;
	_blocks = new Block *[size()]();
	_result = MAP_NULL;
}

void Map::block(uint8_t x, uint8_t y, Block *b) {
	_blocks[(size_t)y * _width + (size_t)x] = b;
}

void Map::clear() {
	delete [] _blocks;
	_blocks = NULL;
	_width = _height = 0;
	_result = MAP_NULL;
}

Map::Result Map::read_blocks(const char *f) {
	FILE *file = fl_fopen(f, "rb");
	if (file == NULL) { return (_result = MAP_BAD_FILE); } // cannot load file

	uint8_t *data = new uint8_t[size()];
	size_t c = fread(data, 1, size(), file);
	if (c < size()) { return (_result = MAP_TOO_SHORT); } // too-short blk

	for (uint8_t y = 0; y < (size_t)_height; y++) {
		for (uint8_t x = 0; x < (size_t)_width; x++) {
			size_t i = (size_t)y * _width + (size_t)x;
			uint8_t id = data[i];
			_blocks[i] = new Block(0, 0, 0, y, x, id);
		}
	}

	// TODO: detect if the file is too big and warn about it

	return (_result = MAP_OK);
}

const char *Map::error_message(Result result) {
	switch (result) {
	case MAP_OK:
		return "OK.";
	case MAP_BAD_FILE:
		return "Cannot open file.";
	case MAP_TOO_SHORT:
		return "File ends too early.";
	case MAP_NULL:
		return "No *.blk file chosen.";
	default:
		return "Unspecified error.";
	}
}
