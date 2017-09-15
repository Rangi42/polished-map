#ifndef MAP_BUTTONS_H
#define MAP_BUTTONS_H

#pragma warning(push, 0)
#include <FL/Fl_Radio_Button.H>
#include <FL/Fl_Box.H>
#pragma warning(pop)

#include "utils.h"
#include "widgets.h"

#define GRID_COLOR FL_INACTIVE_COLOR

class Metatile_Button : public Fl_Radio_Button {
private:
	uint8_t _id;
public:
	Metatile_Button(int x, int y, int s, uint8_t id);
	uint8_t id(void) const { return _id; }
	void id(uint8_t id);
	void draw(void);
};

class Block : public Fl_Box {
private:
	uint8_t _row, _col;
	uint8_t _id;
public:
	Block(int x = 0, int y = 0, int s = 0, uint8_t row = 0, uint8_t col = 0, uint8_t id = 0);
	uint8_t row(void) const { return _row; }
	uint8_t col(void) const { return _col; }
	void coords(uint8_t row, uint8_t col) { _row = row; _col = col; }
	uint8_t id(void) const { return _id; }
	void id(uint8_t id) { _id = id; update_label(); }
	void update_label(void);
	void draw(void);
	int handle(int event);
};

#endif
