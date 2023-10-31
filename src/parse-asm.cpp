#include <string>
#include <fstream>
#include <sstream>

#include "utils.h"
#include "parse-asm.h"

Parsed_Asm::Parsed_Asm(const char *f) : _data(), _result(Result::ASM_NULL) {
	parse_asm(f);
}

static uint32_t parse_value(std::string s) {
	int n = 0;
	trim(s);
	size_t c = s.length();
	if (!s.empty()) {
		if (s[0] == '$') {
			s.erase(0, 1);
			n = strtol(s.c_str(), NULL, 16);
		}
		else if (s[0] == '%') {
			s.erase(0, 1);
			n = strtol(s.c_str(), NULL, 2);
		}
		else if (s[c-1] == 'h' || s[c-1] == 'H') {
			s.erase(c - 1);
			n = strtol(s.c_str(), NULL, 16);
		}
		else {
			n = strtol(s.c_str(), NULL, 10);
		}
	}
	return (uint32_t)n;
}

Parsed_Asm::Result Parsed_Asm::parse_asm(const char *f) {
	std::ifstream ifs;
	open_ifstream(ifs, f);
	if (!ifs.good()) { return (_result = Result::ASM_BAD_FILE); }

	while (ifs.good()) {
		std::string line;
		std::getline(ifs, line);
		remove_comment(line);
		std::istringstream lss(line);

		std::string macro;
		if (!leading_macro(lss, macro)) { continue; }
		bool words = equals_ignore_case(macro, "dw");
		if (!words && !equals_ignore_case(macro, "db")) { continue; }

		for (std::string token; std::getline(lss, token, ',');) {
			uint32_t v = parse_value(token);
			if (words) {
				if (v > 0xFFFF) { return (_result = Result::ASM_OVERFLOW); }
				_data.push_back((uint8_t)(v & 0xFF)); // low
				_data.push_back((uint8_t)((v & 0xFF00) >> 8)); // high
			}
			else {
				if (v > 0xFF) { return (_result = Result::ASM_OVERFLOW); }
				_data.push_back((uint8_t)v);
			}
		}
	}

	return (_result = Result::ASM_OK);
}
