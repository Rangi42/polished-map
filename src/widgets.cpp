#pragma warning(push, 0)
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Hor_Nice_Slider.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Menu_.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#pragma warning(pop)

#include "themes.h"
#include "math.h"
#include "widgets.h"
#include "main-window.h"

Metatile::Metatile(int x, int y, int s, uint8_t id) : Fl_Radio_Button(x, y, s, s), _id(id) {
	user_data(NULL);
	box(FL_NO_BOX);
	align(FL_ALIGN_BOTTOM_RIGHT | FL_ALIGN_INSIDE | FL_ALIGN_TEXT_OVER_IMAGE | FL_ALIGN_IMAGE_BACKDROP);
	labelcolor(FL_WHITE);
	labelsize(12);
	labelfont(FL_COURIER);
	labeltype(FL_NO_LABEL);
	when(FL_WHEN_CHANGED);
}

void Metatile::id(uint8_t id) {
	_id = id;
	Main_Window *mw = (Main_Window *)user_data();
	char buffer[16];
	sprintf(buffer, (mw && mw->hex()) ? "%02X" : "%d", _id);
	copy_label(buffer);
}

void Metatile::draw() {
	Main_Window *mw = (Main_Window *)user_data();
	int ms = mw->metatile_size();
	image()->draw(x(), y(), ms, ms);
	if (mw->grid()) {
		fl_color(FL_INACTIVE_COLOR);
		fl_xyline(x(), y()+ms-1, x()+ms-1, y());
	}
	if (value()) {
		int rs = ms - (mw->grid() ? 1 : 0);
		fl_rect(x(), y(), rs, rs, FL_BLACK);
		fl_rect(x()+1, y()+1, rs-2, rs-2, FL_WHITE);
		if (ms == METATILE_SIZE) {
			fl_rect(x()+2, y()+2, rs-4, rs-4, FL_BLACK);
		}
		else {
			fl_rect(x()+2, y()+2, rs-4, rs-4, FL_WHITE);
			fl_rect(x()+3, y()+3, rs-6, rs-6, FL_BLACK);
		}
	}
	if (!mw->ids()) { return; }
	Fl_Align a = align();
	if (a & FL_ALIGN_CLIP) {
		fl_push_clip(x(), y(), w(), h());
		a = (Fl_Align)(a & ~FL_ALIGN_CLIP);
	}
	fl_font(labelfont() | FL_BOLD, labelsize());
	fl_color(FL_BLACK);
	fl_draw(label(), x()-4, y()-1, w(), h(), a);
	fl_draw(label(), x()-4, y()+1, w(), h(), a);
	fl_draw(label(), x()-2, y()-1, w(), h(), a);
	fl_draw(label(), x()-2, y()+1, w(), h(), a);
	fl_font(labelfont() | FL_BOLD, labelsize());
	fl_color(labelcolor());
	fl_draw(label(), x()-3, y(), w(), h(), a);
	if (align() & FL_ALIGN_CLIP) {
		fl_pop_clip();
	}
}

Block::Block(int x, int y, int s, uint8_t row, uint8_t col, uint8_t id) : Fl_Box(x, y, s, s),
	_row(row), _col(col), _id(id) {
	user_data(NULL);
	box(FL_NO_BOX);
	align(FL_ALIGN_BOTTOM_RIGHT | FL_ALIGN_INSIDE | FL_ALIGN_TEXT_OVER_IMAGE | FL_ALIGN_IMAGE_BACKDROP);
	labelcolor(FL_WHITE);
	labelsize(12);
	labelfont(FL_COURIER);
	labeltype(FL_NO_LABEL);
}

void Block::id(uint8_t id) {
	_id = id;
	Main_Window *mw = (Main_Window *)user_data();
	char buffer[16];
	sprintf(buffer, (mw && mw->hex()) ? "%02X" : "%d", _id);
	copy_label(buffer);
}

void Block::draw() {
	Main_Window *mw = (Main_Window *)user_data();
	int ms = mw->metatile_size();
	Fl_Image *img = mw->metatile_image(_id);
	img->draw(x(), y(), ms, ms);
	if (mw->grid()) {
		fl_color(FL_INACTIVE_COLOR);
		fl_xyline(x(), y()+ms-1, x()+ms-1, y());
	}
	if (Fl::belowmouse() == this) {
		int rs = ms - (mw->grid() ? 1 : 0);
		fl_rect(x(), y(), rs, rs, FL_BLACK);
		fl_rect(x()+1, y()+1, rs-2, rs-2, FL_WHITE);
		if (ms == METATILE_SIZE) {
			fl_rect(x()+2, y()+2, rs-4, rs-4, FL_BLACK);
		}
		else {
			fl_rect(x()+2, y()+2, rs-4, rs-4, FL_WHITE);
			fl_rect(x()+3, y()+3, rs-6, rs-6, FL_BLACK);
		}
	}
	if (!mw->ids()) { return; }
	Fl_Align a = align();
	if (a & FL_ALIGN_CLIP) {
		fl_push_clip(x(), y(), w(), h());
		a = (Fl_Align)(a & ~FL_ALIGN_CLIP);
	}
	fl_font(labelfont() | FL_BOLD, labelsize());
	fl_color(FL_BLACK);
	fl_draw(label(), x()-4, y()-1, w(), h(), a);
	fl_draw(label(), x()-4, y()+1, w(), h(), a);
	fl_draw(label(), x()-2, y()-1, w(), h(), a);
	fl_draw(label(), x()-2, y()+1, w(), h(), a);
	fl_font(labelfont() | FL_BOLD, labelsize());
	fl_color(Fl::belowmouse() == this ? FL_YELLOW : labelcolor());
	fl_draw(label(), x()-3, y(), w(), h(), a);
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

void DnD_Receiver::deferred_callback(DnD_Receiver *dndr) {
	dndr->do_callback();
}

DnD_Receiver::DnD_Receiver(int x, int y, int w, int h, const char *l) : Fl_Box(x, y, w, h, l), _text() {
	labeltype(FL_NO_LABEL);
	box(FL_NO_BOX);
	clear_visible_focus();
}

int DnD_Receiver::handle(int event) {
	if (event == FL_PASTE) {
		_text = Fl::event_text();
		// Callback deferral technique taken from <http://www.fltk.org/articles.php?L770>
		if (callback() && ((when() & FL_WHEN_RELEASE) || (when() & FL_WHEN_CHANGED))) {
			Fl::add_timeout(0.0, (Fl_Timeout_Handler)DnD_Receiver::deferred_callback, this);
		}
		return 1;
	}
	return 0;
}

Label::Label(int x, int y, int w, int h, const char *l) : Fl_Box(x, y, w, h, l) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
}

Spacer::Spacer(int x, int y, int w, int h, const char *l) : Fl_Box(x, y, w, h, l) {
	labeltype(FL_NO_LABEL);
	box(OS_SPACER_THIN_DOWN_FRAME);
	color(FL_DARK1);
}

OS_Button::OS_Button(int x, int y, int w, int h, const char *l) : Fl_Button(x, y, w, h, l) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	box(OS_BUTTON_UP_BOX);
	down_box(OS_DEPRESSED_DOWN_BOX);
}

int OS_Button::handle(int event) {
	switch (event) {
	case FL_ENTER:
		if (active_r()) {
			box(OS_HOVERED_UP_BOX);
			redraw();
			return 1;
		}
		return 0;
	case FL_LEAVE:
	case FL_HIDE:
	case FL_DEACTIVATE:
		box(OS_BUTTON_UP_BOX);
		redraw();
		return 1;
	}
	if (event == FL_PUSH) {
		Fl::focus(this);
	}
	return Fl_Button::handle(event);
}

Default_Button::Default_Button(int x, int y, int w, int h, const char *l) : Fl_Button(x, y, w, h, l) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	box(OS_DEFAULT_BUTTON_BOX);
	down_box(OS_DEPRESSED_DOWN_BOX);
	shortcut(FL_Enter);
}

int Default_Button::handle(int event) {
	if (OS::current_theme() == OS::AERO || OS::current_theme() == OS::METRO || OS::current_theme() == OS::BLUE) {
		switch (event) {
		case FL_ENTER:
			if (active_r()) {
				box(OS_HOVERED_UP_BOX);
				redraw();
				return 1;
			}
			return 0;
		case FL_LEAVE:
		case FL_HIDE:
		case FL_DEACTIVATE:
			box(OS_DEFAULT_BUTTON_BOX);
			redraw();
			return 1;
		}
	}
	return Fl_Button::handle(event);
}

OS_Repeat_Button::OS_Repeat_Button(int x, int y, int w, int h, const char *l) : Fl_Repeat_Button(x, y, w, h, l) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	box(OS_BUTTON_UP_BOX);
	down_box(OS_DEPRESSED_DOWN_BOX);
}

int OS_Repeat_Button::handle(int event) {
	switch (event) {
	case FL_ENTER:
		if (active_r()) {
			box(OS_HOVERED_UP_BOX);
			redraw();
			return 1;
		}
		return 0;
	case FL_LEAVE:
		box(OS_BUTTON_UP_BOX);
		redraw();
		return 1;
	case FL_HIDE:
	case FL_DEACTIVATE:
		box(OS_BUTTON_UP_BOX);
		redraw();
		break;
	}
	return Fl_Repeat_Button::handle(event);
}

OS_Check_Button::OS_Check_Button(int x, int y, int w, int h, const char *l) : Fl_Check_Button(x, y, w, h, l) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	box(FL_FLAT_BOX);
	align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
	selection_color(FL_SELECTION_COLOR);
}

int OS_Check_Button::handle(int event) {
	if (event == FL_PUSH) {
		Fl::focus(this);
	}
	return Fl_Check_Button::handle(event);
}

void OS_Check_Button::draw() {
	// Based on Fl_Light_Button::draw()
	draw_box(this == Fl::pushed() ? fl_down(box()) : box(), active_r() ? color() : fl_inactive(color()));
	int W = labelsize();
	W++;
	int dx = Fl::box_dx(box()) + 2;
	int dy = (h() - W) / 2;
	draw_box(down_box(), x()+dx, y()+dy, W, W, FL_BACKGROUND2_COLOR);
	if (value()) {
		Fl_Color sc = Fl::scheme() && Fl::is_scheme("gtk+") ? selection_color() : FL_FOREGROUND_COLOR;
		fl_color(active_r() ? sc : fl_inactive(sc));
		int tw = W - 6;
		int d1 = tw / 3;
		int d2 = tw - d1;
		int tx = x() + dx + 3;
		int ty = y() + dy + (W + d2) / 2 - d1 - 2;
		for (int n = 0; n < 3; n++, ty++) {
			fl_line(tx, ty, tx+d1, ty+d1);
			fl_line(tx+d1, ty+d1, tx+tw-1, ty+d1-d2+1);
		}
	}
	draw_label(x()+W+2*dx, y(), w()-W-2*dx, h());
	if (Fl::focus() == this) { draw_focus(); }
}

OS_Radio_Button::OS_Radio_Button(int x, int y, int w, int h, const char *l) : Fl_Radio_Round_Button(x, y, w, h, l) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	box(FL_FLAT_BOX);
	align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
}

void OS_Radio_Button::draw() {
	// Based on Fl_Light_Button::draw()
	draw_box(this == Fl::pushed() ? fl_down(box()) : box(), active_r() ? color() : fl_inactive(color()));
	int W  = labelsize();
	W++;
	int dx = Fl::box_dx(box()) + 2;
	int dy = (h() - W) / 2;
	draw_box(down_box(), x()+dx, y()+dy, W, W, FL_BACKGROUND2_COLOR);
	if (value()) {
		int tW = (W - Fl::box_dw(down_box())) / 2 + 1;
		if ((W - tW) & 1) { tW++; }
		int tdx = dx + (W - tW) / 2;
		int tdy = dy + (W - tW) / 2;
		if (Fl::scheme() && Fl::is_scheme("gtk+")) {
			fl_color(FL_SELECTION_COLOR);
			tW--;
			fl_pie(x()+tdx-1, y()+tdy-1, tW+3, tW+3, 0.0, 360.0);
			fl_color(fl_color_average(FL_WHITE, FL_SELECTION_COLOR, 0.2f));
		}
		else {
			fl_color(value() ? (active_r() ? selection_color() : fl_inactive(selection_color())) : color());
		}
		switch (tW) {
		// Larger circles draw fine...
		default:
			fl_pie(x() + tdx, y() + tdy, tW, tW, 0.0, 360.0);
			break;
		// Small circles don't draw well on many systems...
		case 6:
			fl_rectf(x()+tdx+2, y()+tdy,   tW-4, tW);
			fl_rectf(x()+tdx+1, y()+tdy+1, tW-2, tW-2);
			fl_rectf(x()+tdx,   y()+tdy+2, tW,   tW-4);
			break;
		case 5:
		case 4:
		case 3:
			fl_rectf(x()+tdx+1, y()+tdy,   tW-2, tW);
			fl_rectf(x()+tdx,   y()+tdy+1, tW,   tW-2);
			break;
		case 2:
		case 1:
			fl_rectf(x()+tdx, y()+tdy, tW, tW);
			break;
		}
		if (Fl::scheme() && Fl::is_scheme("gtk+")) {
			fl_color(fl_color_average(FL_WHITE, FL_SELECTION_COLOR, 0.5));
			fl_arc(x()+tdx, y()+tdy, tW+1, tW+1, 60.0, 180.0);
		}
	}
	draw_label(x()+W+2*dx, y(), w()-W-2*dx, h());
	if (Fl::focus() == this) { draw_focus(); }
}

int OS_Radio_Button::handle(int event) {
	if (event == FL_PUSH) {
		Fl::focus(this);
	}
	return Fl_Radio_Round_Button::handle(event);
}

Link_Button::Link_Button(int x, int y, int w, int h, const char *l) : OS_Button(x, y, w, h, l),
	_cached_color(FL_BACKGROUND_COLOR), _cached_box(FL_FLAT_BOX) {
	box(FL_FLAT_BOX);
	color(FL_BACKGROUND_COLOR);
	down_box(OS_DEPRESSED_DOWN_BOX);
	down_color(FL_SELECTION_COLOR);
	align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
}

void Link_Button::draw() {
	// Based on Fl_Button::draw()
	Fl_Color col = value() ? selection_color() : color();
	col = active_r() ? col : fl_inactive(col);
	draw_box(value() ? down_box() ? down_box() : fl_down(box()) : box(), col);
	draw_backdrop();
	if (labeltype() == FL_NORMAL_LABEL && value()) {
		Fl_Color c = labelcolor();
		labelcolor(fl_contrast(c, col));
		draw_label();
		labelcolor(c);
	}
	else {
		draw_label();
	}
	if (Fl::focus() == this) { draw_focus(); }
}

int Link_Button::handle(int event) {
	switch (event) {
	case FL_ENTER:
		_cached_color = color();
		_cached_box = box();
		if (active_r()) {
			if (OS::current_theme() == OS::DARK) {
				box(OS_BUTTON_UP_BOX);
			}
			else {
				color(FL_LIGHT3);
			}
			redraw();
			return 1;
		}
		return 0;
	case FL_LEAVE:
		if (OS::current_theme() == OS::DARK) {
			box(_cached_box);
		}
		else {
			color(_cached_color);
		}
		redraw();
		return 1;
	case FL_PUSH:
		Fl::focus(this);
	}
	return Fl_Button::handle(event);
}

Expander_Collapser::Expander_Collapser(int x, int y, int w, int h, const char *l) : Fl_Check_Button(x, y, w, h, l) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	box(FL_FLAT_BOX);
	align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
	selection_color(FL_SELECTION_COLOR);
}

void Expander_Collapser::draw() {
	// Based on Fl_Light_Button::draw()
	draw_box(this == Fl::pushed() ? fl_down(box()) : box(), active_r() ? color() : fl_inactive(color()));
	int W = labelsize() - 6;
	W++;
	int dx = Fl::box_dx(box()) + 2;
	int dy = (h() - W) / 2;
	fl_color(active_r() ? labelcolor() : fl_inactive(labelcolor()));
	if (value()) {
		fl_polygon(x()+dx+1, y()+dy+W-2, x()+dx+W-2, y()+dy+1, x()+dx+W-2, y()+dy+W-2);
	}
	else {
		int da = (W - 1) / 4 + 1;
		fl_loop(x()+dx+da, y()+dy, x()+dx+da, y()+dy+W-1, x()+dx+(W-1)/2+da, y()+dy+(W-1)/2);
	}
	draw_label(x()+W+2*dx+1, y(), w()-W-2*dx-1, h());
	if (Fl::focus() == this) { draw_focus(); }
}

int Expander_Collapser::handle(int event) {
	if (event == FL_PUSH) {
		Fl::focus(this);
	}
	return Fl_Check_Button::handle(event);
}

Toggle_Switch::Toggle_Switch(int x, int y, int w, int h, const char *l) : Fl_Check_Button(x, y, w, h, l),
	_off_text("Off"), _on_text("On"), _text_label() {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	box(OS_BUTTON_UP_BOX);
	down_box(OS_DEPRESSED_DOWN_BOX);
	align(FL_ALIGN_LEFT | FL_ALIGN_CLIP);
	textfont(OS_FONT);
	textsize(OS_FONT_SIZE);
	textcolor(FL_FOREGROUND_COLOR);
	textalign(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
}

void Toggle_Switch::draw() {
	// Based on Fl_Light_Button::draw()
	int pw = h() / 2 + 2;
	int bw = w() - pw;
	Fl_Boxtype bb = this == Fl::pushed() ? down_box() ? down_box() : fl_down(box()) : box();
	draw_box(bb, x(), y(), bw, h(), active_r() ? color() : fl_inactive(color()));
	_text_label.value = value() ? _on_text : _off_text;
	Fl_Color tc = _text_label.color;
	_text_label.color = active_r() ? _text_label.color : fl_inactive(_text_label.color);
	_text_label.draw(x()+Fl::box_dx(bb)+3, y()+Fl::box_dy(bb), MAX(bw-2*Fl::box_dx(bb)-6, 0), h()-2*Fl::box_dy(bb),
		_text_label.align_);
	_text_label.color = tc;
	Fl_Boxtype pb = Fl::scheme() ? value() ? OS_DEPRESSED_DOWN_BOX : OS_BUTTON_UP_BOX : OS_SPACER_THIN_DOWN_BOX;
	Fl_Color pc = value() ? FL_SELECTION_COLOR : FL_DARK2;
	int px = x() + w() - pw;
	draw_box(pb, px, y(), pw, h(), active_r() ? pc : fl_inactive(pc));
	int sw = pw - 2;
	int sy = value() ? y() + 1 : y() + h() - 1 - sw;
	draw_box(box(), px+1, sy, sw, sw, FL_GRAY);
	draw_label();
	if (Fl::focus() == this) { draw_focus(); }
}

int Toggle_Switch::handle(int event) {
	switch (event) {
	case FL_ENTER:
		if (active_r()) {
			box(OS_HOVERED_UP_BOX);
			redraw();
			return 1;
		}
		return 0;
	case FL_LEAVE:
	case FL_HIDE:
	case FL_DEACTIVATE:
		box(OS_BUTTON_UP_BOX);
		redraw();
		return 1;
	}
	if (event == FL_PUSH) {
		Fl::focus(this);
	}
	return Fl_Check_Button::handle(event);
}

OS_Spinner::OS_Spinner(int x, int y, int w, int h, const char *l) : Fl_Spinner(x, y, w, h, l) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	textfont(OS_FONT);
	textsize(OS_FONT_SIZE);
	align(FL_ALIGN_LEFT | FL_ALIGN_CLIP);
	input_.box(OS_INPUT_THIN_DOWN_BOX);
	up_button_.labelfont(OS_FONT);
	up_button_.labelsize(OS_FONT_SIZE);
	up_button_.box(OS_BUTTON_UP_BOX);
	up_button_.down_box(OS_DEPRESSED_DOWN_BOX);
	down_button_.labelfont(OS_FONT);
	down_button_.labelsize(OS_FONT_SIZE);
	down_button_.box(OS_BUTTON_UP_BOX);
	down_button_.down_box(OS_DEPRESSED_DOWN_BOX);
}

Default_Spinner::Default_Spinner(int x, int y, int w, int h, const char *l) : OS_Spinner(x, y, w, h, l),
	_default_value(0.0) {}

int Default_Spinner::handle(int event) {
	switch (event) {
	case FL_PUSH:
		if (Fl::event_button() == FL_MIDDLE_MOUSE) {
			return 1;
		}
		break;
	case FL_RELEASE:
		if (Fl::event_button() == FL_MIDDLE_MOUSE) {
			value(_default_value);
			do_callback();
			return 1;
		}
		break;
	}
	return Fl_Spinner::handle(event);
}

OS_Slider::OS_Slider(int x, int y, int w, int h, const char *l) : Fl_Hor_Nice_Slider(x, y, w, h, l) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	slider(OS_BUTTON_UP_BOX);
	slider_size(0.0);
	align(FL_ALIGN_LEFT | FL_ALIGN_CLIP);
}

int OS_Slider::handle(int event) {
	if (event == FL_PUSH) {
		Fl::focus(this);
	}
	return Fl_Hor_Nice_Slider::handle(event);
}

void OS_Slider::draw() {
	// Based on Fl_Slider::draw()
	if (damage() & FL_DAMAGE_ALL) { draw_box(box(), active_r() ? color() : fl_inactive(color())); }
	draw(x()+Fl::box_dx(box()), y()+Fl::box_dy(box()), w()-Fl::box_dw(box()), h()-Fl::box_dh(box()));
}

void OS_Slider::draw(int X, int Y, int W, int H) {
	// Based on Fl_Slider::draw(...)
	double v = 0.5;
	if (minimum() != maximum()) {
		v = MAX(0.0, MIN(1.0, (value() - minimum()) / (maximum() - minimum())));
	}
	int s = MAX((int)(slider_size() * W + 0.5), H / 2 + 2);
	int lx = X + (int)(v * (W - s) + 0.5);
	fl_push_clip(X, Y, W, H);
	draw_box(box(), active_r() ? color() : fl_inactive(color()));
	fl_pop_clip();
	draw_box(OS_SPACER_THIN_DOWN_BOX, X, Y+H/2-2, W, 4, active_r() ? FL_DARK2 : fl_inactive(FL_DARK2));
	draw_box(slider(), lx, Y, s, H, FL_GRAY);
	draw_label(lx, Y, s, H);
	if (Fl::focus() == this) {
		draw_focus(slider(), lx, Y, s, H);
	}
}

Default_Slider::Default_Slider(int x, int y, int w, int h, const char *l) : OS_Slider(x, y, w, h, l),
	_default_value(0.0) {}

int Default_Slider::handle(int event) {
	return handle(event, x(), y(), w(), h());
}

int Default_Slider::handle(int event, int x, int y, int w, int h) {
	switch (event) {
	case FL_PUSH:
		Fl::focus(this);
		if (Fl::event_button() == FL_MIDDLE_MOUSE) {
			return 1;
		}
		break;
	case FL_RELEASE:
		if (Fl::event_button() == FL_MIDDLE_MOUSE) {
			value(_default_value);
			do_callback();
			return 1;
		}
		break;
	}
	return Fl_Hor_Nice_Slider::handle(event, x, y, w, h);
}

Default_Value_Slider::Default_Value_Slider(int x, int y, int w, int h, int vs, const char *l) :
	Default_Slider(x, y, w, h, l), _value_size(vs), _textfont(OS_FONT), _textsize(OS_FONT_SIZE),
	_textcolor(FL_FOREGROUND_COLOR) {
	box(FL_FLAT_BOX);
	align(FL_ALIGN_LEFT);
}

void Default_Value_Slider::draw() {
	// Based on Fl_Value_Slider::draw()
	int sx = x() + _value_size, sy = y(), sw = w() - _value_size, sh = h();
	if (damage() & FL_DAMAGE_ALL) {
		draw_box(box(), sx, sy, sw, sh, active_r() ? color() : fl_inactive(color()));
	}
	OS_Slider::draw(sx+Fl::box_dx(box()), sy+Fl::box_dy(box()), sw-Fl::box_dw(box()), sh-Fl::box_dh(box()));
	int bx = x(), by = y(), bw = _value_size, bh = h();
	draw_box(box(), bx, by, bw, bh, active_r() ? color() : fl_inactive(color()));
	char buf[128];
	format(buf);
	fl_font(textfont(), textsize());
	fl_color(active_r() ? textcolor() : fl_inactive(textcolor()));
	fl_draw(buf, bx, by, bw, bh, FL_ALIGN_CLIP);
}

int Default_Value_Slider::handle(int event) {
	// Based on Fl_Value_Slider::handle(...)
	int sxx = x() + _value_size, syy = y(), sww = w() - _value_size, shh = h();
	return Default_Slider::handle(event, sxx+Fl::box_dx(box()), syy+Fl::box_dy(box()),
		sww-Fl::box_dw(box()), shh-Fl::box_dh(box()));
}

OS_Progress::OS_Progress(int x, int y, int w, int h, const char *l) : Fl_Progress(x, y, w, h, l) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	box(OS_PROGRESS_ROUND_UP_BOX);
	color(FL_DARK1);
	selection_color(OS_PROGRESS_COLOR);
}

const double Indeterminate_Progress::BOUNCER_SIZE = 0.3;
const double Indeterminate_Progress::UPDATE_INTERVAL = 0.1;

Indeterminate_Progress::Indeterminate_Progress(int x, int y, int w, int h, const char *l) :
	Fl_Widget(x, y, w, h, l), _value(0) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	box(OS_PROGRESS_ROUND_UP_BOX);
	align(FL_ALIGN_INSIDE);
	color(FL_DARK1);
	selection_color(OS_PROGRESS_COLOR);
}

static void indeterminate_progress_tick(Indeterminate_Progress *ip) {
	int v = ip->value() + 1;
	if (v >= Indeterminate_Progress::MAX_VALUE) { v = -Indeterminate_Progress::MAX_VALUE; }
	ip->value(v);
	Fl::repeat_timeout(Indeterminate_Progress::UPDATE_INTERVAL, (Fl_Timeout_Handler)indeterminate_progress_tick, ip);
}

Indeterminate_Progress::~Indeterminate_Progress() {
	Fl::remove_timeout((Fl_Timeout_Handler)indeterminate_progress_tick, this);
}

void Indeterminate_Progress::draw() {
	// Based on Fl_Progress::draw()
	int bx = Fl::box_dx(box());
	int by = Fl::box_dy(box());
	int bw = Fl::box_dw(box());
	int bh = Fl::box_dh(box());
	int tx = x() + bx;
	int tw = w() - bw;
	int bouncer_width = (int)(w() * BOUNCER_SIZE + 0.5f);
	int progress = (int)((w() - bouncer_width) * abs(_value) / MAX_VALUE + 0.5f);
	draw_box(box(), x(), y(), w(), h(), active_r() ? color() : fl_inactive(color()));
	draw_label(tx, y() + by, tw, h() - bh);
	labelcolor(fl_contrast(labelcolor(), selection_color()));
	fl_push_clip(tx + progress, y(), bouncer_width, h());
	draw_box(box(), x(), y(), w(), h(), active_r() ? selection_color() : fl_inactive(selection_color()));
	draw_label(tx, y() + by, tw, h() - bh);
	fl_pop_clip();
}

int Indeterminate_Progress::handle(int event) {
	switch (event) {
	case FL_SHOW:
		Fl::add_timeout(UPDATE_INTERVAL, (Fl_Timeout_Handler)indeterminate_progress_tick, this);
		break;
	case FL_HIDE:
		_value = 0;
		Fl::remove_timeout((Fl_Timeout_Handler)indeterminate_progress_tick, this);
		break;
	}
	return Fl_Widget::handle(event);
}

HTML_View::HTML_View(int x, int y, int w, int h, const char *l) : Fl_Help_View(x, y, w, h, l) {
	box(OS_INPUT_THIN_DOWN_BOX);
	textsize(16);
}

Dropdown::Dropdown(int x, int y, int w, int h, const char *l) : Fl_Choice(x, y, w, h, l) {
	box(OS_PANEL_THIN_UP_BOX);
	down_box(FL_FLAT_BOX);
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	align(FL_ALIGN_LEFT | FL_ALIGN_CLIP);
	textfont(OS_FONT);
	textsize(OS_FONT_SIZE);
}

void Dropdown::draw() {
	// Based on Fl_Choice::draw()
	Fl_Boxtype bb = FL_DOWN_BOX;
	int dx = Fl::box_dx(bb);
	int dy = Fl::box_dy(bb);
	int H = h() - 2 * dy;
	int W = (H > 20) ? 20 : H;
	int X = x() + w() - W - dx;
	int Y = y() + dy;
	int w1 = MAX((W - 4) / 3, 1);
	int x1 = X + (W - 2 * w1 - 1) / 2;
	int y1 = Y + (H - w1 - 1) / 2;
	if (Fl::scheme()) {
		draw_box(FL_UP_BOX, color());
		fl_color(active_r() ? labelcolor() : fl_inactive(labelcolor()));
		x1 = x() + w() - 13 - dx;
		y1 = y() + h() / 2;
		fl_polygon(x1, y1 - 2, x1 + 3, y1 - 5, x1 + 6, y1 - 2);
		fl_polygon(x1, y1 + 2, x1 + 3, y1 + 5, x1 + 6, y1 + 2);
		fl_color(active_r() ? fl_darker(color()) : fl_inactive(fl_darker(color())));
		fl_yxline(x1 - 7, y1 - 8, y1 + 8);
		fl_color(active_r() ? fl_lighter(color()) : fl_inactive(fl_lighter(color())));
		fl_yxline(x1 - 6, y1 - 8, y1 + 8);
	}
	else {
		draw_box(bb, fl_contrast(textcolor(), FL_BACKGROUND2_COLOR) == textcolor() ? FL_BACKGROUND2_COLOR : fl_lighter(color()));
		draw_box(FL_UP_BOX, X, Y, W, H, color());
		fl_color(active_r() ? labelcolor() : fl_inactive(labelcolor()));
		fl_polygon(x1, y1, x1 + w1, y1 + w1, x1 + 2 * w1, y1);
	}
	W += 2 * dx;
	if (mvalue()) {
		Fl_Menu_Item m = *mvalue();
		if (active_r()) { m.activate(); }
		else { m.deactivate(); }
		int xx = x() + dx, yy = y() + dy + 1, ww = w() - W, hh = H - 2;
		fl_push_clip(xx, yy, ww, hh);
		if (Fl::scheme()) {
			Fl_Label l;
			l.value = m.text;
			l.image = 0;
			l.deimage = 0;
			l.type = m.labeltype_;
			l.font = m.labelsize_ || m.labelfont_ ? m.labelfont_ : textfont();
			l.size = m.labelsize_ ? m.labelsize_ : textsize();
			l.color= m.labelcolor_ ? m.labelcolor_ : textcolor();
			if (!m.active()) l.color = fl_inactive((Fl_Color)l.color);
			fl_draw_shortcut = 2; // hack value to make '&' disappear
			l.draw(xx+3, yy, ww > 6 ? ww-6 : 0, hh, FL_ALIGN_LEFT);
			fl_draw_shortcut = 0;
			if (Fl::focus() == this) { draw_focus(box(), xx, yy, ww, hh); }
		}
		else {
			fl_draw_shortcut = 2; // hack value to make '&' disappear
			m.draw(xx, yy, ww, hh, this, Fl::focus() == this);
			fl_draw_shortcut = 0;
		}
		fl_pop_clip();
	}
	draw_label();
}

Control_Group::Control_Group(int x, int y, int w, int h, const char *l) : Fl_Group(x, y, w, h, l) {
	labeltype(FL_NO_LABEL);
	box(OS_GROUP_THIN_UP_BOX);
	resizable(NULL);
}

void Control_Group::draw() {
	if (damage() & ~FL_DAMAGE_CHILD) {
		draw_box(box(), active_r() ? color() : fl_inactive(color()));
		draw_label();
	}
	draw_children();
}

Panel::Panel(int x, int y, int w, int h, const char *l) : Fl_Group(x, y, w, h, l) {
	labeltype(FL_NO_LABEL);
	box(OS_PANEL_THIN_UP_FRAME);
}

OS_Tabs::OS_Tabs(int x, int y, int w, int h, const char *l) : Fl_Tabs(x, y, w, h, l) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	box(OS_TABS_BOX);
	selection_color(OS_TAB_COLOR);
}

OS_Tab::OS_Tab(int x, int y, int w, int h, const char *l) : Fl_Group(x, y, w, h, l) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	resizable(NULL);
}

OS_Scroll::OS_Scroll(int x, int y, int w, int h, const char *l) : Fl_Scroll(x, y, w, h, l) {
	scrollbar.slider(OS_BUTTON_UP_BOX);
	hscrollbar.slider(OS_BUTTON_UP_BOX);
}

Workspace::Workspace(int x, int y, int w, int h, const char *l) : Fl_Scroll(x, y, w, h, l),
	_content_w(0), _content_h(0), _ox(0), _oy(0), _cx(0), _cy(0) {
	labeltype(FL_NO_LABEL);
	box(FL_NO_BOX);
	color(FL_INACTIVE_COLOR);
}

int Workspace::handle(int event) {
	switch (event) {
	case FL_PUSH:
		if (Fl::event_button() != FL_MIDDLE_MOUSE) { break; }
		Fl::belowmouse(this);
		_ox = xposition();
		_oy = yposition();
		_cx = Fl::event_x();
		_cy = Fl::event_y();
		fl_cursor(FL_CURSOR_MOVE);
		return 1;
	case FL_RELEASE:
		fl_cursor(FL_CURSOR_DEFAULT);
		return 1;
	case FL_DRAG:
		int dx = Fl::event_x(), dy = Fl::event_y();
		int nx = _ox + (_cx - dx), ny = _oy + (_cy - dy);
		int max_x = _content_w - w() + (scrollbar.visible() ? Fl::scrollbar_size() : 0);
		int max_y = _content_h - h() + (hscrollbar.visible() ? Fl::scrollbar_size() : 0);
		scroll_to(MAX(MIN(nx, max_x), 0), MAX(MIN(ny, max_y), 0));
		return 1;
	}
	return Fl_Scroll::handle(event);
}

Toolbar::Toolbar(int x, int y, int w, int h, const char *l) : Fl_Pack(x, y, w, h, l), _spacer(0, 0, 0, 0), _alt_h(h) {
	type(HORIZONTAL);
	labeltype(FL_NO_LABEL);
	box(OS_PANEL_THIN_UP_FRAME);
	resizable(_spacer);
	spacing(0);
	clip_children(1);
	begin();
}

void Toolbar::alternate() {
	int ah = _alt_h;
	_alt_h = h();
	int aw = (ah + spacing()) * children();
	if (w() > aw) { aw = w(); }
	size(aw, ah);
}

void Toolbar::resize(int x, int y, int w, int h) {
	add(_spacer); // keep the resizable spacer as the last child
	resizable(_spacer);
	Fl_Pack::resize(x, y, w, h);
}

Alt_Widget::Alt_Widget(int w, int h) : _alt_image(NULL), _alt_deimage(NULL), _overlay_image(NULL),
	_alt_overlay_image(NULL), _alt_w(w), _alt_h(h), _alternated(false) {}

Toolbar_Button::Toolbar_Button(int x, int y, int w, int h, const char *l) : Alt_Widget(w, h), Fl_Button(x, y, w, h, l) {
	box(FL_FLAT_BOX);
	color(FL_BACKGROUND_COLOR);
	down_box(OS_DEPRESSED_DOWN_BOX);
	down_color(FL_SELECTION_COLOR);
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
}

void Toolbar_Button::alternate() {
	Fl_Image *ai = _alt_image;
	_alt_image = image();
	image(ai);
	Fl_Image *ad = _alt_deimage;
	_alt_deimage = deimage();
	deimage(ad);
	Fl_Image *ao = _alt_overlay_image;
	_alt_overlay_image = overlay_image();
	overlay_image(ao);
	int aw = _alt_w; int ah = _alt_h;
	_alt_w = w(); _alt_h = h();
	size(aw, ah);
	_alternated = !_alternated;
}

void Toolbar_Button::draw() {
	// Based on Fl_Button::draw()
	Fl_Color col = value() ? selection_color() : color();
	draw_box(value() ? down_box() ? down_box() : fl_down(box()) : box(), col);
	draw_backdrop();
	if (labeltype() == FL_NORMAL_LABEL && value()) {
		Fl_Color c = labelcolor();
		labelcolor(fl_contrast(c, col));
		draw_label();
		labelcolor(c);
	}
	else {
		draw_label();
	}
	if (_overlay_image) {
		fl_draw(NULL, x(), y(), w(), h(), align(), _overlay_image);
	}
	if (Fl::focus() == this) { draw_focus(); }
}

int Toolbar_Button::handle(int event) {
	switch (event) {
	case FL_ENTER:
		if (active_r()) {
			if (OS::current_theme() == OS::DARK) {
				box(OS_BUTTON_UP_BOX);
			}
			else {
				color(FL_LIGHT3);
			}
			redraw();
			return 1;
		}
		return 0;
	case FL_LEAVE:
		color(FL_BACKGROUND_COLOR);
		box(FL_FLAT_BOX);
		redraw();
		return 1;
	case FL_PUSH:
		Fl::focus(this);
	}
	return Fl_Button::handle(event);
}

Toolbar_Toggle_Button::Toolbar_Toggle_Button(int x, int y, int w, int h, const char *l) :
	Toolbar_Button(x, y, w, h, l) {
	type(FL_TOGGLE_BUTTON);
}

Toolbar_Radio_Button::Toolbar_Radio_Button(int x, int y, int w, int h, const char *l) :
	Toolbar_Button(x, y, w, h, l) {
	type(FL_RADIO_BUTTON);
}

static Toolbar_Dropdown_Button *pressed_dropdown_tb = NULL;

Toolbar_Dropdown_Button::Toolbar_Dropdown_Button(int x, int y, int w, int h, const char *l) : Alt_Widget(w, h),
	Fl_Menu_(x, y, w, h, l), _button_box(FL_FLAT_BOX) {
	box(OS_PANEL_THIN_UP_BOX);
	color(FL_BACKGROUND_COLOR);
	down_box(FL_FLAT_BOX);
	down_color(FL_SELECTION_COLOR);
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
}

void Toolbar_Dropdown_Button::alternate() {
	Fl_Image *ai = _alt_image;
	_alt_image = image();
	image(ai);
	Fl_Image *ad = _alt_deimage;
	_alt_deimage = deimage();
	deimage(ad);
	int aw = _alt_w; int ah = _alt_h;
	_alt_w = w(); _alt_h = h();
	size(aw, ah);
}

const Fl_Menu_Item *Toolbar_Dropdown_Button::dropdown() {
	// Based on Fl_Menu_Button::popup(...)
	pressed_dropdown_tb = this;
	redraw();
	Fl_Widget_Tracker mb(this);
	const Fl_Menu_Item *m = menu()->pulldown(x(), y(), w(), h(), NULL, this);
	picked(m);
	pressed_dropdown_tb = NULL;
	if (mb.exists()) { redraw(); }
	return m;
}

void Toolbar_Dropdown_Button::draw() {
	// Based on Fl_Button::draw()
	bool pressed = pressed_dropdown_tb == this;
	Fl_Color col = pressed ? selection_color() : color();
	draw_box(pressed ? down_box() ? down_box() : fl_down(button_box()) : button_box(), col);
	draw_backdrop();
	if (labeltype() == FL_NORMAL_LABEL && pressed) {
		Fl_Color c = labelcolor();
		labelcolor(fl_contrast(c, col));
		draw_label();
		labelcolor(c);
	}
	else {
		draw_label();
	}
	if (Fl::focus() == this) { draw_focus(); }
}

int Toolbar_Dropdown_Button::handle(int event) {
	// Based on Fl_Menu_Button::handle(int e)
	if (!menu() || !menu()->text) { return 0; }
	switch (event) {
	case FL_ENTER:
		if (active_r()) {
			color(FL_LIGHT3);
			redraw();
			return button_box() ? 1 : 0;
		}
		return 0;
	case FL_LEAVE:
		color(FL_BACKGROUND_COLOR);
		redraw();
		return button_box() ? 1 : 0;
	case FL_PUSH:
		if (!button_box()) {
			if (Fl::event_button() != FL_RIGHT_MOUSE) { return 0; }
		}
		Fl::focus(this);
		color(FL_BACKGROUND_COLOR);
		dropdown();
		return 1;
	case FL_KEYBOARD:
		if (!button_box()) { return 0; }
		if (Fl::event_key() == ' ' && !(Fl::event_state() & (FL_SHIFT | FL_CTRL | FL_ALT | FL_META))) {
			dropdown();
			return 1;
		}
		return 0;
	case FL_SHORTCUT:
		if (Fl_Widget::test_shortcut()) {
			dropdown();
			return 1;
		}
		return test_shortcut() != 0;
	case FL_FOCUS:
	case FL_UNFOCUS:
		if (button_box() && Fl::visible_focus()) {
			redraw();
			return 1;
		}
	}
	return 0;
}

Status_Bar_Field::Status_Bar_Field(int x, int y, int w, int h, const char *l) : Fl_Box(x, y, w, h, l),
	_default_label(l) {
	labelfont(OS_FONT);
	labelsize(OS_FONT_SIZE);
	align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
}

Status_Bar_Field::~Status_Bar_Field() { delete _default_label; }
