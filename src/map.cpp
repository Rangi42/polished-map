#include <cstdio>

#include "map.h"

void Map_Attributes::clear() {
	group = 0;
	environment.clear();
	landmark.clear();
	palette.clear();
}

Map::Map() : _attributes(), _width(0), _height(0), _blocks(NULL), _result(MAP_NULL),
	_modified(false), _history(MAX_HISTORY_SIZE), _future(MAX_HISTORY_SIZE) {}

Map::~Map() {
	clear();
}

void Map::size(uint8_t w, uint8_t h) {
	clear();
	_width = w;
	_height = h;
	_blocks = new Block *[size()]();
}

void Map::block(uint8_t x, uint8_t y, Block *b) {
	_blocks[(size_t)y * _width + (size_t)x] = b;
}

Block *Map::block_under(Event *e) {
	if (!e) { return NULL; }
	uint8_t bx = e->event_x() / 2, by = e->event_y() / 2;
	return bx < _width && by < _height ? block(bx, by) : NULL;
}

void Map::clear() {
	delete [] _blocks;
	_blocks = NULL;
	_attributes.clear();
	_width = _height = 0;
	_result = MAP_NULL;
	_modified = false;
	_history.clear();
	_future.clear();
}

void Map::resize_blocks(int x, int y, int s) {
	size_t n = size();
	for (size_t i = 0; i < n; i++) {
		Block *b = _blocks[i];
		b->resize(x + b->col() * s, y + b->row() * s, s, s);
	}
}

void Map::remember() {
	_future.clear();
	while (_history.size() >= MAX_HISTORY_SIZE) { _history.pop_front(); }

	Map_State ms(size());
	for (size_t i = 0; i < size(); i++) {
		ms.ids[i] = block(i)->id();
	}
	_history.push_back(ms);
}

void Map::undo() {
	if (_history.empty()) { return; }
	while (_future.size() >= MAX_HISTORY_SIZE) { _future.pop_front(); }

	Map_State ms(size());
	for (size_t i = 0; i < size(); i++) {
		ms.ids[i] = block(i)->id();
	}
	_future.push_back(ms);

	const Map_State &prev = _history.back();
	for (size_t i = 0; i < size(); i++) {
		block(i)->id(prev.ids[i]);
	}
	_history.pop_back();
}

void Map::redo() {
	if (_future.empty()) { return; }
	while (_history.size() >= MAX_HISTORY_SIZE) { _history.pop_front(); }

	Map_State ms(size());
	for (size_t i = 0; i < size(); i++) {
		ms.ids[i] = block(i)->id();
	}
	_history.push_back(ms);

	const Map_State &next = _future.back();
	for (size_t i = 0; i < size(); i++) {
		block(i)->id(next.ids[i]);
	}
	_future.pop_back();
}

Map::Result Map::read_blocks(const char *f) {
	bool too_long = false;

	FILE *file = fl_fopen(f, "rb");
	if (file == NULL) { return (_result = MAP_BAD_FILE); } // cannot load file

	uint8_t *data = new uint8_t[size() + 1];
	size_t c = fread(data, 1, size() + 1, file);
	fclose(file);
	if (c < size()) { delete [] data; return (_result = MAP_TOO_SHORT); } // too-short blk
	if (c == size() + 1) { too_long = true; }

	for (uint8_t y = 0; y < (size_t)_height; y++) {
		for (uint8_t x = 0; x < (size_t)_width; x++) {
			size_t i = (size_t)y * _width + (size_t)x;
			uint8_t id = data[i];
			_blocks[i] = new Block(y, x, id);
		}
	}

	delete [] data;
	return (_result = too_long ? MAP_TOO_LONG : MAP_OK);
}

const char *Map::error_message(Result result) {
	switch (result) {
	case MAP_OK:
		return "OK.";
	case MAP_BAD_FILE:
		return "Cannot open file.";
	case MAP_TOO_SHORT:
		return "File ends too early.";
	case MAP_TOO_LONG:
		return "The .blk file is larger than the specified size.";
	case MAP_NULL:
		return "No *.blk file chosen.";
	default:
		return "Unspecified error.";
	}
}
