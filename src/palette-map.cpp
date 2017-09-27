#include <string>
#include <fstream>
#include <sstream>
#include <array>

#include "utils.h"
#include "config.h"
#include "palette-map.h"

Palette_Map::Palette_Map() : _palette(), _palette_size(0), _result(PALETTE_NULL) {
	clear();
}

void Palette_Map::clear() {
	FILL(_palette, Palette::UNDEFINED, MAX_NUM_TILES);
	_palette_size = 0;
	_result = PALETTE_NULL;
}

Palette_Map::Result Palette_Map::read_from(const char *f) {
	clear();

	if (Config::monochrome()) {
		FILL(_palette, Palette::MONOCHROME, MAX_NUM_TILES);
		_palette_size = 128;
		return (_result = PALETTE_OK);
	}

	std::ifstream ifs(f);
	if (!ifs.good()) { return (_result = BAD_PALETTE_FILE); }
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
				_palette[_palette_size++] = Palette::GRAY;
			}
			else if (token == "RED") {
				_palette[_palette_size++] = Palette::RED;
			}
			else if (token == "GREEN") {
				_palette[_palette_size++] = Palette::GREEN;
			}
			else if (token == "WATER") {
				_palette[_palette_size++] = Palette::WATER;
			}
			else if (token == "YELLOW") {
				_palette[_palette_size++] = Palette::YELLOW;
			}
			else if (token == "BROWN") {
				_palette[_palette_size++] = Palette::BROWN;
			}
			else if (token == "ROOF") {
				_palette[_palette_size++] = Palette::ROOF;
			}
			else if (token == "TEXT") {
				_palette[_palette_size++] = Palette::TEXT;
			}
			else {
				return (_result = BAD_PALETTE_NAME);
			}
		}
	}
	return (_result = PALETTE_OK);
}

const char *Palette_Map::error_message(Result result) {
	switch (result) {
	case PALETTE_OK:
		return "OK.";
	case BAD_PALETTE_FILE:
		return "Cannot open file.";
	case BAD_PALETTE_NAME:
		return "Invalid color name.";
	case PALETTE_NULL:
		return "No palette file chosen.";
	default:
		return "Unspecified error.";
	}
}
