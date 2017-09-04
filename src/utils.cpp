#pragma warning(push, 0)
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "utils.h"

const std::string whitespace(" \f\n\r\t\v");

bool starts_with(const std::string &s, const std::string &p) {
	return !s.compare(0, p.size(), p);
}

bool ends_with(const std::string &s, const std::string &p) {
	return s.size() >= p.size() && !s.compare(s.size() - p.size(), p.size(), p);
}

void trim(std::string &s, const std::string &t) {
	std::string::size_type p = s.find_first_not_of(t);
	s.erase(0, p);
	p = s.find_last_not_of(t);
	s.erase(p + 1);
}

void remove_comment(std::string &s, char c) {
	size_t p = s.find(c);
	if (p != std::string::npos) {
		s.erase(p);
	}
}

int text_width(const char *l, int pad) {
	int lw = 0, lh = 0;
	fl_measure(l, lw, lh, 0);
	return lw + 2 * pad;
}
