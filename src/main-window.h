#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <unordered_map>
#include <climits>

#pragma warning(push, 0)
#include <FL/Fl_Overlay_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Native_File_Chooser.H>
#pragma warning(pop)

#include "utils.h"
#include "config.h"
#include "widgets.h"
#include "modal-dialog.h"
#include "option-dialogs.h"
#include "ruler.h"
#include "metatileset.h"
#include "map.h"
#include "map-events.h"
#include "gameboy-screen.h"
#include "help-window.h"
#include "block-window.h"
#include "tileset-window.h"
#include "roof-window.h"
#include "palette-window.h"
#include "directory-chooser.h"

#define METATILES_PER_ROW 4

#define NUM_RECENT 10

enum class Mode { BLOCKS, EVENTS };

class Main_Window : public Fl_Overlay_Window {
private:
	// GUI containers
	Fl_Menu_Bar *_menu_bar;
	Toolbar *_toolbar;
	Workspace *_sidebar, *_map_scroll;
	Toolbar *_status_bar;
	Fl_Group *_map_group;
	// GUI inputs
	DnD_Receiver *_dnd_receiver;
	Fl_Menu_Item *_recent_mis[NUM_RECENT];
	Fl_Menu_Item *_classic_theme_mi = NULL, *_aero_theme_mi = NULL, *_metro_theme_mi = NULL, *_aqua_theme_mi = NULL,
		*_greybird_theme_mi = NULL, *_ocean_theme_mi = NULL, *_blue_theme_mi = NULL, *_olive_theme_mi = NULL,
		*_rose_gold_theme_mi = NULL, *_dark_theme_mi = NULL, *_brushed_metal_theme_mi = NULL, *_high_contrast_theme_mi = NULL;
	Fl_Menu_Item *_grid_mi = NULL, *_rulers_mi, *_zoom_mi = NULL, *_ids_mi = NULL, *_hex_mi = NULL,
		*_show_priority_mi = NULL, *_gameboy_screen_mi = NULL, *_show_events_mi = NULL, *_full_screen_mi = NULL;
	Fl_Menu_Item *_morn_mi = NULL, *_day_mi = NULL, *_night_mi = NULL, *_darkness_mi = NULL, *_indoor_mi = NULL,
		*_custom_mi = NULL;
	Fl_Menu_Item *_blocks_mode_mi = NULL, *_events_mode_mi = NULL;
	Fl_Menu_Item *_monochrome_mi = NULL, *_allow_priority_mi = NULL, *_allow_256_tiles_mi = NULL, *_auto_events_mi = NULL,
		*_special_palettes_mi = NULL, *_roof_colors_mi = NULL, *_drag_and_drop_mi = NULL, *_overworld_map_size_mi = NULL;
	Fl_Menu_Item *_roof_custom_mi = NULL, *_roof_day_nite_mi = NULL, *_roof_morn_day_nite_mi = NULL,
		*_roof_day_nite_custom_mi = NULL, *_roof_morn_day_nite_custom_mi = NULL;
	Toolbar_Button *_new_tb, *_open_tb, *_load_event_script_tb, *_reload_event_script_tb, *_save_tb, *_print_tb,
		*_undo_tb, *_redo_tb, *_add_sub_tb, *_resize_tb, *_change_tileset_tb, *_change_roof_tb, *_edit_tileset_tb,
		*_edit_roof_tb, *_load_palettes_tb, *_edit_current_palettes_tb;
	Toolbar_Toggle_Button *_grid_tb, *_rulers_tb, *_zoom_tb, *_ids_tb, *_hex_tb, *_show_events_tb, *_show_priority_tb,
		*_gameboy_screen_tb;
	Toolbar_Radio_Button *_blocks_mode_tb, *_events_mode_tb;
	Dropdown *_palettes;
	// GUI outputs
	Ruler *_hor_ruler, *_ver_ruler, *_corner_ruler;
	Status_Bar_Field *_metatile_count, *_map_dimensions, *_hover_id, *_hover_xy, *_hover_event;
	// Conditional menu items
	Fl_Menu_Item *_load_event_script_mi = NULL, *_view_event_script_mi, *_reload_event_script_mi = NULL,
		*_unload_event_script_mi = NULL, *_load_roof_colors_mi = NULL, *_close_mi = NULL, *_save_mi = NULL,
		*_save_as_mi = NULL, *_save_map_mi = NULL, *_save_blockset_mi = NULL, *_save_tileset_mi = NULL,
		*_save_roof_mi = NULL, *_save_event_script_mi = NULL, *_print_mi = NULL;
	Fl_Menu_Item *_undo_mi = NULL, *_redo_mi = NULL, *_copy_block_mi = NULL, *_paste_block_mi = NULL, *_swap_block_mi = NULL;
	Fl_Menu_Item *_resize_blockset_mi = NULL, *_resize_map_mi = NULL, *_change_tileset_mi = NULL, *_change_roof_mi = NULL,
		*_edit_tileset_mi = NULL, *_edit_roof_mi = NULL;
	// Dialogs
	Directory_Chooser *_new_dir_chooser;
	Fl_Native_File_Chooser *_blk_open_chooser, *_blk_save_chooser, *_pal_load_chooser, *_pal_save_chooser, *_roof_chooser,
		*_asm_chooser, *_png_chooser;
	Modal_Dialog *_error_dialog, *_warning_dialog, *_success_dialog, *_unsaved_dialog, *_about_dialog;
	Map_Options_Dialog *_map_options_dialog;
	Tileset_Options_Dialog *_tileset_options_dialog;
	Roof_Options_Dialog *_roof_options_dialog;
	Event_Options_Dialog *_event_options_dialog;
	Print_Options_Dialog *_print_options_dialog;
	Resize_Dialog *_resize_dialog;
	Add_Sub_Dialog *_add_sub_dialog;
	Overworld_Map_Size_Dialog *_overworld_map_size_dialog;
	Help_Window *_help_window;
	Block_Window *_block_window;
	Tileset_Window *_tileset_window;
	Roof_Window *_roof_window;
	Palette_Window *_palette_window;
	Monochrome_Palette_Window *_monochrome_palette_window;
	// Data
	std::string _directory, _blk_file, _asm_file;
	std::string _recent[NUM_RECENT];
	Metatileset _metatileset;
	Map _map;
	Map_Events _map_events;
	bool _gameboy_screen;
	int _status_event_x, _status_event_y;
	// Metatile button properties
	Metatile_Button *_metatile_buttons[MAX_NUM_METATILES];
	Metatile_Button *_selected = NULL;
	// Work properties
	Mode _mode = Mode::BLOCKS;
	Roof_Palettes _roof_palettes = Roof_Palettes::ROOF_DAY_NITE;
	bool _has_collisions = false, _edited_palettes = false, _copied = false, _map_editable = false;
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
	bool maximized(void) const;
	void maximize(void);
	inline bool grid(void) const { return _grid_mi && !!_grid_mi->value(); }
	inline bool rulers(void) const { return _rulers_mi && !!_rulers_mi->value(); }
	inline bool zoom(void) const { return _zoom_mi && !!_zoom_mi->value(); }
	inline bool ids(void) const { return _ids_mi && !!_ids_mi->value(); }
	inline bool hex(void) const { return _hex_mi && !!_hex_mi->value(); }
	inline bool show_priority(void) const { return _show_priority_mi && !!_show_priority_mi->value(); }
	inline bool gameboy_screen(void) const { return _gameboy_screen_mi && !!_gameboy_screen_mi->value(); }
	inline bool show_events(void) const { return _show_events_mi && !!_show_events_mi->value(); }
	inline bool full_screen(void) const { return _full_screen_mi && !!_full_screen_mi->value(); }
	inline Palettes palettes(void) const { return (Palettes)_palettes->value(); }
	inline Mode mode(void) const { return _mode; }
	inline bool monochrome(void) const { return _monochrome_mi && !!_monochrome_mi->value(); }
	inline bool allow_priority(void) const { return _allow_priority_mi && !!_allow_priority_mi->value(); }
	inline bool allow_256_tiles(void) const { return _allow_256_tiles_mi && !!_allow_256_tiles_mi->value(); }
	inline bool auto_load_events(void) const { return _auto_events_mi && !!_auto_events_mi->value(); }
	inline bool auto_load_special_palettes(void) const { return _special_palettes_mi && !!_special_palettes_mi->value(); }
	inline bool auto_load_roof_colors(void) const { return _roof_colors_mi && !!_roof_colors_mi->value(); }
	inline bool drag_and_drop(void) const { return _drag_and_drop_mi && !!_drag_and_drop_mi->value(); }
	inline int metatile_size(void) const { return METATILE_PX_SIZE * (zoom() ? ZOOM_FACTOR : 1); }
	inline int map_scroll_x(void) const { return _map_scroll->xposition(); }
	inline int map_scroll_y(void) const { return _map_scroll->yposition(); }
	inline int status_event_x(void) const { return _status_event_x; }
	inline int status_event_y(void) const { return _status_event_y; }
	inline int max_ruler_x(void) const { return _map.size() ? (int)_map.max_event_x() : INT_MAX; }
	inline int max_ruler_y(void) const { return _map.size() ? (int)_map.max_event_y() : INT_MAX; }
	bool unsaved(void) const;
	inline std::unordered_map<uint8_t, int>::const_iterator metatile_hotkey(uint8_t id) const { return _metatile_hotkeys.find(id); }
	inline std::unordered_map<uint8_t, int>::const_iterator no_hotkey(void) const { return _metatile_hotkeys.end(); }
	inline std::unordered_map<int, uint8_t>::const_iterator hotkey_metatile(int key) const { return _hotkey_metatiles.find(key); }
	inline std::unordered_map<int, uint8_t>::const_iterator no_metatile(void) const { return _hotkey_metatiles.end(); }
	inline void map_editable(bool e) { _map_editable = e; }
	const char *modified_filename(void);
	void draw_overlay(void);
	int handle(int event);
	inline void draw_metatile(int x, int y, uint8_t id) const { _metatileset.draw_metatile(x, y, id, zoom(), show_priority()); }
	inline void print_metatile(int x, int y, uint8_t id) const { _metatileset.draw_metatile(x, y, id, false, Config::print_priority()); }
	void update_status(Block *b);
	inline void update_status(Event *e) { update_status(_map.block_under(e)); }
	void update_event_cursor(Block *b);
	inline void update_event_cursor(Event *e) { update_event_cursor(_map.block_under(e)); }
	void update_gameboy_screen(Block *b);
	inline void update_gameboy_screen(Event *e) { update_gameboy_screen(_map.block_under(e)); }
	inline void update_gameboy_screen(void) { update_gameboy_screen(dynamic_cast<Block *>(Fl::belowmouse())); }
	inline void redraw_map(void) { _map_scroll->redraw(); }
	void flood_fill(Block *b, uint8_t f, uint8_t t);
	void substitute_block(uint8_t f, uint8_t t);
	void swap_blocks(uint8_t f, uint8_t t);
	void open_map(const char *filename);
private:
	inline void mode(Mode m) { _mode = m; }
	int handle_hotkey(int key);
	void update_active_controls(void);
	void update_priority_controls(void);
	void update_monochrome_controls(void);
	void store_recent_map(void);
	void update_recent_maps(void);
	void open_map(const char *directory, const char *filename);
	void open_recent(int n);
	void warp_to_map(Event *e);
	void load_events(const char *filename);
	void unload_events(void);
	void view_event_script(Event *e);
	void load_palettes(const char *filename);
	void load_roof_colors(bool quiet);
	bool read_metatile_data(const char *tileset_name, const char *roof_name);
	void add_sub_metatiles(size_t n);
	void force_add_sub_metatiles(size_t s, size_t n);
	void resize_map(int w, int h);
	bool save_map(bool force);
	bool save_metatileset(void);
	bool save_tileset(void);
	bool save_roof(void);
	bool save_event_script(void);
	bool export_palettes(const char *filename);
	void print_map(void);
	void edit_metatile(Metatile *mt);
	void update_icons(void);
	void update_rulers(void);
	void update_zoom(void);
	void update_layout(void);
	void update_labels(void);
	void update_palettes(void);
	void select_metatile(Metatile_Button *mb);
	// Drag-and-drop
	static void drag_and_drop_cb(DnD_Receiver *dndr, Main_Window *mw);
	// File menu
	static void new_cb(Fl_Widget *w, Main_Window *mw);
	static void open_cb(Fl_Widget *w, Main_Window *mw);
	static void open_recent_cb(Fl_Menu_ *m, Main_Window *mw);
	static void clear_recent_cb(Fl_Menu_ *m, Main_Window *mw);
	static void close_cb(Fl_Widget *w, Main_Window *mw);
	static void save_cb(Fl_Widget *w, Main_Window *mw);
	static void save_as_cb(Fl_Widget *w, Main_Window *mw);
	static void save_map_cb(Fl_Widget *w, Main_Window *mw);
	static void save_metatiles_cb(Fl_Widget *w, Main_Window *mw);
	static void save_tileset_cb(Fl_Widget *w, Main_Window *mw);
	static void save_roof_cb(Fl_Widget *w, Main_Window *mw);
	static void print_cb(Fl_Widget *w, Main_Window *mw);
	static void exit_cb(Fl_Widget *w, Main_Window *mw);
	// Data menu
	static void load_event_script_cb(Fl_Widget *w, Main_Window *mw);
	static void view_event_script_cb(Fl_Widget *w, Main_Window *mw);
	static void reload_event_script_cb(Fl_Widget *w, Main_Window *mw);
	static void unload_event_script_cb(Fl_Widget *w, Main_Window *mw);
	static void save_event_script_cb(Fl_Widget *w, Main_Window *mw);
	static void load_roof_colors_cb(Fl_Widget *w, Main_Window *mw);
	static void load_palettes_cb(Fl_Widget *w, Main_Window *mw);
	static void export_palettes_cb(Fl_Widget *w, Main_Window *mw);
	// Edit menu
	static void undo_cb(Fl_Widget *w, Main_Window *mw);
	static void redo_cb(Fl_Widget *w, Main_Window *mw);
	static void copy_metatile_cb(Fl_Widget *w, Main_Window *mw);
	static void paste_metatile_cb(Fl_Widget *w, Main_Window *mw);
	static void swap_metatiles_cb(Fl_Widget *w, Main_Window *mw);
	// View menu
	static void classic_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void aero_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void metro_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void aqua_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void greybird_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void ocean_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void blue_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void olive_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void rose_gold_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void dark_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void brushed_metal_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void high_contrast_theme_cb(Fl_Menu_ *m, Main_Window *mw);
	static void grid_cb(Fl_Menu_ *m, Main_Window *mw);
	static void rulers_cb(Fl_Menu_ *m, Main_Window *mw);
	static void zoom_cb(Fl_Menu_ *m, Main_Window *mw);
	static void ids_cb(Fl_Menu_ *m, Main_Window *mw);
	static void hex_cb(Fl_Menu_ *m, Main_Window *mw);
	static void show_priority_cb(Fl_Menu_ *m, Main_Window *mw);
	static void gameboy_screen_cb(Fl_Menu_ *m, Main_Window *mw);
	static void show_events_cb(Fl_Menu_ *m, Main_Window *mw);
	static void morn_palettes_cb(Fl_Menu_ *m, Main_Window *mw);
	static void day_palettes_cb(Fl_Menu_ *m, Main_Window *mw);
	static void night_palettes_cb(Fl_Menu_ *m, Main_Window *mw);
	static void darkness_palettes_cb(Fl_Menu_ *m, Main_Window *mw);
	static void indoor_palettes_cb(Fl_Menu_ *m, Main_Window *mw);
	static void custom_palettes_cb(Fl_Menu_ *m, Main_Window *mw);
	static void full_screen_cb(Fl_Menu_ *m, Main_Window *mw);
	// Mode menu
	static void blocks_mode_cb(Fl_Menu_ *m, Main_Window *mw);
	static void events_mode_cb(Fl_Menu_ *m, Main_Window *mw);
	static void switch_mode_cb(Fl_Menu_ *m, Main_Window *mw);
	// Tools menu
	static void add_sub_cb(Fl_Widget *w, Main_Window *mw);
	static void resize_cb(Fl_Widget *w, Main_Window *mw);
	static void change_tileset_cb(Fl_Widget *w, Main_Window *mw);
	static void change_roof_cb(Fl_Widget *w, Main_Window *mw);
	static void edit_tileset_cb(Fl_Widget *w, Main_Window *mw);
	static void edit_roof_cb(Fl_Widget *w, Main_Window *mw);
	static void edit_current_palettes_cb(Fl_Widget *w, Main_Window *mw);
	// Options menu
	static void monochrome_cb(Fl_Menu_ *m, Main_Window *mw);
	static void allow_priority_cb(Fl_Menu_ *m, Main_Window *mw);
	static void allow_256_tiles_cb(Fl_Menu_ *m, Main_Window *mw);
	static void roof_custom_cb(Fl_Menu_ *m, Main_Window *mw);
	static void roof_day_nite_cb(Fl_Menu_ *m, Main_Window *mw);
	static void roof_morn_day_nite_cb(Fl_Menu_ *m, Main_Window *mw);
	static void roof_day_nite_custom_cb(Fl_Menu_ *m, Main_Window *mw);
	static void roof_morn_day_nite_custom_cb(Fl_Menu_ *m, Main_Window *mw);
	static void auto_load_events_cb(Fl_Menu_ *m, Main_Window *mw);
	static void auto_load_special_palettes_cb(Fl_Menu_ *m, Main_Window *mw);
	static void auto_load_roof_colors_cb(Fl_Menu_ *m, Main_Window *mw);
	static void drag_and_drop_option_cb(Fl_Menu_ *m, Main_Window *mw);
	static void overworld_map_size_cb(Fl_Menu_ *m, Main_Window *mw);
	// Toolbar buttons
	static void grid_tb_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void rulers_tb_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void zoom_tb_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void ids_tb_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void hex_tb_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void show_priority_tb_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void gameboy_screen_tb_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void show_events_tb_cb(Toolbar_Toggle_Button *tb, Main_Window *mw);
	static void palettes_cb(Dropdown *dd, Main_Window *mw);
	static void blocks_mode_tb_cb(Toolbar_Radio_Button *tb, Main_Window *mw);
	static void events_mode_tb_cb(Toolbar_Radio_Button *tb, Main_Window *mw);
	// Help menu
	static void help_cb(Fl_Widget *w, Main_Window *mw);
	static void about_cb(Fl_Widget *w, Main_Window *mw);
	// Metatiles sidebar
	static void select_metatile_cb(Metatile_Button *mb, Main_Window *mw);
	// Map
	static void change_block_cb(Block *b, Main_Window *mw);
	static void change_event_cb(Event *e, Main_Window *mw);
};

#endif
