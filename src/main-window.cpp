#include <cstdlib>
#include <cwctype>
#include <algorithm>
#include <queue>
#include <utility>

#pragma warning(push, 0)
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Toggle_Button.H>
#pragma warning(pop)

#include "version.h"
#include "utils.h"
#include "themes.h"
#include "image.h"
#include "widgets.h"
#include "modal-dialog.h"
#include "option-dialogs.h"
#include "tileset.h"
#include "metatileset.h"
#include "preferences.h"
#include "config.h"
#include "main-window.h"
#include "image.h"
#include "colors.h"
#include "icons.h"

#ifdef _WIN32
#include "resource.h"
#else
#include <unistd.h>
#include <X11/xpm.h>
#include "app-icon.xpm"
#endif

Main_Window::Main_Window(int x, int y, int w, int h, const char *) : Fl_Double_Window(x, y, w, h, PROGRAM_NAME),
	_directory(), _blk_file(), _asm_file(), _metatileset(), _map(), _map_events(), _metatile_buttons(), _clipboard(0),
	_wx(x), _wy(y), _ww(w), _wh(h) {
	// Get global configs
	Mode mode_config = (Mode)Preferences::get("mode", Mode::BLOCKS);
	mode(mode_config);

	int grid_config = Preferences::get("grid", 1);
	int zoom_config = Preferences::get("zoom", 0);
	int ids_config = Preferences::get("ids", 0);
	int hex_config = Preferences::get("hex", 0);
	int show_priority_config = Preferences::get("priority", 1);
	int show_events_config = Preferences::get("event", 1);
	int gameboy_screen_config = Preferences::get("gameboy", 1);
	Lighting lighting_config = (Lighting)Preferences::get("lighting", Lighting::DAY);

	int monochrome_config = Preferences::get("monochrome", 0);
	int allow_priority_config = Preferences::get("prioritize", 0);
	int allow_256_tiles_config = Preferences::get("all256", 0);
	Config::monochrome(!!monochrome_config);
	Config::allow_priority(!!allow_priority_config);
	Config::allow_256_tiles(!!allow_256_tiles_config);

	int auto_events_config = Preferences::get("events", 1);
	int special_lighting_config = Preferences::get("special", 1);
	int roof_colors_config = Preferences::get("roofs", 1);

	// Populate window

	int wx = 0, wy = 0, ww = w, wh = h;

	// Initialize menu bar
	_menu_bar = new Fl_Menu_Bar(wx, wy, w, 21);
	wy += _menu_bar->h();
	wh -= _menu_bar->h();

	// Toolbar
	_toolbar = new Toolbar(wx, wy, w, 26);
	wy += _toolbar->h();
	wh -= _toolbar->h();
	_new_tb = new Toolbar_Button(0, 0, 24, 24);
	_open_tb = new Toolbar_Button(0, 0, 24, 24);
	_load_event_script_tb = new Toolbar_Button(0, 0, 24, 24);
	_reload_event_script_tb = new Toolbar_Button(0, 0, 24, 24);
	_save_tb = new Toolbar_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 24); new Spacer(0, 0, 2, 24); new Fl_Box(0, 0, 2, 24);
	_print_tb = new Toolbar_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 24); new Spacer(0, 0, 2, 24); new Fl_Box(0, 0, 2, 24);
	_undo_tb = new Toolbar_Button(0, 0, 24, 24);
	_redo_tb = new Toolbar_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 24); new Spacer(0, 0, 2, 24); new Fl_Box(0, 0, 2, 24);
	_grid_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_zoom_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_ids_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_hex_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_show_priority_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_show_events_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_gameboy_screen_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 24); new Spacer(0, 0, 2, 24); new Fl_Box(0, 0, 2, 24);
	_blocks_mode_tb = new Toolbar_Radio_Button(0, 0, 24, 24);
	_events_mode_tb = new Toolbar_Radio_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 24); new Spacer(0, 0, 2, 24); new Fl_Box(0, 0, 2, 24);
	new Label(0, 0, text_width("Lighting:", 3), 24, "Lighting:");
	_lighting = new Dropdown(0, 0, text_width("Custom", 3) + 24, 22);
	_load_lighting_tb = new Toolbar_Button(0, 0, 24, 24);
	_edit_current_lighting_tb = new Toolbar_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 24); new Spacer(0, 0, 2, 24); new Fl_Box(0, 0, 2, 24);
	_add_sub_tb = new Toolbar_Button(0, 0, 24, 24);
	_resize_tb = new Toolbar_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 24); new Spacer(0, 0, 2, 24); new Fl_Box(0, 0, 2, 24);
	_change_tileset_tb = new Toolbar_Button(0, 0, 24, 24);
	_edit_tileset_tb = new Toolbar_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 24); new Spacer(0, 0, 2, 24); new Fl_Box(0, 0, 2, 24);
	_change_roof_tb = new Toolbar_Button(0, 0, 24, 24);
	_edit_roof_tb = new Toolbar_Button(0, 0, 24, 24);
	_toolbar->end();
	begin();

	// Status bar
	_status_bar = new Toolbar(wx, h-23, w, 23);
	wh -= _status_bar->h();
	_metatile_count = new Status_Bar_Field(0, 0, text_width("Blocks: 999", 8), 21, "");
	new Spacer(0, 0, 2, 21);
	_map_dimensions = new Status_Bar_Field(0, 0, text_width("Map: 999 x 999", 8), 21, "");
	new Spacer(0, 0, 2, 21);
	_hover_id = new Status_Bar_Field(0, 0, text_width("ID: $99", 8), 21, "");
	new Spacer(0, 0, 2, 21);
	_hover_xy = new Status_Bar_Field(0, 0, text_width("X/Y ($99, $99)", 8), 21, "");
	new Spacer(0, 0, 2, 21);
	_hover_event = new Status_Bar_Field(0, 0, text_width("Event: X/Y ($999, $999)", 8), 21, "");
	_status_bar->end();
	begin();

	// Sidebar
	int sw = METATILE_PX_SIZE * (zoom_config ? ZOOM_FACTOR : 1) * METATILES_PER_ROW + Fl::scrollbar_size();
	_sidebar = new Workspace(wx, wy, sw, wh);
	wx += _sidebar->w();
	ww -= _sidebar->w();
	_sidebar->type(Fl_Scroll::VERTICAL_ALWAYS);
	_sidebar->end();
	begin();

	// Map
	_map_scroll = new Workspace(wx, wy, ww, wh);
	_map_scroll->type(Fl_Scroll::BOTH);
	_map_group = new Fl_Group(wx, wy, 0, 0);

	// GameBoy screen
	_gameboy_screen = new GBScreen();
	_map_group->end();
	begin();

	// Dialogs
	_new_dir_chooser = new Directory_Chooser(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
	_blk_open_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_FILE);
	_blk_save_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	_pal_load_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_FILE);
	_pal_save_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	_roof_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_FILE);
	_asm_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_FILE);
	_png_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	_error_dialog = new Modal_Dialog(this, "Error", Modal_Dialog::ERROR_ICON);
	_warning_dialog = new Modal_Dialog(this, "Warning", Modal_Dialog::WARNING_ICON);
	_success_dialog = new Modal_Dialog(this, "Success", Modal_Dialog::SUCCESS_ICON);
	_unsaved_dialog = new Modal_Dialog(this, "Warning", Modal_Dialog::WARNING_ICON, true);
	_about_dialog = new Modal_Dialog(this, "About " PROGRAM_NAME, Modal_Dialog::APP_ICON);
	_map_options_dialog = new Map_Options_Dialog("Map Options");
	_tileset_options_dialog = new Tileset_Options_Dialog("Change Tileset", _map_options_dialog);
	_roof_options_dialog = new Roof_Options_Dialog("Change Roof", _map_options_dialog);
	_event_options_dialog = new Event_Options_Dialog("Edit Event");
	_resize_dialog = new Resize_Dialog("Resize Map");
	_add_sub_dialog = new Add_Sub_Dialog("Resize Blockset");
	_help_window = new Help_Window(48, 48, 500, 400, PROGRAM_NAME " Help");
	_block_window = new Block_Window(48, 48);
	_tileset_window = new Tileset_Window(48, 48);
	_roof_window = new Roof_Window(48, 48);
	_lighting_window = new Lighting_Window(48, 48);
	_monochrome_lighting_window = new Monochrome_Lighting_Window(48, 48);

	// Drag-and-drop receiver
	_dnd_receiver = new DnD_Receiver(0, 0, 0, 0);
	_dnd_receiver->callback((Fl_Callback *)drag_and_drop_cb);
	_dnd_receiver->user_data(this);

	// Configure window
	size_range(384, 256);
	resizable(_map_scroll);
	callback((Fl_Callback *)exit_cb, this);
	xclass(PROGRAM_NAME);

	// Configure window icon
#ifdef _WIN32
	icon((const void *)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON1)));
#else
	fl_open_display();
	XpmCreatePixmapFromData(fl_display, DefaultRootWindow(fl_display), (char **)&APP_ICON_XPM, &_icon_pixmap, &_icon_mask, NULL);
	icon((const void *)_icon_pixmap);
#endif

	// Configure workspaces
	_sidebar->dnd_receiver(_dnd_receiver);
	_map_scroll->dnd_receiver(_dnd_receiver);
	_map_scroll->resizable(NULL);
	_map_group->resizable(NULL);
	_map_group->clip_children(1);

	// Configure menu bar
	_menu_bar->box(OS_PANEL_THIN_UP_BOX);
	_menu_bar->down_box(FL_FLAT_BOX);

	// Configure menu bar items
	Fl_Menu_Item menu_items[] = {
		// label, shortcut, callback, data, flags
		OS_SUBMENU("&File"),
		OS_MENU_ITEM("&New...", FL_COMMAND + 'n', (Fl_Callback *)new_cb, this, 0),
		OS_MENU_ITEM("&Open...", FL_COMMAND + 'o', (Fl_Callback *)open_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Close", FL_COMMAND + 'w', (Fl_Callback *)close_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Save", FL_COMMAND + 's', (Fl_Callback *)save_cb, this, 0),
		OS_MENU_ITEM("Save &As...", FL_COMMAND + 'S', (Fl_Callback *)save_as_cb, this, 0),
		OS_MENU_ITEM("Save &Map", 0, (Fl_Callback *)save_map_cb, this, 0),
		OS_MENU_ITEM("Save &Blockset", 0, (Fl_Callback *)save_metatiles_cb, this, 0),
		OS_MENU_ITEM("Save &Tileset", 0, (Fl_Callback *)save_tileset_cb, this, 0),
		OS_MENU_ITEM("Save &Roof", 0, (Fl_Callback *)save_roof_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("Load &Event Script...", FL_COMMAND + 'a', (Fl_Callback *)load_event_script_cb, this, 0),
		OS_MENU_ITEM("Save E&vent Script", FL_COMMAND + 'A', (Fl_Callback *)save_event_script_cb, this, 0),
		OS_MENU_ITEM("V&iew Event Script", FL_COMMAND + 'u', (Fl_Callback *)view_event_script_cb, this, 0),
		OS_MENU_ITEM("Reloa&d Event Script", FL_COMMAND + 'r', (Fl_Callback *)reload_event_script_cb, this, 0),
		OS_MENU_ITEM("&Unload Event Script", FL_COMMAND + 'W', (Fl_Callback *)unload_event_script_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("Load &Lighting...", FL_COMMAND + 'l', (Fl_Callback *)load_lighting_cb, this, 0),
		OS_MENU_ITEM("Export Current Li&ghting...", 0, (Fl_Callback *)export_current_lighting_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("Load Roo&f Colors", 0, (Fl_Callback *)load_roof_colors_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Print...", FL_COMMAND + 'p', (Fl_Callback *)print_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("E&xit", FL_ALT + FL_F + 4, (Fl_Callback *)exit_cb, this, 0),
		{},
		OS_SUBMENU("&Edit"),
		OS_MENU_ITEM("&Undo", FL_COMMAND + 'z', (Fl_Callback *)undo_cb, this, 0),
		OS_MENU_ITEM("&Redo", FL_COMMAND + 'y', (Fl_Callback *)redo_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Copy Block", FL_COMMAND + 'c', (Fl_Callback *)copy_metatile_cb, this, 0),
		OS_MENU_ITEM("&Paste Block", FL_COMMAND + 'v', (Fl_Callback *)paste_metatile_cb, this, 0),
		OS_MENU_ITEM("S&wap Block", FL_COMMAND + 'x', (Fl_Callback *)swap_metatiles_cb, this, 0),
		{},
		OS_SUBMENU("&View"),
		OS_MENU_ITEM("&Theme", 0, NULL, NULL, FL_SUBMENU | FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Aero", 0, (Fl_Callback *)aero_theme_cb, this,
			FL_MENU_RADIO | (OS::current_theme() == OS::AERO ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Metro", 0, (Fl_Callback *)metro_theme_cb, this,
			FL_MENU_RADIO | (OS::current_theme() == OS::METRO ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Greybird", 0, (Fl_Callback *)greybird_theme_cb, this,
			FL_MENU_RADIO | (OS::current_theme() == OS::GREYBIRD ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Blue", 0, (Fl_Callback *)blue_theme_cb, this,
			FL_MENU_RADIO | (OS::current_theme() == OS::BLUE ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Dark", 0, (Fl_Callback *)dark_theme_cb, this,
			FL_MENU_RADIO | (OS::current_theme() == OS::DARK ? FL_MENU_VALUE : 0)),
		{},
		OS_MENU_ITEM("&Grid", FL_COMMAND + 'g', (Fl_Callback *)grid_cb, this,
			FL_MENU_TOGGLE | (grid_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Zoom", FL_COMMAND + '=', (Fl_Callback *)zoom_cb, this,
			FL_MENU_TOGGLE | (zoom_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("Block &IDs", FL_COMMAND + 'i', (Fl_Callback *)ids_cb, this,
			FL_MENU_TOGGLE | (ids_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Hexadecimal", FL_COMMAND + FL_SHIFT + '4', (Fl_Callback *)hex_cb, this,
			FL_MENU_TOGGLE | (hex_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("Show &Priority", FL_COMMAND + 'P', (Fl_Callback *)show_priority_cb, this,
			FL_MENU_TOGGLE | (show_priority_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("Show &Events", FL_COMMAND + 'R', (Fl_Callback *)show_events_cb, this,
			FL_MENU_TOGGLE | (show_events_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("Game&Boy Screen", FL_COMMAND + 'M', (Fl_Callback *)gameboy_screen_cb, this,
			FL_MENU_TOGGLE | (gameboy_screen_config ? FL_MENU_VALUE : 0) | FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Lighting", 0, NULL, NULL, FL_SUBMENU | FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Morn", 0, (Fl_Callback *)morn_lighting_cb, this,
			FL_MENU_RADIO | (lighting_config == Lighting::MORN ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Day", 0, (Fl_Callback *)day_lighting_cb, this,
			FL_MENU_RADIO | (lighting_config == Lighting::DAY ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Night", 0, (Fl_Callback *)night_lighting_cb, this,
			FL_MENU_RADIO | (lighting_config == Lighting::NITE ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Indoor", 0, (Fl_Callback *)indoor_lighting_cb, this,
			FL_MENU_RADIO | (lighting_config == Lighting::INDOOR ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Custom", 0, (Fl_Callback *)custom_lighting_cb, this,
			FL_MENU_RADIO | (lighting_config == Lighting::CUSTOM ? FL_MENU_VALUE : 0)),
		{},
		OS_MENU_ITEM("Full &Screen", FL_F + 11, (Fl_Callback *)full_screen_cb, this, FL_MENU_TOGGLE),
		{},
		OS_SUBMENU("&Mode"),
		OS_MENU_ITEM("&Blocks", FL_COMMAND + 'B', (Fl_Callback *)blocks_mode_cb, this,
			FL_MENU_RADIO | (mode() == Mode::BLOCKS ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Events", FL_COMMAND + 'E', (Fl_Callback *)events_mode_cb, this,
			FL_MENU_RADIO | (mode() == Mode::EVENTS ? FL_MENU_VALUE : 0) | FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Switch Mode", FL_Tab, (Fl_Callback *)switch_mode_cb, this, 0),
		{},
		OS_SUBMENU("&Tools"),
		OS_MENU_ITEM("Resize &Blockset...", FL_COMMAND + 'b', (Fl_Callback *)add_sub_cb, this, 0),
		OS_MENU_ITEM("Resize &Map...", FL_COMMAND + 'e', (Fl_Callback *)resize_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("Chan&ge Tileset...", FL_COMMAND + 'h', (Fl_Callback *)change_tileset_cb, this, 0),
		OS_MENU_ITEM("Edit &Tileset...", FL_COMMAND + 't', (Fl_Callback *)edit_tileset_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("C&hange Roof...", FL_COMMAND + 'H', (Fl_Callback *)change_roof_cb, this, 0),
		OS_MENU_ITEM("Edit &Roof...", FL_COMMAND + 'f', (Fl_Callback *)edit_roof_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("Edit Current &Lighting...", FL_COMMAND + 'L', (Fl_Callback *)edit_current_lighting_cb, this, 0),
		{},
		OS_SUBMENU("&Options"),
		OS_MENU_ITEM("&Monochrome", 0, (Fl_Callback *)monochrome_cb, this,
			FL_MENU_TOGGLE | (monochrome_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("Tile &Priority", 0, (Fl_Callback *)allow_priority_cb, this,
			FL_MENU_TOGGLE | (allow_priority_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("256 &Tiles", 0, (Fl_Callback *)allow_256_tiles_cb, this,
			FL_MENU_TOGGLE | (allow_256_tiles_config ? FL_MENU_VALUE : 0) | FL_MENU_DIVIDER),
		OS_MENU_ITEM("Auto-Load &Events", 0, (Fl_Callback *)auto_load_events_cb, this,
			FL_MENU_TOGGLE | (auto_events_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("Auto-Load &Special Lighting", 0, (Fl_Callback *)auto_load_special_lighting_cb, this,
			FL_MENU_TOGGLE | (special_lighting_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("Auto-Load &Roof Colors", 0, (Fl_Callback *)auto_load_roof_colors_cb, this,
			FL_MENU_TOGGLE | (roof_colors_config ? FL_MENU_VALUE : 0)),
		{},
		OS_SUBMENU("&Help"),
		OS_MENU_ITEM("&Help", FL_F + 1, (Fl_Callback *)help_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("&About", FL_COMMAND + '/', (Fl_Callback *)about_cb, this, 0),
		{},
		{}
	};
	_menu_bar->copy(menu_items);

	// Initialize menu bar items
#define PM_FIND_MENU_ITEM_CB(c) (const_cast<Fl_Menu_Item *>(_menu_bar->find_item((Fl_Callback *)(c))))
	_aero_theme_mi = PM_FIND_MENU_ITEM_CB(aero_theme_cb);
	_metro_theme_mi = PM_FIND_MENU_ITEM_CB(metro_theme_cb);
	_greybird_theme_mi = PM_FIND_MENU_ITEM_CB(greybird_theme_cb);
	_blue_theme_mi = PM_FIND_MENU_ITEM_CB(blue_theme_cb);
	_dark_theme_mi = PM_FIND_MENU_ITEM_CB(dark_theme_cb);
	_grid_mi = PM_FIND_MENU_ITEM_CB(grid_cb);
	_zoom_mi = PM_FIND_MENU_ITEM_CB(zoom_cb);
	_ids_mi = PM_FIND_MENU_ITEM_CB(ids_cb);
	_hex_mi = PM_FIND_MENU_ITEM_CB(hex_cb);
	_show_priority_mi = PM_FIND_MENU_ITEM_CB(show_priority_cb);
	_show_events_mi = PM_FIND_MENU_ITEM_CB(show_events_cb);
	_gameboy_screen_mi = PM_FIND_MENU_ITEM_CB(gameboy_screen_cb);
	_full_screen_mi = PM_FIND_MENU_ITEM_CB(full_screen_cb);
	_morn_mi = PM_FIND_MENU_ITEM_CB(morn_lighting_cb);
	_day_mi = PM_FIND_MENU_ITEM_CB(day_lighting_cb);
	_night_mi = PM_FIND_MENU_ITEM_CB(night_lighting_cb);
	_indoor_mi = PM_FIND_MENU_ITEM_CB(indoor_lighting_cb);
	_custom_mi = PM_FIND_MENU_ITEM_CB(custom_lighting_cb);
	_blocks_mode_mi = PM_FIND_MENU_ITEM_CB(blocks_mode_cb);
	_events_mode_mi = PM_FIND_MENU_ITEM_CB(events_mode_cb);
	_monochrome_mi = PM_FIND_MENU_ITEM_CB(monochrome_cb);
	_allow_priority_mi = PM_FIND_MENU_ITEM_CB(allow_priority_cb);
	_allow_256_tiles_mi = PM_FIND_MENU_ITEM_CB(allow_256_tiles_cb);
	_auto_events_mi = PM_FIND_MENU_ITEM_CB(auto_load_events_cb);
	_special_lighting_mi = PM_FIND_MENU_ITEM_CB(auto_load_special_lighting_cb);
	_roof_colors_mi = PM_FIND_MENU_ITEM_CB(auto_load_roof_colors_cb);
	// Conditional menu items
	_load_event_script_mi = PM_FIND_MENU_ITEM_CB(load_event_script_cb);
	_view_event_script_mi = PM_FIND_MENU_ITEM_CB(view_event_script_cb);
	_reload_event_script_mi = PM_FIND_MENU_ITEM_CB(reload_event_script_cb);
	_unload_event_script_mi = PM_FIND_MENU_ITEM_CB(unload_event_script_cb);
	_load_roof_colors_mi = PM_FIND_MENU_ITEM_CB(load_roof_colors_cb);
	_close_mi = PM_FIND_MENU_ITEM_CB(close_cb);
	_save_mi = PM_FIND_MENU_ITEM_CB(save_cb);
	_save_as_mi = PM_FIND_MENU_ITEM_CB(save_as_cb);
	_save_map_mi = PM_FIND_MENU_ITEM_CB(save_map_cb);
	_save_blockset_mi = PM_FIND_MENU_ITEM_CB(save_metatiles_cb);
	_save_tileset_mi = PM_FIND_MENU_ITEM_CB(save_tileset_cb);
	_save_roof_mi = PM_FIND_MENU_ITEM_CB(save_roof_cb);
	_save_event_script_mi = PM_FIND_MENU_ITEM_CB(save_event_script_cb);
	_print_mi = PM_FIND_MENU_ITEM_CB(print_cb);
	_undo_mi = PM_FIND_MENU_ITEM_CB(undo_cb);
	_redo_mi = PM_FIND_MENU_ITEM_CB(redo_cb);
	_copy_block_mi = PM_FIND_MENU_ITEM_CB(copy_metatile_cb);
	_paste_block_mi = PM_FIND_MENU_ITEM_CB(paste_metatile_cb);
	_swap_block_mi = PM_FIND_MENU_ITEM_CB(swap_metatiles_cb);
	_resize_blockset_mi = PM_FIND_MENU_ITEM_CB(add_sub_cb);
	_resize_map_mi = PM_FIND_MENU_ITEM_CB(resize_cb);
	_change_tileset_mi = PM_FIND_MENU_ITEM_CB(change_tileset_cb);
	_edit_tileset_mi = PM_FIND_MENU_ITEM_CB(edit_tileset_cb);
	_change_roof_mi = PM_FIND_MENU_ITEM_CB(change_roof_cb);
	_edit_roof_mi = PM_FIND_MENU_ITEM_CB(edit_roof_cb);
	_edit_current_lighting_mi = PM_FIND_MENU_ITEM_CB(edit_current_lighting_cb);
#undef PM_FIND_MENU_ITEM_CB

	// Configure toolbar buttons

	_new_tb->tooltip("New... (Ctrl+N)");
	_new_tb->callback((Fl_Callback *)new_cb, this);
	_new_tb->image(NEW_ICON);
	_new_tb->deimage(NEW_DISABLED_ICON);
	_new_tb->take_focus();

	_open_tb->tooltip("Open... (Ctrl+O)");
	_open_tb->callback((Fl_Callback *)open_cb, this);
	_open_tb->image(OPEN_ICON);
	_open_tb->deimage(OPEN_DISABLED_ICON);

	_load_event_script_tb->tooltip("Load Event Script... (Ctrl+A)");
	_load_event_script_tb->callback((Fl_Callback *)load_event_script_cb, this);
	_load_event_script_tb->image(LOAD_ICON);
	_load_event_script_tb->deimage(LOAD_DISABLED_ICON);

	_reload_event_script_tb->tooltip("Reload Event Script... (Ctrl+R)");
	_reload_event_script_tb->callback((Fl_Callback *)reload_event_script_cb, this);
	_reload_event_script_tb->image(RELOAD_ICON);
	_reload_event_script_tb->deimage(RELOAD_DISABLED_ICON);

	_save_tb->tooltip("Save (Ctrl+S)");
	_save_tb->callback((Fl_Callback *)save_cb, this);
	_save_tb->image(SAVE_ICON);
	_save_tb->deimage(SAVE_DISABLED_ICON);

	_print_tb->tooltip("Print (Ctrl+P)");
	_print_tb->callback((Fl_Callback *)print_cb, this);
	_print_tb->image(PRINT_ICON);
	_print_tb->deimage(PRINT_DISABLED_ICON);

	_undo_tb->tooltip("Undo (Ctrl+Z)");
	_undo_tb->callback((Fl_Callback *)undo_cb, this);
	_undo_tb->image(UNDO_ICON);
	_undo_tb->deimage(UNDO_DISABLED_ICON);

	_redo_tb->tooltip("Redo (Ctrl+Y)");
	_redo_tb->callback((Fl_Callback *)redo_cb, this);
	_redo_tb->image(REDO_ICON);
	_redo_tb->deimage(REDO_DISABLED_ICON);

	_grid_tb->tooltip("Grid (Ctrl+G)");
	_grid_tb->callback((Fl_Callback *)grid_tb_cb, this);
	_grid_tb->image(GRID_ICON);
	_grid_tb->deimage(GRID_DISABLED_ICON);
	_grid_tb->value(grid());

	_zoom_tb->tooltip("Zoom (Ctrl+=)");
	_zoom_tb->callback((Fl_Callback *)zoom_tb_cb, this);
	_zoom_tb->image(ZOOM_ICON);
	_zoom_tb->deimage(ZOOM_DISABLED_ICON);
	_zoom_tb->value(zoom());

	_ids_tb->tooltip("Block IDs (Ctrl+I)");
	_ids_tb->callback((Fl_Callback *)ids_tb_cb, this);
	_ids_tb->image(IDS_ICON);
	_ids_tb->deimage(IDS_DISABLED_ICON);
	_ids_tb->value(ids());

	_hex_tb->tooltip("Hexadecimal (Ctrl+$)");
	_hex_tb->callback((Fl_Callback *)hex_tb_cb, this);
	_hex_tb->image(HEX_ICON);
	_hex_tb->deimage(HEX_DISABLED_ICON);
	_hex_tb->value(hex());

	_show_priority_tb->tooltip("Show Priority (Ctrl+Shift+P)");
	_show_priority_tb->callback((Fl_Callback *)show_priority_tb_cb, this);
	_show_priority_tb->image(PRIORITY_ICON);
	_show_priority_tb->deimage(PRIORITY_DISABLED_ICON);
	_show_priority_tb->value(show_priority());

	_show_events_tb->tooltip("Show Events (Ctrl+Shift+R)");
	_show_events_tb->callback((Fl_Callback *)show_events_tb_cb, this);
	_show_events_tb->image(SHOW_ICON);
	_show_events_tb->deimage(SHOW_DISABLED_ICON);
	_show_events_tb->value(show_events());

	_gameboy_screen_tb->tooltip("GameBoy Screen (Ctrl+Shift+M)");
	_gameboy_screen_tb->callback((Fl_Callback *)gameboy_screen_tb_cb, this);
	_gameboy_screen_tb->image(GAMEBOY_ICON);
	_gameboy_screen_tb->deimage(GAMEBOY_DISABLED_ICON);
	_gameboy_screen_tb->value(gameboy_screen());

	_blocks_mode_tb->tooltip("Blocks Mode (Ctrl+Shift+B)");
	_blocks_mode_tb->callback((Fl_Callback *)blocks_mode_tb_cb, this);
	_blocks_mode_tb->image(BLOCKS_ICON);
	_blocks_mode_tb->deimage(BLOCKS_DISABLED_ICON);
	_blocks_mode_tb->value(mode() == Mode::BLOCKS);

	_events_mode_tb->tooltip("Events Mode (Ctrl+Shift+E)");
	_events_mode_tb->callback((Fl_Callback *)events_mode_tb_cb, this);
	_events_mode_tb->image(EVENTS_ICON);
	_events_mode_tb->deimage(EVENTS_DISABLED_ICON);
	_events_mode_tb->value(mode() == Mode::EVENTS);

	_lighting->add("Morn");   // Lighting::MORN
	_lighting->add("Day");    // Lighting::DAY
	_lighting->add("Night");  // Lighting::NITE
	_lighting->add("Indoor"); // Lighting::INDOOR
	_lighting->add("Custom"); // Lighting::CUSTOM
	_lighting->value(lighting_config);
	_lighting->callback((Fl_Callback *)lighting_cb, this);

	_add_sub_tb->tooltip("Resize Blockset... (Ctrl+B)");
	_add_sub_tb->callback((Fl_Callback *)add_sub_cb, this);
	_add_sub_tb->image(ADD_SUB_ICON);
	_add_sub_tb->deimage(ADD_SUB_DISABLED_ICON);

	_resize_tb->tooltip("Resize Map... (Ctrl+E)");
	_resize_tb->callback((Fl_Callback *)resize_cb, this);
	_resize_tb->image(RESIZE_ICON);
	_resize_tb->deimage(RESIZE_DISABLED_ICON);

	_change_tileset_tb->tooltip("Change Tileset... (Ctrl+H)");
	_change_tileset_tb->callback((Fl_Callback *)change_tileset_cb, this);
	_change_tileset_tb->image(CHANGE_ICON);
	_change_tileset_tb->deimage(CHANGE_DISABLED_ICON);

	_edit_tileset_tb->tooltip("Edit Tileset... (Ctrl+T)");
	_edit_tileset_tb->callback((Fl_Callback *)edit_tileset_cb, this);
	_edit_tileset_tb->image(TILESET_ICON);
	_edit_tileset_tb->deimage(TILESET_DISABLED_ICON);

	_change_roof_tb->tooltip("Change Roof... (Ctrl+Shift+H)");
	_change_roof_tb->callback((Fl_Callback *)change_roof_cb, this);
	_change_roof_tb->image(CHANGE_ROOF_ICON);
	_change_roof_tb->deimage(CHANGE_ROOF_DISABLED_ICON);

	_edit_roof_tb->tooltip("Edit Roof... (Ctrl+F)");
	_edit_roof_tb->callback((Fl_Callback *)edit_roof_cb, this);
	_edit_roof_tb->image(ROOF_ICON);
	_edit_roof_tb->deimage(ROOF_DISABLED_ICON);

	_load_lighting_tb->tooltip("Load Lighting... (Ctrl+L)");
	_load_lighting_tb->callback((Fl_Callback *)load_lighting_cb, this);
	_load_lighting_tb->image(LOAD_LIGHTING_ICON);
	_load_lighting_tb->deimage(LOAD_LIGHTING_DISABLED_ICON);

	_edit_current_lighting_tb->tooltip("Edit Current Lighting... (Ctrl+Shift+L)");
	_edit_current_lighting_tb->callback((Fl_Callback *)edit_current_lighting_cb, this);
	_edit_current_lighting_tb->image(LIGHTING_ICON);
	_edit_current_lighting_tb->deimage(LIGHTING_DISABLED_ICON);

	// Configure dialogs

	_new_dir_chooser->title("Choose Project Directory");

	_blk_open_chooser->title("Open Map");
	_blk_open_chooser->filter("BLK Files\t*.blk\n");

	_blk_save_chooser->title("Save Map");
	_blk_save_chooser->filter("BLK Files\t*.blk\n");
	_blk_save_chooser->preset_file("NewMap.blk");

	_pal_load_chooser->title("Open Lighting");
	_pal_load_chooser->filter("PAL Files\t*.pal\n");

	_pal_save_chooser->title("Save Lighting");
	_pal_save_chooser->filter("PAL Files\t*.pal\n");
	_pal_save_chooser->preset_file("lighting.pal");

	_roof_chooser->title("Open Roof Tiles");
	_roof_chooser->filter("PNG Files\t*.png\n2BPP Files\t*.2bpp\n");

	_asm_chooser->title("Open Event Script");
	_asm_chooser->filter("ASM Files\t*.asm\n");

	_png_chooser->title("Print Screenshot");
	_png_chooser->filter("PNG Files\t*.png\n");
	_png_chooser->preset_file("screenshot.png");

	_error_dialog->width_range(280, 700);
	_warning_dialog->width_range(280, 700);
	_success_dialog->width_range(280, 700);
	_unsaved_dialog->width_range(280, 700);

	std::string subject(PROGRAM_NAME " " PROGRAM_VERSION_STRING), message(
		"Copyright \xc2\xa9 " CURRENT_YEAR " " PROGRAM_AUTHOR ".\n"
		"\n"
		"Source code is available at:\n"
		"https://github.com/Rangi42/polished-map\n"
		"\n"
		"Some icons by Yusuke Kamiyamane."
	);
	_about_dialog->subject(subject);
	_about_dialog->message(message);
	_about_dialog->width_range(280, 700);

	_help_window->content(
#include "help.html" // a C++11 raw string literal
		);

	update_active_controls();
}

Main_Window::~Main_Window() {
	delete _menu_bar; // includes menu items
	delete _toolbar; // includes toolbar buttons
	delete _sidebar; // includes metatiles
	delete _status_bar; // includes status bar fields
	delete _map_scroll; // includes map and blocks
	delete _gameboy_screen;
	delete _dnd_receiver;
	delete _blk_open_chooser;
	delete _blk_save_chooser;
	delete _pal_load_chooser;
	delete _pal_save_chooser;
	delete _png_chooser;
	delete _error_dialog;
	delete _warning_dialog;
	delete _success_dialog;
	delete _unsaved_dialog;
	delete _about_dialog;
	delete _map_options_dialog;
	delete _tileset_options_dialog;
	delete _roof_options_dialog;
	delete _event_options_dialog;
	delete _resize_dialog;
	delete _add_sub_dialog;
	delete _help_window;
	delete _block_window;
	delete _tileset_window;
	delete _roof_window;
	delete _lighting_window;
	delete _monochrome_lighting_window;
}

void Main_Window::show() {
	Fl_Double_Window::show();
#ifdef _WIN32
	// Fix for 16x16 icon from <http://www.fltk.org/str.php?L925>
	HWND hwnd = fl_xid(this);
	HANDLE big_icon = LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,
		GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CXICON), 0);
	SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(big_icon));
	HANDLE small_icon = LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0);
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(small_icon));
#else
	// Fix for X11 icon alpha mask <https://www.mail-archive.com/fltk@easysw.com/msg02863.html>
	XWMHints *hints = XGetWMHints(fl_display, fl_xid(this));
	hints->flags |= IconMaskHint;
	hints->icon_mask = _icon_mask;
	XSetWMHints(fl_display, fl_xid(this), hints);
	XFree(hints);
#endif
}

bool Main_Window::unsaved(void) const {
	return _map.modified() || _map_events.modified() || _metatileset.modified() ||
		_metatileset.const_tileset()->modified() || _metatileset.const_tileset()->modified_roof();
}

const char *Main_Window::modified_filename() {
	if (_map.modified()) {
		if (_blk_file.empty()) { return NEW_MAP_NAME; }
		return fl_filename_name(_blk_file.c_str());
	}
	if (_map_events.modified()) {
		if (_asm_file.empty()) { return NEW_MAP_NAME; }
		return fl_filename_name(_asm_file.c_str());
	}
	static char buffer[FL_PATH_MAX] = {};
	const Tileset *tileset = _metatileset.const_tileset();
	if (tileset->modified()) {
		Config::tileset_path(buffer, _directory.c_str(), tileset->name());
	}
	else if (tileset->modified_roof()) {
		Config::roof_path(buffer, _directory.c_str(), tileset->roof_name());
	}
	else {
		Config::metatileset_path(buffer, _directory.c_str(), _metatileset.tileset()->name());
	}
	return fl_filename_name(buffer);
}

int Main_Window::handle(int event) {
	int key = 0;
	switch (event) {
	case FL_FOCUS:
	case FL_UNFOCUS:
		return 1;
	case FL_SHORTCUT:
		key = Fl::event_key();
		//if (key & FL_KP == FL_KP) { key -= FL_KP; } // normalize numpad keys into digits
		if (handle_hotkey(key)) { return 1; }
		// fall through
	default:
		return Fl_Double_Window::handle(event);
	}
}

int Main_Window::handle_hotkey(int key) {
	if (!_map.size()) { return 0; }
	if ((key & FL_KP) == FL_KP) { key -= FL_KP; } // normalize numpad keys into digits
	if (key < '0' || key > '9') { return 0; } // 0-9 keys only
	if (Fl::event_ctrl() && Fl::event_shift()) {
		// Ctrl+Shift+0-9 unassign the hotkey
		auto s = hotkey_metatile(key);
		if (s == no_metatile()) { return 0; }
		uint8_t id = s->second;
		_hotkey_metatiles.erase(key);
		_metatile_hotkeys.erase(id);
		redraw();
	}
	else if (Fl::event_ctrl()) {
		// Ctrl+0-9 assign the selected metatile to the hotkey
		if (!_selected) { return 0; }
		uint8_t id = _selected->id();
		auto sk = metatile_hotkey(id);
		if (sk != no_hotkey()) {
			// Unassign the metatile's previous key
			int prev_key = sk->second;
			_hotkey_metatiles.erase(prev_key);
		}
		auto sm = hotkey_metatile(key);
		if (sm != no_metatile()) {
			// Unassign the key from the previous metatile
			uint8_t prev_id = sm->second;
			_metatile_hotkeys.erase(prev_id);
		}
		_hotkey_metatiles[key] = id;
		_metatile_hotkeys[id] = key;
		redraw();
	}
	else {
		// 0-9 select the metatile assigned to the hotkey
		auto s = hotkey_metatile(key);
		if (s == no_metatile()) { return 0; }
		uint8_t id = s->second;
		if (id >= _metatileset.size()) { return 0; }
		select_metatile(_metatile_buttons[id]);
		return 1;
	}
	return 0;
}

void Main_Window::update_status(Block *b) {
	if (!_map.size()) {
		_metatile_count->label("");
		_map_dimensions->label("");
		_hover_id->label("");
		_hover_xy->label("");
		_hover_event->label("");
		_status_bar->redraw();
		return;
	}
	char buffer[64] = {};
	if (!b) {
#ifdef __GNUC__
		sprintf(buffer, "Blocks: %zu", _metatileset.size());
#else
		sprintf(buffer, "Blocks: %u", (uint32_t)_metatileset.size());
#endif
		_metatile_count->copy_label(buffer);
		sprintf(buffer, "Map: %u x %u", _map.width(), _map.height());
		_map_dimensions->copy_label(buffer);
		_hover_id->label("");
		_hover_xy->label("");
		_hover_event->label("");
		_status_bar->redraw();
		return;
	}
	uint8_t row = b->row(), col = b->col(), id = b->id();
	bool hex_ = hex();
	sprintf(buffer, (hex_ ? "ID: $%02X" : "ID: %u"), id);
	_hover_id->copy_label(buffer);
	sprintf(buffer, (hex_ ? "X/Y ($%X, $%X)" : "X/Y (%u, %u)"), col, row);
	_hover_xy->copy_label(buffer);
	update_event_cursor(b);
}

void Main_Window::update_event_cursor(Block *b) {
	if (_mode != Mode::EVENTS || !b) {
		_hover_event->label("");
		_status_bar->redraw();
		return;
	}
	char buffer[64] = {};
	int ey = (int)b->row() * 2 + b->bottom_half(), ex = (int)b->col() * 2 + b->right_half();
	sprintf(buffer, (hex() ? "Event: X/Y ($%X, $%X)" : "Event: X/Y (%u, %u)"), ex, ey);
	_hover_event->copy_label(buffer);
	_status_bar->redraw();
}

void Main_Window::update_gameboy_screen(Block *b) {
	if (_mode == Mode::EVENTS && b && gameboy_screen()) {
		if (!_gameboy_screen->visible()) {
			_gameboy_screen->show();
		}
		int hx = b->x() + b->right_half() * b->w() / 2, hy = b->y() + b->bottom_half() * b->h() / 2;
		int hs = metatile_size() / 2;
		_gameboy_screen->resize(hx-hs*4, hy-hs*4, hs*10, hs*9);
		redraw_gameboy_screen();
	}
	else if (_gameboy_screen->visible()) {
		_gameboy_screen->hide();
		redraw_map();
	}
	else if (b) {
		b->redraw();
	}
}

void Main_Window::update_active_controls() {
	update_priority_controls();
	if (_map.size()) {
		_load_event_script_mi->activate();
		_load_event_script_tb->activate();
		_close_mi->activate();
		_save_mi->activate();
		_save_tb->activate();
		_save_as_mi->activate();
		_save_map_mi->activate();
		_save_blockset_mi->activate();
		_save_tileset_mi->activate();
		_print_mi->activate();
		_print_tb->activate();
		if (_map_events.loaded()) {
			_view_event_script_mi->activate();
			_unload_event_script_mi->activate();
			_reload_event_script_mi->activate();
			_save_event_script_mi->activate();
			_reload_event_script_tb->activate();
		}
		else {
			_view_event_script_mi->deactivate();
			_unload_event_script_mi->deactivate();
			_reload_event_script_mi->deactivate();
			_save_event_script_mi->deactivate();
			_reload_event_script_tb->deactivate();
		}
		const Tileset *tileset = _metatileset.const_tileset();
		if (tileset && _map.group()) {
			_load_roof_colors_mi->activate();
		}
		else {
			_load_roof_colors_mi->deactivate();
		}
		if (_map.can_undo()) {
			_undo_mi->activate();
			_undo_tb->activate();
		}
		else {
			_undo_mi->deactivate();
			_undo_tb->deactivate();
		}
		if (_map.can_redo()) {
			_redo_mi->activate();
			_redo_tb->activate();
		}
		else {
			_redo_mi->deactivate();
			_redo_tb->deactivate();
		}
		_copy_block_mi->activate();
		if (_copied && _selected) {
			_paste_block_mi->activate();
			_swap_block_mi->activate();
		}
		else {
			_paste_block_mi->deactivate();
			_swap_block_mi->deactivate();
		}
		_resize_blockset_mi->activate();
		_add_sub_tb->activate();
		_resize_map_mi->activate();
		_resize_tb->activate();
		_change_tileset_mi->activate();
		_change_tileset_tb->activate();
		_edit_tileset_mi->activate();
		_edit_tileset_tb->activate();
		if (_map_options_dialog->num_roofs() > 0) {
			_change_roof_mi->activate();
			_change_roof_tb->activate();
		}
		else {
			_change_roof_mi->deactivate();
			_change_roof_tb->deactivate();
		}
		if (tileset->num_roof_tiles() > 0) {
			_save_roof_mi->activate();
			_edit_roof_mi->activate();
			_edit_roof_tb->activate();
		}
		else {
			_save_roof_mi->deactivate();
			_edit_roof_mi->deactivate();
			_edit_roof_tb->deactivate();
		}
		_edit_current_lighting_mi->activate();
	}
	else {
		_load_event_script_mi->deactivate();
		_load_event_script_tb->deactivate();
		_load_roof_colors_mi->deactivate();
		_reload_event_script_mi->deactivate();
		_reload_event_script_tb->deactivate();
		_close_mi->deactivate();
		_view_event_script_mi->deactivate();
		_unload_event_script_mi->deactivate();
		_save_mi->deactivate();
		_save_tb->deactivate();
		_save_as_mi->deactivate();
		_save_map_mi->deactivate();
		_save_blockset_mi->deactivate();
		_save_tileset_mi->deactivate();
		_save_roof_mi->deactivate();
		_save_event_script_mi->deactivate();
		_print_mi->deactivate();
		_print_tb->deactivate();
		_undo_mi->deactivate();
		_undo_tb->deactivate();
		_redo_mi->deactivate();
		_redo_tb->deactivate();
		_copy_block_mi->deactivate();
		_paste_block_mi->deactivate();
		_swap_block_mi->deactivate();
		_resize_blockset_mi->deactivate();
		_add_sub_tb->deactivate();
		_resize_map_mi->deactivate();
		_resize_tb->deactivate();
		_change_tileset_mi->deactivate();
		_edit_tileset_mi->deactivate();
		_change_tileset_tb->deactivate();
		_edit_tileset_tb->deactivate();
		_change_roof_mi->deactivate();
		_change_roof_tb->deactivate();
		_edit_roof_mi->deactivate();
		_edit_roof_tb->deactivate();
		_edit_current_lighting_mi->deactivate();
	}
}

void Main_Window::update_priority_controls() {
	if (Config::allow_priority()) {
		_allow_priority_mi->set();
		_show_priority_mi->activate();
		_show_priority_tb->activate();
	}
	else {
		_allow_priority_mi->clear();
		_show_priority_mi->deactivate();
		_show_priority_tb->deactivate();
	}
}

void Main_Window::flood_fill(Block *b, uint8_t f, uint8_t t) {
	if (f == t) { return; }
	std::queue<size_t> queue;
	uint8_t w = _map.width(), h = _map.height();
	uint8_t row = b->row(), col = b->col();
	size_t i = row * w + col;
	queue.push(i);
	while (!queue.empty()) {
		size_t i = queue.front();
		queue.pop();
		Block *b = _map.block(i);
		if (b->id() != f) { continue; }
		b->id(t); // fill
		uint8_t row = b->row(), col = b->col();
		if (col > 0) { queue.push(i-1); } // left
		if (col < w - 1) { queue.push(i+1); } // right
		if (row > 0) { queue.push(i-w); } // up
		if (row < h - 1) { queue.push(i+w); } // down
	}
}

void Main_Window::substitute_block(uint8_t f, uint8_t t) {
	size_t n = _map.size();
	for (size_t i = 0; i < n; i++) {
		Block *b = _map.block(i);
		if (b->id() == f) {
			b->id(t);
		}
	}
}

void Main_Window::open_map(const char *filename) {
	const char *basename = fl_filename_name(filename);

	char directory[FL_PATH_MAX] = {};
	if (!Config::project_path_from_blk_path(filename, directory)) {
		std::string msg = "Could not find the project directory for\n";
		msg = msg + basename + "!\nMake sure it contains a main.asm file.";
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return;
	}

	open_map(directory, filename);
}

void Main_Window::open_map(const char *directory, const char *filename) {
	// get map options
	Map_Attributes attrs;
	if (!_map_options_dialog->limit_blk_options(filename, directory, attrs)) {
		std::string msg = "This is not a valid project!\n\n"
			"Make sure the Options are correct.";
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return;
	}

	_map_options_dialog->show(this);
	bool canceled = _map_options_dialog->canceled();
	if (canceled) { return; }

	if (!_map_options_dialog->map_width() || !_map_options_dialog->map_height()) {
		std::string msg = "Dimensions must be nonzero!";
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return;
	}

	_map.modified(false);
	_map_events.modified(false);
	_metatileset.modified(false);
	close_cb(NULL, this);

	_directory = directory;
	if (filename) {
		_blk_file = filename;
	}
	else {
		_blk_file = "";
		_blk_save_chooser->directory(directory);
	}
	_asm_file = "";

	// read data
	const char *tileset_name = _map_options_dialog->tileset();
	const char *roof_name = _map_options_dialog->roof();
	if (!read_metatile_data(tileset_name, roof_name)) { return; }

	uint8_t w = _map_options_dialog->map_width(), h = _map_options_dialog->map_height();
	_map.size(w, h);
	int ms = metatile_size();

	_map.attributes(attrs);

	const char *basename;

	// populate map with blocks
	if (filename) {
		basename = fl_filename_name(_blk_file.c_str());
		Map::Result r = _map.read_blocks(filename);
		if (r == Map::Result::MAP_TOO_LONG) {
			std::string msg = "Warning: ";
			msg = msg + basename + ":\n\n" + Map::error_message(r);
			_warning_dialog->message(msg);
			_warning_dialog->show(this);
		}
		else if (r) {
			_map.clear();
			_metatileset.clear();
			std::string msg = "Error reading ";
			msg = msg + basename + "!\n\n" + Map::error_message(r);
			_error_dialog->message(msg);
			_error_dialog->show(this);
			return;
		}
	}
	else {
		basename = NEW_MAP_NAME;
		_map.modified(true);
		for (uint8_t row = 0; row < h; row++) {
			for (uint8_t col = 0; col < w; col++) {
				_map.block(col, row, new Block(row, col, 0x00));
			}
		}
	}

	_map_group->remove(_gameboy_screen);
	_map_group->size(ms * (int)w, ms * (int)h);
	for (uint8_t y = 0; y < h; y++) {
		for (uint8_t x = 0; x < w; x++) {
			Block *block = _map.block(x, y);
			block->callback((Fl_Callback *)change_block_cb, this);
			_map_group->add(block);
		}
	}
	_map_group->add(_gameboy_screen);
	_map.resize_blocks(_map_group->x(), _map_group->y(), ms);
	_map_scroll->scroll_to(0, 0);
	_map_scroll->init_sizes();
	_map_scroll->contents(_map_group->w(), _map_group->h());

	// set filenames
	char buffer[FL_PATH_MAX] = {};
	sprintf(buffer, PROGRAM_NAME " - %s", basename);
	copy_label(buffer);
	if (ends_with(basename, ".blk")) {
		sprintf(buffer, "%s", basename);
		size_t n = strlen(buffer);
		buffer[n-4] = '\0';
		strcat(buffer, ".png");
	}
	else {
		sprintf(buffer, "%s.png", basename);
	}
	_png_chooser->preset_file(buffer);

	// populate sidebar with metatile buttons
	_sidebar->scroll_to(0, 0);
	size_t n = _metatileset.size();
	for (size_t i = 0; i < n; i++) {
		int x = ms * (i % METATILES_PER_ROW), y = ms * (i / METATILES_PER_ROW);
		Metatile_Button *mtb = new Metatile_Button(_sidebar->x() + x, _sidebar->y() + y, ms, (uint8_t)i);
		mtb->callback((Fl_Callback *)select_metatile_cb, this);
		_sidebar->add(mtb);
		_metatile_buttons[i] = mtb;
	}
	_sidebar->init_sizes();
	_sidebar->contents(ms * METATILES_PER_ROW, ms * (((int)n + METATILES_PER_ROW - 1) / METATILES_PER_ROW));

	if (n) {
		_metatile_buttons[0]->setonly();
		_selected = _metatile_buttons[0];
	}
	_copied = false;

	Tileset *tileset = _metatileset.tileset();
	_block_window->tileset(tileset);
	_tileset_window->tileset(tileset);
	_roof_window->tileset(tileset);

	// load default palettes
	Config::bg_tiles_pal_path(buffer, directory);
	load_lighting(buffer);

	if (auto_load_special_lighting()) {
		// load unique tileset palettes if they exist
		sprintf(buffer, "%s%s%s.pal", directory, Config::gfx_tileset_dir(), tileset_name);
		if (file_exists(buffer)) {
			load_lighting(buffer);
		}
		// load unique landmark palettes if they exist
		if (_map.landmark() != tileset_name) {
			sprintf(buffer, "%s%s%s.pal", directory, Config::gfx_tileset_dir(), _map.landmark().c_str());
			if (file_exists(buffer)) {
				load_lighting(buffer);
			}
		}
		// load unique map palettes if they exist
		if (filename) {
			strcpy(buffer, filename);
			fl_filename_setext(buffer, FL_PATH_MAX, ".pal");
			if (file_exists(buffer)) {
				load_lighting(buffer);
			}
		}
	}

	// use lighting coresponding to palette
	Lighting new_lighting = lighting();
	if (new_lighting != Lighting::CUSTOM) {
		if (_map.palette() == "PALETTE_NITE" || _map.palette() == "PALETTE_DARK" ||
			_map.palette() == "2" || _map.palette() == "4") {
			new_lighting = Lighting::NITE;
		}
		else if (_map.environment() == "INDOOR" || _map.environment() == "GATE" ||
			_map.environment() == "3" || _map.environment() == "6") {
			new_lighting = Lighting::INDOOR;
		}
		else if (_map.palette() == "PALETTE_MORN" || _map.palette() == "3") {
			new_lighting = Lighting::MORN;
		}
		else {
			new_lighting = Lighting::DAY;
		}
	}
	if (new_lighting != lighting()) {
		_lighting->value(new_lighting);
		lighting_cb(NULL, this);
		update_lighting();
	}

	// load roof palettes if applicable
	if (auto_load_roof_colors() && _map.group() && _map.is_outside()) {
		load_roof_colors(true);
	}

	// load map events if applicable and they exist
	if (auto_load_events() && filename) {
		char map_name[FL_PATH_MAX] = {};
		remove_dot_ext(filename, map_name);
		Config::event_script_path(buffer, directory, map_name);
		if (!file_exists(buffer)) {
			strcpy(buffer, filename);
			fl_filename_setext(buffer, ".asm");
		}
		if (file_exists(buffer)) {
			load_events(buffer);
		}
	}

	update_active_controls();
	update_labels();
	update_status((Block *)NULL);

	redraw();
}

void Main_Window::warp_to_map(Event *e) {
	char destination[FL_PATH_MAX] = {};
	if (!e->warp_map_name(destination)) { return; }

	char filename[FL_PATH_MAX] = {};
	strcpy(filename, _blk_file.c_str());
	strcpy(const_cast<char *>(fl_filename_name(filename)), destination);
	strcat(filename, fl_filename_ext(_blk_file.c_str()));

	const char *basename = fl_filename_name(filename);
	if (!strcmp(filename, _blk_file.c_str())) {
		std::string msg = basename;
		msg = msg + " is already open!";
		_error_dialog->message(msg);
		_error_dialog->show(this);
	}
	else if (file_exists(filename)) {
		if (unsaved()) {
			std::string msg = modified_filename();
			msg = msg + " has unsaved changes!\n\n"
				"Warp to " + basename + " anyway?";
			_unsaved_dialog->message(msg);
			_unsaved_dialog->show(this);
			if (_unsaved_dialog->canceled()) { return; }
		}
		open_map(filename);
	}
	else {
		std::string msg = "Could not warp to ";
		msg = msg + basename + "!";
		_error_dialog->message(msg);
		_error_dialog->show(this);
	}
}

void Main_Window::load_events(const char *filename) {
	Map_Events::Result et = _map_events.read_events(filename, monochrome());
	if (et) {
		const char *basename = fl_filename_name(filename);
		std::string msg = "Error reading ";
		msg = msg + basename + "!\n\n" + Map_Events::error_message(et);
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return;
	}

	_map_group->remove(_gameboy_screen);
	_map_events.resize_events(_map_group->x(), _map_group->y(), metatile_size() / 2);
	size_t n = _map_events.size();
	for (size_t i = 0; i < n; i++) {
		Event *event = _map_events.event(i);
		event->callback((Fl_Callback *)change_event_cb, this);
		_map_group->add(event);
	}
	_map_group->add(_gameboy_screen);

	_asm_file = filename;
}

void Main_Window::unload_events() {
	size_t n = _map_events.size();
	for (size_t i = 0; i < n; i++) {
		Event *event = _map_events.event(i);
		_map_group->remove(event);
		delete event;
	}
	_map_events.clear();
	_asm_file = "";
}

void Main_Window::view_event_script(Event *e) {
#ifdef _WIN32
	HWND hwnd = fl_xid(this);
	wchar_t filename[FL_PATH_MAX] = {};
	fl_utf8towc(_asm_file.c_str(), _asm_file.length(), filename, FL_PATH_MAX);

	if (e) {
		wchar_t directory[FL_PATH_MAX] = {}, program[FL_PATH_MAX] = {};
		GetCurrentDirectory(FL_PATH_MAX, directory);
		FindExecutable(filename, directory, program);

		std::wstring program_lower(program);
		std::transform(program_lower.begin(), program_lower.end(), program_lower.begin(), towlower);

		std::wstringstream wss;
		// Notepad2 or Notepad3: /g <#> <filename>
		if (ends_with(program_lower, L"notepad2.exe") || ends_with(program_lower, L"notepad3.exe")) {
			wss << L"/g " << e->line() << L" \"" << filename << L"\"";
		}
		// Notepad++: -n<#> <filename>
		else if (ends_with(program_lower, L"notepad++.exe")) {
			wss << L"-n" << e->line() << L" \"" << filename << L"\"";
		}
		// VS Code: -g <filename>:<#>
		else if (ends_with(program_lower, L"code.exe")) {
			wss << L"-g \"" << filename << L"\":" << e->line();
		}
		// Sublime Text: <filename>:<#>
		else if (ends_with(program_lower, L"subl.exe") || ends_with(program_lower, L"sublime_text.exe")) {
			wss << L"\"" << filename << L"\":" << e->line();
		}

		std::wstring params = wss.str();
		if (!params.empty()) {
			ShellExecute(hwnd, L"open", program, params.c_str(), NULL, SW_SHOW);
			return;
		}
	}

	ShellExecute(hwnd, L"edit", filename, NULL, NULL, SW_SHOW);
#else
	if (fork() == 0) {
		execl("/usr/bin/xdg-open", "xdg-open", _asm_file.c_str(), NULL);
		exit(EXIT_SUCCESS);
	}
#endif
}

void Main_Window::load_lighting(const char *filename) {
	if (_edited_lighting) {
		const char *basename = fl_filename_name(filename);
		std::string msg = "The lighting has been edited!\n\n";
		msg = msg + "Load " + basename + " anyway?";
		_unsaved_dialog->message(msg);
		_unsaved_dialog->show(this);
		if (_unsaved_dialog->canceled()) { return; }
	}

	Lighting new_lighting = Color::read_lighting(filename, lighting());
	_edited_lighting = false;
	if (new_lighting != lighting()) {
		_lighting->value(new_lighting);
		lighting_cb(NULL, this);
	}
	update_lighting();
}

void Main_Window::load_roof_colors(bool quiet) {
	char buffer[FL_PATH_MAX] = {};
	Config::roofs_pal_path(buffer, _directory.c_str());

	if (_edited_lighting) {
		const char *basename = fl_filename_name(buffer);
		std::string msg = "The lighting has been edited!\n\n";
		msg = msg + "Load " + basename + " anyway?";
		_unsaved_dialog->message(msg);
		_unsaved_dialog->show(this);
		if (_unsaved_dialog->canceled()) { return; }
	}

	if (!Color::read_roof_colors(buffer, _map.group())) {
		Config::roofs_pal_path(buffer, "");
		if (quiet) {
			std::string msg = "Warning: Could not read ";
			msg = msg + buffer + "!";
			_warning_dialog->message(msg);
			_warning_dialog->show(this);
		}
		else {
			std::string msg = "Could not read ";
			msg = msg + buffer + "!";
			_error_dialog->message(msg);
			_error_dialog->show(this);
		}
	}
	else {
		update_lighting();
		if (!quiet) {
			std::string msg = "Loaded roof colors for map group ";
			msg = msg + std::to_string(_map.group()) + "!";
			_success_dialog->message(msg);
			_success_dialog->show(this);
		}
	}
}

bool Main_Window::read_metatile_data(const char *tileset_name, const char *roof_name) {
	char buffer[FL_PATH_MAX] = {};

	Tileset *tileset = _metatileset.tileset();
	tileset->name(tileset_name);
	tileset->roof_name(roof_name);

	const char *directory = _directory.c_str();

	Config::palette_map_path(buffer, directory, tileset_name);
	Palette_Map::Result rp = tileset->read_palette_map(buffer);
	if (Config::allow_priority() && !_allow_priority_mi->value()) {
		update_priority_controls();
		redraw();
	}
	if (rp == Palette_Map::Result::PALETTE_TOO_LONG) {
		Config::palette_map_path(buffer, "", tileset_name);
		std::string msg = "Warning: ";
		msg = msg + buffer + ":\n\n" + Palette_Map::error_message(rp);
		_warning_dialog->message(msg);
		_warning_dialog->show(this);
	}
	else if (rp) {
		Config::palette_map_path(buffer, "", tileset_name);
		std::string msg = "Error reading ";
		msg = msg + buffer + "!\n\n" + Palette_Map::error_message(rp);
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return false;
	}

	Config::tileset_path(buffer, directory, tileset_name);
	Tileset::Result rt = tileset->read_graphics(buffer, lighting());
	if (rt) {
		Config::tileset_path(buffer, "", tileset_name);
		std::string msg = "Error reading ";
		msg = msg + buffer + "!\n\n" + Tileset::error_message(rt);
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return false;
	}

	Config::metatileset_path(buffer, directory, tileset_name);
	Metatileset::Result rm = _metatileset.read_metatiles(buffer);
	if (rm == Metatileset::Result::META_TOO_SHORT || rm == Metatileset::Result::META_TOO_LONG) {
		Config::metatileset_path(buffer, "", tileset_name);
		std::string msg = "Warning: ";
		msg = msg + buffer + ":\n\n" + Metatileset::error_message(rm);
		_warning_dialog->message(msg);
		_warning_dialog->show(this);
	}
	else if (rm) {
		Config::metatileset_path(buffer, "", tileset_name);
		std::string msg = "Error reading ";
		msg = msg + buffer + "!\n\n" + Metatileset::error_message(rm);
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return false;
	}

	bool bin_collisions = Config::collisions_path(buffer, directory, tileset_name);
	_metatileset.bin_collisions(bin_collisions);
	rm = _metatileset.read_collisions(buffer);
	_has_collisions = (rm == Metatileset::Result::META_OK);

	if (tileset->has_roof()) {
		Config::roof_path(buffer, directory, roof_name);
		rt = tileset->read_roof_graphics(buffer);
		if (rt) {
			Config::roof_path(buffer, "", roof_name);
			std::string msg = "Error reading ";
			msg = msg + buffer + "!\n\n" + Tileset::error_message(rt);
			_warning_dialog->message(msg);
			_warning_dialog->show(this);
		}
	}

	return true;
}

void Main_Window::add_sub_metatiles(size_t n) {
	size_t s = _metatileset.size();
	if (n == s) { return; }
	_metatileset.size(n);
	force_add_sub_metatiles(s, n);
}

void Main_Window::force_add_sub_metatiles(size_t s, size_t n) {
	int ms = metatile_size();

	if (n > s) {
		// add metatiles
		for (size_t i = (int)s; i < n; i++) {
			int x = ms * (i % METATILES_PER_ROW), y = ms * (i / METATILES_PER_ROW);
			Metatile_Button *mtb = new Metatile_Button(_sidebar->x() + x, _sidebar->y() - _sidebar->yposition() + y, ms, (uint8_t)i);
			mtb->callback((Fl_Callback *)select_metatile_cb, this);
			_sidebar->add(mtb);
			_metatile_buttons[i] = mtb;
		}
	}
	else if (n < s) {
		// remove metatiles
		if (_clipboard.id() >= n) {
			_copied = false;
		}
		for (auto it = _metatile_hotkeys.begin(); it != _metatile_hotkeys.end();) {
			uint8_t id = it->first;
			if (id >= n) {
				int key = it->second;
				_hotkey_metatiles.erase(key);
				_metatile_hotkeys.erase(it++);
			}
			else {
				++it;
			}
		}
		if (_selected->id() >= n) {
			_selected = _metatile_buttons[0];
			_selected->setonly();
			_sidebar->scroll_to(0, 0);
		}
		for (size_t i = n; i < s; i++) {
			_sidebar->remove((int)n);
			_metatile_buttons[i] = NULL;
		}
		int k = ms * ((int)(n - 1) / METATILES_PER_ROW + 1);
		if (_sidebar->yposition() + _sidebar->h() > k) {
			_sidebar->scroll_to(0, MAX(k - _sidebar->h(), 0));
		}
	}

	_sidebar->size(ms * METATILES_PER_ROW + Fl::scrollbar_size(), _sidebar->h());
	_sidebar->init_sizes();
	_sidebar->contents(ms * METATILES_PER_ROW, ms * (((int)_metatileset.size() + METATILES_PER_ROW - 1) / METATILES_PER_ROW));

	Tileset *tileset = _metatileset.tileset();
	_block_window->tileset(tileset);
	_tileset_window->tileset(tileset);
	_roof_window->tileset(tileset);

	update_labels();
	update_status((Block *)NULL);

	redraw();
}

void Main_Window::resize_map(int w, int h) {
	int dw = w - _map.width(), dh = h - _map.height();

	int px, py;
	switch (_resize_dialog->horizontal_anchor()) {
	case Resize_Dialog::Hor_Align::LEFT:
		px = 0;
		break;
	case Resize_Dialog::Hor_Align::RIGHT:
		px = dw;
		break;
	case Resize_Dialog::Hor_Align::CENTER:
	default:
		px = dw / 2;
	}
	switch (_resize_dialog->vertical_anchor()) {
	case Resize_Dialog::Vert_Align::TOP:
		py = 0;
		break;
	case Resize_Dialog::Vert_Align::BOTTOM:
		py = dh;
		break;
	case Resize_Dialog::Vert_Align::MIDDLE:
	default:
		py = dh / 2;
	}

	while (_map_group->children()) {
		_map_group->remove(0);
	}
	int mx = MAX(px, 0), my = MAX(py, 0), mw = MIN(w, _map.width() + px), mh = MIN(h, _map.height() + py);
	for (int y = 0; y < py; y++) {
		for (int x = 0; x < w; x++) {
			_map_group->add(new Block());
		}
	}
	for (int y = my; y < mh; y++) {
		for (int x = 0; x < px; x++) {
			_map_group->add(new Block());
		}
		for (int x = mx; x < mw; x++) {
			_map_group->add(_map.block((uint8_t)(x - px), (uint8_t)(y - py)));
		}
		for (int x = mw; x < w; x++) {
			_map_group->add(new Block());
		}
	}
	for (int y = mh; y < h; y++) {
		for (int x = 0; x < w; x++) {
			_map_group->add(new Block());
		}
	}
	size_t n = _map_events.size();
	for (size_t i = 0; i < n; i++) {
		Event *event = _map_events.event(i);
		_map_group->add(event);
	}
	_map_group->add(_gameboy_screen);

	int ms = metatile_size();
	_map_group->size(ms * (int)w, ms * (int)h);
	_map.size((uint8_t)w, (uint8_t)h);
	int i = 0;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			Block *block = (Block *)_map_group->child(i++);
			block->coords((uint8_t)y, (uint8_t)x);
			block->callback((Fl_Callback *)change_block_cb, this);
			_map.block((uint8_t)x, (uint8_t)y, block);
		}
	}
	_map.resize_blocks(_map_group->x(), _map_group->y(), ms);

	_map_scroll->scroll_to(0, 0);
	_map_scroll->init_sizes();
	_map_events.resize_events(_map_group->x(), _map_group->y(), ms / 2);
	_map_scroll->contents(_map_group->w(), _map_group->h());

	_map.modified(true);
	redraw();
}

bool Main_Window::save_map(bool force) {
	const char *filename = _blk_file.c_str();
	const char *basename = fl_filename_name(filename);

	if (_map.modified() || force) {
		FILE *file = fl_fopen(filename, "wb");
		if (!file) {
			std::string msg = "Could not write to ";
			msg = msg + basename + "!";
			_error_dialog->message(msg);
			_error_dialog->show(this);
			return false;
		}

		size_t n = _map.size();
		for (size_t i = 0; i < n; i++) {
			uint8_t id = _map.block(i)->id();
			fputc(id, file);
		}
		fclose(file);

		_map.modified(false);
	}

	std::string msg = "Saved ";
	msg = msg + basename + "!";
	_success_dialog->message(msg);
	_success_dialog->show(this);

	return true;
}

bool Main_Window::save_metatileset() {
	const char *directory = _directory.c_str();
	const char *tileset_name = _metatileset.tileset()->name();

	char filename[FL_PATH_MAX] = {};
	Config::metatileset_path(filename, directory, tileset_name);
	const char *basename = fl_filename_name(filename);

	char filename_coll[FL_PATH_MAX] = {};
	Config::collisions_path(filename_coll, directory, tileset_name);
	const char *basename_coll = fl_filename_name(filename_coll);

	if (_metatileset.modified()) {
		if (!_metatileset.write_metatiles(filename)) {
			std::string msg = "Could not write to ";
			msg = msg + basename + "!";
			_error_dialog->message(msg);
			_error_dialog->show(this);
			return false;
		}

		if (_has_collisions && !_metatileset.write_collisions(filename_coll)) {
			std::string msg = "Could not write to ";
			msg = msg + basename_coll + "!";
			_error_dialog->message(msg);
			_error_dialog->show(this);
		}

		_metatileset.modified(false);
	}

	std::string msg = "Saved ";
	msg = msg + basename;
	if (_has_collisions) {
		msg = msg + "\nand " + basename_coll;
	}
	msg = msg + "!";
	_success_dialog->message(msg);
	_success_dialog->show(this);

	return true;
}

bool Main_Window::save_tileset() {
	Tileset *tileset = _metatileset.tileset();

	char filename[FL_PATH_MAX] = {};
	const char *directory = _directory.c_str();
	const char *tileset_name = tileset->name();

	if (!tileset->modified()) {
		std::string msg = "Saved ";
		msg = msg + tileset_name + "!";
		_success_dialog->message(msg);
		_success_dialog->show(this);
		return true;
	}

	Config::tileset_png_path(filename, directory, tileset_name);
	const char *basename = fl_filename_name(filename);

	if (!tileset->write_graphics(filename)) {
		std::string msg = "Could not write to ";
		msg = msg + basename + "!";
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return false;
	}

	std::string msg = "Saved ";
	msg = msg + basename + "!";
	_success_dialog->message(msg);
	_success_dialog->show(this);

	if (!Config::monochrome()) {
		Config::palette_map_path(filename, directory, tileset_name);
		const char *basename = fl_filename_name(filename);

		if (!tileset->palette_map().write_palette_map(filename)) {
			std::string msg = "Could not write to ";
			msg = msg + basename + "!";
			_error_dialog->message(msg);
			_error_dialog->show(this);
			return false;
		}

		std::string msg = "Saved ";
		msg = msg + basename + "!";
		_success_dialog->message(msg);
		_success_dialog->show(this);
	}

	tileset->modified(false);
	return true;
}

bool Main_Window::save_roof() {
	Tileset *tileset = _metatileset.tileset();

	char filename[FL_PATH_MAX] = {};
	const char *directory = _directory.c_str();
	const char *roof_name = tileset->roof_name();

	if (!tileset->modified_roof()) {
		std::string msg = "Saved ";
		msg = msg + roof_name + "!";
		_success_dialog->message(msg);
		_success_dialog->show(this);
		return true;
	}

	Config::roof_png_path(filename, directory, roof_name);
	const char *basename = fl_filename_name(filename);

	if (!tileset->write_roof_graphics(filename)) {
		std::string msg = "Could not write to ";
		msg = msg + basename + "!";
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return false;
	}

	std::string msg = "Saved ";
	msg = msg + basename + "!";
	_success_dialog->message(msg);
	_success_dialog->show(this);

	tileset->modified_roof(false);
	return true;
}

bool Main_Window::save_event_script() {
	const char *filename = _asm_file.c_str();
	const char *basename = fl_filename_name(filename);

	if (!_map_events.modified()) {
		std::string msg = "Saved ";
		msg = msg + basename + "!";
		_success_dialog->message(msg);
		_success_dialog->show(this);
		return true;
	}

	if (!_map_events.write_event_script(filename)) {
		std::string msg = "Could not write to ";
		msg = msg + basename + "!";
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return false;
	}

	std::string msg = "Saved ";
	msg = msg + basename + "!";
	_success_dialog->message(msg);
	_success_dialog->show(this);

	_map_events.modified(false);
	return true;
}

bool Main_Window::export_lighting(const char *filename, Lighting l) {
	const char *basename = fl_filename_name(filename);

	if (!Color::write_lighting(filename, l)) {
		std::string msg = "Could not write to ";
		msg = msg + basename + "!";
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return false;
	}

	std::string msg = "Exported ";
	msg = msg + basename + "!";
	_success_dialog->message(msg);
	_success_dialog->show(this);

	_edited_lighting = false;
	return true;
}

void Main_Window::edit_metatile(Metatile *mt) {
	for (int y = 0; y < METATILE_SIZE; y++) {
		for (int x = 0; x < METATILE_SIZE; x++) {
			uint8_t id = _block_window->tile_id(x, y);
			mt->tile_id(x, y, id);
			for (int i = 0; i < NUM_QUADRANTS; i++) {
				Quadrant q = (Quadrant)i;
				const char *c = _block_window->collision(q);
				mt->collision(q, c);
				uint8_t b = _block_window->bin_collision(q);
				mt->bin_collision(q, b);
			}
		}
	}
	_metatileset.modified(true);
	redraw();
}

void Main_Window::update_zoom() {
	// FIX: resizing window after this resets sidebar width to original zoom size
	int ms = metatile_size();
	size_t n = _metatileset.size();
	_sidebar->init_sizes();
	_sidebar->size(ms * METATILES_PER_ROW + Fl::scrollbar_size(), _sidebar->h());
	_map_group->init_sizes();
	_map_scroll->init_sizes();
	_map_scroll->resize(_sidebar->w(), _map_scroll->y(), w() - _sidebar->w(), _map_scroll->h());
	_map_group->resize(_sidebar->w(), _map_group->y(), (int)_map.width() * ms, (int)_map.height() * ms);
	_sidebar->contents(ms * METATILES_PER_ROW, ms * (((int)n + METATILES_PER_ROW - 1) / METATILES_PER_ROW));
	_map_scroll->contents(_map_group->w(), _map_group->h());
	int sx = _sidebar->x(), sy = _sidebar->y();
	for (size_t i = 0; i < n; i++) {
		Metatile_Button *mt = _metatile_buttons[i];
		int dx = ms * (i % METATILES_PER_ROW), dy = ms * (i / METATILES_PER_ROW);
		mt->resize(sx + dx, sy + dy, ms + 1, ms + 1);
	}
	int mx = _map_group->x(), my = _map_group->y();
	_map.resize_blocks(mx, my, ms);
	_map_events.resize_events(mx, my, ms / 2);
}

void Main_Window::update_labels() {
	size_t n = _metatileset.size();
	for (size_t i = 0; i < n; i++) {
		_metatile_buttons[i]->id(_metatile_buttons[i]->id());
	}
	n = _map.size();
	for (size_t i = 0; i < n; i++) {
		_map.block(i)->update_label();
	}
	redraw();
}

void Main_Window::update_lighting() {
	Tileset *tileset = _metatileset.tileset();
	tileset->update_lighting(lighting());
	redraw();
}

void Main_Window::select_metatile(Metatile_Button *mb) {
	_selected = mb;
	_selected->setonly();
	uint8_t id = mb->id();
	int ms = metatile_size();
	if (ms * (id / METATILES_PER_ROW) >= _sidebar->yposition() + _sidebar->h() - ms / 2) {
		_sidebar->scroll_to(0, ms * (id / METATILES_PER_ROW + 1) - _sidebar->h());
		_sidebar->redraw();
	}
	else if (ms * (id / METATILES_PER_ROW + 1) <= _sidebar->yposition() + ms / 2) {
		_sidebar->scroll_to(0, ms * (id / METATILES_PER_ROW));
		_sidebar->redraw();
	}
}

void Main_Window::drag_and_drop_cb(DnD_Receiver *dndr, Main_Window *mw) {
	Fl_Window *top = Fl::modal();
	if (top && top != mw) { return; }
	std::string filename = dndr->text().substr(0, dndr->text().find('\n'));
	mw->open_map(filename.c_str());
}

void Main_Window::new_cb(Fl_Widget *, Main_Window *mw) {
	if (mw->unsaved()) {
		std::string msg = mw->modified_filename();
		msg = msg + " has unsaved changes!\n\n"
			"Create a new map anyway?";
		mw->_unsaved_dialog->message(msg);
		mw->_unsaved_dialog->show(mw);
		if (mw->_unsaved_dialog->canceled()) { return; }
	}

	char directory[FL_PATH_MAX] = {};

	if (!mw->_map.size()) {
		int status = mw->_new_dir_chooser->show();
		if (status == 1) { return; }
		if (status == -1) {
			std::string msg = "Could not get project directory!";
			mw->_error_dialog->message(msg);
			mw->_error_dialog->show(mw);
			return;
		}

		const char *project_dir = mw->_new_dir_chooser->filename();
		strcpy(directory, project_dir);
		strcat(directory, DIR_SEP);
	}
	else {
		strcpy(directory, mw->_directory.c_str());
	}

	mw->open_map(directory, NULL);
}

void Main_Window::open_cb(Fl_Widget *, Main_Window *mw) {
	if (mw->unsaved()) {
		std::string msg = mw->modified_filename();
		msg = msg + " has unsaved changes!\n\n"
			"Open another map anyway?";
		mw->_unsaved_dialog->message(msg);
		mw->_unsaved_dialog->show(mw);
		if (mw->_unsaved_dialog->canceled()) { return; }
	}

	int status = mw->_blk_open_chooser->show();
	if (status == 1) { return; }

	const char *filename = mw->_blk_open_chooser->filename();
	const char *basename = fl_filename_name(filename);
	if (status == -1) {
		std::string msg = "Could not open ";
		msg = msg + basename + "!\n\n" + mw->_blk_open_chooser->errmsg();
		mw->_error_dialog->message(msg);
		mw->_error_dialog->show(mw);
		return;
	}

	mw->open_map(filename);
}

void Main_Window::close_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }

	if (mw->unsaved()) {
		std::string msg = mw->modified_filename();
		msg = msg + " has unsaved changes!\n\n"
			"Close it anyway?";
		mw->_unsaved_dialog->message(msg);
		mw->_unsaved_dialog->show(mw);
		if (mw->_unsaved_dialog->canceled()) { return; }
	}

	mw->label(PROGRAM_NAME);
	mw->_sidebar->clear();
	mw->_sidebar->scroll_to(0, 0);
	mw->_sidebar->contents(0, 0);
	FILL(mw->_metatile_buttons, 0, MAX_NUM_METATILES);
	mw->_selected = NULL;
	mw->_copied = false;
	mw->_hotkey_metatiles.clear();
	mw->_metatile_hotkeys.clear();
	mw->_map_group->remove(mw->_gameboy_screen);
	mw->_map_group->clear();
	mw->_map_group->size(0, 0);
	mw->_map_group->add(mw->_gameboy_screen);
	mw->_map.clear();
	mw->_map_events.clear();
	mw->_map_scroll->contents(0, 0);
	mw->init_sizes();
	mw->update_status((Block *)NULL);
	mw->_directory.clear();
	mw->_blk_file.clear();
	mw->_metatileset.clear();
	mw->_block_window->tileset(NULL);
	mw->_tileset_window->tileset(NULL);
	mw->_roof_window->tileset(NULL);

	mw->update_active_controls();
	mw->redraw();
}

void Main_Window::save_cb(Fl_Widget *w, Main_Window *mw) {
	if (!mw->_map.size()) { return; }
	bool other_modified = false;
	if (mw->_metatileset.const_tileset()->modified()) {
		save_tileset_cb(w, mw);
		other_modified = true;
	}
	if (mw->_metatileset.const_tileset()->modified_roof()) {
		save_roof_cb(w, mw);
		other_modified = true;
	}
	if (mw->_metatileset.modified()) {
		save_metatiles_cb(w, mw);
		other_modified = true;
	}
	if (mw->_map_events.modified()) {
		save_event_script_cb(w, mw);
		other_modified = true;
	}
	if (other_modified && !mw->_map.modified()) { return; }
	save_map_cb(w, mw);
}

void Main_Window::save_as_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }

	int status = mw->_blk_save_chooser->show();
	if (status == 1) { return; }

	char filename[FL_PATH_MAX] = {};
	add_dot_ext(mw->_blk_save_chooser->filename(), ".blk", filename);
	const char *basename = fl_filename_name(filename);

	if (status == -1) {
		std::string msg = "Could not open ";
		msg = msg + basename + "!\n\n" + mw->_blk_save_chooser->errmsg();
		mw->_error_dialog->message(msg);
		mw->_error_dialog->show(mw);
		return;
	}

	char directory[FL_PATH_MAX] = {};
	if (!Config::project_path_from_blk_path(filename, directory)) {
		std::string msg = "Could not get project directory for ";
		msg = msg + basename + "!";
		mw->_error_dialog->message(msg);
		mw->_error_dialog->show(mw);
		return;
	}

	mw->_directory = directory;
	mw->_blk_file = filename ? filename : "";

	char buffer[FL_PATH_MAX] = {};
	sprintf(buffer, PROGRAM_NAME " - %s", basename);
	mw->copy_label(buffer);

	strcpy(buffer, basename);
	fl_filename_setext(buffer, FL_PATH_MAX, ".png");
	mw->_png_chooser->preset_file(buffer);

	mw->save_map(true);
}

void Main_Window::save_map_cb(Fl_Widget *w, Main_Window *mw) {
	if (mw->_blk_file.empty()) {
		save_as_cb(w, mw);
	}
	else {
		mw->save_map(false);
	}
}

void Main_Window::save_metatiles_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }
	mw->save_metatileset();
}

void Main_Window::save_tileset_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }
	mw->save_tileset();
}

void Main_Window::save_roof_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size() || !mw->_metatileset.const_tileset()->num_roof_tiles()) { return; }
	mw->save_roof();
}

void Main_Window::load_event_script_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }

	int status = mw->_asm_chooser->show();
	if (status == 1) { return; }

	const char *filename = mw->_asm_chooser->filename();
	const char *basename = fl_filename_name(filename);
	if (status == -1) {
		std::string msg = "Could not open ";
		msg = msg + basename + "!\n\n" + mw->_asm_chooser->errmsg();
		mw->_error_dialog->message(msg);
		mw->_error_dialog->show(mw);
		return;
	}

	if (mw->_map_events.modified()) {
		std::string msg = "The events have been edited!\n\n";
		msg = msg + "Load " + basename + " anyway?";
		mw->_unsaved_dialog->message(msg);
		mw->_unsaved_dialog->show(mw);
		if (mw->_unsaved_dialog->canceled()) { return; }
	}

	mw->unload_events();
	mw->load_events(filename);

	mw->update_active_controls();
	mw->redraw();
}

void Main_Window::view_event_script_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map_events.loaded()) { return; }
	mw->view_event_script(NULL);
}

void Main_Window::save_event_script_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map_events.loaded()) { return; }
	mw->save_event_script();
}

void Main_Window::reload_event_script_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map_events.loaded()) { return; }

	if (mw->_map_events.modified()) {
		const char *basename = fl_filename_name(mw->_asm_file.c_str());
		std::string msg = "The events have been edited!\n\n";
		msg = msg + "Reload " + basename + " anyway?";
		mw->_unsaved_dialog->message(msg);
		mw->_unsaved_dialog->show(mw);
		if (mw->_unsaved_dialog->canceled()) { return; }
	}

	std::string filename(mw->_asm_file);
	mw->unload_events();
	mw->load_events(filename.c_str());

	mw->update_active_controls();
	mw->redraw();
}

void Main_Window::unload_event_script_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map_events.loaded()) { return; }

	if (mw->_map_events.modified()) {
		const char *basename = fl_filename_name(mw->_asm_file.c_str());
		std::string msg = "The events have been edited!\n\n";
		msg = msg + "Unload " + basename + " anyway?";
		mw->_unsaved_dialog->message(msg);
		mw->_unsaved_dialog->show(mw);
		if (mw->_unsaved_dialog->canceled()) { return; }
	}

	mw->unload_events();

	mw->update_active_controls();
	mw->redraw();
}

void Main_Window::load_roof_colors_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }

	if (!mw->_map.group()) {
		std::string msg = "The map's group is unknown!";
		mw->_error_dialog->message(msg);
		mw->_error_dialog->show(mw);
		return;
	}

	mw->load_roof_colors(false);
}

void Main_Window::load_lighting_cb(Fl_Widget *, Main_Window *mw) {
	int status = mw->_pal_load_chooser->show();
	if (status == 1) { return; }

	const char *filename = mw->_pal_load_chooser->filename();
	const char *basename = fl_filename_name(filename);
	if (status == -1) {
		std::string msg = "Could not open ";
		msg = msg + basename + "!\n\n" + mw->_pal_load_chooser->errmsg();
		mw->_error_dialog->message(msg);
		mw->_error_dialog->show(mw);
		return;
	}

	mw->load_lighting(filename);

	mw->update_active_controls();
	mw->redraw();
}

void Main_Window::export_current_lighting_cb(Fl_Widget *, Main_Window *mw) {
	int status = mw->_pal_save_chooser->show();
	if (status == 1) { return; }

	char filename[FL_PATH_MAX] = {};
	add_dot_ext(mw->_pal_save_chooser->filename(), ".pal", filename);
	const char *basename = fl_filename_name(filename);

	if (status == -1) {
		std::string msg = "Could not open ";
		msg = msg + basename + "!\n\n" + mw->_pal_save_chooser->errmsg();
		mw->_error_dialog->message(msg);
		mw->_error_dialog->show(mw);
		return;
	}

	mw->export_lighting(filename, mw->lighting());
}

void Main_Window::print_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }

	int status = mw->_png_chooser->show();
	if (status == 1) { return; }

	char filename[FL_PATH_MAX] = {};
	add_dot_ext(mw->_png_chooser->filename(), ".png", filename);
	const char *basename = fl_filename_name(filename);

	if (status == -1) {
		std::string msg = "Could not print to ";
		msg = msg + basename + "!\n\n" + mw->_png_chooser->errmsg();
		mw->_error_dialog->message(msg);
		mw->_error_dialog->show(mw);
		return;
	}

	Image::Result result = Image::write_map_image(filename, mw->_map, mw->_metatileset);
	if (result) {
		std::string msg = "Could not print to ";
		msg = msg + basename + "!\n\n" + Image::error_message(result);
		mw->_error_dialog->message(msg);
		mw->_error_dialog->show(mw);
	}
	else {
		std::string msg = "Printed ";
		msg = msg + basename + "!";
		mw->_success_dialog->message(msg);
		mw->_success_dialog->show(mw);
	}
}

void Main_Window::exit_cb(Fl_Widget *, Main_Window *mw) {
	// Override default behavior of Esc to close main window
	if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) { return; }

	if (mw->unsaved()) {
		std::string msg = mw->modified_filename();
		msg = msg + " has unsaved changes!\n\n"
			"Exit anyway?";
		mw->_unsaved_dialog->message(msg);
		mw->_unsaved_dialog->show(mw);
		if (mw->_unsaved_dialog->canceled()) { return; }
	}

	/*if (mw->_edited_lighting) {
		std::string msg = "The lighting has been edited!\n\n"
			"Exit anyway?";
		mw->_unsaved_dialog->message(msg);
		mw->_unsaved_dialog->show(mw);
		if (mw->_unsaved_dialog->canceled()) { return; }
	}*/

	// Save global config
	Preferences::set("theme", OS::current_theme());
	Preferences::set("x", mw->x());
	Preferences::set("y", mw->y());
	Preferences::set("w", mw->w());
	Preferences::set("h", mw->h());
	Preferences::set("mode", (int)mw->mode());
	Preferences::set("grid", mw->grid());
	Preferences::set("zoom", mw->zoom());
	Preferences::set("ids", mw->ids());
	Preferences::set("hex", mw->hex());
	Preferences::set("priority", mw->show_priority());
	Preferences::set("event", mw->show_events());
	Preferences::set("gameboy", mw->gameboy_screen());
	Preferences::set("lighting", mw->lighting());
	Preferences::set("monochrome", mw->monochrome());
	Preferences::set("prioritize", mw->allow_priority());
	Preferences::set("all256", mw->allow_256_tiles());
	Preferences::set("events", mw->auto_load_events());
	Preferences::set("special", mw->auto_load_special_lighting());
	Preferences::set("roofs", mw->auto_load_roof_colors());
	if (mw->_resize_dialog->initialized()) {
		Preferences::set("resize-anchor", mw->_resize_dialog->anchor());
	}

	exit(EXIT_SUCCESS);
}

void Main_Window::undo_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }
	mw->_map.undo();
	mw->update_active_controls();
	mw->redraw();
}

void Main_Window::redo_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }
	mw->_map.redo();
	mw->update_active_controls();
	mw->redraw();
}

void Main_Window::copy_metatile_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_selected) { return; }
	uint8_t id = mw->_selected->id();
	Metatile *src = mw->_metatileset.metatile(id);
	mw->_clipboard = *src;
	mw->_copied = true;
	mw->update_active_controls();
	mw->redraw();
}

void Main_Window::paste_metatile_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_copied || !mw->_selected) { return; }
	uint8_t id = mw->_selected->id();
	Metatile *dest = mw->_metatileset.metatile(id);
	dest->copy(&mw->_clipboard);
	mw->_metatileset.modified(true);
	mw->redraw();
}

void Main_Window::swap_metatiles_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_copied || !mw->_selected) { return; }
	uint8_t id1 = mw->_clipboard.id(), id2 = mw->_selected->id();
	Metatile *mt1 = mw->_metatileset.metatile(id1), *mt2 = mw->_metatileset.metatile(id2);
	mt1->swap(mt2);
	mw->_metatileset.modified(true);
	mw->redraw();
}

void Main_Window::aero_theme_cb(Fl_Menu_ *, Main_Window *mw) {
	OS::use_aero_theme();
	mw->_aero_theme_mi->setonly();
	mw->redraw();
}

void Main_Window::metro_theme_cb(Fl_Menu_ *, Main_Window *mw) {
	OS::use_metro_theme();
	mw->_metro_theme_mi->setonly();
	mw->redraw();
}

void Main_Window::greybird_theme_cb(Fl_Menu_ *, Main_Window *mw) {
	OS::use_greybird_theme();
	mw->_greybird_theme_mi->setonly();
	mw->redraw();
}

void Main_Window::blue_theme_cb(Fl_Menu_ *, Main_Window *mw) {
	OS::use_blue_theme();
	mw->_blue_theme_mi->setonly();
	mw->redraw();
}

void Main_Window::dark_theme_cb(Fl_Menu_ *, Main_Window *mw) {
	OS::use_dark_theme();
	mw->_dark_theme_mi->setonly();
	mw->redraw();
}

void Main_Window::full_screen_cb(Fl_Menu_ *m, Main_Window *mw) {
	if (m->mvalue()->value()) {
		mw->_wx = mw->x(); mw->_wy = mw->y();
		mw->_ww = mw->w(); mw->_wh = mw->h();
		mw->fullscreen();
	}
	else {
		mw->fullscreen_off(mw->_wx, mw->_wy, mw->_ww, mw->_wh);
	}
}

#define SYNC_TB_WITH_M(tb, m) tb->value(m->mvalue()->value())

void Main_Window::grid_cb(Fl_Menu_ *m, Main_Window *mw) {
	SYNC_TB_WITH_M(mw->_grid_tb, m);
	mw->redraw();
}

void Main_Window::zoom_cb(Fl_Menu_ *m, Main_Window *mw) {
	SYNC_TB_WITH_M(mw->_zoom_tb, m);
	mw->update_zoom();
	mw->redraw();
}

void Main_Window::ids_cb(Fl_Menu_ *m, Main_Window *mw) {
	SYNC_TB_WITH_M(mw->_ids_tb, m);
	mw->redraw();
}

void Main_Window::hex_cb(Fl_Menu_ *m, Main_Window *mw) {
	SYNC_TB_WITH_M(mw->_hex_tb, m);
	mw->update_labels();
	mw->redraw();
}

void Main_Window::show_priority_cb(Fl_Menu_ *m, Main_Window *mw) {
	SYNC_TB_WITH_M(mw->_show_priority_tb, m);
	mw->update_labels();
	mw->redraw();
}

void Main_Window::show_events_cb(Fl_Menu_ *m, Main_Window *mw) {
	SYNC_TB_WITH_M(mw->_show_events_tb, m);
	mw->update_labels();
	mw->redraw();
}

void Main_Window::gameboy_screen_cb(Fl_Menu_ *m, Main_Window *mw) {
	SYNC_TB_WITH_M(mw->_gameboy_screen_tb, m);
	mw->update_gameboy_screen();
	mw->redraw();
}

#undef SYNC_TB_WITH_M

#define SYNC_MI_WITH_TB(tb, mi) if (tb->value()) mi->set(); else mi->clear()

void Main_Window::grid_tb_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	SYNC_MI_WITH_TB(mw->_grid_tb, mw->_grid_mi);
	mw->redraw();
}

void Main_Window::zoom_tb_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	SYNC_MI_WITH_TB(mw->_zoom_tb, mw->_zoom_mi);
	mw->update_zoom();
	mw->redraw();
}

void Main_Window::ids_tb_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	SYNC_MI_WITH_TB(mw->_ids_tb, mw->_ids_mi);
	mw->redraw();
}

void Main_Window::hex_tb_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	SYNC_MI_WITH_TB(mw->_hex_tb, mw->_hex_mi);
	mw->update_labels();
	mw->redraw();
}

void Main_Window::show_priority_tb_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	SYNC_MI_WITH_TB(mw->_show_priority_tb, mw->_show_priority_mi);
	mw->update_labels();
	mw->redraw();
}

void Main_Window::show_events_tb_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	SYNC_MI_WITH_TB(mw->_show_events_tb, mw->_show_events_mi);
	mw->update_labels();
	mw->redraw();
}

void Main_Window::gameboy_screen_tb_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	SYNC_MI_WITH_TB(mw->_gameboy_screen_tb, mw->_gameboy_screen_mi);
	mw->update_gameboy_screen();
	mw->redraw();
}

#undef SYNC_MI_WITH_TB

void Main_Window::morn_lighting_cb(Fl_Menu_ *, Main_Window *mw) {
	mw->_lighting->value(Lighting::MORN);
	mw->update_lighting();
	mw->redraw();
}

void Main_Window::day_lighting_cb(Fl_Menu_ *, Main_Window *mw) {
	mw->_lighting->value(Lighting::DAY);
	mw->update_lighting();
	mw->redraw();
}

void Main_Window::night_lighting_cb(Fl_Menu_ *, Main_Window *mw) {
	mw->_lighting->value(Lighting::NITE);
	mw->update_lighting();
	mw->redraw();
}

void Main_Window::indoor_lighting_cb(Fl_Menu_ *, Main_Window *mw) {
	mw->_lighting->value(Lighting::INDOOR);
	mw->update_lighting();
	mw->redraw();
}

void Main_Window::custom_lighting_cb(Fl_Menu_ *, Main_Window *mw) {
	mw->_lighting->value(Lighting::CUSTOM);
	mw->update_lighting();
	mw->redraw();
}

void Main_Window::lighting_cb(Dropdown *, Main_Window *mw) {
	Lighting lighting = (Lighting)mw->_lighting->value();
	switch (lighting) {
	case Lighting::MORN:   mw->_morn_mi->setonly(); break;
	case Lighting::DAY:    mw->_day_mi->setonly(); break;
	case Lighting::NITE:   mw->_night_mi->setonly(); break;
	case Lighting::INDOOR: mw->_indoor_mi->setonly(); break;
	case Lighting::CUSTOM: mw->_custom_mi->setonly(); break;
	}
	mw->update_lighting();
	mw->redraw();
}

void Main_Window::blocks_mode_cb(Fl_Menu_ *, Main_Window *mw) {
	mw->_blocks_mode_tb->setonly();
	mw->mode(Mode::BLOCKS);
	mw->redraw();
}

void Main_Window::events_mode_cb(Fl_Menu_ *, Main_Window *mw) {
	mw->_events_mode_tb->setonly();
	mw->mode(Mode::EVENTS);
	mw->redraw();
}

void Main_Window::switch_mode_cb(Fl_Menu_ *, Main_Window *mw) {
	if (mw->mode() == Mode::BLOCKS) {
		mw->_events_mode_mi->setonly();
		mw->_events_mode_tb->setonly();
		mw->mode(Mode::EVENTS);
	}
	else {
		mw->_blocks_mode_mi->setonly();
		mw->_blocks_mode_tb->setonly();
		mw->mode(Mode::BLOCKS);
	}
	mw->update_gameboy_screen();
	mw->redraw();
}

void Main_Window::blocks_mode_tb_cb(Toolbar_Radio_Button *, Main_Window *mw) {
	mw->_blocks_mode_mi->setonly();
	mw->mode(Mode::BLOCKS);
	mw->redraw();
}

void Main_Window::events_mode_tb_cb(Toolbar_Radio_Button *, Main_Window *mw) {
	mw->_events_mode_mi->setonly();
	mw->mode(Mode::EVENTS);
	mw->redraw();
}

void Main_Window::add_sub_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }
	mw->_add_sub_dialog->num_metatiles(mw->_metatileset.size());
	mw->_add_sub_dialog->show(mw);
	if (mw->_add_sub_dialog->canceled()) { return; }
	if (mw->_add_sub_dialog->num_metatiles() != mw->_metatileset.size()) {
		mw->add_sub_metatiles((int)mw->_add_sub_dialog->num_metatiles());
	}
}

void Main_Window::resize_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }
	mw->_resize_dialog->map_size(mw->_map.width(), mw->_map.height());
	mw->_resize_dialog->show(mw);
	if (mw->_resize_dialog->canceled()) { return; }
	if (mw->_resize_dialog->map_width() != mw->_map.width() || mw->_resize_dialog->map_height() != mw->_map.height()) {
		mw->resize_map(mw->_resize_dialog->map_width(), mw->_resize_dialog->map_height());
	}
}

void Main_Window::change_tileset_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }

	const Tileset *tileset = mw->_metatileset.tileset();

	if (mw->_metatileset.modified() || tileset->modified() || tileset->modified_roof()) {
		std::string msg = mw->modified_filename();
		msg = msg + " has unsaved changes!\n\n"
			"Change the tileset anyway?";
		mw->_unsaved_dialog->message(msg);
		mw->_unsaved_dialog->show(mw);
		if (mw->_unsaved_dialog->canceled()) { return; }
	}

	char old_name[FL_PATH_MAX] = {};
	strcpy(old_name, tileset->name());
	size_t old_size = mw->_metatileset.size();

	if (!mw->_tileset_options_dialog->limit_tileset_options(old_name)) {
		const char *basename = fl_filename_name(mw->_blk_file.c_str());
		std::string msg = "This is not a valid project!\n\n";
		msg = msg + "Make sure the Options match\n" + basename + ".";
		mw->_error_dialog->message(msg);
		mw->_error_dialog->show(mw);
		return;
	}

	mw->_tileset_options_dialog->show(mw);
	bool canceled = mw->_tileset_options_dialog->canceled();
	if (canceled) { return; }

	mw->_metatileset.clear();

	const char *tileset_name = mw->_tileset_options_dialog->tileset();
	const char *roof_name = tileset->roof_name();
	if (!mw->read_metatile_data(tileset_name, roof_name)) {
		mw->_map.modified(false);
		mw->_metatileset.modified(false);
		close_cb(NULL, mw);
		return;
	}

	mw->force_add_sub_metatiles(old_size, mw->_metatileset.size());
	mw->_metatileset.modified(false);
	mw->redraw();
}

void Main_Window::edit_tileset_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }

	mw->_tileset_window->tileset(mw->_metatileset.tileset());
	mw->_tileset_window->show(mw, mw->show_priority());
	bool canceled = mw->_tileset_window->canceled();
	if (canceled) { return; }

	mw->_tileset_window->apply_modifications();
	mw->redraw();
}

void Main_Window::change_roof_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }

	Tileset *tileset = mw->_metatileset.tileset();

	if (tileset->modified_roof()) {
		char basename[FL_PATH_MAX] = {};
		Config::roof_png_path(basename, "", tileset->roof_name());
		std::string msg = fl_filename_name(basename);
		msg = msg + " has unsaved changes!\n\n"
			"Change the roof anyway?";
		mw->_unsaved_dialog->message(msg);
		mw->_unsaved_dialog->show(mw);
		if (mw->_unsaved_dialog->canceled()) { return; }
	}

	char old_name[FL_PATH_MAX] = {};
	strcpy(old_name, tileset->roof_name());

	if (!mw->_roof_options_dialog->limit_roof_options(old_name)) {
		const char *basename = fl_filename_name(mw->_blk_file.c_str());
		std::string msg = "This is not a valid project!\n\n";
		msg = msg + "Make sure the Options match\n" + basename + ".";
		mw->_error_dialog->message(msg);
		mw->_error_dialog->show(mw);
		return;
	}

	mw->_roof_options_dialog->show(mw);
	bool canceled = mw->_roof_options_dialog->canceled();
	if (canceled) { return; }

	const char *roof_name = mw->_roof_options_dialog->roof();
	tileset->roof_name(roof_name);
	if (tileset->has_roof()) {
		char filename[FL_PATH_MAX] = {};
		Config::roof_path(filename, mw->_directory.c_str(), roof_name);
		Tileset::Result rt = tileset->read_roof_graphics(filename);
		if (rt) {
			Config::roof_path(filename, "", roof_name);
			std::string msg = "Error reading ";
			msg = msg + filename + "!\n\n" + Tileset::error_message(rt);
			mw->_error_dialog->message(msg);
			mw->_error_dialog->show(mw);
		}
	}
	else {
		tileset->clear_roof_graphics();
	}

	mw->update_active_controls();
	mw->redraw();
}

void Main_Window::edit_roof_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }

	mw->_roof_window->tileset(mw->_metatileset.tileset());
	mw->_roof_window->show(mw);
	bool canceled = mw->_roof_window->canceled();
	if (canceled) { return; }

	mw->_roof_window->apply_modifications();
	mw->redraw();
}

void Main_Window::edit_current_lighting_cb(Fl_Widget *, Main_Window *mw) {
	Abstract_Lighting_Window *alw = mw->monochrome() ? (Abstract_Lighting_Window *)mw->_monochrome_lighting_window
		                                             : (Abstract_Lighting_Window *)mw->_lighting_window;
	alw->current_lighting(mw->lighting());
	alw->show(mw);
	bool canceled = alw->canceled();
	if (canceled) { return; }

	mw->_edited_lighting = true;
	alw->apply_modifications();
	mw->update_lighting();
	mw->redraw();
}

void Main_Window::monochrome_cb(Fl_Menu_ *m, Main_Window *mw) {
	Config::monochrome(!!m->mvalue()->value());
	mw->redraw();
}

void Main_Window::allow_priority_cb(Fl_Menu_ *m, Main_Window *mw) {
	Config::allow_priority(!!m->mvalue()->value());
	mw->update_priority_controls();
	mw->redraw();
}

void Main_Window::allow_256_tiles_cb(Fl_Menu_ *m, Main_Window *mw) {
	Config::allow_256_tiles(!!m->mvalue()->value());
	mw->redraw();
}

void Main_Window::auto_load_events_cb(Fl_Menu_ *m, Main_Window *mw) {
	if (mw->auto_load_events() == !m->mvalue()->value()) {
		mw->redraw();
	}
}

void Main_Window::auto_load_special_lighting_cb(Fl_Menu_ *m, Main_Window *mw) {
	if (mw->auto_load_special_lighting() == !m->mvalue()->value()) {
		mw->redraw();
	}
}

void Main_Window::auto_load_roof_colors_cb(Fl_Menu_ *m, Main_Window *mw) {
	if (mw->auto_load_roof_colors() == !m->mvalue()->value()) {
		mw->redraw();
	}
}

void Main_Window::help_cb(Fl_Widget *, Main_Window *mw) {
	mw->_help_window->show(mw);
}

void Main_Window::about_cb(Fl_Widget *, Main_Window *mw) {
	mw->_about_dialog->show(mw);
}

void Main_Window::select_metatile_cb(Metatile_Button *mb, Main_Window *mw) {
	// Click to select
	mw->_selected = mb;
	if (Fl::event_button() == FL_RIGHT_MOUSE) {
		// Right-click to edit
		Metatile *mt = mw->_metatileset.metatile(mb->id());
		mw->_block_window->metatile(mt, mw->_has_collisions, mw->_metatileset.bin_collisions());
		mw->_block_window->show(mw, mw->show_priority());
		if (!mw->_block_window->canceled()) {
			mw->edit_metatile(mt);
		}
	}
}

void Main_Window::change_block_cb(Block *b, Main_Window *mw) {
	if (!mw->_map_editable || mw->_mode != Mode::BLOCKS) { return; }
	if (Fl::event_button() == FL_LEFT_MOUSE) {
		if (!mw->_selected) { return; }
		if (Fl::event_is_click()) {
			mw->_map.remember();
			mw->update_active_controls();
		}
		if (Fl::event_shift()) {
			// Shift+left-click to flood fill
			mw->flood_fill(b, b->id(), mw->_selected->id());
			mw->_map_group->redraw();
			mw->_map.modified(true);
			mw->update_status(b);
		}
		else if (Fl::event_ctrl()) {
			// Ctrl+left-click to replace
			mw->substitute_block(b->id(), mw->_selected->id());
			mw->_map_group->redraw();
			mw->_map.modified(true);
			mw->update_status(b);
		}
		else {
			// Left-click/drag to edit
			uint8_t id = mw->_selected->id();
			b->id(id);
			b->damage(1);
			mw->_map.modified(true);
			mw->update_status(b);
		}
	}
	else if (Fl::event_button() == FL_RIGHT_MOUSE) {
		// Right-click to select
		uint8_t id = b->id();
		if (id >= mw->_metatileset.size()) { return; }
		mw->select_metatile(mw->_metatile_buttons[id]);
	}
}

void Main_Window::change_event_cb(Event *e, Main_Window *mw) {
	if (mw->_mode != Mode::EVENTS) { return; }
	int sx = mw->_map_scroll->x() - mw->_map_scroll->xposition();
	int sy = mw->_map_scroll->y() - mw->_map_scroll->yposition();
	if (Fl::event_button() == FL_LEFT_MOUSE) {
		if (!Fl::event_is_click()) {
			// Left-drag to move
			uint8_t rx = (uint8_t)((Fl::event_x() - sx) / e->w());
			uint8_t ry = (uint8_t)((Fl::event_y() - sy) / e->h());
			uint8_t ex = MIN(MAX(rx, 0), mw->_map.max_event_x());
			uint8_t ey = MIN(MAX(ry, 0), mw->_map.max_event_y());
			e->coords(ex, ey);
			e->reposition(sx, sy);
			mw->_map_events.modified(true);
			mw->redraw_map();
		}
		else if (Fl::event_shift()) {
			// Shift+click to open a warp's map
			mw->warp_to_map(e);
		}
		else if (Fl::event_clicks()) {
			// Double-click to view .asm file
			mw->view_event_script(e);
		}
	}
	else if (Fl::event_button() == FL_RIGHT_MOUSE && Fl::event_is_click()) {
		// Right-click to edit
		mw->_event_options_dialog->use_event(e);
		mw->_event_options_dialog->limit_event_coords(mw->_map.max_event_x(), mw->_map.max_event_y());
		mw->_event_options_dialog->show(mw);
		if (!mw->_event_options_dialog->canceled()) {
			mw->_event_options_dialog->update_event(e);
			e->reposition(sx, sy);
			mw->_map_events.modified(true);
			mw->redraw();
		}
	}
}
