#include <string>
#include <fstream>
#include <sstream>

#pragma warning(push, 0)
#include <FL/filename.H>
#include <FL/fl_utf8.h>
#pragma warning(pop)

#include "map-events.h"

static const std::unordered_map<std::string, Event_Meta> events_meta = {
	// pokecrystal
	{"warp_event",             {EventTexture::TX_PURPLE, 'W', 0, false, false}},
	{"coord_event",            {EventTexture::TX_GREEN,  'C', 0, false, false}},
	{"bg_event",               {EventTexture::TX_RED,    'B', 0, false, false}},
	{"object_event",           {EventTexture::TX_BLUE,   'O', 0, false, false}},
	// old pokecrystal
	{"warp_def",               {EventTexture::TX_PURPLE, 'W', 0, true,  true}},
	{"xy_trigger",             {EventTexture::TX_GREEN,  'X', 1, true,  false}},
	{"signpost",               {EventTexture::TX_RED,    'S', 0, true,  false}},
	{"person_event",           {EventTexture::TX_BLUE,   'P', 1, true,  false}},
	// Prism
	{"dummy_warp",             {EventTexture::TX_PURPLE, 'D', 0, true,  false}},
	// Polished Crystal
	{"itemball_event",         {EventTexture::TX_AZURE,  'I', 0, false, false}},
	{"keyitemball_event",      {EventTexture::TX_AZURE,  'K', 0, false, false}},
	{"tmhmball_event",         {EventTexture::TX_AZURE,  'T', 0, false, false}},
	{"cuttree_event",          {EventTexture::TX_AZURE,  'C', 0, false, false}},
	{"fruittree_event",        {EventTexture::TX_AZURE,  'F', 0, false, false}},
	{"strengthboulder_event",  {EventTexture::TX_AZURE,  'B', 0, false, false}},
	{"smashrock_event",        {EventTexture::TX_AZURE,  'R', 0, false, false}},
	{"pokemon_event",          {EventTexture::TX_AZURE,  'P', 0, false, false}},
	{"pc_nurse_event",         {EventTexture::TX_AZURE,  'N', 0, false, false}},
	{"mart_clerk_event",       {EventTexture::TX_AZURE,  'M', 0, false, false}},
	// Red++
	{"pc_chansey_event",       {EventTexture::TX_ORANGE, 'C', 0, false, false}},
	{"pc_blissey_event",       {EventTexture::TX_ORANGE, 'B', 0, false, false}},
	{"cavebase_event",         {EventTexture::TX_ORANGE, 'V', 0, false, false}},
	{"treebase_left_event_1",  {EventTexture::TX_ORANGE, '\xbf', 0, false, false}}, // U+00BF INVERTED QUESTION MARK
	{"treebase_left_event_2",  {EventTexture::TX_ORANGE, '\xa1', 0, false, false}}, // U+00A1 INVERTED EXCLAMATION MARK
	{"treebase_right_event_1", {EventTexture::TX_ORANGE, '?', 0, false, false}},
	{"treebase_right_event_2", {EventTexture::TX_ORANGE, '!', 0, false, false}},
};

Map_Events::Map_Events() : _map_name(), _events(), _warps(), _result(Result::MAP_EVENTS_NULL),
	_loaded(false), _modified(false), _mod_time(0) {}

Map_Events::~Map_Events() {
	clear();
}

void Map_Events::clear() {
	_map_name[0] = '\0';
	_events.clear();
	_warps.clear();
	_result = Result::MAP_EVENTS_NULL;
	_loaded = false;
	_modified = false;
	_mod_time = 0;
}

void Map_Events::resize_events(int x, int y, int s) const {
	for (Event *e : _events) {
		int ex = x + ((int)e->event_x() + EVENT_MARGIN) * s;
		int ey = y + ((int)e->event_y() + EVENT_MARGIN) * s;
		e->resize(ex, ey, s, s);
	}
}

void Map_Events::refresh_warps() {
	for (Event *e : _events) {
		auto [name, id] = e->warp_destination();
		e->warp_to(name == _map_name && id > 0 ? _warps[id] : NULL);
	}
}

Map_Events::Result Map_Events::read_events(const char *f) {
	std::ifstream ifs;
	open_ifstream(ifs, f);
	if (!ifs.good()) { return (_result = Result::BAD_MAP_EVENTS_FILE); }

	size_t n = 0;
	std::string prelude;
	int warp_id = 1;
	while (ifs.good()) {
		std::string line;
		std::getline(ifs, line);
		n++;
		std::istringstream lss(line);
		std::string token;
		if (!leading_macro(lss, token)) {
			prelude += line + '\n';
			continue;
		}
		const auto it = events_meta.find(token);
		if (it == events_meta.end()) {
			prelude += line + '\n';
			continue;
		}

		bool is_warp = it->second.is_warp();
		Event *e = new Event(n, prelude, token, it->second, line, is_warp ? warp_id : 0);
		if (is_warp) {
			_warps[warp_id++] = e;
		}
		e->parse(lss);
		_events.push_back(e);
		prelude.clear();
	}
	if (!prelude.empty()) {
		prelude.pop_back(); // remove extra newline
	}
	_coda = prelude;

	remove_dot_ext(f, _map_name);
	refresh_warps();

	_loaded = true;
	_modified = false;
	_mod_time = file_modified(f);

	return (_result = Result::MAP_EVENTS_OK);
}

const char *Map_Events::error_message(Result result) {
	switch (result) {
	case Result::MAP_EVENTS_OK:
		return "OK.";
	case Result::BAD_MAP_EVENTS_FILE:
		return "Cannot open file.";
	case Result::MAP_EVENTS_NULL:
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
	_mod_time = file_modified(f);
	return true;
}
