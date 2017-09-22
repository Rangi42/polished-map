#include "tile.h"
#include "tileset-window.h"

Tileset_Window::Tileset_Window(int x, int y) : _dx(x), _dy(y), _metatile_id(0), _tileset(NULL), _canceled(false),
	_window(NULL), _tileset_heading(NULL), _metatile_heading(NULL), _tileset_group(NULL), _metatile_group(NULL),
	_tile_buttons(), _chips(), _ok_button(NULL), _cancel_button(NULL), _selected(NULL) {}

Tileset_Window::~Tileset_Window() {
	delete _window;
	delete _tileset_heading;
	delete _metatile_heading;
	delete _tileset_group;
	delete _metatile_group;
	delete _cancel_button;
	delete _ok_button;
}

void Tileset_Window::initialize() {
	if (_window) { return; }
	Fl_Group *prev_current = Fl_Group::current();
	Fl_Group::current(NULL);
	// Populate window
	_window = new Fl_Double_Window(_dx, _dy, 386, 304, "Edit Metatile");
	_tileset_heading = new Label(10, 10, 258, 22);
	_metatile_heading = new Label(278, 10, 98, 22);
	_tileset_group = new Fl_Group(10, 36, 258, 258);
	_tileset_group->end();
	_window->begin();
	_metatile_group = new Fl_Group(278, 36, 98, 98);
	_metatile_group->end();
	_window->begin();
	_ok_button = new Default_Button(296, 233, 80, 22, "OK");
	_cancel_button = new OS_Button(296, 272, 80, 22, "Cancel");
	_window->end();
	// Populate tileset group
	_tileset_group->begin();
	for (int y = 0; y < TILES_PER_COL; y++) {
		for (int x = 0; x < TILES_PER_ROW; x++) {
			int bx = _tileset_group->x() + 1 + x * TILE_PX_SIZE, by = _tileset_group->y() + 1 + y * TILE_PX_SIZE;
			uint8_t id = (uint8_t)(y * TILES_PER_ROW + x);
			Tile_Button *tb = new Tile_Button(bx, by, TILE_PX_SIZE, id);
			tb->callback((Fl_Callback *)select_tile_cb, this);
			_tile_buttons[id] = tb;
		}
	}
	_tileset_group->end();
	// Populate metatile
	_metatile_group->begin();
	for (int y = 0; y < METATILE_SIZE; y++) {
		for (int x = 0; x < METATILE_SIZE; x++) {
			int cx = _metatile_group->x() + 1 + x * CHIP_PX_SIZE, cy = _metatile_group->y() + 1 + y * CHIP_PX_SIZE;
			int id = y * METATILE_SIZE + x;
			Chip *c = new Chip(cx, cy, CHIP_PX_SIZE, 0, 0, 0);
			c->callback((Fl_Callback *)change_chip_cb, this);
			_chips[id] = c;
		}
	}
	_metatile_group->end();
	// Initialize window
	_window->callback((Fl_Callback *)close_cb, this);
	_window->set_modal();
	// Initialize window's children
	_tileset_group->box(FL_THIN_DOWN_BOX);
	_metatile_group->box(FL_THIN_DOWN_BOX);
	_ok_button->tooltip("OK (Enter)");
	_ok_button->callback((Fl_Callback *)close_cb, this);
	_cancel_button->tooltip("Cancel (Esc)");
	_cancel_button->shortcut(FL_Escape);
	_cancel_button->callback((Fl_Callback *)cancel_cb, this);
	Fl_Group::current(prev_current);
}

void Tileset_Window::refresh() {
	_canceled = false;
	_selected = _tile_buttons[0];
	_selected->setonly();
}

void Tileset_Window::tileset(const Tileset *t) {
	initialize();
	_tileset = t;
	if (t) {
		std::string label("Tileset: ");
		label = label + t->name();
		_tileset_heading->copy_label(label.c_str());
	}
	else {
		_tileset_heading->label(NULL);
	}
}

void Tileset_Window::metatile(const Metatile *mt) {
	_metatile_id = mt->id();
	for (int y = 0; y < METATILE_SIZE; y++) {
		for (int x = 0; x < METATILE_SIZE; x++) {
			int i = y * METATILE_SIZE + x;
			uint8_t id = mt->tile_id(x, y);
			_chips[i]->id(id);
		}
	}
	char buffer[32];
	sprintf(buffer, "Metatile: $%02X", _metatile_id);
	_metatile_heading->copy_label(buffer);
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

void Tileset_Window::draw_tile(int x, int y, uint8_t id, bool z) {
	const Tile *t = _tileset->tile(id);
	const uchar *rgb = t->rgb();
	if (z) {
		// TODO: draw tile at 3x zoom
		fl_rectf(x, y, CHIP_PX_SIZE, CHIP_PX_SIZE, FL_BACKGROUND_COLOR);
		fl_draw_image(rgb, x+4, y+4, TILE_PX_SIZE, TILE_PX_SIZE, NUM_CHANNELS, LINE_BYTES);
	}
	else {
		fl_draw_image(rgb, x, y, TILE_PX_SIZE, TILE_PX_SIZE, NUM_CHANNELS, LINE_BYTES);
	}
}

void Tileset_Window::close_cb(Fl_Widget *, Tileset_Window *tw) {
	tw->_window->hide();
}

void Tileset_Window::cancel_cb(Fl_Widget *w, Tileset_Window *tw) {
	tw->_canceled = true;
	close_cb(w, tw);
}

void Tileset_Window::select_tile_cb(Tile_Button *tb, Tileset_Window *tw) {
	// Click to select
	tw->_selected = tb;
}

void Tileset_Window::change_chip_cb(Chip *c, Tileset_Window *tw) {
	if (Fl::event_button() == FL_LEFT_MOUSE) {
		// Left-click to edit
		uint8_t id = tw->_selected->id();
		c->id(id);
		c->damage(1);
	}
	else if (Fl::event_button() == FL_RIGHT_MOUSE) {
		// Right-click to select
		uint8_t id = c->id();
		tw->_selected = tw->_tile_buttons[id];
		tw->_selected->setonly();
	}
}
