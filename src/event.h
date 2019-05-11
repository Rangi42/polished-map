#include <string>

#ifndef EVENTS_H
#define EVENTS_H

#pragma warning(push, 0)
#include <FL/Fl_Box.H>
#pragma warning(pop)

#include "utils.h"

enum EventTexture { TX_RED, TX_GREEN, TX_BLUE, TX_PURPLE, TX_ORANGE, TX_AZURE };

#define NUM_EVENT_TEXTURES 6

class Event : public Fl_Box {
private:
	uint8_t _event_x, _event_y;
	char _symbol;
	EventTexture _texture;
	std::string _line;
public:
	Event(int8_t event_x = 0, int8_t event_y = 0, char s = '?', EventTexture t = TX_RED, std::string l = "?");
	inline uint8_t event_x(void) const { return _event_x; }
	inline uint8_t event_y(void) const { return _event_y; }
	inline void coords(uint8_t event_x, uint8_t event_y) { _event_x = event_x; _event_y = event_y; }
	inline char symbol(void) const { return _symbol; }
	inline void symbol(char s) { _symbol = s; }
	inline EventTexture texture(void) const { return _texture; }
	inline void texture(EventTexture t) { _texture = t; }
	inline std::string line(void) const { return _line; }
	inline void line(std::string l) { _line = l; tooltip(_line.c_str()); }
	void draw(void);
	int handle(int event);
};

#endif
