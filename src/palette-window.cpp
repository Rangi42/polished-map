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
	_window(NULL), _selected(NULL), _chosen(NULL), _color_group(NULL), _red_spinner(NULL), _green_spinner(NULL),
	_blue_spinner(NULL), _red_slider(NULL), _green_slider(NULL), _blue_slider(NULL), _hex_color_rgb(NULL),
	_hex_color_swatch(NULL), _ok_button(NULL), _cancel_button(NULL), _copied(false), _clipboard(), _debounce() {}

Abstract_Palette_Window::~Abstract_Palette_Window() {
	delete _window;
	delete _color_group;
	delete _red_spinner;
	delete _green_spinner;
	delete _blue_spinner;
	delete _red_slider;
	delete _green_slider;
	delete _blue_slider;
	delete _hex_color_rgb;
	delete _hex_color_swatch;
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
	_red_spinner->callback((Fl_Callback *)change_channel_cb, this);
	_green_spinner->range(0, 31);
	_green_spinner->callback((Fl_Callback *)change_channel_cb, this);
	_blue_spinner->range(0, 31);
	_blue_spinner->callback((Fl_Callback *)change_channel_cb, this);
	_red_slider->range(0, 31);
	_red_slider->step(1);
	_red_slider->callback((Fl_Callback *)change_channel_cb, this);
	_green_slider->range(0, 31);
	_green_slider->step(1);
	_green_slider->callback((Fl_Callback *)change_channel_cb, this);
	_blue_slider->range(0, 31);
	_blue_slider->step(1);
	_blue_slider->callback((Fl_Callback *)change_channel_cb, this);
	_hex_color_rgb->maximum_size(6);
	_hex_color_rgb->callback((Fl_Callback *)hex_color_rgb_cb, this);
	_hex_color_rgb->when(FL_WHEN_ENTER_KEY);
	_hex_color_rgb->textfont(FL_COURIER);
	_hex_color_swatch->box(OS_SWATCH_BOX);
	_hex_color_swatch->down_box(OS_SWATCH_BOX);
	_hex_color_swatch->callback((Fl_Callback *)hex_color_swatch_cb, this);
	_ok_button->tooltip("OK (Enter)");
	_ok_button->callback((Fl_Callback *)close_cb, this);
	_cancel_button->tooltip("Cancel (Esc)");
	_cancel_button->shortcut(FL_Escape);
	_cancel_button->callback((Fl_Callback *)cancel_cb, this);
	Fl_Group::current(prev_current);
}

void Abstract_Palette_Window::initialize_inputs(int ox, int oy, int ss, int bx, int by) {
	_window->begin();
	int wo = std::max(std::max(text_width("R:", 2), text_width("G:", 2)), text_width("B:", 2));
	int ww = text_width("99", 2) + 22;
	_red_spinner = new Default_Spinner(ox+wo, oy, ww, 22, "R:");
	_red_slider = new Default_Slider(ox+wo+ww+6, oy, ss-wo-ww, 22);
	_green_spinner = new Default_Spinner(ox+wo, oy+26, ww, 22, "G:");
	_green_slider = new Default_Slider(ox+wo+ww+6, oy+26, ss-wo-ww, 22);
	_blue_spinner = new Default_Spinner(ox+wo, oy+52, ww, 22, "B:");
	_blue_slider = new Default_Slider(ox+wo+ww+6, oy+52, ss-wo-ww, 22);
	wo = text_width("Hex: #", 2);
	Fl_Font font = fl_font();
	fl_font(FL_COURIER, fl_size());
	ww = std::max(text_width("AAAAAA", 4), text_width("FFFFFF", 4));
	fl_font(font, fl_size());
	_hex_color_rgb = new OS_Hex_Input(ox+wo, oy+78, ww, 22, "Hex: #");
	_hex_color_swatch = new Fl_Button(ox+wo+ww+4, oy+78, 22, 22);
#ifdef _WIN32
	_ok_button = new Default_Button(bx, by, 80, 22, "OK");
	_cancel_button = new OS_Button(bx+94, by, 80, 22, "Cancel");
#else
	_cancel_button = new OS_Button(bx, by, 80, 22, "Cancel");
	_ok_button = new Default_Button(bx+94, by, 80, 22, "OK");
#endif
	_window->end();
}

void Abstract_Palette_Window::update_color(Fl_Widget *wgt) {
	uchar r, g, b;
	Fl::get_color(_selected->color(), r, g, b);

	if (wgt->parent() == _color_group) {
		_red_spinner->default_value(CRGB5(r));
		_green_spinner->default_value(CRGB5(g));
		_blue_spinner->default_value(CRGB5(b));
		_red_slider->default_value(CRGB5(r));
		_green_slider->default_value(CRGB5(g));
		_blue_slider->default_value(CRGB5(b));
	}
	else {
		if (wgt == _red_spinner) {
			double v = _red_spinner->value();
			_red_slider->value(v);
			r = RGB5C((uchar)v);
		}
		else if (wgt == _green_spinner) {
			double v = _green_spinner->value();
			_green_slider->value(v);
			g = RGB5C((uchar)v);
		}
		else if (wgt == _blue_spinner) {
			double v = _blue_spinner->value();
			_blue_slider->value(v);
			b = RGB5C((uchar)v);
		}
		else if (wgt == _red_slider) {
			double v = _red_slider->value();
			_red_spinner->value(v);
			r = RGB5C((uchar)v);
		}
		else if (wgt == _green_slider) {
			double v = _green_slider->value();
			_green_spinner->value(v);
			g = RGB5C((uchar)v);
		}
		else if (wgt == _blue_slider) {
			double v = _blue_slider->value();
			_blue_spinner->value(v);
			b = RGB5C((uchar)v);
		}
		else {
			_red_spinner->value(CRGB5(r));
			_green_spinner->value(CRGB5(g));
			_blue_spinner->value(CRGB5(b));
			_red_slider->value(CRGB5(r));
			_green_slider->value(CRGB5(g));
			_blue_slider->value(CRGB5(b));
		}
	}

	Fl_Color c = fl_rgb_color(r, g, b);
	_selected->color(c);
	_selected->damage(1);

	char buffer[10] = {};
	sprintf(buffer, "%02X%02X%02X", r, g, b);
	_hex_color_rgb->value(buffer);

	_hex_color_swatch->color(c);
	_hex_color_swatch->selection_color(c);
	_hex_color_swatch->damage(1);

	_window->redraw();
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
	update_color(cb);
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

void Abstract_Palette_Window::change_channel_cb(Fl_Widget *w, Abstract_Palette_Window *alw) {
	if (!alw->_selected) { return; }
	alw->update_color(w);
}

void Abstract_Palette_Window::hex_color_rgb_cb(OS_Hex_Input *, Abstract_Palette_Window *alw) {
	const char *s = alw->_hex_color_rgb->value();
	char rgb[7] = {};
	if (size_t n = strlen(s); n < 6) {
		for (size_t i = 0; i < 6 - n; i++) {
			rgb[i] = '0';
		}
	}
	strncat(rgb, s, 6);

	char buffer[3] = {};
	buffer[0] = rgb[0];
	buffer[1] = rgb[1];
	uchar r = (uchar)strtoul(buffer, NULL, 16);
	buffer[0] = rgb[2];
	buffer[1] = rgb[3];
	uchar g = (uchar)strtoul(buffer, NULL, 16);
	buffer[0] = rgb[4];
	buffer[1] = rgb[5];
	uchar b = (uchar)strtoul(buffer, NULL, 16);

	alw->_selected->color(fl_rgb_color(r, g, b));
	alw->update_color(alw->_hex_color_rgb);
}

#ifdef _WIN32
static DWORD fl_to_win_color(Fl_Color c) {
	uchar r, g, b;
	Fl::get_color(c, r, g, b);
	return RGB(r, g, b);
}
#endif

void Abstract_Palette_Window::hex_color_swatch_cb(Fl_Button *, Abstract_Palette_Window *alw) {
#ifdef _WIN32
	static COLORREF customColors[16] = {};

	CHOOSECOLOR cc = {};
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = fl_xid(alw->_window);
	cc.lpCustColors = (LPDWORD)customColors;
	cc.rgbResult = fl_to_win_color(alw->_selected->color());
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc)) {
		uchar r = GetRValue(cc.rgbResult), g = GetGValue(cc.rgbResult), b = GetBValue(cc.rgbResult);
		alw->_selected->color(fl_rgb_color(r, g, b));
		alw->update_color(alw->_hex_color_swatch);
	}
#else
	alw->update_color(alw->_hex_color_swatch);
#endif
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
	_palette_headings(), _color_buttons() {}

Palette_Window::~Palette_Window() {
	delete _palette_heading_group;
	delete _color_group;
}

static const char *palette_labels[NUM_GAME_PALETTES] = {"GRAY:", "RED:", "GREEN:", "WATER:", "YELLOW:", "BROWN:", "ROOF:", "TEXT:"};

void Palette_Window::initial_setup() {
	// Populate window
	int off = std::max(text_width("YELLOW:", 2), text_width("BROWN:", 2));
	_window = new Swatch_Window(_dx, _dy, 295+off, 191, "Edit Current Palettes");
	_palette_heading_group = new Fl_Group(10, 10, off, 171);
	_palette_heading_group->end();
	_window->begin();
	_color_group = new Fl_Group(10+off, 10, 87, 171);
	_color_group->end();
	initialize_inputs(107+off, 10, 172, 111+off, 159);
	// Populate hue heading group
	_palette_heading_group->begin();
	for (int i = 0; i < NUM_GAME_PALETTES; i++) {
		Label *hhl = new Label(10, 11+21*i, off, 22, palette_labels[i]);
		hhl->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
	}
	_palette_heading_group->end();
	// Populate swatch group
	_color_group->color(FL_INACTIVE_COLOR);
	_color_group->begin();
	for (int y = 0; y < NUM_GAME_PALETTES; y++) {
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
	for (int y = 0; y < NUM_GAME_PALETTES; y++) {
		for (int x = 0; x < NUM_HUES; x++) {
			Fl_Color c = Color::fl_color(_current_palettes, (Palette)y, Color::ordered_hue(x));
			_color_buttons[y][x]->color(c);
		}
	}
	select(_color_buttons[0][0]);
}

void Palette_Window::apply_modifications() {
	for (int y = 0; y < NUM_GAME_PALETTES; y++) {
		for (int x = 0; x < NUM_HUES; x++) {
			Color_Button *cb = _color_buttons[y][x];
			Color::color(_current_palettes, (Palette)y, Color::ordered_hue(x), cb->color());
		}
	}
}

Monochrome_Palette_Window::Monochrome_Palette_Window(int x, int y) : Abstract_Palette_Window(x, y),
	_palette_heading(NULL), _color_buttons() {}

Monochrome_Palette_Window::~Monochrome_Palette_Window() {
	delete _palette_heading;
	delete _color_group;
}

void Monochrome_Palette_Window::initial_setup() {
	// Populate window
	int off = text_width("MONOCHROME:", 2);
	_window = new Swatch_Window(_dx, _dy, 107+off, 192, "Edit Current Palettes");
	_palette_heading = new Label(10, 11, off, 22, "MONOCHROME:");
	_color_group = new Fl_Group(10+off, 10, 87, 24);
	_color_group->end();
	initialize_inputs(10, 44, 81+off, off-77, 160);
	// Populate swatch group
	_color_group->color(FL_INACTIVE_COLOR);
	_color_group->begin();
	for (int i = 0; i < NUM_HUES; i++) {
		Color_Button *cb = new Color_Button(_color_group->x()+2+21*i, _color_group->y()+2, 20);
		cb->color(Color::fl_color(_current_palettes, Palette::MONOCHROME, Color::ordered_hue(i)));
		cb->callback((Fl_Callback *)select_color_cb, this);
		_color_buttons[i] = cb;
	}
	_color_group->end();
	// Initialize window's children
	_palette_heading->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
	_color_group->box(OS_SPACER_THIN_DOWN_BOX);
}

void Monochrome_Palette_Window::refresh() {
	_canceled = false;
	_copied = false;
	for (int i = 0; i < NUM_HUES; i++) {
		Fl_Color c = Color::fl_color(_current_palettes, Palette::MONOCHROME, Color::ordered_hue(i));
		_color_buttons[i]->color(c);
	}
	select(_color_buttons[0]);
}

void Monochrome_Palette_Window::apply_modifications() {
	for (int i = 0; i < NUM_HUES; i++) {
		Color_Button *cb = _color_buttons[i];
		Color::color(_current_palettes, Palette::MONOCHROME, Color::ordered_hue(i), cb->color());
	}
}
