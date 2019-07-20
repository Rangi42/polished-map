#pragma warning(push, 0)
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "ruler.h"
#include "main-window.h"
#include "themes.h"
#include "tile.h"
#include "event.h"

Ruler::Ruler(int x, int y, int w, int h, const char *l) : Fl_Box(x, y, w, h, l) {}

static inline void print_tick_label(char *s, int n, bool hex) {
	if (hex) {
		if (n < 0) {
			sprintf(s, "-%02X\n", -n);
		}
		else {
			sprintf(s, "%02X", n);
		}
	}
	else {
		sprintf(s, "%d", n);
	}
}

void Ruler::draw() {
	int X = x(), Y = y(), W = w(), H = h();
	Main_Window *mw = (Main_Window *)user_data();
	int S = TILE_PX_SIZE * (mw->zoom() ? ZOOM_FACTOR : 1);
	// background
	fl_color(FL_DARK2);
	fl_rectf(X, Y, W, H);
	// edges
	fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND_COLOR, 0.4));
	if (_direction != Direction::HORIZONTAL) {
		fl_yxline(X+W-1, Y, Y+H-1);
	}
	if (_direction != Direction::VERTICAL) {
		fl_xyline(X, Y+H-1, X+W-1);
	}
	// tick marks and labels
	if (_direction == Direction::CORNER) {
		return;
	}
	if (_direction == Direction::HORIZONTAL) {
		int mx = mw->map_scroll_x();
		int es = mw->metatile_size() / 2;
		int r = mx % es;
		// tick marks
		int d = 6;
		for (int i = S-r-1; i < W; i += S, d = d ? 0 : 6) {
			fl_yxline(X+i, Y+d+2, Y+H-1);
		}
		// labels
		char t[8] = {};
		bool hex = mw->hex();
		fl_font(FL_COURIER, 9);
		fl_color(FL_FOREGROUND_COLOR);
		int en = mw->status_event_x();
		int n = mx / es - EVENT_MARGIN;
		for (int i = S-r-1; i < W && n <= INT8_MAX; i += S, n++) {
			print_tick_label(t, n, hex);
			if (n == en) {
				fl_rectf(X+i-S+1, Y, S-1, H-1);
				fl_color(FL_BACKGROUND_COLOR);
			}
			fl_draw(t, X+i-S+1, Y, S-2, H, FL_ALIGN_BOTTOM_RIGHT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
			if (n == en) {
				fl_color(FL_FOREGROUND_COLOR);
			}
		}
	}
	else if (_direction == Direction::VERTICAL) {
		int my = mw->map_scroll_y();
		int es = mw->metatile_size() / 2;
		int r = my % es;
		// tick marks
		int d = 6;
		for (int i = S-r-1; i < H; i += S, d = d ? 0 : 6) {
			fl_xyline(X+d+2, Y+i, X+W-1);
		}
		// labels
		char t[8] = {};
		bool hex = mw->hex();
		fl_font(FL_COURIER, 9);
		fl_color(FL_FOREGROUND_COLOR);
		int en = mw->status_event_y();
		int n = my / es - EVENT_MARGIN;
		for (int i = S-r-1; i < H && n <= INT8_MAX; i += S, n++) {
			print_tick_label(t, n, hex);
			if (n == en) {
				fl_rectf(X, Y+i-S+1, W-1, S-1);
				fl_color(FL_BACKGROUND_COLOR);
			}
			fl_draw(t, X, Y+i-S+1, W-2, S, FL_ALIGN_BOTTOM_RIGHT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
			if (n == en) {
				fl_color(FL_FOREGROUND_COLOR);
			}
		}
	}
}