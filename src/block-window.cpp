#include "themes.h"
#include "config.h"
#include "tile.h"
#include "icons.h"
#include "image.h"
#include "block-window.h"

Block_Double_Window::Block_Double_Window(int x, int y, int w, int h, const char *l) : Fl_Double_Window(x, y, w, h, l) {}

int Block_Double_Window::handle(int event) {
	Block_Window *bw = (Block_Window *)user_data();
	switch (event) {
	case FL_FOCUS:
	case FL_UNFOCUS:
		return 1;
	case FL_KEYUP:
	case FL_KEYDOWN:
		switch (Fl::event_key()) {
		case FL_Control_L:
		case FL_Control_R:
		case FL_Alt_L:
		case FL_Alt_R:
		case FL_Shift_L:
		case FL_Shift_R:
			bw->update_multiedit();
			return 1;
		}
	}
	return Fl_Double_Window::handle(event);
}

Block_Window::Block_Window(int x, int y) : _dx(x), _dy(y), _tileset(NULL), _metatile_id(0), _canceled(false),
	_show_priority(false), _window(NULL), _tileset_heading(NULL), _tile_heading(NULL), _multiedit_heading(NULL),
	_metatile_heading(NULL), _hover_tile_heading(NULL), _tileset_group(NULL), _metatile_group(NULL), _tile_buttons(),
	_selected(NULL), _chips(), _collision_inputs(), _bin_collision_spinners(), _ok_button(NULL), _cancel_button(NULL) {}

Block_Window::~Block_Window() {
	delete _window;
	delete _tileset_heading;
	delete _tile_heading;
	delete _multiedit_heading;
	delete _metatile_heading;
	delete _hover_tile_heading;
	delete _tileset_group;
	delete _metatile_group;
	delete _cancel_button;
	delete _ok_button;
}

void Block_Window::initialize() {
	if (_window) { return; }
	Fl_Group *prev_current = Fl_Group::current();
	Fl_Group::current(NULL);
	// Populate window
	_window = new Block_Double_Window(_dx, _dy, 466, 304, "Edit Block");
	int thw = text_width("Tile: $FFF", 2);
	_tile_heading = new Label(268-thw, 10, thw, 22);
	int mhw = text_width("Place 4+4+4+4", 2);
	_multiedit_heading = new Label(268-mhw, 10, mhw, 22);
	_tileset_heading = new Label(10, 10, 258-thw, 22);
	_metatile_heading = new Label(278, 10, 98, 22);
	_hover_tile_heading = new Label(376, 10, thw, 22);
	_tileset_group = new Fl_Group(10, 36, 258, 258);
	_tileset_group->end();
	_window->begin();
	_metatile_group = new Fl_Group(278, 36, 98, 98);
	_metatile_group->end();
	_window->begin();
	_collision_inputs[(int)Quadrant::TOP_LEFT]     = new OS_Input(300, 144, 156, 22);
	_collision_inputs[(int)Quadrant::TOP_RIGHT]    = new OS_Input(300, 170, 156, 22);
	_collision_inputs[(int)Quadrant::BOTTOM_LEFT]  = new OS_Input(300, 196, 156, 22);
	_collision_inputs[(int)Quadrant::BOTTOM_RIGHT] = new OS_Input(300, 222, 156, 22);
	int bsw = std::max(text_width("AA", 2), text_width("FF", 2)) + 22;
	_bin_collision_spinners[(int)Quadrant::TOP_LEFT]     = new Default_Hex_Spinner(300, 144, bsw, 22);
	_bin_collision_spinners[(int)Quadrant::TOP_RIGHT]    = new Default_Hex_Spinner(332+bsw, 144, bsw, 22);
	_bin_collision_spinners[(int)Quadrant::BOTTOM_LEFT]  = new Default_Hex_Spinner(300, 170, bsw, 22);
	_bin_collision_spinners[(int)Quadrant::BOTTOM_RIGHT] = new Default_Hex_Spinner(332+bsw, 170, bsw, 22);
	_ok_button = new Default_Button(282, 272, 80, 22, "OK");
	_cancel_button = new OS_Button(376, 272, 80, 22, "Cancel");
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
	for (uint8_t y = 0; y < METATILE_SIZE; y++) {
		for (uint8_t x = 0; x < METATILE_SIZE; x++) {
			int cx = _metatile_group->x() + 1 + x * CHIP_PX_SIZE, cy = _metatile_group->y() + 1 + y * CHIP_PX_SIZE;
			int id = y * METATILE_SIZE + x;
			Chip *c = new Chip(cx, cy, CHIP_PX_SIZE, y, x);
			c->callback((Fl_Callback *)change_chip_cb, this);
			_chips[id] = c;
		}
	}
	_metatile_group->end();
	// Initialize window
	_window->callback((Fl_Callback *)close_cb, this);
	_window->set_modal();
	// Initialize window's children
	_multiedit_heading->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
	_multiedit_heading->hide();
	_tileset_group->box(OS_SPACER_THIN_DOWN_FRAME);
	_metatile_group->box(OS_SPACER_THIN_DOWN_FRAME);
	_collision_inputs[(int)Quadrant::TOP_LEFT]->image(COLL_TOP_LEFT_ICON);
	_collision_inputs[(int)Quadrant::TOP_RIGHT]->image(COLL_TOP_RIGHT_ICON);
	_collision_inputs[(int)Quadrant::BOTTOM_LEFT]->image(COLL_BOTTOM_LEFT_ICON);
	_collision_inputs[(int)Quadrant::BOTTOM_RIGHT]->image(COLL_BOTTOM_RIGHT_ICON);
	_bin_collision_spinners[(int)Quadrant::TOP_LEFT]->image(COLL_TOP_LEFT_ICON);
	_bin_collision_spinners[(int)Quadrant::TOP_RIGHT]->image(COLL_TOP_RIGHT_ICON);
	_bin_collision_spinners[(int)Quadrant::BOTTOM_LEFT]->image(COLL_BOTTOM_LEFT_ICON);
	_bin_collision_spinners[(int)Quadrant::BOTTOM_RIGHT]->image(COLL_BOTTOM_RIGHT_ICON);
	_ok_button->tooltip("OK (Enter)");
	_ok_button->callback((Fl_Callback *)close_cb, this);
	_cancel_button->tooltip("Cancel (Esc)");
	_cancel_button->shortcut(FL_Escape);
	_cancel_button->callback((Fl_Callback *)cancel_cb, this);
	Fl_Group::current(prev_current);
}

void Block_Window::refresh() {
	_canceled = false;
	_selected = _tile_buttons[0];
	_selected->setonly();
	_selected->do_callback();
}

void Block_Window::update_icons() {
	initialize();
	bool dark = OS::is_dark_theme(OS::current_theme());
	_collision_inputs[(int)Quadrant::TOP_LEFT]->image(dark ? COLL_TOP_LEFT_DARK_ICON : COLL_TOP_LEFT_ICON);
	_collision_inputs[(int)Quadrant::TOP_RIGHT]->image(dark ? COLL_TOP_RIGHT_DARK_ICON : COLL_TOP_RIGHT_ICON);
	_collision_inputs[(int)Quadrant::BOTTOM_LEFT]->image(dark ? COLL_BOTTOM_LEFT_DARK_ICON : COLL_BOTTOM_LEFT_ICON);
	_collision_inputs[(int)Quadrant::BOTTOM_RIGHT]->image(dark ? COLL_BOTTOM_RIGHT_DARK_ICON : COLL_BOTTOM_RIGHT_ICON);
	_bin_collision_spinners[(int)Quadrant::TOP_LEFT]->image(dark ? COLL_TOP_LEFT_DARK_ICON : COLL_TOP_LEFT_ICON);
	_bin_collision_spinners[(int)Quadrant::TOP_RIGHT]->image(dark ? COLL_TOP_RIGHT_DARK_ICON : COLL_TOP_RIGHT_ICON);
	_bin_collision_spinners[(int)Quadrant::BOTTOM_LEFT]->image(dark ? COLL_BOTTOM_LEFT_DARK_ICON : COLL_BOTTOM_LEFT_ICON);
	_bin_collision_spinners[(int)Quadrant::BOTTOM_RIGHT]->image(dark ? COLL_BOTTOM_RIGHT_DARK_ICON : COLL_BOTTOM_RIGHT_ICON);
	Image::make_deimage(_collision_inputs[(int)Quadrant::TOP_LEFT]);
	Image::make_deimage(_collision_inputs[(int)Quadrant::TOP_RIGHT]);
	Image::make_deimage(_collision_inputs[(int)Quadrant::BOTTOM_LEFT]);
	Image::make_deimage(_collision_inputs[(int)Quadrant::BOTTOM_RIGHT]);
	Image::make_deimage(_bin_collision_spinners[(int)Quadrant::TOP_LEFT]);
	Image::make_deimage(_bin_collision_spinners[(int)Quadrant::TOP_RIGHT]);
	Image::make_deimage(_bin_collision_spinners[(int)Quadrant::BOTTOM_LEFT]);
	Image::make_deimage(_bin_collision_spinners[(int)Quadrant::BOTTOM_RIGHT]);
}

void Block_Window::tileset(const Tileset *t) {
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

void Block_Window::metatile(const Metatile *mt, bool has_collisions, bool bin_collisions) {
	_metatile_id = mt->id();
	for (int y = 0; y < METATILE_SIZE; y++) {
		for (int x = 0; x < METATILE_SIZE; x++) {
			int i = y * METATILE_SIZE + x;
			uint8_t id = mt->tile_id(x, y);
			_chips[i]->id(id);
		}
	}
	for (int i = 0; i < NUM_QUADRANTS; i++) {
		OS_Input *cin = _collision_inputs[i];
		Default_Hex_Spinner *bin = _bin_collision_spinners[i];
		if (bin_collisions) {
			cin->hide();
			cin->deactivate();
			bin->show();
			if (has_collisions) {
				bin->default_value(mt->bin_collision((Quadrant)i));
				bin->activate();
			}
			else {
				bin->value(0);
				bin->deactivate();
			}
		}
		else {
			bin->hide();
			bin->deactivate();
			cin->show();
			if (has_collisions) {
				cin->value(mt->collision((Quadrant)i).c_str());
				cin->activate();
			}
			else {
				cin->value(NULL);
				cin->deactivate();
			}
			cin->position(0);
		}
	}
	char buffer[32];
	sprintf(buffer, "Block: $%02X", _metatile_id);
	_metatile_heading->copy_label(buffer);
}

void Block_Window::show(const Fl_Widget *p, bool show_priority) {
	initialize();
	refresh();
	_show_priority = show_priority;
	int x = p->x() + (p->w() - _window->w()) / 2;
	int y = p->y() + (p->h() - _window->h()) / 2;
	_window->position(x, y);
	_ok_button->take_focus();
	_window->show();
	while (_window->shown()) { Fl::wait(); }
}

void Block_Window::draw_tile(uint8_t id, int x, int y, int s) const {
	const Tile *t = _tileset->const_tile_or_roof(id);
	t->draw_with_priority(x, y, s, _show_priority);
}

void Block_Window::update_status(const Chip *c) {
	if (!c) {
		_hover_tile_heading->label("");
	}
	else {
		char buffer[16] = {};
		sprintf(buffer, "Tile: $%02X", c->id());
		_hover_tile_heading->copy_label(buffer);
	}
	_hover_tile_heading->redraw();
}

void Block_Window::update_multiedit() {
	const char *place_labels[8] = {
		"",              // no modifiers
		"Place 2/2",     // Shift
		"Place 2x2",     // Ctrl
		"Place 4x4",     // Ctrl+Shift
		"Place 2+2",     // Alt
		"Place 4+4+4+4", // Alt+Shift
		"Place 2-2",     // Ctrl+Alt
		"Place 4-4-4-4", // Ctrl+Alt+Shift
	};
	int i = (Fl::event_alt() ? 4 : 0) | (Fl::event_ctrl() ? 2 : 0) | (Fl::event_shift() ? 1 : 0);
	_multiedit_heading->label(place_labels[i]);
	if (i) {
		_tile_heading->hide();
		_multiedit_heading->show();
	}
	else {
		_tile_heading->show();
		_multiedit_heading->hide();
	}
	_tileset_heading->redraw();
	_tile_heading->redraw();
	_multiedit_heading->redraw();
}

void Block_Window::close_cb(Fl_Widget *, Block_Window *bw) {
	bw->_window->hide();
}

void Block_Window::cancel_cb(Fl_Widget *w, Block_Window *bw) {
	bw->_canceled = true;
	close_cb(w, bw);
}

void Block_Window::select_tile_cb(Tile_Button *tb, Block_Window *bw) {
	// Click to select
	bw->_selected = tb;
	char buffer[16] = {};
	sprintf(buffer, "Tile: $%02X", tb->id());
	bw->_tile_heading->copy_label(buffer);
}

void Block_Window::change_chip_cb(Chip *c, Block_Window *bw) {
	if (Fl::event_button() == FL_LEFT_MOUSE) {
		// Left-click to edit
		// Shift+left-click to edit 2/2
		// Ctrl+left-click to edit 2x2
		// Ctrl+Shift+left-click to edit 4x4
		// Alt+left-click to edit 2+2
		// Alt+Shift+left-click to edit 4+4+4+4
		// Ctrl+Alt+left-click to edit 2-2
		// Ctrl+Alt+Shift+left-click to edit 4-4-4-4
		uint8_t id = bw->_selected->id();
		bool ctrl = Fl::event_ctrl(), alt = Fl::event_alt(), shift = Fl::event_shift();
		bool checkered = !ctrl && !alt && shift;
		uint8_t n = ((ctrl || alt) ? 2 : 1) * (shift ? 2 : 1);
		bool allow_256_tiles = Config::allow_256_tiles();
		bool skip = !allow_256_tiles && id < 0x60;
		for (uint8_t dy = 0; dy < n; dy++) {
			for (uint8_t dx = 0; dx < n; dx++) {
				uint8_t y = c->row() + dy, x = c->col() + dx;
				uint8_t tid = id + (checkered ? dy ^ dx : (ctrl ? alt ? -n : TILES_PER_ROW : n) * dy + dx);
				if (skip && tid >= 0x60) { tid += 0x20; }
				if (y < METATILE_SIZE && x < METATILE_SIZE && (allow_256_tiles || tid < 0xE0)) {
					Chip *chip = bw->chip(x, y);
					chip->id(tid);
					chip->damage(1);
				}
			}
		}
	}
	else if (Fl::event_button() == FL_RIGHT_MOUSE) {
		// Right-click to select
		uint8_t id = c->id();
		bw->_selected = bw->_tile_buttons[id];
		bw->_selected->setonly();
		bw->_selected->do_callback();
	}
}
