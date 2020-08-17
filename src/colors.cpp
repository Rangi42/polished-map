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

// Canonical hue color channels
static const uchar hue_monos[NUM_HUES] = {0xFF, 0x55, 0xAA, 0x00}; // WHITE, DARK, LIGHT, BLACK

// Monochrome-color-to-hue conversion
static const Hue mono_hues[NUM_HUES] = {Hue::BLACK, Hue::DARK, Hue::LIGHT, Hue::WHITE}; // 00-3F, 40-7F, 80-BF, C0-FF

// Palettes x Palette x Hue x RGB
uchar tileset_colors[NUM_PALETTE_SETS][NUM_PALETTES][NUM_HUES][NUM_CHANNELS] = {
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

const uchar *Color::monochrome_color(Hue h) {
	return monochrome_colors[(int)h];
}

const uchar *Color::undefined_color(Hue h) {
	return undefined_colors[(int)h];
}

const uchar *Color::color(Palettes l, Palette p, Hue h) {
	return tileset_colors[(int)l][(int)p][(int)h];
}

void Color::color(Palettes l, Palette p, Hue h, ColorArray v) {
	uchar *cs = tileset_colors[(int)l][(int)p][(int)h];
	for (int i = 0; i < NUM_CHANNELS; i++) {
		cs[i] = RGB5C(v[i]);
	}
}

void Color::color(Palettes l, Palette p, Hue h, Fl_Color f) {
	uchar *cs = tileset_colors[(int)l][(int)p][(int)h];
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
	case 1 * NUM_PALETTES: // CUSTOM
		for (int p = 0; p < NUM_PALETTES; p++) {
			color(Palettes::CUSTOM, (Palette)p, custom_colors[p]);
		}
		pals = Palettes::CUSTOM;
		break;
	case 2 * NUM_PALETTES: // DAY, NITE
		for (int p = 0; p < NUM_PALETTES; p++) {
			color(Palettes::DAY, (Palette)p, custom_colors[p]);
		}
		for (int p = 0; p < NUM_PALETTES; p++) {
			color(Palettes::NITE, (Palette)p, custom_colors[p+NUM_PALETTES]);
		}
		if (pals != Palettes::NITE) {
			pals = Palettes::DAY;
		}
		break;
	case 3 * NUM_PALETTES: // MORN, DAY, NITE
		for (int l = 0; l < 3; l++) {
			for (int p = 0; p < NUM_PALETTES; p++) {
				color((Palettes)l, (Palette)p, custom_colors[p+l*NUM_PALETTES]);
			}
		}
		if (pals != Palettes::MORN && pals != Palettes::NITE) {
			pals = Palettes::DAY;
		}
		break;
	case 4 * NUM_PALETTES: // MORN, DAY, NITE, CUSTOM
		for (int l = 0; l < 3; l++) {
			for (int p = 0; p < NUM_PALETTES; p++) {
				color((Palettes)l, (Palette)p, custom_colors[p+l*NUM_PALETTES]);
			}
		}
		for (int p = 0; p < NUM_PALETTES; p++) {
			color(Palettes::CUSTOM, (Palette)p, custom_colors[p+3*NUM_PALETTES]);
		}
		if (pals == Palettes::INDOOR) {
			pals = Palettes::CUSTOM;
		}
		break;
	case 5 * NUM_PALETTES: // MORN, DAY, NITE, INDOOR, CUSTOM
		for (int l = 0; l < 5; l++) {
			for (int p = 0; p < NUM_PALETTES; p++) {
				color((Palettes)l, (Palette)p, custom_colors[p+l*NUM_PALETTES]);
			}
		}
		break;
	case 5 * NUM_PALETTES + 2: // MORN, DAY, NITE, (DARKNESS), INDOOR, MORN/DAY WATER, NITE WATER
	case 5 * NUM_PALETTES + 3: // MORN, DAY, NITE, (DARKNESS), INDOOR, MORN WATER, DAY WATER, NITE WATER
		for (int l = 0; l < 3; l++) {
			for (int p = 0; p < NUM_PALETTES; p++) {
				color((Palettes)l, (Palette)p, custom_colors[p+l*NUM_PALETTES]);
			}
		}
		// skip DARKNESS
		for (int p = 0; p < NUM_PALETTES; p++) {
			color(Palettes::INDOOR, (Palette)p, custom_colors[p+4*NUM_PALETTES]);
		}
		// apply separate WATER hues
		{
			bool two_waters = n == 5 * NUM_PALETTES + 2;
			color(Palettes::MORN, Palette::WATER, custom_colors[5*NUM_PALETTES]);
			color(Palettes::DAY,  Palette::WATER, custom_colors[5*NUM_PALETTES+(two_waters ? 0 : 1)]);
			color(Palettes::NITE, Palette::WATER, custom_colors[5*NUM_PALETTES+(two_waters ? 1 : 2)]);
		}
		if (pals == Palettes::CUSTOM) {
			pals = Palettes::DAY;
		}
		break;
	case 5 * NUM_PALETTES + 4: // MORN, DAY, NITE, CUSTOM, INDOOR, MORN WATER, DAY WATER, NITE WATER, CUSTOM WATER
		for (int l = 0; l < 3; l++) {
			for (int p = 0; p < NUM_PALETTES; p++) {
				color((Palettes)l, (Palette)p, custom_colors[p+l*NUM_PALETTES]);
			}
		}
		// DARKNESS is CUSTOM
		for (int p = 0; p < NUM_PALETTES; p++) {
			color(Palettes::CUSTOM, (Palette)p, custom_colors[p+3*NUM_PALETTES]);
		}
		for (int p = 0; p < NUM_PALETTES; p++) {
			color(Palettes::INDOOR, (Palette)p, custom_colors[p+4*NUM_PALETTES]);
		}
		// apply separate WATER hues
		for (int l = 0; l < 3; l++) {
			color((Palettes)l, Palette::WATER, custom_colors[5*NUM_PALETTES+l]);
		}
		color(Palettes::CUSTOM, Palette::WATER, custom_colors[5*NUM_PALETTES+3]);
		break;
	}
	return pals;
}

bool Color::read_roof_colors(const char *f, uint8_t map_group, Roof_Palettes roof_palettes) {
	PalVec roof_colors = parse_palettes(f);
	int num_palettes = 0;
	Palettes palettes[4] = {};
	switch (roof_palettes) {
	case Roof_Palettes::ROOF_CUSTOM:
		num_palettes = 1;
		palettes[0] = Palettes::CUSTOM;
		break;
	case Roof_Palettes::ROOF_DAY_NITE:
		num_palettes = 2;
		palettes[0] = Palettes::DAY;
		palettes[1] = Palettes::NITE;
		break;
	case Roof_Palettes::ROOF_MORN_DAY_NITE:
		num_palettes = 3;
		palettes[0] = Palettes::MORN;
		palettes[1] = Palettes::DAY;
		palettes[2] = Palettes::NITE;
		break;
	case Roof_Palettes::ROOF_DAY_NITE_CUSTOM:
		num_palettes = 3;
		palettes[0] = Palettes::DAY;
		palettes[1] = Palettes::NITE;
		palettes[2] = Palettes::CUSTOM;
		break;
	case Roof_Palettes::ROOF_MORN_DAY_NITE_CUSTOM:
		num_palettes = 4;
		palettes[0] = Palettes::MORN;
		palettes[1] = Palettes::DAY;
		palettes[2] = Palettes::NITE;
		palettes[3] = Palettes::CUSTOM;
		break;
	}
	if (num_palettes < 0 || num_palettes > 4) { return false; }
	// Each HueArray in a PalVec contains 4 RGB hues
	int ps[8] = {}, hs[8] = {};
	int ci = (int)map_group * num_palettes * 2;
	ps[0] = ci / NUM_HUES;
	hs[0] = ci % NUM_HUES;
	for (int i = 1; i < num_palettes * 2; i++) {
		ps[i] = ps[i-1] + (hs[i-1] + 1) / NUM_HUES;
		hs[i] = (hs[i-1] + 1) % NUM_HUES;
	}
	if (roof_colors.size() < (size_t)ps[num_palettes*2-1] + 1) { return false; }
	for (int k = 0; k < num_palettes; k++) {
		color(palettes[k], Palette::ROOF, Hue::LIGHT, roof_colors[ps[k*2]][(int)ordered_hue(hs[k*2])]);
		color(palettes[k], Palette::ROOF, Hue::DARK,  roof_colors[ps[k*2+1]][(int)ordered_hue(hs[k*2+1])]);
	}
	if (roof_palettes == Roof_Palettes::ROOF_DAY_NITE || roof_palettes == Roof_Palettes::ROOF_DAY_NITE_CUSTOM) {
		color(Palettes::MORN, Palette::ROOF, Hue::LIGHT, fl_color(Palettes::DAY, Palette::ROOF, Hue::LIGHT));
		color(Palettes::MORN, Palette::ROOF, Hue::DARK,  fl_color(Palettes::DAY, Palette::ROOF, Hue::DARK));
	}
	return true;
}

bool Color::write_palettes(const char *f) {
	FILE *file = fl_fopen(f, "wb");
	if (!file) { return false; }
	const char *palettes_names[NUM_PALETTE_SETS] = {"morn", "day", "nite", "indoor", "custom"};
	const char *palette_names[NUM_PALETTES] = {"gray", "red", "green", "water", "yellow", "brown", "roof", "text"};
	for (int l = 0; l < NUM_PALETTE_SETS; l++) {
		fprintf(file, "; %s\n", palettes_names[l]);
		for (int p = 0; p < NUM_PALETTES; p++) {
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
	fclose(file);
	return true;
}
