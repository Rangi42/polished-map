#ifndef TILE_H
#define TILE_H

#include <FL/fl_types.h>

#define TILE_SIZE 8

class Tile {
private:
	uchar _rgb[TILE_SIZE * TILE_SIZE * 3];
public:
	Tile();
};

#endif
