#pragma warning(push, 0)
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "themes.h"
#include "main-window.h"
#include "block-window.h"
#include "map-buttons.h"

// 32x32 translucent zigzag pattern for tile priority
static uchar chip_priority_png_buffer[166] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x02, 0x03, 0x00, 0x00, 0x00, 0x0e, 0x14, 0x92,
	0x67, 0x00, 0x00, 0x00, 0x0c, 0x50, 0x4c, 0x54, 0x45, 0x20, 0x60, 0x60, 0x32, 0x7e, 0x7e, 0x56,
	0xba, 0xba, 0x68, 0xd8, 0xd8, 0xdc, 0xe3, 0x64, 0x6e, 0x00, 0x00, 0x00, 0x04, 0x74, 0x52, 0x4e,
	0x53, 0x60, 0x60, 0x60, 0x60, 0xe8, 0x2d, 0x50, 0x46, 0x00, 0x00, 0x00, 0x45, 0x49, 0x44, 0x41,
	0x54, 0x78, 0x5e, 0xcd, 0xce, 0x3b, 0x11, 0x00, 0x20, 0x0c, 0x04, 0xd1, 0xa5, 0x42, 0x06, 0x52,
	0x13, 0xa9, 0xc8, 0xa0, 0xe2, 0x98, 0x75, 0x41, 0xf1, 0x26, 0x45, 0x7e, 0x47, 0x6e, 0x45, 0xe4,
	0xac, 0x88, 0xec, 0x19, 0x91, 0x1e, 0x11, 0x17, 0x4a, 0x1c, 0x58, 0x62, 0xc3, 0x14, 0x0d, 0x43,
	0x16, 0x61, 0x43, 0x38, 0x2a, 0x5c, 0x16, 0x78, 0x4e, 0xf8, 0x40, 0xf8, 0x52, 0x18, 0x42, 0xbf,
	0xe5, 0x79, 0xbd, 0x49, 0x7f, 0x81, 0xfb, 0xa8, 0x04, 0xb5, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
	0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static Fl_PNG_Image chip_priority_png(NULL, chip_priority_png_buffer, 166);

static void draw_outlined_text(const char *l, int x, int y, int w, int h, Fl_Align a, Fl_Color c, Fl_Color s) {
	fl_color(s);
	fl_draw(l, x-1, y-1, w, h, a);
	fl_draw(l, x-1, y+1, w, h, a);
	fl_draw(l, x+1, y-1, w, h, a);
	fl_draw(l, x+1, y+1, w, h, a);
	fl_color(c);
	fl_draw(l, x, y, w, h, a);
}

static void draw_selection_border(int x, int y, int rs, bool zoom) {
	fl_rect(x, y, rs, rs, FL_BLACK);
	fl_rect(x+1, y+1, rs-2, rs-2, FL_WHITE);
	if (zoom) {
		fl_rect(x+2, y+2, rs-4, rs-4, FL_WHITE);
		fl_rect(x+3, y+3, rs-6, rs-6, FL_BLACK);
	}
	else {
		fl_rect(x+2, y+2, rs-4, rs-4, FL_BLACK);
	}
}

static void draw_map_button(Fl_Widget *wgt, uint8_t id, bool border) {
	Main_Window *mw = (Main_Window *)wgt->user_data();
	int x = wgt->x(), y = wgt->y();
	int ms = mw->metatile_size();
	mw->draw_metatile(x, y, id);
	if (mw->grid()) {
		fl_color(FL_INACTIVE_COLOR);
		fl_xyline(x, y+ms-1, x+ms-1, y);
	}
	if (border) {
		int rs = ms - (mw->grid() ? 1 : 0);
		draw_selection_border(x, y, rs, mw->zoom());
	}
	if (!mw->ids()) { return; }
	const char *l = wgt->label();
	int cx = x + (mw->zoom() ? 2 : 1) + 2, cy = y + (mw->zoom() ? 2 : 1);
	fl_font(FL_COURIER_BOLD, 14);
	draw_outlined_text(l, cx, cy, wgt->w(), wgt->h(), FL_ALIGN_TOP_LEFT | FL_ALIGN_INSIDE,
		border ? wgt->labelcolor() : FL_WHITE, FL_BLACK);
}

Metatile_Button *Metatile_Button::dragging = NULL;

Metatile_Button::Metatile_Button(int x, int y, int s, uint8_t id) : Fl_Radio_Button(x, y, s, s), _id(id) {
	user_data(NULL);
	box(FL_NO_BOX);
	labeltype(FL_NO_LABEL);
	labelcolor(FL_WHITE);
}

void Metatile_Button::id(uint8_t id) {
	_id = id;
	Main_Window *mw = (Main_Window *)user_data();
	char buffer[16];
	sprintf(buffer, (mw && mw->hex()) ? "%02X" : "%u", _id);
	copy_label(buffer);
}

void Metatile_Button::draw() {
	Main_Window *mw = (Main_Window *)user_data();
	draw_map_button(this, _id, !!value());
	auto s = mw->metatile_hotkey(_id);
	if (s == mw->no_hotkey()) { return; }
	int key = s->second;
	const char *l = fl_shortcut_label(key);
	int cx = x() - (mw->zoom() ? 2 : 1) - 2, cy = y() + (mw->zoom() ? 2 : 1);
	if (!mw->zoom() && mw->ids() && !mw->hex() && _id >= 100) { cy += 14; } // don't overlap three-digit IDs
	fl_font(FL_COURIER_BOLD, 14);
	draw_outlined_text(l, cx, cy, w(), h(), FL_ALIGN_TOP_RIGHT | FL_ALIGN_INSIDE, FL_RED, FL_WHITE);
}

int Metatile_Button::handle(int event) {
	switch (event) {
	case FL_ENTER:
	case FL_LEAVE:
		return 1;
	case FL_PUSH:
		value(1);
		set_changed();
		return 1;
	case FL_RELEASE:
	case FL_PASTE:
		do_callback();
		return 1;
	case FL_DRAG:
		if (Fl::event_button() == FL_LEFT_MOUSE) {
			dragging = this;
			Fl::dnd();
			return 1;
		}
		else {
			return 0;
		}
	case FL_DND_ENTER:
	case FL_DND_DRAG:
	case FL_DND_RELEASE:
		return 1;
	default:
		return 0;
	}
}

Block::Block(uint8_t row, uint8_t col, uint8_t id) : Fl_Box(0, 0, 0, 0), _row(row), _col(col), _id(id) {
	user_data(NULL);
	box(FL_NO_BOX);
	labeltype(FL_NO_LABEL);
	labelcolor(FL_YELLOW);
}

void Block::update_label() {
	Main_Window *mw = (Main_Window *)user_data();
	char buffer[16];
	sprintf(buffer, (mw && mw->hex()) ? "%02X" : "%u", _id);
	copy_label(buffer);
}

void Block::draw() {
	Main_Window *mw = (Main_Window *)user_data();
	bool below_mouse = Fl::belowmouse() == this, event_cursor = mw->mode() == Mode::EVENTS;
	draw_map_button(this, _id, below_mouse && !event_cursor);
	if (!below_mouse || !event_cursor) { return; }
	int hx = x() + right_half() * w() / 2, hy = y() + bottom_half() * h() / 2;
	int hs = mw->metatile_size() / 2;
	draw_selection_border(hx, hy, hs, mw->zoom());
}

int Block::handle(int event) {
	Main_Window *mw = (Main_Window *)user_data();
	switch (event) {
	case FL_ENTER:
		if (Fl::event_button1() && !Fl::pushed()) {
			Fl::pushed(this);
			do_callback();
		}
		mw->update_status(this);
		mw->update_gameboy_screen(this);
		return 1;
	case FL_LEAVE:
		mw->update_status((Block *)NULL);
		mw->update_gameboy_screen((Block *)NULL);
		redraw();
		return 1;
	case FL_MOVE:
		mw->update_event_cursor(this);
		if (mw->mode() == Mode::EVENTS && mw->gameboy_screen()) {
			mw->redraw_gameboy_screen();
		}
		else {
			redraw();
		}
		return 1;
	case FL_PUSH:
		mw->map_editable(true);
		do_callback();
		return 1;
	case FL_RELEASE:
		mw->map_editable(false);
		return 1;
	case FL_DRAG:
		if (!Fl::event_inside(x(), y(), w(), h())) {
			Fl::pushed(NULL);
		}
		return 1;
	}
	return Fl_Box::handle(event);
}

Tile_Button::Tile_Button(int x, int y, int s, uint8_t id) : Fl_Radio_Button(x, y, s, s), Attributable(id) {
	user_data(NULL);
	box(FL_NO_BOX);
	labeltype(FL_NO_LABEL);
	when(FL_WHEN_RELEASE);
}

void Tile_Button::draw() {
	Block_Window *bw = (Block_Window *)user_data();
	const Tileset *tileset = bw->tileset();
	const Tile *t = tileset->const_tile_or_roof(_id);
	fl_draw_image(t->rgb(_palette), x(), y(), TILE_PX_SIZE, TILE_PX_SIZE, NUM_CHANNELS, LINE_BYTES);
	if (value()) {
		draw_selection_border(x(), y(), TILE_PX_SIZE, false);
	}
}

Chip::Chip(int x, int y, int s, uint8_t row, uint8_t col) : Fl_Box(x, y, s, s), Attributable(0), _row(row), _col(col) {
	user_data(NULL);
	box(FL_NO_BOX);
	labeltype(FL_NO_LABEL);
	labelcolor(FL_YELLOW);
}

void Chip::draw() {
	Block_Window *bw = (Block_Window *)user_data();
	const Tileset *tileset = bw->tileset();
	const Tile *t = tileset->const_tile_or_roof(_id);
	const uchar *rgb = t->rgb(_palette);
	uchar chip[CHIP_PX_SIZE * CHIP_PX_SIZE * NUM_CHANNELS] = {};
	for (int ty = 0; ty < TILE_SIZE; ty++) {
		int my = _y_flip ? TILE_SIZE - ty - 1 : ty;
		for (int tx = 0; tx < TILE_SIZE; tx++) {
			int mx = _x_flip ? TILE_SIZE - tx - 1 : tx;
			int ti = (my * LINE_BYTES + mx * NUM_CHANNELS) * ZOOM_FACTOR;
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
	fl_draw_image(chip, x(), y(), CHIP_PX_SIZE, CHIP_PX_SIZE, NUM_CHANNELS, CHIP_LINE_BYTES);
	if (_priority) {
		chip_priority_png.draw(x(), y(), CHIP_PX_SIZE, CHIP_PX_SIZE);
	}
	if (_extra) {
		fl_rect(x(), y(), CHIP_PX_SIZE, CHIP_PX_SIZE, FL_CYAN);
		fl_rect(x()+1, y()+1, CHIP_PX_SIZE-2, CHIP_PX_SIZE-2, FL_CYAN);
	}
	if (active() && Fl::belowmouse() == this) {
		draw_selection_border(x(), y(), CHIP_PX_SIZE, false);
	}
}

int Chip::handle(int event) {
	Block_Window *bw = (Block_Window *)user_data();
	switch (event) {
	case FL_ENTER:
		if (Fl::event_button1() && !Fl::pushed()) {
			Fl::pushed(this);
			do_callback();
		}
		if (active()) {
			bw->update_status(this);
		}
		redraw();
		return 1;
	case FL_LEAVE:
		if (active()) {
			bw->update_status(NULL);
		}
		redraw();
		return 1;
	case FL_MOVE:
		redraw();
		return 1;
	case FL_PUSH:
		do_callback();
		return 1;
	case FL_RELEASE:
		return 1;
	case FL_DRAG:
		if (!Fl::event_inside(x(), y(), w(), h())) {
			Fl::pushed(NULL);
		}
		return 1;
	}
	return Fl_Box::handle(event);
}

Deep_Tile_Button *Deep_Tile_Button::_dragging = NULL;

Deep_Tile_Button::Deep_Tile_Button(int x, int y, int s, uint8_t id) : Fl_Radio_Button(x, y, s, s), Tile(id),
	_for_clipboard(false) {
	user_data(NULL);
	when(FL_WHEN_RELEASE);
}

void Deep_Tile_Button::copy_pixels(Pixel_Button **pbs, Lighting l) {
	for (int i = 0; i < TILE_AREA; i++) {
		copy_pixel(pbs[i], l);
	}
}

void Deep_Tile_Button::drag_to_swap_or_copy() {
	if (Fl::event() == FL_PASTE && _dragging && _dragging != this && _dragging->active() && active()) {
		if (Fl::event_ctrl()) {
			// Ctrl+drag to copy
			copy(_dragging);
		}
		else {
			// Drag to swap
			Tile temp(0);
			temp.copy(this);
			copy(_dragging);
			_dragging->copy(&temp);
		}
	}
	_dragging = NULL;
}

void Deep_Tile_Button::draw() {
	if (_for_clipboard) {
		draw_for_clipboard(x(), y());
		return;
	}
	const uchar *rgb = _undefined ? _undefined_rgb : _monochrome_rgb;
	fl_draw_image(rgb, x(), y(), TILE_PX_SIZE, TILE_PX_SIZE, NUM_CHANNELS, LINE_BYTES);
	if (value()) {
		draw_selection_border(x(), y(), TILE_PX_SIZE, false);
	}
}

int Deep_Tile_Button::handle(int event) {
	switch (event) {
	case FL_DRAG:
		_dragging = this;
		Fl::dnd();
		break;
	case FL_DND_ENTER:
	case FL_DND_DRAG:
	case FL_DND_RELEASE:
		return 1;
	case FL_PASTE:
		drag_to_swap_or_copy();
		do_callback();
		return 1;
	}
	return Fl_Radio_Button::handle(event);
}

Pixel_Button::Pixel_Button(int x, int y, int s) : Fl_Box(x, y, s, s), _x(), _y(), _hue() {
	user_data(NULL);
	box(FL_FLAT_BOX);
}

void Pixel_Button::coloring() {
	const uchar *rgb = Color::monochrome_color(_hue);
	color(fl_rgb_color(rgb[0], rgb[1], rgb[2]));
}

void Pixel_Button::draw() {
	draw_box();
	if (Fl::belowmouse() == this) {
		draw_selection_border(x(), y(), w(), false);
	}
}

int Pixel_Button::handle(int event) {
	//Tileset_Window *tw = (Tileset_Window *)user_data();
	switch (event) {
	case FL_ENTER:
		if (Fl::event_button1() && !Fl::pushed()) {
			Fl::pushed(this);
			do_callback();
		}
		redraw();
		return 1;
	case FL_LEAVE:
		redraw();
		return 1;
	case FL_MOVE:
		redraw();
		return 1;
	case FL_PUSH:
		do_callback();
		return 1;
	case FL_RELEASE:
		return 1;
	case FL_DRAG:
		if (!Fl::event_inside(x(), y(), w(), h())) {
			Fl::pushed(NULL);
		}
		return 1;
	}
	return Fl_Box::handle(event);
}

Swatch::Swatch(int x, int y, int s, const char *l) : Fl_Radio_Button(x, y, s, s, l), _hue() {
	user_data(NULL);
	box(OS_SWATCH_BOX);
}

void Swatch::coloring() {
	const uchar *rgb = Color::monochrome_color(_hue);
	color(fl_rgb_color(rgb[0], rgb[1], rgb[2]));
}

void Swatch::draw() {
	draw_box();
	fl_font(FL_COURIER_BOLD, 14);
	draw_outlined_text(label(), x(), y(), w(), h(), FL_ALIGN_CENTER, value() ? FL_YELLOW : FL_WHITE, FL_BLACK);
	if (value()) {
		draw_selection_border(x(), y(), w(), false);
	}
}

Color_Button *Color_Button::_dragging = NULL;

Color_Button::Color_Button(int x, int y, int s, const char *l) : Fl_Radio_Button(x, y, s, s, l) {
	user_data(NULL);
	box(FL_FLAT_BOX);
	down_box(FL_FLAT_BOX);
}

void Color_Button::drag_to_swap_or_copy() {
	if (Fl::event() == FL_PASTE && _dragging && _dragging != this && _dragging->active() && active()) {
		if (Fl::event_ctrl()) {
			// Ctrl+drag to copy
			color(_dragging->color());
		}
		else {
			// Drag to swap
			Fl_Color temp = color();
			color(_dragging->color());
			_dragging->color(temp);
		}
	}
	_dragging = NULL;
}

void Color_Button::draw() {
	draw_box();
	if (value()) {
		draw_selection_border(x(), y(), w(), false);
	}
}

int Color_Button::handle(int event) {
	switch (event) {
	case FL_DRAG:
		_dragging = this;
		Fl::dnd();
		break;
	case FL_DND_ENTER:
	case FL_DND_DRAG:
	case FL_DND_RELEASE:
		return 1;
	case FL_PASTE:
		drag_to_swap_or_copy();
		do_callback();
		return 1;
	}
	return Fl_Radio_Button::handle(event);
}
