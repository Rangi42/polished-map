#ifndef TILESET_WINDOW_H
#define TILESET_WINDOW_H

#include <string>

#pragma warning(push, 0)
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#pragma warning(pop)

#include "tileset.h"
#include "map-buttons.h"
#include "metatile.h"
#include "widgets.h"
#include "block-window.h"

#define PIXEL_ZOOM_FACTOR 17

class Tile_Window : public Fl_Double_Window {
public:
	Tile_Window(int x, int y, int w, int h, const char *l = NULL);
	int handle(int event);
};

class Tileset_Window {
private:
	int _dx, _dy;
	Tileset *_tileset;
	bool _canceled;
	Tile_Window *_window;
	Label *_tileset_heading, *_tile_heading;
	Fl_Group *_tileset_group, *_tile_group;
	Deep_Tile_Button *_deep_tile_buttons[MAX_NUM_TILES], *_selected;
	Pixel_Button *_pixels[TILE_AREA];
	Swatch *_swatch1, *_swatch2, *_swatch3, *_swatch4, *_chosen;
	Default_Button *_ok_button;
	OS_Button *_cancel_button;
	friend class Tile_Window;
	bool _copied;
	Tile _clipboard;
public:
	Tileset_Window(int x, int y);
	~Tileset_Window();
private:
	void initialize(void);
	void refresh(void);
public:
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
	static void close_cb(Fl_Widget *w, Tileset_Window *tw);
	static void cancel_cb(Fl_Widget *w, Tileset_Window *tw);
	static void select_tile_cb(Deep_Tile_Button *dtb, Tileset_Window *tw);
	static void choose_swatch_cb(Swatch *s, Tileset_Window *tw);
	static void change_pixel_cb(Pixel_Button *pb, Tileset_Window *tw);
	static void copy_tile_cb(Fl_Widget *w, Tileset_Window *tw);
	static void paste_tile_cb(Fl_Widget *w, Tileset_Window *tw);
	static void swap_tiles_cb(Fl_Widget *w, Tileset_Window *tw);
	static void delete_tile_cb(Fl_Widget *w, Tileset_Window *tw);
};

#endif
