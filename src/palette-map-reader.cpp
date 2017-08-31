#include <string>
#include <fstream>
#include <sstream>

#include "palette-map-reader.h"

static const std::string whitespace(" \f\n\r\t\v");

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

Palette_Map_Reader::Palette_Map_Reader(const char *f) : _good(false), _palette(), _palette_size(0) {
	std::ifstream ifs(f);
	if (!ifs.good()) { return; }
	while (ifs.good()) {
		std::string line;
		std::getline(ifs, line);
		std::istringstream lss(line);
		//std::getline(lss, letter_s, ',');
	}
}
