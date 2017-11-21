#ifndef COLORS_H
#define COLORS_H

#pragma warning(push, 0)
#include <FL/Enumerations.H>
#pragma warning(pop)

#define NUM_CHANNELS 3

enum Lighting { DAY, NITE, INDOOR, ICE_PATH, ARTIFICIAL };

enum Palette { GRAY, RED, GREEN, WATER, YELLOW, BROWN, ROOF, TEXT, MONOCHROME, UNDEFINED,
	PRIORITY_GRAY = 0x80, PRIORITY_RED, PRIORITY_GREEN, PRIORITY_WATER,
	PRIORITY_YELLOW, PRIORITY_BROWN, PRIORITY_ROOF, PRIORITY_TEXT };

enum Hue { WHITE, DARK, LIGHT, BLACK };

class Color {
public:
	static const uchar *color(Lighting l, Palette p, Hue h);
	static Fl_Color fl_color(Lighting l, Palette p, Hue h);
};

#endif
