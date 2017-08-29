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

Main_Window::Main_Window(int x, int y, int w, int h, const char *) : Fl_Double_Window(x, y, w, h, PROGRAM_NAME) {
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

	// Initialize drag-and-drop receiver
	begin();
	_dnd_receiver = new DnD_Receiver(0, 0, 0, 0);
	end();
	_dnd_receiver->callback((Fl_Callback *)drag_and_drop_cb);
	_dnd_receiver->user_data(this);

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

void Main_Window::drag_and_drop_cb(DnD_Receiver *dndr, Main_Window *mw) {
}

static const char *dummy_xpm[] = {
	"32 32 4 1",
	"0 c #B0F850", // white
	"1 c #287000", // dark
	"2 c #60C808", // light
	"3 c #383838", // black
	"2020200020202000202020002020200020202000202020002020200020202000",
	"0212020202120202021202020212020202120202021202020212020202120202",
	"2020102020201020202010202020102020201020202010202020102020201020",
	"0002020000020200000202000002020000020200000202000002020000020200",
	"2020202020202020202020202020202020202020202020202020202020202020",
	"0202021202020212020202120202021202020212020202120202021202020212",
	"0000202000002020000020200000202000002020000020200000202000002020",
	"0002020000020200000202000002020000020200000202000002020000020200",
	"2020200020202000202020002020200020202000202020002020200020202000",
	"0212020202120202021202020212020202120202021202020212020202120202",
	"2020102020201020202010202020102020201020202010202020102020201020",
	"0002020000020200000202000002020000020200000202000002020000020200",
	"2020202020202020202020202020202020202020202020202020202020202020",
	"0202021202020212020202120202021202020212020202120202021202020212",
	"0000202000002020000020200000202000002020000020200000202000002020",
	"0002020000020200000202000002020000020200000202000002020000020200",
	"2020200020202000202020002020200020202000202020002020200020202000",
	"0212020202120202021202020212020202120202021202020212020202120202",
	"2020102020201020202010202020102020201020202010202020102020201020",
	"0002020000020200000202000002020000020200000202000002020000020200",
	"2020202020202020202020202020202020202020202020202020202020202020",
	"0202021202020212020202120202021202020212020202120202021202020212",
	"0000202000002020000020200000202000002020000020200000202000002020",
	"0002020000020200000202000002020000020200000202000002020000020200",
	"2020200020202000202020002020200020202000202020002020200020202000",
	"0212020202120202021202020212020202120202021202020212020202120202",
	"2020102020201020202010202020102020201020202010202020102020201020",
	"0002020000020200000202000002020000020200000202000002020000020200",
	"2020202020202020202020202020202020202020202020202020202020202020",
	"0202021202020212020202120202021202020212020202120202021202020212",
	"0000202000002020000020200000202000002020000020200000202000002020",
	"0002020000020200000202000002020000020200000202000002020000020200"
};

void Main_Window::open_cb(Fl_Widget *w, Main_Window *mw) {
	// dummy metatiles
	Fl_Pixmap dummy_icon(dummy_xpm);
	mw->_sidebar->clear();
	char label[4] = {};
	for (int i = 0; i < 256; i++) {
		int x = 33 * (i % 4), y = 33 * (i / 4);
		Metatile *metatile = new Metatile(mw->_sidebar->x() + x, mw->_sidebar->y() + y, (uint8_t)i);
		Fl_Pixmap *dummy_icon = new Fl_Pixmap(dummy_xpm);
		metatile->image(dummy_icon);
		mw->_sidebar->add(metatile);
	}
	mw->_sidebar->scrollbar.value(0);
	mw->_sidebar->init_sizes();
	mw->_sidebar->redraw();

	// dummy map
	mw->_map->clear();
	mw->_map->size(33 * 18, 33 * 10);
	for (int i = 0; i < 18 * 10; i++) {
		int x = 33 * (i % 18), y = 33 * (i / 18);
		Metatile *metatile = new Metatile(mw->_map->x() + x, mw->_map->y() + y, 0, false);
		Fl_Pixmap *dummy_icon = new Fl_Pixmap(dummy_xpm);
		metatile->image(dummy_icon);
		mw->_map->add(metatile);
	}
	mw->_map_scroll->scrollbar.value(0);
	mw->_map_scroll->hscrollbar.value(0);
	mw->_map_scroll->init_sizes();
	mw->_map_scroll->redraw();
}

void Main_Window::close_cb(Fl_Widget *w, Main_Window *mw) {
}

void Main_Window::save_cb(Fl_Widget *w, Main_Window *mw) {
}

void Main_Window::exit_cb(Fl_Widget *, void *) {
	// Override default behavior of Esc to close main window
	if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) { return; }
	exit(EXIT_SUCCESS);
}
