#ifndef VIZ_WINDOW_H
#define VIZ_WINDOW_H

#pragma warning(push, 0)
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Pixmap.H>
#pragma warning(pop)

#include "utils.h"
#include "widgets.h"

#define MAX_METATILES 256
#define METATILES_PER_ROW 4

class Main_Window : public Fl_Double_Window {
private:
	Fl_Menu_Bar *_menu_bar;
	Toolbar *_toolbar;
	Workspace *_sidebar;
	Workspace *_map_scroll;
	Fl_Group *_map;
	Toolbar *_status_bar;
	Toolbar_Button *_open_tb, *_save_tb, *_hex_tb, *_zoom_tb;
	Status_Bar_Field *_metatile_count, *_map_dimensions, *_hover_x, *_hover_y, *_hover_id;
	uint8_t _num_metatiles;
	Metatile *_metatiles[256];
	Metatile *_selected;
	Block **_blocks;
	uint8_t _map_w, _map_h;
	Fl_Pixmap _default_metatile_image;
	bool _show_hex_ids, _zoom;
public:
	Main_Window(int x, int y, int w, int h, const char *l = NULL);
	void show(void);
	Fl_Image *metatile_image(uint8_t id);
	bool show_hex_ids(void) const { return _show_hex_ids; }
	int metatile_size(void) const { return _zoom ? 64 : 32; }
	void update_status(Block *b);
	void flood_fill(Block *b, uint8_t f, uint8_t t);
private:
	void toggle_zoom(void);
	static void open_cb(Fl_Widget *w, Main_Window *mw);
	static void close_cb(Fl_Widget *w, Main_Window *mw);
	static void save_cb(Fl_Widget *w, Main_Window *mw);
	static void exit_cb(Fl_Widget *w, void *v);
	static void hex_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void zoom_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void select_metatile_cb(Metatile *mt, Main_Window *mw);
	static void change_block_cb(Block *b, Main_Window *mw);
};

#endif
