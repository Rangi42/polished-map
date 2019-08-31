#ifndef LIGHTING_WINDOW_H
#define LIGHTING_WINDOW_H

#pragma warning(push, 0)
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#pragma warning(pop)

#include "tileset.h"
#include "map-buttons.h"
#include "metatile.h"
#include "widgets.h"

class Swatch_Window : public Fl_Double_Window {
public:
	Swatch_Window(int x, int y, int w, int h, const char *l = NULL);
	int handle(int event);
};

class Abstract_Lighting_Window {
protected:
	int _dx, _dy;
	Lighting _current_lighting;
	bool _canceled;
	Swatch_Window *_window;
	Color_Button *_selected, *_chosen;
	Default_Spinner *_red_spinner, *_green_spinner, *_blue_spinner;
	Default_Button *_ok_button;
	OS_Button *_cancel_button;
	friend class Swatch_Window;
	bool _copied;
	uchar _clipboard[NUM_CHANNELS];
	bool _debounce;
public:
	Abstract_Lighting_Window(int x, int y);
	virtual ~Abstract_Lighting_Window();
protected:
	void initialize(void);
	virtual void initial_setup(void) = 0;
	virtual void refresh(void) = 0;
public:
	inline void current_lighting(Lighting l) { _current_lighting = l; }
	inline bool canceled(void) const { return _canceled; }
	inline void canceled(bool c) { _canceled = c; }
	void show(const Fl_Widget *p);
	virtual void apply_modifications(void) = 0;
	void select(Color_Button *cb);
protected:
	static void close_cb(Fl_Widget *w, Abstract_Lighting_Window *alw);
	static void cancel_cb(Fl_Widget *w, Abstract_Lighting_Window *alw);
	static void select_color_cb(Color_Button *cb, Abstract_Lighting_Window *alw);
	static void change_red_cb(Default_Spinner *sp, Abstract_Lighting_Window *alw);
	static void change_green_cb(Default_Spinner *sp, Abstract_Lighting_Window *alw);
	static void change_blue_cb(Default_Spinner *sp, Abstract_Lighting_Window *alw);
	static void copy_color_cb(Fl_Widget *w, Abstract_Lighting_Window *alw);
	static void paste_color_cb(Fl_Widget *w, Abstract_Lighting_Window *alw);
	static void swap_colors_cb(Fl_Widget *w, Abstract_Lighting_Window *alw);
};

class Lighting_Window : public Abstract_Lighting_Window {
private:
	Fl_Group *_palette_heading_group, *_color_group;
	Label *_palette_headings[NUM_PALETTES];
	Color_Button *_color_buttons[NUM_PALETTES][NUM_HUES];
public:
	Lighting_Window(int x, int y);
	~Lighting_Window();
private:
	void initial_setup(void);
	void refresh(void);
public:
	void apply_modifications(void);
};

#endif
