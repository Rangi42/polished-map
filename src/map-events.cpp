#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

#pragma warning(push, 0)
#include <FL/fl_utf8.h>
#pragma warning(pop)

#include "map-events.h"

static const std::unordered_map<std::string, Event_Meta> events_meta = {
	// pokered
	{"warp",                  {TX_PURPLE, 'W', 0, false, 2}},
	{"sign",                  {TX_RED,    'S', 0, false, 0}},
	{"object",                {TX_BLUE,   'O', 1, false, 0}},
	{"warp_to",               {TX_ORANGE, 'T', 0, false, 0}},
	// pokecrystal
	{"warp_event",            {TX_PURPLE, 'W', 0, false, 1}},
	{"coord_event",           {TX_GREEN,  'C', 0, false, 0}},
	{"bg_event",              {TX_RED,    'B', 0, false, 0}},
	{"object_event",          {TX_BLUE,   'O', 0, false, 0}},
	// old pokecrystal
	{"warp_def",              {TX_PURPLE, 'W', 0, true,  2}},
	{"xy_trigger",            {TX_GREEN,  'X', 1, true,  0}},
	{"signpost",              {TX_RED,    'S', 0, true,  0}},
	{"person_event",          {TX_BLUE,   'P', 1, true,  0}},
	// Prism
	{"dummy_warp",            {TX_PURPLE, 'D', 0, true,  0}},
	// Polished Crystal
	{"itemball_event",        {TX_AZURE,  'I', 0, false, 0}},
	{"tmhmball_event",        {TX_AZURE,  'T', 0, false, 0}},
	{"cuttree_event",         {TX_AZURE,  'C', 0, false, 0}},
	{"fruittree_event",       {TX_AZURE,  'F', 0, false, 0}},
	{"strengthboulder_event", {TX_AZURE,  'B', 0, false, 0}},
	{"smashrock_event",       {TX_AZURE,  'R', 0, false, 0}},
	{"pc_nurse_event",        {TX_AZURE,  'N', 0, false, 0}},
	{"mart_clerk_event",      {TX_AZURE,  'M', 0, false, 0}},
};

Map_Events::Map_Events() : _events(), _result(MAP_EVENTS_NULL), _loaded(false), _modified(false) {}

Map_Events::~Map_Events() {
	clear();
}

void Map_Events::clear() {
	_events.clear();
	_result = MAP_EVENTS_NULL;
	_loaded = false;
	_modified = false;
}

void Map_Events::resize_events(int x, int y, int s) {
	size_t n = size();
	for (size_t i = 0; i < n; i++) {
		Event *e = _events[i];
		int ex = x + ((int)e->event_x() + EVENT_MARGIN) * s;
		int ey = y + ((int)e->event_y() + EVENT_MARGIN) * s;
		e->resize(ex, ey, s, s);
	}
}

Map_Events::Result Map_Events::read_events(const char *f) {
	std::ifstream ifs(f);
	if (!ifs.good()) { return (_result = BAD_MAP_EVENTS_FILE); }

	size_t n = 0;
	std::string prelude;
	while (ifs.good()) {
		std::string line;
		std::getline(ifs, line);
		n++;
		std::istringstream lss(line);
		std::string token;
		std::getline(lss, token, ' ');
		trim(token);
		const auto it = events_meta.find(token);
		if (it == events_meta.end()) {
			prelude += line + '\n';
			continue;
		}

		if (token == "warp") {
			// warp is a script command in pokecrystal, but with 2 commas, not 3
			std::string tail(lss.str().substr((size_t)lss.tellg()));
			remove_comment(tail);
			if (std::count(tail.begin(), tail.end(), ',') != 3) {
				prelude += line + '\n';
				continue;
			}
		}

		Event *e = new Event(n, prelude, token, it->second, line);
		e->parse(lss);
		_events.push_back(e);
		prelude.clear();
	}
	if (!prelude.empty()) {
		prelude.pop_back(); // remove extra newline
	}
	_coda = prelude;

	_loaded = true;
	_modified = false;

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

bool Map_Events::write_event_script(const char *f) {
	FILE *file = fl_fopen(f, "wb");
	if (!file) { return false; }
	for (Event *e : _events) {
		fputs(e->prelude().c_str(), file);
		fputc('\t', file);
		fputs(e->tip().c_str(), file);
		fputc('\n', file);
	}
	fputs(_coda.c_str(), file);
	fclose(file);
	return true;
}
