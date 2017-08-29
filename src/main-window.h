#ifndef VIZ_WINDOW_H
#define VIZ_WINDOW_H

#pragma warning(push, 0)
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Group.H>
#pragma warning(pop)

#include "utils.h"
#include "widgets.h"

class Main_Window : public Fl_Double_Window {
private:
	static const double FIRING_SPEED_TIMEOUTS[10];
private:
	DnD_Receiver *_dnd_receiver;
	Fl_Menu_Bar *_menu_bar;
	Toolbar *_toolbar;
	Workspace *_sidebar;
	Workspace *_map_scroll;
	Fl_Group *_map;
	Toolbar *_status_bar;
	Toolbar_Button *_open_tb, *_save_tb;
public:
	Main_Window(int x, int y, int w, int h, const char *l = NULL);
	void show(void);
private:
	static void drag_and_drop_cb(DnD_Receiver *dndr, Main_Window *mw);
	static void open_cb(Fl_Widget *w, Main_Window *mw);
	static void close_cb(Fl_Widget *w, Main_Window *mw);
	static void save_cb(Fl_Widget *w, Main_Window *mw);
	static void exit_cb(Fl_Widget *w, void *v);
};

#endif
