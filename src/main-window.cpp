#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ios>
#include <iomanip>
#include <locale>
#include <algorithm>

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

Main_Window::Main_Window(int x, int y, int w, int h, const char *) : Fl_Double_Window(x, y, w, h, PROGRAM_NAME),
	_num_metatiles(0), _metatiles(), _selected(NULL), _default_metatile_image(DEFAULT_METATILE_XPM),
	_show_hex_ids(false) {
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
	_toolbar->end();
	begin();

	_status_bar = new Toolbar(wx, h-23, w, 23);
	wh -= _status_bar->h();
	_status_bar->end();
	begin();

	_sidebar = new Workspace(wx, wy, (32 + 1) * 4 + Fl::scrollbar_size(), wh);
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

// right-click to select the metatile with a block's id
// middle-click to flood fill

void Main_Window::open_cb(Fl_Widget *w, Main_Window *mw) {
	close_cb(w, mw);

	// dummy metatiles
	for (int i = 0; i < 245; i++) {
		int x = 33 * (i % 4), y = 33 * (i / 4);
		Metatile *metatile = new Metatile(mw->_sidebar->x() + x, mw->_sidebar->y() + y, (uint8_t)(i % 256));
		Fl_Pixmap *dummy_icon = new Fl_Pixmap(dummy_metatiles[i]);
		metatile->image(dummy_icon);
		metatile->callback((Fl_Callback *)select_metatile_cb, mw);
		mw->_sidebar->add(metatile);
		mw->_metatiles[i] = metatile;
		mw->_num_metatiles++;
	}
	mw->_sidebar->scrollbar.value(0);
	mw->_sidebar->init_sizes();
	mw->_sidebar->redraw();

	// dummy map
	mw->_map->clear();
	mw->_map->size(33 * 18, 33 * 10);
	for (int i = 0; i < 18 * 10; i++) {
		int x = 33 * (i % 18), y = 33 * (i / 18);
		Block *block = new Block(mw->_map->x() + x, mw->_map->y() + y, (uint8_t)(i % 256));
		block->callback((Fl_Callback *)change_block_cb, mw);
		mw->_map->add(block);
	}
	mw->_map_scroll->scrollbar.value(0);
	mw->_map_scroll->hscrollbar.value(0);
	mw->_map_scroll->init_sizes();
	mw->_map_scroll->redraw();
}

void Main_Window::close_cb(Fl_Widget *, Main_Window *mw) {
	mw->_sidebar->clear();
	mw->_num_metatiles = 0;
	memset(mw->_metatiles, 0, sizeof(mw->_metatiles));
	mw->_selected = NULL;
	mw->_map->clear();
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

void Main_Window::select_metatile_cb(Metatile *mt, Main_Window *mw) {
	mw->_selected = mt;
}

void Main_Window::change_block_cb(Block *b, Main_Window *mw) {
	if (Fl::event_button() == FL_RIGHT_MOUSE) {
		uint8_t id = b->id();
		if (id < mw->_num_metatiles) {
			mw->_selected = mw->_metatiles[id];
			mw->_selected->setonly();
		}
	}
	else if (mw->_selected != NULL) {
		uint8_t id = mw->_selected->id();
		b->id(id);
		b->damage(1);
	}
}
