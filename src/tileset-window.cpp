#include "themes.h"
#include "tileset-window.h"

Tileset_Window::Tileset_Window(int x, int y) : _dx(x), _dy(y), _tileset(NULL), _canceled(false),
	_window(NULL), _tileset_heading(NULL), _tile_heading(NULL), _tileset_group(NULL), _tile_group(NULL),
	_deep_tile_buttons(), _selected(NULL), _pixels(), _color1(NULL), _color2(NULL), _color3(NULL), _color4(NULL),
	_hue(NULL), _priority(NULL), _ok_button(NULL), _cancel_button(NULL) {}

Tileset_Window::~Tileset_Window() {
	delete _window;
	delete _tileset_heading;
	delete _tile_heading;
	delete _tileset_group;
	delete _tile_group;
	delete _color1;
	delete _color2;
	delete _color3;
	delete _color4;
	delete _hue;
	delete _priority;
	delete _ok_button;
	delete _cancel_button;
}

void Tileset_Window::initialize() {
	if (_window) { return; }
	Fl_Group *prev_current = Fl_Group::current();
	Fl_Group::current(NULL);
	// Populate window
	_window = new Fl_Double_Window(_dx, _dy, 466, 304, "Edit Tileset");
	_tileset_heading = new Label(10, 10, 258, 22);
	_tile_heading = new Label(278, 10, 146, 22);
	_tileset_group = new Fl_Group(10, 36, 258, 258);
	_tileset_group->end();
	_window->begin();
	_tile_group = new Fl_Group(278, 36, 146, 146);
	_tile_group->end();
	_window->begin();
	_color1 = new Swatch(434, 36, 22, "1");
	_color2 = new Swatch(434, 64, 22, "2");
	_color3 = new Swatch(434, 92, 22, "3");
	_color4 = new Swatch(434, 120, 22, "4");
	int ho = text_width("Hue:", 3);
	_hue = new Dropdown(278 + ho, 192, 146 - ho, 22, "Hue:");
	_priority = new OS_Check_Button(278, 218, 146, 22, "Priority");
	_ok_button = new Default_Button(282, 272, 80, 22, "OK");
	_cancel_button = new OS_Button(376, 272, 80, 22, "Cancel");
	_window->end();
	// Populate tileset group
	_tileset_group->begin();
	for (int y = 0; y < TILES_PER_COL; y++) {
		for (int x = 0; x < TILES_PER_ROW; x++) {
			int bx = _tileset_group->x() + 1 + x * TILE_PX_SIZE, by = _tileset_group->y() + 1 + y * TILE_PX_SIZE;
			uint8_t id = (uint8_t)(y * TILES_PER_ROW + x);
			Deep_Tile_Button *dtb = new Deep_Tile_Button(bx, by, TILE_PX_SIZE, id);
			dtb->callback((Fl_Callback *)select_tile_cb, this);
			_deep_tile_buttons[id] = dtb;
		}
	}
	_tileset_group->end();
	// Populate tile
	_tile_group->begin();
	for (int y = 0; y < TILE_SIZE; y++) {
		for (int x = 0; x < TILE_SIZE; x++) {
			int px = _tile_group->x() + 1 + x * PIXEL_ZOOM_FACTOR, py = _tile_group->y() + 1 + y * PIXEL_ZOOM_FACTOR;
			int id = y * TILE_SIZE + x;
			Pixel *pxl = new Pixel(px, py, PIXEL_ZOOM_FACTOR);
			pxl->callback((Fl_Callback *)change_pixel_cb, this);
			_pixels[id] = pxl;
		}
	}
	_tile_group->end();
	// Initialize window
	_window->callback((Fl_Callback *)close_cb, this);
	_window->set_modal();
	// Initialize window's children
	_tileset_group->box(OS_SPACER_THIN_DOWN_FRAME);
	_tile_group->box(OS_SPACER_THIN_DOWN_FRAME);
	_ok_button->tooltip("OK (Enter)");
	_ok_button->callback((Fl_Callback *)close_cb, this);
	_cancel_button->tooltip("Cancel (Esc)");
	_cancel_button->shortcut(FL_Escape);
	_cancel_button->callback((Fl_Callback *)cancel_cb, this);
	Fl_Group::current(prev_current);
}

void Tileset_Window::refresh() {
	_canceled = false;
	_selected = _deep_tile_buttons[0];
	_selected->setonly();
}

void Tileset_Window::tileset(const Tileset *t) {
	initialize();
	_tileset = t;
	if (!t) {
		_tileset_heading->label(NULL);
		return;
	}
	std::string label("Tileset: ");
	label = label + t->name();
	_tileset_heading->copy_label(label.c_str());
	for (int y = 0; y < TILES_PER_COL; y++) {
		for (int x = 0; x < TILES_PER_ROW; x++) {
			uint8_t id = (uint8_t)(y * TILES_PER_ROW + x);
			Deep_Tile_Button *dtb = _deep_tile_buttons[id];
			const Tile *t = _tileset->tile(id);
			dtb->copy_tile(t);
		}
	}
}

void Tileset_Window::show(const Fl_Widget *p) {
	initialize();
	refresh();
	int x = p->x() + (p->w() - _window->w()) / 2;
	int y = p->y() + (p->h() - _window->h()) / 2;
	_window->position(x, y);
	_ok_button->take_focus();
	_window->show();
	while (_window->shown()) { Fl::wait(); }
}

void Tileset_Window::close_cb(Fl_Widget *, Tileset_Window *tw) {
	tw->_window->hide();
}

void Tileset_Window::cancel_cb(Fl_Widget *w, Tileset_Window *tw) {
	tw->_canceled = true;
	close_cb(w, tw);
}

void Tileset_Window::select_tile_cb(Deep_Tile_Button *dtb, Tileset_Window *tw) {
	// Click to select
	tw->_selected = dtb;
}

void Tileset_Window::change_pixel_cb(Pixel *, Tileset_Window *) {
	// TODO: Tileset_Window::change_pixel_cb
}
