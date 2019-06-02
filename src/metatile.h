#ifndef METATILE_H
#define METATILE_H

#include <string>

#pragma warning(push, 0)
#include <FL/fl_types.h>
#pragma warning(pop)

#include "tile.h"
#include "attributable.h"

enum Quadrant { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };

#define NUM_QUADRANTS 4

#define METATILE_SIZE 4

class Metatile {
private:
	uint8_t _id;
	Attributable _attributes[METATILE_SIZE][METATILE_SIZE];
	std::string _collisions[NUM_QUADRANTS];
	uint8_t _bin_collisions[NUM_QUADRANTS];
public:
	Metatile(uint8_t id);
	inline uint8_t id(void) const { return _id; }
	inline void id(uint8_t id) { _id = id; }
	inline uint8_t tile_id(int x, int y) const { return _attributes[y][x].id(); }
	inline void tile_id(int x, int y, uint8_t tid) { _attributes[y][x].id(tid); }
	inline bool extra(int x, int y) const { return _attributes[y][x].extra(); }
	inline void extra(int x, int y, bool e) { _attributes[y][x].extra(e); }
	inline const Attributable *attributes(int x, int y) const { return &_attributes[y][x]; }
	inline void attributes(int x, int y, const Attributable *a) { _attributes[y][x].copy(*a); }
	inline uchar attribute_byte(int x, int y) const { return _attributes[y][x].byte(); }
	inline void attribute_byte(int x, int y, uchar a) { _attributes[y][x].byte(a); }
	inline std::string collision(Quadrant q) const { return _collisions[q]; }
	inline void collision(Quadrant q, std::string c) { _collisions[q] = c; }
	inline uint8_t bin_collision(Quadrant q) const { return _bin_collisions[q]; }
	inline const uint8_t *bin_collisions(void) const { return _bin_collisions; }
	inline void bin_collision(Quadrant q, uint8_t c) { _bin_collisions[q] = c; }
	void clear(void);
	void copy(const Metatile *src);
	void swap(Metatile *mt);
};

#endif
