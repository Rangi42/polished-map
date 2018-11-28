#include "utils.h"
#include "metatile.h"

Metatile::Metatile(uint8_t id) : _id(id), _tile_ids(), _collisions(), _bin_collisions() {}

void Metatile::clear() {
	for (int y = 0; y < METATILE_SIZE; y++) {
		for (int x = 0; x < METATILE_SIZE; x++) {
			_tile_ids[y][x] = 0;
		}
	}
	for (int i = 0; i < NUM_QUADRANTS; i++) {
		_collisions[i].clear();
		_bin_collisions[i] = 0;
	}
}
