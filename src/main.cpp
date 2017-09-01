#include <iostream>

#pragma warning(push, 0)
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#pragma warning(pop)

#include "version.h"
#include "themes.h"
#include "main-window.h"

#include <shlobj.h>
#include <tchar.h>

#define MAKE_WSTR_HELPER(x) L ## x
#define MAKE_WSTR(x) MAKE_WSTR_HELPER(x)

static void use_theme(int &argc, char **&argv) {
	if (argc < 2) {
		OS::use_blue_theme();
		return;
	}
	else if (!strcmp(argv[1], "--native")) {
		OS::use_native_theme();
	}
	else if (!strcmp(argv[1], "--aero")) {
		OS::use_aero_theme();
	}
	else if (!strcmp(argv[1], "--metro")) {
		OS::use_metro_theme();
	}
	else if (!strcmp(argv[1], "--blue")) {
		OS::use_blue_theme();
	}
	else if (!strcmp(argv[1], "--dark")) {
		OS::use_dark_theme();
	}
	else {
		OS::use_native_theme();
		return;
	}
	argv[1] = argv[0];
	argv++;
	argc--;
}

int main(int argc, char **argv) {
	std::ios::sync_with_stdio(false);
	SetCurrentProcessExplicitAppUserModelID(MAKE_WSTR(PROGRAM_NAME));
	Fl::visual(FL_DOUBLE | FL_RGB);
	use_theme(argc, argv);
	Main_Window *window = new Main_Window(48, 48 + GetSystemMetrics(SM_CYCAPTION), 640, 480);
	window->show();
	return Fl::run();
}
