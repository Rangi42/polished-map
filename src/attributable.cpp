#include "attributable.h"

Attributable::Attributable(uint8_t id) : _id(id), _palette(), _priority(), _x_flip(), _y_flip() {}

uchar Attributable::byte() const {
	uchar a = (uchar)_palette;
	if (_id >= 0x80) { a |= 0x08; }
	if (_x_flip) { a |= 0x20; }
	if (_y_flip) { a |= 0x40; }
	return a;
}

void Attributable::byte(uchar a) {
	_palette = (Palette)(a & 0x87);
	_x_flip = !!(a & 0x20);
	_y_flip = !!(a & 0x40);
}

void Attributable::clear() {
	_id = 0;
	_palette = Palette::GRAY;
	_x_flip = _y_flip = _priority = false;
}

void Attributable::copy(const Attributable &a) {
	_id = a._id;
	_palette = a._palette;
	_x_flip = a._x_flip;
	_y_flip = a._y_flip;
}
