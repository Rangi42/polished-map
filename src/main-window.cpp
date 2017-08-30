#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ios>
#include <iomanip>
#include <locale>
#include <algorithm>
#include <queue>

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
	_default_metatile_image(DEFAULT_METATILE_XPM), _show_hex_ids(false), _zoom(false) {
	// Populate window
	int wx = 0, wy = 0, ww = w, wh = h;

	_menu_bar = new Fl_Menu_Bar(wx, wy, w, 21);
	wy += _menu_bar->h();
	wh -= _menu_bar->h();

	_toolbar = new Toolbar(wx, wy, w, 26);
	wy += _toolbar->h();
	wh -= _toolbar->h();
	_open_tb = new Toolbar_Button(0, 0, 24, 24);
	_save_tb = new Toolbar_Button(0, 0, 24, 24);
	new Fl_Box(0, 0, 2, 0); new Spacer(0, 0, 2, 0); new Fl_Box(0, 0, 2, 0);
	_hex_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_zoom_tb = new Toolbar_Toggle_Button(0, 0, 24, 24);
	_toolbar->end();
	begin();

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

	_sidebar = new Workspace(wx, wy, (metatile_size() + 1) * METATILES_PER_ROW + Fl::scrollbar_size(), wh);
	wx += _sidebar->w();
	ww -= _sidebar->w();
	_sidebar->type(Fl_Scroll::VERTICAL_ALWAYS);
	_sidebar->end();
	begin();

	_map_scroll = new Workspace(wx, wy, ww, wh);
	_map_scroll->type(Fl_Scroll::BOTH);
	_map = new Fl_Group(wx, wy, 0, 0);
	_map->end();

	_open_tb->tooltip("Open... (Ctrl+O)");
	_open_tb->callback((Fl_Callback *)open_cb, this);
	_open_tb->image(OPEN_ICON);
	_open_tb->take_focus();

	_save_tb->tooltip("Save (Ctrl+S)");
	_save_tb->callback((Fl_Callback *)save_cb, this);
	_save_tb->image(SAVE_ICON);

	_hex_tb->tooltip("Show Hex IDs (Ctrl+H)");
	_hex_tb->callback((Fl_Callback *)hex_cb, this);
	_hex_tb->shortcut(FL_COMMAND + 'h');
	_hex_tb->image(HEX_ICON);
	_hex_tb->user_data(this);
	_hex_tb->value(_show_hex_ids ? 1 : 0);

	_zoom_tb->tooltip("Zoom (Ctrl+=)");
	_zoom_tb->callback((Fl_Callback *)zoom_cb, this);
	_zoom_tb->shortcut(FL_COMMAND + '=');
	_zoom_tb->image(ZOOM_ICON);
	_zoom_tb->user_data(this);
	_zoom_tb->value(_zoom ? 1 : 0);

	// Initialize window
	size_range(384, 256);
	resizable(_map_scroll);
	callback(exit_cb);
	icon((const void *)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON1)));

	// Initialize menu bar
	_menu_bar->box(OS_PANEL_THIN_UP_BOX);
	_menu_bar->down_box(FL_FLAT_BOX);
	Fl_Menu_Item menu_items[] = {
		// label, shortcut, callback, data, flags
		OS_SUBMENU("&File"),
		OS_MENU_ITEM("&Open...", FL_COMMAND + 'o', (Fl_Callback *)open_cb, this, 0),
		OS_MENU_ITEM("&Close", FL_COMMAND + 'w', (Fl_Callback *)close_cb, this, 0),
		OS_MENU_ITEM("&Save", FL_COMMAND + 's', (Fl_Callback *)save_cb, this, 0),
		OS_MENU_ITEM("E&xit", FL_ALT + FL_F + 4, (Fl_Callback *)exit_cb, this, 0),
		{},
		{}
	};
	_menu_bar->copy(menu_items);
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
	sprintf(buffer, "ID: $%02x", id);
	_hover_id->copy_label(buffer);
	_status_bar->redraw();
}

void Main_Window::flood_fill(Block *b, uint8_t f, uint8_t t) {
	if (f == t || b->id() != f) { return; }
	b->id(t);
	uint8_t row = b->row(), col = b->col();
	uint8_t i = row * _map_w + col;
	if (col > 0) { flood_fill(_blocks[i-1], f, t); } // left
	if (col < _map_w - 1) { flood_fill(_blocks[i+1], f, t); } // right
	if (row > 0) { flood_fill(_blocks[i-_map_w], f, t); } // up
	if (row < _map_h - 1) { flood_fill(_blocks[i+_map_w], f, t); } // down
}

void Main_Window::toggle_zoom() {
	int ms = metatile_size();
	_sidebar->size((ms + 1) * METATILES_PER_ROW + Fl::scrollbar_size(), _sidebar->h());
	_map_scroll->resize(_sidebar->w(), _map_scroll->y(), w() - _sidebar->w(), _map_scroll->h());
	_map->resize(_sidebar->w(), _map->y(), _map_w * (ms + 1), _map_h * (ms + 1));
	_sidebar->init_sizes();
	_map->init_sizes();
	_map_scroll->init_sizes();
	int sx = _sidebar->x(), sy = _sidebar->y();
	for (int i = 0; i < _num_metatiles; i++) {
		Metatile *mt = _metatiles[i];
		int dx = (ms + 1) * (i % METATILES_PER_ROW), dy = (ms + 1) * (i / METATILES_PER_ROW);
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
			int dx = col * (ms + 1), dy = row * (ms + 1);
			block->resize(mx + dx, my + dy, ms + 1, ms + 1);
		}
	}
}

void Main_Window::open_cb(Fl_Widget *, Main_Window *mw) {
	close_cb(NULL, mw);

	int ms = mw->metatile_size();

	// dummy metatiles
	mw->_num_metatiles = 245;
	for (int i = 0; i < mw->_num_metatiles; i++) {
		int x = (ms + 1) * (i % METATILES_PER_ROW), y = (ms + 1) * (i / METATILES_PER_ROW);
		Metatile *metatile = new Metatile(mw->_sidebar->x() + x, mw->_sidebar->y() + y, ms, (uint8_t)(i % MAX_METATILES));
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
	mw->_sidebar->redraw();

	// dummy map
	mw->_map_w = 18;
	mw->_map_h = 10;
	mw->_blocks = new Block *[mw->_map_w * mw->_map_h]();
	mw->_map->size((ms + 1) * mw->_map_w, (ms + 1) * mw->_map_h);
	for (int row = 0; row < mw->_map_h; row++) {
		for (int col = 0; col < mw->_map_w; col++) {
			int x = col * (ms + 1), y = row * (ms + 1);
			Block *block = new Block(mw->_map->x() + x, mw->_map->y() + y, ms, row, col, 0);
			block->callback((Fl_Callback *)change_block_cb, mw);
			mw->_map->add(block);
			mw->_blocks[row * mw->_map_w + col] = block;
		}
	}
	mw->_map_scroll->scroll_to(0, 0);
	mw->_map_scroll->init_sizes();
	mw->_map_scroll->redraw();

	mw->update_status(NULL);
	mw->_status_bar->redraw();
}

void Main_Window::close_cb(Fl_Widget *, Main_Window *mw) {
	mw->_sidebar->clear();
	mw->_sidebar->scroll_to(0, 0);
	mw->_num_metatiles = 0;
	memset(mw->_metatiles, 0, sizeof(mw->_metatiles));
	mw->_selected = NULL;
	mw->_map->clear();
	delete [] mw->_blocks;
	mw->_blocks = NULL;
	mw->_map_w = mw->_map_h = 0;
	mw->init_sizes();
	mw->update_status(NULL);
	mw->redraw();
}

void Main_Window::save_cb(Fl_Widget *, Main_Window *) {
}

void Main_Window::exit_cb(Fl_Widget *, void *) {
	// Override default behavior of Esc to close main window
	if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) { return; }
	exit(EXIT_SUCCESS);
}

void Main_Window::hex_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	mw->_show_hex_ids = !!mw->_hex_tb->value();
	mw->redraw();
}

void Main_Window::zoom_cb(Toolbar_Toggle_Button *, Main_Window *mw) {
	mw->_zoom = !!mw->_zoom_tb->value();
	mw->toggle_zoom();
	mw->redraw();
}

void Main_Window::select_metatile_cb(Metatile *mt, Main_Window *mw) {
	mw->_selected = mt;
}

void Main_Window::change_block_cb(Block *b, Main_Window *mw) {
	if (Fl::event_button() == FL_RIGHT_MOUSE) {
		uint8_t id = b->id();
		if (id >= mw->_num_metatiles) { return; }
		mw->_selected = mw->_metatiles[id];
		mw->_selected->setonly();
		if ((mw->metatile_size() + 1) * (id / 4) >= mw->_sidebar->yposition() + mw->_sidebar->h() - mw->metatile_size() / 2) {
			mw->_sidebar->scroll_to(0, (mw->metatile_size() + 1) * (id / 4 + 1) - mw->_sidebar->h());
			mw->_sidebar->redraw();
		}
		else if ((mw->metatile_size() + 1) * (id / 4 + 1) <= mw->_sidebar->yposition() + mw->metatile_size() / 2) {
			mw->_sidebar->scroll_to(0, (mw->metatile_size() + 1) * (id / 4));
			mw->_sidebar->redraw();
		}
	}
	else if (Fl::event_button() == FL_MIDDLE_MOUSE) {
		if (mw->_selected) {
			mw->flood_fill(b, b->id(), mw->_selected->id());
			mw->_map->redraw();
			mw->update_status(b);
		}
	}
	else if (mw->_selected) { // FL_LEFT_MOUSE or FL_DRAG
		uint8_t id = mw->_selected->id();
		b->id(id);
		b->damage(1);
		mw->update_status(b);
	}
}
