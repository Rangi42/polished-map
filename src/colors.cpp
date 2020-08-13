#include <cstdio>
#include <fstream>
#include <sstream>
#include <cassert>

#pragma warning(push, 0)
#include <FL/Fl.H>
#pragma warning(pop)

#include "colors.h"
#include "utils.h"
#include "config.h"

#define RGB5(r, g, b) {RGB5C(r), RGB5C(g), RGB5C(b)}
#define RGBX(rgb) {(((rgb) & 0xFF0000) >> 16), (((rgb) & 0xFF00) >> 8), ((rgb) & 0xFF)}

// Internal GB hue order
static const Hue hue_order[NUM_HUES] = {Hue::WHITE, Hue::LIGHT, Hue::DARK, Hue::BLACK};

// Canonical hue color channels
static const uchar hue_monos[NUM_HUES] = {0xFF, 0x55, 0xAA, 0x00}; // WHITE, DARK, LIGHT, BLACK

// Monochrome-color-to-hue conversion
static const Hue mono_hues[NUM_HUES] = {Hue::BLACK, Hue::DARK, Hue::LIGHT, Hue::WHITE}; // 00-3F, 40-7F, 80-BF, C0-FF

// Paletteset x Palette x Hue x RGB
uchar tileset_colors[NUM_PALETTE_SETS][NUM_GAME_PALETTES+1][NUM_HUES][NUM_CHANNELS] = {
	{ // MORN
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(28,31,16), RGB5(13,13,13), RGB5(21,21,21), RGB5( 7, 7, 7)}, // GRAY, PRIORITY_GRAY
		{RGB5(28,31,16), RGB5(30,10, 6), RGB5(31,19,24), RGB5( 7, 7, 7)}, // RED, PRIORITY_RED
		{RGB5(22,31,10), RGB5( 5,14, 0), RGB5(12,25, 1), RGB5( 7, 7, 7)}, // GREEN, PRIORITY_GREEN
		{RGB5(28,31,16), RGB5(13,12,31), RGB5(18,19,31), RGB5( 7, 7, 7)}, // WATER, PRIORITY_WATER
		{RGB5(28,31,16), RGB5(31,16, 1), RGB5(31,31, 7), RGB5( 7, 7, 7)}, // YELLOW, PRIORITY_YELLOW
		{RGB5(28,31,16), RGB5(20,15, 3), RGB5(24,18, 7), RGB5( 7, 7, 7)}, // BROWN, PRIORITY_BROWN
		{RGB5(28,31,16), RGB5( 5,17,31), RGB5(15,31,31), RGB5( 7, 7, 7)}, // ROOF, PRIORITY_ROOF
		{RGB5(31, 0,31), RGB5(16, 0,16), RGB5(31, 0,31), RGB5( 0, 0, 0)}, // TEXT, PRIORITY_TEXT
		{RGB5(31,29,31), RGB5(20,26,31), RGB5(21,28,11), RGB5( 3, 2, 2)}, // MONOCHROME (SGB)
	},
	{ // DAY
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(27,31,27), RGB5(13,13,13), RGB5(21,21,21), RGB5( 7, 7, 7)}, // GRAY, PRIORITY_GRAY
		{RGB5(27,31,27), RGB5(30,10, 6), RGB5(31,19,24), RGB5( 7, 7, 7)}, // RED, PRIORITY_RED
		{RGB5(22,31,10), RGB5( 5,14, 0), RGB5(12,25, 1), RGB5( 7, 7, 7)}, // GREEN, PRIORITY_GREEN
		{RGB5(27,31,27), RGB5(13,12,31), RGB5(18,19,31), RGB5( 7, 7, 7)}, // WATER, PRIORITY_WATER
		{RGB5(27,31,27), RGB5(31,16, 1), RGB5(31,31, 7), RGB5( 7, 7, 7)}, // YELLOW, PRIORITY_YELLOW
		{RGB5(27,31,27), RGB5(20,15, 3), RGB5(24,18, 7), RGB5( 7, 7, 7)}, // BROWN, PRIORITY_BROWN
		{RGB5(27,31,27), RGB5( 5,17,31), RGB5(15,31,31), RGB5( 7, 7, 7)}, // ROOF, PRIORITY_ROOF
		{RGB5(31, 0,31), RGB5(16, 0,16), RGB5(31, 0,31), RGB5( 0, 0, 0)}, // TEXT, PRIORITY_TEXT
		{RGB5(28,31,26), RGB5( 6,13,10), RGB5(17,24,14), RGB5( 1, 3, 4)}, // MONOCHROME (DMG)
	},
	{ // NITE
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(15,14,24), RGB5( 7, 7,12), RGB5(11,11,19), RGB5( 0, 0, 0)}, // GRAY, PRIORITY_GRAY
		{RGB5(15,14,24), RGB5(13, 0, 8), RGB5(14, 7,17), RGB5( 0, 0, 0)}, // RED, PRIORITY_RED
		{RGB5(15,14,24), RGB5( 0,11,13), RGB5( 8,13,19), RGB5( 0, 0, 0)}, // GREEN, PRIORITY_GREEN
		{RGB5(15,14,24), RGB5( 4, 3,18), RGB5(10, 9,20), RGB5( 0, 0, 0)}, // WATER, PRIORITY_WATER
		{RGB5(30,30,11), RGB5(16,14,10), RGB5(16,14,18), RGB5( 0, 0, 0)}, // YELLOW, PRIORITY_YELLOW
		{RGB5(15,14,24), RGB5( 8, 4, 5), RGB5(12, 9,15), RGB5( 0, 0, 0)}, // BROWN, PRIORITY_BROWN
		{RGB5(15,14,24), RGB5(11, 9,20), RGB5(13,12,23), RGB5( 0, 0, 0)}, // ROOF, PRIORITY_ROOF
		{RGB5(31, 0,31), RGB5(16, 0,16), RGB5(31, 0,31), RGB5( 0, 0, 0)}, // TEXT, PRIORITY_TEXT
		{RGB5(31,29,31), RGB5(18,24,22), RGB5(21,14, 9), RGB5( 3, 2, 2)}, // MONOCHROME (cave)
	},
	{ // INDOOR
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(30,28,26), RGB5(13,13,13), RGB5(19,19,19), RGB5( 7, 7, 7)}, // GRAY, PRIORITY_GRAY
		{RGB5(30,28,26), RGB5(30,10, 6), RGB5(31,19,24), RGB5( 7, 7, 7)}, // RED, PRIORITY_RED
		{RGB5(18,24, 9), RGB5( 9,13, 0), RGB5(15,20, 1), RGB5( 7, 7, 7)}, // GREEN, PRIORITY_GREEN
		{RGB5(30,28,26), RGB5( 9, 9,31), RGB5(15,16,31), RGB5( 7, 7, 7)}, // WATER, PRIORITY_WATER
		{RGB5(30,28,26), RGB5(31,16, 1), RGB5(31,31, 7), RGB5( 7, 7, 7)}, // YELLOW, PRIORITY_YELLOW
		{RGB5(26,24,17), RGB5(16,13, 3), RGB5(21,17, 7), RGB5( 7, 7, 7)}, // BROWN, PRIORITY_BROWN
		{RGB5(30,28,26), RGB5(14,16,31), RGB5(17,19,31), RGB5( 7, 7, 7)}, // ROOF, PRIORITY_ROOF
		{RGB5(31, 0,31), RGB5(16, 0,16), RGB5(31, 0,31), RGB5( 0, 0, 0)}, // TEXT, PRIORITY_TEXT
		{RGB5(31,31,31), RGB5(18, 7, 7), RGB5(31,16,16), RGB5( 3, 2, 2)}, // MONOCHROME (GBC)
	},
	{ // CUSTOM
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(28,31,28), RGB5(13,13,13), RGB5(21,21,21), RGB5( 7, 7, 7)}, // GRAY, PRIORITY_GRAY
		{RGB5(28,31,28), RGB5(24, 6, 8), RGB5(29,16,13), RGB5( 7, 7, 7)}, // RED, PRIORITY_RED
		{RGB5(14,25,20), RGB5( 5,14,10), RGB5( 9,19, 5), RGB5( 7, 7, 7)}, // GREEN, PRIORITY_GREEN
		{RGB5(28,31,28), RGB5( 1,12,19), RGB5( 8,20,27), RGB5( 7, 7, 7)}, // WATER, PRIORITY_WATER
		{RGB5(28,31,28), RGB5(12,20,27), RGB5(19,26,31), RGB5( 7, 7, 7)}, // YELLOW, PRIORITY_YELLOW
		{RGB5(28,31,28), RGB5(17,13,10), RGB5(22,18,15), RGB5( 7, 7, 7)}, // BROWN, PRIORITY_BROWN
		{RGB5(28,31,28), RGB5(17,13,23), RGB5(22,18,27), RGB5( 7, 7, 7)}, // ROOF, PRIORITY_ROOF
		{RGB5(28,31,28), RGB5(26, 8, 5), RGB5(31,31,31), RGB5( 0, 0, 0)}, // TEXT, PRIORITY_TEXT
		{RGB5(31,31,31), RGB5(10,10,10), RGB5(20,20,20), RGB5( 0, 0, 0)}, // MONOCHROME (2BPP)
	},
};

static const uchar undefined_colors[NUM_HUES][NUM_CHANNELS] = {
	// WHITE, DARK, LIGHT, BLACK
	RGBX(0xABCDEF), RGBX(0x456789), RGBX(0x789ABC), RGBX(0x123456) // UNDEFINED
};

uchar Color::desaturated(uchar r, uchar g, uchar b) {
	// Same formula as Fl_Image::desaturate()
	return (uchar)((r * 31 + g * 61 + b * 8) / 100);
}

Hue Color::ordered_hue(int i) {
	return hue_order[i];
}

uchar Color::hue_mono(Hue h) {
	return hue_monos[(int)h];
}

Hue Color::mono_hue(uchar c) {
	return mono_hues[c / (0x100 / NUM_HUES)]; // [0, 255] -> [0, 3]
}

uchar *Color::colors(Palettes l, Palette p, Hue h) {
	int c = (int)p & 0xf;
	assert(c < NUM_GAME_PALETTES + 1);
	return tileset_colors[(int)l][c][(int)h];
}

const uchar *Color::color(Palettes l, Palette p, Hue h) {
	return p == Palette::UNDEFINED ? undefined_colors[(int)h] : colors(l, p, h);
}

void Color::color(Palettes l, Palette p, Hue h, ColorArray v) {
	uchar *cs = colors(l, p, h);
	for (int i = 0; i < NUM_CHANNELS; i++) {
		cs[i] = RGB5C(v[i]);
	}
}

void Color::color(Palettes l, Palette p, Hue h, Fl_Color f) {
	uchar* cs = colors(l, p, h);
	Fl::get_color(f, cs[0], cs[1], cs[2]);
}

void Color::color(Palettes l, Palette p, HueArray v) {
	for (int i = 0; i < NUM_HUES; i++) {
		color(l, p, (Hue)i, v[i]);
	}
}

Fl_Color Color::fl_color(Palettes l, Palette p, Hue h) {
	const uchar *rgb = color(l, p, h);
	return fl_rgb_color(rgb[0], rgb[1], rgb[2]);
}

PalVec Color::parse_palettes(const char *f) {
	PalVec colors;
	int palette = 0, hue = 0, channel = 0;
	std::ifstream ifs(f);
	while (ifs.good()) {
		std::string line;
		std::getline(ifs, line);
		trim(line);
		if (!starts_with(line, "RGB")) { continue; }
		line.erase(0, strlen("RGB") + 1); // include next whitespace character
		line += ";"; // ensure trailing separator
		std::istringstream lss(line);
		while (lss.good()) {
			unsigned int v;
			char sep;
			lss >> v >> sep;
			if (hue == 0 && channel == 0) {
				colors.emplace_back();
			}
			colors[palette][(int)ordered_hue(hue)][channel] = (uchar)v;
			if (++channel == NUM_CHANNELS) {
				channel = 0;
				if (++hue == NUM_HUES) {
					hue = 0;
					palette++;
				}
			}
			if (sep == ';') { break; }
		}
	}
	return colors;
}

Palettes Color::read_palettes(const char *f, Palettes pals) {
	PalVec custom_colors = parse_palettes(f);
	size_t n = custom_colors.size();
	switch (n) {
	// MONOCHROME cases
	case 1: // CUSTOM
		color(Palettes::CUSTOM, Palette::MONOCHROME, custom_colors[0]);
		pals = Palettes::CUSTOM;
		break;
	case 2: // DAY, NITE
		color(Palettes::DAY,  Palette::MONOCHROME, custom_colors[0]);
		color(Palettes::NITE, Palette::MONOCHROME, custom_colors[1]);
		if (pals != Palettes::NITE) {
			pals = Palettes::DAY;
		}
		break;
	case 3: // MORN, DAY, NITE
		for (int l = 0; l < 3; l++) {
			color((Palettes)l, Palette::MONOCHROME, custom_colors[l]);
		}
		if (pals != Palettes::MORN && pals != Palettes::NITE) {
			pals = Palettes::DAY;
		}
		break;
	case 4: // MORN, DAY, NITE, INDOOR
		for (int l = 0; l < 4; l++) {
			color((Palettes)l, Palette::MONOCHROME, custom_colors[l]);
		}
		if (pals == Palettes::CUSTOM) {
			pals = Palettes::DAY;
		}
		break;
	case 5: // MORN, DAY, NITE, INDOOR, CUSTOM
		for (int l = 0; l < 5; l++) {
			color((Palettes)l, Palette::MONOCHROME, custom_colors[l]);
		}
		break;
	// colored (GRAY-TEXT) cases
	case 1 * NUM_GAME_PALETTES: // CUSTOM
		for (int p = 0; p < NUM_GAME_PALETTES; p++) {
			color(Palettes::CUSTOM, (Palette)p, custom_colors[p]);
		}
		pals = Palettes::CUSTOM;
		break;
	case 2 * NUM_GAME_PALETTES: // DAY, NITE
		for (int p = 0; p < NUM_GAME_PALETTES; p++) {
			color(Palettes::DAY, (Palette)p, custom_colors[p]);
		}
		for (int p = 0; p < NUM_GAME_PALETTES; p++) {
			color(Palettes::NITE, (Palette)p, custom_colors[p+NUM_GAME_PALETTES]);
		}
		if (pals != Palettes::NITE) {
			pals = Palettes::DAY;
		}
		break;
	case 3 * NUM_GAME_PALETTES: // MORN, DAY, NITE
		for (int l = 0; l < 3; l++) {
			for (int p = 0; p < NUM_GAME_PALETTES; p++) {
				color((Palettes)l, (Palette)p, custom_colors[p+l*NUM_GAME_PALETTES]);
			}
		}
		if (pals != Palettes::MORN && pals != Palettes::NITE) {
			pals = Palettes::DAY;
		}
		break;
	case 4 * NUM_GAME_PALETTES: // MORN, DAY, NITE, CUSTOM
		for (int l = 0; l < 3; l++) {
			for (int p = 0; p < NUM_GAME_PALETTES; p++) {
				color((Palettes)l, (Palette)p, custom_colors[p+l*NUM_GAME_PALETTES]);
			}
		}
		for (int p = 0; p < NUM_GAME_PALETTES; p++) {
			color(Palettes::CUSTOM, (Palette)p, custom_colors[p+3*NUM_GAME_PALETTES]);
		}
		if (pals == Palettes::INDOOR) {
			pals = Palettes::CUSTOM;
		}
		break;
	case 5 * NUM_GAME_PALETTES: // MORN, DAY, NITE, INDOOR, CUSTOM
		for (int l = 0; l < 5; l++) {
			for (int p = 0; p < NUM_GAME_PALETTES; p++) {
				color((Palettes)l, (Palette)p, custom_colors[p+l*NUM_GAME_PALETTES]);
			}
		}
		break;
	case 5 * NUM_GAME_PALETTES + 2: // MORN, DAY, NITE, (DARKNESS), INDOOR, MORN/DAY WATER, NITE WATER
	case 5 * NUM_GAME_PALETTES + 3: // MORN, DAY, NITE, (DARKNESS), INDOOR, MORN WATER, DAY WATER, NITE WATER
		for (int l = 0; l < 3; l++) {
			for (int p = 0; p < NUM_GAME_PALETTES; p++) {
				color((Palettes)l, (Palette)p, custom_colors[p+l*NUM_GAME_PALETTES]);
			}
		}
		// skip DARKNESS
		for (int p = 0; p < NUM_GAME_PALETTES; p++) {
			color(Palettes::INDOOR, (Palette)p, custom_colors[p+4*NUM_GAME_PALETTES]);
		}
		// apply separate WATER hues
		{
			bool two_waters = n == 5 * NUM_GAME_PALETTES + 2;
			color(Palettes::MORN, Palette::WATER, custom_colors[5*NUM_GAME_PALETTES]);
			color(Palettes::DAY,  Palette::WATER, custom_colors[5*NUM_GAME_PALETTES+(two_waters ? 0 : 1)]);
			color(Palettes::NITE, Palette::WATER, custom_colors[5*NUM_GAME_PALETTES+(two_waters ? 1 : 2)]);
		}
		if (pals == Palettes::CUSTOM) {
			pals = Palettes::DAY;
		}
		break;
	case 5 * NUM_GAME_PALETTES + 4: // MORN, DAY, NITE, CUSTOM, INDOOR, MORN WATER, DAY WATER, NITE WATER, CUSTOM WATER
		for (int l = 0; l < 3; l++) {
			for (int p = 0; p < NUM_GAME_PALETTES; p++) {
				color((Palettes)l, (Palette)p, custom_colors[p+l*NUM_GAME_PALETTES]);
			}
		}
		// DARKNESS is CUSTOM
		for (int p = 0; p < NUM_GAME_PALETTES; p++) {
			color(Palettes::CUSTOM, (Palette)p, custom_colors[p+3*NUM_GAME_PALETTES]);
		}
		for (int p = 0; p < NUM_GAME_PALETTES; p++) {
			color(Palettes::INDOOR, (Palette)p, custom_colors[p+4*NUM_GAME_PALETTES]);
		}
		// apply separate WATER hues
		for (int l = 0; l < 3; l++) {
			color((Palettes)l, Palette::WATER, custom_colors[5*NUM_GAME_PALETTES+l]);
		}
		color(Palettes::CUSTOM, Palette::WATER, custom_colors[5*NUM_GAME_PALETTES+3]);
		break;
	}
	return pals;
}

bool Color::read_roof_colors(const char *f, uint8_t map_group) {
	PalVec roof_colors = parse_palettes(f);
	if (!Config::custom_roof_color()) {
		if (roof_colors.size() < (size_t)map_group + 1) { return false; }
		// Each HueArray in a PalVec contains 4 RGB hues, so treat them as <DAY LIGHT, NITE LIGHT, DAY DARK, NITE DARK>
		color(Palettes::MORN, Palette::ROOF, Hue::LIGHT, roof_colors[map_group][(int)ordered_hue(0)]);
		color(Palettes::MORN, Palette::ROOF, Hue::DARK,  roof_colors[map_group][(int)ordered_hue(1)]);
		color(Palettes::DAY,  Palette::ROOF, Hue::LIGHT, roof_colors[map_group][(int)ordered_hue(0)]);
		color(Palettes::DAY,  Palette::ROOF, Hue::DARK,  roof_colors[map_group][(int)ordered_hue(1)]);
		color(Palettes::NITE, Palette::ROOF, Hue::LIGHT, roof_colors[map_group][(int)ordered_hue(2)]);
		color(Palettes::NITE, Palette::ROOF, Hue::DARK,  roof_colors[map_group][(int)ordered_hue(3)]);
	}
	else {
		// Each map group has 6 RGB hues (3 day parts per roof, 2 colors per day part)
		int ci = (int)map_group * 3 * 2;
		int p0 = ci / NUM_HUES, h0 = ci % NUM_HUES;
		int p1 = p0 + (h0 + 1) / NUM_HUES, h1 = (h0 + 1) % NUM_HUES;
		int p2 = p1 + (h1 + 1) / NUM_HUES, h2 = (h1 + 1) % NUM_HUES;
		int p3 = p2 + (h2 + 1) / NUM_HUES, h3 = (h2 + 1) % NUM_HUES;
		int p4 = p3 + (h3 + 1) / NUM_HUES, h4 = (h3 + 1) % NUM_HUES;
		int p5 = p4 + (h4 + 1) / NUM_HUES, h5 = (h4 + 1) % NUM_HUES;
		if (roof_colors.size() < (size_t)p5 + 1) { return false; }
		color(Palettes::MORN,   Palette::ROOF, Hue::LIGHT, roof_colors[p0][(int)ordered_hue(h0)]);
		color(Palettes::MORN,   Palette::ROOF, Hue::DARK,  roof_colors[p1][(int)ordered_hue(h1)]);
		color(Palettes::DAY,    Palette::ROOF, Hue::LIGHT, roof_colors[p0][(int)ordered_hue(h0)]);
		color(Palettes::DAY,    Palette::ROOF, Hue::DARK,  roof_colors[p1][(int)ordered_hue(h1)]);
		color(Palettes::NITE,   Palette::ROOF, Hue::LIGHT, roof_colors[p2][(int)ordered_hue(h2)]);
		color(Palettes::NITE,   Palette::ROOF, Hue::DARK,  roof_colors[p3][(int)ordered_hue(h3)]);
		color(Palettes::CUSTOM, Palette::ROOF, Hue::LIGHT, roof_colors[p4][(int)ordered_hue(h4)]);
		color(Palettes::CUSTOM, Palette::ROOF, Hue::DARK,  roof_colors[p5][(int)ordered_hue(h5)]);
	}
	return true;
}

bool Color::write_palettes(const char *f) {
	FILE *file = fl_fopen(f, "wb");
	if (!file) { return false; }
	const char *palettes_names[NUM_PALETTE_SETS] = {"morn", "day", "nite", "indoor", "custom"};
	const char *palette_names[NUM_GAME_PALETTES] = {"gray", "red", "green", "water", "yellow", "brown", "roof", "text"};
	if (Config::monochrome()) {
		for (int l = 0; l < NUM_PALETTE_SETS; l++) {
			fprintf(file, "\tRGB ");
			for (int h = 0; h < NUM_HUES; h++) {
				const uchar *rgb = color((Palettes)l, Palette::MONOCHROME, ordered_hue(h));
				if (h > 0) {
					fprintf(file, ", ");
				}
				fprintf(file, "%02u,%02u,%02u", CRGB5(rgb[0]), CRGB5(rgb[1]), CRGB5(rgb[2]));
			}
			fprintf(file, " ; %s\n", palettes_names[l]);
		}
	}
	else {
		for (int l = 0; l < NUM_PALETTE_SETS; l++) {
			fprintf(file, "; %s\n", palettes_names[l]);
			for (int p = 0; p < NUM_GAME_PALETTES; p++) {
				fprintf(file, "\tRGB ");
				for (int h = 0; h < NUM_HUES; h++) {
					const uchar *rgb = color((Palettes)l, (Palette)p, ordered_hue(h));
					if (h > 0) {
						fprintf(file, ", ");
					}
					fprintf(file, "%02u,%02u,%02u", CRGB5(rgb[0]), CRGB5(rgb[1]), CRGB5(rgb[2]));
				}
				fprintf(file, " ; %s\n", palette_names[p]);
			}
		}
	}
	fclose(file);
	return true;
}
