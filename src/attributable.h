#ifndef ATTRIBUTABLE_H
#define ATTRIBUTABLE_H

#include "colors.h"

#define PALETTE_MASK   0x07
#define BANK_1_MASK    0x08
#define X_FLIP_MASK    0x20
#define Y_FLIP_MASK    0x40
#define PRIORITY_MASK  0x80

class Attributable {
protected:
	uint8_t _offset;
	Palette _palette;
	bool _bank1, _x_flip, _y_flip, _priority;
public:
	static void bank_offset(int idx, bool &bank1, uint8_t &offset);
	Attributable(int idx = 0x000);
	inline uint8_t offset(void) const { return _offset; }
	inline void offset(uint8_t off) { _offset = off; }
	inline Palette palette(void) const { return _palette; }
	inline void palette(Palette p) { _palette = p; }
	inline bool bank1(void) const { return _bank1; }
	inline void bank1(bool b) { _bank1 = b; }
	inline bool x_flip(void) const { return _x_flip; }
	inline void x_flip(bool f) { _x_flip = f; }
	inline bool y_flip(void) const { return _y_flip; }
	inline void y_flip(bool f) { _y_flip = f; }
	inline bool priority(void) const { return _priority; }
	inline void priority(bool p) { _priority = p; }
	int index(void) const;
	void index(int idx);
	uchar attribute_byte(void) const;
	void attribute_byte(uchar a);
	void clear(void);
	void copy(const Attributable &a);
};

#endif
