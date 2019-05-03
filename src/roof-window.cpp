#include <queue>
#include <utility>

#include "themes.h"
#include "config.h"
#include "roof-window.h"

Roof_Tile_Window::Roof_Tile_Window(int x, int y, int w, int h, const char *l) : Fl_Double_Window(x, y, w, h, l) {}

int Roof_Tile_Window::handle(int event) {
	Roof_Window *rw = (Roof_Window *)user_data();
	if (Fl::test_shortcut(FL_COMMAND + 'c')) {
		Roof_Window::copy_tile_cb(NULL, rw);
	}
	else if (Fl::test_shortcut(FL_COMMAND + 'v')) {
		Roof_Window::paste_tile_cb(NULL, rw);
	}
	else if (Fl::test_shortcut(FL_COMMAND + 'x')) {
		Roof_Window::swap_tiles_cb(NULL, rw);
	}
	return Fl_Double_Window::handle(event);
}

Roof_Window::Roof_Window(int x, int y) : _dx(x), _dy(y), _tileset(NULL), _canceled(false), _window(NULL),
	_roof_heading(NULL), _tile_heading(NULL), _roof_group(NULL), _tile_group(NULL), _deep_tile_buttons(),
	_selected(NULL), _pixels(), _swatch1(NULL), _swatch2(NULL), _swatch3(NULL), _swatch4(NULL), _chosen(NULL),
	_ok_button(NULL), _cancel_button(NULL), _copied(false), _clipboard(0) {}

Roof_Window::~Roof_Window() {
	delete _window;
	delete _roof_heading;
	delete _tile_heading;
	delete _roof_group;
	delete _tile_group;
	delete _swatch1;
	delete _swatch2;
	delete _swatch3;
	delete _swatch4;
	delete _ok_button;
	delete _cancel_button;
}

void Roof_Window::initialize() {
	if (_window) { return; }
	Fl_Group *prev_current = Fl_Group::current();
	Fl_Group::current(NULL);
	// Populate window
	_window = new Roof_Tile_Window(_dx, _dy, 226, 228, "Edit Roof");
	int thw = text_width("Tile: $FFF", 2);
	_roof_heading = new Label(10, 10, 206-thw, 22);
	_tile_heading = new Label(216-thw, 10, thw, 22);
	_roof_group = new Fl_Group(10, 36, 50, 50);
	_roof_group->end();
	_window->begin();
	_tile_group = new Fl_Group(70, 36, 146, 146);
	_tile_group->end();
	_window->begin();
	_swatch1 = new Swatch(10, 100, 22, "1");
	_swatch2 = new Swatch(38, 100, 22, "2");
	_swatch3 = new Swatch(10, 128, 22, "3");
	_swatch4 = new Swatch(38, 128, 22, "4");
	_ok_button = new Default_Button(42, 196, 80, 22, "OK");
	_cancel_button = new OS_Button(136, 196, 80, 22, "Cancel");
	_window->end();
	// Populate tileset group
	_roof_group->begin();
	for (int y = 0; y < ROOF_TILES_PER_COL; y++) {
		for (int x = 0; x < ROOF_TILES_PER_ROW; x++) {
			int bx = _roof_group->x() + 1 + x * TILE_PX_SIZE,
				by = _roof_group->y() + 1 + (ROOF_TILES_PER_COL - y - 1) * TILE_PX_SIZE;
			uint8_t i = (uint8_t)(y * ROOF_TILES_PER_ROW + x);
			uint8_t id = i + FIRST_ROOF_TILE_ID;
			Deep_Tile_Button *dtb = new Deep_Tile_Button(bx, by, TILE_PX_SIZE, id);
			dtb->callback((Fl_Callback *)select_tile_cb, this);
			_deep_tile_buttons[i] = dtb;
		}
	}
	_roof_group->end();
	// Populate tile
	_tile_group->begin();
	for (int y = 0; y < TILE_SIZE; y++) {
		for (int x = 0; x < TILE_SIZE; x++) {
			int px = _tile_group->x() + 1 + x * PIXEL_ZOOM_FACTOR, py = _tile_group->y() + 1 + y * PIXEL_ZOOM_FACTOR;
			Pixel_Button *pb = new Pixel_Button(px, py, PIXEL_ZOOM_FACTOR);
			pb->coords(x, y);
			pb->callback((Fl_Callback *)change_pixel_cb, this);
			_pixels[y * TILE_SIZE + x] = pb;
		}
	}
	_tile_group->end();
	// Initialize window
	_window->callback((Fl_Callback *)close_cb, this);
	_window->set_modal();
	// Initialize window's children
	_roof_group->box(OS_SPACER_THIN_DOWN_FRAME);
	_tile_group->box(OS_SPACER_THIN_DOWN_FRAME);
	_swatch1->shortcut('1');
	_swatch1->callback((Fl_Callback *)choose_swatch_cb, this);
	_swatch2->shortcut('2');
	_swatch2->callback((Fl_Callback *)choose_swatch_cb, this);
	_swatch3->shortcut('3');
	_swatch3->callback((Fl_Callback *)choose_swatch_cb, this);
	_swatch4->shortcut('4');
	_swatch4->callback((Fl_Callback *)choose_swatch_cb, this);
	_ok_button->tooltip("OK (Enter)");
	_ok_button->callback((Fl_Callback *)close_cb, this);
	_cancel_button->tooltip("Cancel (Esc)");
	_cancel_button->shortcut(FL_Escape);
	_cancel_button->callback((Fl_Callback *)cancel_cb, this);
	Fl_Group::current(prev_current);
}

void Roof_Window::refresh() {
	_canceled = false;
	_copied = false;
	select(_deep_tile_buttons[(ROOF_TILES_PER_COL - 1) * ROOF_TILES_PER_ROW]);
	choose(_swatch1);
}

void Roof_Window::tileset(Tileset *t) {
	initialize();
	_tileset = t;
	if (!t) {
		_roof_heading->label(NULL);
		return;
	}
	std::string label("Roof: ");
	label = label + t->roof_name();
	_roof_heading->copy_label(label.c_str());
	for (uint8_t i = 0; i < NUM_ROOF_TILES; i++) {
		uint8_t id = i + FIRST_ROOF_TILE_ID;
		const Tile *t = _tileset->const_roof_tile(id);
		_deep_tile_buttons[i]->copy(t);
		_deep_tile_buttons[i]->activate();
	}
}

void Roof_Window::show(const Fl_Widget *p) {
	initialize();
	refresh();
	int x = p->x() + (p->w() - _window->w()) / 2;
	int y = p->y() + (p->h() - _window->h()) / 2;
	_window->position(x, y);
	_ok_button->take_focus();
	_window->show();
	while (_window->shown()) { Fl::wait(); }
}

void Roof_Window::apply_modifications() {
	for (int i = 0; i < NUM_ROOF_TILES; i++) {
		const Tile *t = _deep_tile_buttons[i];
		uint8_t id = t->id();
		_tileset->roof_tile(id)->copy(t);
	}
	_tileset->modified_roof(true);
}

void Roof_Window::select(Deep_Tile_Button *dtb) {
	_selected = dtb;
	_selected->setonly();

	char buffer[32];
	sprintf(buffer, "Tile: $%02X", _selected->id());
	_tile_heading->copy_label(buffer);

	Lighting l = _tileset->lighting();
	Palette p = _selected->palette();
	for (int y = 0; y < TILE_SIZE; y++) {
		for (int x = 0; x < TILE_SIZE; x++) {
			Pixel_Button *pb = _pixels[y * TILE_SIZE + x];
			Hue h = _selected->hue(x, y);
			pb->coloring(l, p, h);
		}
	}
	_swatch1->coloring(l, p, Hue::WHITE);
	_swatch2->coloring(l, p, Hue::LIGHT);
	_swatch3->coloring(l, p, Hue::DARK);
	_swatch4->coloring(l, p, Hue::BLACK);
}

void Roof_Window::choose(Swatch *swatch) {
	_chosen = swatch;
	_chosen->setonly();
}

void Roof_Window::flood_fill(Pixel_Button *pb, Hue f, Hue t) {
	if (f == t) { return; }
	std::queue<size_t> queue;
	int row = pb->row(), col = pb->col();
	size_t i = row * TILE_SIZE + col;
	queue.push(i);
	while (!queue.empty()) {
		size_t i = queue.front();
		queue.pop();
		Pixel_Button *pb = _pixels[i];
		if (pb->hue() != f) { continue; }
		pb->hue(t); // fill
		int row = pb->row(), col = pb->col();
		if (col > 0) { queue.push(i-1); } // left
		if (col < TILE_SIZE - 1) { queue.push(i+1); } // right
		if (row > 0) { queue.push(i-TILE_SIZE); } // up
		if (row < TILE_SIZE - 1) { queue.push(i+TILE_SIZE); } // down
	}
}

void Roof_Window::substitute_hue(Hue f, Hue t) {
	for (size_t i = 0; i < TILE_SIZE * TILE_SIZE; i++) {
		Pixel_Button *pb = _pixels[i];
		if (pb->hue() == f) {
			pb->hue(t);
		}
	}
}

void Roof_Window::close_cb(Fl_Widget *, Roof_Window *rw) {
	rw->_window->hide();
}

void Roof_Window::cancel_cb(Fl_Widget *w, Roof_Window *rw) {
	rw->_canceled = true;
	close_cb(w, rw);
}

void Roof_Window::select_tile_cb(Deep_Tile_Button *dtb, Roof_Window *rw) {
	// Click to select
	rw->select(dtb);
	rw->_window->redraw();
}

void Roof_Window::choose_swatch_cb(Swatch *s, Roof_Window *rw) {
	// Click to choose
	rw->choose(s);
	rw->_window->redraw();
}

void Roof_Window::change_pixel_cb(Pixel_Button *pb, Roof_Window *rw) {
	if (Fl::event_button() == FL_LEFT_MOUSE) {
		if (!rw->_chosen) { return; }
		if (Fl::event_shift()) {
			// Shift+left-click to flood fill
			rw->flood_fill(pb, pb->hue(), rw->_chosen->hue());
			rw->_tile_group->redraw();
			rw->_selected->copy_pixels(rw->_pixels);
			rw->_selected->redraw();
		}
		else if (Fl::event_ctrl()) {
			// Ctrl+left-click to replace
			rw->substitute_hue(pb->hue(), rw->_chosen->hue());
			rw->_tile_group->redraw();
			rw->_selected->copy_pixels(rw->_pixels);
			rw->_selected->redraw();
		}
		else {
			// Left-click/drag to edit
			Hue h = rw->_chosen->hue();
			pb->hue(h);
			pb->damage(1);
			rw->_selected->copy_pixel(pb);
			rw->_selected->redraw();
		}
	}
	else if (Fl::event_button() == FL_RIGHT_MOUSE) {
		// Right-click to choose
		Hue h = pb->hue();
		switch (h) {
		case Hue::WHITE:
			rw->choose(rw->_swatch1);
			break;
		case Hue::LIGHT:
			rw->choose(rw->_swatch2);
			break;
		case Hue::DARK:
			rw->choose(rw->_swatch3);
			break;
		case Hue::BLACK:
			rw->choose(rw->_swatch4);
			break;
		}
	}
}

void Roof_Window::copy_tile_cb(Fl_Widget *, Roof_Window *rw) {
	if (!rw->_selected) { return; }
	rw->_clipboard.id(rw->_selected->id());
	rw->_clipboard.copy(rw->_selected);
	rw->_copied = true;
}

void Roof_Window::paste_tile_cb(Fl_Widget *, Roof_Window *rw) {
	if (!rw->_copied || !rw->_selected) { return; }
	rw->_selected->copy(&rw->_clipboard);
	rw->select(rw->_selected);
	rw->_window->redraw();
}

void Roof_Window::swap_tiles_cb(Fl_Widget *, Roof_Window *rw) {
	if (!rw->_copied || !rw->_selected) { return; }
	uint8_t cid = rw->_clipboard.id();
	uint8_t i = cid - FIRST_ROOF_TILE_ID;
	Deep_Tile_Button *copied = rw->_deep_tile_buttons[i];
	Tile temp(0);
	temp.copy(rw->_selected);
	rw->_selected->copy(copied);
	copied->copy(&temp);
	rw->_window->redraw();
}
