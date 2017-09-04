#pragma warning(push, 0)
#include <FL/Enumerations.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#pragma warning(pop)

#include "themes.h"
#include "widgets.h"
#include "utils.h"
#include "option-dialogs.h"

Option_Dialog::Option_Dialog(const char *t) : _title(t), _canceled(false), _dialog(NULL), _content(NULL),
	_ok_button(NULL), _cancel_button(NULL) {}

Option_Dialog::~Option_Dialog() {
	delete _dialog;
	delete _content;
	delete _ok_button;
	delete _cancel_button;
}

void Option_Dialog::initialize() {
	if (_dialog) { return; }
	Fl_Group *prev_current = Fl_Group::current();
	Fl_Group::current(NULL);
	// Populate dialog
	_dialog = new Fl_Double_Window(0, 0, 0, 0, _title);
	_content = new Fl_Group(0, 0, 0, 0);
	_content->begin();
	initialize_content();
	_content->end();
	_dialog->begin();
	_ok_button = new Default_Button(0, 0, 0, 0, "OK");
	_cancel_button = new OS_Button(0, 0, 0, 0, "Cancel");
	_dialog->end();
	// Initialize dialog
	_dialog->resizable(NULL);
	_dialog->callback((Fl_Callback *)cancel_cb, this);
	_dialog->set_modal();
	// Initialize dialog's children
	_ok_button->tooltip("OK (Enter)");
	_ok_button->callback((Fl_Callback *)close_cb, this);
	_cancel_button->shortcut(FL_Escape);
	_cancel_button->tooltip("Cancel (Esc)");
	_cancel_button->callback((Fl_Callback *)cancel_cb, this);
	Fl_Group::current(prev_current);
}

void Option_Dialog::refresh() {
	_canceled = false;
	_dialog->copy_label(_title);
	// Refresh widget positions and sizes
	int dy = 10;
	int w = 320;
	dy += refresh_content(w - 20, dy) + 16;
	_ok_button->resize(w - 184, dy, 80, 22);
	_cancel_button->resize(w - 90, dy, 80, 22);
	dy += _cancel_button->h() + 10;
	_dialog->size_range(w, dy, w, dy);
	_dialog->size(w, dy);
	_dialog->redraw();
}

void Option_Dialog::show(const Fl_Widget *p) {
	initialize();
	refresh();
	int x = p->x() + (p->w() - _dialog->w()) / 2;
	int y = p->y() + (p->h() - _dialog->h()) / 2;
	_dialog->position(x, y);
	_ok_button->take_focus();
	_dialog->show();
	while (_dialog->shown()) { Fl::wait(); }
}

void Option_Dialog::close_cb(Fl_Widget *, Option_Dialog *od) {
	od->_dialog->hide();
}

void Option_Dialog::cancel_cb(Fl_Widget *, Option_Dialog *od) {
	od->_canceled = true;
	od->_dialog->hide();
}

Open_Blk_Dialog::Open_Blk_Dialog(const char *t) : Option_Dialog(t),
	_map_width(NULL), _map_height(NULL), _tileset(NULL), _lighting(NULL) {}

Open_Blk_Dialog::~Open_Blk_Dialog() {
	delete _map_width;
	delete _map_height;
	delete _tileset;
	delete _lighting;
}

bool Open_Blk_Dialog::limit_blk_options(const char *d) {
	initialize();

	char td[MAX_PATH * 2] = {};
	strcpy(td, d);
	strcat(td, /* "gfx\\tilesets\\" */ "..\\..\\Dropbox\\pkmn\\tilesets\\");

	dirent **list;
	int n = fl_filename_list(td, &list);
	if (n < 0) {
		fl_alert("fl_filename_list %d", n);
		return false;
	}

	_map_width->value(0);
	_map_height->value(0);
	_tileset->clear();

	std::string dir(d);
	for (int i = 0; i < n; i++) {
		const char *name = list[i]->d_name;
		if (ends_with(name, ".png")) {
			std::string v(name);
			v.erase(v.size() - 4, 4);
			_tileset->add(v.c_str());
		}
	}
	_tileset->value(0);

	return true;
}

void Open_Blk_Dialog::initialize_content() {
	// Populate content group
	_map_width = new OS_Spinner(0, 0, 0, 0, "Width:");
	_map_height = new OS_Spinner(0, 0, 0, 0, "Height:");
	_tileset = new Dropdown(0, 0, 0, 0, "Tileset:");
	_lighting = new Dropdown(0, 0, 0, 0, "Lighting:");
	// Initialize content group's children
	_map_width->align(FL_ALIGN_LEFT);
	_map_width->range(0, 255);
	_map_height->align(FL_ALIGN_LEFT);
	_map_height->range(0, 255);
	_tileset->align(FL_ALIGN_LEFT);
	_lighting->align(FL_ALIGN_LEFT);
	_lighting->add("Day"); // DAY
	_lighting->add("Nite"); // NITE
	_lighting->add("Indoor"); // INDOOR
	_lighting->value(0);
}

int Open_Blk_Dialog::refresh_content(int ww, int dy) {
	int wgt_w = 0, wgt_h = 23;
	int ch = (wgt_h + 2) * 2 + wgt_h;
	_content->resize(10, dy, ww, ch);

	int wgt_off = 10 + MAX(MAX(text_width(_map_width->label()), text_width(_tileset->label())), text_width(_lighting->label()));

	wgt_w = text_width("999", 2) + wgt_h;
	_map_width->resize(10 + wgt_off, dy, wgt_w, wgt_h);
	_map_height->resize(_map_width->x() + _map_width->w() + 10 + text_width("Height:"), dy, wgt_w, wgt_h);
	dy += _map_height->h() + 2;

	wgt_w = ww - wgt_off;
	_tileset->resize(10 + wgt_off, dy, wgt_w, wgt_h);
	dy += _tileset->h() + 2;

	wgt_w = text_width(_lighting->label()) + MAX(MAX(text_width("Day", 2), text_width("Nite", 2)), text_width("Indoor", 2));
	_lighting->resize(10 + wgt_off, dy, wgt_w, wgt_h);

	return ch;
}
