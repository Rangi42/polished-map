#include <cstdlib>

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
	_grid_mi(NULL), _zoom_mi(NULL), _ids_mi(NULL), _hex_mi(NULL), _prism_mi(NULL),
	_directory(), _blk_file(), _metatileset(), _map(), _metatile_buttons(), _selected(NULL),
	_unsaved(false), _wx(x), _wy(y), _ww(w), _wh(h) {
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
	new Fl_Box(0, 0, 2, 0); new Spacer(0, 0, 2, 0); new Fl_Box(0, 0, 2, 0);
	_undo_tb = new Toolbar_Button(0, 0, 24, 24);
	_redo_tb = new Toolbar_Button(0, 0, 24, 24);
	_add_sub_tb = new Toolbar_Button(0, 0, 24, 24);
	_resize_tb = new Toolbar_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 0); new Spacer(0, 0, 2, 0); new Fl_Box(0, 0, 2, 0);
	_grid_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_zoom_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_ids_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_hex_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_toolbar->end();
	begin();

	// Status bar
	_status_bar = new Toolbar(wx, h-23, w, 23);
	wh -= _status_bar->h();
	_metatile_count = new Status_Bar_Field(0, 0, text_width("Blocks: 9999", 3), 0, "");
	new Spacer(0, 0, 2, 0);
	_map_dimensions = new Status_Bar_Field(0, 0, text_width("Map: 9999 x 9999", 3), 0, "");
	new Spacer(0, 0, 2, 0);
	_hover_x = new Status_Bar_Field(0, 0, text_width("X: $999", 3), 0, "");
	_hover_y = new Status_Bar_Field(0, 0, text_width("Y: $999", 3), 0, "");
	_hover_id = new Status_Bar_Field(0, 0, text_width("ID: $999", 3), 0, "");
	_status_bar->end();
	begin();

	// Sidebar
	_sidebar = new Workspace(wx, wy, metatile_size() * METATILES_PER_ROW + Fl::scrollbar_size(), wh);
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

	// Dialogs
	_blk_open_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_FILE);
	_blk_save_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	_new_dir_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
	_png_chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	_error_dialog = new Modal_Dialog(this, "Error", Modal_Dialog::ERROR_ICON);
	_warning_dialog = new Modal_Dialog(this, "Warning", Modal_Dialog::WARNING_ICON);
	_success_dialog = new Modal_Dialog(this, "Success", Modal_Dialog::SUCCESS_ICON);
	_unsaved_dialog = new Modal_Dialog(this, "Warning", Modal_Dialog::WARNING_ICON, true);
	_about_dialog = new Modal_Dialog(this, "About " PROGRAM_NAME, Modal_Dialog::APP_ICON);
	_map_options_dialog = new Map_Options_Dialog("Map Options");
	_resize_dialog = new Resize_Dialog("Resize Map");
	_add_sub_dialog = new Add_Sub_Dialog("Add/Remove Blocks");
	_help_window = new Help_Window(48, 48, 500, 400, PROGRAM_NAME " Help");
	_block_window = new Block_Window(48, 48);

	// Drag-and-drop receiver
	begin();
	_dnd_receiver = new DnD_Receiver(0, 0, 0, 0);
	end();
	_dnd_receiver->callback((Fl_Callback *)drag_and_drop_cb);
	_dnd_receiver->user_data(this);

	// Get global configs
	int grid_config, zoom_config, ids_config, hex_config, prism_config;
	global_config.get("grid", grid_config, 1);
	global_config.get("zoom", zoom_config, 0);
	global_config.get("ids", ids_config, 0);
	global_config.get("hex", hex_config, 1);
	global_config.get("options-prism", prism_config, 0);

	// Configure window
	size_range(384, 256);
	resizable(_map_scroll);
	callback((Fl_Callback *)exit_cb, this);

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
		OS_MENU_ITEM("Save &Metatiles", FL_COMMAND + 'm', (Fl_Callback *)save_metatiles_cb, this, 0),
		OS_MENU_ITEM("&Close", FL_COMMAND + 'w', (Fl_Callback *)close_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Print...", FL_COMMAND + 'p', (Fl_Callback *)print_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("E&xit", FL_ALT + FL_F + 4, (Fl_Callback *)exit_cb, this, 0),
		{},
		OS_SUBMENU("&Edit"),
		OS_MENU_ITEM("&Undo", FL_COMMAND + 'z', (Fl_Callback *)undo_cb, this, FL_MENU_INACTIVE),
		OS_MENU_ITEM("&Redo", FL_COMMAND + 'y', (Fl_Callback *)redo_cb, this, FL_MENU_DIVIDER | FL_MENU_INACTIVE),
		OS_MENU_ITEM("Add/Remove &Blocks...", FL_COMMAND + 'b', (Fl_Callback *)add_sub_cb, this, 0),
		OS_MENU_ITEM("Re&size Map...", FL_COMMAND + 'e', (Fl_Callback *)resize_cb, this, 0),
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
		OS_MENU_ITEM("&IDs", FL_COMMAND + 'i', (Fl_Callback *)ids_cb, this,
			FL_MENU_TOGGLE | (ids_config ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Hex", FL_COMMAND + 'h', (Fl_Callback *)hex_cb, this,
			FL_MENU_TOGGLE | (hex_config ? FL_MENU_VALUE : 0) | FL_MENU_DIVIDER),
		OS_MENU_ITEM("Full &Screen", FL_F + 11, (Fl_Callback *)full_screen_cb, this, FL_MENU_TOGGLE),
		{},
		OS_SUBMENU("&Options"),
		OS_MENU_ITEM("&Prism Compatibility", 0, (Fl_Callback *)prism_cb, this,
			FL_MENU_TOGGLE | (prism_config ? FL_MENU_VALUE : 0)),
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
	_prism_mi = PM_FIND_MENU_ITEM_CB(prism_cb);
	_full_screen_mi = PM_FIND_MENU_ITEM_CB(full_screen_cb);
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
	_undo_tb->deactivate(); // TODO: implement undo

	_redo_tb->tooltip("Redo (Ctrl+Y)");
	_redo_tb->callback((Fl_Callback *)redo_cb, this);
	_redo_tb->image(REDO_ICON);
	_redo_tb->deactivate(); // TODO: implement redo

	_add_sub_tb->tooltip("Add/Remove Blocks... (Ctrl+B)");
	_add_sub_tb->callback((Fl_Callback *)add_sub_cb, this);
	_add_sub_tb->image(ADD_SUB_ICON);

	_resize_tb->tooltip("Resize Map... (Ctrl+E)");
	_resize_tb->callback((Fl_Callback *)resize_cb, this);
	_resize_tb->image(RESIZE_ICON);

	_grid_tb->tooltip("Grid (Ctrl+G)");
	_grid_tb->callback((Fl_Callback *)grid_tb_cb, this);
	_grid_tb->image(GRID_ICON);
	_grid_tb->value(grid() ? 1 : 0);

	_zoom_tb->tooltip("Zoom (Ctrl+=)");
	_zoom_tb->callback((Fl_Callback *)zoom_tb_cb, this);
	_zoom_tb->image(ZOOM_ICON);
	_zoom_tb->value(zoom() ? 1 : 0);

	_ids_tb->tooltip("IDs (Ctrl+I)");
	_ids_tb->callback((Fl_Callback *)ids_tb_cb, this);
	_ids_tb->image(IDS_ICON);
	_ids_tb->value(ids() ? 1 : 0);

	_hex_tb->tooltip("Hex (Ctrl+H)");
	_hex_tb->callback((Fl_Callback *)hex_tb_cb, this);
	_hex_tb->image(HEX_ICON);
	_hex_tb->value(hex() ? 1 : 0);

	// Configure dialogs

	_blk_open_chooser->title("Open Map");
	_blk_open_chooser->filter("BLK Files\t*.blk\n");

	_blk_save_chooser->title("Save Map");
	_blk_save_chooser->filter("BLK Files\t*.blk\n");
	_blk_save_chooser->preset_file(NEW_BLK_NAME);

	_new_dir_chooser->title("Choose pokecrystal project directory:");

	_png_chooser->title("Print Screenshot");
	_png_chooser->filter("PNG Files\t*.png\n");
	_png_chooser->preset_file("map.png");

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
	delete _resize_dialog;
	delete _add_sub_dialog;
	delete _help_window;
	delete _block_window;
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
		return fl_filename_name(_blk_file.c_str());
	}
	static char buffer[FL_PATH_MAX] = {};
	metatileset_path(buffer, _directory.c_str(), _metatileset.tileset()->name());
	return fl_filename_name(buffer);
}

void Main_Window::draw_metatile(int x, int y, uint8_t id) const {
	_metatileset.draw_metatile(x, y, id, zoom());
}

void Main_Window::update_status(Block *b) {
	if (!_map.size()) {
		_metatile_count->label("");
		_map_dimensions->label("");
		_hover_x->label("");
		_hover_y->label("");
		_hover_id->label("");
		_status_bar->redraw();
		return;
	}
	char buffer[64] = {};
	if (!b) {
#ifdef __GNUC__
		sprintf(buffer, "Blocks: %zu", _metatileset.size());
#else
		sprintf(buffer, "Blocks: %llu", _metatileset.size());
#endif
		_metatile_count->copy_label(buffer);
		sprintf(buffer, "Map: %u x %u", _map.width(), _map.height());
		_map_dimensions->copy_label(buffer);
		_hover_x->label("");
		_hover_y->label("");
		_hover_id->label("");
		_status_bar->redraw();
		return;
	}
	uint8_t row = b->row(), col = b->col(), id = b->id();
	if (hex()) {
		sprintf(buffer, "X: $%X", col);
		_hover_x->copy_label(buffer);
		sprintf(buffer, "Y: $%X", row);
		_hover_y->copy_label(buffer);
		sprintf(buffer, "ID: $%02X", id);
		_hover_id->copy_label(buffer);
	}
	else {
		sprintf(buffer, "X: %u", col);
		_hover_x->copy_label(buffer);
		sprintf(buffer, "Y: %u", row);
		_hover_y->copy_label(buffer);
		sprintf(buffer, "ID: %u", id);
		_hover_id->copy_label(buffer);
	}
	_status_bar->redraw();
}

void Main_Window::flood_fill(Block *b, uint8_t f, uint8_t t) {
	if (f == t || b->id() != f) { return; }
	b->id(t);
	int row = b->row(), col = b->col();
	size_t i = row * _map.width() + col;
	if (col > 0) { flood_fill(_map.block(i-1), f, t); } // left
	if (col < _map.width() - 1) { flood_fill(_map.block(i+1), f, t); } // right
	if (row > 0) { flood_fill(_map.block(i-_map.width()), f, t); } // up
	if (row < _map.height() - 1) { flood_fill(_map.block(i+_map.width()), f, t); } // down
}

void Main_Window::open_map(const char *filename) {
	const char *basename = fl_filename_name(filename);

	char directory[FL_PATH_MAX] = {};
	if (!project_path_from_blk_path(filename, directory, prism())) {
		std::string msg = "Could not get project directory for ";
		msg = msg + basename + "!";
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return;
	}

	open_map(directory, filename);
}

void Main_Window::open_map(const char *directory, const char *filename) {
	// get map options
	if (!_map_options_dialog->limit_blk_options(filename, directory)) {
		std::string msg = "Wrong project directory structure!\n\n"
			"Double-check Options->Prism Compatibility.";
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
		char new_filename[FL_PATH_MAX] = {};
		blk_path_from_project_path(directory, new_filename, prism());
		strcat(new_filename, NEW_BLK_NAME);
		_blk_file = new_filename;
	}

	// read data
	const char *tileset_name = _map_options_dialog->tileset();
	char buffer[FL_PATH_MAX] = {};

	Tileset *tileset = _metatileset.tileset();
	tileset->name(tileset_name);

	palette_map_path(buffer, directory, tileset_name);
	if (Palette_Map::Result r = tileset->read_palette_map(buffer)) {
		std::string msg = "Error reading ";
		palette_map_path(buffer, "", tileset_name);
		msg = msg + buffer + "!\n\n" + Palette_Map::error_message(r);
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return;
	}

	tileset_path(buffer, directory, tileset_name);
	if (Tileset::Result r = tileset->read_graphics(buffer, _map_options_dialog->lighting(), _map_options_dialog->skip_60_7f())) {
		std::string msg = "Error reading ";
		tileset_path(buffer, "", tileset_name);
		msg = msg + buffer + "!\n\n" + Tileset::error_message(r);
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return;
	}

	metatileset_path(buffer, directory, tileset_name);
	if (Metatileset::Result r = _metatileset.read_metatiles(buffer)) {
		std::string msg = "Error reading ";
		metatileset_path(buffer, "", tileset_name);
		msg = msg + buffer + "!\n\n" + Metatileset::error_message(r);
		_error_dialog->message(msg);
		_error_dialog->show(this);
		return;
	}

	uint8_t w = _map_options_dialog->map_width(), h = _map_options_dialog->map_height();
	_map.size(w, h);
	int ms = metatile_size();

	// populate map with blocks
	if (filename) {
		const char *basename = fl_filename_name(filename);

		Map::Result r = _map.read_blocks(filename);
		if (r == Map::Result::MAP_TOO_LONG) {
			std::string msg = "Warning: ";
			msg = msg + basename + ":\n\n" + _map.error_message(r);
			_warning_dialog->message(msg);
			_warning_dialog->show(this);
		}
		else if (r) {
			_map.clear();
			std::string msg = "Error reading ";
			msg = msg + basename + "!\n\n" + _map.error_message(r);
			_error_dialog->message(msg);
			_error_dialog->show(this);
			return;
		}

		sprintf(buffer, PROGRAM_NAME " - %s", basename);
		copy_label(buffer);

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
		// new map
		label(PROGRAM_NAME " - " NEW_BLK_NAME);

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

		_map.modified(true);
	}
	_map_scroll->scroll_to(0, 0);
	_map_scroll->init_sizes();
	_map_scroll->contents(_map_group->w(), _map_group->h());

	// populate sidebar with metatile buttons
	_sidebar->scroll_to(0, 0);
	for (int i = 0; i < _metatileset.size(); i++) {
		int x = ms * (i % METATILES_PER_ROW), y = ms * (i / METATILES_PER_ROW);
		Metatile_Button *mtb = new Metatile_Button(_sidebar->x() + x, _sidebar->y() + y, ms, (uint8_t)i);
		mtb->callback((Fl_Callback *)select_metatile_cb, this);
		_sidebar->add(mtb);
		_metatile_buttons[i] = mtb;
	}
	_sidebar->init_sizes();
	_sidebar->contents(ms * METATILES_PER_ROW, ms * (((int)_metatileset.size() + METATILES_PER_ROW - 1) / METATILES_PER_ROW));

	_metatile_buttons[0]->setonly();
	_selected = _metatile_buttons[0];

	_block_window->tileset(tileset);

	update_labels();
	update_status(NULL);

	redraw();
}

void Main_Window::add_sub_metatiles(size_t n) {
	size_t s = _metatileset.size();
	if (n == s) { return; }
	_metatileset.size(n);
	int ms = metatile_size();

	if (n > s) {
		// add metatiles
		for (int i = (int)s; i < n; i++) {
			int x = ms * (i % METATILES_PER_ROW), y = ms * (i / METATILES_PER_ROW);
			Metatile_Button *mtb = new Metatile_Button(_sidebar->x() + x, _sidebar->y() - _sidebar->yposition() + y, ms, (uint8_t)i);
			mtb->callback((Fl_Callback *)select_metatile_cb, this);
			_sidebar->add(mtb);
			_metatile_buttons[i] = mtb;
		}
	}
	else if (n < s) {
		// remove metatiles
		if (_selected->id() >= n) {
			_selected = _metatile_buttons[0];
			_selected->setonly();
			_sidebar->scroll_to(0, 0);
		}
		for (int i = (int)n; i < s; i++) {
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

bool Main_Window::save_map() {
	const char *filename = _blk_file.c_str();

	if (_map.modified()) {
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
	metatileset_path(filename, directory, tileset_name);

	if (_metatileset.modified()) {
		FILE *file = fl_fopen(filename, "wb");
		if (!file) {
			const char *basename = fl_filename_name(filename);
			std::string msg = "Could not write to ";
			msg = msg + basename + "!";
			_error_dialog->message(msg);
			_error_dialog->show(this);
			return false;
		}

		size_t n = _metatileset.size();
		for (size_t i = 0; i < n; i++) {
			Metatile *mt = _metatileset.metatile((uint8_t)i);
			for (int y = 0; y < METATILE_SIZE; y++) {
				for (int x = 0; x < METATILE_SIZE; x++) {
					uint8_t id = mt->tile_id(x, y);
					fputc(id, file);
				}
			}
		}
		fclose(file);

		_metatileset.modified(false);
	}

	const char *basename = fl_filename_name(filename);
	std::string msg = "Saved ";
	msg = msg + basename + "!";
	_success_dialog->message(msg);
	_success_dialog->show(this);

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
	int ms = metatile_size();
	_sidebar->size(ms * METATILES_PER_ROW + Fl::scrollbar_size(), _sidebar->h());
	_map_scroll->resize(_sidebar->w(), _map_scroll->y(), w() - _sidebar->w(), _map_scroll->h());
	_map_group->resize(_sidebar->w(), _map_group->y(), (int)_map.width() * ms, (int)_map.height() * ms);
	_sidebar->init_sizes();
	_sidebar->contents(ms * METATILES_PER_ROW, ms * (((int)_metatileset.size() + METATILES_PER_ROW - 1) / METATILES_PER_ROW));
	_map_group->init_sizes();
	_map_scroll->init_sizes();
	_map_scroll->contents(_map_group->w(), _map_group->h());
	int sx = _sidebar->x(), sy = _sidebar->y();
	size_t n = _metatileset.size();
	for (int i = 0; i < n; i++) {
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
	for (int i = 0; i < n; i++) {
		_metatile_buttons[i]->id(_metatile_buttons[i]->id());
	}
	n = _map.size();
	for (int i = 0; i < n; i++) {
		_map.block(i)->update_label();
	}
	redraw();
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
	if (mw->_metatileset.modified()) {
		save_metatiles_cb(w, mw);
		if (!mw->_map.modified()) { return; }
	}
	if (mw->_blk_file.empty()) {
		save_as_cb(w, mw);
		return;
	}
	mw->save_map();
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
	if (!project_path_from_blk_path(filename, directory, mw->prism())) {
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

	mw->save_map();
}

void Main_Window::save_metatiles_cb(Fl_Widget *, Main_Window *mw) {
	if (!mw->_map.size()) { return; }
	mw->save_metatileset();
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
	memset(mw->_metatile_buttons, 0, sizeof(mw->_metatile_buttons));
	mw->_selected = NULL;
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
	global_config.set("theme", OS::current_theme());
	global_config.set("x", mw->x());
	global_config.set("y", mw->y());
	global_config.set("w", mw->w());
	global_config.set("h", mw->h());
	global_config.set("grid", mw->grid());
	global_config.set("zoom", mw->zoom());
	global_config.set("ids", mw->ids());
	global_config.set("hex", mw->hex());
	global_config.set("options-prism", mw->prism());
	if (mw->_map_options_dialog->initialized()) {
		global_config.set("map-lighting", mw->_map_options_dialog->lighting());
		global_config.set("map-skip", mw->_map_options_dialog->skip_60_7f());
	}
	if (mw->_resize_dialog->initialized()) {
		global_config.set("resize-anchor", mw->_resize_dialog->anchor());
	}

	exit(EXIT_SUCCESS);
}

void Main_Window::undo_cb(Fl_Widget *, Main_Window *) {
	// TODO: undo
}

void Main_Window::redo_cb(Fl_Widget *, Main_Window *) {
	// TODO: redo
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

void Main_Window::grid_cb(Fl_Menu_ *m, Main_Window *mw) {
	int v = m->mvalue()->value();
	mw->_grid_tb->value(v);
	mw->redraw();
}

void Main_Window::zoom_cb(Fl_Menu_ *m, Main_Window *mw) {
	int v = m->mvalue()->value();
	mw->_zoom_tb->value(v);
	mw->update_zoom();
	mw->redraw();
}

void Main_Window::ids_cb(Fl_Menu_ *m, Main_Window *mw) {
	int v = m->mvalue()->value();
	mw->_ids_tb->value(v);
	mw->redraw();
}

void Main_Window::hex_cb(Fl_Menu_ *m, Main_Window *mw) {
	int v = m->mvalue()->value();
	mw->_hex_tb->value(v);
	mw->update_labels();
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

void Main_Window::prism_cb(Fl_Menu_ *, Main_Window *mw) {
	mw->redraw();
}

void Main_Window::grid_tb_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	bool v = !!mw->_grid_tb->value();
	if (v) { mw->_grid_mi->set(); }
	else { mw->_grid_mi->clear(); }
	mw->redraw();
}

void Main_Window::zoom_tb_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	bool v = !!mw->_zoom_tb->value();
	if (v) { mw->_zoom_mi->set(); }
	else { mw->_zoom_mi->clear(); }
	mw->update_zoom();
	mw->redraw();
}

void Main_Window::ids_tb_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	bool v = !!mw->_ids_tb->value();
	if (v) { mw->_ids_mi->set(); }
	else { mw->_ids_mi->clear(); }
	mw->redraw();
}

void Main_Window::hex_tb_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	bool v = !!mw->_hex_tb->value();
	if (v) { mw->_hex_mi->set(); }
	else { mw->_hex_mi->clear(); }
	mw->update_labels();
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
		if (Fl::event_shift()) {
			// Shift+left-click to flood fill
			mw->flood_fill(b, b->id(), mw->_selected->id());
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
		mw->_selected = mw->_metatile_buttons[id];
		mw->_selected->setonly();
		int ms = mw->metatile_size();
		if (ms * (id / METATILES_PER_ROW) >= mw->_sidebar->yposition() + mw->_sidebar->h() - ms / 2) {
			mw->_sidebar->scroll_to(0, ms * (id / METATILES_PER_ROW + 1) - mw->_sidebar->h());
			mw->_sidebar->redraw();
		}
		else if (ms * (id / METATILES_PER_ROW + 1) <= mw->_sidebar->yposition() + ms / 2) {
			mw->_sidebar->scroll_to(0, ms * (id / METATILES_PER_ROW));
			mw->_sidebar->redraw();
		}
	}
}
