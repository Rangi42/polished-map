#ifndef METATILE_H
#define METATILE_H

#include <string>

#pragma warning(push, 0)
#include <FL/fl_types.h>
#pragma warning(pop)

#include "tile.h"

enum Quadrant { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };

#define NUM_QUADRANTS 4

#define METATILE_SIZE 4

class Metatile {
private:
	uint8_t _id;
	uint8_t _tile_ids[METATILE_SIZE][METATILE_SIZE];
	Palette _palettes[METATILE_SIZE][METATILE_SIZE];
	bool _x_flips[METATILE_SIZE][METATILE_SIZE];
	bool _y_flips[METATILE_SIZE][METATILE_SIZE];
	std::string _collisions[NUM_QUADRANTS];
	uint8_t _bin_collisions[NUM_QUADRANTS];
public:
	Metatile(uint8_t id);
	inline uint8_t id(void) const { return _id; }
	inline void id(uint8_t id) { _id = id; }
	inline uint8_t tile_id(int x, int y) const { return _tile_ids[y][x]; }
	inline void tile_id(int x, int y, uint8_t id) { _tile_ids[y][x] = id; }
	inline Palette palette(int x, int y) const { return _palettes[y][x]; }
	inline void palette(int x, int y, Palette p) { _palettes[y][x] = p; }
	inline bool x_flip(int x, int y) const { return _x_flips[y][x]; }
	inline void x_flip(int x, int y, bool f) { _x_flips[y][x] = f; }
	inline bool y_flip(int x, int y) const { return _y_flips[y][x]; }
	inline void y_flip(int x, int y, bool f) { _y_flips[y][x] = f; }
	std::string collision(Quadrant q) const { return _collisions[q]; }
	void collision(Quadrant q, std::string c) { _collisions[q] = c; }
	uint8_t bin_collision(Quadrant q) const { return _bin_collisions[q]; }
	const uint8_t *bin_collisions(void) const { return _bin_collisions; }
	void bin_collision(Quadrant q, uint8_t c) { _bin_collisions[q] = c; }
	void clear(void);
	void copy(const Metatile *src);
	void swap(Metatile *mt);
};

#endif
