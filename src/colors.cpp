#include <string>
#include <fstream>
#include <sstream>

#include "utils.h"
#include "colors.h"

Tile_Colors::Tile_Colors() : _colors(), _colors_size(0), _result(COLORS_NULL) {
	clear();
}

void Tile_Colors::clear() {
	memset(_colors, Color::UNDEFINED, MAX_NUM_TILES);
	_colors_size = 0;
	_result = COLORS_NULL;
}

Tile_Colors::Result Tile_Colors::read_from(const char *f) {
	clear();
	std::ifstream ifs(f);
	if (!ifs.good()) { return (_result = BAD_COLORS_FILE); }
	std::string prefix("\ttilepal");
	while (ifs.good()) {
		std::string line;
		std::getline(ifs, line);
		if (!starts_with(line, prefix)) { continue; }
		remove_comment(line);
		std::istringstream lss(line);
		std::string token;
		std::getline(lss, token, ','); // skip "tilepal 0/1"
		while (std::getline(lss, token, ',')) {
			trim(token);
			if (token == "GRAY") {
				_colors[_colors_size++] = Color::GRAY;
			}
			else if (token == "RED") {
				_colors[_colors_size++] = Color::RED;
			}
			else if (token == "GREEN") {
				_colors[_colors_size++] = Color::GREEN;
			}
			else if (token == "WATER") {
				_colors[_colors_size++] = Color::WATER;
			}
			else if (token == "YELLOW") {
				_colors[_colors_size++] = Color::YELLOW;
			}
			else if (token == "BROWN") {
				_colors[_colors_size++] = Color::BROWN;
			}
			else if (token == "ROOF") {
				_colors[_colors_size++] = Color::ROOF;
			}
			else if (token == "TEXT") {
				_colors[_colors_size++] = Color::TEXT;
			}
			else {
				return (_result = BAD_COLOR_NAME);
			}
		}
	}
	return (_result = COLORS_OK);
}

const char *Tile_Colors::error_message(Result result) {
	switch (result) {
	case COLORS_OK:
		return "OK.";
	case BAD_COLORS_FILE:
		return "Cannot open file.";
	case BAD_COLOR_NAME:
		return "Invalid color name.";
	case COLORS_NULL:
		return "No *_palette_map.asm file chosen.";
	default:
		return "Unspecified error.";
	}
}
