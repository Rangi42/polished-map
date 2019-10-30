#ifndef ROOF_WINDOW_H
#define ROOF_WINDOW_H

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
#include "tileset-window.h"

class Roof_Tile_Window : public Fl_Double_Window {
public:
	Roof_Tile_Window(int x, int y, int w, int h, const char *l = NULL);
	int handle(int event);
};

class Roof_Window {
private:
	int _dx, _dy;
	Tileset *_tileset;
	bool _canceled;
	Roof_Tile_Window *_window;
	Label *_roof_heading, *_tile_heading;
	Fl_Group *_roof_group, *_tile_group;
	Deep_Tile_Button *_deep_tile_buttons[NUM_ROOF_TILES], *_selected;
	Pixel_Button *_pixels[TILE_SIZE * TILE_SIZE];
	Toolbar_Button *_copy_tb, *_paste_tb;
	Swatch *_swatch1, *_swatch2, *_swatch3, *_swatch4, *_chosen;
	Default_Button *_ok_button;
	OS_Button *_cancel_button;
	friend class Roof_Tile_Window;
	bool _copied;
	Tile _clipboard;
	bool _debounce;
public:
	Roof_Window(int x, int y);
	~Roof_Window();
private:
	void initialize(void);
	void refresh(void);
public:
	void update_icons(void);
	void tileset(Tileset *t);
	inline bool canceled(void) const { return _canceled; }
	inline void canceled(bool c) { _canceled = c; }
	void show(const Fl_Widget *p);
	void apply_modifications(void);
	void select(Deep_Tile_Button *dtb);
	void choose(Swatch *swatch);
	void flood_fill(Pixel_Button *pb, Hue f, Hue t);
	void substitute_hue(Hue f, Hue t);
private:
	static void close_cb(Fl_Widget *w, Roof_Window *rw);
	static void cancel_cb(Fl_Widget *w, Roof_Window *rw);
	static void select_tile_cb(Deep_Tile_Button *dtb, Roof_Window *rw);
	static void choose_swatch_cb(Swatch *s, Roof_Window *rw);
	static void change_pixel_cb(Pixel_Button *pb, Roof_Window *rw);
	static void copy_tile_cb(Fl_Widget *w, Roof_Window *rw);
	static void paste_tile_cb(Fl_Widget *w, Roof_Window *rw);
	static void swap_tiles_cb(Fl_Widget *w, Roof_Window *rw);
	static void copy_tile_graphics_cb(Toolbar_Button *tb, Roof_Window *rw);
	static void paste_tile_graphics_cb(Toolbar_Button *tb, Roof_Window *rw);
};

#endif
