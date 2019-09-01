#ifndef MAP_BUTTONS_H
#define MAP_BUTTONS_H

#pragma warning(push, 0)
#include <FL/Fl_Radio_Button.H>
#include <FL/Fl_Box.H>
#pragma warning(pop)

#include "utils.h"
#include "widgets.h"
#include "tile.h"
#include "attributable.h"

class Metatile_Button : public Fl_Radio_Button {
public:
	static Metatile_Button *dragging;
private:
	uint8_t _id;
public:
	Metatile_Button(int x, int y, int s, uint8_t id);
	inline uint8_t id(void) const { return _id; }
	void id(uint8_t id);
	void draw(void);
	int handle(int event);
};

class Block : public Fl_Box {
private:
	uint8_t _row, _col;
	uint8_t _id;
public:
	Block(uint8_t row = 0, uint8_t col = 0, uint8_t id = 0);
	inline uint8_t row(void) const { return _row; }
	inline uint8_t col(void) const { return _col; }
	inline void coords(uint8_t row, uint8_t col) { _row = row; _col = col; }
	inline uint8_t id(void) const { return _id; }
	inline void id(uint8_t id) { _id = id; update_label(); }
	inline bool right_half(void) const { return Fl::event_x() >= x() + w() / 2; }
	inline bool bottom_half(void) const { return Fl::event_y() >= y() + h() / 2; }
	void update_label(void);
	void draw(void);
	void print(void);
	int handle(int event);
};

class Tile_Button : public Fl_Radio_Button, public Attributable {
public:
	Tile_Button(int x, int y, int s, uint8_t id);
	void draw(void);
};

class Chip : public Fl_Box, public Attributable {
private:
	uint8_t _row, _col;
public:
	Chip(int x, int y, int s, uint8_t row, uint8_t col);
	inline uint8_t row(void) const { return _row; }
	inline uint8_t col(void) const { return _col; }
	inline void coords(uint8_t row, uint8_t col) { _row = row; _col = col; }
	void draw(void);
	int handle(int event);
};

class Pixel_Button : public Fl_Box {
private:
	int _x, _y;
	Hue _hue;
public:
	Pixel_Button(int x = 0, int y = 0, int s = 0);
	inline int row(void) const { return _y; }
	inline int col(void) const { return _x; }
	inline void coords(int x, int y) { _x = x; _y = y; }
	inline Hue hue(void) const { return _hue; }
	inline void hue(Hue h) { _hue = h; coloring(); }
	void coloring(void);
	void draw(void);
	int handle(int event);
};

class Swatch : public Fl_Radio_Button {
private:
	Hue _hue;
public:
	Swatch(int x, int y, int s, const char *l = NULL);
	inline Hue hue(void) const { return _hue; }
	void hue(Hue h) { _hue = h; coloring(); }
	void coloring(void);
	void draw(void);
};

class Deep_Tile_Button : public Fl_Radio_Button, public Tile {
private:
	static Deep_Tile_Button *_dragging;
	bool _for_clipboard;
public:
	Deep_Tile_Button(int x, int y, int s, uint8_t id);
	inline bool for_clipboard(void) const { return _for_clipboard; }
	inline void for_clipboard(bool f) { _for_clipboard = f; }
	inline void copy_pixel(const Pixel_Button *pb, Lighting l) { render_pixel(pb->col(), pb->row(), l, pb->hue()); }
	void copy_pixels(Pixel_Button **pbs, Lighting l);
	void draw(void);
	int handle(int event);
private:
	void drag_to_swap_or_copy(void);
};

class Color_Button : public Fl_Radio_Button {
private:
	static Color_Button *_dragging;
public:
	Color_Button(int x, int y, int s, const char *l = NULL);
	void draw(void);
	int handle(int event);
private:
	void drag_to_swap_or_copy(void);
};

#endif
