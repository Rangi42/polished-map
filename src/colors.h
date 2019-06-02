#ifndef COLORS_H
#define COLORS_H

#include <vector>
#include <array>

#pragma warning(push, 0)
#include <FL/Enumerations.H>
#pragma warning(pop)

#include "utils.h"

#define RGB5C(x) (uchar)((x) * 8) // (uchar)((x) * 33 / 4) for BGB instead of VBA
#define CRGB5(c) (uchar)((c) / 8)

enum Lighting { MORN, DAY, NITE, INDOOR, CUSTOM };

enum Palette { GRAY, RED, GREEN, WATER, YELLOW, BROWN, ROOF, TEXT, MONOCHROME, UNDEFINED,
	PRIORITY_GRAY = 0x80, PRIORITY_RED, PRIORITY_GREEN, PRIORITY_WATER,
	PRIORITY_YELLOW, PRIORITY_BROWN, PRIORITY_ROOF, PRIORITY_TEXT };

enum Hue { WHITE, DARK, LIGHT, BLACK };

#define NUM_LIGHTINGS 5
#define NUM_GAME_PALETTES 8
#define NUM_HUES 4
#define NUM_CHANNELS 3

typedef std::array<uchar, NUM_CHANNELS> ColorArray;
typedef std::array<ColorArray, NUM_HUES> HueArray;
typedef std::vector<HueArray> PalVec;

class Color {
private:
	static void color(Lighting l, Palette p, Hue h, ColorArray v);
	static void color(Lighting l, Palette p, HueArray v);
public:
	static Hue ordered_hue(int i);
	static const uchar *color(Lighting l, Palette p, Hue h);
	static void color(Lighting l, Palette p, Hue h, Fl_Color f);
	static Fl_Color fl_color(Lighting l, Palette p, Hue h);
	static PalVec parse_lighting(const char *f);
	static Lighting read_lighting(const char *f, Lighting lighting);
	static bool read_roof_colors(const char *f, uint8_t map_group);
	static bool write_lighting(const char *f, Lighting lighting);
};

#endif
