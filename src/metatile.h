#ifndef METATILE_H
#define METATILE_H

#include <string>

#pragma warning(push, 0)
#include <FL/fl_types.h>
#pragma warning(pop)

#include "tile.h"

enum class Quadrant { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };

#define NUM_QUADRANTS 4

#define METATILE_SIZE 4
#define METATILE_PX_SIZE (TILE_SIZE * METATILE_SIZE)

class Metatile {
private:
	uint8_t _id;
	uint8_t _tile_ids[METATILE_SIZE][METATILE_SIZE];
	std::string _collisions[NUM_QUADRANTS];
	uint8_t _bin_collisions[NUM_QUADRANTS];
public:
	Metatile(uint8_t id);
	inline uint8_t id(void) const { return _id; }
	inline void id(uint8_t id) { _id = id; }
	inline uint8_t tile_id(int x, int y) const { return _tile_ids[y][x]; }
	inline void tile_id(int x, int y, uint8_t id) { _tile_ids[y][x] = id; }
	inline std::string collision(Quadrant q) const { return _collisions[(int)q]; }
	inline void collision(Quadrant q, const std::string &c) { _collisions[(int)q] = c; }
	inline uint8_t bin_collision(Quadrant q) const { return _bin_collisions[(int)q]; }
	inline const uint8_t *bin_collisions(void) const { return _bin_collisions; }
	inline void bin_collision(Quadrant q, uint8_t c) { _bin_collisions[(int)q] = c; }
	bool uses_tile(uint8_t id) const;
	void clear(void);
	void copy(const Metatile *src);
	void swap(Metatile *mt);
};

#endif
