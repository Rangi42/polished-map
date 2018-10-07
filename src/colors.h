#ifndef COLORS_H
#define COLORS_H

#pragma warning(push, 0)
#include <FL/Enumerations.H>
#pragma warning(pop)

#define NUM_CHANNELS 3

enum Lighting { MORN, DAY, NITE, INDOOR, CUSTOM };

enum Palette { GRAY, RED, GREEN, WATER, YELLOW, BROWN, ROOF, TEXT, MONOCHROME, UNDEFINED,
	PRIORITY_GRAY = 0x80, PRIORITY_RED, PRIORITY_GREEN, PRIORITY_WATER,
	PRIORITY_YELLOW, PRIORITY_BROWN, PRIORITY_ROOF, PRIORITY_TEXT };

enum Hue { WHITE, DARK, LIGHT, BLACK };

class Color {
public:
	static const uchar *color(Lighting l, Palette p, Hue h);
	static void color(Lighting l, Palette p, Hue h, uchar v[3]);
	static void color(Lighting l, Palette p, uchar v[4][3]);
	static Fl_Color fl_color(Lighting l, Palette p, Hue h);
	static void read_custom_lighting(const char *f);
};

#endif
