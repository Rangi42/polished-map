#include <queue>
#include <utility>

#include "themes.h"
#include "config.h"
#include "tileset-window.h"

Tile_Window::Tile_Window(int x, int y, int w, int h, const char *l) : Fl_Double_Window(x, y, w, h, l) {}

int Tile_Window::handle(int event) {
	Tileset_Window *tw = (Tileset_Window *)user_data();
	if (Fl::test_shortcut(FL_COMMAND + 'c')) {
		Tileset_Window::copy_tile_cb(NULL, tw);
	}
	else if (Fl::test_shortcut(FL_COMMAND + 'v')) {
		Tileset_Window::paste_tile_cb(NULL, tw);
	}
	else if (Fl::test_shortcut(FL_COMMAND + 'x')) {
		Tileset_Window::swap_tiles_cb(NULL, tw);
	}
	else if (Fl::test_shortcut(FL_Delete)) {
		Tileset_Window::delete_tile_cb(NULL, tw);
	}
	return Fl_Double_Window::handle(event);
}

Tileset_Window::Tileset_Window(int x, int y) : _dx(x), _dy(y), _tileset(NULL), _canceled(false), _window(NULL),
	_tileset_heading(NULL), _tile_heading(NULL), _tileset_group(NULL), _tile_group(NULL), _deep_tile_buttons(),
	_selected(NULL), _pixels(), _swatch1(NULL), _swatch2(NULL), _swatch3(NULL), _swatch4(NULL), _chosen(NULL),
	_palette(NULL), _priority(NULL), _ok_button(NULL), _cancel_button(NULL), _copied(false), _clipboard(0) {}

Tileset_Window::~Tileset_Window() {
	delete _window;
	delete _tileset_heading;
	delete _tile_heading;
	delete _tileset_group;
	delete _tile_group;
	delete _swatch1;
	delete _swatch2;
	delete _swatch3;
	delete _swatch4;
	delete _palette;
	delete _priority;
	delete _ok_button;
	delete _cancel_button;
}

void Tileset_Window::initialize() {
	if (_window) { return; }
	Fl_Group *prev_current = Fl_Group::current();
	Fl_Group::current(NULL);
	// Populate window
	_window = new Tile_Window(_dx, _dy, 466, 304, "Edit Tileset");
	_tileset_heading = new Label(10, 10, 258, 22);
	_tile_heading = new Label(278, 10, 146, 22);
	_tileset_group = new Fl_Group(10, 36, 258, 258);
	_tileset_group->end();
	_window->begin();
	_tile_group = new Fl_Group(278, 36, 146, 146);
	_tile_group->end();
	_window->begin();
	_swatch1 = new Swatch(434, 36, 22, "1");
	_swatch2 = new Swatch(434, 64, 22, "2");
	_swatch3 = new Swatch(434, 92, 22, "3");
	_swatch4 = new Swatch(434, 120, 22, "4");
	int off = text_width("Color:", 3);
	_palette = new Dropdown(278 + off, 192, 146 - off, 22, "Color:");
	_priority = new OS_Check_Button(278, 218, 178, 22, "Priority (above sprites)");
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
	_tileset_group->box(OS_SPACER_THIN_DOWN_FRAME);
	_tile_group->box(OS_SPACER_THIN_DOWN_FRAME);
	_swatch1->shortcut('1');
	_swatch1->callback((Fl_Callback *)choose_swatch_cb, this);
	_swatch2->shortcut('2');
	_swatch2->callback((Fl_Callback *)choose_swatch_cb, this);
	_swatch3->shortcut('3');
	_swatch3->callback((Fl_Callback *)choose_swatch_cb, this);
	_swatch4->shortcut('4');
	_swatch4->callback((Fl_Callback *)choose_swatch_cb, this);
	_palette->align(FL_ALIGN_LEFT);
	_palette->add("GRAY");
	_palette->add("RED");
	_palette->add("GREEN");
	_palette->add("WATER");
	_palette->add("YELLOW");
	_palette->add("BROWN");
	_palette->add("ROOF");
	_palette->add("TEXT");
	_palette->add("MONOCHROME", 0, NULL, 0, FL_MENU_INACTIVE | FL_MENU_INVISIBLE);
	_palette->add("UNDEFINED", 0, NULL, 0, FL_MENU_INACTIVE | FL_MENU_INVISIBLE);
	_palette->callback((Fl_Callback *)change_palette_cb, this);
	_priority->callback((Fl_Callback *)change_palette_cb, this);
	_ok_button->tooltip("OK (Enter)");
	_ok_button->callback((Fl_Callback *)close_cb, this);
	_cancel_button->tooltip("Cancel (Esc)");
	_cancel_button->shortcut(FL_Escape);
	_cancel_button->callback((Fl_Callback *)cancel_cb, this);
	Fl_Group::current(prev_current);
}

void Tileset_Window::refresh() {
	_canceled = false;
	_copied = false;
	select(_deep_tile_buttons[0]);
	choose(_swatch1);
}

void Tileset_Window::tileset(Tileset *t) {
	initialize();
	_tileset = t;
	if (!t) {
		_tileset_heading->label(NULL);
		return;
	}
	std::string label("Tileset: ");
	label = label + t->name();
	_tileset_heading->copy_label(label.c_str());
	for (int i = 0; i < MAX_NUM_TILES; i++) {
		const Tile *t = _tileset->const_tile((uint8_t)i);
		_deep_tile_buttons[i]->copy(t);
	}
	for (int i = 0x00; i < 0x100; i++) {
		_deep_tile_buttons[i]->activate();
	}
	if (!Config::allow_256_tiles()) {
		for (int i = 0x60; i < 0x80; i++) {
			_deep_tile_buttons[i]->deactivate();
		}
		for (int i = 0xe0; i < 0x100; i++) {
			_deep_tile_buttons[i]->deactivate();
		}
		if (Config::monochrome()) { // assume pokered's tileset size
			for (int i = 0x80; i < 0xe0; i++) {
				_deep_tile_buttons[i]->deactivate();
			}
		}
	}
	if (Config::monochrome()) {
		_palette->deactivate();
		_priority->deactivate();
	}
	else {
		_palette->activate();
		_priority->activate();
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

void Tileset_Window::apply_modifications() {
	Palette_Map &palette_map = _tileset->palette_map();
	for (int i = 0; i < MAX_NUM_TILES; i++) {
		const Tile *t = _deep_tile_buttons[i];
		uint8_t id = (uint8_t)i;
		_tileset->tile(id)->copy(t);
		palette_map.palette(id, t->palette());
	}
	_tileset->modified(true);
}

void Tileset_Window::select(Deep_Tile_Button *dtb) {
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

	_palette->value((int)p & 0xf);
	if ((int)p & 0x80) {
		_priority->set();
	}
	else {
		_priority->clear();
	}
}

void Tileset_Window::choose(Swatch *swatch) {
	_chosen = swatch;
	_chosen->setonly();
}

void Tileset_Window::flood_fill(Pixel_Button *pb, Hue f, Hue t) {
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

void Tileset_Window::substitute_hue(Hue f, Hue t) {
	for (size_t i = 0; i < TILE_SIZE * TILE_SIZE; i++) {
		Pixel_Button *pb = _pixels[i];
		if (pb->hue() == f) {
			pb->hue(t);
		}
	}
}

void Tileset_Window::palette(Palette p) {
	for (int i = 0; i < TILE_SIZE * TILE_SIZE; i++) {
		_pixels[i]->palette(p);
	}
	Lighting l = _tileset->lighting();
	_swatch1->coloring(l, p, Hue::WHITE);
	_swatch2->coloring(l, p, Hue::LIGHT);
	_swatch3->coloring(l, p, Hue::DARK);
	_swatch4->coloring(l, p, Hue::BLACK);
	_selected->copy_pixels(_pixels);
	_window->redraw();
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
	tw->select(dtb);
	tw->_window->redraw();
}

void Tileset_Window::choose_swatch_cb(Swatch *s, Tileset_Window *tw) {
	// Click to choose
	tw->choose(s);
	tw->_window->redraw();
}

void Tileset_Window::change_pixel_cb(Pixel_Button *pb, Tileset_Window *tw) {
	if (Fl::event_button() == FL_LEFT_MOUSE) {
		if (!tw->_chosen) { return; }
		if (Fl::event_shift()) {
			// Shift+left-click to flood fill
			tw->flood_fill(pb, pb->hue(), tw->_chosen->hue());
			tw->_tile_group->redraw();
			tw->_selected->copy_pixels(tw->_pixels);
			tw->_selected->redraw();
		}
		else if (Fl::event_ctrl()) {
			// Ctrl+left-click to replace
			tw->substitute_hue(pb->hue(), tw->_chosen->hue());
			tw->_tile_group->redraw();
			tw->_selected->copy_pixels(tw->_pixels);
			tw->_selected->redraw();
		}
		else {
			// Left-click/drag to edit
			Hue h = tw->_chosen->hue();
			pb->hue(h);
			pb->damage(1);
			tw->_selected->copy_pixel(pb);
			tw->_selected->redraw();
		}
	}
	else if (Fl::event_button() == FL_RIGHT_MOUSE) {
		// Right-click to choose
		Hue h = pb->hue();
		switch (h) {
		case Hue::WHITE:
			tw->choose(tw->_swatch1);
			break;
		case Hue::LIGHT:
			tw->choose(tw->_swatch2);
			break;
		case Hue::DARK:
			tw->choose(tw->_swatch3);
			break;
		case Hue::BLACK:
			tw->choose(tw->_swatch4);
			break;
		}
	}
}

void Tileset_Window::change_palette_cb(Fl_Widget *, Tileset_Window *tw) {
	int pv = tw->_palette->value();
	if (tw->_priority->value()) { pv |= 0x80; }
	tw->palette((Palette)pv);
}

void Tileset_Window::copy_tile_cb(Fl_Widget *, Tileset_Window *tw) {
	if (!tw->_selected) { return; }
	tw->_clipboard.id(tw->_selected->id());
	tw->_clipboard.copy(tw->_selected);
	tw->_copied = true;
}

void Tileset_Window::paste_tile_cb(Fl_Widget *, Tileset_Window *tw) {
	if (!tw->_copied || !tw->_selected) { return; }
	tw->_selected->copy(&tw->_clipboard);
	tw->select(tw->_selected);
	tw->_window->redraw();
}

void Tileset_Window::swap_tiles_cb(Fl_Widget *, Tileset_Window *tw) {
	if (!tw->_copied || !tw->_selected) { return; }
	uint8_t cid = tw->_clipboard.id();
	Deep_Tile_Button *copied = tw->_deep_tile_buttons[cid];
	Tile temp(0);
	temp.copy(tw->_selected);
	tw->_selected->copy(copied);
	copied->copy(&temp);
	tw->_window->redraw();
}

void Tileset_Window::delete_tile_cb(Fl_Widget *, Tileset_Window *tw) {
	if (!tw->_selected) { return; }
	tw->_selected->palette(Palette::UNDEFINED);
	const uchar *rgb = Color::color(tw->_tileset->lighting(), tw->_selected->palette(), Hue::WHITE);
	for (int y = 0; y < TILE_SIZE; y++) {
		for (int x = 0; x < TILE_SIZE; x++) {
			tw->_selected->pixel(x, y, Hue::WHITE, rgb[0], rgb[1], rgb[2]);
		}
	}
	tw->select(tw->_selected);
	tw->_window->redraw();
}
