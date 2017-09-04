#ifndef MAP_H
#define MAP_H

#include "utils.h"
#include "map-buttons.h"

class Map {
private:
	uint8_t _width, _height;
	Block **_blocks;
public:
	Map();
	~Map();
	inline uint8_t width(void) const { return _width; }
	inline uint8_t height(void) const { return _height; }
	void size(uint8_t w, uint8_t h);
	inline size_t size(void) const { return (size_t)_width * (size_t)_height; }
	inline Block *block(uint8_t x, uint8_t y) const { return _blocks[(size_t)y * _width + (size_t)x]; }
	inline Block *block(size_t i) const { return _blocks[i]; }
	void block(uint8_t x, uint8_t y, Block *b);
	void clear();
};

#endif
