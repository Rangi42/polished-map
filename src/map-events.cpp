#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

#pragma warning(push, 0)
#include <FL/fl_utf8.H>
#pragma warning(pop)

#include "map-events.h"

struct Event_Meta {
public:
	char symbol;
	uint8_t texture; // EventTexture, stored as uint8_t for struct packing
	uint8_t skip;
	bool yx;
};

static const std::unordered_map<std::string, Event_Meta> events_meta = {
	// pokecrystal
	{"warp_event",            {'W', TX_PURPLE, 0, false}},
	{"coord_event",           {'C', TX_GREEN,  0, false}},
	{"bg_event",              {'B', TX_RED,    0, false}},
	{"object_event",          {'O', TX_BLUE,   0, false}},
	// old pokecrystal
	{"warp_def",              {'W', TX_PURPLE, 0, true}},
	{"xy_trigger",            {'X', TX_GREEN,  1, true}},
	{"signpost",              {'S', TX_RED,    0, true}},
	{"person_event",          {'P', TX_BLUE,   1, true}},
	// Prism
	{"dummy_warp",            {'D', TX_PURPLE, 0, true}},
	// Polished Crystal
	{"itemball_event",        {'I', TX_AZURE,  0, false}},
	{"tmhmball_event",        {'T', TX_AZURE,  0, false}},
	{"cuttree_event",         {'C', TX_AZURE,  0, false}},
	{"fruittree_event",       {'F', TX_AZURE,  0, false}},
	{"strengthboulder_event", {'B', TX_AZURE,  0, false}},
	{"smashrock_event",       {'R', TX_AZURE,  0, false}},
	{"pc_nurse_event",        {'N', TX_AZURE,  0, false}},
	{"mart_clerk_event",      {'M', TX_AZURE,  0, false}},
};

Map_Events::Map_Events() : _events(), _result(MAP_EVENTS_NULL), _modified(false) {}

Map_Events::~Map_Events() {
	clear();
}

void Map_Events::clear() {
	_events.clear();
	_result = MAP_EVENTS_NULL;
	_modified = false;
}

void Map_Events::resize_events(int x, int y, int s) {
	size_t n = size();
	for (size_t i = 0; i < n; i++) {
		Event *e = _events[i];
		e->resize(x + e->event_x() * s, y + e->event_y() * s, s, s);
	}
}

static uint8_t parse_number(std::string s) {
	int n = 0;
	trim(s);
	if (!s.empty()) {
		if (s[0] == '$') {
			s.erase(0, 1);
			n = std::stoi(s, NULL, 16);
		}
		else if (s[0] == '%') {
			s.erase(0, 1);
			n = std::stoi(s, NULL, 2);
		}
		else {
			n = std::stoi(s, NULL);
		}
	}
	return (uint8_t)n;
}

Map_Events::Result Map_Events::read_events(const char *f) {
	std::ifstream ifs(f);
	if (!ifs.good()) { return (_result = BAD_MAP_EVENTS_FILE); }

	while (ifs.good()) {
		std::string line;
		std::getline(ifs, line);
		std::string tooltip(line);
		trim(tooltip);
		remove_comment(line);
		std::istringstream lss(line);
		std::string token;
		std::getline(lss, token, ' ');
		trim(token);
		const auto it = events_meta.find(token);
		if (it == events_meta.end()) {
			continue;
		}
		Event_Meta meta = it->second;
		for (uint8_t i = 0; i < meta.skip; i++) {
			std::getline(lss, token, ',');
		}
		std::getline(lss, token, ',');
		uint8_t x = parse_number(token);
		std::getline(lss, token, ',');
		uint8_t y = parse_number(token);
		if (meta.yx) {
			std::swap(x, y);
		}
		_events.push_back(new Event(x, y, meta.symbol, (EventTexture)meta.texture, tooltip));
	}

	return (_result = MAP_EVENTS_OK);
}

const char *Map_Events::error_message(Result result) {
	switch (result) {
	case MAP_EVENTS_OK:
		return "OK.";
	case BAD_MAP_EVENTS_FILE:
		return "Cannot open file.";
	case MAP_EVENTS_NULL:
		return "No *.asm file chosen.";
	default:
		return "Unspecified error.";
	}
}
