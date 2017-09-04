#include "map.h"

Map::Map() : _width(0), _height(0), _blocks(NULL) {}

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
}
