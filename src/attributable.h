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
	uint8_t _id;
	Palette _palette;
	bool _extra, _x_flip, _y_flip, _priority, _undefined;
public:
	Attributable(uint8_t id = 0);
	inline uint8_t id(void) const { return _id; }
	inline void id(uint8_t id) { _id = id; }
	inline Palette palette(void) const { return _palette; }
	inline void palette(Palette p) { _palette = p; }
	inline bool extra(void) const { return _extra; }
	inline void extra(bool e) { _extra = e; }
	inline bool x_flip(void) const { return _x_flip; }
	inline void x_flip(bool f) { _x_flip = f; }
	inline bool y_flip(void) const { return _y_flip; }
	inline void y_flip(bool f) { _y_flip = f; }
	inline bool priority(void) const { return _priority; }
	inline void priority(bool p) { _priority = p; }
	uchar byte(void) const;
	void byte(uchar a);
	void clear(void);
	void copy(const Attributable &a);
};

#endif
