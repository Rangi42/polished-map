#ifndef MAP_BUTTONS_H
#define MAP_BUTTONS_H

#pragma warning(push, 0)
#include <FL/Fl_Radio_Button.H>
#include <FL/Fl_Box.H>
#pragma warning(pop)

#include "utils.h"
#include "widgets.h"
#include "tile.h"
#include "palette-map.h"

class Metatile_Button : public Fl_Radio_Button {
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
	Block(int x = 0, int y = 0, int s = 0, uint8_t row = 0, uint8_t col = 0, uint8_t id = 0);
	inline uint8_t row(void) const { return _row; }
	inline uint8_t col(void) const { return _col; }
	inline void coords(uint8_t row, uint8_t col) { _row = row; _col = col; }
	inline uint8_t id(void) const { return _id; }
	inline void id(uint8_t id) { _id = id; update_label(); }
	inline bool right_half(void) const { return Fl::event_x() >= x() + w() / 2; }
	inline bool bottom_half(void) const { return Fl::event_y() >= y() + h() / 2; }
	void update_label(void);
	void draw(void);
	int handle(int event);
};

class Tile_Button : public Fl_Radio_Button {
private:
	uint8_t _id;
public:
	Tile_Button(int x, int y, int s, uint8_t id);
	inline uint8_t id(void) const { return _id; }
	inline void id(uint8_t id) { _id = id; }
	void draw(void);
};

class Chip : public Fl_Box {
private:
	uint8_t _row, _col;
	uint8_t _id;
public:
	Chip(int x = 0, int y = 0, int s = 0, uint8_t row = 0, uint8_t col = 0, uint8_t id = 0);
	inline uint8_t row(void) const { return _row; }
	inline uint8_t col(void) const { return _col; }
	inline void coords(uint8_t row, uint8_t col) { _row = row; _col = col; }
	inline uint8_t id(void) const { return _id; }
	inline void id(uint8_t id) { _id = id; }
	void draw(void);
	int handle(int event);
};

class Deep_Tile_Button : public Fl_Radio_Button {
private:
	uint8_t _id;
	Palette _palette;
	Hue _hues[TILE_SIZE * TILE_SIZE];
	uchar _rgb[LINE_PX * LINE_PX * NUM_CHANNELS];
public:
	Deep_Tile_Button(int x, int y, int s, uint8_t id);
	inline uint8_t id(void) const { return _id; }
	inline void id(uint8_t id) { _id = id; }
	void copy_tile(const Tile *t);
	void draw(void);
};

class Pixel : public Fl_Box {
private:
	Hue _hue;
	Palette _palette;
	Fl_Color _color;
public:
	Pixel(int x = 0, int y = 0, int s = 0);
	inline Hue hue(void) const { return _hue; }
	inline void hue(Hue hue) { _hue = hue; }
	void draw(void);
	int handle(int event);
};

class Swatch : public Fl_Radio_Button {
private:
	Hue _hue;
	Palette _palette;
	Fl_Color _color;
public:
	Swatch(int x, int y, int s, const char *l = NULL);
	void draw(void);
	int handle(int event);
};

#endif
