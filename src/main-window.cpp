#include <cstdlib>

#pragma warning(push, 0)
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/fl_ask.H>
#pragma warning(pop)

#include "version.h"
#include "utils.h"
#include "themes.h"
#include "image.h"
#include "widgets.h"
#include "modal-dialog.h"
#include "progress-dialog.h"
#include "waiting-dialog.h"
#include "tileset.h"
#include "metatileset.h"
#include "main-window.h"
#include "icons.h"

#include "resource.h"

#include "metatile.xpm"

#include "dummy.h"

static int text_width(const char *l, int pad = 0) {
	int lw = 0, lh = 0;
	fl_measure(l, lw, lh, 0);
	return lw + 2 * pad;
}

Main_Window::Main_Window(int x, int y, int w, int h, const char *) : Fl_Double_Window(x, y, w, h, PROGRAM_NAME),
	_num_metatiles(0), _metatiles(), _selected(NULL), _blocks(), _map_w(0), _map_h(0),
	_grid_mi(NULL), _zoom_mi(NULL), _ids_mi(NULL), _hex_mi(NULL),
	_default_metatile_image(DEFAULT_METATILE_XPM), _unsaved(false), _wx(x), _wy(y), _ww(w), _wh(h) {
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
	_metatile_count = new Status_Bar_Field(0, 0, text_width("Metatiles: 9999", 3), 0, "");
	new Spacer(0, 0, 2, 0);
	_map_dimensions = new Status_Bar_Field(0, 0, text_width("Map: 9999 x 9999", 3), 0, "");
	new Spacer(0, 0, 2, 0);
	_hover_x = new Status_Bar_Field(0, 0, text_width("X: 9999", 3), 0, "");
	_hover_y = new Status_Bar_Field(0, 0, text_width("Y: 9999", 3), 0, "");
	_hover_id = new Status_Bar_Field(0, 0, text_width("ID: $9999", 3), 0, "");
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
	_map = new Fl_Group(wx, wy, 0, 0);
	_map->end();

	// Configure window
	size_range(384, 256);
	resizable(_map_scroll);
	callback(exit_cb);
	icon((const void *)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON1)));

	// Configure menu bar
	_menu_bar->box(OS_PANEL_THIN_UP_BOX);
	_menu_bar->down_box(FL_FLAT_BOX);

	// Configure menu bar items
	Fl_Menu_Item menu_items[] = {
		// label, shortcut, callback, data, flags
		OS_SUBMENU("&File"),
		OS_MENU_ITEM("&New...", FL_COMMAND + 'n', (Fl_Callback *)new_cb, this, FL_MENU_INACTIVE),
		OS_MENU_ITEM("&Open...", FL_COMMAND + 'o', (Fl_Callback *)open_cb, this, 0),
		OS_MENU_ITEM("&Save", FL_COMMAND + 's', (Fl_Callback *)save_cb, this, FL_MENU_INACTIVE),
		OS_MENU_ITEM("&Save As...", FL_COMMAND + 'S', (Fl_Callback *)save_as_cb, this, FL_MENU_INACTIVE),
		OS_MENU_ITEM("&Close", FL_COMMAND + 'w', (Fl_Callback *)close_cb, this, FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Print...", FL_COMMAND + 'p', (Fl_Callback *)print_cb, this, FL_MENU_DIVIDER | FL_MENU_INACTIVE),
		OS_MENU_ITEM("E&xit", FL_ALT + FL_F + 4, (Fl_Callback *)exit_cb, this, 0),
		{},
		OS_SUBMENU("&Edit"),
		OS_MENU_ITEM("&Undo", FL_COMMAND + 'z', (Fl_Callback *)undo_cb, this, FL_MENU_INACTIVE),
		OS_MENU_ITEM("&Redo", FL_COMMAND + 'y', (Fl_Callback *)redo_cb, this, FL_MENU_DIVIDER | FL_MENU_INACTIVE),
		OS_MENU_ITEM("Re&size", FL_COMMAND + 'e', (Fl_Callback *)resize_cb, this, FL_MENU_INACTIVE),
		{},
		OS_SUBMENU("&View"),
		OS_MENU_ITEM("&Theme", 0, NULL, NULL, FL_SUBMENU | FL_MENU_DIVIDER),
		OS_MENU_ITEM("&Aero", 0, (Fl_Callback *)aero_theme_cb, this,
			FL_MENU_RADIO | (OS::current_theme() == OS::AERO ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Metro", 0, (Fl_Callback *)metro_theme_cb, this,
			FL_MENU_RADIO | (OS::current_theme() == OS::METRO ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Blue", 0, (Fl_Callback *)blue_theme_cb, this,
			FL_MENU_RADIO | (OS::current_theme() == OS::BLUE ? FL_MENU_VALUE : 0)),
		OS_MENU_ITEM("&Dark", 0, (Fl_Callback *)dark_theme_cb, this,
			FL_MENU_RADIO | (OS::current_theme() == OS::DARK ? FL_MENU_VALUE : 0)),
		{},
		OS_MENU_ITEM("&Grid", FL_COMMAND + 'g', (Fl_Callback *)grid_cb, this, FL_MENU_TOGGLE | FL_MENU_VALUE),
		OS_MENU_ITEM("&Zoom", FL_COMMAND + '=', (Fl_Callback *)zoom_cb, this, FL_MENU_TOGGLE),
		OS_MENU_ITEM("&IDs", FL_COMMAND + 'i', (Fl_Callback *)ids_cb, this, FL_MENU_TOGGLE | FL_MENU_VALUE),
		OS_MENU_ITEM("&Hex", FL_COMMAND + 'h', (Fl_Callback *)hex_cb, this, FL_MENU_DIVIDER | FL_MENU_TOGGLE | FL_MENU_VALUE),
		OS_MENU_ITEM("Full &Screen", FL_F + 11, (Fl_Callback *)full_screen_cb, this, FL_MENU_TOGGLE),
		{},
		OS_SUBMENU("&Help"),
		OS_MENU_ITEM("&Help", FL_F + 1, (Fl_Callback *)about_cb, this, FL_MENU_DIVIDER | FL_MENU_INACTIVE),
		OS_MENU_ITEM("&About", FL_COMMAND + 'a', (Fl_Callback *)about_cb, this, FL_MENU_INACTIVE),
		{},
		{}
	};
	_menu_bar->copy(menu_items);

	// Initialize menu bar items
#define PM_FIND_MENU_ITEM_CB(c) (const_cast<Fl_Menu_Item *>(_menu_bar->find_item((Fl_Callback *)(c))))
	_aero_theme_mi = PM_FIND_MENU_ITEM_CB(aero_theme_cb);
	_metro_theme_mi = PM_FIND_MENU_ITEM_CB(metro_theme_cb);
	_blue_theme_mi = PM_FIND_MENU_ITEM_CB(blue_theme_cb);
	_dark_theme_mi = PM_FIND_MENU_ITEM_CB(dark_theme_cb);
	_grid_mi = PM_FIND_MENU_ITEM_CB(grid_cb);
	_zoom_mi = PM_FIND_MENU_ITEM_CB(zoom_cb);
	_ids_mi = PM_FIND_MENU_ITEM_CB(ids_cb);
	_hex_mi = PM_FIND_MENU_ITEM_CB(hex_cb);
	_full_screen_mi = PM_FIND_MENU_ITEM_CB(full_screen_cb);
#undef PM_FIND_MENU_ITEM_CB

	// Configure toolbar buttons

	_new_tb->tooltip("New... (Ctrl+N)");
	_new_tb->callback((Fl_Callback *)new_cb, this);
	_new_tb->image(NEW_ICON);
	_new_tb->take_focus();
	_new_tb->deactivate(); // TODO: implement new

	_open_tb->tooltip("Open... (Ctrl+O)");
	_open_tb->callback((Fl_Callback *)open_cb, this);
	_open_tb->image(OPEN_ICON);

	_save_tb->tooltip("Save (Ctrl+S)");
	_save_tb->callback((Fl_Callback *)save_cb, this);
	_save_tb->image(SAVE_ICON);
	_save_tb->deactivate(); // TODO: implement save

	_print_tb->tooltip("Print (Ctrl+P)");
	_print_tb->callback((Fl_Callback *)print_cb, this);
	_print_tb->image(PRINT_ICON);
	_print_tb->deactivate(); // TODO: implement print

	_undo_tb->tooltip("Undo (Ctrl+Z)");
	_undo_tb->callback((Fl_Callback *)undo_cb, this);
	_undo_tb->image(UNDO_ICON);
	_undo_tb->deactivate(); // TODO: implement undo

	_redo_tb->tooltip("Redo (Ctrl+Y)");
	_redo_tb->callback((Fl_Callback *)redo_cb, this);
	_redo_tb->image(REDO_ICON);
	_redo_tb->deactivate(); // TODO: implement redo

	_resize_tb->tooltip("Resize (Ctrl+E)");
	_resize_tb->callback((Fl_Callback *)resize_cb, this);
	_resize_tb->image(RESIZE_ICON);
	_resize_tb->deactivate(); // TODO: implement resize

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
}

Main_Window::~Main_Window() {
	delete _menu_bar; // includes menu items
	delete _toolbar; // includes toolbar buttons
	delete _sidebar; // includes metatiles
	delete _status_bar; // includes status bar fields
	delete _map_scroll; // includes map and blocks
}

void Main_Window::show() {
	Fl_Double_Window::show();
	// Fix for 16x16 icon from <http://www.fltk.org/str.php?L925>
	HWND hwnd = fl_xid(this);
	HANDLE big_icon = LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,
		GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CXICON), 0);
	SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(big_icon));
	HANDLE small_icon = LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0);
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(small_icon));
}

Fl_Image *Main_Window::metatile_image(uint8_t id) {
	if (id >= _num_metatiles) { return &_default_metatile_image; }
	return _metatiles[id]->image();
}

void Main_Window::update_status(Block *b) {
	char buffer[256];
	if (!b) {
		sprintf(buffer, "Metatiles: %d", _num_metatiles);
		_metatile_count->copy_label(buffer);
		sprintf(buffer, "Map: %d x %d", _map_w, _map_h);
		_map_dimensions->copy_label(buffer);
		_hover_x->label("X:");
		_hover_y->label("Y:");
		_hover_id->label("ID:");
		return;
	}
	uint8_t row = b->row(), col = b->col(), id = b->id();
	sprintf(buffer, "X: %d", col);
	_hover_x->copy_label(buffer);
	sprintf(buffer, "Y: %d", row);
	_hover_y->copy_label(buffer);
	sprintf(buffer, hex() ? "ID: $%02X" : "ID: %d", id);
	_hover_id->copy_label(buffer);
	_status_bar->redraw();
}

void Main_Window::flood_fill(Block *b, uint8_t f, uint8_t t) {
	if (f == t || b->id() != f) { return; }
	b->id(t);
	int row = b->row(), col = b->col();
	int i = row * _map_w + col;
	if (col > 0) { flood_fill(_blocks[i-1], f, t); } // left
	if (col < _map_w - 1) { flood_fill(_blocks[i+1], f, t); } // right
	if (row > 0) { flood_fill(_blocks[i-_map_w], f, t); } // up
	if (row < _map_h - 1) { flood_fill(_blocks[i+_map_w], f, t); } // down
}

void Main_Window::update_zoom() {
	int ms = metatile_size();
	_sidebar->size(ms * METATILES_PER_ROW + Fl::scrollbar_size(), _sidebar->h());
	_map_scroll->resize(_sidebar->w(), _map_scroll->y(), w() - _sidebar->w(), _map_scroll->h());
	_map->resize(_sidebar->w(), _map->y(), _map_w * ms, _map_h * ms);
	_sidebar->init_sizes();
	_sidebar->contents(ms * METATILES_PER_ROW, ms * ((_num_metatiles + METATILES_PER_ROW - 1) / METATILES_PER_ROW));
	_map->init_sizes();
	_map_scroll->init_sizes();
	_map_scroll->contents(_map->w(), _map->h());
	int sx = _sidebar->x(), sy = _sidebar->y();
	for (int i = 0; i < _num_metatiles; i++) {
		Metatile_Button *mt = _metatiles[i];
		int dx = ms * (i % METATILES_PER_ROW), dy = ms * (i / METATILES_PER_ROW);
		mt->resize(sx + dx, sy + dy, ms + 1, ms + 1);
		Fl_Image *img = mt->image();
		Fl_Image *resized = img->copy(ms, ms);
		mt->image(resized);
		delete img;
	}
	int mx = _map->x(), my = _map->y();
	for (int row = 0; row < _map_h; row++) {
		for (int col = 0; col < _map_w; col++) {
			Block *block = _blocks[row * _map_w + col];
			int dx = col * ms, dy = row * ms;
			block->resize(mx + dx, my + dy, ms, ms);
		}
	}
}

void Main_Window::update_labels() {
	for (int i = 0; i < _num_metatiles; i++) {
		_metatiles[i]->id(_metatiles[i]->id());
	}
	for (int i = 0; i < _map_w * _map_h; i++) {
		_blocks[i]->id(_blocks[i]->id());
	}
	redraw();
}

void Main_Window::new_cb(Fl_Widget *, Main_Window *) {
	// TODO: new map
}

void Main_Window::open_cb(Fl_Widget *, Main_Window *mw) {
	close_cb(NULL, mw);

	Metatileset mts;
	if (Palette_Map::Result pm_r = mts.read_palette_map("E:/Code/polishedcrystal/tilesets/johto1_palette_map.asm")) {
		fl_alert("bad palette map %d", pm_r);
	}
	if (Tileset::Result ts_r = mts.read_png_graphics("E:/Dropbox/pkmn/tilesets/johto1.png")) {
		fl_alert("bad png %d", ts_r);
	}
	if (Metatileset::Result mts_r = mts.read_metatiles("E:/Code/polishedcrystal/tilesets/johto1_metatiles.bin")) {
		fl_alert("bad meta %d", mts_r);
	}

	int ms = mw->metatile_size();

	// dummy metatiles
	mw->_num_metatiles = 245;
	for (int i = 0; i < mw->_num_metatiles; i++) {
		int x = ms * (i % METATILES_PER_ROW), y = ms * (i / METATILES_PER_ROW);
		Metatile_Button *metatile = new Metatile_Button(mw->_sidebar->x() + x, mw->_sidebar->y() + y, ms, (uint8_t)(i % MAX_NUM_METATILES));
		Fl_Pixmap *dummy_icon = new Fl_Pixmap(dummy_metatiles[i]);
		if (dummy_icon->w() != ms || dummy_icon->h() != ms) {
			Fl_Image *copy = dummy_icon->copy(ms, ms);
			delete dummy_icon;
			metatile->image(copy);
		}
		else {
			metatile->image(dummy_icon);
		}
		metatile->callback((Fl_Callback *)select_metatile_cb, mw);
		mw->_sidebar->add(metatile);
		mw->_metatiles[i] = metatile;
	}
	mw->_sidebar->scroll_to(0, 0);
	mw->_sidebar->init_sizes();
	mw->_sidebar->contents(ms * METATILES_PER_ROW, ms * ((mw->_num_metatiles + METATILES_PER_ROW - 1) / METATILES_PER_ROW));

	mw->_metatiles[0]->setonly();
	select_metatile_cb(mw->_metatiles[0], mw);

	// dummy map
	mw->_map_w = 32;
	mw->_map_h = 32;
	mw->_blocks = new Block *[mw->_map_w * mw->_map_h]();
	mw->_map->size(ms * mw->_map_w, ms * mw->_map_h);
	for (uint8_t row = 0; row < mw->_map_h; row++) {
		for (uint8_t col = 0; col < mw->_map_w; col++) {
			int x = col * ms, y = row * ms;
			Block *block = new Block(mw->_map->x() + x, mw->_map->y() + y, ms, row, col, 0);
			block->callback((Fl_Callback *)change_block_cb, mw);
			mw->_map->add(block);
			mw->_blocks[row * mw->_map_w + col] = block;
		}
	}
	mw->_map_scroll->scroll_to(0, 0);
	mw->_map_scroll->init_sizes();
	mw->_map_scroll->contents(mw->_map->w(), mw->_map->h());

	mw->update_labels();
	mw->update_status(NULL);

	mw->redraw();
}

void Main_Window::save_cb(Fl_Widget *, Main_Window *) {
	// TODO: save
}

void Main_Window::save_as_cb(Fl_Widget *, Main_Window *) {
	// TODO: save as
}

void Main_Window::close_cb(Fl_Widget *, Main_Window *mw) {
	mw->_sidebar->clear();
	mw->_sidebar->scroll_to(0, 0);
	mw->_sidebar->contents(0, 0);
	mw->_num_metatiles = 0;
	memset(mw->_metatiles, 0, sizeof(mw->_metatiles));
	mw->_selected = NULL;
	mw->_map->clear();
	mw->_map->size(0, 0);
	delete [] mw->_blocks;
	mw->_blocks = NULL;
	mw->_map_w = mw->_map_h = 0;
	mw->_map_scroll->contents(0, 0);
	mw->init_sizes();
	mw->update_status(NULL);
	mw->redraw();
}

void Main_Window::print_cb(Fl_Widget *, Main_Window *) {
	// TODO: print
}

void Main_Window::exit_cb(Fl_Widget *, void *) {
	// Override default behavior of Esc to close main window
	if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) { return; }
	exit(EXIT_SUCCESS);
}

void Main_Window::undo_cb(Fl_Widget *, Main_Window *) {
	// TODO: undo
}

void Main_Window::redo_cb(Fl_Widget *, Main_Window *) {
	// TODO: redo
}

void Main_Window::resize_cb(Fl_Widget *, Main_Window *) {
	// TODO: resize
	/*
	W: [___] H: [___]
	Anchor:
	(.) Top (_) Bottom
	(.) Left (_) Right
		  [OK] [Cancel]
	*/
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

void Main_Window::help_cb(Fl_Widget *, Main_Window *) {
	// TODO: help
}

void Main_Window::about_cb(Fl_Widget *, Main_Window *) {
	// TODO: about
}

void Main_Window::select_metatile_cb(Metatile_Button *mt, Main_Window *mw) {
	mw->_selected = mt;
}

void Main_Window::change_block_cb(Block *b, Main_Window *mw) {
	if (Fl::event_button() == FL_LEFT_MOUSE) {
		if (!mw->_selected) { return; }
		if (Fl::event_shift()) {
			// Shift+left click to flood fill
			mw->flood_fill(b, b->id(), mw->_selected->id());
			mw->_map->redraw();
			mw->update_status(b);
		}
		else {
			// Left click/drag to edit
			uint8_t id = mw->_selected->id();
			b->id(id);
			b->damage(1);
			mw->update_status(b);
		}
	}
	else if (Fl::event_button() == FL_RIGHT_MOUSE) {
		// Right-click to select
		uint8_t id = b->id();
		if (id >= mw->_num_metatiles) { return; }
		mw->_selected = mw->_metatiles[id];
		mw->_selected->setonly();
		int ms = mw->metatile_size();
		if (ms * (id / 4) >= mw->_sidebar->yposition() + mw->_sidebar->h() - ms / 2) {
			mw->_sidebar->scroll_to(0, ms * (id / 4 + 1) - mw->_sidebar->h());
			mw->_sidebar->redraw();
		}
		else if (ms * (id / 4 + 1) <= mw->_sidebar->yposition() + ms / 2) {
			mw->_sidebar->scroll_to(0, ms * (id / 4));
			mw->_sidebar->redraw();
		}
	}
}
