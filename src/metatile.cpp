#include "utils.h"
#include "metatile.h"

Metatile::Metatile(uint8_t id) : _id(id), _tile_ids() {}

void Metatile::clear() {
	for (int y = 0; y < METATILE_SIZE; y++) {
		for (int x = 0; x < METATILE_SIZE; x++) {
			_tile_ids[y][x] = 0;
		}
	}
}
