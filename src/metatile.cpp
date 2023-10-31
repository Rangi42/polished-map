#include "utils.h"
#include "metatile.h"

Metatile::Metatile(uint8_t id) : _id(id), _tile_ids(), _collisions(), _bin_collisions() {}

bool Metatile::uses_tile(uint8_t id) const {
	for (int y = 0; y < METATILE_SIZE; y++) {
		for (int x = 0; x < METATILE_SIZE; x++) {
			if (_tile_ids[y][x] == id) {
				return true;
			}
		}
	}
	return false;
}

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

void Metatile::copy(const Metatile *src) {
	for (int y = 0; y < METATILE_SIZE; y++) {
		for (int x = 0; x < METATILE_SIZE; x++) {
			_tile_ids[y][x] = src->_tile_ids[y][x];
		}
	}
	for (int i = 0; i < NUM_QUADRANTS; i++) {
		_collisions[i] = src->_collisions[i];
		_bin_collisions[i] = src->_bin_collisions[i];
	}
}

void Metatile::swap(Metatile *mt) {
	for (int y = 0; y < METATILE_SIZE; y++) {
		for (int x = 0; x < METATILE_SIZE; x++) {
			std::swap(_tile_ids[y][x], mt->_tile_ids[y][x]);
		}
	}
	for (int i = 0; i < NUM_QUADRANTS; i++) {
		std::swap(_collisions[i], mt->_collisions[i]);
		std::swap(_bin_collisions[i], mt->_bin_collisions[i]);
	}
}
