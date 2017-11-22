#pragma warning(push, 0)
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "themes.h"
#include "main-window.h"
#include "block-window.h"
#include "map-buttons.h"

// 32x32 translucent red highlight for the event quadrant of a block
static uchar event_cursor_png_buffer[96] = {
	0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x01, 0x03, 0x00, 0x00, 0x00, 0x49, 0xB4, 0xE8,
	0xB7, 0x00, 0x00, 0x00, 0x03, 0x50, 0x4C, 0x54, 0x45, 0xFF, 0x00, 0x00, 0x19, 0xE2, 0x09, 0x37,
	0x00, 0x00, 0x00, 0x01, 0x74, 0x52, 0x4E, 0x53, 0x80, 0xAD, 0x5E, 0x5B, 0x46, 0x00, 0x00, 0x00,
	0x0B, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5E, 0x63, 0x18, 0xE4, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x01,
	0x1C, 0x35, 0x43, 0x02, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82
};

static Fl_PNG_Image event_cursor_png(NULL, event_cursor_png_buffer, 96);

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

static void draw_tileset_button(Fl_Widget *wgt, uint8_t id, bool border, bool zoom) {
	Block_Window *bw = (Block_Window *)wgt->user_data();
	int x = wgt->x(), y = wgt->y();
	bw->draw_tile(x, y, id, zoom);
	if (border) {
		int rs = zoom ? CHIP_PX_SIZE : TILE_PX_SIZE;
		draw_selection_border(x, y, rs, false);
	}
}

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
		setonly();
		do_callback();
		return 1;
	default:
		return 0;
	}
}

Block::Block(int x, int y, int s, uint8_t row, uint8_t col, uint8_t id) : Fl_Box(x, y, s, s),
	_row(row), _col(col), _id(id) {
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
	bool below_mouse = Fl::belowmouse() == this, event_cursor = mw->event_cursor();
	draw_map_button(this, _id, below_mouse && !event_cursor);
	if (!below_mouse || !event_cursor) { return; }
	int hw = w() / 2, hh = h() / 2;
	int hx = x() + right_half() * hw, hy = y() + bottom_half() * hh;
	event_cursor_png.draw(hx, hy, hw, hh);
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
		redraw();
		return 1;
	case FL_LEAVE:
		mw->update_status(NULL);
		redraw();
		return 1;
	case FL_MOVE:
		mw->update_event_cursor(this);
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

Tile_Button::Tile_Button(int x, int y, int s, uint8_t id) : Fl_Radio_Button(x, y, s, s), _id(id) {
	user_data(NULL);
	box(FL_NO_BOX);
	labeltype(FL_NO_LABEL);
	when(FL_WHEN_RELEASE);
}

void Tile_Button::draw() {
	draw_tileset_button(this, _id, !!value(), false);
}

Chip::Chip(int x, int y, int s, uint8_t row, uint8_t col, uint8_t id) : Fl_Box(x, y, s, s),
	_row(row), _col(col), _id(id) {
	user_data(NULL);
	box(FL_NO_BOX);
	labeltype(FL_NO_LABEL);
	labelcolor(FL_YELLOW);
}

void Chip::draw() {
	draw_tileset_button(this, _id, Fl::belowmouse() == this, true);
}

int Chip::handle(int event) {
	Block_Window *bw = (Block_Window *)user_data();
	switch (event) {
	case FL_ENTER:
		if (Fl::event_button1() && !Fl::pushed()) {
			Fl::pushed(this);
			do_callback();
		}
		bw->update_status(this);
		redraw();
		return 1;
	case FL_LEAVE:
		bw->update_status(NULL);
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

Deep_Tile_Button::Deep_Tile_Button(int x, int y, int s, uint8_t id) : Fl_Radio_Button(x, y, s, s), Tile(id) {
	user_data(NULL);
	when(FL_WHEN_RELEASE);
}

void Deep_Tile_Button::copy_pixel(const Pixel_Button *pb) {
	_palette = pb->palette();
	uchar r, g, b;
	Fl::get_color(pb->color(), r, g, b);
	pixel(pb->col(), pb->row(), pb->hue(), r, g, b);
}

void Deep_Tile_Button::copy_pixels(Pixel_Button **pbs) {
	for (int i = 0; i < TILE_SIZE * TILE_SIZE; i++) {
		copy_pixel(pbs[i]);
	}
}

void Deep_Tile_Button::draw() {
	fl_draw_image(_rgb, x(), y(), TILE_PX_SIZE, TILE_PX_SIZE, NUM_CHANNELS, LINE_BYTES);
	if (value()) {
		draw_selection_border(x(), y(), TILE_PX_SIZE, false);
	}
}

Pixel_Button::Pixel_Button(int x, int y, int s) : Fl_Box(x, y, s, s), _x(), _y(), _lighting(), _palette(), _hue() {
	user_data(NULL);
	box(FL_FLAT_BOX);
}

void Pixel_Button::coloring(Lighting l, Palette p, Hue h) {
	_lighting = l;
	_palette = p;
	_hue = h;
	const uchar *rgb = Color::color(l, p, h);
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

void Swatch::coloring(Lighting l, Palette p, Hue h) {
	_hue = h;
	const uchar *rgb = Color::color(l, p, h);
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
