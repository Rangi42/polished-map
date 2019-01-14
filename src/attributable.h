#ifndef ATTRIBUTABLE_H
#define ATTRIBUTABLE_H

#include "colors.h"

class Attributable {
protected:
	uint8_t _id;
	Palette _palette;
	bool _priority, _x_flip, _y_flip;
public:
	Attributable(uint8_t id = 0);
	inline uint8_t id(void) const { return _id; }
	inline void id(uint8_t id) { _id = id; }
	inline Palette palette(void) const { return _palette; }
	inline void palette(Palette p) { _palette = p; }
	inline bool priority(void) const { return (int)_palette >= 0x80; }
	inline void priority(bool p) { _palette = (Palette)((int)_palette & (p ? 0xff : 0xf)); }
	inline bool x_flip(void) const { return _x_flip; }
	inline void x_flip(bool f) { _x_flip = f; }
	inline bool y_flip(void) const { return _y_flip; }
	inline void y_flip(bool f) { _y_flip = f; }
	uchar byte(void) const;
	void byte(uchar a);
	void clear(void);
	void copy(const Attributable &a);
};

#endif
