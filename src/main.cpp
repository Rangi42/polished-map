#include <iostream>

#pragma warning(push, 0)
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#pragma warning(pop)

#include "version.h"
#include "config.h"
#include "themes.h"
#include "main-window.h"

#ifdef _WIN32

#include <shlobj.h>
#include <tchar.h>

#define MAKE_WSTR_HELPER(x) L ## x
#define MAKE_WSTR(x) MAKE_WSTR_HELPER(x)

#endif

static void use_theme(OS::Theme theme) {
	OS::use_native_font();
	switch (theme) {
	case OS::Theme::AERO:
		OS::use_aero_theme();
		return;
	case OS::Theme::METRO:
		OS::use_metro_theme();
		return;
	case OS::Theme::GREYBIRD:
		OS::use_greybird_theme();
		return;
	case OS::Theme::BLUE:
		OS::use_blue_theme();
		return;
	case OS::Theme::DARK:
		OS::use_dark_theme();
	}
}

int main(int argc, char **argv) {
	std::ios::sync_with_stdio(false);
#ifdef _WIN32
	SetCurrentProcessExplicitAppUserModelID(MAKE_WSTR(PROGRAM_NAME));
#endif
	Fl::visual(FL_DOUBLE | FL_RGB);

	int theme;
	global_config.get("theme", theme, (int)OS::Theme::BLUE);
	use_theme((OS::Theme)theme);

	int x, y, w, h;
	global_config.get("x", x, 48);
	global_config.get("y", y, 48 + GetSystemMetrics(SM_CYCAPTION));
	global_config.get("w", w, 640);
	global_config.get("h", h, 480);
	Main_Window window(x, y, w, h);
	window.show();

	if (argc > 1) {
		window.open_map(argv[1]);
	}

	return Fl::run();
}
