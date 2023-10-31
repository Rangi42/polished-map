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
#define MIN_EVENT_COORD ((int16_t)-EVENT_MARGIN)
#define MAX_EVENT_COORD ((int16_t)(UINT8_MAX - EVENT_MARGIN))

enum class EventTexture { TX_RED, TX_GREEN, TX_BLUE, TX_PURPLE, TX_ORANGE, TX_AZURE };

#define NUM_EVENT_TEXTURES 6

struct Event_Meta {
public:
	EventTexture texture;
	char symbol;
	uint8_t skip;
	bool yx, id_map;
public:
	bool is_warp(void) const { return texture == EventTexture::TX_PURPLE; }
};

class Event : public Fl_Box {
private:
	size_t _line;
	Event_Meta _meta;
	int16_t _event_x, _event_y;
	std::string _prelude, _macro, _prefix, _suffix, _tip;
	int _warp_id;
	Event *_warp_to;
	bool _prefixed, _suffixed, _hex_coords;
	friend class Event_Options_Dialog;
public:
	Event(size_t line, const std::string &prelude, const std::string &macro, Event_Meta meta, const std::string &tip_, int warp_id);
	inline size_t line(void) const { return _line; }
	inline std::string prelude(void) const { return _prelude; }
	inline int16_t event_x(void) const { return _event_x; }
	inline int16_t event_y(void) const { return _event_y; }
	inline void coords(int16_t event_x, int16_t event_y) { _event_x = event_x; _event_y = event_y; }
	inline void reposition(int dx, int dy) {
		position(dx + ((int)_event_x + EVENT_MARGIN) * w(), dy + ((int)_event_y + EVENT_MARGIN) * h());
	}
	inline std::string tip(void) const { return _tip; }
	inline void tip(const std::string &l) { _tip = l; tooltip(_tip.c_str()); }
	inline Event *warp_to(void) const { return _warp_to; }
	inline void warp_to(Event *w) { _warp_to = w; }
	std::pair<std::string, int> warp_destination(void) const;
	void parse(std::istringstream &lss);
	void update_tooltip(void);
	void draw(void);
	void print(bool print_warp_ids) const;
	int handle(int event);
private:
	void draw_warp_id(int x, int y) const;
	void draw_warp_id_zoomed(int x, int y) const;
};

#endif
