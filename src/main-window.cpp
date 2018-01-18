#include <cstdlib>
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
#include "config.h"
#include "main-window.h"
#include "image.h"
#include "icons.h"

#ifdef _WIN32
#include "resource.h"
#else
#include <X11/xpm.h>
#include "app-icon.xpm"
#endif

Main_Window::Main_Window(int x, int y, int w, int h, const char *) : Fl_Double_Window(x, y, w, h, PROGRAM_NAME),
	_grid_mi(NULL), _zoom_mi(NULL), _ids_mi(NULL), _hex_mi(NULL), _event_cursor_mi(NULL), _monochrome_mi(NULL),
	_skip_60_to_7f_mi(NULL), _tile_priority_mi(NULL), _directory(), _blk_file(), _metatileset(), _map(), _metatile_buttons(),
	_selected(NULL), _unsaved(false), _copied(false), _clipboard(0), _wx(x), _wy(y), _ww(w), _wh(h) {
	// Get global configs
	int grid_config = Config::get("grid", 1);
	int zoom_config = Config::get("zoom", 0);
	int ids_config = Config::get("ids", 0);
	int hex_config = Config::get("hex", 0);
	int event_cursor_config = Config::get("event", 0);
	Lighting lighting_config = (Lighting)Config::get("lighting", Lighting::DAY);

	int monochrome_config = Config::get("monochrome", 0);
	int skip_60_to_7f_config = Config::get("skip", 1);
	int tile_priority_config = Config::get("priority", 0);
	Config::monochrome(monochrome_config);
	Config::skip_tiles_60_to_7f(skip_60_to_7f_config);
	Config::tile_priority(tile_priority_config);

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
	_save_tb = new Toolbar_Button(0, 0, 24, 24);
	_print_tb = new Toolbar_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 24); new Spacer(0, 0, 2, 24); new Fl_Box(0, 0, 2, 24);
	_undo_tb = new Toolbar_Button(0, 0, 24, 24);
	_redo_tb = new Toolbar_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 24); new Spacer(0, 0, 2, 24); new Fl_Box(0, 0, 2, 24);
	_add_sub_tb = new Toolbar_Button(0, 0, 24, 24);
	_resize_tb = new Toolbar_Button(0, 0, 24, 24);
	_change_tileset_tb = new Toolbar_Button(0, 0, 24, 24);
	_edit_tileset_tb = new Toolbar_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 24); new Spacer(0, 0, 2, 24); new Fl_Box(0, 0, 2, 24);
	_grid_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_zoom_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_ids_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_hex_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_event_cursor_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 24); new Spacer(0, 0, 2, 24); new Fl_Box(0, 0, 2, 24);
	new Label(0, 0, text_width("Lighting:", 3), 24, "Lighting:");
	_lighting = new Dropdown(0, 0, text_width("Artificial", 3) + 24, 22);
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
	_map_group->end();
	begin();

	// Dialogs
	_new_dir_chooser = new Directory_Chooser(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
	_blk_open_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_FILE);
	_blk_save_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	_png_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	_error_dialog = new Modal_Dialog(this, "Error", Modal_Dialog::ERROR_ICON);
	_warning_dialog = new Modal_Dialog(this, "Warning", Modal_Dialog::WARNING_ICON);
	_success_dialog = new Modal_Dialog(this, "Success", Modal_Dialog::SUCCESS_ICON);
	_unsaved_dialog = new Modal_Dialog(this, "Warning", Modal_Dialog::WARNING_ICON, true);
	_about_dialog = new Modal_Dialog(this, "About " PROGRAM_NAME, Modal_Dialog::APP_ICON);
	_map_options_dialog = new Map_Options_Dialog("Map Options");
	_tileset_options_dialog = new Tileset_Options_Dialog("Change Tileset", _map_options_dialog);
	_resize_dialog = new Resize_Dialog("Resize Map");
	_add_sub_dialog = new Add_Sub_Dialog("Resize Blockset");
	_help_window = new Help_Window(48, 48, 500, 400, PROGRAM_NAME " Help");
	_block_window = new Block_Window(48, 48);
	_tileset_window = new Tileset_Window(48, 48);

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

	// Configure menu bar
	_menu_bar->box(OS_PANEL_THIN_UP_BOX);
	_menu_bar->down_box(FL_FLAT_BOX);

	// Configure menu bar items
	Fl_Menu_Item menu_items[] = {
		// label, shortcut, callback, data, flags
		OS_SUBMENU("&File"),
		OS_MENU_ITEM("&New...", FL_COMMAND + 'n', (Fl_Callback *)new_cb, this, 0),
		OS_MENU_ITEM("&Open...", FL_COMMAND + 'o', (Fl_Callback *)open_cb, this, 0),
		OS_MENU_ITEM("&Save", FL_COMMAND + 's', (Fl_Callback *)save_cb, this, 0),
		OS_MENU_ITEM("&Save As...", FL_COMMAND + 'S', (Fl_Callback *)save_as_cb, this, 0),
		OS_MENU_ITEM("Save &Blockset", FL_COMMAND + 'm', (Fl_Callback *)save_metatiles_cb, this, 0),
		OS_MENU_ITEM("Save &Tileset", FL_COMMAND + 'T', (Fl_Callback *)save_tileset_cb, this, 0),
		OS_MENU_ITEM("&Close", FL_COMMAND + 'w', (Fl_Callback *)close_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Print...", FL_COMMAND + 'p', (Fl_Callback *)print_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("E&xit", FL_ALT + FL_F + 4, (Fl_Callback *)exit_cb, this, 0),
		{},
		OS_SUBMENU("&Edit"),
		OS_MENU_ITEM("&Undo", FL_COMMAND + 'z', (Fl_Callback *)undo_cb, this, 0),
		OS_MENU_ITEM("&Redo", FL_COMMAND + 'y', (Fl_Callback *)redo_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Copy Block", FL_COMMAND + 'c', (Fl_Callback *)copy_metatile_cb, this, 0),
		OS_MENU_ITEM("&Paste Block", FL_COMMAND + 'v', (Fl_Callback *)paste_metatile_cb, this, 0),
		OS_MENU_ITEM("S&wap Block", FL_COMMAND + 'x', (Fl_Callback *)swap_metatiles_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("Resize &Blockset...", FL_COMMAND + 'b', (Fl_Callback *)add_sub_cb, this, 0),
		OS_MENU_ITEM("Resize &Map...", FL_COMMAND + 'e', (Fl_Callback *)resize_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("Chan&ge Tileset...", FL_COMMAND + 'h', (Fl_Callback *)change_tileset_cb, this, 0),
		OS_MENU_ITEM("Edit &Tileset...", FL_COMMAND + 't', (Fl_Callback *)edit_tileset_cb, this, 0),
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
		OS_MENU_ITEM("&Hexadecimal", FL_COMMAND + 'h', (Fl_Callback *)hex_cb, this,
			FL_MENU_TOGGLE | (hex_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Event Cursor", FL_COMMAND + 'u', (Fl_Callback *)event_cursor_cb, this,
			FL_MENU_TOGGLE | (event_cursor_config ? FL_MENU_VALUE : 0) | FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Lighting", 0, NULL, NULL, FL_SUBMENU | FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Day", 0, (Fl_Callback *)day_lighting_cb, this,
			FL_MENU_RADIO | (lighting_config == Lighting::DAY ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Night", 0, (Fl_Callback *)night_lighting_cb, this,
			FL_MENU_RADIO | (lighting_config == Lighting::NITE ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Indoor", 0, (Fl_Callback *)indoor_lighting_cb, this,
			FL_MENU_RADIO | (lighting_config == Lighting::INDOOR ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("Ice &Path", 0, (Fl_Callback *)ice_path_lighting_cb, this,
			FL_MENU_RADIO | (lighting_config == Lighting::ICE_PATH ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Artificial", 0, (Fl_Callback *)artificial_lighting_cb, this,
			FL_MENU_RADIO | (lighting_config == Lighting::ARTIFICIAL ? FL_MENU_VALUE : 0)),
		{},
		OS_MENU_ITEM("Full &Screen", FL_F + 11, (Fl_Callback *)full_screen_cb, this, FL_MENU_TOGGLE),
		{},
		OS_SUBMENU("&Options"),
		OS_MENU_ITEM("&Monochrome", 0, (Fl_Callback *)monochrome_cb, this,
			FL_MENU_TOGGLE | (monochrome_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Skip Tiles $60 to $7F", 0, (Fl_Callback *)skip_60_to_7f_cb, this,
			FL_MENU_TOGGLE | (skip_60_to_7f_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("Tile &Priority", 0, (Fl_Callback *)tile_priority_cb, this,
			FL_MENU_TOGGLE | (tile_priority_config ? FL_MENU_VALUE : 0)),
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
	_event_cursor_mi = PM_FIND_MENU_ITEM_CB(event_cursor_cb);
	_day_mi = PM_FIND_MENU_ITEM_CB(day_lighting_cb);
	_night_mi = PM_FIND_MENU_ITEM_CB(night_lighting_cb);
	_indoor_mi = PM_FIND_MENU_ITEM_CB(indoor_lighting_cb);
	_ice_path_mi = PM_FIND_MENU_ITEM_CB(ice_path_lighting_cb);
	_artificial_mi = PM_FIND_MENU_ITEM_CB(artificial_lighting_cb);
	_full_screen_mi = PM_FIND_MENU_ITEM_CB(full_screen_cb);
	_monochrome_mi = PM_FIND_MENU_ITEM_CB(monochrome_cb);
	_skip_60_to_7f_mi = PM_FIND_MENU_ITEM_CB(skip_60_to_7f_cb);
	_tile_priority_mi = PM_FIND_MENU_ITEM_CB(tile_priority_cb);
#undef PM_FIND_MENU_ITEM_CB

	// Configure toolbar buttons

	_new_tb->tooltip("New... (Ctrl+N)");
	_new_tb->callback((Fl_Callback *)new_cb, this);
	_new_tb->image(NEW_ICON);
	_new_tb->take_focus();

	_open_tb->tooltip("Open... (Ctrl+O)");
	_open_tb->callback((Fl_Callback *)open_cb, this);
	_open_tb->image(OPEN_ICON);

	_save_tb->tooltip("Save (Ctrl+S)");
	_save_tb->callback((Fl_Callback *)save_cb, this);
	_save_tb->image(SAVE_ICON);

	_print_tb->tooltip("Print (Ctrl+P)");
	_print_tb->callback((Fl_Callback *)print_cb, this);
	_print_tb->image(PRINT_ICON);

	_undo_tb->tooltip("Undo (Ctrl+Z)");
	_undo_tb->callback((Fl_Callback *)undo_cb, this);
	_undo_tb->image(UNDO_ICON);

	_redo_tb->tooltip("Redo (Ctrl+Y)");
	_redo_tb->callback((Fl_Callback *)redo_cb, this);
	_redo_tb->image(REDO_ICON);

	_add_sub_tb->tooltip("Resize Blockset... (Ctrl+B)");
	_add_sub_tb->callback((Fl_Callback *)add_sub_cb, this);
	_add_sub_tb->image(ADD_SUB_ICON);

	_resize_tb->tooltip("Resize Map... (Ctrl+E)");
	_resize_tb->callback((Fl_Callback *)resize_cb, this);
	_resize_tb->image(RESIZE_ICON);

	_change_tileset_tb->tooltip("Change Tileset... (Ctrl+H)");
	_change_tileset_tb->callback((Fl_Callback *)change_tileset_cb, this);
	_change_tileset_tb->image(CHANGE_ICON);

	_edit_tileset_tb->tooltip("Edit Tileset... (Ctrl+T)");
	_edit_tileset_tb->callback((Fl_Callback *)edit_tileset_cb, this);
	_edit_tileset_tb->image(TILESET_ICON);

	_grid_tb->tooltip("Grid (Ctrl+G)");
	_grid_tb->callback((Fl_Callback *)grid_tb_cb, this);
	_grid_tb->image(GRID_ICON);
	_grid_tb->value(grid());

	_zoom_tb->tooltip("Zoom (Ctrl+=)");
	_zoom_tb->callback((Fl_Callback *)zoom_tb_cb, this);
	_zoom_tb->image(ZOOM_ICON);
	_zoom_tb->value(zoom());

	_ids_tb->tooltip("Block IDs (Ctrl+I)");
	_ids_tb->callback((Fl_Callback *)ids_tb_cb, this);
	_ids_tb->image(IDS_ICON);
	_ids_tb->value(ids());

	_hex_tb->tooltip("Hexadecimal (Ctrl+H)");
	_hex_tb->callback((Fl_Callback *)hex_tb_cb, this);
	_hex_tb->image(HEX_ICON);
	_hex_tb->value(hex());

	_event_cursor_tb->tooltip("Event Cursor (Ctrl+U)");
	_event_cursor_tb->callback((Fl_Callback *)event_cursor_tb_cb, this);
	_event_cursor_tb->image(EVENT_ICON);
	_event_cursor_tb->value(event_cursor());

	_lighting->add("Day");        // Lighting::DAY
	_lighting->add("Night");      // Lighting::NITE
	_lighting->add("Indoor");     // Lighting::INDOOR
	_lighting->add("Ice Path");   // Lighting::ICE_PATH
	_lighting->add("Artificial"); // Lighting::ARTIFICIAL
	_lighting->value(lighting_config);
	_lighting->callback((Fl_Callback *)lighting_cb, this);

	// Configure dialogs

	_blk_open_chooser->title("Open Map");
	_blk_open_chooser->filter("BLK Files\t*.blk\n");

	_blk_save_chooser->title("Save Map");
	_blk_save_chooser->filter("BLK Files\t*.blk\n");
	_blk_save_chooser->preset_file("NewMap.blk");

	_new_dir_chooser->title("Choose Project Directory");

	_png_chooser->title("Print Screenshot");
	_png_chooser->filter("PNG Files\t*.png\n");
	_png_chooser->preset_file("screenshot.png");

	_error_dialog->width_range(280, 700);
	_warning_dialog->width_range(280, 700);
	_success_dialog->width_range(280, 700);
	_unsaved_dialog->width_range(280, 700);

	std::string subject(PROGRAM_NAME " " PROGRAM_VERSION_STRING), message(
		"Copyright \xc2\xa9 2017 Rangi.\n"
		"\n"
		"Source code is available at:\n"
		"https://github.com/roukaour/polished-map\n"
		"\n"
		"Some icons by Yusuke Kamiyamane."
	);
	_about_dialog->subject(subject);
	_about_dialog->message(message);
	_about_dialog->width_range(280, 700);

	_help_window->content(
#include "help.html" // a C++11 raw string literal
		);
}

Main_Window::~Main_Window() {
	delete _menu_bar; // includes menu items
	delete _toolbar; // includes toolbar buttons
	delete _sidebar; // includes metatiles
	delete _status_bar; // includes status bar fields
	delete _map_scroll; // includes map and blocks
	delete _dnd_receiver;
	delete _blk_open_chooser;
	delete _blk_save_chooser;
	delete _png_chooser;
	delete _error_dialog;
	delete _warning_dialog;
	delete _success_dialog;
	delete _unsaved_dialog;
	delete _about_dialog;
	delete _map_options_dialog;
	delete _tileset_options_dialog;
	delete _resize_dialog;
	delete _add_sub_dialog;
	delete _help_window;
	delete _block_window;
	delete _tileset_window;
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

const char *Main_Window::modified_filename() {
	if (_map.modified()) {
		if (_blk_file.empty()) { return NEW_MAP_NAME; }
		return fl_filename_name(_blk_file.c_str());
	}
	static char buffer[FL_PATH_MAX] = {};
	const Tileset *tileset = _metatileset.const_tileset();
	if (tileset->modified()) {
		Config::tileset_path(buffer, _directory.c_str(), tileset->name());
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

void Main_Window::draw_metatile(int x, int y, uint8_t id) const {
	_metatileset.draw_metatile(x, y, id, zoom());
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
	if (!event_cursor() || !b) {
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
	if (!_map_options_dialog->limit_blk_options(filename, directory)) {
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

	// read data
	const char *tileset_name = _map_options_dialog->tileset();
	if (!read_metatile_data(tileset_name)) { return; }

	uint8_t w = _map_options_dialog->map_width(), h = _map_options_dialog->map_height();
	_map.size(w, h);
	int ms = metatile_size();

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
			std::string msg = "Error reading ";
			msg = msg + basename + "!\n\n" + Map::error_message(r);
			_error_dialog->message(msg);
			_error_dialog->show(this);
			return;
		}

		// map from file
		_map_group->size(ms * (int)w, ms * (int)h);
		for (uint8_t y = 0; y < h; y++) {
			for (uint8_t x = 0; x < w; x++) {
				Block *block = _map.block(x, y);
				block->resize(_map_group->x() + x * ms, _map_group->y() + y * ms, ms, ms);
				block->callback((Fl_Callback *)change_block_cb, this);
				_map_group->add(block);
			}
		}
	}
	else {
		basename = NEW_MAP_NAME;
		_map.modified(true);

		// new map
		_map_group->size(ms * (int)w, ms * (int)h);
		for (uint8_t row = 0; row < h; row++) {
			for (uint8_t col = 0; col < w; col++) {
				int x = col * ms, y = row * ms;
				Block *block = new Block(_map_group->x() + x, _map_group->y() + y, ms, row, col, 0);
				block->callback((Fl_Callback *)change_block_cb, this);
				_map_group->add(block);
				_map.block(col, row, block);
			}
		}
	}
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

	update_labels();
	update_status(NULL);

	redraw();
}

bool Main_Window::read_metatile_data(const char *tileset_name) {
	char buffer[FL_PATH_MAX] = {};

	Tileset *tileset = _metatileset.tileset();
	tileset->name(tileset_name);

	const char *directory = _directory.c_str();

	Config::palette_map_path(buffer, directory, tileset_name);
	Palette_Map::Result rp = tileset->read_palette_map(buffer);
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

	update_labels();
	update_status(NULL);

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

	int ms = metatile_size();
	_map_group->size(ms * (int)w, ms * (int)h);
	_map.size((uint8_t)w, (uint8_t)h);
	int i = 0;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			Block *block = (Block *)_map_group->child(i++);
			block->coords((uint8_t)y, (uint8_t)x);
			block->resize(_map_group->x() + x * ms, _map_group->y() + y * ms, ms, ms);
			block->callback((Fl_Callback *)change_block_cb, this);
			_map.block((uint8_t)x, (uint8_t)y, block);
		}
	}

	_map_scroll->scroll_to(0, 0);
	_map_scroll->init_sizes();
	_map_scroll->contents(_map_group->w(), _map_group->h());

	_map.modified(true);
	redraw();
}

bool Main_Window::save_map(bool force) {
	const char *filename = _blk_file.c_str();

	if (_map.modified() || force) {
		FILE *file = fl_fopen(filename, "wb");
		if (!file) {
			const char *basename = fl_filename_name(filename);
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

	const char *basename = fl_filename_name(filename);
	std::string msg = "Saved ";
	msg = msg + basename + "!";
	_success_dialog->message(msg);
	_success_dialog->show(this);

	return true;
}

bool Main_Window::save_metatileset() {
	char filename[FL_PATH_MAX] = {};
	const char *directory = _directory.c_str();
	const char *tileset_name = _metatileset.tileset()->name();
	Config::metatileset_path(filename, directory, tileset_name);

	if (_metatileset.modified()) {
		if (!_metatileset.write_metatiles(filename)) {
			const char *basename = fl_filename_name(filename);
			std::string msg = "Could not write to ";
			msg = msg + basename + "!";
			_error_dialog->message(msg);
			_error_dialog->show(this);
			return false;
		}
		_metatileset.modified(false);
	}

	const char *basename = fl_filename_name(filename);
	std::string msg = "Saved ";
	msg = msg + basename + "!";
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
	if (!tileset->write_graphics(filename)) {
		const char *basename = fl_filename_name(filename);
		std::string msg = "Could not write to ";
		msg = msg + basename + "!";
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return false;
	}

	const char *basename = fl_filename_name(filename);
	std::string msg = "Saved ";
	msg = msg + basename + "!";
	_success_dialog->message(msg);
	_success_dialog->show(this);

	if (!Config::monochrome()) {
		Config::palette_map_path(filename, directory, tileset_name);
		if (!tileset->palette_map().write_palette_map(filename)) {
			const char *basename = fl_filename_name(filename);
			std::string msg = "Could not write to ";
			msg = msg + basename + "!";
			_error_dialog->message(msg);
			_error_dialog->show(this);
			return false;
		}

		const char *basename = fl_filename_name(filename);
		std::string msg = "Saved ";
		msg = msg + basename + "!";
		_success_dialog->message(msg);
		_success_dialog->show(this);
	}

	tileset->modified(false);
	return true;
}

void Main_Window::edit_metatile(Metatile *mt) {
	for (int y = 0; y < METATILE_SIZE; y++) {
		for (int x = 0; x < METATILE_SIZE; x++) {
			uint8_t id = _block_window->tile_id(x, y);
			mt->tile_id(x, y, id);
		}
	}
	_metatileset.modified(true);
	redraw();
}

void Main_Window::update_zoom() {
	// TODO: FIX: resizing window after this resets sidebar width to original zoom size
	int ms = metatile_size();
	size_t n = _metatileset.size();
	_sidebar->size(ms * METATILES_PER_ROW + Fl::scrollbar_size(), _sidebar->h());
	_map_scroll->resize(_sidebar->w(), _map_scroll->y(), w() - _sidebar->w(), _map_scroll->h());
	_map_group->resize(_sidebar->w(), _map_group->y(), (int)_map.width() * ms, (int)_map.height() * ms);
	_sidebar->init_sizes();
	_sidebar->contents(ms * METATILES_PER_ROW, ms * (((int)n + METATILES_PER_ROW - 1) / METATILES_PER_ROW));
	_map_group->init_sizes();
	_map_scroll->init_sizes();
	_map_scroll->contents(_map_group->w(), _map_group->h());
	int sx = _sidebar->x(), sy = _sidebar->y();
	for (size_t i = 0; i < n; i++) {
		Metatile_Button *mt = _metatile_buttons[i];
		int dx = ms * (i % METATILES_PER_ROW), dy = ms * (i / METATILES_PER_ROW);
		mt->resize(sx + dx, sy + dy, ms + 1, ms + 1);
	}
	int mx = _map_group->x(), my = _map_group->y();
	for (uint8_t row = 0; row < _map.height(); row++) {
		for (uint8_t col = 0; col < _map.width(); col++) {
			Block *block = _map.block(col, row);
			int dx = col * ms, dy = row * ms;
			block->resize(mx + dx, my + dy, ms, ms);
		}
	}
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

	int status = mw->_new_dir_chooser->show();
	if (status == 1) { return; }
	if (status == -1) {
		std::string msg = "Could not get project directory!";
		mw->_error_dialog->message(msg);
		mw->_error_dialog->show(mw);
		return;
	}

	const char *project_dir = mw->_new_dir_chooser->filename();
	char directory[FL_PATH_MAX] = {};
	strcpy(directory, project_dir);
	strcat(directory, DIR_SEP);
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

void Main_Window::save_cb(Fl_Widget *w, Main_Window *mw) {
	if (!mw->_map.size()) { return; }
	bool other_modified = mw->_metatileset.modified() || mw->_metatileset.const_tileset()->modified();
	if (mw->_metatileset.const_tileset()->modified()) {
		save_tileset_cb(w, mw);
	}
	if (mw->_metatileset.modified()) {
		save_metatiles_cb(w, mw);
	}
	if (other_modified && !mw->_map.modified()) { return; }
	if (mw->_blk_file.empty()) {
		save_as_cb(w, mw);
		return;
	}
	mw->save_map(false);
}

void Main_Window::save_as_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }

	int status = mw->_blk_save_chooser->show();
	if (status == 1) { return; }

	const char *filename = mw->_blk_save_chooser->filename();
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
	if (ends_with(basename, ".blk")) {
		sprintf(buffer, "%s", basename);
		size_t n = strlen(buffer);
		buffer[n-4] = '\0';
		strcat(buffer, ".png");
	}
	else {
		sprintf(buffer, "%s.png", basename);
	}
	mw->_png_chooser->preset_file(buffer);

	mw->save_map(true);
}

void Main_Window::save_metatiles_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }
	mw->save_metatileset();
}

void Main_Window::save_tileset_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }
	mw->save_tileset();
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
	mw->_map_group->clear();
	mw->_map_group->size(0, 0);
	mw->_map.clear();
	mw->_map_scroll->contents(0, 0);
	mw->init_sizes();
	mw->update_status(NULL);
	mw->_directory.clear();
	mw->_blk_file.clear();
	mw->_metatileset.clear();
	mw->_block_window->tileset(NULL);
	mw->_tileset_window->tileset(NULL);
	mw->redraw();
}

void Main_Window::print_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }

	int status = mw->_png_chooser->show();
	if (status == 1) { return; }

	const char *filename = mw->_png_chooser->filename();
	const char *basename = fl_filename_name(filename);

	if (status == -1) {
		std::string msg = "Could not print to ";
		msg = msg + basename + "!\n\n" + mw->_png_chooser->errmsg();
		mw->_error_dialog->message(msg);
		mw->_error_dialog->show(mw);
		return;
	}

	Image::Result result = Image::write_image(filename, mw->_map, mw->_metatileset);
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

	// Save global config
	Config::set("theme", OS::current_theme());
	Config::set("x", mw->x());
	Config::set("y", mw->y());
	Config::set("w", mw->w());
	Config::set("h", mw->h());
	Config::set("grid", mw->grid());
	Config::set("zoom", mw->zoom());
	Config::set("ids", mw->ids());
	Config::set("hex", mw->hex());
	Config::set("event", mw->event_cursor());
	Config::set("lighting", mw->lighting());
	Config::set("monochrome", mw->monochrome());
	Config::set("skip", mw->skip_60_to_7f());
	Config::set("priority", mw->tile_priority());
	if (mw->_resize_dialog->initialized()) {
		Config::set("resize-anchor", mw->_resize_dialog->anchor());
	}

	exit(EXIT_SUCCESS);
}

void Main_Window::undo_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }
	mw->_map.undo();
	mw->redraw();
}

void Main_Window::redo_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }
	mw->_map.redo();
	mw->redraw();
}

void Main_Window::copy_metatile_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_selected) { return; }
	uint8_t id = mw->_selected->id();
	Metatile *src = mw->_metatileset.metatile(id);
	mw->_clipboard = *src;
	mw->_copied = true;
}

void Main_Window::paste_metatile_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_copied || !mw->_selected) { return; }
	uint8_t id = mw->_selected->id();
	Metatile *dest = mw->_metatileset.metatile(id);
	for (int y = 0; y < METATILE_SIZE; y++) {
		for (int x = 0; x < METATILE_SIZE; x++) {
			uint8_t tid = mw->_clipboard.tile_id(x, y);
			dest->tile_id(x, y, tid);
		}
	}
	mw->_metatileset.modified(true);
	mw->redraw();
}

void Main_Window::swap_metatiles_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_copied || !mw->_selected) { return; }
	uint8_t id1 = mw->_clipboard.id(), id2 = mw->_selected->id();
	Metatile *mt1 = mw->_metatileset.metatile(id1), *mt2 = mw->_metatileset.metatile(id2);
	for (int y = 0; y < METATILE_SIZE; y++) {
		for (int x = 0; x < METATILE_SIZE; x++) {
			uint8_t tid1 = mt1->tile_id(x, y), tid2 = mt2->tile_id(x, y);
			mt1->tile_id(x, y, tid2);
			mt2->tile_id(x, y, tid1);
		}
	}
	mw->_metatileset.modified(true);
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

	if (mw->_metatileset.modified() || mw->_metatileset.const_tileset()->modified()) {
		std::string msg = mw->modified_filename();
		msg = msg + " has unsaved changes!\n\n"
			"Change the tileset anyway?";
		mw->_unsaved_dialog->message(msg);
		mw->_unsaved_dialog->show(mw);
		if (mw->_unsaved_dialog->canceled()) { return; }
	}

	const Tileset *tileset = mw->_metatileset.tileset();
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
	if (!mw->read_metatile_data(tileset_name)) {
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
	mw->_tileset_window->show(mw);
	bool canceled = mw->_tileset_window->canceled();
	if (canceled) { return; }

	mw->_tileset_window->apply_modifications();
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

void Main_Window::event_cursor_cb(Fl_Menu_ *m, Main_Window *mw) {
	SYNC_TB_WITH_M(mw->_event_cursor_tb, m);
	mw->update_labels();
	mw->redraw();
}

#undef SYNC_TB_WITH_M

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

void Main_Window::ice_path_lighting_cb(Fl_Menu_ *, Main_Window *mw) {
	mw->_lighting->value(Lighting::ICE_PATH);
	mw->update_lighting();
	mw->redraw();
}

void Main_Window::artificial_lighting_cb(Fl_Menu_ *, Main_Window *mw) {
	mw->_lighting->value(Lighting::ARTIFICIAL);
	mw->update_lighting();
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

void Main_Window::monochrome_cb(Fl_Menu_ *m, Main_Window *mw) {
	Config::monochrome(!!m->mvalue()->value());
	mw->redraw();
}

void Main_Window::skip_60_to_7f_cb(Fl_Menu_ *m, Main_Window *mw) {
	Config::skip_tiles_60_to_7f(!!m->mvalue()->value());
	mw->redraw();
}

void Main_Window::tile_priority_cb(Fl_Menu_ *m, Main_Window *mw) {
	Config::tile_priority(!!m->mvalue()->value());
	mw->redraw();
}

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

void Main_Window::event_cursor_tb_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	SYNC_MI_WITH_TB(mw->_event_cursor_tb, mw->_event_cursor_mi);
	mw->update_labels();
	mw->redraw();
}

#undef SYNC_MI_WITH_TB

void Main_Window::lighting_cb(Dropdown *, Main_Window *mw) {
	Lighting lighting = (Lighting)mw->_lighting->value();
	switch (lighting) {
	case Lighting::DAY:        mw->_day_mi->setonly(); break;
	case Lighting::NITE:       mw->_night_mi->setonly(); break;
	case Lighting::INDOOR:     mw->_indoor_mi->setonly(); break;
	case Lighting::ICE_PATH:   mw->_ice_path_mi->setonly(); break;
	case Lighting::ARTIFICIAL: mw->_artificial_mi->setonly(); break;
	}
	mw->update_lighting();
	mw->redraw();
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
		mw->_block_window->metatile(mt);
		mw->_block_window->show(mw);
		if (!mw->_block_window->canceled()) {
			mw->edit_metatile(mt);
		}
	}
}

void Main_Window::change_block_cb(Block *b, Main_Window *mw) {
	if (Fl::event_button() == FL_LEFT_MOUSE) {
		if (!mw->_selected) { return; }
		if (Fl::event_is_click()) {
			mw->_map.remember();
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
