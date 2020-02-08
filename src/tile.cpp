#include "config.h"
#include "tile.h"
#include "map-buttons.h"

Tile::Tile(int idx) : _offset(), _palette(), _bank1(), _x_flip(), _y_flip(), _priority() {
	index(idx);
}

// index $000-07F <-> VRAM $0:00-7F
// index $080-0FF <-> VRAM $1:00-7F
// index $100-17F <-> arrange_0_before_1 ? VRAM $0:80-FF : VRAM $1:80-FF
// index $180-1FF <-> arrange_0_before_1 ? VRAM $1:80-FF : VRAM $0:80-FF

void Tile::bank_offset(int idx, bool &bank1, uint8_t &offset) {
	offset = (uint8_t)(idx & 0x07f) | (idx >= 0x100 ? 0x80 : 0);
	bank1 = Config::arrange_0_before_1() ? (idx & 0x0ff) >= 0x80 : idx >= 0x080 && idx < 0x180;
}

int Tile::index() const {
	if (Config::arrange_0_before_1()) {
		return (int)_offset + (_bank1 ? 0x80 : 0) + (_offset >= 0x80 ? 0x80 : 0);
	}
	return (int)_offset + (_bank1 ? 0x80 : _offset >= 0x080 ? 0x100 : 0);
}

void Tile::index(int idx) {
	bank_offset(idx, _bank1, _offset);
}

uchar Tile::attribute() const {
	uchar a = (uchar)_palette & PALETTE_MASK;
	if (_bank1) { a |= BANK_1_MASK; }
	if (_x_flip) { a |= X_FLIP_MASK; }
	if (_y_flip) { a |= Y_FLIP_MASK; }
	if (_priority) { a |= PRIORITY_MASK; }
	return a;
}

void Tile::attribute(uchar a) {
	_palette = (Palette)(a & PALETTE_MASK);
	_bank1 = !!(a & BANK_1_MASK);
	_x_flip = !!(a & X_FLIP_MASK);
	_y_flip = !!(a & Y_FLIP_MASK);
	_priority = !!(a & PRIORITY_MASK);
}

void Tile::clear() {
	_offset = 0x00;
	_palette = Palette::GRAY;
	_bank1 = _x_flip = _y_flip = _priority = false;
}

void Tile::copy(const Tile &t) {
	_offset = t._offset;
	_palette = t._palette;
	_bank1 = t._bank1;
	_x_flip = t._x_flip;
	_y_flip = t._y_flip;
	_priority = t._priority;
}

void Tile::copy(const Chip &c) {
	bank_offset(c.index(), _bank1, _offset);
	_palette = c.palette();
	_x_flip = c.x_flip();
	_y_flip = c.y_flip();
	_priority = c.priority();
}
