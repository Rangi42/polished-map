#include "utils.h"
#include "metatile.h"

Metatile::Metatile(uint8_t id) : _id(id), _tile_ids(), _palettes(), _x_flips(), _y_flips(),
	_collisions(), _bin_collisions() {}

void Metatile::clear() {
	for (int y = 0; y < METATILE_SIZE; y++) {
		for (int x = 0; x < METATILE_SIZE; x++) {
			_tile_ids[y][x] = 0;
			_palettes[y][x] = Palette::GRAY; // TODO: use a different default palette?
			_x_flips[y][x] = false;
			_y_flips[y][x] = false;
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
			_palettes[y][x] = src->_palettes[y][x];
			_x_flips[y][x] = src->_x_flips[y][x];
			_y_flips[y][x] = src->_y_flips[y][x];
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
			std::swap(_palettes[y][x], mt->_palettes[y][x]);
			std::swap(_x_flips[y][x], mt->_x_flips[y][x]);
			std::swap(_y_flips[y][x], mt->_y_flips[y][x]);
		}
	}
	for (int i = 0; i < NUM_QUADRANTS; i++) {
		std::swap(_collisions[i], mt->_collisions[i]);
		std::swap(_bin_collisions[i], mt->_bin_collisions[i]);
	}
}
