#include <cstdio>
#include <fstream>
#include <sstream>

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

// Lighting x Palette x Hue x RGB
uchar tileset_colors[NUM_LIGHTINGS][NUM_PALETTES][NUM_HUES][NUM_CHANNELS] = {
	{ // MORN
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(28,31,16), RGB5(13,13,13), RGB5(21,21,21), RGB5( 7, 7, 7)}, // GRAY
		{RGB5(28,31,16), RGB5(30,10, 6), RGB5(31,19,24), RGB5( 7, 7, 7)}, // RED
		{RGB5(22,31,10), RGB5( 5,14, 0), RGB5(12,25, 1), RGB5( 7, 7, 7)}, // GREEN
		{RGB5(28,31,16), RGB5(13,12,31), RGB5(18,19,31), RGB5( 7, 7, 7)}, // WATER
		{RGB5(28,31,16), RGB5(31,16, 1), RGB5(31,31, 7), RGB5( 7, 7, 7)}, // YELLOW
		{RGB5(28,31,16), RGB5(20,15, 3), RGB5(24,18, 7), RGB5( 7, 7, 7)}, // BROWN
		{RGB5(28,31,16), RGB5( 5,17,31), RGB5(15,31,31), RGB5( 7, 7, 7)}, // ROOF
		{RGB5(31, 0,31), RGB5(16, 0,16), RGB5(31, 0,31), RGB5( 0, 0, 0)}, // TEXT
	},
	{ // DAY
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(27,31,27), RGB5(13,13,13), RGB5(21,21,21), RGB5( 7, 7, 7)}, // GRAY
		{RGB5(27,31,27), RGB5(30,10, 6), RGB5(31,19,24), RGB5( 7, 7, 7)}, // RED
		{RGB5(22,31,10), RGB5( 5,14, 0), RGB5(12,25, 1), RGB5( 7, 7, 7)}, // GREEN
		{RGB5(27,31,27), RGB5(13,12,31), RGB5(18,19,31), RGB5( 7, 7, 7)}, // WATER
		{RGB5(27,31,27), RGB5(31,16, 1), RGB5(31,31, 7), RGB5( 7, 7, 7)}, // YELLOW
		{RGB5(27,31,27), RGB5(20,15, 3), RGB5(24,18, 7), RGB5( 7, 7, 7)}, // BROWN
		{RGB5(27,31,27), RGB5( 5,17,31), RGB5(15,31,31), RGB5( 7, 7, 7)}, // ROOF
		{RGB5(31, 0,31), RGB5(16, 0,16), RGB5(31, 0,31), RGB5( 0, 0, 0)}, // TEXT
	},
	{ // NITE
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(15,14,24), RGB5( 7, 7,12), RGB5(11,11,19), RGB5( 0, 0, 0)}, // GRAY
		{RGB5(15,14,24), RGB5(13, 0, 8), RGB5(14, 7,17), RGB5( 0, 0, 0)}, // RED
		{RGB5(15,14,24), RGB5( 0,11,13), RGB5( 8,13,19), RGB5( 0, 0, 0)}, // GREEN
		{RGB5(15,14,24), RGB5( 4, 3,18), RGB5(10, 9,20), RGB5( 0, 0, 0)}, // WATER
		{RGB5(30,30,11), RGB5(16,14,10), RGB5(16,14,18), RGB5( 0, 0, 0)}, // YELLOW
		{RGB5(15,14,24), RGB5( 8, 4, 5), RGB5(12, 9,15), RGB5( 0, 0, 0)}, // BROWN
		{RGB5(15,14,24), RGB5(11, 9,20), RGB5(13,12,23), RGB5( 0, 0, 0)}, // ROOF
		{RGB5(31, 0,31), RGB5(16, 0,16), RGB5(31, 0,31), RGB5( 0, 0, 0)}, // TEXT
	},
	{ // INDOOR
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(30,28,26), RGB5(13,13,13), RGB5(19,19,19), RGB5( 7, 7, 7)}, // GRAY
		{RGB5(30,28,26), RGB5(30,10, 6), RGB5(31,19,24), RGB5( 7, 7, 7)}, // RED
		{RGB5(18,24, 9), RGB5( 9,13, 0), RGB5(15,20, 1), RGB5( 7, 7, 7)}, // GREEN
		{RGB5(30,28,26), RGB5( 9, 9,31), RGB5(15,16,31), RGB5( 7, 7, 7)}, // WATER
		{RGB5(30,28,26), RGB5(31,16, 1), RGB5(31,31, 7), RGB5( 7, 7, 7)}, // YELLOW
		{RGB5(26,24,17), RGB5(16,13, 3), RGB5(21,17, 7), RGB5( 7, 7, 7)}, // BROWN
		{RGB5(30,28,26), RGB5(14,16,31), RGB5(17,19,31), RGB5( 7, 7, 7)}, // ROOF
		{RGB5(31, 0,31), RGB5(16, 0,16), RGB5(31, 0,31), RGB5( 0, 0, 0)}, // TEXT
	},
	{ // CUSTOM
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(28,31,28), RGB5(13,13,13), RGB5(21,21,21), RGB5( 7, 7, 7)}, // GRAY
		{RGB5(28,31,28), RGB5(24, 6, 8), RGB5(29,16,13), RGB5( 7, 7, 7)}, // RED
		{RGB5(14,25,20), RGB5( 5,14,10), RGB5( 9,19, 5), RGB5( 7, 7, 7)}, // GREEN
		{RGB5(28,31,28), RGB5( 1,12,19), RGB5( 8,20,27), RGB5( 7, 7, 7)}, // WATER
		{RGB5(28,31,28), RGB5(12,20,27), RGB5(19,26,31), RGB5( 7, 7, 7)}, // YELLOW
		{RGB5(28,31,28), RGB5(17,13,10), RGB5(22,18,15), RGB5( 7, 7, 7)}, // BROWN
		{RGB5(28,31,28), RGB5(17,13,23), RGB5(22,18,27), RGB5( 7, 7, 7)}, // ROOF
		{RGB5(28,31,28), RGB5(26, 8, 5), RGB5(31,31,31), RGB5( 0, 0, 0)}, // TEXT
	},
};

static const uchar monochrome_colors[NUM_HUES][NUM_CHANNELS] = {
	// WHITE, DARK, LIGHT, BLACK
	RGBX(0xFFFFFF), RGBX(0x555555), RGBX(0xAAAAAA), RGBX(0x000000)
};

static const uchar undefined_colors[NUM_HUES][NUM_CHANNELS] = {
	// WHITE, DARK, LIGHT, BLACK
	RGBX(0xABCDEF), RGBX(0x456789), RGBX(0x789ABC), RGBX(0x123456)
};

Hue Color::ordered_hue(int i) {
	return hue_order[i];
}

const uchar *Color::monochrome_color(Hue h) {
	return monochrome_colors[h];
}

const uchar *Color::undefined_color(Hue h) {
	return undefined_colors[h];
}

const uchar *Color::color(Lighting l, Palette p, Hue h) {
	return tileset_colors[l][p][h];
}

void Color::color(Lighting l, Palette p, Hue h, ColorArray v) {
	for (int i = 0; i < NUM_CHANNELS; i++) {
		tileset_colors[l][p][h][i] = RGB5C(v[i]);
	}
}

void Color::color(Lighting l, Palette p, Hue h, Fl_Color f) {
	uchar r, g, b;
	Fl::get_color(f, r, g, b);
	tileset_colors[l][p][h][0] = r;
	tileset_colors[l][p][h][1] = g;
	tileset_colors[l][p][h][2] = b;
}

void Color::color(Lighting l, Palette p, HueArray v) {
	for (int i = 0; i < NUM_HUES; i++) {
		color(l, p, (Hue)i, v[i]);
	}
}

Fl_Color Color::fl_color(Lighting l, Palette p, Hue h) {
	const uchar *rgb = color(l, p, h);
	return fl_rgb_color(rgb[0], rgb[1], rgb[2]);
}

PalVec Color::parse_lighting(const char *f) {
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
			colors[palette][ordered_hue(hue)][channel] = (uchar)v;
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

Lighting Color::read_lighting(const char *f, Lighting lighting) {
	PalVec custom_colors = parse_lighting(f);
	size_t n = custom_colors.size();
	switch (n) {
	case 1 * NUM_PALETTES: // CUSTOM
		for (int p = 0; p < NUM_PALETTES; p++) {
			color(Lighting::CUSTOM, (Palette)p, custom_colors[p]);
		}
		lighting = Lighting::CUSTOM;
		break;
	case 2 * NUM_PALETTES: // DAY, NITE
		for (int p = 0; p < NUM_PALETTES; p++) {
			color(Lighting::DAY, (Palette)p, custom_colors[p]);
		}
		for (int p = 0; p < NUM_PALETTES; p++) {
			color(Lighting::NITE, (Palette)p, custom_colors[p+NUM_PALETTES]);
		}
		if (lighting != Lighting::NITE) {
			lighting = Lighting::DAY;
		}
		break;
	case 3 * NUM_PALETTES: // MORN, DAY, NITE
		for (int l = 0; l < 3; l++) {
			for (int p = 0; p < NUM_PALETTES; p++) {
				color((Lighting)l, (Palette)p, custom_colors[p+l*NUM_PALETTES]);
			}
		}
		if (lighting != Lighting::MORN && lighting != Lighting::NITE) {
			lighting = Lighting::DAY;
		}
		break;
	case 4 * NUM_PALETTES: // MORN, DAY, NITE, INDOOR
		for (int l = 0; l < 4; l++) {
			for (int p = 0; p < NUM_PALETTES; p++) {
				color((Lighting)l, (Palette)p, custom_colors[p+l*NUM_PALETTES]);
			}
		}
		if (lighting == Lighting::CUSTOM) {
			lighting = Lighting::DAY;
		}
		break;
	case 5 * NUM_PALETTES: // MORN, DAY, NITE, INDOOR, CUSTOM
		for (int l = 0; l < 5; l++) {
			for (int p = 0; p < NUM_PALETTES; p++) {
				color((Lighting)l, (Palette)p, custom_colors[p+l*NUM_PALETTES]);
			}
		}
		break;
	case 5 * NUM_PALETTES + 2: // MORN, DAY, NITE, (DARKNESS), INDOOR, MORN/DAY WATER, NITE WATER
	case 5 * NUM_PALETTES + 3: // MORN, DAY, NITE, (DARKNESS), INDOOR, MORN WATER, DAY WATER, NITE WATER
		for (int l = 0; l < 3; l++) {
			for (int p = 0; p < NUM_PALETTES; p++) {
				color((Lighting)l, (Palette)p, custom_colors[p+l*NUM_PALETTES]);
			}
		}
		// skip DARKNESS
		for (int p = 0; p < NUM_PALETTES; p++) {
			color(Lighting::INDOOR, (Palette)p, custom_colors[p+4*NUM_PALETTES]);
		}
		// apply separate WATER hues
		bool two_waters = n == 5 * NUM_PALETTES + 2;
		color(Lighting::MORN, Palette::WATER, custom_colors[NUM_PALETTES*5]);
		color(Lighting::DAY,  Palette::WATER, custom_colors[NUM_PALETTES*5+(two_waters ? 0 : 1)]);
		color(Lighting::NITE, Palette::WATER, custom_colors[NUM_PALETTES*5+(two_waters ? 1 : 2)]);
		if (lighting == Lighting::CUSTOM) {
			lighting = Lighting::DAY;
		}
		break;
	}
	return lighting;
}

bool Color::read_roof_colors(const char *f, uint8_t map_group) {
	PalVec roof_colors = parse_lighting(f);
	if (roof_colors.size() < (size_t)map_group + 1) { return false; }

	// Each HueArray in a PalVec contains 4 RGB hues, so treat them as <DAY LIGHT, NITE LIGHT, DAY DARK, NITE DARK>
	color(Lighting::MORN, Palette::ROOF, Hue::LIGHT, roof_colors[map_group][ordered_hue(0)]);
	color(Lighting::MORN, Palette::ROOF, Hue::DARK,  roof_colors[map_group][ordered_hue(1)]);
	color(Lighting::DAY,  Palette::ROOF, Hue::LIGHT, roof_colors[map_group][ordered_hue(0)]);
	color(Lighting::DAY,  Palette::ROOF, Hue::DARK,  roof_colors[map_group][ordered_hue(1)]);
	color(Lighting::NITE, Palette::ROOF, Hue::LIGHT, roof_colors[map_group][ordered_hue(2)]);
	color(Lighting::NITE, Palette::ROOF, Hue::DARK,  roof_colors[map_group][ordered_hue(3)]);

	return true;
}

bool Color::write_lighting(const char *f, Lighting lighting) {
	FILE *file = fl_fopen(f, "wb");
	if (!file) { return false; }
	const char *names[NUM_PALETTES] = {"gray", "red", "green", "water", "yellow", "brown", "roof", "text"};
	for (int p = 0; p < NUM_PALETTES; p++) {
		fprintf(file, "; %s\n", names[p]);
		for (int h = 0; h < NUM_HUES; h++) {
			const uchar *rgb = color(lighting, (Palette)p, ordered_hue(h));
			fprintf(file, "\tRGB %02u, %02u, %02u\n", CRGB5(rgb[0]), CRGB5(rgb[1]), CRGB5(rgb[2]));
		}
	}
	fclose(file);
	return true;
}
