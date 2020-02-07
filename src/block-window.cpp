#include "themes.h"
#include "tile.h"
#include "icons.h"
#include "image.h"
#include "block-window.h"

Block_Window::Block_Window(int x, int y) : _dx(x), _dy(y), _tileset(NULL), _metatile_id(0), _canceled(false),
	_window(NULL), _tileset_heading(NULL), _tile_heading(NULL), _metatile_heading(NULL), _hover_tile_heading(NULL),
	_collision_heading(NULL), _tileset_group(NULL), _current_group(NULL), _metatile_group(NULL), _tile_buttons(),
	_selected(NULL), _chips(), _current(NULL), _palette(NULL), _x_flip(NULL), _y_flip(NULL), _priority(NULL),
	_collision_inputs(), _bin_collision_spinners(), _ok_button(NULL), _cancel_button(NULL) {}

Block_Window::~Block_Window() {
	delete _window;
	delete _tileset_heading;
	delete _tile_heading;
	delete _metatile_heading;
	delete _hover_tile_heading;
	delete _collision_heading;
	delete _tileset_group;
	delete _current_group;
	delete _metatile_group;
	delete _x_flip;
	delete _y_flip;
	delete _priority;
	delete _cancel_button;
	delete _ok_button;
}

void Block_Window::initialize() {
	if (_window) { return; }
	Fl_Group *prev_current = Fl_Group::current();
	Fl_Group::current(NULL);
	// Populate window
	_window = new Fl_Double_Window(_dx, _dy, 477, 432, "Edit Block");
	int thw = text_width("Tile: $FFFF", 2);
	_tileset_heading = new Label(10, 10, 273, 22);
	_tile_heading = new Label(328-thw, 10, thw, 22);
	_metatile_heading = new Label(337, 10, 130-thw, 22);
	_hover_tile_heading = new Label(467-thw, 10, thw, 22);
	_collision_heading = new Label(293, 253, 174, 22, "Collision:");
	_tileset_group = new Workspace(10, 36, 273, 258);
	_tileset_group->end();
	_window->begin();
	_current_group = new Fl_Group(293, 36, 34, 34);
	_current = new Chip(294, 37, CHIP_PX_SIZE, 0, 0);
	_current_group->end();
	_window->begin();
	_metatile_group = new Fl_Group(337, 36, 130, 130);
	_metatile_group->end();
	_window->begin();
	int off = text_width("Palette:", 3);
	_palette = new Dropdown(293+off, 176, 174-off, 22, "Palette:");
	off = MAX(text_width("X flip", 3), text_width("Y flip", 3));
	_x_flip = new OS_Check_Button(293, 202, 22+off, 22, "X flip");
	_y_flip = new OS_Check_Button(319+off, 202, 22+off, 22, "Y flip");
	_priority = new OS_Check_Button(293, 228, 174, 22, "Priority (above sprites)");
	_collision_inputs[(int)Quadrant::TOP_LEFT]     = new OS_Input(313, 279, 154, 22);
	_collision_inputs[(int)Quadrant::TOP_RIGHT]    = new OS_Input(313, 305, 154, 22);
	_collision_inputs[(int)Quadrant::BOTTOM_LEFT]  = new OS_Input(313, 331, 154, 22);
	_collision_inputs[(int)Quadrant::BOTTOM_RIGHT] = new OS_Input(313, 357, 154, 22);
	int bsw = MAX(text_width("AA", 2), text_width("FF", 2)) + 22;
	_bin_collision_spinners[(int)Quadrant::TOP_LEFT] = new Default_Hex_Spinner(313, 279, bsw, 22);
	_bin_collision_spinners[(int)Quadrant::TOP_RIGHT] = new Default_Hex_Spinner(347+bsw, 279, bsw, 22);
	_bin_collision_spinners[(int)Quadrant::BOTTOM_LEFT] = new Default_Hex_Spinner(313, 305, bsw, 22);
	_bin_collision_spinners[(int)Quadrant::BOTTOM_RIGHT] = new Default_Hex_Spinner(347+bsw, 305, bsw, 22);
	_ok_button = new Default_Button(293, 400, 80, 22, "OK");
	_cancel_button = new OS_Button(387, 400, 80, 22, "Cancel");
	_window->end();
	// Populate tileset group
	_tileset_group->begin();
	for (int y = 0; y < TILES_PER_COL; y++) {
		for (int x = 0; x < TILES_PER_ROW; x++) {
			int bx = _tileset_group->x() + 1 + x * TILE_PX_SIZE, by = _tileset_group->y() + 1 + y * TILE_PX_SIZE;
			uint16_t id = (uint16_t)(y * TILES_PER_ROW + x);
			Tile_Button *tb = new Tile_Button(bx, by, TILE_PX_SIZE, id);
			tb->callback((Fl_Callback *)select_tile_cb, this);
			_tile_buttons[id] = tb;
		}
	}
	_tileset_group->end();
	_tileset_group->init_sizes();
	_tileset_group->contents(TILES_PER_ROW * TILE_PX_SIZE, TILES_PER_COL * TILE_PX_SIZE);
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
	_tileset_group->type(Fl_Scroll::VERTICAL_ALWAYS);
	_tileset_group->box(OS_SPACER_THIN_DOWN_FRAME);
	_current_group->box(OS_SPACER_THIN_DOWN_FRAME);
	_metatile_group->box(OS_SPACER_THIN_DOWN_FRAME);
	_current->deactivate();
	_current->user_data(this);
	_palette->align(FL_ALIGN_LEFT);
	_palette->add("GRAY");
	_palette->add("RED");
	_palette->add("GREEN");
	_palette->add("WATER");
	_palette->add("YELLOW");
	_palette->add("BROWN");
	_palette->add("ROOF");
	_palette->add("TEXT");
	_palette->callback((Fl_Callback *)change_attributes_cb, this);
	_priority->callback((Fl_Callback *)change_attributes_cb, this);
	_x_flip->callback((Fl_Callback *)change_attributes_cb, this);
	_y_flip->callback((Fl_Callback *)change_attributes_cb, this);
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
	_tileset_group->scroll_to(0, 0);
	Tile_Button *tb = _tile_buttons[0];
	tb->Attributable::clear();
	select(tb);
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
			const Attributable *a = mt->attributes(x, y);
			_chips[i]->copy(*a);
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

void Block_Window::show(const Fl_Widget *p) {
	initialize();
	refresh();
	int x = p->x() + (p->w() - _window->w()) / 2;
	int y = p->y() + (p->h() - _window->h()) / 2;
	_window->position(x, y);
	_ok_button->take_focus();
	_window->show();
	while (_window->shown()) { Fl::wait(); }
}

void Block_Window::select(const Attributable *a) {
	uint16_t id = a->id();
	_selected = _tile_buttons[id];
	_selected->setonly();
	if (TILE_PX_SIZE * (id / TILES_PER_ROW) >= _tileset_group->yposition() + _tileset_group->h() - TILE_PX_SIZE / 2) {
		_tileset_group->scroll_to(0, TILE_PX_SIZE * (id / TILES_PER_ROW + 1) - _tileset_group->h());
	}
	else if (TILE_PX_SIZE * (id / TILES_PER_ROW + 1) <= _tileset_group->yposition() + TILE_PX_SIZE / 2) {
		_tileset_group->scroll_to(0, TILE_PX_SIZE * (id / TILES_PER_ROW));
	}
	_selected->do_callback();
	_palette->value((int)a->palette());
	_x_flip->value(a->x_flip());
	_y_flip->value(a->y_flip());
	_priority->value(a->priority());
	_palette->do_callback();
}

void Block_Window::update_status(Chip *c) {
	if (!c) {
		_hover_tile_heading->label("");
	}
	else {
		char buffer[32] = {};
		sprintf(buffer, "Tile: $%03X", c->id());
		_hover_tile_heading->copy_label(buffer);
	}
	_hover_tile_heading->redraw();
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
	bw->_current->copy(*tb);
	bw->_current->redraw();
	char buffer[32] = {};
	sprintf(buffer, "Tile: $%03X", tb->id());
	bw->_tile_heading->copy_label(buffer);
}

void Block_Window::change_chip_cb(Chip *c, Block_Window *bw) {
	if (Fl::event_button() == FL_LEFT_MOUSE) {
		// Left-click to edit
		// Ctrl+left-click to edit 2x2
		// Ctrl+Shift+left-click to edit 4x4
		uint16_t id = bw->_selected->id();
		uint16_t n = Fl::event_ctrl() ? Fl::event_shift() ? 4 : 2 : 1;
		for (uint16_t dy = 0; dy < n; dy++) {
			for (uint16_t dx = 0; dx < n; dx++) {
				uint16_t y = c->row() + (bw->_y_flip->value() ? n - dy - 1 : dy);
				uint16_t x = c->col() + (bw->_x_flip->value() ? n - dx - 1 : dx);
				bool row_free = y < METATILE_SIZE && id < MAX_NUM_TILES - TILES_PER_ROW * dy;
				bool col_free = x < METATILE_SIZE && id % TILES_PER_ROW < TILES_PER_ROW - dx;
				if (row_free && col_free) {
					Chip *chip = bw->chip(x, y);
					chip->id(id + TILES_PER_ROW * dy + dx);
					chip->palette((Palette)bw->_palette->value());
					chip->priority(!!bw->_priority->value());
					chip->x_flip(!!bw->_x_flip->value());
					chip->y_flip(!!bw->_y_flip->value());
					chip->damage(1);
				}
			}
		}
	}
	else if (Fl::event_button() == FL_RIGHT_MOUSE) {
		// Right-click to select
		bw->select(c);
		bw->_window->redraw();
	}
}

void Block_Window::change_attributes_cb(Fl_Widget *, Block_Window *bw) {
	for (int i = 0; i < MAX_NUM_TILES; i++) {
		Tile_Button *tb = bw->_tile_buttons[i];
		tb->palette((Palette)bw->_palette->value());
		tb->x_flip(!!bw->_x_flip->value());
		tb->y_flip(!!bw->_y_flip->value());
		tb->priority(!!bw->_priority->value());
		tb->damage(1);
	}
	bw->_current->palette((Palette)bw->_palette->value());
	bw->_current->x_flip(!!bw->_x_flip->value());
	bw->_current->y_flip(!!bw->_y_flip->value());
	bw->_current->priority(!!bw->_priority->value());
	bw->_current->damage(1);
}
