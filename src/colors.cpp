#include <fstream>
#include <sstream>

#include "colors.h"
#include "utils.h"

#define RGB5C(x) (uchar)((x) * 8) // (uchar)((x) * 33 / 4) for BGB instead of VBA
#define RGB5(r, g, b) {RGB5C(r), RGB5C(g), RGB5C(b)}
#define RGBX(rgb) {(((rgb) & 0xFF0000) >> 16), (((rgb) & 0xFF00) >> 8), ((rgb) & 0xFF)}

// Lighting x Palette x Hue x RGB
uchar tileset_colors[5][9][4][NUM_CHANNELS] = {
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
		{RGB5(28,31,26), RGB5( 6,13,10), RGB5(17,24,14), RGB5( 1, 3, 4)}, // MONOCHROME
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
		{RGB5(17,24,14), RGB5( 6,13,10), RGB5( 6,13,10), RGB5( 1, 3, 4)}, // MONOCHROME
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
		{RGB5(31,29,31), RGB5(20,26,31), RGB5(21,28,11), RGB5( 3, 2, 2)}, // MONOCHROME
	},
	{ // ICE_PATH
		// WHITE, DARK, LIGHT, BLACK
		{RGB5(15,14,24), RGB5( 7, 7,12), RGB5(11,11,19), RGB5( 0, 0, 0)}, // GRAY, PRIORITY_GRAY
		{RGB5(15,14,24), RGB5(13, 0, 8), RGB5(14, 7,17), RGB5( 0, 0, 0)}, // RED, PRIORITY_RED
		{RGB5(22,29,31), RGB5(31,31,31), RGB5(10,27,31), RGB5( 5, 0, 9)}, // GREEN, PRIORITY_GREEN
		{RGB5(15,14,24), RGB5( 3, 3,10), RGB5( 5, 5,17), RGB5( 0, 0, 0)}, // WATER, PRIORITY_WATER
		{RGB5(30,30,11), RGB5(16,14,10), RGB5(16,14,18), RGB5( 0, 0, 0)}, // YELLOW, PRIORITY_YELLOW
		{RGB5(15,14,24), RGB5( 8, 4, 5), RGB5(12, 9,15), RGB5( 0, 0, 0)}, // BROWN, PRIORITY_BROWN
		{RGB5(25,31,31), RGB5(16,11,31), RGB5( 9,28,31), RGB5( 5, 0, 9)}, // ROOF, PRIORITY_ROOF
		{RGB5(31, 0,31), RGB5(16, 0,16), RGB5(31, 0,31), RGB5( 0, 0, 0)}, // TEXT, PRIORITY_TEXT
		{RGB5(31,29,31), RGB5(18,24,22), RGB5(21,14, 9), RGB5( 3, 2, 2)}, // MONOCHROME
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
		{RGB5(31,31,31), RGB5(10,10,10), RGB5(20,20,20), RGB5( 0, 0, 0)}, // MONOCHROME
	},
};

static const uchar undefined_colors[4][NUM_CHANNELS] = {
	// WHITE, DARK, LIGHT, BLACK
	RGBX(0xABCDEF), RGBX(0x456789), RGBX(0x789ABC), RGBX(0x123456) // UNDEFINED
};

const uchar *Color::color(Lighting l, Palette p, Hue h) {
	int c = (int)p & 0xf;
	return p == Palette::UNDEFINED ? undefined_colors[h] : tileset_colors[l][c][h];
}

void Color::color(Lighting l, Palette p, Hue h, uchar v[3]) {
	int c = (int)p & 0xf;
	for (int i = 0; i < 3; i++) {
		tileset_colors[l][c][h][i] = RGB5C(v[i]);
	}
}

void Color::color(Lighting l, Palette p, uchar v[4][3]) {
	for (int i = 0; i < 4; i++) {
		color(l, p, (Hue)i, v[i]);
	}
}

Fl_Color Color::fl_color(Lighting l, Palette p, Hue h) {
	const uchar *rgb = color(l, p, h);
	return fl_rgb_color(rgb[0], rgb[1], rgb[2]);
}

void Color::read_custom_lighting(const char *f) {
	static const Hue hue_order[4] = {Hue::WHITE, Hue::LIGHT, Hue::DARK, Hue::BLACK};
	uchar custom_colors[8][4][NUM_CHANNELS] = {};
	int palette = 0, hue = 0, channel = 0;
	std::ifstream ifs(f);
	while (ifs.good() && palette < 8) {
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
			custom_colors[palette][hue_order[hue]][channel] = (uchar)v;
			channel++;
			if (channel == NUM_CHANNELS) {
				channel = 0;
				hue++;
				if (hue == 4) {
					hue = 0;
					palette++;
					if (palette == 8) { break; }
				}
			}
			if (sep == ';') { break; }
		}
	}
	if (palette == 1) {
		color(Lighting::CUSTOM, Palette::MONOCHROME, custom_colors[0]);
		return;
	}
	for (int p = 0; p < palette; p++) {
		color(Lighting::CUSTOM, (Palette)p, custom_colors[p]);
	}
}
