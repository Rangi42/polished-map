#include <string>
#include <sstream>

#ifndef EVENTS_H
#define EVENTS_H

#pragma warning(push, 0)
#include <FL/Fl_Box.H>
#pragma warning(pop)

#include "utils.h"

#define MAP_MARGIN 3
#define EVENT_MARGIN (MAP_MARGIN * 2)

enum EventTexture { TX_RED, TX_GREEN, TX_BLUE, TX_PURPLE, TX_ORANGE, TX_AZURE };

#define NUM_EVENT_TEXTURES 6

struct Event_Meta {
public:
	EventTexture texture;
	char symbol;
	uint8_t skip;
	bool yx;
	int8_t warp;
};

class Event : public Fl_Box {
private:
	size_t _line;
	Event_Meta _meta;
	int8_t _event_x, _event_y;
	std::string _prelude, _macro, _prefix, _suffix, _tip;
	bool _prefixed, _suffixed, _hex_coords;
	friend class Event_Options_Dialog;
public:
	Event(size_t line, const std::string &prelude, const std::string &macro, Event_Meta meta, const std::string &tip_);
	inline size_t line(void) const { return _line; }
	inline std::string prelude(void) const { return _prelude; }
	inline int8_t event_x(void) const { return _event_x; }
	inline int8_t event_y(void) const { return _event_y; }
	inline void coords(int8_t event_x, int8_t event_y) { _event_x = event_x; _event_y = event_y; }
	inline void reposition(int dx, int dy) {
		position(dx + ((int)_event_x + EVENT_MARGIN) * w(), dy + ((int)_event_y + EVENT_MARGIN) * h());
	}
	inline std::string tip(void) const { return _tip; }
	inline void tip(const std::string &l) { _tip = l; tooltip(_tip.c_str()); }
	bool warp_map_name(char *name) const;
	void parse(std::istringstream &lss);
	void update_tooltip(void);
	void draw(void);
	void print(void);
	int handle(int event);
};

#endif
