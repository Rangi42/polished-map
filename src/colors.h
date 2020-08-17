#ifndef COLORS_H
#define COLORS_H

#include <vector>
#include <array>

#pragma warning(push, 0)
#include <FL/Enumerations.H>
#pragma warning(pop)

#include "utils.h"

#define RGB5C(x) (uchar)((x) * 33 / 4) // 5-bit to 8-bit
#define CRGB5(c) (uchar)((c) / 8)      // 8-bit to 5-bit

enum class Palettes { MORN, DAY, NITE, INDOOR, CUSTOM };

enum class Palette { GRAY, RED, GREEN, WATER, YELLOW, BROWN, ROOF, TEXT };

enum class Hue { WHITE, DARK, LIGHT, BLACK };

enum class Roof_Palettes { ROOF_CUSTOM, ROOF_DAY_NITE, ROOF_MORN_DAY_NITE,
	ROOF_DAY_NITE_CUSTOM, ROOF_MORN_DAY_NITE_CUSTOM };

#define NUM_PALETTE_SETS 5
#define NUM_PALETTES 8
#define NUM_HUES 4
#define NUM_CHANNELS 3
#define NUM_ROOF_PALETTES 5

typedef std::array<uchar, NUM_CHANNELS> ColorArray;
typedef std::array<ColorArray, NUM_HUES> HueArray;
typedef std::vector<HueArray> PalVec;

class Color {
private:
	static void color(Palettes l, Palette p, Hue h, ColorArray v);
	static void color(Palettes l, Palette p, HueArray v);
public:
	static uchar desaturated(uchar r, uchar g, uchar b);
	static Hue ordered_hue(int i);
	static uchar hue_mono(Hue h);
	static Hue mono_hue(uchar c);
	static const uchar *monochrome_color(Hue h);
	static const uchar *undefined_color(Hue h);
	static const uchar *color(Palettes l, Palette p, Hue h);
	static void color(Palettes l, Palette p, Hue h, Fl_Color f);
	static Fl_Color fl_color(Palettes l, Palette p, Hue h);
	static PalVec parse_palettes(const char *f);
	static Palettes read_palettes(const char *f, Palettes pals);
	static bool read_roof_colors(const char *f, uint8_t map_group, Roof_Palettes roof_palettes);
	static bool write_palettes(const char *f);
};

#endif
