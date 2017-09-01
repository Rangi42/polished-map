#include <string>
#include <fstream>
#include <sstream>

#include "palette-map.h"

static const std::string whitespace(" \f\n\r\t\v");

static bool startswith(std::string &s, std::string &p) {
	return !s.compare(0, p.size(), p);
}

static void trim(std::string &s, const std::string &t = whitespace) {
	std::string::size_type p = s.find_first_not_of(t);
	s.erase(0, p);
	p = s.find_last_not_of(t);
	s.erase(p + 1);
}

static void remove_comment(std::string &s, char c = ';') {
	size_t p = s.find(c);
	if (p != std::string::npos) {
		s.erase(p);
	}
}

Palette_Map::Palette_Map() : _palette(), _palette_size(0) {}

bool Palette_Map::read_from(const char *f) {
	clear();
	std::string prefix("\ttilepal"), skip_macro("\ttilepal_skip");
	std::ifstream ifs(f);
	if (!ifs.good()) { return false; }
	while (ifs.good()) {
		std::string line;
		std::getline(ifs, line);
		if (startswith(line, skip_macro)) {
			for (int i = 0; i < 8; i++) {
				_palette[_palette_size++] = Palette::TEXT;
			}
			continue;
		}
		if (!startswith(line, prefix)) { continue; }
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
				return false;
			}
		}
		return true;
	}
}
