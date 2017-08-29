#include <cstdlib>
#include <cstring>

#pragma warning(push, 0)
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Tooltip.H>
#pragma warning(pop)

#include "utils.h"
#include "math.h"
#include "themes.h"

bool OS::is_modern_windows() {
	// Return true for Windows 8 and above, false for Windows 7 and below
	OSVERSIONINFOEX osvi;
	DWORDLONG cm = 0;
	char op = VER_GREATER_EQUAL;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 6;
	osvi.dwMinorVersion = 2;
	osvi.wServicePackMajor = 0;
	osvi.wServicePackMinor = 0;
	VER_SET_CONDITION(cm, VER_MAJORVERSION, op);
	VER_SET_CONDITION(cm, VER_MINORVERSION, op);
	VER_SET_CONDITION(cm, VER_SERVICEPACKMAJOR, op);
	VER_SET_CONDITION(cm, VER_SERVICEPACKMINOR, op);
	return VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION |
		VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR, cm) != FALSE;
}

static void use_font(const char *font, const char *alt_font) {
	Fl::set_font(OS_FONT, FL_HELVETICA);
	int num_fonts = Fl::set_fonts(NULL);
	for (Fl_Font f = 0; f < num_fonts; f++) {
		const char *name = Fl::get_font_name(f);
		if (!strcmp(name, alt_font)) {
			Fl::set_font(OS_FONT, name);
		}
		if (!strcmp(name, font)) {
			Fl::set_font(OS_FONT, name);
			break;
		}
	}
	fl_font(OS_FONT, OS_FONT_SIZE);
}

static Fl_Color activated_color(Fl_Color c) {
	return Fl::draw_box_active() ? c : fl_inactive(c);
}

static Fl_Color desaturated(Fl_Color c, float a) {
	return fl_color_average(FL_WHITE, c, a);
}

static Fl_Color devalued(Fl_Color c, float a) {
	return fl_color_average(FL_BLACK, c, a);
}

static void vertical_gradient(int x1, int y1, int x2, int y2, Fl_Color c1, Fl_Color c2) {
	int imax = y2-y1;
	if (Fl::draw_box_active()) {
		for (int i = 0; i <= imax; i++) {
			float w = 1.0f-(float)i / imax;
			fl_color(fl_color_average(c1, c2, w));
			fl_xyline(x1, y1+i, x2);
		}
	}
	else {
		for (int i = 0; i <= imax; i++) {
			float w = 1.0f-(float)i / imax;
			fl_color(fl_inactive(fl_color_average(c1, c2, w)));
			fl_xyline(x1, y1+i, x2);
		}
	}
}

/****************************** Aero (Windows 7) ******************************/

static void aero_button_up_frame(int x, int y, int w, int h, Fl_Color) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0x70, 0x70, 0x70)));
	fl_xyline(x+2, y, x+w-3);
	fl_xyline(x+2, y+h-1, x+w-3);
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// top inner borders
	fl_color(activated_color(fl_rgb_color(0xFB, 0xFB, 0xFB)));
	fl_xyline(x+2, y+1, x+w-3);
	fl_yxline(x+1, y+2, y+h/2-1);
	fl_yxline(x+w-2, y+2, y+h/2-1);
	fl_point(x+2, y+2);
	fl_point(x+w-3, y+2);
	// bottom inner borders
	fl_color(activated_color(fl_rgb_color(0xF2, 0xF2, 0xF2)));
	fl_yxline(x+1, y+h/2, y+h-3);
	fl_yxline(x+w-2, y+h/2, y+h-3);
	fl_xyline(x+2, y+h-2, x+w-3);
	fl_point(x+2, y+h-3);
	fl_point(x+w-3, y+h-3);
	// top corners
	fl_color(activated_color(fl_rgb_color(0x90, 0x90, 0x90)));
	fl_xyline(x, y+1, x+1, y);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	// bottom corners
	fl_color(activated_color(fl_rgb_color(0x88, 0x88, 0x88)));
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
}

static void aero_button_up_box(int x, int y, int w, int h, Fl_Color c) {
	// top gradient
	vertical_gradient(x+2, y+2, x+w-3, y+h/2-1, activated_color(fl_rgb_color(0xF1, 0xF1, 0xF1)),
		activated_color(fl_rgb_color(0xEA, 0xEA, 0xEA)));
	// bottom gradient
	vertical_gradient(x+2, y+h/2, x+w-3, y+h-3, activated_color(fl_rgb_color(0xDC, 0xDC, 0xDC)),
		activated_color(fl_rgb_color(0xCE, 0xCE, 0xCE)));
	aero_button_up_frame(x, y, w, h, c);
}

static void aero_check_down_frame(int x, int y, int w, int h, Fl_Color) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0x8F, 0x8F, 0x8F)));
	fl_rect(x, y, w, h);
	// middle border
	fl_color(activated_color(fl_rgb_color(0xF4, 0xF4, 0xF4)));
	fl_rect(x+1, y+1, w-2, h-2);
	// top and left inner borders
	fl_color(activated_color(fl_rgb_color(0xAE, 0xB3, 0xB9)));
	fl_yxline(x+2, y+h-3, y+2, x+w-3);
	// bottom and right inner borders
	fl_color(activated_color(fl_rgb_color(0xE9, 0xE9, 0xEA)));
	fl_xyline(x+3, y+h-3, x+w-3, y+3);
}

static void aero_check_down_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(fl_rgb_color(0xF5, 0xF5, 0xF5)));
	fl_rectf(x+3, y+3, w-6, h-6);
	aero_check_down_frame(x, y, w, h, c);
}

static void aero_panel_thin_up_frame(int x, int y, int w, int h, Fl_Color) {
	// top and left borders
	fl_color(activated_color(fl_rgb_color(0xFA, 0xFA, 0xFA)));
	fl_yxline(x, y+h-2, y, x+w-2);
	// bottom and right borders
	fl_color(activated_color(fl_rgb_color(0xC9, 0xC9, 0xC9)));
	fl_xyline(x, y+h-1, x+w-1, y);
}

static void aero_panel_thin_up_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+1, w-2, h-2);
	aero_panel_thin_up_frame(x, y, w, h, c);
}

static void aero_spacer_thin_down_frame(int x, int y, int w, int h, Fl_Color) {
	// top and left borders
	fl_color(activated_color(fl_rgb_color(0xC9, 0xC9, 0xC9)));
	fl_yxline(x, y+h-2, y, x+w-2);
	// bottom and right borders
	fl_color(activated_color(fl_rgb_color(0xFA, 0xFA, 0xFA)));
	fl_xyline(x, y+h-1, x+w-1, y);
}

static void aero_spacer_thin_down_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+1, w-2, h-2);
	aero_spacer_thin_down_frame(x, y, w, h, c);
}

static void aero_progress_round_up_frame(int x, int y, int w, int h, Fl_Color c) {
	// top outer borders
	fl_color(activated_color(fl_rgb_color(0xB2, 0xB2, 0xB2)));
	fl_xyline(x+2, y, x+w-3);
	// side outer borders
	fl_color(activated_color(fl_rgb_color(0x9F, 0x9F, 0x9F)));
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// bottom outer border
	fl_color(activated_color(fl_rgb_color(0x8C, 0x8C, 0x8C)));
	fl_xyline(x+2, y+h-1, x+w-3);
	// side inner top borders
	fl_color(activated_color(devalued(desaturated(c, 0.27f), 0.07f)));
	fl_yxline(x+1, y+2, y+h/2-1);
	fl_yxline(x+w-2, y+2, y+h/2-1);
	// side inner bottom borders
	fl_color(activated_color(devalued(c, 0.19f)));
	fl_yxline(x+1, y+h/2, y+h-3);
	fl_yxline(x+w-2, y+h/2, y+h-3);
	// top corners
	fl_color(activated_color(fl_rgb_color(0xC9, 0xC9, 0xC9)));
	fl_xyline(x, y+1, x+1, y);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	// bottom corners
	fl_color(activated_color(fl_rgb_color(0xA1, 0xA1, 0xA1)));
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
}

static void aero_progress_round_up_box(int x, int y, int w, int h, Fl_Color c) {
	// top fill
	fl_color(activated_color(devalued(desaturated(c, 0.52f), 0.06f)));
	fl_rectf(x+2, y+1, w-4, h/2-1);
	// top gradient
	vertical_gradient(x+2, y+1, x+w-3, y+6, activated_color(desaturated(c, 0.77f)),
		activated_color(devalued(desaturated(c, 0.52f), 0.06f)));
	// bottom fill
	fl_color(activated_color(devalued(c, 0.16f)));
	fl_rectf(x+2, y+h/2, w-4, h/2-1);
	// bottom gradient
	vertical_gradient(x+2, y+h-7, x+w-3, y+h-2, activated_color(devalued(c, 0.16f)),
		activated_color(devalued(c, 0.07f)));
	aero_progress_round_up_frame(x, y, w, h, c);
}

static void aero_radio_round_down_frame(int x, int y, int w, int h, Fl_Color) {
	// middle border
	fl_color(activated_color(fl_rgb_color(0xF4, 0xF4, 0xF4)));
	fl_arc(x+1, y+1, w-2, h-2, 0.0, 360.0);
	// outer border
	fl_color(activated_color(fl_rgb_color(0x8F, 0x8F, 0x8F)));
	fl_arc(x, y, w, h, 0.0, 360.0);
	// top and left inner border
	fl_color(activated_color(fl_rgb_color(0xAE, 0xAE, 0xAE)));
	fl_arc(x+2, y+2, w-4, h-4, 45.0, 225.0);
	// bottom and right inner border
	fl_color(activated_color(fl_rgb_color(0xE4, 0xE4, 0xE4)));
	fl_arc(x+2, y+2, w-4, h-4, -135.0, 45.0);
}

static void aero_radio_round_down_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(fl_rgb_color(0xF5, 0xF5, 0xF5)));
	fl_pie(x+2, y+2, w-4, h-4, 0.0, 360.0);
	aero_radio_round_down_frame(x, y, w, h, c);
}

static void aero_hovered_up_frame(int x, int y, int w, int h, Fl_Color) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0x3C, 0x7F, 0xB0)));
	fl_xyline(x+2, y, x+w-3);
	fl_xyline(x+2, y+h-1, x+w-3);
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// top inner borders
	fl_color(activated_color(fl_rgb_color(0xF9, 0xFC, 0xFD)));
	fl_xyline(x+2, y+1, x+w-3);
	fl_yxline(x+1, y+2, y+h/2-1);
	fl_yxline(x+w-2, y+2, y+h/2-1);
	fl_point(x+2, y+2);
	fl_point(x+w-3, y+2);
	// bottom inner borders
	fl_color(activated_color(fl_rgb_color(0xE7, 0xF4, 0xFB)));
	fl_yxline(x+1, y+h/2, y+h-3);
	fl_yxline(x+w-2, y+h/2, y+h-3);
	fl_xyline(x+2, y+h-2, x+w-3);
	fl_point(x+2, y+h-3);
	fl_point(x+w-3, y+h-3);
	// top corners
	fl_color(activated_color(fl_rgb_color(0x69, 0x9C, 0xC2)));
	fl_xyline(x, y+1, x+1, y);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	// bottom corners
	fl_color(activated_color(fl_rgb_color(0x64, 0x9A, 0xC2)));
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
}

static void aero_hovered_up_box(int x, int y, int w, int h, Fl_Color c) {
	// top gradient
	vertical_gradient(x+2, y+2, x+w-3, y+h/2-1, activated_color(fl_rgb_color(0xE9, 0xF5, 0xFC)),
		activated_color(fl_rgb_color(0xD8, 0xEF, 0xFB)));
	// bottom gradient
	vertical_gradient(x+2, y+h/2, x+w-3, y+h-3, activated_color(fl_rgb_color(0xBD, 0xE5, 0xFC)),
		activated_color(fl_rgb_color(0xA7, 0xD8, 0xF4)));
	aero_hovered_up_frame(x, y, w, h, c);
}

static void aero_depressed_down_frame(int x, int y, int w, int h, Fl_Color) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0x2C, 0x62, 0x8B)));
	fl_xyline(x+2, y, x+w-3);
	fl_xyline(x+2, y+h-1, x+w-3);
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// top inner borders
	fl_color(activated_color(fl_rgb_color(0x9D, 0xAF, 0xB9)));
	fl_xyline(x+2, y+1, x+w-3);
	fl_yxline(x+1, y+2, y+h/2-1);
	fl_yxline(x+w-2, y+2, y+h/2-1);
	fl_point(x+2, y+2);
	fl_point(x+w-3, y+2);
	// bottom inner borders
	fl_color(activated_color(fl_rgb_color(0x55, 0x92, 0xB5)));
	fl_yxline(x+1, y+h/2, y+h-3);
	fl_yxline(x+w-2, y+h/2, y+h-3);
	// corners
	fl_color(activated_color(fl_rgb_color(0x33, 0x56, 0x71)));
	fl_xyline(x, y+1, x+1, y);
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
}

static void aero_depressed_down_box(int x, int y, int w, int h, Fl_Color c) {
	// top gradient
	vertical_gradient(x+2, y+2, x+w-3, y+h/2-1, activated_color(fl_rgb_color(0xE4, 0xF3, 0xFB)),
		activated_color(fl_rgb_color(0xC4, 0xE5, 0xF6)));
	// bottom gradient
	vertical_gradient(x+2, y+h/2, x+w-3, y+h-2, activated_color(fl_rgb_color(0x98, 0xD1, 0xEF)),
		activated_color(fl_rgb_color(0x68, 0xB3, 0xDB)));
	aero_depressed_down_frame(x, y, w, h, c);
}

static void aero_group_thin_up_frame(int x, int y, int w, int h, Fl_Color) {
	// outer borders
	fl_color(activated_color(fl_rgb_color(0xD5, 0xDF, 0xE5)));
	fl_xyline(x+2, y, x+w-3);
	fl_xyline(x+2, y+h-2, x+w-3);
	fl_yxline(x, y+2, y+h-4);
	fl_yxline(x+w-1, y+2, y+h-4);
	// corners
	fl_point(x+1, y+1);
	fl_point(x+w-2, y+1);
	fl_point(x+w-2, y+h-3);
	fl_point(x+1, y+h-3);
	// inner borders
	fl_color(activated_color(fl_rgb_color(0xFF, 0xFF, 0xFF)));
	fl_xyline(x+2, y+1, x+w-3);
	fl_xyline(x+2, y+h-1, x+w-3);
	fl_yxline(x+1, y+2, y+h-4);
	fl_yxline(x+w-2, y+2, y+h-4);
	// inner corners
	fl_point(x+w-1, y+h-3);
	fl_point(x+w-2, y+h-2);
	fl_point(x+1, y+h-2);
	fl_point(x, y+h-3);
}

static void aero_group_thin_up_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+2, w-2, h-4);
	aero_group_thin_up_frame(x, y, w, h, c);
}

static void aero_input_thin_down_frame(int x, int y, int w, int h, Fl_Color) {
	// top border
	fl_color(activated_color(fl_rgb_color(0xAA, 0xAC, 0xB2)));
	fl_xyline(x+1, y, x+w-2);
	// side borders
	fl_color(activated_color(fl_rgb_color(0xDA, 0xDE, 0xE5)));
	fl_yxline(x, y+1, y+h-2);
	fl_yxline(x+w-1, y+1, y+h-2);
	// bottom border
	fl_color(activated_color(fl_rgb_color(0xE2, 0xE8, 0xEE)));
	fl_xyline(x+1, y+h-1, x+w-2);
	// inner corners
	fl_color(activated_color(fl_rgb_color(0xE8, 0xEB, 0xEF)));
	fl_point(x+1, y+1);
	fl_point(x+w-2, y+1);
	fl_point(x+1, y+h-2);
	fl_point(x+w-2, y+h-2);
}

static void aero_input_thin_down_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+1, w-2, h-2);
	aero_input_thin_down_frame(x, y, w, h, c);
}

static void aero_default_button_frame(int x, int y, int w, int h, Fl_Color) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0x3C, 0x7F, 0xB1)));
	fl_xyline(x+2, y, x+w-3);
	fl_xyline(x+2, y+h-1, x+w-3);
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// top inner borders
	fl_color(activated_color(fl_rgb_color(0x40, 0xD7, 0xFC)));
	fl_xyline(x+2, y+1, x+w-3);
	fl_yxline(x+1, y+2, y+h/2-1);
	fl_yxline(x+w-2, y+2, y+h/2-1);
	fl_point(x+2, y+2);
	fl_point(x+w-3, y+2);
	// bottom inner borders
	fl_color(activated_color(fl_rgb_color(0x35, 0xCE, 0xF4)));
	fl_yxline(x+1, y+h/2, y+h-3);
	fl_yxline(x+w-2, y+h/2, y+h-3);
	fl_xyline(x+2, y+h-2, x+w-3);
	fl_point(x+2, y+h-3);
	fl_point(x+w-3, y+h-3);
	// top innermost borders
	fl_color(activated_color(fl_rgb_color(0xD2, 0xEE, 0xF6)));
	fl_xyline(x+3, y+2, x+w-4);
	fl_yxline(x+2, y+3, y+h/2-1);
	fl_yxline(x+w-3, y+3, y+h/2-1);
	// bottom innermost borders
	fl_color(activated_color(fl_rgb_color(0xB0, 0xD1, 0xDC)));
	fl_yxline(x+2, y+h/2, y+h-4);
	fl_yxline(x+w-3, y+h/2, y+h-4);
	fl_xyline(x+3, y+h-3, x+w-4);
	// top corners
	fl_color(activated_color(fl_rgb_color(0x3A, 0x93, 0xC2)));
	fl_xyline(x, y+1, x+1, y);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	// bottom corners
	fl_color(activated_color(fl_rgb_color(0x38, 0x91, 0xC1)));
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
}

static void aero_default_button_box(int x, int y, int w, int h, Fl_Color c) {
	// top gradient
	vertical_gradient(x+3, y+3, x+w-4, y+h/2-1, activated_color(fl_rgb_color(0xF0, 0xF3, 0xF5)),
		activated_color(fl_rgb_color(0xE9, 0xEE, 0xF1)));
	// bottom gradient
	vertical_gradient(x+3, y+h/2, x+w-4, y+h-4, activated_color(fl_rgb_color(0xD7, 0xE1, 0xE7)),
		activated_color(fl_rgb_color(0xC8, 0xD5, 0xDD)));
	aero_default_button_frame(x, y, w, h, c);
}

static void aero_tabs_frame(int x, int y, int w, int h, Fl_Color) {
	fl_color(activated_color(fl_rgb_color(0x88, 0x8B, 0x94)));
	fl_rect(x, y, w, h);
}

static void aero_tabs_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+1, w-2, h-2);
	aero_tabs_frame(x, y, w, h, c);
}

static void aero_swatch_frame(int x, int y, int w, int h, Fl_Color) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0xA0, 0xA0, 0xA0)));
	fl_rect(x, y, w, h);
	// inner border
	fl_color(activated_color(fl_rgb_color(0xFF, 0xFF, 0xFF)));
	fl_rect(x+1, y+1, w-2, h-2);
}

static void aero_swatch_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+2, y+2, w-4, h-4);
	aero_swatch_frame(x, y, w, h, c);
}

void use_aero_font() {
	use_font("Segoe UI", "Tahoma");
}

static void use_aero_scheme() {
	Fl::scheme("gtk+");
	Fl::set_boxtype(OS_BUTTON_UP_BOX, aero_button_up_box, 2, 2, 4, 4);
	Fl::set_boxtype(OS_CHECK_DOWN_BOX, aero_check_down_box, 2, 2, 4, 4);
	Fl::set_boxtype(OS_BUTTON_UP_FRAME, aero_button_up_frame, 2, 2, 4, 4);
	Fl::set_boxtype(OS_CHECK_DOWN_FRAME, aero_check_down_frame, 2, 2, 4, 4);
	Fl::set_boxtype(OS_PANEL_THIN_UP_BOX, aero_panel_thin_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_SPACER_THIN_DOWN_BOX, aero_spacer_thin_down_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_PANEL_THIN_UP_FRAME, aero_panel_thin_up_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_SPACER_THIN_DOWN_FRAME, aero_spacer_thin_down_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_PROGRESS_ROUND_UP_BOX, aero_progress_round_up_box, 2, 1, 4, 2);
	Fl::set_boxtype(OS_RADIO_ROUND_DOWN_BOX, aero_radio_round_down_box, 3, 3, 6, 6);
	Fl::set_boxtype(OS_HOVERED_UP_BOX, aero_hovered_up_box, 2, 2, 4, 4);
	Fl::set_boxtype(OS_DEPRESSED_DOWN_BOX, aero_depressed_down_box, 2, 2, 4, 4);
	Fl::set_boxtype(OS_HOVERED_UP_FRAME, aero_hovered_up_frame, 2, 2, 4, 4);
	Fl::set_boxtype(OS_DEPRESSED_DOWN_FRAME, aero_depressed_down_frame, 2, 2, 4, 4);
	Fl::set_boxtype(OS_GROUP_THIN_UP_BOX, aero_group_thin_up_box, 2, 2, 4, 4);
	Fl::set_boxtype(OS_INPUT_THIN_DOWN_BOX, aero_input_thin_down_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_GROUP_THIN_UP_FRAME, aero_group_thin_up_frame, 2, 2, 4, 4);
	Fl::set_boxtype(OS_INPUT_THIN_DOWN_FRAME, aero_input_thin_down_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_DEFAULT_BUTTON_BOX, aero_default_button_box, 2, 2, 4, 4);
	Fl::set_boxtype(OS_TABS_BOX, aero_tabs_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_SWATCH_BOX, aero_swatch_box, 2, 2, 4, 4);
	Fl::set_boxtype(FL_UP_BOX, aero_button_up_box, 2, 2, 4, 4);
	Fl::set_boxtype(FL_DOWN_BOX, aero_check_down_box, 2, 2, 4, 4);
	Fl::set_boxtype(FL_ROUND_DOWN_BOX, aero_radio_round_down_box, 3, 3, 6, 6);
}

static void use_aero_colors() {
	Fl::background(0xF0, 0xF0, 0xF0);
	Fl::background2(0xFF, 0xFF, 0xFF);
	Fl::foreground(0x00, 0x00, 0x00);
	Fl::set_color(FL_SELECTION_COLOR, 0x33, 0x99, 0xFF);
	Fl::set_color(OS_TAB_COLOR, 0xFF, 0xFF, 0xFF);
	Fl::set_color(OS_PROGRESS_COLOR, 0x00, 0xFF, 0x4C);
	Fl_Tooltip::color(fl_rgb_color(0xFF, 0xFF, 0xF0));
	Fl_Tooltip::textcolor(FL_FOREGROUND_COLOR);
}

void OS::use_aero_theme() {
	use_aero_font();
	use_aero_scheme();
	use_aero_colors();
	use_native_settings();
	global_current_theme = AERO;
}

/****************************** Metro (Windows 8) *****************************/

static void metro_button_up_frame(int x, int y, int w, int h, Fl_Color) {
	fl_color(activated_color(fl_rgb_color(0xAC, 0xAC, 0xAC)));
	fl_rect(x, y, w, h);
}

static void metro_button_up_box(int x, int y, int w, int h, Fl_Color c) {
	vertical_gradient(x+1, y+1, x+w-2, y+h-2, activated_color(fl_rgb_color(0xF0, 0xF0, 0xF0)),
		activated_color(fl_rgb_color(0xE5, 0xE5, 0xE5)));
	metro_button_up_frame(x, y, w, h, c);
}

static void metro_check_down_frame(int x, int y, int w, int h, Fl_Color) {
	fl_color(activated_color(fl_rgb_color(0x70, 0x70, 0x70)));
	fl_rect(x, y, w, h);
}

static void metro_check_down_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(fl_rgb_color(0xFF, 0xFF, 0xFF)));
	fl_rectf(x+1, y+1, w-2, h-2);
	metro_check_down_frame(x, y, w, h, c);
}

static void metro_progress_round_up_frame(int x, int y, int w, int h, Fl_Color) {
	fl_color(activated_color(fl_rgb_color(0xBB, 0xBB, 0xBB)));
	fl_rect(x, y, w, h);
}

static void metro_progress_round_up_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+1, w-2, h-2);
	metro_progress_round_up_frame(x, y, w, h, c);
}

static void metro_radio_round_down_frame(int x, int y, int w, int h, Fl_Color) {
	fl_color(activated_color(fl_rgb_color(0x73, 0x73, 0x73)));
	fl_arc(x, y, w, h, 0.0, 360.0);
}

static void metro_radio_round_down_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(fl_rgb_color(0xFF, 0xFF, 0xFF)));
	fl_pie(x, y, w, h, 0.0, 360.0);
	metro_radio_round_down_frame(x, y, w, h, c);
}

static void metro_hovered_up_frame(int x, int y, int w, int h, Fl_Color) {
	fl_color(activated_color(fl_rgb_color(0x7E, 0xB4, 0xEA)));
	fl_rect(x, y, w, h);
}

static void metro_hovered_up_box(int x, int y, int w, int h, Fl_Color c) {
	vertical_gradient(x+1, y+1, x+w-2, y+h-2, activated_color(fl_rgb_color(0xEC, 0xF4, 0xFC)),
		activated_color(fl_rgb_color(0xDC, 0xEC, 0xFC)));
	metro_hovered_up_frame(x, y, w, h, c);
}

static void metro_depressed_down_frame(int x, int y, int w, int h, Fl_Color) {
	fl_color(activated_color(fl_rgb_color(0x56, 0x9D, 0xE5)));
	fl_rect(x, y, w, h);
}

static void metro_depressed_down_box(int x, int y, int w, int h, Fl_Color c) {
	vertical_gradient(x+1, y+1, x+w-2, y+h-2, activated_color(fl_rgb_color(0xDA, 0xEC, 0xFC)),
		activated_color(fl_rgb_color(0xC4, 0xE0, 0xFC)));
	metro_depressed_down_frame(x, y, w, h, c);
}

static void metro_group_thin_up_frame(int x, int y, int w, int h, Fl_Color) {
	fl_color(activated_color(fl_rgb_color(0xDD, 0xDD, 0xDD)));
	fl_rect(x, y, w, h);
}

static void metro_group_thin_up_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+1, w-2, h-2);
	metro_group_thin_up_frame(x, y, w, h, c);
}


static void metro_input_thin_down_frame(int x, int y, int w, int h, Fl_Color) {
	fl_color(activated_color(fl_rgb_color(0xAB, 0xAD, 0xB3)));
	fl_rect(x, y, w, h);
}

static void metro_input_thin_down_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(fl_rgb_color(0xFF, 0xFF, 0xFF)));
	fl_rectf(x+1, y+1, w-2, h-2);
	metro_input_thin_down_frame(x, y, w, h, c);
}

static void metro_default_button_frame(int x, int y, int w, int h, Fl_Color) {
	fl_color(activated_color(fl_rgb_color(0x33, 0x99, 0xFF)));
	fl_rect(x, y, w, h);
}

static void metro_default_button_box(int x, int y, int w, int h, Fl_Color c) {
	vertical_gradient(x+1, y+1, x+w-2, y+h-2, activated_color(fl_rgb_color(0xF0, 0xF0, 0xF0)),
		activated_color(fl_rgb_color(0xE5, 0xE5, 0xE5)));
	metro_default_button_frame(x, y, w, h, c);
}

void use_metro_font() {
	use_aero_font();
}

static void use_metro_scheme() {
	Fl::scheme("gtk+");
	Fl::set_boxtype(OS_BUTTON_UP_BOX, metro_button_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_CHECK_DOWN_BOX, metro_check_down_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_BUTTON_UP_FRAME, metro_button_up_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_CHECK_DOWN_FRAME, metro_check_down_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_PANEL_THIN_UP_BOX, aero_panel_thin_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_SPACER_THIN_DOWN_BOX, aero_spacer_thin_down_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_PANEL_THIN_UP_FRAME, aero_panel_thin_up_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_SPACER_THIN_DOWN_FRAME, aero_spacer_thin_down_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_PROGRESS_ROUND_UP_BOX, metro_progress_round_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_RADIO_ROUND_DOWN_BOX, metro_radio_round_down_box, 3, 3, 6, 6);
	Fl::set_boxtype(OS_HOVERED_UP_BOX, metro_hovered_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_DEPRESSED_DOWN_BOX, metro_depressed_down_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_HOVERED_UP_FRAME, metro_hovered_up_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_DEPRESSED_DOWN_FRAME, metro_depressed_down_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_GROUP_THIN_UP_BOX, metro_group_thin_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_INPUT_THIN_DOWN_BOX, metro_input_thin_down_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_GROUP_THIN_UP_FRAME, metro_group_thin_up_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_INPUT_THIN_DOWN_FRAME, metro_input_thin_down_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_DEFAULT_BUTTON_BOX, metro_default_button_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_TABS_BOX, aero_tabs_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_SWATCH_BOX, aero_swatch_box, 2, 2, 4, 4);
	Fl::set_boxtype(FL_UP_BOX, metro_button_up_box, 2, 2, 4, 4);
	Fl::set_boxtype(FL_DOWN_BOX, metro_check_down_box, 1, 1, 2, 2);
	Fl::set_boxtype(FL_ROUND_DOWN_BOX, metro_radio_round_down_box, 3, 3, 6, 6);
}

static void use_metro_colors() {
	Fl::reload_scheme();
	Fl::background(0xF0, 0xF0, 0xF0);
	Fl::background2(0xFF, 0xFF, 0xFF);
	Fl::foreground(0x00, 0x00, 0x00);
	Fl::set_color(FL_SELECTION_COLOR, 0x33, 0x99, 0xFF);
	Fl::set_color(OS_TAB_COLOR, 0xFF, 0xFF, 0xFF);
	Fl::set_color(OS_PROGRESS_COLOR, 0x00, 0xB2, 0x3A);
	Fl_Tooltip::color(fl_rgb_color(0xFF, 0xFF, 0xFF));
	Fl_Tooltip::textcolor(FL_FOREGROUND_COLOR);
}

void OS::use_metro_theme() {
	use_metro_font();
	use_metro_scheme();
	use_metro_colors();
	use_native_settings();
	global_current_theme = METRO;
}

/************************** Blue (Windows Calculator) *************************/

static void blue_button_up_frame(int x, int y, int w, int h, Fl_Color) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0x87, 0x97, 0xAA)));
	fl_xyline(x+2, y, x+w-3);
	fl_xyline(x+2, y+h-1, x+w-3);
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// top inner borders
	fl_color(activated_color(fl_rgb_color(0xF6, 0xFA, 0xFE)));
	fl_xyline(x+2, y+1, x+w-3);
	fl_yxline(x+1, y+2, y+h/2-1);
	fl_yxline(x+w-2, y+2, y+h/2-1);
	fl_point(x+2, y+2);
	fl_point(x+w-3, y+2);
	// bottom inner borders
	fl_color(activated_color(fl_rgb_color(0xFE, 0xFF, 0xFF)));
	fl_yxline(x+1, y+h/2, y+h-3);
	fl_yxline(x+w-2, y+h/2, y+h-3);
	fl_xyline(x+2, y+h-2, x+w-3);
	fl_point(x+2, y+h-3);
	fl_point(x+w-3, y+h-3);
	// top corners
	fl_color(activated_color(fl_rgb_color(0x9B, 0xAA, 0xBB)));
	fl_xyline(x, y+1, x+1, y);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	// bottom corners
	fl_color(activated_color(fl_rgb_color(0xA1, 0xAE, 0xBD)));
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
}

static void blue_button_up_box(int x, int y, int w, int h, Fl_Color c) {
	// top gradient
	vertical_gradient(x+2, y+2, x+w-3, y+h/2-1, activated_color(fl_rgb_color(0xF0, 0xF6, 0xFB)),
		activated_color(fl_rgb_color(0xE2, 0xEA, 0xF3)));
	// bottom gradient
	vertical_gradient(x+2, y+h/2, x+w-3, y+h-3, activated_color(fl_rgb_color(0xD5, 0xE0, 0xED)),
		activated_color(fl_rgb_color(0xD7, 0xE2, 0xEF)));
	blue_button_up_frame(x, y, w, h, c);
}

static void blue_panel_thin_up_frame(int x, int y, int w, int h, Fl_Color) {
	// top and left borders
	fl_color(activated_color(fl_rgb_color(0xF0, 0xF6, 0xFB)));
	fl_yxline(x, y+h-2, y, x+w-2);
	// bottom and right borders
	fl_color(activated_color(fl_rgb_color(0xBF, 0xCB, 0xDA)));
	fl_xyline(x, y+h-1, x+w-1, y);
}

static void blue_panel_thin_up_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+1, w-2, h-2);
	blue_panel_thin_up_frame(x, y, w, h, c);
}

static void blue_spacer_thin_down_frame(int x, int y, int w, int h, Fl_Color) {
	// top and left borders
	fl_color(activated_color(fl_rgb_color(0xBF, 0xCB, 0xDA)));
	fl_yxline(x, y+h-2, y, x+w-2);
	// bottom and right borders
	fl_color(activated_color(fl_rgb_color(0xF0, 0xF6, 0xFB)));
	fl_xyline(x, y+h-1, x+w-1, y);
}

static void blue_spacer_thin_down_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+1, w-2, h-2);
	blue_spacer_thin_down_frame(x, y, w, h, c);
}

static void blue_progress_round_up_frame(int x, int y, int w, int h, Fl_Color c) {
	// top outer borders
	fl_color(activated_color(fl_rgb_color(0xB2, 0xB2, 0xB2)));
	fl_xyline(x+2, y, x+w-3);
	// side outer borders
	fl_color(activated_color(fl_rgb_color(0x9F, 0x9F, 0x9F)));
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// bottom outer border
	fl_color(activated_color(fl_rgb_color(0x8C, 0x8C, 0x8C)));
	fl_xyline(x+2, y+h-1, x+w-3);
	// side inner top borders
	fl_color(activated_color(devalued(desaturated(c, 0.27f), 0.07f)));
	fl_yxline(x+1, y+2, y+h/2-1);
	fl_yxline(x+w-2, y+2, y+h/2-1);
	// side inner bottom borders
	fl_color(activated_color(devalued(c, 0.19f)));
	fl_yxline(x+1, y+h/2, y+h-3);
	fl_yxline(x+w-2, y+h/2, y+h-3);
	// top corners
	fl_color(activated_color(fl_rgb_color(0xC9, 0xC9, 0xC9)));
	fl_xyline(x, y+1, x+1, y);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	// bottom corners
	fl_color(activated_color(fl_rgb_color(0xA1, 0xA1, 0xA1)));
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
}

static void blue_progress_round_up_box(int x, int y, int w, int h, Fl_Color c) {
	// top fill
	fl_color(activated_color(devalued(desaturated(c, 0.52f), 0.06f)));
	fl_rectf(x+2, y+1, w-4, h/2-1);
	// top gradient
	vertical_gradient(x+2, y+1, x+w-3, y+6, activated_color(desaturated(c, 0.77f)),
		activated_color(devalued(desaturated(c, 0.52f), 0.06f)));
	// bottom fill
	fl_color(activated_color(devalued(c, 0.16f)));
	fl_rectf(x+2, y+h/2, w-4, h/2-1);
	// bottom gradient
	vertical_gradient(x+2, y+h-7, x+w-3, y+h-2, activated_color(devalued(c, 0.16f)),
		activated_color(devalued(c, 0.07f)));
	blue_progress_round_up_frame(x, y, w, h, c);
}

static void blue_hovered_up_frame(int x, int y, int w, int h, Fl_Color) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0xFF, 0xDB, 0x00)));
	fl_xyline(x+2, y, x+w-3);
	fl_xyline(x+2, y+h-1, x+w-3);
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// top inner borders
	fl_color(activated_color(fl_rgb_color(0xFF, 0xFC, 0xF8)));
	fl_xyline(x+2, y+1, x+w-3);
	fl_yxline(x+1, y+2, y+h/2-1);
	fl_yxline(x+w-2, y+2, y+h/2-1);
	fl_point(x+2, y+2);
	fl_point(x+w-3, y+2);
	// bottom inner borders
	fl_color(activated_color(fl_rgb_color(0xFF, 0xFA, 0xE2)));
	fl_yxline(x+1, y+h/2, y+h-3);
	fl_yxline(x+w-2, y+h/2, y+h-3);
	fl_xyline(x+2, y+h-2, x+w-3);
	fl_point(x+2, y+h-3);
	fl_point(x+w-3, y+h-3);
	// corners
	fl_color(activated_color(fl_rgb_color(0xF7, 0xD7, 0x3F)));
	fl_xyline(x, y+1, x+1, y);
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
}

static void blue_hovered_up_box(int x, int y, int w, int h, Fl_Color c) {
	// top gradient
	vertical_gradient(x+2, y+2, x+w-3, y+h/2-1, activated_color(fl_rgb_color(0xFF, 0xF0, 0xDF)),
		activated_color(fl_rgb_color(0xFF, 0xE2, 0xC2)));
	// bottom gradient
	vertical_gradient(x+2, y+h/2, x+w-3, y+h-3, activated_color(fl_rgb_color(0xFF, 0xCF, 0x6A)),
		activated_color(fl_rgb_color(0xFF, 0xE9, 0x83)));
	blue_hovered_up_frame(x, y, w, h, c);
}

static void blue_depressed_down_frame(int x, int y, int w, int h, Fl_Color) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0xC2, 0x9B, 0x29)));
	fl_xyline(x+2, y, x+w-3);
	fl_xyline(x+2, y+h-1, x+w-3);
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// top inner border
	fl_color(activated_color(fl_rgb_color(0xE3, 0xC1, 0x85)));
	fl_xyline(x+2, y+1, x+w-3);
	// corners
	fl_color(activated_color(fl_rgb_color(0xCB, 0xAB, 0x53)));
	fl_xyline(x, y+1, x+1, y);
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
}

static void blue_depressed_down_box(int x, int y, int w, int h, Fl_Color c) {
	// top gradient
	vertical_gradient(x+1, y+2, x+w-2, y+h/2-1, activated_color(fl_rgb_color(0xEE, 0xCB, 0x8E)),
		activated_color(fl_rgb_color(0xF5, 0xC7, 0x79)));
	// bottom gradient
	vertical_gradient(x+1, y+h/2, x+w-2, y+h-1, activated_color(fl_rgb_color(0xF5, 0xBB, 0x57)),
		activated_color(fl_rgb_color(0xF3, 0xE1, 0x77)));
	blue_depressed_down_frame(x, y, w, h, c);
}

static void blue_group_thin_up_frame(int x, int y, int w, int h, Fl_Color) {
	// border
	fl_color(activated_color(fl_rgb_color(0x87, 0x97, 0xAA)));
	fl_xyline(x+2, y, x+w-3);
	fl_xyline(x+2, y+h-1, x+w-3);
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// top corners
	fl_color(activated_color(fl_rgb_color(0x9B, 0xAA, 0xBB)));
	fl_xyline(x, y+1, x+1, y);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	// bottom corners
	fl_color(activated_color(fl_rgb_color(0xA1, 0xAE, 0xBD)));
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
}

static void blue_group_thin_up_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+1, w-2, h-2);
	blue_group_thin_up_frame(x, y, w, h, c);
}

static void blue_default_button_frame(int x, int y, int w, int h, Fl_Color) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0x87, 0x97, 0xAA)));
	fl_xyline(x+2, y, x+w-3);
	fl_xyline(x+2, y+h-1, x+w-3);
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// top inner borders
	fl_color(activated_color(fl_rgb_color(0xF7, 0xFB, 0xFF)));
	fl_xyline(x+2, y+1, x+w-3);
	fl_yxline(x+1, y+2, y+h/2-1);
	fl_yxline(x+w-2, y+2, y+h/2-1);
	fl_point(x+2, y+2);
	fl_point(x+w-3, y+2);
	// bottom inner borders
	fl_color(activated_color(fl_rgb_color(0xFB, 0xFE, 0xFF)));
	fl_yxline(x+1, y+h/2, y+h-3);
	fl_yxline(x+w-2, y+h/2, y+h-3);
	fl_xyline(x+2, y+h-2, x+w-3);
	fl_point(x+2, y+h-3);
	fl_point(x+w-3, y+h-3);
	// top corners
	fl_color(activated_color(fl_rgb_color(0x9B, 0xAA, 0xBB)));
	fl_xyline(x, y+1, x+1, y);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	// bottom corners
	fl_color(activated_color(fl_rgb_color(0xA1, 0xAE, 0xBD)));
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
}

static void blue_default_button_box(int x, int y, int w, int h, Fl_Color c) {
	// top gradient
	vertical_gradient(x+2, y+2, x+w-3, y+h/2-1, activated_color(fl_rgb_color(0xF7, 0xFB, 0xFF)),
		activated_color(fl_rgb_color(0xED, 0xF3, 0xF8)));
	// bottom gradient
	vertical_gradient(x+2, y+h/2, x+w-3, y+h-3, activated_color(fl_rgb_color(0xE7, 0xED, 0xF5)),
		activated_color(fl_rgb_color(0xEA, 0xF1, 0xF8)));
	blue_default_button_frame(x, y, w, h, c);
}

static void blue_tabs_frame(int x, int y, int w, int h, Fl_Color) {
	// border
	fl_color(activated_color(fl_rgb_color(0x87, 0x97, 0xAA)));
	fl_xyline(x+2, y, x+w-3);
	fl_xyline(x+2, y+h-1, x+w-3);
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// top corners
	fl_color(activated_color(fl_rgb_color(0x9B, 0xAA, 0xBB)));
	fl_xyline(x, y+1, x+1, y);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	// bottom corners
	fl_color(activated_color(fl_rgb_color(0xA1, 0xAE, 0xBD)));
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
}

static void blue_tabs_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+1, w-2, h-2);
	blue_tabs_frame(x, y, w, h, c);
}

static void use_blue_scheme() {
	Fl::scheme("gtk+");
	Fl::set_boxtype(OS_BUTTON_UP_BOX, blue_button_up_box, 2, 2, 4, 4);
	Fl::set_boxtype(OS_CHECK_DOWN_BOX, aero_check_down_box, 2, 2, 4, 4);
	Fl::set_boxtype(OS_BUTTON_UP_FRAME, blue_button_up_frame, 2, 2, 4, 4);
	Fl::set_boxtype(OS_CHECK_DOWN_FRAME, aero_check_down_frame, 2, 2, 4, 4);
	Fl::set_boxtype(OS_PANEL_THIN_UP_BOX, blue_panel_thin_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_SPACER_THIN_DOWN_BOX, blue_spacer_thin_down_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_PANEL_THIN_UP_FRAME, blue_panel_thin_up_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_SPACER_THIN_DOWN_FRAME, blue_spacer_thin_down_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_PROGRESS_ROUND_UP_BOX, blue_progress_round_up_box, 2, 1, 4, 2);
	Fl::set_boxtype(OS_RADIO_ROUND_DOWN_BOX, aero_radio_round_down_box, 3, 3, 6, 6);
	Fl::set_boxtype(OS_HOVERED_UP_BOX, blue_hovered_up_box, 2, 2, 4, 4);
	Fl::set_boxtype(OS_DEPRESSED_DOWN_BOX, blue_depressed_down_box, 2, 2, 4, 4);
	Fl::set_boxtype(OS_HOVERED_UP_FRAME, blue_hovered_up_frame, 2, 2, 4, 4);
	Fl::set_boxtype(OS_DEPRESSED_DOWN_FRAME, blue_depressed_down_frame, 2, 2, 4, 4);
	Fl::set_boxtype(OS_GROUP_THIN_UP_BOX, blue_group_thin_up_box, 2, 2, 4, 4);
	Fl::set_boxtype(OS_INPUT_THIN_DOWN_BOX, aero_input_thin_down_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_GROUP_THIN_UP_FRAME, blue_group_thin_up_frame, 2, 2, 4, 4);
	Fl::set_boxtype(OS_INPUT_THIN_DOWN_FRAME, aero_input_thin_down_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_DEFAULT_BUTTON_BOX, blue_default_button_box, 2, 2, 4, 4);
	Fl::set_boxtype(OS_TABS_BOX, blue_tabs_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_SWATCH_BOX, aero_swatch_box, 2, 2, 4, 4);
	Fl::set_boxtype(FL_UP_BOX, blue_button_up_box, 2, 2, 4, 4);
	Fl::set_boxtype(FL_DOWN_BOX, aero_check_down_box, 2, 2, 4, 4);
	Fl::set_boxtype(FL_ROUND_DOWN_BOX, aero_radio_round_down_box, 3, 3, 6, 6);
}

static void use_blue_colors() {
	Fl::background(0xD9, 0xE4, 0xF1);
	Fl::background2(0xFF, 0xFF, 0xFF);
	Fl::foreground(0x1E, 0x39, 0x5B);
	Fl::set_color(FL_SELECTION_COLOR, 0x33, 0x33, 0x33);
	Fl::set_color(OS_TAB_COLOR, 0xEA, 0xF1, 0xFA);
	Fl::set_color(OS_PROGRESS_COLOR, 0xFF, 0xDB, 0x00);
	Fl_Tooltip::color(fl_rgb_color(0xFF, 0xFF, 0xFF));
	Fl_Tooltip::textcolor(FL_FOREGROUND_COLOR);
}

void OS::use_blue_theme() {
	use_aero_font();
	use_blue_scheme();
	use_blue_colors();
	use_native_settings();
	global_current_theme = BLUE;
}

/************************* Dark (Adobe Photoshop CS6) *************************/

static void dark_button_up_frame(int x, int y, int w, int h, Fl_Color) {
	// top outer border
	fl_color(activated_color(fl_rgb_color(0x30, 0x30, 0x30)));
	fl_xyline(x+2, y, x+w-3);
	// side outer borders
	fl_color(activated_color(fl_rgb_color(0x2C, 0x2C, 0x2C)));
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// bottom outer border
	fl_color(activated_color(fl_rgb_color(0x27, 0x27, 0x27)));
	fl_xyline(x+2, y+h-1, x+w-3);
	// top inner border
	fl_color(activated_color(fl_rgb_color(0x91, 0x91, 0x91)));
	fl_xyline(x+2, y+1, x+w-3);
	// corners
	fl_color(activated_color(fl_rgb_color(0x47, 0x47, 0x47)));
	fl_xyline(x, y+1, x+1, y);
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
}

static void dark_button_up_box(int x, int y, int w, int h, Fl_Color c) {
	vertical_gradient(x+1, y+2, x+w-2, y+h-1, activated_color(fl_rgb_color(0x75, 0x75, 0x75)),
		activated_color(fl_rgb_color(0x62, 0x62, 0x62)));
	dark_button_up_frame(x, y, w, h, c);
}

static void dark_panel_thin_up_frame(int x, int y, int w, int h, Fl_Color) {
	// top and left borders
	fl_color(activated_color(fl_rgb_color(0x6A, 0x6A, 0x6A)));
	fl_yxline(x, y+h-2, y, x+w-2);
	// bottom and right borders
	fl_color(activated_color(fl_rgb_color(0x28, 0x28, 0x28)));
	fl_xyline(x, y+h-1, x+w-1, y);
}

static void dark_panel_thin_up_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+1, w-2, h-2);
	dark_panel_thin_up_frame(x, y, w, h, c);
}

static void dark_spacer_thin_down_frame(int x, int y, int w, int h, Fl_Color) {
	// top and left borders
	fl_color(activated_color(fl_rgb_color(0x38, 0x38, 0x38)));
	fl_yxline(x, y+h-2, y, x+w-2);
	// bottom and right borders
	fl_color(activated_color(fl_rgb_color(0x74, 0x74, 0x74)));
	fl_xyline(x, y+h-1, x+w-1, y);
}

static void dark_spacer_thin_down_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+1, w-2, h-2);
	dark_spacer_thin_down_frame(x, y, w, h, c);
}

static void dark_progress_round_up_frame(int x, int y, int w, int h, Fl_Color c) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0x1A, 0x1A, 0x1A)));
	fl_xyline(x+1, y, x+w-2);
	fl_xyline(x+1, y+h-1, x+w-2);
	fl_yxline(x, y+1, y+h-2);
	fl_yxline(x+w-1, y+1, y+h-2);
	// top and side inner borders
	fl_color(activated_color(desaturated(c, 0.62f)));
	fl_xyline(x+1, y+1, x+w-2);
	fl_yxline(x+1, y+2, y+h/2-1);
	fl_yxline(x+w-2, y+2, y+h/2-1);
	// bottom and side inner borders
	fl_color(activated_color(desaturated(c, 0.28f)));
	fl_xyline(x+1, y+h-2, x+w-2);
	fl_yxline(x+1, y+h/2, y+h-3);
	fl_yxline(x+w-2, y+h/2, y+h-3);
}

static void dark_progress_round_up_box(int x, int y, int w, int h, Fl_Color c) {
	// top gradient
	vertical_gradient(x+2, y+2, x+w-3, y+h/2-1, activated_color(desaturated(c, 0.48f)),
		activated_color(desaturated(c, 0.35f)));
	// bottom gradient
	vertical_gradient(x+2, y+h/2, x+w-3, y+h-3, activated_color(desaturated(c, 0.29f)),
		activated_color(desaturated(c, 0.16f)));
	dark_progress_round_up_frame(x, y, w, h, c);
}

static void dark_radio_round_down_frame(int x, int y, int w, int h, Fl_Color) {
	fl_color(activated_color(fl_rgb_color(0x30, 0x30, 0x30)));
	fl_arc(x, y, w, h, 0.0, 360.0);
}

static void dark_radio_round_down_box(int x, int y, int w, int h, Fl_Color c) {
	// top edges
	fl_color(activated_color(fl_rgb_color(0x75, 0x75, 0x75)));
	fl_arc(x+1, y+1, w-2, h-2, 0.0, 180.0);
	// bottom edges
	fl_color(activated_color(fl_rgb_color(0x62, 0x62, 0x62)));
	fl_arc(x+1, y+1, w-2, h-2, 180.0, 360.0);
	// gradient
	vertical_gradient(x+2, y+2, x+w-3, y+h-3, fl_rgb_color(0x74, 0x74, 0x74), fl_rgb_color(0x63, 0x63, 0x63));
	dark_radio_round_down_frame(x, y, w, h, c);
}

static void dark_depressed_down_frame(int x, int y, int w, int h, Fl_Color) {
	// top outer border
	fl_color(activated_color(fl_rgb_color(0x27, 0x27, 0x27)));
	fl_xyline(x+2, y, x+w-3);
	// side outer borders
	fl_color(activated_color(fl_rgb_color(0x2C, 0x2C, 0x2C)));
	fl_yxline(x, y+2, y+h-3);
	fl_yxline(x+w-1, y+2, y+h-3);
	// bottom outer border
	fl_color(activated_color(fl_rgb_color(0x30, 0x30, 0x30)));
	fl_xyline(x+2, y+h-1, x+w-3);
	// top inner border
	fl_color(activated_color(fl_rgb_color(0x33, 0x33, 0x33)));
	fl_xyline(x+2, y+1, x+w-3);
	// corners
	fl_color(activated_color(fl_rgb_color(0x32, 0x32, 0x32)));
	fl_xyline(x, y+1, x+1, y);
	fl_xyline(x, y+h-2, x+1, y+h-1);
	fl_yxline(x+w-2, y, y+1, x+w-1);
	fl_yxline(x+w-2, y+h-1, y+h-2, x+w-1);
	fl_color(activated_color(fl_rgb_color(0x4B, 0x4B, 0x4B)));
	fl_point(x, y);
	fl_point(x+w-1, y);
	fl_point(x, y+h-1);
	fl_point(x+w-1, y+h-1);
}

static void dark_depressed_down_box(int x, int y, int w, int h, Fl_Color c) {
	vertical_gradient(x+1, y+2, x+w-2, y+h-1, activated_color(fl_rgb_color(0x3F, 0x3F, 0x3F)),
		activated_color(fl_rgb_color(0x37, 0x37, 0x37)));
	dark_depressed_down_frame(x, y, w, h, c);
}

static void dark_group_thin_up_frame(int x, int y, int w, int h, Fl_Color) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0x6A, 0x6A, 0x6A)));
	fl_rect(x, y, w, h);
	// inner border
	fl_color(activated_color(fl_rgb_color(0x28, 0x28, 0x28)));
	fl_rect(x+1, y+1, w-2, h-2);
}

static void dark_group_thin_up_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+2, y+2, w-4, h-4);
	dark_group_thin_up_frame(x, y, w, h, c);
}

static void dark_input_thin_down_frame(int x, int y, int w, int h, Fl_Color) {
	// top and side outer borders
	fl_color(activated_color(fl_rgb_color(0x30, 0x30, 0x30)));
	fl_xyline(x, y, x+w-1);
	fl_yxline(x, y+1, y+h-2);
	fl_yxline(x+w-1, y+1, y+h-2);
	// bottom outer border
	fl_color(activated_color(fl_rgb_color(0x29, 0x29, 0x29)));
	fl_xyline(x, y+h-1, x+w-1);
	// top inner border
	fl_color(activated_color(fl_rgb_color(0x37, 0x37, 0x37)));
	fl_xyline(x+1, y+1, x+w-2);
	// top and side innermost borders
	fl_color(activated_color(fl_rgb_color(0x39, 0x39, 0x39)));
	fl_xyline(x+1, y+2, x+w-2);
	fl_yxline(x+1, y+3, y+h-2);
	fl_yxline(x+w-2, y+3, y+h-2);
}

static void dark_input_thin_down_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+2, y+3, w-4, h-4);
	dark_input_thin_down_frame(x, y, w, h, c);
}

static void dark_tabs_frame(int x, int y, int w, int h, Fl_Color) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0x28, 0x28, 0x28)));
	fl_xyline(x+1, y, x+w-2);
	fl_xyline(x+1, y+h-1, x+w-2);
	fl_yxline(x, y+1, y+h-2);
	fl_yxline(x+w-1, y+1, y+h-2);
	// top inner border
	fl_color(activated_color(fl_rgb_color(0x6A, 0x6A, 0x6A)));
	fl_xyline(x+2, y+1, x+w-3);
}

static void dark_tabs_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+1, y+1, w-2, h-2);
	dark_tabs_frame(x, y, w, h, c);
}

static void dark_swatch_frame(int x, int y, int w, int h, Fl_Color) {
	// outer border
	fl_color(activated_color(fl_rgb_color(0x25, 0x25, 0x25)));
	fl_rect(x, y, w, h);
	// inner border
	fl_color(activated_color(fl_rgb_color(0xFF, 0xFF, 0xFF)));
	fl_rect(x+1, y+1, w-2, h-2);
}

static void dark_swatch_box(int x, int y, int w, int h, Fl_Color c) {
	fl_color(activated_color(c));
	fl_rectf(x+2, y+2, w-4, h-4);
	dark_swatch_frame(x, y, w, h, c);
}

void use_dark_font() {
	OS::use_native_font();
}

static void use_dark_scheme() {
	Fl::scheme("gtk+");
	Fl::set_boxtype(OS_BUTTON_UP_BOX, dark_button_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_CHECK_DOWN_BOX, dark_button_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_BUTTON_UP_FRAME, dark_button_up_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_CHECK_DOWN_FRAME, dark_button_up_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_PANEL_THIN_UP_BOX, dark_panel_thin_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_SPACER_THIN_DOWN_BOX, dark_spacer_thin_down_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_PANEL_THIN_UP_FRAME, dark_panel_thin_up_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_SPACER_THIN_DOWN_FRAME, dark_spacer_thin_down_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_PROGRESS_ROUND_UP_BOX, dark_progress_round_up_box, 2, 1, 4, 2);
	Fl::set_boxtype(OS_RADIO_ROUND_DOWN_BOX, dark_radio_round_down_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_HOVERED_UP_BOX, dark_button_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_DEPRESSED_DOWN_BOX, dark_depressed_down_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_HOVERED_UP_FRAME, dark_button_up_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_DEPRESSED_DOWN_FRAME, dark_depressed_down_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_GROUP_THIN_UP_BOX, dark_group_thin_up_box, 2, 2, 4, 4);
	Fl::set_boxtype(OS_INPUT_THIN_DOWN_BOX, dark_input_thin_down_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_GROUP_THIN_UP_FRAME, dark_group_thin_up_frame, 2, 2, 4, 4);
	Fl::set_boxtype(OS_INPUT_THIN_DOWN_FRAME, dark_input_thin_down_frame, 1, 1, 2, 2);
	Fl::set_boxtype(OS_DEFAULT_BUTTON_BOX, dark_button_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(OS_TABS_BOX, dark_tabs_box, 1, 2, 2, 4);
	Fl::set_boxtype(OS_SWATCH_BOX, dark_swatch_box, 2, 2, 4, 4);
	Fl::set_boxtype(FL_UP_BOX, dark_button_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(FL_DOWN_BOX, dark_button_up_box, 1, 1, 2, 2);
	Fl::set_boxtype(FL_ROUND_DOWN_BOX, dark_radio_round_down_box, 3, 3, 6, 6);
}

static void use_dark_colors() {
	Fl::background(0x53, 0x53, 0x53);
	Fl::background2(0x3A, 0x3A, 0x3A);
	Fl::foreground(0xFF, 0xFF, 0xFF);
	Fl::set_color(FL_SELECTION_COLOR, 0xD6, 0xD6, 0xD6);
	Fl::set_color(OS_TAB_COLOR, 0x53, 0x53, 0x53);
	Fl::set_color(OS_PROGRESS_COLOR, 0x44, 0x6E, 0x99);
	Fl_Tooltip::color(fl_rgb_color(0xFF, 0xFF, 0xCC));
	Fl_Tooltip::textcolor(fl_rgb_color(0x00, 0x00, 0x00));
}

void OS::use_dark_theme() {
	use_dark_font();
	use_dark_scheme();
	use_dark_colors();
	use_native_settings();
	global_current_theme = DARK;
}

/********************************** OS Native *********************************/

OS::Theme OS::global_current_theme = is_modern_windows() ? METRO : AERO;

void OS::use_native_font() {
	if (is_modern_windows()) {
		use_metro_font();
	}
	else {
		use_aero_font();
	}
}

void OS::use_native_scheme() {
	if (is_modern_windows()) {
		use_metro_scheme();
	}
	else {
		use_aero_scheme();
	}
}

void OS::use_native_colors() {
	if (is_modern_windows()) {
		use_metro_colors();
	}
	else {
		use_aero_colors();
	}
}

void OS::use_native_settings() {
	Fl::visible_focus(0);
	Fl::scrollbar_size(15);
	Fl_Tooltip::font(OS_FONT);
	Fl_Tooltip::size(OS_FONT_SIZE);
	Fl_Tooltip::delay(0.5f);
}

void OS::use_native_theme() {
	if (is_modern_windows()) {
		use_metro_theme();
	}
	else {
		use_aero_theme();
	}
}
