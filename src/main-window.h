#ifndef VIZ_WINDOW_H
#define VIZ_WINDOW_H

#include <unordered_map>

#pragma warning(push, 0)
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Native_File_Chooser.H>
#pragma warning(pop)

#include "utils.h"
#include "config.h"
#include "widgets.h"
#include "modal-dialog.h"
#include "option-dialogs.h"
#include "metatileset.h"
#include "map.h"
#include "help-window.h"
#include "block-window.h"
#include "tileset-window.h"
#include "directory-chooser.h"

#define METATILES_PER_ROW 4
#define METATILE_PX_SIZE (TILE_SIZE * METATILE_SIZE)

class Main_Window : public Fl_Double_Window {
private:
	// GUI containers
	Fl_Menu_Bar *_menu_bar;
	Toolbar *_toolbar;
	Workspace *_sidebar, *_map_scroll;
	Toolbar *_status_bar;
	Fl_Group *_map_group;
	// GUI inputs
	DnD_Receiver *_dnd_receiver;
	Fl_Menu_Item *_aero_theme_mi, *_metro_theme_mi, *_greybird_theme_mi, *_blue_theme_mi, *_dark_theme_mi;
	Fl_Menu_Item *_day_mi, *_night_mi, *_indoor_mi, *_ice_path_mi, *_artificial_mi;
	Fl_Menu_Item *_grid_mi, *_zoom_mi, *_ids_mi, *_hex_mi, *_event_cursor_mi, *_full_screen_mi;
	Fl_Menu_Item *_monochrome_mi, *_skip_tiles_60_to_7f_mi, *_tile_priority_mi;
	Toolbar_Button *_new_tb, *_open_tb, *_save_tb, *_print_tb, *_undo_tb, *_redo_tb, *_add_sub_tb, *_resize_tb,
		*_change_tileset_tb, *_edit_tileset_tb;
	Toolbar_Toggle_Button *_grid_tb, *_zoom_tb, *_ids_tb, *_hex_tb, *_event_cursor_tb;
	Dropdown *_lighting;
	// GUI outputs
	Status_Bar_Field *_metatile_count, *_map_dimensions, *_hover_id, *_hover_xy, *_hover_event;
	// Dialogs
	Directory_Chooser *_new_dir_chooser;
	Fl_Native_File_Chooser *_blk_open_chooser, *_blk_save_chooser, *_png_chooser;
	Modal_Dialog *_error_dialog, *_warning_dialog, *_success_dialog, *_unsaved_dialog, *_about_dialog;
	Map_Options_Dialog *_map_options_dialog;
	Tileset_Options_Dialog *_tileset_options_dialog;
	Resize_Dialog *_resize_dialog;
	Add_Sub_Dialog *_add_sub_dialog;
	Help_Window *_help_window;
	Block_Window *_block_window;
	Tileset_Window *_tileset_window;
	// Data
	std::string _directory, _blk_file;
	Metatileset _metatileset;
	Map _map;
	// Metatile button properties
	Metatile_Button *_metatile_buttons[MAX_NUM_METATILES];
	Metatile_Button *_selected;
	// Work properties
	bool _unsaved, _copied;
	Metatile _clipboard;
	std::unordered_map<int, uint8_t> _hotkey_metatiles;
	std::unordered_map<uint8_t, int> _metatile_hotkeys;
	// Window size cache
	int _wx, _wy, _ww, _wh;
#ifndef _WIN32
	// Window icons
	Pixmap _icon_pixmap, _icon_mask;
#endif
public:
	Main_Window(int x, int y, int w, int h, const char *l = NULL);
	~Main_Window();
	void show(void);
	inline bool grid(void) const { return _grid_mi && !!_grid_mi->value(); }
	inline bool zoom(void) const { return _zoom_mi && !!_zoom_mi->value(); }
	inline bool ids(void) const { return _ids_mi && !!_ids_mi->value(); }
	inline bool hex(void) const { return _hex_mi && !!_hex_mi->value(); }
	inline bool event_cursor(void) const { return _event_cursor_mi && !!_event_cursor_mi->value(); }
	inline Lighting lighting(void) const { return (Lighting)_lighting->value(); }
	inline bool monochrome(void) const { return _monochrome_mi && !!_monochrome_mi->value(); }
	inline bool skip_tiles_60_to_7f(void) const { return _skip_tiles_60_to_7f_mi && !!_skip_tiles_60_to_7f_mi->value(); }
	inline bool tile_priority(void) const { return _tile_priority_mi && !!_tile_priority_mi->value(); }
	inline int metatile_size(void) const { return METATILE_PX_SIZE * (zoom() ? ZOOM_FACTOR : 1); }
	inline bool unsaved(void) const { return _map.modified() || _metatileset.modified() || _metatileset.const_tileset()->modified(); }
	inline std::unordered_map<uint8_t, int>::const_iterator metatile_hotkey(uint8_t id) const { return _metatile_hotkeys.find(id); }
	inline std::unordered_map<uint8_t, int>::const_iterator no_hotkey(void) const { return _metatile_hotkeys.end(); }
	inline std::unordered_map<int, uint8_t>::const_iterator hotkey_metatile(int key) const { return _hotkey_metatiles.find(key); }
	inline std::unordered_map<int, uint8_t>::const_iterator no_metatile(void) const { return _hotkey_metatiles.end(); }
	const char *modified_filename(void);
	int handle(int event);
	void draw_metatile(int x, int y, uint8_t id) const;
	void update_status(Block *b);
	void update_event_cursor(Block *b);
	void flood_fill(Block *b, uint8_t f, uint8_t t);
	void substitute_block(uint8_t f, uint8_t t);
	void open_map(const char *filename);
private:
	int handle_hotkey(int key);
	void open_map(const char *directory, const char *filename);
	bool read_metatile_data(const char *tileset_name);
	void add_sub_metatiles(size_t n);
	void force_add_sub_metatiles(size_t s, size_t n);
	void resize_map(int w, int h);
	bool save_map(bool force);
	bool save_metatileset(void);
	bool save_tileset(void);
	void edit_metatile(Metatile *mt);
	void update_zoom(void);
	void update_labels(void);
	void update_lighting(void);
	void select_metatile(Metatile_Button *mb);
	// Drag-and-drop
	static void drag_and_drop_cb(DnD_Receiver *dndr, Main_Window *mw);
	// File menu
	static void new_cb(Fl_Widget *w, Main_Window *mw);
	static void open_cb(Fl_Widget *w, Main_Window *mw);
	static void save_cb(Fl_Widget *w, Main_Window *mw);
	static void save_as_cb(Fl_Widget *w, Main_Window *mw);
	static void save_metatiles_cb(Fl_Widget *w, Main_Window *mw);
	static void save_tileset_cb(Fl_Widget *w, Main_Window *mw);
	static void close_cb(Fl_Widget *w, Main_Window *mw);
	static void print_cb(Fl_Widget *w, Main_Window *mw);
	static void exit_cb(Fl_Widget *w, Main_Window *mw);
	// Edit menu
	static void undo_cb(Fl_Widget *w, Main_Window *mw);
	static void redo_cb(Fl_Widget *w, Main_Window *mw);
	static void copy_metatile_cb(Fl_Widget *w, Main_Window *mw);
	static void paste_metatile_cb(Fl_Widget *w, Main_Window *mw);
	static void swap_metatiles_cb(Fl_Widget *w, Main_Window *mw);
	static void add_sub_cb(Fl_Widget *w, Main_Window *mw);
	static void resize_cb(Fl_Widget *w, Main_Window *mw);
	static void change_tileset_cb(Fl_Widget *w, Main_Window *mw);
	static void edit_tileset_cb(Fl_Widget *w, Main_Window *mw);
	// View menu
	static void aero_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void metro_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void greybird_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void blue_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void dark_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void grid_cb(Fl_Menu_ *m, Main_Window *mw);
	static void zoom_cb(Fl_Menu_ *m, Main_Window *mw);
	static void ids_cb(Fl_Menu_ *m, Main_Window *mw);
	static void hex_cb(Fl_Menu_ *m, Main_Window *mw);
	static void event_cursor_cb(Fl_Menu_ *m, Main_Window *mw);
	static void day_lighting_cb(Fl_Menu_ *m, Main_Window *mw);
	static void night_lighting_cb(Fl_Menu_ *m, Main_Window *mw);
	static void indoor_lighting_cb(Fl_Menu_ *m, Main_Window *mw);
	static void ice_path_lighting_cb(Fl_Menu_ *m, Main_Window *mw);
	static void artificial_lighting_cb(Fl_Menu_ *m, Main_Window *mw);
	static void full_screen_cb(Fl_Menu_ *m, Main_Window *mw);
	// Options menu
	static void monochrome_cb(Fl_Menu_ *m, Main_Window *mw);
	static void skip_tiles_60_to_7f_cb(Fl_Menu_ *m, Main_Window *mw);
	static void tile_priority_cb(Fl_Menu_ *m, Main_Window *mw);
	// Toolbar buttons
	static void grid_tb_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void zoom_tb_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void ids_tb_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void hex_tb_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void event_cursor_tb_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void lighting_cb(Dropdown *dd, Main_Window *mw);
	// Help menu
	static void help_cb(Fl_Widget *w, Main_Window *mw);
	static void about_cb(Fl_Widget *w, Main_Window *mw);
	// Metatiles sidebar
	static void select_metatile_cb(Metatile_Button *mb, Main_Window *mw);
	// Map
	static void change_block_cb(Block *b, Main_Window *mw);
};

#endif
