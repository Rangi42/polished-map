#include <queue>
#include <utility>

#include "themes.h"
#include "config.h"
#include "palette-window.h"

Swatch_Window::Swatch_Window(int x, int y, int w, int h, const char *l) : Fl_Double_Window(x, y, w, h, l) {}

int Swatch_Window::handle(int event) {
	Palette_Window *lw = (Palette_Window *)user_data();
	// FIX: actual cut/copy/paste events interfere here somehow, but not in Tile_Window::handle
	if (Fl::event_text()) {
		if (lw->_debounce) {
			if (Fl::event() == FL_KEYUP) {
				lw->_debounce = false;
			}
		}
		if (Fl::test_shortcut(FL_COMMAND + 'c')) {
			Palette_Window::copy_color_cb(NULL, lw);
		}
		else if (Fl::test_shortcut(FL_COMMAND + 'v')) {
			Palette_Window::paste_color_cb(NULL, lw);
		}
		else if (Fl::test_shortcut(FL_COMMAND + 'x')) {
			Palette_Window::swap_colors_cb(NULL, lw);
			lw->_debounce = true;
		}
	}
	return Fl_Double_Window::handle(event);
}

Abstract_Palette_Window::Abstract_Palette_Window(int x, int y) : _dx(x), _dy(y), _current_palettes(), _canceled(false),
	_window(NULL), _selected(NULL), _chosen(NULL), _red_spinner(NULL), _green_spinner(NULL), _blue_spinner(NULL),
	_ok_button(NULL), _cancel_button(NULL), _copied(false), _clipboard(), _debounce() {}

Abstract_Palette_Window::~Abstract_Palette_Window() {
	delete _window;
	delete _red_spinner;
	delete _green_spinner;
	delete _blue_spinner;
	delete _ok_button;
	delete _cancel_button;
}

void Abstract_Palette_Window::initialize() {
	if (_window) { return; }
	Fl_Group *prev_current = Fl_Group::current();
	Fl_Group::current(NULL);
	// Populate window
	initial_setup();
	// Initialize window
	_window->box(OS_BG_BOX);
	_window->callback((Fl_Callback *)close_cb, this);
	_window->set_modal();
	// Initialize window's children
	_red_spinner->range(0, 31);
	_red_spinner->callback((Fl_Callback *)change_red_cb, this);
	_green_spinner->range(0, 31);
	_green_spinner->callback((Fl_Callback *)change_green_cb, this);
	_blue_spinner->range(0, 31);
	_blue_spinner->callback((Fl_Callback *)change_blue_cb, this);
	_ok_button->tooltip("OK (Enter)");
	_ok_button->callback((Fl_Callback *)close_cb, this);
	_cancel_button->tooltip("Cancel (Esc)");
	_cancel_button->shortcut(FL_Escape);
	_cancel_button->callback((Fl_Callback *)cancel_cb, this);
	Fl_Group::current(prev_current);
}

void Abstract_Palette_Window::show(const Fl_Widget *p) {
	initialize();
	refresh();
	int x = p->x() + (p->w() - _window->w()) / 2;
	int y = p->y() + (p->h() - _window->h()) / 2;
	_window->position(x, y);
	_ok_button->take_focus();
	_window->show();
	while (_window->shown()) { Fl::wait(); }
}

void Abstract_Palette_Window::select(Color_Button *cb) {
	_selected = cb;
	_selected->setonly();
	uchar r, g, b;
	Fl::get_color(cb->color(), r, g, b);
	_red_spinner->default_value(CRGB5(r));
	_green_spinner->default_value(CRGB5(g));
	_blue_spinner->default_value(CRGB5(b));
}

void Abstract_Palette_Window::close_cb(Fl_Widget *, Abstract_Palette_Window *alw) {
	alw->_window->hide();
}

void Abstract_Palette_Window::cancel_cb(Fl_Widget *w, Abstract_Palette_Window *alw) {
	alw->_canceled = true;
	close_cb(w, alw);
}

void Abstract_Palette_Window::select_color_cb(Color_Button *cb, Abstract_Palette_Window *alw) {
	// Click to select
	alw->select(cb);
	alw->_window->redraw();
}

void Abstract_Palette_Window::change_red_cb(Default_Spinner *sp, Abstract_Palette_Window *alw) {
	if (!alw->_selected) { return; }
	uchar r, g, b;
	Fl::get_color(alw->_selected->color(), r, g, b);
	r = RGB5C((uchar)sp->value());
	alw->_selected->color(fl_rgb_color(r, g, b));
	alw->_window->redraw();
}

void Abstract_Palette_Window::change_green_cb(Default_Spinner *sp, Abstract_Palette_Window *alw) {
	if (!alw->_selected) { return; }
	uchar r, g, b;
	Fl::get_color(alw->_selected->color(), r, g, b);
	g = RGB5C((uchar)sp->value());
	alw->_selected->color(fl_rgb_color(r, g, b));
	alw->_window->redraw();
}

void Abstract_Palette_Window::change_blue_cb(Default_Spinner *sp, Abstract_Palette_Window *alw) {
	if (!alw->_selected) { return; }
	uchar r, g, b;
	Fl::get_color(alw->_selected->color(), r, g, b);
	b = RGB5C((uchar)sp->value());
	alw->_selected->color(fl_rgb_color(r, g, b));
	alw->_window->redraw();
}

void Abstract_Palette_Window::copy_color_cb(Fl_Widget *, Abstract_Palette_Window *alw) {
	if (!alw->_selected) { return; }
	uchar r, g, b;
	Fl::get_color(alw->_selected->color(), r, g, b);
	alw->_clipboard[0] = CRGB5(r);
	alw->_clipboard[1] = CRGB5(g);
	alw->_clipboard[2] = CRGB5(b);
	alw->_chosen = alw->_selected;
	alw->_copied = true;
}

void Abstract_Palette_Window::paste_color_cb(Fl_Widget *, Abstract_Palette_Window *alw) {
	if (!alw->_copied || !alw->_selected) { return; }
	uchar r = RGB5C(alw->_clipboard[0]), g = RGB5C(alw->_clipboard[1]), b = RGB5C(alw->_clipboard[2]);
	alw->_selected->color(fl_rgb_color(r, g, b));
	alw->select(alw->_selected);
	alw->_window->redraw();
}

void Abstract_Palette_Window::swap_colors_cb(Fl_Widget *, Abstract_Palette_Window *alw) {
	if (!alw->_copied || !alw->_selected || !alw->_chosen) { return; }
	Fl_Color cc = alw->_chosen->color();
	Fl_Color sc = alw->_selected->color();
	alw->_chosen->color(sc);
	alw->_selected->color(cc);
	alw->select(alw->_selected);
	alw->_window->redraw();
}

Palette_Window::Palette_Window(int x, int y) : Abstract_Palette_Window(x, y), _palette_heading_group(NULL),
	_color_group(NULL), _palette_headings(), _color_buttons() {}

Palette_Window::~Palette_Window() {
	delete _palette_heading_group;
	delete _color_group;
}

void Palette_Window::initial_setup() {
	// Populate window
	int hhgw = text_width("YELLOW:", 2);
	_window = new Swatch_Window(_dx, _dy, 201+hhgw, 191, "Edit Current Palettes");
	_palette_heading_group = new Fl_Group(10, 10, hhgw, 171);
	_palette_heading_group->end();
	_window->begin();
	_color_group = new Fl_Group(10+hhgw, 10, 87, 171);
	_color_group->end();
	_window->begin();
	int rgblw = std::max(std::max(text_width("R:", 2), text_width("G:", 2)), text_width("B:", 2));
	int rgbsw = text_width("99", 2) + 22;
	_red_spinner = new Default_Spinner(107 + hhgw + rgblw, 10, rgbsw, 22, "R:");
	_green_spinner = new Default_Spinner(107 + hhgw + rgblw, 36, rgbsw, 22, "G:");
	_blue_spinner = new Default_Spinner(107 + hhgw + rgblw, 62, rgbsw, 22, "B:");
	_ok_button = new Default_Button(111 + hhgw, 120, 80, 22, "OK");
	_cancel_button = new OS_Button(111 + hhgw, 159, 80, 22, "Cancel");
	_window->end();
	// Populate hue heading group
	const char *hhll[NUM_PALETTES] = {"GRAY:", "RED:", "GREEN:", "WATER:", "YELLOW:", "BROWN:", "ROOF:", "TEXT:"};
	_palette_heading_group->begin();
	for (int i = 0; i < NUM_PALETTES; i++) {
		Label *hhl = new Label(10, 11+21*i, hhgw, 22, hhll[i]);
		hhl->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
	}
	_palette_heading_group->end();
	// Populate swatch group
	_color_group->color(FL_INACTIVE_COLOR);
	_color_group->begin();
	for (int y = 0; y < NUM_PALETTES; y++) {
		for (int x = 0; x < NUM_HUES; x++) {
			Color_Button *cb = new Color_Button(_color_group->x()+2+21*x, _color_group->y()+2+21*y, 20);
			cb->color(Color::fl_color(_current_palettes, (Palette)y, Color::ordered_hue(x)));
			cb->callback((Fl_Callback *)select_color_cb, this);
			_color_buttons[y][x] = cb;
		}
	}
	_color_group->end();
	// Initialize window's children
	_palette_heading_group->box(FL_NO_BOX);
	_color_group->box(OS_SPACER_THIN_DOWN_BOX);
}

void Palette_Window::refresh() {
	_canceled = false;
	_copied = false;
	for (int y = 0; y < NUM_PALETTES; y++) {
		for (int x = 0; x < NUM_HUES; x++) {
			Fl_Color c = Color::fl_color(_current_palettes, (Palette)y, Color::ordered_hue(x));
			_color_buttons[y][x]->color(c);
		}
	}
	select(_color_buttons[0][0]);
}

void Palette_Window::apply_modifications() {
	for (int y = 0; y < NUM_PALETTES; y++) {
		for (int x = 0; x < NUM_HUES; x++) {
			Color_Button *cb = _color_buttons[y][x];
			Color::color(_current_palettes, (Palette)y, Color::ordered_hue(x), cb->color());
		}
	}
}
