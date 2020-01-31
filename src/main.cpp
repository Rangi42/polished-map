#include <iostream>

#pragma warning(push, 0)
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Shared_Image.H>
#pragma warning(pop)

#include "version.h"
#include "preferences.h"
#include "themes.h"
#include "main-window.h"

#ifdef _WIN32

#include <shlobj.h>
#include <tchar.h>

#define MAKE_WSTR_HELPER(x) L ## x
#define MAKE_WSTR(x) MAKE_WSTR_HELPER(x)

#endif

#ifdef __APPLE__

#pragma warning(push, 0)
#include <FL/x.H>
#pragma warning(pop)

Main_Window *window = NULL;

void open_dragged_cb(const char *filename) {
	if (window) {
		window->open_map(filename);
	}
}

#endif

static void use_theme(OS::Theme theme) {
	OS::use_native_fonts();
	switch (theme) {
	case OS::Theme::CLASSIC:
		OS::use_classic_theme();
		return;
	case OS::Theme::AERO:
		OS::use_aero_theme();
		return;
	case OS::Theme::METRO:
		OS::use_metro_theme();
		return;
	case OS::Theme::AQUA:
		OS::use_aqua_theme();
		return;
	case OS::Theme::GREYBIRD:
		OS::use_greybird_theme();
		return;
	case OS::Theme::METAL:
		OS::use_metal_theme();
		return;
	case OS::Theme::BLUE:
		OS::use_blue_theme();
		return;
	case OS::Theme::OLIVE:
		OS::use_olive_theme();
		return;
	case OS::Theme::ROSE_GOLD:
		OS::use_rose_gold_theme();
		return;
	case OS::Theme::DARK:
		OS::use_dark_theme();
		return;
	case OS::Theme::HIGH_CONTRAST:
		OS::use_high_contrast_theme();
	}
}

int main(int argc, char **argv) {
	Preferences::initialize(argv[0]);
	std::ios::sync_with_stdio(false);
	fl_register_images(); // required for Linux/X11 to allow pasting tile graphics
#ifdef _WIN32
	SetCurrentProcessExplicitAppUserModelID(MAKE_WSTR(PROGRAM_AUTHOR) L"." MAKE_WSTR(PROGRAM_NAME));
#endif
#ifdef __APPLE__
	setenv("LANG", "en_US.UTF-8", 1);
#endif
	Fl::visual(FL_DOUBLE | FL_RGB);

#ifdef _WIN32
	OS::Theme theme = (OS::Theme)Preferences::get("theme", (int)OS::Theme::BLUE);
#elif defined(__APPLE__)
	OS::Theme theme = (OS::Theme)Preferences::get("theme", (int)OS::Theme::AQUA);
#else
	OS::Theme theme = (OS::Theme)Preferences::get("theme", (int)OS::Theme::GREYBIRD);
#endif
	use_theme(theme);

#ifdef _WIN32
	int x = Preferences::get("x", 48), y = Preferences::get("y", 48 + GetSystemMetrics(SM_CYCAPTION));
#else
	int x = Preferences::get("x", 48), y = Preferences::get("y", 48);
#endif
	int w = Preferences::get("w", 800), h = Preferences::get("h", 600);
#ifdef __APPLE__
	fl_open_callback(open_dragged_cb);
	window = new Main_Window(x, y, w, h);
#else
	Main_Window *window = new Main_Window(x, y, w, h);
#endif
	window->show();

	if (argc > 1) {
		window->open_map(argv[1]);
	}

#ifdef __APPLE__
	int r = Fl::run();
	delete window;
	window = NULL;
	return r;
#else
	return Fl::run();
#endif
}
