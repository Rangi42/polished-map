#include "attributable.h"

Attributable::Attributable(uint16_t id) : _id(id), _palette(), _x_flip(), _y_flip(),
	_priority(), _undefined() {}

uchar Attributable::tile_byte() const {
	// IDs $000-0FF -> VRAM $00-7F
	// IDs $100-1FF -> VRAM $80-FF
	return (uchar)(_id & 0x7f) | (_id >= 0x100 ? 0x80 : 0);
}

uchar Attributable::attribute_byte() const {
	uchar a = (uchar)_palette & PALETTE_MASK;
	// IDs $000-07F and $180-1FF -> VRAM $0:xx
	// IDs $080-17F -> VRAM $1:xx
	if (_id >= 0x080 && _id < 0x180) { a |= BANK_1_MASK; }
	if (_x_flip) { a |= X_FLIP_MASK; }
	if (_y_flip) { a |= Y_FLIP_MASK; }
	if (_priority) { a |= PRIORITY_MASK; }
	return a;
}

void Attributable::apply_attribute_byte(uchar a) {
	// VRAM $0:00-7F -> IDs $000-07F
	// VRAM $1:00-7F -> IDs $080-0FF
	// VRAM $1:80-FF -> IDs $100-17F
	// VRAM $0:80-FF -> IDs $180-1FF
	_id = _id + ((a & BANK_1_MASK) ? 0x80 : (_id >= 0x080 ? 0x100 : 0));
	_palette = (Palette)(a & PALETTE_MASK);
	_x_flip = !!(a & X_FLIP_MASK);
	_y_flip = !!(a & Y_FLIP_MASK);
	_priority = !!(a & PRIORITY_MASK);
}

void Attributable::clear() {
	_id = 0x000;
	_palette = Palette::GRAY;
	_x_flip = _y_flip = _priority = false;
}

void Attributable::copy(const Attributable &a) {
	_id = a._id;
	_palette = a._palette;
	_x_flip = a._x_flip;
	_y_flip = a._y_flip;
	_priority = a._priority;
}
