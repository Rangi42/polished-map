#include "main-window.h"
#include "tileset-window.h"
#include "map-buttons.h"

static void draw_map_button(Fl_Widget *wgt, uint8_t id, bool highlight) {
	Main_Window *mw = (Main_Window *)wgt->user_data();
	int x = wgt->x(), y = wgt->y(), w = wgt->w(), h = wgt->h();
	int ms = mw->metatile_size();
	mw->draw_metatile(x, y, id);
	if (mw->grid()) {
		fl_color(FL_INACTIVE_COLOR);
		fl_xyline(x, y+ms-1, x+ms-1, y);
	}
	if (highlight) {
		int rs = ms - (mw->grid() ? 1 : 0);
		fl_rect(x, y, rs, rs, FL_BLACK);
		fl_rect(x+1, y+1, rs-2, rs-2, FL_WHITE);
		if (mw->zoom()) {
			fl_rect(x+2, y+2, rs-4, rs-4, FL_WHITE);
			fl_rect(x+3, y+3, rs-6, rs-6, FL_BLACK);
		}
		else {
			fl_rect(x+2, y+2, rs-4, rs-4, FL_BLACK);
		}
	}
	if (!mw->ids()) { return; }
	const char *l = wgt->label();
	Fl_Align a = FL_ALIGN_TOP_LEFT | FL_ALIGN_INSIDE;
	int p = mw->zoom() ? 2 : 1;
	fl_font(FL_COURIER_BOLD, 14);
	fl_color(FL_BLACK);
	fl_draw(l, x+p+1, y+p-1, w, h, a);
	fl_draw(l, x+p+1, y+p+1, w, h, a);
	fl_draw(l, x+p+3, y+p-1, w, h, a);
	fl_draw(l, x+p+3, y+p+1, w, h, a);
	fl_color(highlight ? wgt->labelcolor() : FL_WHITE);
	fl_draw(l, x+p+2, y+p, w, h, a);
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
	sprintf(buffer, (mw && mw->hex()) ? "%02X" : "%d", _id);
	copy_label(buffer);
}

void Metatile_Button::draw() {
	draw_map_button(this, _id, !!value());
}

int Metatile_Button::handle(int event) {
	switch (event) {
	case FL_ENTER:
		return 1;
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
	sprintf(buffer, (mw && mw->hex()) ? "%02X" : "%d", _id);
	copy_label(buffer);
}

void Block::draw() {
	draw_map_button(this, _id, Fl::belowmouse() == this);
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

static void draw_tileset_button(Fl_Widget *wgt, uint8_t id, bool highlight, bool zoom) {
	Tileset_Window *tw = (Tileset_Window *)wgt->user_data();
	int x = wgt->x(), y = wgt->y();
	tw->draw_tile(x, y, id, zoom);
	if (highlight) {
		int rs = TILE_SIZE * (zoom ? 3 : 2);
		fl_rect(x, y, rs, rs, FL_BLACK);
		fl_rect(x+1, y+1, rs-2, rs-2, FL_WHITE);
		fl_rect(x+2, y+2, rs-4, rs-4, FL_BLACK);
	}
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
