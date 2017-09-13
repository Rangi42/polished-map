#include "main-window.h"
#include "map-buttons.h"

Metatile_Button::Metatile_Button(int x, int y, int s, uint8_t id) : Fl_Radio_Button(x, y, s, s), _id(id) {
	user_data(NULL);
	box(FL_NO_BOX);
	align(FL_ALIGN_BOTTOM_RIGHT | FL_ALIGN_INSIDE);
	labelcolor(FL_WHITE);
	labelsize(14);
	labelfont(FL_COURIER_BOLD);
	labeltype(FL_NO_LABEL);
	when(FL_WHEN_RELEASE);
}

void Metatile_Button::id(uint8_t id) {
	_id = id;
	Main_Window *mw = (Main_Window *)user_data();
	char buffer[16];
	sprintf(buffer, (mw && mw->hex()) ? "%02X" : "%d", _id);
	copy_label(buffer);
}

void Metatile_Button::draw() {
	Main_Window *mw = (Main_Window *)user_data();
	mw->draw_metatile(x(), y(), _id);
	int ms = mw->metatile_size();
	if (mw->grid()) {
		fl_color(GRID_COLOR);
		fl_xyline(x(), y()+ms-1, x()+ms-1, y());
	}
	if (value()) {
		int rs = ms - (mw->grid() ? 1 : 0);
		fl_rect(x(), y(), rs, rs, FL_BLACK);
		fl_rect(x()+1, y()+1, rs-2, rs-2, FL_WHITE);
		if (mw->zoom()) {
			fl_rect(x()+2, y()+2, rs-4, rs-4, FL_WHITE);
			fl_rect(x()+3, y()+3, rs-6, rs-6, FL_BLACK);
		}
		else {
			fl_rect(x()+2, y()+2, rs-4, rs-4, FL_BLACK);
		}
	}
	if (!mw->ids()) { return; }
	Fl_Align a = align();
	if (a & FL_ALIGN_CLIP) {
		fl_push_clip(x(), y(), w(), h());
		a = (Fl_Align)(a & ~FL_ALIGN_CLIP);
	}
	fl_font(labelfont(), labelsize());
	fl_color(FL_BLACK);
	fl_draw(label(), x()-6, y()-3, w(), h(), a);
	fl_draw(label(), x()-6, y()-1, w(), h(), a);
	fl_draw(label(), x()-4, y()-3, w(), h(), a);
	fl_draw(label(), x()-4, y()-1, w(), h(), a);
	fl_color(labelcolor());
	fl_draw(label(), x()-5, y()-2, w(), h(), a);
	if (align() & FL_ALIGN_CLIP) {
		fl_pop_clip();
	}
}

Block::Block(int x, int y, int s, uint8_t row, uint8_t col, uint8_t id) : Fl_Box(x, y, s, s),
	_row(row), _col(col), _id(id) {
	user_data(NULL);
	box(FL_NO_BOX);
	align(FL_ALIGN_BOTTOM_RIGHT | FL_ALIGN_INSIDE);
	labelcolor(FL_WHITE);
	labelsize(14);
	labelfont(FL_COURIER_BOLD);
	labeltype(FL_NO_LABEL);
}

void Block::update_label() {
	Main_Window *mw = (Main_Window *)user_data();
	char buffer[16];
	sprintf(buffer, (mw && mw->hex()) ? "%02X" : "%d", _id);
	copy_label(buffer);
}

void Block::draw() {
	Main_Window *mw = (Main_Window *)user_data();
	mw->draw_metatile(x(), y(), _id);
	int ms = mw->metatile_size();
	if (mw->grid()) {
		fl_color(GRID_COLOR);
		fl_xyline(x(), y()+ms-1, x()+ms-1, y());
	}
	if (Fl::belowmouse() == this) {
		int rs = ms - (mw->grid() ? 1 : 0);
		fl_rect(x(), y(), rs, rs, FL_BLACK);
		fl_rect(x()+1, y()+1, rs-2, rs-2, FL_WHITE);
		if (mw->zoom()) {
			fl_rect(x()+2, y()+2, rs-4, rs-4, FL_WHITE);
			fl_rect(x()+3, y()+3, rs-6, rs-6, FL_BLACK);
		}
		else {
			fl_rect(x()+2, y()+2, rs-4, rs-4, FL_BLACK);
		}
	}
	if (!mw->ids()) { return; }
	Fl_Align a = align();
	if (a & FL_ALIGN_CLIP) {
		fl_push_clip(x(), y(), w(), h());
		a = (Fl_Align)(a & ~FL_ALIGN_CLIP);
	}
	fl_font(labelfont(), labelsize());
	fl_color(FL_BLACK);
	fl_draw(label(), x()-6, y()-3, w(), h(), a);
	fl_draw(label(), x()-6, y()-1, w(), h(), a);
	fl_draw(label(), x()-4, y()-3, w(), h(), a);
	fl_draw(label(), x()-4, y()-1, w(), h(), a);
	fl_color(Fl::belowmouse() == this ? FL_YELLOW : labelcolor());
	fl_draw(label(), x()-5, y()-2, w(), h(), a);
	if (align() & FL_ALIGN_CLIP) {
		fl_pop_clip();
	}
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
