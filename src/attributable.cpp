#include "attributable.h"

Attributable::Attributable(int idx) : _offset(), _palette(), _bank1(), _x_flip(), _y_flip(), _priority() {
	index(idx);
}

// index $000-07F <-> VRAM $0:00-7F
// index $080-0FF <-> VRAM $1:00-7F
// index $100-17F <-> VRAM $1:80-FF
// index $180-1FF <-> VRAM $0:80-FF

void Attributable::bank_offset(int idx, bool &bank1, uint8_t &offset) {
	offset = (uint8_t)(idx & 0x07f) | (idx >= 0x100 ? 0x80 : 0);
	bank1 = idx >= 0x080 && idx < 0x180;
}

int Attributable::index() const {
	return (int)_offset + (_bank1 ? 0x80 : _offset >= 0x080 ? 0x100 : 0);
}

void Attributable::index(int idx) {
	bank_offset(idx, _bank1, _offset);
}

uchar Attributable::attribute_byte() const {
	uchar a = (uchar)_palette & PALETTE_MASK;
	if (_bank1) { a |= BANK_1_MASK; }
	if (_x_flip) { a |= X_FLIP_MASK; }
	if (_y_flip) { a |= Y_FLIP_MASK; }
	if (_priority) { a |= PRIORITY_MASK; }
	return a;
}

void Attributable::attribute_byte(uchar a) {
	_palette = (Palette)(a & PALETTE_MASK);
	_bank1 = !!(a & BANK_1_MASK);
	_x_flip = !!(a & X_FLIP_MASK);
	_y_flip = !!(a & Y_FLIP_MASK);
	_priority = !!(a & PRIORITY_MASK);
}

void Attributable::clear() {
	_offset = 0x00;
	_palette = Palette::GRAY;
	_bank1 = _x_flip = _y_flip = _priority = false;
}

void Attributable::copy(const Attributable &a) {
	_offset = a._offset;
	_palette = a._palette;
	_bank1 = a._bank1;
	_x_flip = a._x_flip;
	_y_flip = a._y_flip;
	_priority = a._priority;
}
