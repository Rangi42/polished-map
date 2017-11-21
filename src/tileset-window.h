#ifndef TILESET_WINDOW_H
#define TILESET_WINDOW_H

#include <string>

#pragma warning(push, 0)
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#pragma warning(pop)

#include "tileset.h"
#include "map-buttons.h"
#include "palette-map.h"
#include "metatile.h"
#include "widgets.h"
#include "block-window.h"

#define PIXEL_ZOOM_FACTOR 18
#define ZOOMED_TILE_PX_SIZE (TILE_SIZE * PIXEL_ZOOM_FACTOR)

class Tileset_Window {
private:
	int _dx, _dy;
	const Tileset *_tileset;
	bool _canceled;
	Fl_Double_Window *_window;
	Label *_tileset_heading, *_tile_heading;
	Fl_Group *_tileset_group, *_tile_group;
	Deep_Tile_Button *_deep_tile_buttons[MAX_NUM_TILES], *_selected;
	Pixel *_pixels[TILE_SIZE * TILE_SIZE];
	Swatch *_color1, *_color2, *_color3, *_color4;
	Dropdown *_hue;
	OS_Check_Button *_priority;
	Default_Button *_ok_button;
	OS_Button *_cancel_button;
public:
	Tileset_Window(int x, int y);
	~Tileset_Window();
private:
	void initialize(void);
	void refresh(void);
public:
	void tileset(const Tileset *t);
	inline bool canceled(void) const { return _canceled; }
	inline void canceled(bool c) { _canceled = c; }
	void show(const Fl_Widget *p);
private:
	static void close_cb(Fl_Widget *w, Tileset_Window *bw);
	static void cancel_cb(Fl_Widget *w, Tileset_Window *bw);
	static void select_tile_cb(Deep_Tile_Button *dtb, Tileset_Window *tw);
	static void change_pixel_cb(Pixel *pxl, Tileset_Window *tw);
};

#endif
