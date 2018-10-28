#include "themes.h"
#include "tile.h"
#include "block-window.h"
#include "icons.h"

Block_Window::Block_Window(int x, int y) : _dx(x), _dy(y), _tileset(NULL), _metatile_id(0), _canceled(false),
	_window(NULL), _tileset_heading(NULL), _tile_heading(NULL), _metatile_heading(NULL), _hover_tile_heading(NULL),
	_tileset_group(NULL), _metatile_group(NULL), _tile_buttons(), _selected(NULL), _chips(), _collision_inputs(),
	_ok_button(NULL), _cancel_button(NULL) {}

Block_Window::~Block_Window() {
	delete _window;
	delete _tileset_heading;
	delete _tile_heading;
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
	_window = new Fl_Double_Window(_dx, _dy, 466, 304, "Edit Block");
	int thw = text_width("Tile: $FFF", 2);
	_tile_heading = new Label(268-thw, 10, thw, 22);
	_tileset_heading = new Label(10, 10, 258-thw, 22);
	_metatile_heading = new Label(278, 10, 98, 22);
	_hover_tile_heading = new Label(376, 10, thw, 22);
	_tileset_group = new Fl_Group(10, 36, 258, 258);
	_tileset_group->end();
	_window->begin();
	_metatile_group = new Fl_Group(278, 36, 98, 98);
	_metatile_group->end();
	_window->begin();
	_collision_inputs[Quadrant::TOP_LEFT]     = new OS_Input(300, 144, 156, 22);
	_collision_inputs[Quadrant::TOP_RIGHT]    = new OS_Input(300, 170, 156, 22);
	_collision_inputs[Quadrant::BOTTOM_LEFT]  = new OS_Input(300, 196, 156, 22);
	_collision_inputs[Quadrant::BOTTOM_RIGHT] = new OS_Input(300, 222, 156, 22);
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
	_tileset_group->box(OS_SPACER_THIN_DOWN_FRAME);
	_metatile_group->box(OS_SPACER_THIN_DOWN_FRAME);
	_collision_inputs[Quadrant::TOP_LEFT]->image(COLL_TOP_LEFT_ICON);
	_collision_inputs[Quadrant::TOP_LEFT]->deimage(COLL_TOP_LEFT_DISABLED_ICON);
	_collision_inputs[Quadrant::TOP_RIGHT]->image(COLL_TOP_RIGHT_ICON);
	_collision_inputs[Quadrant::TOP_RIGHT]->deimage(COLL_TOP_RIGHT_DISABLED_ICON);
	_collision_inputs[Quadrant::BOTTOM_LEFT]->image(COLL_BOTTOM_LEFT_ICON);
	_collision_inputs[Quadrant::BOTTOM_LEFT]->deimage(COLL_BOTTOM_LEFT_DISABLED_ICON);
	_collision_inputs[Quadrant::BOTTOM_RIGHT]->image(COLL_BOTTOM_RIGHT_ICON);
	_collision_inputs[Quadrant::BOTTOM_RIGHT]->deimage(COLL_BOTTOM_RIGHT_DISABLED_ICON);
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

void Block_Window::metatile(const Metatile *mt, bool has_collisions) {
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

void Block_Window::draw_tile(int x, int y, uint8_t id, bool zoom) const {
	const Tile *t = _tileset->const_tile_or_roof(id);
	const uchar *rgb = t->rgb();
	if (zoom) {
		uchar chip[CHIP_PX_SIZE * CHIP_PX_SIZE * NUM_CHANNELS] = {};
		for (int ty = 0; ty < TILE_SIZE; ty++) {
			for (int tx = 0; tx < TILE_SIZE; tx++) {
				int ti = (ty * LINE_BYTES + tx * NUM_CHANNELS) * ZOOM_FACTOR;
				int ci = (ty * CHIP_LINE_BYTES + tx * NUM_CHANNELS) * CHIP_ZOOM_FACTOR;
				for (int c = 0; c < NUM_CHANNELS; c++) {
					uchar v = rgb[ti + c];
					for (int row = 0; row < CHIP_ZOOM_FACTOR; row++) {
						for (int col = 0; col < CHIP_ZOOM_FACTOR; col++) {
							chip[ci + CHIP_LINE_BYTES * row + NUM_CHANNELS * col + c] = v;
						}
					}
				}
			}
		}
		fl_draw_image(chip, x, y, CHIP_PX_SIZE, CHIP_PX_SIZE, NUM_CHANNELS, CHIP_LINE_BYTES);
	}
	else {
		fl_draw_image(rgb, x, y, TILE_PX_SIZE, TILE_PX_SIZE, NUM_CHANNELS, LINE_BYTES);
	}
}

void Block_Window::update_status(Chip *c) {
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
		// Ctrl+left-click to edit 2x2
		// Ctrl+Shift+left-click to edit 4x4
		uint8_t id = bw->_selected->id();
		uint8_t n = Fl::event_ctrl() ? Fl::event_shift() ? 4 : 2 : 1;
		for (uint8_t dy = 0; dy < n; dy++) {
			for (uint8_t dx = 0; dx < n; dx++) {
				uint8_t y = c->row() + dy, x = c->col() + dx;
				bool row_free = y < METATILE_SIZE && id < MAX_NUM_TILES - TILES_PER_ROW * dy;
				bool col_free = x < METATILE_SIZE && id % TILES_PER_ROW < TILES_PER_ROW - dx;
				if (row_free && col_free) {
					Chip *chip = bw->chip(x, y);
					chip->id(id + TILES_PER_ROW * dy + dx);
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
	}
}
