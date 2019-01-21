#include "attributable.h"

Attributable::Attributable(uint8_t id) : _id(id), _palette(), _extra(), _x_flip(), _y_flip(), _priority() {}

uchar Attributable::byte() const {
	uchar a = (uchar)_palette & PALETTE_MASK;
	if (_id >= 0x80) { a |= BANK_1_MASK; }
	if (_x_flip) { a |= X_FLIP_MASK; }
	if (_y_flip) { a |= Y_FLIP_MASK; }
	if (_priority) { a |= PRIORITY_MASK; }
	return a;
}

void Attributable::byte(uchar a) {
	_palette = (Palette)(a & PALETTE_MASK);
	_x_flip = !!(a & X_FLIP_MASK);
	_y_flip = !!(a & Y_FLIP_MASK);
	_priority = !!(a & PRIORITY_MASK);
}

void Attributable::clear() {
	_id = 0x00;
	_palette = Palette::GRAY;
	_extra = _x_flip = _y_flip = _priority = false;
}

void Attributable::copy(const Attributable &a) {
	_id = a._id;
	_extra = a._extra;
	_palette = a._palette;
	_x_flip = a._x_flip;
	_y_flip = a._y_flip;
	_priority = a._priority;
}
